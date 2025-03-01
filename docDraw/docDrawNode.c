#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	<docPageGrid.h>
#   include	<docLayout.h>
#   include	<layoutContext.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>
#   include	<docSelect.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static int docDrawNodeLow(	LayoutPosition *		lpBelow,
				struct BufferItem *		node,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo );


static void docTableHeaderRectangle(
				DocumentRectangle *	drPixels,
				const BlockFrame *	bf,
				struct BufferItem *	node,
				DrawingContext *	dc )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    LayoutPosition	lpHeader= node->biRowAboveHeaderPosition;

    docGetPixelRectForPos( drPixels, lc,
			    bf->bfContentRect.drX0, bf->bfContentRect.drX1,
			    &lpHeader, &(node->biTopPosition) );

    return;
    }

/************************************************************************/

static int docDrawHeaderForClippedRow(	struct BufferItem *	childNode,
					void *			through,
					DrawingContext *	dc )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				rval= 0;
    BlockFrame			bf;

    DocumentRectangle		drChild;

    docLayoutInitBlockFrame( &bf );

    docSectionBlockFrameTwips( &bf, childNode, dc->dcBodySectNode,
					    lc->lcDocument,
					    childNode->biTopPosition.lpPage,
					    childNode->biTopPosition.lpColumn );

    docTableHeaderRectangle( &drChild, &bf, childNode, dc );

    if  ( geoIntersectRectangle( &drChild, &drChild, dc->dcClipRect ) )
	{
	if  ( docDrawTableHeader( (int *)0,
		    childNode, &bf, through,
		    dc, &(childNode->biRowAboveHeaderPosition) ) )
	    { LDEB(childNode->biRowPrecededByHeader); rval= -1; goto ready; }
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the children of a group item.					*/
/*									*/
/*  3)  Do not proceed beyond last page. To illustrate that table	*/
/*	headers fit on one page, the situation where we are forced into	*/
/*	a particular page block is excepted.				*/
/*  4)  If the child is on an accessible page, move to the column of	*/
/*	the child.							*/
/*									*/
/************************************************************************/

int docDrawGroupNode(	LayoutPosition *		lpBelow,
			const struct BufferItem *	node,
			void *				through,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    int				i;
    LayoutPosition		lpHere;

    struct BufferItem *		prevNode= (struct BufferItem *)0;

    if  ( dc->dcStartNode && (*dc->dcStartNode)( through, dc, bo->boOverrideFrame, 0, node ) )
	{ LDEB(node->biLevel); return -1; }

    if  ( node->biChildCount > 0 )
	{ prevNode= node->biChildren[0];	}

    lpHere= node->biTopPosition;
    for ( i= 0; i < node->biChildCount; i++ )
	{
	struct BufferItem *		childNode= node->biChildren[i];

	if  ( dc->dcClipRect )
	    {
	    DocumentRectangle	drChild;

	    docNodeRectangle( &drChild, childNode, dc->dcLayoutContext, bo );

	    if  ( ! geoIntersectRectangle( &drChild, &drChild,
							dc->dcClipRect ) )
		{
		if  ( childNode->biLevel == DOClevROW	&&
		      childNode->biRowPrecededByHeader	)
		    {
		    if  ( docDrawHeaderForClippedRow( childNode, through, dc ) )
			{ LDEB(i); return -1;	}
		    }

		continue;
		}
	    }

	if  ( dc->dcSelection						&&
	      docSelectionSameRoot( dc->dcSelection, childNode )	&&
	      docCompareNodePositionToSelection( childNode,
						dc->dcSelection ) > 0	)
	    { break;	}

	/*  3  */
	if  ( dc->dcLastPage >= 0				&&
	      ( ! bo || ! bo->boOverrideFrame )			&&
	      childNode->biTopPosition.lpPage > dc->dcLastPage	)
	    { break;	}

	/*  4  */
	if  ( dc->dcFirstPage < 0					||
	      childNode->biBelowPosition.lpPage >= dc->dcFirstPage	)
	    {
	    if  ( docDrawToColumnOfNode( prevNode, childNode,
					    through, &lpHere, dc, bo ) )
		{ SDEB(docLevelStr(childNode->biLevel)); return -1;	}

	    if  ( docDrawNodeLow( lpBelow, childNode, through, dc, bo ) )
		{ LDEB(i); return -1;	}

	    lpHere= childNode->biBelowPosition;
	    }

	prevNode= childNode;
	}

    if  ( dc->dcFinishNode && (*dc->dcFinishNode)( through, dc, bo->boOverrideFrame, 0, node ) )
	{ LDEB(node->biLevel); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the document root item.					*/
/*									*/
/************************************************************************/

static int docDrawDocNode(	LayoutPosition *		lpBelow,
				const struct BufferItem *	docNode,
				void *				through,
				DrawingContext *		dc )
    {
    int				rval= 0;

    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    BlockOrigin			bo;

    const struct DocumentTree *	saveTree= dc->dcTree;
    const struct BufferItem *	saveBodySectNode= dc->dcBodySectNode;

    /* dc->dcStartNode ... Done by docDrawGroupNode */

    dc->dcTree= &(bd->bdBody);

    docInitBlockOrigin( &bo );

    if  ( docNode->biTreeType == DOCinBODY		&&
	  docNode->biChildCount > 0			)
	{ dc->dcBodySectNode= docNode->biChildren[0];	}

    if  ( docDrawGroupNode( lpBelow, docNode, through, dc, &bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	if  ( docDrawEndnotesForDocument( lpBelow, through, dc ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( lpBelow && dc->dcDrawOtherTrees )
	{
	docDrawFootnotesForColumn( lpBelow->lpPage, lpBelow->lpColumn,
								through, dc );
	}

    /* dc->dcFinishNode ... Done by docDrawGroupNode */

  ready:

    dc->dcBodySectNode= saveBodySectNode;
    dc->dcTree= saveTree;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw a section and its dependants.					*/
/*									*/
/************************************************************************/

static int docDrawSectNode(	LayoutPosition *		lpBelow,
				const struct BufferItem *	sectNode,
				void *				through,
				DrawingContext *		dc )
    {
    int				rval= 0;

    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    BlockOrigin			bo;

    const struct DocumentTree *	saveTree= dc->dcTree;
    const struct BufferItem *	saveBodySectNode= dc->dcBodySectNode;

    /* dc->dcStartNode ... Done by docDrawGroupNode */

    docInitBlockOrigin( &bo );

    if  ( sectNode->biTreeType == DOCinBODY )
	{ dc->dcBodySectNode= sectNode;	}

    if  ( docDrawGroupNode( lpBelow, sectNode, through, dc, &bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( sectNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceSECT_END	)
	{
	if  ( docDrawEndnotesForSection( lpBelow, sectNode->biNumberInParent,
							    through, dc ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /* dc->dcFinishNode ... Done by docDrawGroupNode */

  ready:

    dc->dcBodySectNode= saveBodySectNode;
    dc->dcTree= saveTree;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw an arbitrary node.						*/
/*									*/
/*  1)  Exception for table rows with 'rowspan' cells.			*/
/*  2)  Intersects clipping rectangle?					*/
/*  3)  Inside selection to draw?					*/
/*  4)  Inside page range to draw.					*/
/*									*/
/************************************************************************/

static int docDrawNodeLow(	LayoutPosition *		lpBelow,
				struct BufferItem *		node,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const DocumentSelection *	ds= dc->dcSelection;

    LayoutPosition		lpBelowNodeOffspring;

    /*  1  */
    lpBelowNodeOffspring= node->biBelowPosition;
    if  ( docIsRowNode( node ) )
	{ lpBelowNodeOffspring= node->biRowBelowAllCellsPosition;	}

    /*  2  */
    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drNode;

	docNodeRectangle( &drNode, node, dc->dcLayoutContext, bo );

	if  ( ! geoIntersectRectangle( &drNode, &drNode, dc->dcClipRect )  )
	    { return 0;	}
	}

    /*  3  */
    if  ( ds && docSelectionSameRoot( ds, node ) )
	{
	if  ( docCompareNodePositionToSelection( node, ds ) != 0 )
	    { return 0;	}
	}

    /*  4  */
    if  ( dc->dcFirstPage >= 0				&&
	  lpBelowNodeOffspring.lpPage < dc->dcFirstPage	)
	{ return 0;	}

    if  ( dc->dcLastPage >= 0				&&
	  node->biTopPosition.lpPage > dc->dcLastPage	)
	{ return 0;	}

    if  ( lpBelow )
	{
	docLayoutPushBottomDown( lpBelow, &(node->biTopPosition) );
	}

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docDrawDocNode( lpBelow, node, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docDrawSectNode( lpBelow, node, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docDrawGroupNode( lpBelow, node, through, dc, bo ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( ! docIsRowNode( node ) )
		{
		if  ( docDrawGroupNode( lpBelow, node, through, dc, bo ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( docDrawRowNode( node, through, dc, bo ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( docDrawParaNode( lpBelow, node, through, dc, bo ) < 0 )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return 0;
    }

int docDrawNode(	LayoutPosition *		lpBelow,
			struct BufferItem *		node,
			void *				through,
			DrawingContext *		dc )
    {
    BlockOrigin			bo;

    docInitBlockOrigin( &bo );

    return docDrawNodeLow( lpBelow, node, through, dc, &bo );
    }

/************************************************************************/
/*									*/
/*  Draw a series of pages.						*/
/*									*/
/************************************************************************/

int docDrawPageRange(	DrawingContext *	dc,
			void *			through )
    {
    struct BufferItem * rootNode= dc->dcLayoutContext->lcDocument->bdBody.dtRoot;

    int			sectNr;
    int			page;

    LayoutPosition	lpBelow;

    docInitLayoutPosition( &lpBelow );

    if  ( docDrawNode( (LayoutPosition *)0, rootNode, through, dc ) )
	{ LDEB(1);	}

    for ( sectNr= 0; sectNr < rootNode->biChildCount; sectNr++ )
	{
	struct BufferItem *	bodySectNode= rootNode->biChildren[sectNr];
	struct BufferItem *	prevSectNode= (struct BufferItem *)0;
	int			first= bodySectNode->biTopPosition.lpPage;
	int			last= bodySectNode->biBelowPosition.lpPage;

	if  ( first < dc->dcFirstPage )
	    { first=  dc->dcFirstPage;	}
	if  ( last >  dc->dcLastPage )
	    { last=   dc->dcLastPage;		}

	if  ( sectNr > 0 )
	    { prevSectNode= rootNode->biChildren[sectNr- 1];	}
	/*
	Word draws the footer of the first section on the page.
	if  ( sectNr < rootNode->biChildCount- 1 )
	    { nextSectNode= rootNode->biChildren[sectNr+ 1];	}
	*/

	for ( page= first; page <= last; page++ )
	    {
	    int		col;
	    const int	belowText= 0;

	    if  ( ! prevSectNode				||
		  prevSectNode->biBelowPosition.lpPage < page	)
		{ docDrawPageHeader( bodySectNode, through, dc, page ); }

	    for ( col= 0; col < bodySectNode->biSectColumnCount; col++ )
		{
		docDrawFootnotesForColumn( page, col, through, dc );
		}

	    /* Word draws the footer of the first section on the page. */
	    if  ( ! prevSectNode				||
		  prevSectNode->biBelowPosition.lpPage < page	)
		{ docDrawPageFooter( bodySectNode, through, dc, page ); }

	    if  ( docDrawShapesForPage( through, dc, belowText, page ) )
		{ LDEB(page);	}
	    }
	}

    return 0;
    }
