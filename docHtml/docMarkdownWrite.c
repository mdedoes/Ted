/************************************************************************/
/*									*/
/*  Save a BufferDocument into a markdown text file.			*/
/*  See: http://daringfireball.net/projects/markdown/syntax		*/
/*  See: http://spec.commonmark.org/					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	"docMarkdownReadWrite.h"
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<sioGeneral.h>
#   include	<docSelect.h>
#   include	<docTextRun.h>
#   include	<docParaScanner.h>
#   include	<textAttribute.h>
#   include	<docHyperlinkField.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

typedef struct MarkdownDocumentWriter
    {
    SimpleOutputStream *	mdwSos;
    struct BufferDocument *	mdwDocument;

    int				mdwNoteRefCount;
    int				mdwNoteDefCount;
    } MarkdownDocumentWriter;

static void docInitMarkdownDocumentWriter(
				    MarkdownDocumentWriter *	mdw )
    {
    mdw->mdwSos= (SimpleOutputStream *)0;
    mdw->mdwDocument= (struct BufferDocument *)0;

    mdw->mdwNoteRefCount= 0;
    mdw->mdwNoteDefCount= 0;
    }

typedef struct MarkdownParagraphWriter
    {
    MarkdownDocumentWriter *	mpwMarkdownWriter;

    TextAttribute		mpwCurrentAttribute;

    int				mpwAtHead;
    int				mpwPos;
    int				mpwLineCount;
    unsigned char		mpwClose;
    } MarkdownParagraphWriter;

static int docMarkdownSaveObject(	const VisitParticule *	vp,
					struct InsertedObject *	io,
					void *			vmpw )
    {
    MarkdownParagraphWriter *	mpw= (MarkdownParagraphWriter *)vmpw;
    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    const char *		alt= "ALT";
    const char *		src= "src.jpg";

    if  ( sioOutPutString( "![", sos ) < 0 )
	{ LDEB(1); return -1;	}
    mpw->mpwPos += 2;
    if  ( sioOutPutString( alt, sos ) < 0 )
	{ LDEB(1); return -1;	}
    mpw->mpwPos += strlen( alt );

    if  ( sioOutPutString( "](", sos ) < 0 )
	{ LDEB(1); return -1;	}
    mpw->mpwPos += 2;
    if  ( sioOutPutString( src, sos ) < 0 )
	{ LDEB(1); return -1;	}
    mpw->mpwPos += strlen( src );
    if  ( sioOutPutString( ")", sos ) < 0 )
	{ LDEB(1); return -1;	}
    mpw->mpwPos += 1;

    mpw->mpwAtHead= 0;
    return SCANadviceOK;
    }

static int docMarkdownSaveParticule( const VisitParticule *	vp,
				void *				vppw )
    {
    MarkdownParagraphWriter *	mpw= (MarkdownParagraphWriter *)vppw;
    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    switch( vp->vpTextParticule->tpKind )
	{
	case TPkindTAB:
	    if  ( mpw->mpwAtHead )
		{
		if  ( sioOutPutString( "> ", sos ) < 0 )
		    { LDEB(1); return -1;	}
		mpw->mpwPos += 2;
		}
	    else{
		if  ( sioOutPutByte( ' ', sos ) < 0 )
		    { LDEB(1); return -1;	}
		mpw->mpwPos += 1;
		}
	    break;

	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    if  ( sioOutPutString( "\\\n", sos ) < 0 )
		{ LDEB(1); return -1;	}
	    mpw->mpwPos= 0;
	    break;

	case TPkindFIELDHEAD:
	    LDEB(vp->vpTextParticule->tpKind); return SCANadviceOK;

	default:
	    LDEB(vp->vpTextParticule->tpKind);
	    /*FALLTHROUGH*/
	case TPkindOBJECT:
	case TPkindFIELDTAIL:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    break;
	}

    mpw->mpwAtHead= 0;
    return SCANadviceOK;
    }

static int docMarkdownSaveLinkDestination(
			MarkdownParagraphWriter *	mpw,
			const DocumentField *		df )
    {
    int				rval= 0;

    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    HyperlinkField		hf;

    const unsigned char *	fileBytes;
    int				fileSize;
    const unsigned char *	markBytes;
    int				markSize;

    docInitHyperlinkField( &hf );

    if  ( docGetHyperlinkField( &hf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fileBytes= utilMemoryBufferGetBytes( &fileSize, &(hf.hfFile) );
    markBytes= utilMemoryBufferGetBytes( &markSize, &(hf.hfBookmark) );

    if  ( sioOutPutString( "(<", sos ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    mpw->mpwPos += 2;

    if  ( sioOutWriteBytes( sos, fileBytes, fileSize ) < 0 )
	{ LDEB(fileSize); rval= -1; goto ready;	}

    if  ( markSize > 0 )
	{
	if  ( sioOutPutByte( '#', sos ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	mpw->mpwPos += 1;

	if  ( sioOutWriteBytes( sos, markBytes, markSize ) < 0 )
	    { LDEB(markSize); rval= -1; goto ready;	}
	}

    if  ( sioOutPutString( ">)", sos ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    mpw->mpwPos += 2;

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

static int docMarkdownSaveField( const VisitParticule *	vp,
				DocumentField *		df,
				void *			vppw )
    {
    MarkdownParagraphWriter *	mpw= (MarkdownParagraphWriter *)vppw;
    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    if  ( df->dfKind == DOCfkPAGEREF )
	{ return SCANadviceSKIP;	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	char	scratch[20+1];

	if  ( vp->vpParaNode->biTreeType == DOCinBODY )
	    {
	    sprintf( scratch, "[%d]", mdw->mdwNoteRefCount+ 1 );
	    mdw->mdwNoteRefCount++;
	    }
	else{
	    sprintf( scratch, "[%d]", mdw->mdwNoteDefCount+ 1 );
	    mdw->mdwNoteDefCount++;
	    }

	sioOutPutString( scratch, sos );
	mpw->mpwPos += strlen( scratch );

	return SCANadviceSKIP;
	}

    if  ( df->dfKind == DOCfkHYPERLINK )
	{
	switch( vp->vpTextParticule->tpKind )
	    {
	    case TPkindFIELDHEAD:
		if  ( sioOutPutString( "[", sos ) < 0 )
		    { LDEB(1); return -1;	}
		mpw->mpwPos += 1;
		break;
	    case TPkindFIELDTAIL:
		if  ( sioOutPutString( "]", sos ) < 0 )
		    { LDEB(1); return -1;	}
		mpw->mpwPos += 1;

		if  ( docMarkdownSaveLinkDestination( mpw, df ) )
		    { LDEB(1); return -1;	}

		break;
	    default:
		SDEB(docKindStr(vp->vpTextParticule->tpKind));
		return -1;
	    }

	return SCANadviceOK;
	}

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	}

    return SCANadviceOK;
    }

static int docMarkdownSwitchTextAttributes(
			MarkdownParagraphWriter *	mpw,
			const TextAttribute *		from,
			const TextAttribute *		to )
    {
    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    int				toBold= 0;
    int				toSlanted= 0;

    if  ( to->taFontIsBold		&&
	  ! from->taFontIsBold	)
	{ toBold= +1;	}
    if  ( ! to->taFontIsBold		&&
	  from->taFontIsBold		)
	{ toBold= -1;	}

    if  ( to->taFontIsSlanted		&&
	  ! from->taFontIsSlanted	)
	{ toSlanted= +1;	}
    if  ( ! to->taFontIsSlanted		&&
	  from->taFontIsSlanted		)
	{ toSlanted= -1;	}

    if  ( toBold != 0 && toBold == toSlanted )
	{
	if  ( sioOutPutString( "***", sos ) < 0 )
	    { LDEB(1); return -1;	}
	mpw->mpwPos += 3;
	}

    if  ( toBold != 0 && toSlanted == 0 )
	{
	if  ( sioOutPutString( "**", sos ) < 0 )
	    { LDEB(1); return -1;	}
	mpw->mpwPos += 2;
	}

    if  ( toBold == 0 && toSlanted != 0 )
	{
	if  ( sioOutPutString( "*", sos ) < 0 )
	    { LDEB(1); return -1;	}
	mpw->mpwPos += 1;
	}

    return 0;
    }

static int docMarkdownSaveRun(	const TextRun *		tr,
				void *			vppw )
    {
    MarkdownParagraphWriter *	mpw= (MarkdownParagraphWriter *)vppw;
    MarkdownDocumentWriter *	mdw= mpw->mpwMarkdownWriter;
    SimpleOutputStream *	sos= mdw->mdwSos;

    const BufferItem *		paraNode= tr->trParaNode;
    const char *		s= docParaString( paraNode, tr->trStroff );
    int				from= 0;
    int				past= from;

    if  ( tr->trStrlen > 0 )
	{
	if  ( docMarkdownSwitchTextAttributes( mpw,
			&(mpw->mpwCurrentAttribute), tr->trTextAttribute ) )
	    { LDEB(1); return -1;	}

	mpw->mpwCurrentAttribute= *(tr->trTextAttribute);
	}

    while( from < tr->trStrlen )
	{
	while( past < tr->trStrlen && s[past] != ' ' )
	    {  past++;	}
	while( past < tr->trStrlen && s[past] == ' ' )
	    {  past++;	}

	if  ( mpw->mpwPos > 8			&&
	      mpw->mpwPos+ past- from >= 72	&&
	      s[0] != ' '			)
	    {
	    if  ( sioOutPutByte( '\n', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    mpw->mpwPos= 0;
	    }

	while( from < past )
	    {
	    if  ( sioOutPutByte( s[from], sos ) < 0 )
		{ LDEB(1); return -1;	}

	    from++; mpw->mpwPos++;
	    mpw->mpwAtHead= 0;
	    }

	}

    return SCANadviceOK;
    }

static int docMarkdownSaveParaNode(
				MarkdownDocumentWriter *	mdw,
				struct BufferItem *		paraNode,
				const DocumentSelection *	ds )
    {
    MarkdownParagraphWriter	mpw;
    const int			scanFlags= 0;
    TextAttribute		taPlain;

    mpw.mpwMarkdownWriter= mdw;
    mpw.mpwPos= 0;
    mpw.mpwClose= 1;
    mpw.mpwAtHead= 1;

    textInitTextAttribute( &(mpw.mpwCurrentAttribute) );

    /*
    if  ( ds )
	{
	if  ( ds->dsTail.dpNode == paraNode )
	    { mpw.mpwClose= ! mdw->mdwHasOpenEnd;	}
	}
    else{
	if  ( ! docNextParagraph( (struct BufferItem *)paraNode ) )
	    { mpw.mpwClose= ! mdw->mdwHasOpenEnd;	}
	}
    */

    if  ( docScanParagraphLogicalOrder( mdw->mdwDocument, paraNode,
				ds, scanFlags,
				docMarkdownSaveParticule,
				docMarkdownSaveField,
				docMarkdownSaveRun,
				docMarkdownSaveObject,
				(TabVisitor)0,
				&mpw ) < 0 )
	{ LDEB(1); return -1;	}

    textInitTextAttribute( &taPlain );

    if  ( docMarkdownSwitchTextAttributes( &mpw,
			&taPlain, &(mpw.mpwCurrentAttribute) ) )
	{ LDEB(1); return -1;	}

    if  ( mpw.mpwClose )
	{
	if  ( sioOutPutString( "\n\n", mdw->mdwSos ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docMarkdownWriteEnterNode(
				struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voidpwc )
    {
    MarkdownDocumentWriter *	mdw= (MarkdownDocumentWriter *)voidpwc;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    return SCANadviceOK;

	case DOClevPARA:
	    if  ( docMarkdownSaveParaNode( mdw, node, ds ) )
		{ LDEB(1); return -1;	}
	    return SCANadviceOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }


int docMarkdownSaveDocument(	SimpleOutputStream *		sos,
				struct BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    const int			flags= 0;
    MarkdownDocumentWriter		mdw;

    docInitMarkdownDocumentWriter( &mdw );

    /*
    mdw.mdwHasOpenEnd= bd->bdProperties->dpHasOpenEnd;
    */
    mdw.mdwDocument= bd;
    mdw.mdwSos= sos;

    /*
    if  ( ds )
	{ mdw.mdwHasOpenEnd= 1;	}
    */

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
				docMarkdownWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&mdw ) < 0 )
	    { LDEB(1); return -1; }
	}
    else{
	if  ( docScanTree( bd, &(bd->bdBody),
				docMarkdownWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&mdw ) < 0 )
	    { LDEB(1); return -1; }
	}

    if  ( ! ds && mdw.mdwNoteRefCount > 0 )
	{
	DocumentField *	dfNote;
	DocumentNote *	dn;

	sioOutPutString( "\n-----\n\n", sos );

	for ( dfNote= docGetFirstNoteOfDocument( &dn, bd, -1 );
	      dfNote;
	      dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 ) )
	    {
	    if  ( docScanTree( bd, &(dn->dnDocumentTree),
		    docMarkdownWriteEnterNode, (NodeVisitor)0,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)&mdw ) < 0 )
		{ LDEB(1); return -1; }
	    }

	if  ( mdw.mdwNoteDefCount != mdw.mdwNoteRefCount )
	    { LLDEB(mdw.mdwNoteDefCount,mdw.mdwNoteRefCount);	}
	}

    return 0;
    }
