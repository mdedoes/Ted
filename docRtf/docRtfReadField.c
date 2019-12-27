/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*  Read fields, bookmarks.						*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfFindProperty.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docBookmarkField.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFieldStack.h>
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docParaBuilder.h>
#   include	<docFields.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  To ensure proper field/bookmark nesting.. A stack op active		*/
/*  is kept to be flushed at the appropriate transitions. (Field end,	*/
/*  field start, paragraph end.)					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  While reading the field instructions, we have treated the fields	*/
/*  inside the field instructions like ordinary fields.			*/
/*									*/
/*  But in the fields inside the instructions of another field we abuse	*/
/*  epParaNr/epStroff for the number of the instruction component in	*/
/*  the parent and the offset in the component. Fix the administration	*/
/*  here.								*/
/*									*/
/************************************************************************/

static int docFixInstructionFieldPositions(	const DocumentField *	df )
    {
    int		comp= df->dfInstructions.fiComponentCount- 1;
    int		field;

    const InstructionsComponent *	ic;

    ic= df->dfInstructions.fiComponents+ comp;

    for ( field= df->dfInstructionFields.cfChildCount- 1; field >= 0; field-- )
	{
	DocumentField * dfCh= df->dfInstructionFields.cfChildren[field];

	while( ic->icOffset > dfCh->dfTailPosition.epStroff )
	    { comp--; ic--; }

	if  ( comp < 0 )
	    {
	    LLLLDEB(field,dfCh->dfTailPosition.epStroff,comp,ic->icOffset);
	    return -1;
	    }
	dfCh->dfTailPosition.epParaNr= comp;
	dfCh->dfTailPosition.epStroff -= ic->icOffset;

	while( ic->icOffset > dfCh->dfHeadPosition.epStroff )
	    { comp--; ic--; }

	if  ( comp < 0 )
	    {
	    LLLLDEB(field,dfCh->dfHeadPosition.epStroff,comp,ic->icOffset);
	    return -1;
	    }
	dfCh->dfHeadPosition.epParaNr= comp;
	dfCh->dfHeadPosition.epStroff -= ic->icOffset;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Are we inside a SHAPE field?					*/
/*									*/
/************************************************************************/

int docRtfInsideShapeField(	RtfReader *	rr )
    {
    if  ( ! rr->rrTreeStack						||
	  ! rr->rrTreeStack->rtsFieldStack				||
	  ! rr->rrTreeStack->rtsFieldStack->fslField			||
	  rr->rrTreeStack->rtsFieldStack->fslField->dfKind != DOCfkSHAPE )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Consume a field.							*/
/*									*/
/************************************************************************/

int docRtfReadFldrslt(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    int			res;
    int			piece= rr->rrTreeStack->rtsFieldPiece;

    rr->rrTreeStack->rtsFieldPiece= FSpieceFLDRSLT;

    res= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfDocumentGroups,
				docRtfGotText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rr->rrTreeStack->rtsFieldPiece= piece;

    return res;
    }

static int docRtfCommitFldinst(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    int			rval= 0;
    FieldStackLevel *	fsl= rr->rrTreeStack->rtsFieldStack;
    DocumentField *	df;
    int			fieldKind;

    char *		instBytes= (char *)0;
    int			instSize;
    const int		removeSemicolon= 0;
    int			keepSpace= 1;

    if  ( ! fsl )
	{ XDEB(fsl); goto ready;	}

    df= fsl->fslField;

    if  ( docRtfStoreSavedText( &instBytes, &instSize, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fieldKind= docFieldKindFromInstructions( &keepSpace, instBytes, instSize );
    if  ( fieldKind >= 0 )
	{ df->dfKind= fieldKind;	}
    if  ( docSetFieldInst( df, keepSpace, instBytes, instSize ) )
	{ LDEB(instSize); rval= -1; goto ready;	}

    if  ( docFixInstructionFieldPositions( df ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( instBytes )
	{ free( instBytes );		}

    return rval;
    }

int docRtfReadFldinst(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    int			res;
    int			piece= rr->rrTreeStack->rtsFieldPiece;

    rr->rrTreeStack->rtsFieldPiece= FSpieceFLDINST;

    res= docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitFldinst );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rr->rrTreeStack->rtsFieldPiece= piece;

    return res;
    }

static int docRtfReadPushResultField(	DocumentField **	pField,
					RtfReader *		rr,
					int			fieldKind,
					const char *		instBytes,
					int			instSize )
    {
    int				rval= 0;
    RtfTreeStack *		rts= rr->rrTreeStack;
    struct BufferItem *		paraNode;

    RtfReadingState *		rrs= rr->rrState;
    struct DocumentField *	df= (struct DocumentField *)0;

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ XDEB(paraNode); rval= -1; goto ready; }

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    df= docParaBuilderAppendFieldHead( rts->rtsParagraphBuilder,
					    &(rrs->rrsTextAttribute),
					    fieldKind, instBytes, instSize );

    if  ( ! df )
	{ XDEB(df); rval= -1; goto ready;	}

    if  ( docFieldStackPushLevel( &(rts->rtsFieldStack), df, FSpieceFLDRSLT ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfResetParagraphReadingState( rr );

    *pField= df; df= (struct DocumentField *)0; /* steal */

  ready:

    if  ( df )
	{ docDeleteFieldFromDocument( rr->rrDocument, df );	}

    return rval;
    }

static int docRtfReadPushInstructionsField(
					DocumentField **	pField,
					RtfReader *		rr )
    {
    int				rval= 0;
    RtfTreeStack *		rts= rr->rrTreeStack;
    RtfReadingState *		rrs= rr->rrState;
    struct BufferDocument *	bd= rr->rrDocument;

    DocumentField *	df;

    df= docClaimField( bd );
    if  ( ! df )
	{ XDEB(df); rval= -1; goto ready;	}
    df->dfKind= DOCfkUNKNOWN;
    df->dfSelectionScope= rr->rrTreeStack->rtsSelectionScope; /* needed to pop it */
    df->dfHeadPosition.epParaNr= 0; /* fix later on */
    df->dfHeadPosition.epStroff= rrs->rrsSavedText.mbSize; /* fix later on */

    if  ( docRtfSaveDocEncodedText( rr, "{", 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFieldStackPushLevel( &(rts->rtsFieldStack),
							df, FSpieceFLDINST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfResetParagraphReadingState( rr );

    *pField= df;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Terminate the top field on the field stack and pop it from the	*/
/*  stack.								*/
/*									*/
/************************************************************************/

static int docRtfTerminateTopResultField(	RtfReader *		rr,
						DocumentField *		df )
    {
    struct BufferItem *		node= rr->rrTreeStack->rtsNode;
    RtfReadingState *		rrs= rr->rrState;
    RtfTreeStack *		rts= rr->rrTreeStack;

    if  ( ! node )
	{ XDEB(node); return -1;	}
    if  ( node->biLevel != DOClevPARA )
	{
	DocumentPosition	dp;

	if  ( docTailPosition( &dp, node ) )
	    { XDEB(node); return -1;	}

	if  ( docParaBuilderStartExistingParagraph(
			rts->rtsParagraphBuilder, dp.dpNode, dp.dpStroff ) )
	    { LDEB(1); return -1;	}
	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    if  ( docParaBuilderAppendFieldTail( rts->rtsParagraphBuilder,
					    &(rrs->rrsTextAttribute), df ) )
	{ LDEB(1); return -1;	}

    rr->rrTreeStack->rtsLastFieldNumber= df->dfFieldNumber;

    docRtfResetParagraphReadingState( rr );

    return 0;
    }

static int docRtfTerminateTopInstructionField(	RtfReader *		rr,
						DocumentField *		df )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( docRtfSaveDocEncodedText( rr, "}", 1 ) )
	{ LDEB(1); return -1;	}

    df->dfTailPosition.epParaNr= 0; /* fix later on */
    df->dfTailPosition.epStroff= rrs->rrsSavedText.mbSize; /* fix later on */

    return 0;
    }

static int docRtfTerminateTopField(	RtfReader *		rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    if  ( ! rts->rtsFieldStack )
	{ XDEB(rts->rtsFieldStack); return -1;	}

    switch( rts->rtsFieldStack->fslPiece )
	{
	case FSpieceFLDINST:
	    if  ( docRtfTerminateTopInstructionField( rr,
					rts->rtsFieldStack->fslField ) )
		{ LDEB(1); return -1;	}
	    break;

	case FSpieceFLDRSLT:
	    if  ( docRtfTerminateTopResultField( rr,
					rts->rtsFieldStack->fslField ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(rts->rtsFieldStack->fslPiece);
	    return -1;
	}

    if  ( docFieldStackPopLevel( &(rts->rtsFieldStack),
					    rr->rrTreeStack->rtsTree ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Pop a particular field from the field stack.			*/
/*  Improper field/bookmark nesting may cause enclosed fields to be	*/
/*  closed and hence that the field that we try to close/pop has	*/
/*  already  been closed/popped.					*/
/*									*/
/************************************************************************/

static int docRtfPopFieldFromFieldStack(	DocumentField *		df,
						RtfReader *		rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    {
    FieldStackLevel *	fsl;

    fsl= rts->rtsFieldStack;
    while( fsl )
	{
	if  ( fsl->fslField == df )
	    { break;	}

	fsl= fsl->fslPrev;
	}

    if  ( ! fsl )
	{ return 0;	}
    }

    while( rts->rtsFieldStack )
	{
	int	found= ( rts->rtsFieldStack->fslField == df );

	if  ( docRtfTerminateTopField( rr ) )
	    { LDEB(1); return -1;	}

	if  ( found )
	    { break;	}
	}

    return 0;
    }

int docRtfPopTreeFromFieldStack(	RtfReader *	rr,
					RtfTreeStack *	rts )
    {
    while( rts->rtsFieldStack )
	{
	if  ( docRtfTerminateTopField( rr ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int docRtfPopParaFromFieldStack(	RtfReader *	rr,
					int		paraNr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    while( rts->rtsFieldStack )
	{
	const FieldKindInformation *	fki;
	DocumentField *			df= rts->rtsFieldStack->fslField;

	if  ( df->dfHeadPosition.epParaNr != paraNr )
	    { break;	}

	if  ( df->dfKind >= DOC_FieldKindCount	)
	    { break;	}
	fki= DOC_FieldKinds+ df->dfKind;
	if  ( fki->fkiLevel != DOClevSPAN )
	    { break;	}

	if  ( docRtfTerminateTopField( rr ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int docRtfReadField(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    int			res= -1;
    DocumentField *	df= (DocumentField *)0;

    int			savedPiece= rr->rrTreeStack->rtsFieldPiece;

    rr->rrTreeStack->rtsFieldPiece= FSpieceFIELD;

    switch( savedPiece )
	{
	case FSpieceFLDINST:
	    if  ( docRtfReadPushInstructionsField( &df, rr ) )
		{ SDEB(rcw->rcwWord); goto ready;	}
	    break;

	default:
	    LDEB(savedPiece);
	    /*FALLTHROUGH*/
	case FSpieceFLDRSLT:
	    /*  docRtfReadPushResultField() adjusts level */
	    if  ( docRtfReadPushResultField( &df, rr,
					    DOCfkUNKNOWN, (char *)0, 0 ) )
		{ SDEB(rcw->rcwWord); goto ready;	}
	    break;
	}


    res= docRtfReadGroup( rcw, 0, 0, rr,
				(RtfControlWord *)0,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ LDEB(df->dfFieldNumber); goto ready;	}

  ready:

    rr->rrTreeStack->rtsFieldPiece= savedPiece;

    return res;
    }

/************************************************************************/
/*									*/
/*  Trick: Remember rtf for lookup words in field data.			*/
/*									*/
/************************************************************************/

int docRtfLookupWord(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    FieldStackLevel *	fsl= rr->rrTreeStack->rtsFieldStack;
    char		scratch[40];

    DocumentField *	df= (DocumentField *)0;

    const char *	instBytes= (const char *)0;
    int			instSize= 0;

    switch( rcw->rcwID )
	{
	case RTFlookupTCF:
	case RTFlookupTCL:
	    while( fsl					&&
		   fsl->fslField->dfKind != DOCfkTC	&&
		   fsl->fslField->dfKind != DOCfkTCN	)
		{ fsl= fsl->fslPrev;	}
	    if  ( ! fsl )
		{ SXDEB(rcw->rcwWord,fsl); return 0;	}
	    break;

	case RTFlookupXEF:
	case RTFlookupBXE:
	case RTFlookupIXE:
	    while( fsl					&&
		   fsl->fslField->dfKind != DOCfkXE	)
		{ fsl= fsl->fslPrev;	}
	    if  ( ! fsl )
		{ SXDEB(rcw->rcwWord,fsl); return 0;	}
	    break;

	case RTFlookupTC:
	    /*  docRtfReadPushResultField() adjusts level */
	    if  ( docRtfReadPushResultField( &df, rr,
					DOCfkTC, instBytes, instSize ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTFlookupTCN:
	    /*  docRtfReadPushResultField() adjusts level */
	    if  ( docRtfReadPushResultField( &df, rr,
					DOCfkTCN, instBytes, instSize ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return 0;
	}

    switch( rcw->rcwID )
	{
	case RTFlookupXEF:
	case RTFlookupTCF:
	case RTFlookupTCL:
	    sprintf( scratch, "\\%s%d", rcw->rcwWord, arg );
	    if  ( docAddToFieldData( fsl->fslField,
						scratch, strlen( scratch ) ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTFlookupBXE:
	case RTFlookupIXE:
	    sprintf( scratch, "\\%s", rcw->rcwWord );
	    if  ( docAddToFieldData( fsl->fslField,
						scratch, strlen( scratch ) ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return 0;
	}
    }

int docRtfLookupEntry(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;

    int			fieldKind;

    DocumentField *	df= (DocumentField *)0;

    const char *	instBytes= (const char *)0;
    int			size= 0;

    switch( rcw->rcwID )
	{
	case DOCfkXE:
	    fieldKind= DOCfkXE;
	    break;

	case DOCfkTC:
	    fieldKind= DOCfkTC;
	    break;

	case DOCfkTCN:
	    fieldKind= DOCfkTCN;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    /*  docRtfReadPushResultField() adjusts level */
    if  ( docRtfReadPushResultField( &df, rr, fieldKind, instBytes, size ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rr,
				(RtfControlWord *)0,
				docRtfGotText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Process bookmarks.							*/
/*									*/
/************************************************************************/

static int docRtfCommitBookmarkName(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    int			rval= 0;

    if  ( docRtfMemoryBufferAppendText( &(rr->rrcBookmark), rr ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( rr->rrcBookmark.mbSize > DOCmaxBOOKMARK )
	{
	LLDEB(rr->rrcBookmark.mbSize,DOCmaxBOOKMARK);
	utilMemoryBufferSetSize( &(rr->rrcBookmark), DOCmaxBOOKMARK );
	}

  ready:

    return rval;
    }

int docRtfBkmkStart(	const RtfControlWord *		rcw,
			int				arg,
			RtfReader *			rr )
    {
    int			res;

    DocumentField *	df= (DocumentField *)0;

    const char *	instBytes= (const char *)0;
    int			size= 0;

    utilMemoryBufferSetSize( &(rr->rrcBookmark), 0 );

    /*  docRtfReadPushResultField() adjusts level */
    if  ( docRtfReadPushResultField( &df, rr,
					DOCfkBOOKMARK, instBytes, size ) )
	{ LDEB(1); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitBookmarkName );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    if  ( docSetBookmarkField( &(df->dfInstructions), &(rr->rrcBookmark) ) )
	{ LDEB(1); return -1;	}
    df->dfKind= DOCfkBOOKMARK;

    /* Do not pop field stack */

    return res;
    }

int docRtfBkmkEnd(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    struct BufferDocument *	bd= rr->rrDocument;

    int			res;
    int			fieldNumber;

    DocumentField *	df;

    /*  do not push the field */

    utilMemoryBufferSetSize( &(rr->rrcBookmark), 0 );

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    res= docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitBookmarkName );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    fieldNumber= docFindBookmarkField( &df, bd, &(rr->rrcBookmark) );
    if  ( fieldNumber < 0 )
	{ /*SDEB((char *)rr->rrcBookmarkName);*/ return 0;	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ LDEB(fieldNumber);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Translate a special character to the corresponding field type.	*/
/*									*/
/************************************************************************/

DocumentField * docRtfSpecialField(
				int			fieldKind,
				const char *		instBytes,
				int			instSize,
				const char *		fieldRslt,
				RtfReader *		rr )
    {
    DocumentField *	df;

    /*  docRtfReadPushResultField() adjusts level */
    if  ( docRtfReadPushResultField( &df, rr,
					fieldKind, instBytes, instSize ) )
	{ LDEB(1); return (DocumentField *)0;	}

    if  ( fieldRslt )
	{
	/*  fieldRslt is an ascii string from the Ted source code here,	*/
	/*  so its encoding is irrelevant.				*/

	if  ( docRtfGotText( rr, fieldRslt, strlen( fieldRslt ) ) )
	    { LDEB(2); return (DocumentField *)0;	}
	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return df;
    }

/************************************************************************/
/*									*/
/*  Remember the tail offset of the paragraph head field that we just	*/
/*  inserted at the head of the current paragraph.			*/
/*									*/
/************************************************************************/

static int docRtfSetParaHeadFieldTailOffset(	RtfReader *	rr )
    {
    struct BufferItem *	paraNode= docRtfGetParaNode( rr );

    if  ( ! paraNode )
	{ XDEB(paraNode); return -1; }

    rr->rrParaHeadFieldTailOffset= docParaStrlen( paraNode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle an rtf control word that results in a special character in	*/
/*  the RTF spec, but that is implemented as a field in Ted.		*/
/*									*/
/************************************************************************/

int docRtfTextSpecialToField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    const char *		instBytes= (const char *)"?";
    int				instSize= 1;
    const char *		fieldRslt= (const char *)"?";

    int				afterNoteref= 0;

    switch( rcw->rcwID )
	{
	case DOCfkPAGE:
	    instBytes= " PAGE ";
	    instSize= 6;
	    break;
	case DOCfkDATE:
	    instBytes= " DATE \\* MERGEFORMAT ";
	    instSize= 21;
	    break;
	case DOCfkTIME:
	    instBytes= " TIME \\* MERGEFORMAT ";
	    instSize= 21;
	    break;
	case DOCfkSECTION:
	    instBytes= " SECTION ";
	    instSize= 9;
	    break;

	case DOCfkCHFTN:
	    instBytes= " -CHFTN ";
	    instSize= 8;
	    fieldRslt= (const char *)"1";
	    afterNoteref= 1;
	    break;
	case DOCfkCHATN:
	    instBytes= " -CHATN ";
	    instSize= 8;
	    fieldRslt= (const char *)"!";
	    afterNoteref= 1;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( ! docRtfSpecialField( rcw->rcwID,
				    instBytes, instSize, fieldRslt, rr ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    rr->rrAfterNoteref= afterNoteref;
    if  ( afterNoteref )
	{
	if  ( docRtfSetParaHeadFieldTailOffset( rr ) )
	    { SLDEB(rcw->rcwWord,afterNoteref); return -1;	}
	}
    rr->rrAfterInlineShape= 0;
    rr->rrInlineShapeObjectNumber= -1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a group that results in a text field.				*/
/*									*/
/************************************************************************/

int docRtfReadTextField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;

    DocumentField *	df= (DocumentField *)0;

    const char *	instBytes= (const char *)0;
    int			instSize= 0;

#   if 0
    /*  Will be added later on, as the table nesting is given inside */
    /*  the field, we have a problem if we start the paragraph now. */
    if  ( rcw->rcwID == DOCfkLISTTEXT )
	{
	rr->rrAfterParaHeadField= 1;
	return docRtfSkipGroup( rcw, arg, rr );
	}
#   endif

    /*  docRtfReadPushResultField() adjusts level */
    if  ( docRtfReadPushResultField( &df, rr,
					rcw->rcwID, instBytes, instSize ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfDocumentGroups,
				docRtfGotText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ LDEB(1);	}

    if  ( rcw->rcwID == DOCfkLISTTEXT )
	{
	if  ( docRtfSetParaHeadFieldTailOffset( rr ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Handle a field property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberFieldProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    FieldStackLevel *	fsl= rr->rrTreeStack->rtsFieldStack;

    if  ( ! fsl )
	{ SXDEB(rcw->rcwWord,fsl); return 0;	}

    if  ( docSetFieldProperty( fsl->fslField, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a note property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberNoteProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	RtfTreeStack *	rts;

	case NOTEpropTREE_TYPE:
	    rts= rr->rrTreeStack;

	    if  ( rts->rtsSelectionScope.ssTreeType != DOCinFOOTNOTE )
		{ LLDEB(rts->rtsSelectionScope.ssTreeType,DOCinFOOTNOTE); }
	    else{
		rts->rtsSelectionScope.ssTreeType= rcw->rcwEnumValue;
		rr->rrNoteProperties.npTreeType= rcw->rcwEnumValue;
		}
	    break;

	case NOTEpropAUTO_NUMBER:
	    rr->rrNoteProperties.npAutoNumber= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    PROPmaskADD( &(rr->rrNotePropertyMask), rcw->rcwID );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a NeXTGraphic as an IncludePicture field			*/
/*									*/
/************************************************************************/

static int docRtfCommitNeXTGraphicName(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    int			rval= 0;

    RtfReadingState *	rrs= rr->rrState;
    FieldStackLevel *	fsl= rr->rrTreeStack->rtsFieldStack;
    MemoryBuffer	mbName;

    utilInitMemoryBuffer( &mbName );

    if  ( ! fsl )
	{ SXDEB(rcw->rcwWord,fsl); goto ready;	}

    if  ( docRtfMemoryBufferAppendText( &mbName, rr ) )
	{ SXDEB(rcw->rcwWord,fsl); goto ready;	}

    while( mbName.mbSize > 0 && mbName.mbBytes[mbName.mbSize- 1] == ' ' )
	{  mbName.mbSize--;	}

    if  ( docFieldInstructionsAddComponent(
			&(fsl->fslField->dfInstructions), &mbName ) )
	{ SDEB(rcw->rcwWord); goto ready;	}

    if  ( rrs->rrsWidth > 0 )
	{
	if  ( docFieldInstructionsAddIntFlag(
					&(fsl->fslField->dfInstructions),
					'w', rrs->rrsWidth ) )
	    { SDEB(rcw->rcwWord); goto ready;	}
	}

    if  ( rrs->rrsHeight > 0 )
	{
	if  ( docFieldInstructionsAddIntFlag(
					&(fsl->fslField->dfInstructions),
					'h', rrs->rrsHeight ) )
	    { SDEB(rcw->rcwWord); goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &mbName );

    return rval;
    }

int docRtfNeXTDimension(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    switch( rcw->rcwID )
	{
	case PIPpropPICTWIPS_WIDE:
	    rrs->rrsWidth= arg;
	    return 0;

	case PIPpropPICTWIPS_HIGH:
	    rrs->rrsHeight= arg;
	    return 0;

	default:
	    SLDEB(rcw->rcwWord,arg); return -1;
	}
    }

int docRtfReadNeXTGraphic(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;

    DocumentField *	df= (DocumentField *)0;

    const char *	instBytes= rcw->rcwWord;
    int			instSize= strlen( rcw->rcwWord );

    /*  docRtfReadPushResultField() adjusts level */
    if  ( docRtfReadPushResultField( &df, rr,
					rcw->rcwID, instBytes, instSize ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitNeXTGraphicName );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rr ) )
	{ LDEB(1);	}

    {
    int		c= docRtfReadByte( rr );

    if  ( c != 0xac )
	{
	XDEB(c);
	if  ( c > 0 )
	    { docRtfUngetLastRead( rr );	}
	}
    }

    return res;
    }

