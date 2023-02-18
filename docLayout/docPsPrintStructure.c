/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 */
static const char STRUCTtypeP[]= "P";
/*static const char STRUCTtypeDIV[]= "Div";*/
static const char STRUCTtypeSPAN[]= "Span";
static const char STRUCTtypePART[]= "Part";
static const char STRUCTtypeSECT[]= "Sect";
static const char STRUCTtypeTD[]= "TD";
static const char STRUCTtypeTR[]= "TR";
static const char STRUCTtypeTABLE[]= "Table";
static const char STRUCTtypeFIGURE[]= "Figure";
static const char STRUCTtypeLINK[]= "Link";

/************************************************************************/

static int docPsMarkNode(	struct BufferItem *		node )
    {
    return 0 && node->biTreeType == DOCinBODY &&
	    node->biTopPosition.lpPage == node->biBelowPosition.lpPage;
    }

/**
 *  Make sure that if we are emitting textual content that is part 
 *  of the reading order of the document, that the current leaf in 
 *  the structure tree is a /Span or a /Link.
 */
int docPsPrintClaimInline(	PrintingState *		ps,
				struct BufferItem *	paraNode )
    {
    if  ( paraNode->biTreeType == DOCinBODY )
	{
	if  ( ps->psCurrentStructItem 					&&
	      ps->psCurrentStructItem->siIsLeaf				&&
	      ! ps->psCurrentStructItem->siIsInline			)
	    {
	    psPdfPopStructItem( ps );
	    psPdfEndMarkedContent( ps );
	    }

	if  ( ! ps->psCurrentStructItem 		||
	      ! ps->psCurrentStructItem->siIsLeaf	)
	    {
	    StructItem *	structItem;

	    if  ( ps->psInsideLink )
		{
		structItem= psPdfAnnotatedStructItem( ps, STRUCTtypeLINK, 1 );

		if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
		    { XDEB(structItem); return -1;	}

		if  ( psPdfmarkAppendMarkedLink( ps, structItem ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		structItem= psPdfLeafStructItem( ps, STRUCTtypeSPAN, 1 );

		if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
		    { XDEB(structItem); return -1;	}

		if  ( psPdfmarkAppendMarkedLeaf( ps, structItem ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( psPdfBeginMarkedContent( ps,
		  structItem->siStructureType, structItem->siContentId ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/**
 *  If the current StructItem on the stack is a /Span, pop it from 
 *  the stack
 */
int docPsPrintFinishInline(	PrintingState *		ps )
    {
    if  ( ps->psCurrentStructItem 					&&
	  ps->psCurrentStructItem->siIsLeaf				&&
	  ps->psCurrentStructItem->siIsInline				)
	{
	psPdfPopStructItem( ps );
	psPdfEndMarkedContent( ps );
	}

    return 0;
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

int docPsPrintEndFigure(	PrintingState *		ps )
    {
    psPdfPopStructItem( ps );
    return psPdfEndMarkedContent( ps );
    }

int docPsPrintEndArtifact(	PrintingState *		ps )
    {
    return psPdfEndMarkedContent( ps );
    }

int docPsPrintBeginFigure(
		PrintingState *				ps,
		const struct DocumentRectangle *	drTwips,
		const struct MemoryBuffer *		altText )
    {
    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	StructItem * structItem= psPdfLeafStructItem( ps, STRUCTtypeFIGURE, 0 );

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(1); return -1;	}

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return -1;	}

	if  ( psPdfmarkAppendMarkedIllustration(
				ps, structItem, drTwips, altText ) )
	    { LDEB(1); return -1;	}

	if  ( psPdfBeginMarkedContent( ps,
	      structItem->siStructureType, structItem->siContentId ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    else{
	return docPsPrintBeginArtifact( ps );
	}
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

    return psPdfBeginArtifact( ps, typeName, subtypeName );
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
			struct BufferItem *		node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ps->psInsideLink )
	{
	SLDEB("####",ps->psInsideLink);
	ps->psInsideLink= 0;
	}

    /* This would be the correct code
    if  ( docPsMarkNode( node )				&&
	  docPsPrintBeginMarkedGroup( ps, STRUCTtypeP ) )
	{ LDEB(node->biLevel); return -1;	}
    */

    /* Temporarily, we do this */
    if  ( node->biTreeType == DOCinBODY )
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
			struct BufferItem *		node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( node->biTreeType == DOCinBODY	&&
	  docPsPrintFinishInline( ps )	)
	{ LDEB(1); return -1;	}


    /* This would be the correct code
    if  ( docPsMarkNode( node )			&&
	  docPsPrintEndMarkedGroup( ps )	)
	{ LDEB(node->biLevel); return -1;	}
    */

    /* Temporarily, we do this */
    if  ( node->biTreeType == DOCinBODY )
	{
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

    return 0;
    }

