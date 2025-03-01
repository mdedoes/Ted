#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	"docDrawPara.h"
#   include	<docSelectLayout.h>
#   include	<layoutContext.h>
#   include	<docPageGrid.h>
#   include	<docRowLayout.h>
#   include	<docTreeNode.h>
#   include	<docRowProperties.h>
#   include	<docBlockOrnaments.h>
#   include	<docStripFrame.h>
#   include	<docCellProperties.h>
#   include	<docBlockFrame.h>
#   include	<docSelect.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static int docDrawRowPageStrip(	const struct BufferItem *	rowNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

/************************************************************************/
/*									*/
/*  Draw part of the grid originating from a table row.			*/
/*									*/
/*  4)  For all cells.....						*/
/*  5)  Skip cells that are included in the colspan or rowspan of	*/
/*	another cell. The left/top cell of a span has to draw the	*/
/*	borders and the background.					*/
/*									*/
/************************************************************************/

static int docDrawCellOrnamentsForRow(
			const struct BufferItem *		rowNode,
			const BlockFrame *			bf,
			const BlockOrigin *			bo,
			void *					through,
			DrawingContext *			dc,
			int					atRowTop,
			int					atRowBottom,
			const LayoutPosition *			lpTop )
    {
    const LayoutContext *		lc= dc->dcLayoutContext;
    const struct BufferDocument *	bd= lc->lcDocument;
    const RowProperties * 		rp= rowNode->biRowProperties;
    const CellProperties *		cp;
    int					col;

    if  ( ! dc->dcDrawOrnaments )
	{ return 0;	}

    /*  4  */
    cp= rp->rpCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	struct BufferItem *		cellNode= rowNode->biChildren[col];

	BlockOrnaments			cellOrnaments;
	DocumentRectangle		drCell;
	DocumentRectangle		drOutside;
	DocumentRectangle		drInside;

	docInitBlockOrnaments( &cellOrnaments );

	/*  5  */
	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	docCellRectangleTwips( &drCell, bf, cellNode );

	drCell.drY0= lpTop->lpPageYTwips;

	if  ( bo )
	    {
	    drCell.drX0 += bo->boXShift;
	    drCell.drX1 += bo->boXShift;
	    drCell.drY1 += bo->boYShift;
	    }

	docGetCellOrnaments( &cellOrnaments, &drOutside, &drInside, &drCell,
			bd, rowNode, col,
			atRowTop, atRowBottom, dc->dcDrawTableGrid );

	if  ( (*dc->dcDrawOrnaments)( &cellOrnaments, lpTop->lpPage,
					&drOutside, &drInside, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/

static int docDrawAdvanceRowInCell(
				void *				through,
				const struct BufferItem *	rowNode,
				LayoutPosition *		lpThisFrame,
				LayoutPosition *		lpBelow,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    if  ( docDrawRowPageStrip( rowNode, lpThisFrame, bf, through, dc, bo ) )
	{ LDEB(1); return -1;	}

    /*  Only used in same frame */
    *lpBelow= rowNode->biBelowPosition;

    return 0;
    }

/************************************************************************/

static int docDrawAdvanceParaInCell(
				void *				through,
				const struct BufferItem *	cellNode,
				struct BufferItem *		paraNode,
				const LayoutPosition *		lpThisFrame,
				LayoutPosition *		lpBelow,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    ParagraphDrawingStrip	pds;
    const int			countAfter= 0;

    ParagraphFrame		pf;
    int				line= 0;

    LayoutPosition		lpShadeTop= *lpThisFrame;

    docParagraphFrameTwips( &pf, bf, paraNode );

    line= docParaFindFirstLineInFrame( paraNode, 0, lpThisFrame, bo );

    if  ( paraNode->biNumberInParent == 0 && line == 0 )
	{ lpShadeTop.lpPageYTwips += cellNode->biCellTopInset;	}

    if  ( docDrawParagraphStrip( through, &pds, paraNode, countAfter,
				&lpShadeTop, line, &pf, dc, lpThisFrame, bo ) )
	{ LDEB(line); return -1;	}

    /*  Only used in same frame */
    *lpBelow= pds.pdsShadeBelow;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print as much of a table-cell/column in a row as fits on the	*/
/*  current page.							*/
/*									*/
/*  1)  Skip entries that end before the current page.			*/
/*  2)  Draw entries that do not start after the current page.		*/
/*									*/
/************************************************************************/

static int docDrawCellPageStrip( void *				through,
				const struct BufferItem *	cellNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo,
				int				atRowTop,
				int				atRowBottom )
				
    {
    int				child= 0;

    LayoutPosition		lpShadeTop= *lpThisFrame;
    LayoutPosition		lpShadeBelow= *lpThisFrame;

    /*  1  */
    while( child < cellNode->biChildCount )
	{
	struct BufferItem *	childNode= cellNode->biChildren[child];
	const LayoutPosition *	belowChildPos;
	LayoutPosition		lpChildBottom;

	if  ( docIsRowNode( childNode ) )
	    { belowChildPos= &(childNode->biRowBelowAllCellsPosition);	}
	else{ belowChildPos= &(childNode->biBelowPosition);		}

	docShiftPosition( &lpChildBottom, bo, belowChildPos );
	if  ( docCompareLayoutPositionFrames( lpThisFrame,
							&lpChildBottom ) <= 0 )
	    { break;	}

	child++;
	}

    /* We do this for every page that the row of the cell appears on */
    if  ( dc->dcStartNode && (*dc->dcStartNode)( through, dc,
				    bo && bo->boOverrideFrame, ! atRowTop, cellNode ) )
	{ LDEB(cellNode->biLevel); return -1; }

    if  ( child < cellNode->biChildCount )
	{
	/*  2  */
	while( child < cellNode->biChildCount )
	    {
	    struct BufferItem *	childNode= cellNode->biChildren[child];
	    LayoutPosition	lpChildTop;

	    docShiftPosition( &lpChildTop, bo, &(childNode->biTopPosition) );

	    if  ( docCompareLayoutPositionFrames( &lpChildTop, lpThisFrame ) > 0 )
		{ break;	}

	    switch( childNode->biLevel )
		{
		case DOClevPARA:
		    if  ( docDrawAdvanceParaInCell( through, cellNode, childNode,
					&lpShadeTop, &lpShadeBelow, bf, dc, bo ) )
			{ LDEB(1); return -1;	}
		    break;

		case DOClevROW:
		    if  ( docDrawAdvanceRowInCell( through, childNode,
					&lpShadeTop, &lpShadeBelow, bf, dc, bo ) )
			{ LDEB(1); return -1;	}
		    break;

		default:
		    LDEB(childNode->biLevel);
		    break;
		}

	    /*  Only used in same frame */
	    lpShadeTop= lpShadeBelow;
	    child++;
	    }

	}

    /* We do this for every page that the row of the cell appears on */
    if  ( dc->dcFinishNode && (*dc->dcFinishNode)( through, dc,
					    bo->boOverrideFrame, ! atRowBottom, cellNode ) )
	{ LDEB(cellNode->biLevel); return -1; }

    return 0;
    }

/************************************************************************/

/**
 * Is the next node on the same page as the current row? (This to not 
 * finish the row where the pagination logic would also close it.)
 */
static int docNextNodeOnNextPage(	const BufferItem *	rowNode )
    {
    const int	nextNumber= rowNode->biNumberInParent+ 1;

    /* NO!  if  ( nextNumber < rowNode->biRowTablePast ) */
    if  ( nextNumber < rowNode->biParent->biChildCount )
	{
	const BufferItem *	nextNode=
				    rowNode->biParent->biChildren[nextNumber];

	return nextNode->biTopPosition.lpPage > rowNode->biBelowPosition.lpPage;
	}
    else{
	return 0;
	}
    }

static int docPrevRowNodeOnPrevPage(	const BufferItem *	rowNode )
    {
    const int	prevRowNumber= rowNode->biNumberInParent- 1;

    if  ( prevRowNumber >= rowNode->biRowTableFirst )
	{
	const BufferItem *	prevRowNode=
				rowNode->biParent->biChildren[prevRowNumber];

	return prevRowNode->biBelowPosition.lpPage < rowNode->biTopPosition.lpPage;
	}
    else{
	return 0;
	}
    }

/************************************************************************/
/*									*/
/*  Print as much of a table row as fits on the current page.		*/
/*									*/
/*  1)  Find the bottom of this row on the page: We need this to draw	*/
/*	the cell ornaments before we draw the row contents.		*/
/*									*/
/************************************************************************/

static int docDrawRowPageStrip(	const BufferItem *		rowNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    int			col;

    LayoutPosition	lpTop;

    int			atRowTop= 0;
    int			atRowBottom= 0;
    int			isTableHeader;

    const int		nextOnNextPage= docNextNodeOnNextPage( rowNode );
    const int		prevOnPrevPage= docPrevRowNodeOnPrevPage( rowNode );

    isTableHeader= rowNode->biNumberInParent < rowNode->biRowPastHeaderRow;
    lpTop= *lpThisFrame;

    if  ( isTableHeader && bo && bo->boOverrideFrame )
	{
	/* assume that the headers fit */
	atRowTop= 1;
	atRowBottom= 1;
	}
    else{
	if  ( docCompareLayoutPositionFrames( lpThisFrame,
				&(rowNode->biTopPosition) ) == 0 )
	    { atRowTop= 1;	}
	if  ( docCompareLayoutPositionFrames( lpThisFrame,
				&(rowNode->biRowBelowAllCellsPosition) ) == 0 )
	    { atRowBottom= 1;	}
	}

    if  ( ! isTableHeader )
	{
	if  ( atRowTop )
	    {
	    docShiftPosition( &lpTop, bo, &(rowNode->biTopPosition) );

	    if  ( rowNode->biRowPrecededByHeader )
		{
		if  ( docDrawTableHeader( (int *)0, rowNode, bf, through,
				    dc, &(rowNode->biRowAboveHeaderPosition) ) )
		    { LDEB(rowNode->biRowPrecededByHeader); return -1;	}
		}
	    }
	else{
	    const int tableHasHeader=
		rowNode->biRowPastHeaderRow > rowNode->biRowTableFirst;

	    if  ( tableHasHeader )
		{
		int		high;

		if  ( docDrawTableHeader( &high, rowNode, bf, through,
							    dc, lpThisFrame ) )
		    { LDEB(tableHasHeader); return -1;	}

		lpTop.lpPageYTwips += high;
		}
	    }
	}

    /* We do this for every page that the row appears on */
    if  ( dc->dcStartNode && (*dc->dcStartNode)( through, dc,
					bo && bo->boOverrideFrame, ! atRowTop || prevOnPrevPage, rowNode ) )
	{ LDEB(rowNode->biLevel); return -1; }

    if  ( docDrawCellOrnamentsForRow( rowNode, bf, bo, through, dc,
					    atRowTop, atRowBottom, &lpTop ) )
	{ LDEB(1); return -1;	}

    {
    const RowProperties * 	rp= rowNode->biRowProperties;
    const CellProperties *	cp= rp->rpCells;

    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	struct BufferItem *	cellNode= rowNode->biChildren[col];

	int			selected;
	int			cmp;

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	cmp= docCompareCellPositionToSelection( &selected, cellNode,
							    dc->dcSelection );
	if  ( cmp > 0 )
	    { break;    }
	if  ( cmp < 0 || ! selected )
	    { continue; }

	if  ( docDrawCellPageStrip( through, cellNode, &lpTop, bf, dc, bo, atRowTop, atRowBottom ) )
	    { LDEB(1); return -1;	}
	}
    }

    /* The pagination code finishes the row. So, only finish at the end of the row */
    if  ( atRowBottom && ! nextOnNextPage )
	{
	if  ( dc->dcFinishNode && (*dc->dcFinishNode)( through, dc,
					bo->boOverrideFrame, ! atRowBottom || nextOnNextPage, rowNode ) )
	    { LDEB(rowNode->biLevel); return -1; }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a table header.						*/
/*									*/
/*  1)  Find the header that belongs to this row.			*/
/*  2)  Determine where the header would be drawn to find the		*/
/*	horizontal offset for header drawing.				*/
/*									*/
/************************************************************************/

int docDrawTableHeader(	int *				pHigh,
			const struct BufferItem *	rowNode,
			const BlockFrame *		bfRef,
			void *				through,
			DrawingContext *		dc,
			const LayoutPosition *		lpHeader )
    {
    int				rval= 0;
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferItem *		headerNode0;
    struct BufferItem *		headerNode1;
    BlockFrame			bfHeader;

    BlockOrigin			bo;

    int				row;

    docInitBlockOrigin( &bo );
    docLayoutInitBlockFrame( &bfHeader );

    /*  1  */
    if  ( rowNode->biRowPastHeaderRow <= rowNode->biRowTableFirst )
	{
	LLDEB(rowNode->biRowPastHeaderRow,rowNode->biRowTableFirst);
	rval= -1; goto ready;
	}

    headerNode0= rowNode->biParent->biChildren[rowNode->biRowTableFirst];
    headerNode1= rowNode->biParent->biChildren[rowNode->biRowPastHeaderRow- 1];

    /*  2  */
    docSectionBlockFrameTwips( &bfHeader, headerNode0, dc->dcBodySectNode,
			    lc->lcDocument,
			    headerNode0->biTopPosition.lpPage,
			    headerNode0->biTopPosition.lpColumn );

    bo.boOverrideFrame= 1;
    bo.boXShift= bfRef->bfContentRect.drX0- bfHeader.bfContentRect.drX0;
    bo.boYShift= lpHeader->lpPageYTwips- headerNode0->biTopPosition.lpPageYTwips;
    bo.boOverridePage= lpHeader->lpPage;
    bo.boOverrideColumn= lpHeader->lpPage;

    for ( row= rowNode->biRowTableFirst;
				row < rowNode->biRowPastHeaderRow; row++ )
	{
	BufferItem *	headerRowNode= rowNode->biParent->biChildren[row];

	if  ( docDrawRowNode( headerRowNode, through, dc, &bo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( pHigh )
	{
	*pHigh= headerNode1->biBelowPosition.lpPageYTwips- 
					headerNode0->biTopPosition.lpPageYTwips;
	}

  ready:

    docLayoutCleanBlockFrame( &bfHeader );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw a table row by successively drawing the portions that fit in	*/
/*  the current (newspaper style) column and moving to the next column.	*/
/*									*/
/*  NOTE that any drawing routine can be recursive.			*/
/*									*/
/************************************************************************/

static const char DRAW_ROW_BREAK_FLOW[]= "row flow";

int docDrawRowNode(	struct BufferItem *		rowNode,
			void *				through,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				rval= 0;
    LayoutPosition		lpThisFrame;
    BlockFrame			bf;

    docLayoutInitBlockFrame( &bf );
    docSectionBlockFrameTwips( &bf, rowNode, dc->dcBodySectNode,
					    lc->lcDocument,
					    rowNode->biTopPosition.lpPage,
					    rowNode->biTopPosition.lpColumn );

    docShiftPosition( &lpThisFrame, bo, &(rowNode->biTopPosition) );

    if  ( docDrawRowPageStrip( rowNode, &lpThisFrame, &bf, through, dc, bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    while( docCompareLayoutPositionFrames(
	    &(rowNode->biRowBelowAllCellsPosition), &lpThisFrame ) > 0 )
	{
	int		ret;

	/*  1  */
	ret= docDrawToNextColumn( rowNode, rowNode, through,
				&lpThisFrame, &bf, dc, DRAW_ROW_BREAK_FLOW );
	if  ( ret < 0 )
	    { SLDEB(docLevelStr(rowNode->biLevel),ret); rval= -1; goto ready; }
	if  ( ret > 0 )
	    { break;	}

	if  ( docDrawRowPageStrip( rowNode,
				    &lpThisFrame, &bf, through, dc, bo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

