/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<docParaParticules.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Block level ones
 */
static const char STRUCTtypeP[]= "P";
/*static const char STRUCTtypeDIV[]= "Div";*/
static const char STRUCTtypePART[]= "Part";
static const char STRUCTtypeSECT[]= "Sect";
static const char STRUCTtypeTD[]= "TD";
static const char STRUCTtypeTR[]= "TR";
static const char STRUCTtypeTABLE[]= "Table";

/************************************************************************/

/**
 * Only mark nodes that intersect a single page.
 */
static int docPsMarkNode(	const struct BufferItem *		node )
    {
    return 0 && node->biTopPosition.lpPage == node->biBelowPosition.lpPage;
    }

static int docPsPrintBeginMarkedGroup(
				PrintingState *		ps,
				const char *		structureType )
    {
    const int	contentId= -1;

    StructItem * structItem= psPdfGroupStructItem( ps, structureType, contentId );

    if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	{ XDEB(structItem); return -1;	}

    if  ( psPdfmarkAppendMarkedGroup( ps, structItem ) )
	{ LDEB(1); return -1;	}

    return psPdfBeginMarkedContent( ps, structureType, -1 );
    }

static int docPsPrintEndMarkedGroup(
				PrintingState *		ps )
    {
    psPdfPopStructItem( ps );
    return psPdfEndMarkedContent( ps );
    }

/**
 *  Begin an artifact in the output. We want to avoid /Artifacts 
 *  inside marked content. For that reason, we pop a possible 
 *  open /Span from the stack
 */
static int docPsPrintBeginTypedArtifact(
			PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName )
    {
    if  ( docPsPrintFinishInline( ps ) )
	{ LDEB(1); return -1;	}

    if  ( psPdfBeginTypedArtifact( ps, typeName, subtypeName ) )
	{ LDEB(ps->psInArtifact); return -1;	}

    ps->psInArtifact++;

    return 0;
    }

/**
 *  Begin an artifact in the output. We want to avoid /Artifacts 
 *  inside marked content. For that reason, we pop a possible 
 *  open /Span from the stack
 */
int docPsPrintBeginArtifact(
			PrintingState *			ps )
    {
    if  ( docPsPrintFinishInline( ps ) )
	{ LDEB(1); return -1;	}

    if  ( psPdfBeginMarkedContent( ps, "Artifact", -1 ) )
	{ LDEB(ps->psInArtifact); return -1;	}

    ps->psInArtifact++;

    return 0;
    }

/**
 *  End an artifact. Simply pop the piece of marked content.
 */
int docPsPrintEndArtifact(	PrintingState *		ps )
    {
    ps->psInArtifact--;

    return psPdfEndMarkedContent( ps );
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

    if  ( ps->psInsideLink )
	{
	SLDEB("####",ps->psInsideLink);
	ps->psInsideLink= 0;
	}

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
	    if  ( docPsPrintBeginTypedArtifact( ps, "Pagination", "Header" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	/* See ISO 14289-1, 7.8 and ISO 32000-1:2008, 14.8.2.2.2, Table 330 */
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docPsPrintBeginTypedArtifact( ps, "Pagination", "Footer" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
	    if  ( docPsPrintBeginArtifact( ps )	)
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
	    if  ( docPsPrintEndArtifact( ps )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    if  ( docPsPrintEndArtifact( ps ) )
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;
	}

    return 0;
    }

/**
 *  Start printing a slice of a paragraph. Currently, we make 
 *  a /Div content item. Once we have the structural hierarchy 
 *  in place, consider using /NonStruct: The paragraph that holds 
 *  the lines determines the document structure.
 */
int docPsPrintStartLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	node,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;

    /* Temporarily, we do this */
    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( node ) )
	{
	if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeP ) )
	    { LDEB(node->biLevel); return -1;	}
	}

    return 0;
    }

/**
 *  Finish printing a slice of a paragraph. If we are in an open 
 *  span, close the span.
 */
int docPsPrintFinishLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	node,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( node ) )
	{
	if  ( docPsPrintFinishInline( ps )	)
	    { LDEB(1); return -1;	}

	if  ( docPsPrintEndMarkedGroup( ps ) )
	    { LDEB(node->biLevel); return -1;	}
	}

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

    if  ( ps->psInsideLink )
	{
	SLDEB("####",ps->psInsideLink);
	ps->psInsideLink= 0;
	}

    if  ( ! ps->psInArtifact )
	{
	switch( node->biLevel )
	    {
	    case DOClevBODY:
		if  ( docPsMarkNode( node )			&&
		      docPsPrintBeginMarkedGroup( ps, STRUCTtypePART ) )
		    { LDEB(node->biLevel); return -1;	}
		break;

	    case DOClevSECT:
		if  ( docPsMarkNode( node )			&&
		      docPsPrintBeginMarkedGroup( ps, STRUCTtypeSECT ) )
		    { LDEB(node->biLevel); return -1;	}
		break;

	    case DOClevROW:
		if  ( docPsMarkNode( node )			&&
		      docIsRowNode( node )			)
		    {
		    if  ( node->biNumberInParent == node->biRowTableFirst )
			{
			if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTABLE ) )
			    { LDEB(node->biLevel); return -1;	}
			}

		    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTR ) )
			{ LDEB(node->biLevel); return -1;	}
		    }
		break;

	    case DOClevCELL:
		if  ( docPsMarkNode( node )			&&
		      docIsRowNode( node->biParent )	)
		    {
		    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTD ) )
			{ LDEB(node->biLevel); return -1;	}
		    }
		break;

	    case DOClevPARA:
		/* Do not include in hierarchy: The block of lines on one page is marked as a paragraph */
		break;

	    default:
		SDEB(docLevelStr(node->biLevel)); return -1;
	    }
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

    if  ( ! ps->psInArtifact )
	{
	switch( node->biLevel )
	    {
	    case DOClevBODY:
		if  ( docPsMarkNode( node )			&&
		      docPsPrintEndMarkedGroup( ps )	)
		    { LDEB(node->biLevel); return -1;	}
		break;

	    case DOClevSECT:
		if  ( docPsMarkNode( node )			&&
		    docPsPrintEndMarkedGroup( ps )		)
		    { LDEB(node->biLevel); return -1;	}
		break;


	    case DOClevROW:
		if  ( docPsMarkNode( node )			&&
		      docIsRowNode( node )			)
		    {
		    if  ( docPsPrintEndMarkedGroup( ps ) )
			{ LDEB(node->biLevel); return -1;	}

		    if  ( node->biNumberInParent == node->biRowTablePast- 1 )
			{
			/* Finish table */
			if  ( docPsPrintEndMarkedGroup( ps ) )
			    { LDEB(node->biLevel); return -1;	}
			}
		    }

		break;

	    case DOClevCELL:
		if  ( docPsMarkNode( node )			&&
		      docIsRowNode( node->biParent )	&&
		      docPsPrintEndMarkedGroup( ps )	)
		    { LDEB(node->biLevel); return -1;	}
		break;

	    case DOClevPARA:
		/* Do not include in hierarchy: The block of lines on one page is marked as a paragraph */
		break;

	    default:
		SDEB(docLevelStr(node->biLevel)); return -1;
	    }
	}

    return 0;
    }

int docPsFinishAnnotation(	PrintingState *		ps,
				int			x1Twips,
				int			lineTop,
				int			lineHeight )
    {
    const StructItem *	structItem= ps->psCurrentStructItem;

    if  ( ! structItem || utilMemoryBufferIsEmpty( &(structItem->siAnnotationDictionaryName) ) )
	{ XDEB(structItem); return -1;	}

    psSetLinkRectangle( ps, x1Twips, lineTop, lineHeight,
					utilMemoryBufferGetString( &(structItem->siAnnotationDictionaryName) ) );

    return 0;
    }

