/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

#   define	SHOW_PAGE_GRID	0

/************************************************************************/

static int docPsMarkNode(	struct BufferItem *		node )
    {
    return 0 && node->biTreeType == DOCinBODY &&
	    node->biTopPosition.lpPage == node->biBelowPosition.lpPage;
    }

int docPsPrintBeginMarkedContent(
				struct DrawingContext *	dc,
				PrintingState *		ps,
				const char *		structureType,
				int			contentId )
    {
    /*sioOutPrintf( ps->psSos, "gsave\n" );*/
    return psPdfBeginMarkedContent( ps, structureType, contentId );
    }

int docPsPrintEndMarkedContent(	struct DrawingContext *	dc,
				PrintingState *		ps )
    {
    psPdfEndMarkedContent( ps );

    /*
    sioOutPrintf( ps->psSos, "grestore\n" );
    docResetDrawingContextState( dc );
    */

    return 0;
    }

static int docPsPrintBeginTypedArtifact(
			struct DrawingContext *		dc,
			PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName )
    {
    /*sioOutPrintf( ps->psSos, "gsave\n" );*/
    return psPdfBeginArtifact( ps, typeName, subtypeName, -1 );
    }

int docPsPrintBeginFigure(
			struct DrawingContext *		dc,
			PrintingState *			ps,
			const struct MemoryBuffer *	altText )
    {
    /*sioOutPrintf( ps->psSos, "gsave\n" );*/
    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	const int contentId= psNewContentId( ps );

	return psPdfBeginFigure( ps, altText, contentId );
	}
    else{
	return docPsPrintBeginArtifact( dc, ps );
	}
    }

int docPsPrintBeginArtifact(
			struct DrawingContext *		dc,
			PrintingState *			ps )
    {
    /*sioOutPrintf( ps->psSos, "gsave\n" );*/
    return psPdfBeginMarkedContent( ps, "Artifact", -1 );
    }

/************************************************************************/
/*									*/
/*  Start/Finish printing a (sub)tree in the document tree. This is	*/
/*  invoked to mark the content structure E.G. to support reflowing or	*/
/*  PDF-UA.								*/
/*									*/
/************************************************************************/

int docPsPrintStartTree(	void *				vps,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree )
    {
    PrintingState *	ps= (PrintingState *)vps;

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    /* Part of the regular flow */
	    break;

	/* See ISO 14289-1, 7.8 and ISO 32000-1:2008, 14.8.2.2.2, Table 330 */
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	    if  ( docPsPrintBeginTypedArtifact( dc, ps, "Pagination", "Header" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	/* See ISO 14289-1, 7.8 and ISO 32000-1:2008, 14.8.2.2.2, Table 330 */
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docPsPrintBeginTypedArtifact( dc, ps, "Pagination", "Footer" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
	    if  ( docPsPrintBeginMarkedContent( dc, ps, "Artifact", -1 )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;
	}

    return 0;
    }

int docPsPrintFinishTree( void *			vps,
			struct DrawingContext *		dc,
			struct DocumentTree *		tree )
    {
    PrintingState *	ps= (PrintingState *)vps;

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    /* Part of the regular flow */
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docPsPrintEndMarkedContent( dc, ps )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    if  ( tree->dtRoot->biTreeType != DOCinBODY		&&
		  docPsPrintEndMarkedContent( dc, ps )			)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start printing slice of a paragraph.				*/
/*									*/
/*  This may involve closing and reopening many levels in the marked	*/
/*  content. NOTE 3 in ISO 32000-1:2008, 14.6.1 explicitly forbids	*/
/*  marked sequences to span pages.					*/
/*									*/
/************************************************************************/

int docPsPrintStartLines( void *			vps,
			struct DrawingContext *		dc,
			struct BufferItem *		node )
    {
    if  ( node->biTreeType == DOCinBODY )
	{
	PrintingState *	ps= (PrintingState *)vps;
	const int	contentId= psNewContentId( ps );

	if  ( psPdfmarkAppendContentToReadingOrder(
					ps, "P", contentId ) )
	    { LDEB(node->biLevel); return -1;	}

	if  ( docPsPrintBeginMarkedContent( dc, ps, "P", contentId ) )
	    { LDEB(node->biLevel); return -1;	}
	}

    return 0;
    }

int docPsPrintFinishLines( void *			vps,
			struct DrawingContext *		dc,
			struct BufferItem *		node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( node->biTreeType == DOCinBODY 	&&
	  docPsPrintEndMarkedContent( dc, ps )	)
	{ LDEB(node->biLevel); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start printing a node in the document tree. This is only invoked	*/
/*  to mark the content structure E.G. to support reflowing or PDF-UA.	*/
/*									*/
/************************************************************************/

int docPsPrintStartNode(	void *				vps,
				struct DrawingContext *		dc,
				struct BufferItem *		node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docPsMarkNode( node )			&&
	          docPsPrintBeginMarkedContent( dc, ps, "Part", -1 ) )
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docPsMarkNode( node )			&&
		  docPsPrintBeginMarkedContent( dc, ps, "Sect", -1 ) )
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docPsMarkNode( node )			&&
		  docIsRowNode( node->biParent )	)
		{
		if  ( docPsPrintBeginMarkedContent( dc, ps, "TD", -1 ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( docPsMarkNode( node )			&&
		  docIsRowNode( node )			)
		{
		if  ( node->biNumberInParent == node->biRowTableFirst )
		    {
		    if  ( docPsPrintBeginMarkedContent( dc, ps, "Table", -1 ) )
			{ LDEB(node->biLevel); return -1;	}
		    }

		if  ( docPsPrintBeginMarkedContent( dc, ps, "TR", -1 ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevPARA:
	    /* Include as "Div": The actual lines are embedded in a "P" and */
	    /* inserted in the reading order. */
	    if  ( docPsMarkNode( node )			&&
		  docPsPrintBeginMarkedContent( dc, ps, "Div", -1 ) )
		{ LDEB(node->biLevel); return -1;	}
	    break;

	default:
	    SDEB(docLevelStr(node->biLevel)); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish printing a node in the document tree. This is only invoked	*/
/*  to mark the content structure E.G. to support reflowing or PDF-UA.	*/
/*									*/
/************************************************************************/

int docPsPrintFinishNode( void *			vps,
			struct DrawingContext *		dc,
			struct BufferItem *		node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docPsMarkNode( node )			&&
		  docPsPrintEndMarkedContent( dc, ps )	)
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docPsMarkNode( node )			&&
		  docPsPrintEndMarkedContent( dc, ps )	)
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docPsMarkNode( node )			)
		{
		if  ( docPsPrintEndMarkedContent( dc, ps ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevCELL:
	    if  ( docPsMarkNode( node )			&&
		  docIsRowNode( node->biParent )	)
		{
		if  ( docPsPrintEndMarkedContent( dc, ps ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevROW:

	    if  ( docPsMarkNode( node )			&&
		  docIsRowNode( node )			)
		{
		if  ( docPsPrintEndMarkedContent( dc, ps ) )
		    { LDEB(node->biLevel); return -1;	}

		if  ( node->biNumberInParent == node->biRowTablePast- 1 )
		    {
		    /* Finish table */
		    if  ( docPsPrintEndMarkedContent( dc, ps ) )
			{ LDEB(node->biLevel); return -1;	}
		    }
		}

	    break;

	default:
	    SDEB(docLevelStr(node->biLevel)); return -1;
	}

    return 0;
    }

