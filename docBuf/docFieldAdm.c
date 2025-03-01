/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docField.h"
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	<docBookmarkField.h>
#   include	"docParaString.h"
#   include	"docNodeTree.h"
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docParaProperties.h>
#   include	"docSelect.h"
#   include	"docNotes.h"
#   include	"docFields.h"
#   include	"docParaBuilder.h"
#   include	"docParaBuilderImpl.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/

void docDeleteFieldFromDocument(	struct BufferDocument *	bd,
					DocumentField *		df )
    {
    if  ( docFieldHasNote( df->dfKind ) && df->dfNoteIndex >= 0 )
	{
	docDeleteNote( bd, df->dfNoteIndex );
	}

    docDeleteFieldFromList( &(bd->bdFieldList), df );

    return;
    }

/************************************************************************/
/*									*/
/*  Find the inner most field for a particular position.		*/
/*									*/
/************************************************************************/

DocumentField * docFindFieldForPosition(
				struct BufferDocument *		bd,
				const DocumentPosition *	dp )
    {
    EditPosition		ep;
    struct DocumentTree *	tree;
    struct BufferItem *		bodySectNode;
    const int			lastOne= 0;

    docSetEditPosition( &ep, dp );

    if  ( docGetTreeOfNode( &tree, &bodySectNode, bd, dp->dpNode ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return docFindChildField( &(tree->dtRootFields), &ep, lastOne );
    }

/************************************************************************/
/*									*/
/*  Find a field of a certain type that holds a position.		*/
/*									*/
/************************************************************************/

DocumentField * docFindTypedFieldForPosition(
				struct BufferDocument *		bd,
				const DocumentPosition *	dp,
				int				kind,
				int				lastOne )
    {
    EditPosition		ep;
    struct DocumentTree *	tree;
    DocumentField *		dfInner;

    docSetEditPosition( &ep, dp );

    if  ( docGetTreeOfNode( &tree, (struct BufferItem **)0, bd, dp->dpNode ) )
	{ LDEB(1); return (DocumentField *)0;	}

    dfInner= docFindChildField( &(tree->dtRootFields), &ep, lastOne );

    while( dfInner && docEditPositionInField( dfInner, &ep ) )
	{
	DocumentField *	df= dfInner;

	while( df )
	    {
	    if  ( df->dfKind == kind || kind == -1 )
		{ return df;	}

	    df= df->dfParent;
	    }

	dfInner= docGetNextField( &(tree->dtRootFields), dfInner );
	}

    return (DocumentField *)0;
    }

DocumentField * docFindTypedFieldInSelection(
			    struct BufferDocument *		bd,
			    const struct DocumentSelection *	ds,
			    int					kind,
			    int					lastOne )
    {
    EditRange		er;
    struct DocumentTree *	tree;

    docSetEditRange( &er, ds );

    if  ( docGetRootOfSelectionScope( &tree, (struct BufferItem **)0,
					    bd, &(ds->dsSelectionScope) ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return docFindFieldInRange( &er, &(tree->dtRootFields), lastOne, kind );
    }

/************************************************************************/
/*									*/
/*  Suggest a bookmark name that matches the text in a selection.	*/
/*									*/
/************************************************************************/

int docSuggestNewBookmarkName(	MemoryBuffer *			markName,
				const struct BufferDocument *	bd,
				const DocumentSelection *	ds )
    {
    int			rval= 0;
    const struct BufferItem *	paraNode= ds->dsHead.dpNode;
    int			stroff= ds->dsHead.dpStroff;

    if  ( paraNode )
	{
	const char *	s= docParaString( paraNode, stroff );
	int		stroffTail;

	if  ( paraNode == ds->dsTail.dpNode )
	    { stroffTail= ds->dsTail.dpStroff;	}
	else{ stroffTail= docParaStrlen( paraNode );	}

	if  ( docBookmarkFromText( markName, s, stroffTail- stroff ) )
	    { LDEB(1); return -1;	}

	if  ( docMakeBookmarkUnique( bd, markName ) )
	    { LDEB(1); return -1;	}

	rval= 1;
	}

    return rval;
    }

/************************************************************************/

int docMakeBookmarkUnique(	const struct BufferDocument *	bd,
				MemoryBuffer *			markName )
    {
    DocumentField *		df;

    if  ( docFindBookmarkField( &df, bd, markName ) >= 0 )
	{
	int		i;
	char		scratch[6+2]; /* 2: Make fortify happy */
	int		offset= docBookmarkSuffixIndex( markName, 6 );

	for ( i= 0; i < 1000000; i++ )
	    {
	    sprintf( scratch, "%06d", i );
	    utilMemoryBufferReplaceBytes( markName,
						offset, markName->mbSize,
						(unsigned char *)scratch, 6 );

	    if  ( docFindBookmarkField( &df, bd, markName ) < 0 )
		{ return 0;	}
	    }

	LDEB(i); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a bookmark field by name					*/
/*									*/
/************************************************************************/

int docFindBookmarkField(	struct DocumentField **		pDf,
				const struct BufferDocument *	bd,
				const MemoryBuffer *		markName )
    {
    return docFieldListFindBookmark( pDf, &(bd->bdFieldList), markName );
    }

/************************************************************************/
/*									*/
/*  Derive field kind from the unparsed field instructions.		*/
/*									*/
/************************************************************************/

int docFieldKindFromInstructions(	int *			pKeepSpace,
					const char *		text,
					int			size )
    {
    char *	to;
    int		len;

    char	scratch[30];

    int					kind;
    const FieldKindInformation *	fki;

    while( size > 0 && *text == ' ' )
	{ size--; text++;	}

    /* Cope with OpenOffice that inserts backslashes before the instructions */
    while( size > 0 && *text == '\\' )
	{ size--; text++;	}

    len= 0; to= scratch;
    while( len < size && len < sizeof(scratch)- 1 && ! isspace( *text ) )
	{
	if  ( islower( *text ) )
	    { *to= toupper( *text );	}
	else{ *to= *text;		}

	len++; to++; text++;

	if  ( to[-1] == '=' )
	    { break;	}
	}
    *to= '\0';

    if  ( DOC_FieldKindCount != DOCfk_COUNT )
	{ LLDEB(DOC_FieldKindCount,DOCfk_COUNT); return -1;	}

    fki= DOC_FieldKinds;
    for ( kind= 0; kind < DOC_FieldKindCount; fki++, kind++ )
	{
	if  ( ! fki->fkiIsFieldInRtf )
	    { continue;	}

	if  ( ! strcmp( fki->fkiLabel, scratch ) )
	    {
	    if  ( pKeepSpace )
		{ *pKeepSpace= fki->fkiKeepInstructionsSpace;	}

	    return kind;
	    }
	}

    return -1;
    }

int docShiftFieldReferences(		struct DocumentTree *	tree,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    /*
    appDebug( "docShiftFieldReferences( %s,"
		" para:%d+%d, stroff:%d+%d )\n",
	    docTreeTypeStr( tree->dtRoot?tree->dtRoot->biTreeType:-1 ),
	    paraFrom, paraShift, stroffFrom, stroffShift );
    */

    /*  1  */
    if  ( paraFrom < 1 )
	{ LDEB(paraFrom); return -1;	}

    docShiftChildFieldReferences( &(tree->dtRootFields),
					sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that completely fall inside a range in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

static void docDeleteFieldChildren(	int *			pUpdateFlags,
					struct BufferDocument *	bd,
					DocumentField *		dfPa )
    {
    if  ( dfPa->dfKind == DOCfkLISTTEXT )
	{ (*pUpdateFlags) |= FIELDdoLISTTEXT;	}
    if  ( dfPa->dfKind == DOCfkSEQ )
	{ (*pUpdateFlags) |= FIELDdoSEQ;	}
    if  ( docFieldHasNote( dfPa->dfKind ) && dfPa->dfNoteIndex >= 0 )
	{ (*pUpdateFlags) |= FIELDdoCHFTN;	}

    docDeleteChildFields( pUpdateFlags, bd, &(dfPa->dfResultFields) );
    return;
    }

void docDeleteChildFields(	int *			pUpdateFlags,
				struct BufferDocument *	bd,
				ChildFields *		cf )
    {
    int		i;

    for ( i= 0; i < cf->cfChildCount; i++ )
	{
	DocumentField *	dfCh= cf->cfChildren[i];

	docDeleteFieldChildren( pUpdateFlags, bd, dfCh );
	docDeleteFieldFromDocument( bd, dfCh );
	}

    docCleanChildFields( cf );
    docInitChildFields( cf );
    }

/************************************************************************/
/*									*/
/*  Delete a field from a document and remove all references.		*/
/*									*/
/*  NOTE The shortcut that optimizes many superfluous calls.		*/
/*									*/
/************************************************************************/

int docDeleteFieldFromParent(	struct DocumentTree *		tree,
				DocumentField *			df )
    {
    ChildFields *	cf;

    if  ( df->dfParent )
	{ cf= &(df->dfParent->dfResultFields);	}
    else{ cf= &(tree->dtRootFields);		}

    if  ( df->dfResultFields.cfChildCount == 0	&&
	  cf->cfChildCount == 1			)
	{ cf->cfChildCount--;	}
    else{
	if  ( docDeleteChildField( cf, df ) )
	    {
	    SDEB(docTreeTypeStr(df->dfSelectionScope.ssTreeType));
	    XDEB(df->dfParent);
	    return -1;
	    }
	}

    df->dfNumberInParent= -1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that overlap a range.				*/
/*									*/
/************************************************************************/

int docDeleteFieldRange(	int *			pUpdateFlags,
				struct BufferDocument *	bd,
				const EditRange *	er,
				ChildFields *		rootFields )
    {
    int			rval= 0;
    int			i;

    int			count= rootFields->cfChildCount;
    DocumentField **	fields= (DocumentField **)0;

    if  ( rootFields->cfChildCount == 0 )
	{ goto ready;	}

    fields= (DocumentField **)malloc( count* sizeof(DocumentField *) );
    if  ( ! fields )
	{ LXDEB(count,fields); rval= -1; goto ready;	}

    for ( i= 0; i < count; i++ )
	{ fields[i]= rootFields->cfChildren[i];	}

    /*  1  */
    for ( i= 0; i < count; i++ )
	{
	DocumentField *		df= fields[i];
	int			bcm;
	int			ecm;

	if  ( docCompareEditPositions( &(df->dfTailPosition),
						    &(er->erHead) ) <= 0 )
	    { continue;	}

	if  ( docCompareEditPositions( &(df->dfHeadPosition),
						    &(er->erTail) ) >= 0 )
	    { break;	}

	bcm= docCompareEditPositions( &(df->dfHeadPosition), &(er->erHead) );
	ecm= docCompareEditPositions( &(df->dfTailPosition), &(er->erTail) );

	if  ( bcm < 0 || ecm > 0 )
	    {
	    docDeleteFieldRange( pUpdateFlags, bd, er, &(df->dfResultFields) );
	    }
	else{
	    if  ( df->dfKind == DOCfkLISTTEXT )
		{ (*pUpdateFlags) |= FIELDdoLISTTEXT;	}
	    if  ( df->dfKind == DOCfkSEQ )
		{ (*pUpdateFlags) |= FIELDdoSEQ;	}
	    if  ( docFieldHasNote( df->dfKind ) && df->dfNoteIndex >= 0 )
		{ (*pUpdateFlags) |= FIELDdoCHFTN;	}

	    docDeleteFieldChildren( pUpdateFlags, bd, df );
	    docDeleteChildField( rootFields, df );
	    docDeleteFieldFromDocument( bd, df );
	    }
	}

  ready:

    if  ( fields )
	{ free( fields );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert the special field at the head of a numbered paragraph.	*/
/*  (buller), or of a foot/endnote.					*/
/*									*/
/*  0)  Insert a text particule at the head of the paragraph as a	*/
/*	temporary field value.						*/
/*  1)  Allocate a field.						*/
/*  2)  Insert start particule.						*/
/*  3)  Insert end particule.						*/
/*  4)  Make sure there is at least one particule after the field.	*/
/*									*/
/************************************************************************/

int docInsertParaHeadField(	DocumentField **		pDfHead,
				DocumentSelection *		dsInsideHead,
				DocumentSelection *		dsAroundHead,
				int *				pHeadPart,
				int *				pTailPart,
				struct ParagraphBuilder *	pb,
				int				fieldKind,
				int				textAttrNr )
    {
    BufferItem *		paraNode= pb->pbParaNode;
    BufferDocument *		bd= pb->pbDocument;

    DocumentField *		df;
    TextParticule *		tpText;
    int				stroffShift= 0;
    int				head= 0;
    const int			stroffHead= 0;
    int				wasEmpty= ( docParaStrlen( paraNode ) == 0 );

    DocumentFieldList *		dfl= &(bd->bdFieldList);

    while( head+ 1 < paraNode->biParaParticuleCount			&&
	   paraNode->biParaParticules[head+ 1].tpStroff == stroffHead	&&
	   paraNode->biParaParticules[head+ 1].tpKind == TPkindFIELDHEAD &&
	   docGetFieldKindByNumber( dfl,
	     paraNode->biParaParticules[head+ 1].tpObjectNumber==DOCfkBOOKMARK ) )
	{ head++;	}

    /*  4  */
    if  ( docParaStringReplace( &stroffShift, paraNode,
			stroffHead, stroffHead, "?", 1 ) )
	{ LDEB(docParaStrlen(paraNode)); return -1; }

    if  ( paraNode->biParaParticuleCount > 0 && wasEmpty )
	{
	tpText= paraNode->biParaParticules;
	if  ( tpText->tpKind != TPkindSPAN )
	    { SDEB(docKindStr(tpText->tpKind)); return -1;	}

	tpText->tpStrlen= 1;
	tpText->tpTextAttrNr= textAttrNr;
	}
    else{
	tpText= docParaGraphBuilderInsertSpanParticule( pb, head,
			    stroffHead, 1, textAttrNr );
	if  ( ! tpText )
	    { LXDEB(paraNode->biParaParticuleCount,tpText); return -1; }
	}

    if  ( docParagraphBuilderShiftOffsets( pb,
				    head+ 1, stroffHead, stroffShift ) )
	{ LDEB(stroffShift); }

    /*  2,3  */
    df= docMakeTextLevelField( pb, stroffHead, stroffHead+ stroffShift,
					head, head+ 1,
					textAttrNr, fieldKind );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    /*  4  */
    if  ( paraNode->biParaParticuleCount == head+ 3 )
	{
	tpText= docParaGraphBuilderInsertSpanParticule( pb, head+ 3,
					docParaStrlen( paraNode ), 0,
					textAttrNr );
	if  ( ! tpText )
	    { LXDEB(paraNode->biParaParticuleCount,tpText); return -1; }
	}

    if  ( docDelimitFieldInDoc( dsInsideHead, dsAroundHead,
					    pHeadPart, pTailPart, bd, df ) )
	{ LDEB(1); return -1; }

    *pDfHead= df;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a text level field. I.E. a field in a single paragraph.	*/
/*									*/
/************************************************************************/

DocumentField * docMakeTextLevelField(
				struct ParagraphBuilder *	pb,
				int				headStroff,
				int				tailStroff,
				int				headPart,
				int				tailPart,
				int				textAttrNr,
				int				fieldKind )
    {
    BufferDocument *		bd= pb->pbDocument;

    const struct BufferItem *	sectNode;

    DocumentField *		rval= (DocumentField *)0;
    DocumentField *		df;

    df= docClaimField( bd );
    if  ( ! df )
	{ XDEB(df); goto ready;	}
    df->dfKind= DOCfkUNKNOWN;

    sectNode= docGetSectNode( pb->pbParaNode );
    if  ( ! sectNode )
	{ XXDEB(pb->pbParaNode,sectNode); goto ready;	}

    if  ( docParaBuilderInsertFieldParticules( &headStroff, &tailStroff, pb,
		    headPart, tailPart, textAttrNr, df->dfFieldNumber ) )
	{ LLDEB(headStroff,tailStroff); goto ready;	}

    df->dfSelectionScope= sectNode->biSectSelectionScope;
    df->dfHeadPosition.epParaNr= pb->pbParaNr;
    df->dfHeadPosition.epStroff= headStroff;
    df->dfTailPosition.epParaNr= pb->pbParaNr;
    df->dfTailPosition.epStroff= tailStroff;

    if  ( docInsertFieldInTree( &(pb->pbTree->dtRootFields), df ) )
	{ LDEB(1); goto ready;	}

    df->dfKind= fieldKind;

    rval= df; df= (DocumentField *)0; /* steal */

  ready:

    if  ( df )
	{ docDeleteFieldFromList( &(bd->bdFieldList), df );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the special field at the head of a paragraph in a list.	*/
/*									*/
/************************************************************************/

int docParaHeadFieldKind(	const struct BufferItem *	paraNode )
    {
    int		fieldKind= -1;

    if  ( paraNode->biTreeType == DOCinFOOTNOTE	||
	  paraNode->biTreeType == DOCinENDNOTE	)
	{
	struct BufferItem *	node= docGetSectNode( paraNode->biParent );
	if  ( ! node )
	    { XDEB(node); return -1;		}
	else{
	    DocumentPosition	dp;

	    if  ( docHeadPosition( &dp, node ) )
		{ LDEB(1); return -1;	}

	    if  ( dp.dpNode == paraNode )
		{ fieldKind= DOCfkCHFTN;	}
	    }
	}

    if  ( paraNode->biParaProperties->ppListOverride > 0 )
	{ fieldKind= DOCfkLISTTEXT;	}

    return fieldKind;
    }

/************************************************************************/
/*									*/
/*  Retrieve a field by field number.					*/
/*									*/
/************************************************************************/

struct DocumentField * docGetFieldByNumber(
					const struct BufferDocument *	bd,
					int				n )
    {
    return docFieldListGetByNumber( &(bd->bdFieldList), n );
    }

int docFindBookmarkInDocument(	struct DocumentSelection *	dsInside,
				const struct BufferDocument *	bd,
				const struct MemoryBuffer *	markName )
    {
    int				i;
    struct DocumentField *	df;

    i= docFindBookmarkField( &df, bd, markName );
    if  ( i >= 0 )
	{
	DocumentSelection	dsAround;

	if  ( docDelimitFieldInDoc( dsInside, &dsAround,
					    (int *)0, (int *)0, bd, df ) )
	    { LDEB(i); return -1;	}

	return 0;
	}

    return 1;
    }

struct DocumentField * docClaimField(	struct BufferDocument *	bd )
    {
    return docFieldListClaimField( &(bd->bdFieldList) );
    }

int docForAllFields(	const BufferDocument *		bd,
			DocFieldListForAllFun		fun,
			void *				through )
    {
    return docFieldListForAll( &(bd->bdFieldList), fun, through );
    }
