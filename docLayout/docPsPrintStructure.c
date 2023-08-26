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
#   include	<docRowProperties.h>
#   include	<docParaProperties.h>
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
static const char STRUCTtypeTH[]= "TH";
static const char STRUCTtypeTR[]= "TR";
static const char STRUCTtypeTABLE[]= "Table";

static const char STRUCTtypeH1[]= "H1";
static const char STRUCTtypeH2[]= "H2";
static const char STRUCTtypeH3[]= "H3";
static const char STRUCTtypeH4[]= "H4";
static const char STRUCTtypeH5[]= "H5";
static const char STRUCTtypeH6[]= "H6";

/************************************************************************/

/**
 * Only mark nodes that land on a single page.
 */
static int docPsMarkNode(	const struct BufferItem *		node )
    {
    return node->biTopPosition.lpPage == node->biBelowPosition.lpPage;
    }

static const char * docPsCellNodeMark( const struct BufferItem *	cellNode )
    {
    const BufferItem *	rowNode= cellNode->biParent;
    const BufferItem *	tableNode= cellNode->biParent;

    if  ( ! docPsMarkNode( tableNode ) )
	{ return (char *)0;	}

    /* I could not find a cell property to decide between td and th (MdD Aug 2023) */
    if  ( rowNode->biRowProperties->rpIsTableHeader )
	{ return STRUCTtypeTH;	}
    else{ return STRUCTtypeTD;	}
    }

static int docPsMarkRowNode(
	    const struct BufferItem *	rowNode,
	    int *			pAsTableFirst,
	    int *			pAsTableLast )
    {
    const BufferItem *	tableNode= rowNode->biParent;

    if  ( ! docIsRowNode( rowNode ) || ! docPsMarkNode( tableNode ) )
	{ return 0;	}

    if  ( pAsTableFirst )
	{ *pAsTableFirst= rowNode->biNumberInParent == rowNode->biRowTableFirst;	}

    if  ( pAsTableLast )
	{ *pAsTableLast= rowNode->biNumberInParent == rowNode->biRowTablePast- 1;	}

    return 1;
    }

static const char * docPsParagraphNodeMark(
	    PrintingState *		ps,
	    const struct BufferItem *	paraNode )
    {
    if  ( ps->psInArtifact || docParagraphIsEmpty( paraNode ) )
	{ return (char *)0;	}

    switch( paraNode->biParaProperties->ppOutlineLevel ) {
	case 0: return STRUCTtypeH1;
	case 1: return STRUCTtypeH2;
	case 2: return STRUCTtypeH3;
	case 3: return STRUCTtypeH4;
	case 4: return STRUCTtypeH5;
	case 5: return STRUCTtypeH6;
	default: return STRUCTtypeP;
	}
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
			const struct BufferItem *	paraNode,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;
    const char *	mark= docPsParagraphNodeMark( ps, paraNode );

    if  ( mark && docPsPrintBeginMarkedGroup( ps, STRUCTtypeP ) )
	{ LSDEB(paraNode->biLevel,mark); return -1;	}

    return 0;
    }

/**
 *  Finish printing a slice of a paragraph. If we are in an open 
 *  span, close the span.
 */
int docPsPrintFinishLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;
    const char *	mark= docPsParagraphNodeMark( ps, paraNode );

    if  ( mark )
	{
	if  ( docPsPrintFinishInline( ps )	)
	    { LSDEB(paraNode->biLevel,mark); return -1;	}

	if  ( docPsPrintEndMarkedGroup( ps ) )
	    { LSDEB(paraNode->biLevel,mark); return -1;	}
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

	    case DOClevROW: {
		    int		asTableFirst= 0;

		    if  ( docPsMarkRowNode( node, &asTableFirst, (int *)0 ) )
			{
			if  ( asTableFirst && docPsPrintBeginMarkedGroup( ps, STRUCTtypeTABLE ) )
			    { LLDEB(node->biLevel,asTableFirst); return -1;	}

			if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTR ) )
			    { LDEB(node->biLevel); return -1;	}
			}

		    break;
		}

	    case DOClevCELL: {
		const char *	cellNodeMark= docPsCellNodeMark( node );

		if  ( cellNodeMark && docPsPrintBeginMarkedGroup( ps, cellNodeMark ) )
		    { LSDEB(node->biLevel,cellNodeMark);	}

		break;
		}

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


	    case DOClevROW: {
		    int		asTableLast= 0;

		    if  ( docPsMarkRowNode( node, (int *)0, &asTableLast ) )
			{
			if  ( docPsPrintEndMarkedGroup( ps ) )
			    { LDEB(node->biLevel); return -1;	}

			if  ( asTableLast && docPsPrintEndMarkedGroup( ps ) )
			    { LLDEB(node->biLevel,asTableLast); return -1;	}
			}

		    break;
		}

	    case DOClevCELL: {
		const char *	cellNodeMark= docPsCellNodeMark( node );

		if  ( cellNodeMark && docPsPrintEndMarkedGroup( ps ) )
		    { LSDEB(node->biLevel,cellNodeMark);	}

		break;
		}

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

