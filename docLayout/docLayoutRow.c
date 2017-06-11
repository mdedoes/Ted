/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	"docRowLayout.h"
#   include	"docStripLayoutJob.h"
#   include	<docPropVal.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Shift cell contents of the cells in a row that are not top-aligned.	*/
/*									*/
/*  Experimentation with MS-Word shows the following behavior for cells	*/
/*  that span more than one row:					*/
/*									*/
/*  TOP    aligned cells behave as expected.				*/
/*  CENTER aligned cells with content that fits on the first page,	*/
/*	   vertically center the contents on the first page. If the	*/
/*	   content is higher than the space on the first page, the cell	*/
/*	   behaves as a top-aligned cell.				*/
/*  BOTTOM aligned cells with content that fits on the first page, top	*/
/*	   align the contents on the first page. If the content is	*/
/*	   higher than the space on the first page, the cell behaves as	*/
/*	   a top-aligned cell.						*/
/*									*/
/************************************************************************/

static void docLayoutValignCell( BufferItem *			cellNode,
				BlockFrame *			bf,
				const LayoutPosition *		lpRowTop,
				const LayoutPosition *		lpBelow,
				const LayoutJob *		lj )
    {
    const CellProperties *	thisCp= cellNode->biCellProperties;

    LayoutPosition		lpCellTop= cellNode->biTopPosition;
    int				rowHigh= lpBelow->lpPageYTwips-
							lpRowTop->lpPageYTwips;
    int				cellHigh= cellNode->biBelowPosition.lpPageYTwips-
							lpCellTop.lpPageYTwips;

    int				paraFrom= 0;
    int				paraUpto= cellNode->biChildCount;

    if  ( cellHigh >= rowHigh )
	{ return;	}

    switch( thisCp->cpValign )
	{
	case DOCtvaTOP:
	    return;

	case DOCtvaCENTERED:
	    cellNode->biCellTopInset= ( rowHigh- cellHigh )/ 2;
	    lpCellTop.lpPageYTwips += cellNode->biCellTopInset;

	    docRedoParaStripLayout( lj, bf, &lpCellTop,
					    cellNode, paraFrom, paraUpto );
	    return;

	case DOCtvaBOTTOM:
	    cellNode->biCellTopInset= rowHigh- cellHigh;
	    lpCellTop.lpPageYTwips += cellNode->biCellTopInset;

	    docRedoParaStripLayout( lj, bf, &lpCellTop,
					    cellNode, paraFrom, paraUpto );
	    return;

	default:
	    LDEB(thisCp->cpValign);
	    return;
	}
    }

static int docLayoutRowValignCells(
				BufferItem *			rowNode,
				BlockFrame *			bf,
				const LayoutPosition *		lpBelow,
				const ParagraphLayoutPosition *	rowPlp,
				const LayoutJob *		lj )
    {
    int				col;
    const CellProperties *	cp;
    const ParagraphLayoutJob *	plj;

    const RowProperties *	rp= rowNode->biRowProperties;

    cp= rp->rpCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];

	if  ( cellNode->biCellRowspan > 1 )
	    { continue;	}
	if  ( ! docLayoutStripDone( &(plj->pljPos), plj) )
	    { continue;	}


	if  ( docCompareLayoutPositionFrames( &(rowNode->biTopPosition),
							    lpBelow ) != 0 )
	    { continue;	}

	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	    {
	    if  ( cellNode->biCellMergedCellTopRow < 0	)
		{ continue;	}
	    else{
		struct BufferItem *	topRowNode;

		int		topRow= cellNode->biCellMergedCellTopRow;
		int		topCol= cellNode->biCellMergedCellTopCol;

		topRowNode= rowNode->biParent->biChildren[topRow];

		if  ( docCompareLayoutPositionFrames(
				&(topRowNode->biTopPosition), lpBelow ) != 0 )
		    { continue;	}

		docLayoutValignCell( topRowNode->biChildren[topCol], bf,
			    &(topRowNode->biTopPosition), lpBelow, lj );
		}
	    }
	else{
	    docLayoutValignCell( cellNode, bf, &(rowNode->biTopPosition),
								lpBelow, lj );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish row layout.							*/
/*									*/
/*  1)  The bottom is below all cells.					*/
/*  2)  If there is a bottom inset, step over it.			*/
/*  3)  Stretch height to the minimum height (If any)			*/
/*  4)  Stretch height to the exact given height (If any)		*/
/*  5)  Fix the current position if a fixed-height row exceeds its	*/
/*	fixed height.							*/
/*									*/
/************************************************************************/

static void docLayoutFinishRow(	LayoutPosition *		lpHere,
				struct BufferItem *		rowNode,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				ParagraphLayoutPosition *	rowPlp )
    {
    int				col;
    CellProperties *		cp;
    ParagraphLayoutJob *	plj;

    const RowProperties *	rp= rowNode->biRowProperties;

    /*  1  */
    cp= rp->rpCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];
	ParagraphLayoutPosition *	plp= &(plj->pljPos);

	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW		&&
	      cellNode->biCellMergedCellTopRow >= 0		)
	    {
	    docLayoutPushBottomDown( &(rowNode->biRowBelowAllCellsPosition),
					&(plj->cljMergedCellBelowPosion) );
	    docLayoutPushBottomDown( lpHere,
					&(plj->cljMergedCellBelowPosion) );
	    continue;
	    }

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	cellNode->biBelowPosition= plp->plpPos;

	docLayoutPushBottomDown(
		    &(rowNode->biRowBelowAllCellsPosition), &(plp->plpPos) );

	if  ( cellNode->biCellRowspan <= 1 )
	    { docLayoutPushBottomDown( lpHere, &(plp->plpPos) ); }
	}

    if  ( docCompareLayoutPositionFrames( lpHere,
					&(rowNode->biTopPosition) ) == 0 )
	{
	int		rowHeightTwips;

	rowHeightTwips= lpHere->lpPageYTwips-
				    rowNode->biTopPosition.lpPageYTwips;

	/*  3: if rowNode->biRowHeightTwips > 0   */
	if  ( rowHeightTwips < rp->rpHeightTwips )
	    {
	    lpHere->lpPageYTwips +=  rp->rpHeightTwips- rowHeightTwips;
	    lpHere->lpAtTopOfColumn= 0;
	    }

	/*  4: if rowNode->biRowHeightTwips < 0  */
	if  ( rowHeightTwips < -rp->rpHeightTwips )
	    {
	    lpHere->lpPageYTwips += -rp->rpHeightTwips- rowHeightTwips;
	    lpHere->lpAtTopOfColumn= 0;
	    }

	/*  5  */
	if  ( rp->rpHeightTwips < 0			&&
	      rowHeightTwips > -rp->rpHeightTwips	)
	    {
	    /* LLDEB(rowNode->biRowHeightTwips,rowHeightTwips); */
	    lpHere->lpPageYTwips -= rowHeightTwips+ rp->rpHeightTwips;
	    }
	}

    if  ( docLayoutRowValignCells( rowNode, bf, lpHere, rowPlp, lj ) )
	{ LDEB(1);	}

    docLayoutPushBottomDown( &(rowNode->biRowBelowAllCellsPosition), lpHere );

    return;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a table row as fits on the current page.		*/
/*									*/
/*  1)  For a fixed height row, determine the amount of space left for	*/
/*	it.								*/
/*  2)  For all columns in the table..					*/
/*  3)  Cells in a row span or a colspan play no role...		*/
/*  4)  This column was already exhausted on a previous page..		*/
/*  5)  Format paragraphs inside this cell.				*/
/*  6)  Remember how far we have descended on the page.			*/
/*  7)  Anything left over that must be moved to the next page?		*/
/*  7a) Distinguish between the situation where a fixed height row has	*/
/*	reached its capacity and that where the page is full.		*/
/*									*/
/************************************************************************/

static int docLayoutRowPageStrip(
			    LayoutPosition *		lpHere,
			    int *			pToNextColumn,
			    int *			pSomeAtHead,
			    struct BufferItem *		rowNode,
			    ParagraphLayoutPosition *	rowPlp,
			    BlockFrame *		bf,
			    const LayoutJob *		lj )
    {
    int				col;

    int				continueInNextFrame= 0;
    int				someAtHead= 0;

    LayoutPosition		lpBottom;
    const CellProperties *	cp;
    ParagraphLayoutJob *	plj;

    const RowProperties *	rp= rowNode->biRowProperties;

    lpBottom= *lpHere;

    continueInNextFrame= 0;

    if  ( rowPlp->pspChildCount != rowNode->biChildCount )
	{ LLDEB(rowPlp->pspChildCount,rowNode->biChildCount); return -1; }

    /*  1  */

    /*  2  */
    cp= rp->rpCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];
	ParagraphLayoutPosition *	plp= &(plj->pljPos);
	int				stopCode= FORMATstopREADY;

	/*  3  */
	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	/*  4  */
	if  ( docLayoutStripDone( plp, plj ) )
	    { continue;	}

	/*  5  */
	plp->plpPos= *lpHere;

	docCellStripFrame( cellNode, bf, plj );

	if  ( docLayoutStripChildren( &stopCode, plj, bf, cellNode ) )
	    { LDEB(1); return -1;	}

	/*  6  */
	docLayoutPushBottomDown(
		    &(rowNode->biRowBelowAllCellsPosition), &(plp->plpPos) );
	if  ( cellNode->biCellRowspan <= 1 )
	    { docLayoutPushBottomDown( &lpBottom, &(plp->plpPos) );	}

	/*  7,7a  */
	if  ( cellNode->biTreeType == DOCinBODY		&&
	      stopCode == FORMATstopBLOCK_FULL		)
	    { continueInNextFrame= 1;	}
	}

    cp= rp->rpCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	const ParagraphLayoutPosition *	plp= &(plj->pljPos);

	if  ( docLayoutAtStripHead( &(plj->pljPos) ) )
	    {
	    if  ( ! docLayoutStripDone( plp, plj ) )
		{ someAtHead= 1;	}
	    }
	else{ rowPlp->pspChildAdvanced= 1;	}
	}

    *lpHere= lpBottom;
    *pToNextColumn= continueInNextFrame;
    *pSomeAtHead= someAtHead;

    if  ( ! continueInNextFrame )
	{ docLayoutFinishRow( lpHere, rowNode, bf, lj, rowPlp );	}
    else{
	if  ( docLayoutRowValignCells( rowNode, bf, lpHere, rowPlp, lj ) )
	    { LDEB(1);	}
	}

    return 0;
    }

/************************************************************************/

static int docLayoutStartRowCells(
				BufferItem *			rowNode,
				const LayoutJob *		lj,
				const LayoutPosition *		lpHere,
				ParagraphLayoutPosition *	rowPlp )
    {
    int				col;
    ParagraphLayoutJob *	cellPlj;

    int				nextRow= rowNode->biNumberInParent+ 1;
    const BufferItem *		nextRowNode= (const struct BufferItem *)0;

    if  ( nextRow < rowNode->biRowTablePast )
	{ nextRowNode= rowNode->biParent->biChildren[nextRow]; }

    rowNode->biRowBelowAllCellsPosition= *lpHere;

    /*  2  */
    cellPlj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cellPlj++, col++ )
	{
	struct BufferItem *	cellNode= rowNode->biChildren[col];
	const int		para= 0;
	const int		line= 0;
	const int		part= 0;

	const int	recursively= 0;

	docDelimitTables( cellNode, recursively );

	cellNode->biCellTopInset= 0;

	docBeginParagraphLayoutProgress( cellPlj, lj,
					    para, line, part,
					    cellNode->biChildCount, lpHere );

	cellNode->biTopPosition= *lpHere;

	/*  3  */
	cellNode->biCellRowspan= 1;
	if  ( cellNode->biCellProperties->cpVerticalMerge != CELLmergeNONE )
	    {
	    docRowLayoutRowspanAdmin( cellNode, rowNode, nextRowNode,
							nextRow, cellPlj );
	    }

	cellPlj += cellNode->biCellColspan- 1;

	if  ( cellNode->biCellColspan < 1 )
	    { LDEB(cellNode->biCellColspan);	}
	if  ( cellNode->biCellColspan > 1 )
	    { col += cellNode->biCellColspan- 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reserve space for the table headers.				*/
/*									*/
/************************************************************************/

static void docLayoutRowSkipHeaderHeight( LayoutPosition *	lp,
					struct BufferItem *	rowNode,
					int			atTopOfRow )
    {
    const struct BufferItem *	headNode0;
    const struct BufferItem *	headNode1;
    int				headerHeight= 0;

    if  ( rowNode->biRowPastHeaderRow <= rowNode->biRowTableFirst )
	{
	LLDEB(rowNode->biRowPastHeaderRow,rowNode->biRowTableFirst);
	return;
	}
    if  ( rowNode->biNumberInParent < rowNode->biRowPastHeaderRow )
	{
	LLDEB(rowNode->biNumberInParent,rowNode->biRowPastHeaderRow);
	LDEB(rowNode->biRowProperties->rpIsTableHeader);
	return;
	}

    headNode0= rowNode->biParent->biChildren[rowNode->biRowTableFirst];
    headNode1= rowNode->biParent->biChildren[rowNode->biRowPastHeaderRow- 1];

    if  ( docCompareLayoutPositionFrames( &(headNode0->biTopPosition),
					&(headNode1->biBelowPosition) ) != 0 )
	{
	LLDEB(headNode0->biTopPosition.lpPage,
				    headNode0->biTopPosition.lpColumn);
	LLDEB(headNode1->biBelowPosition.lpPage,
				    headNode1->biBelowPosition.lpColumn);
	return;
	}

    if  ( atTopOfRow )
	{
	rowNode->biRowAboveHeaderPosition= *lp;
	rowNode->biRowPrecededByHeader= 1;
	}

    headerHeight= headNode1->biBelowPosition.lpPageYTwips- 
					headNode0->biTopPosition.lpPageYTwips;

    lp->lpAtTopOfColumn= 0;
    lp->lpPageYTwips += headerHeight;

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a row in the document.			*/
/*									*/
/*  1)  Do not advance by row top inset. This is done between the top	*/
/*	of the paragraph and its first line. [This is what MS-Word	*/
/*	does.]								*/
/*  2)  Begin formatting.						*/
/*  3)  Keep track of the rowspan of the cells.				*/
/*									*/
/************************************************************************/

static int docLayoutStartRow(
			int *				pToNextColumn,
			BufferItem *			rowNode,
			int				precededByHeader,
			const LayoutJob *		lj,
			LayoutPosition *		lpHere,
			const BlockFrame *		bf,
			ParagraphLayoutPosition *	rowPlp )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    const int			atRowTop= 1;

    const RowProperties * const	rp= rowNode->biRowProperties;
    int				minHeightTwips= rp->rpHeightTwips;

    if  ( docParagraphLayoutPosClaimChildren( rowPlp, lj,
						rowNode->biChildCount ) )
	{ LDEB(rowNode->biChildCount); return -1;	}

    if  ( rp->rpHeightTwips < 0 )
	{ minHeightTwips= -rp->rpHeightTwips;	}

    /*  3a  */
    if  ( lpHere->lpPageYTwips+ minHeightTwips > bf->bfFlowRect.drY1	&&
	  ! lpHere->lpAtTopOfColumn					)
	{ *pToNextColumn= 1; return 0;	}

    if  ( precededByHeader )
	{
	const int	atTopOfRow= 1;

	docLayoutRowSkipHeaderHeight( lpHere, rowNode, atTopOfRow );

	rowNode->biTopPosition= *lpHere;
	}
    else{
	rowNode->biRowAboveHeaderPosition= rowNode->biTopPosition;
	rowNode->biRowPrecededByHeader= 0;
	}

    /*  1  */
    /*  NO! lpHere->lpPageYTwips += rowNode->biRowTopInset; */

    {
    int rti= rowNode->biRowTopInset;
    docLayoutCalculateRowTopInset( &rti, lc->lcDocument, rowNode, atRowTop );
    rowNode->biRowTopInset= rti;
    }

    if  ( docLayoutStartRowCells( rowNode, lj, lpHere, rowPlp ) )
	{ LDEB(1); return -1;	}

    *pToNextColumn= 0;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Continue a table row on the next page.				*/
/*									*/
/*  1)  Finish the footnotes that we found on the current page.		*/
/*  2)  Move to the next page. (Column in multi column sections)	*/
/*  3)  Skipping to a subsequent page.. Allocate space for the table	*/
/*	header.								*/
/*  4)  Reserve space for the top border of the cells.			*/
/*  5)  Initialize the layout of the different table columns.		*/
/*									*/
/************************************************************************/

static int docRowLayoutToNextColumn(
				struct BufferItem *	rowNode,
				LayoutPosition *	lpHere,
				const LayoutJob *	lj,
				int			atTopOfRow,
				BlockFrame *		bf )
    {
    const LayoutContext *	lc= &(lj->ljContext);

    int				inset= 0;
    const int			belowText= 0;
    const int			atRowTop= 0;
    LayoutPosition		lpBelowNotes;

    /*  1  */
    if  ( BF_HAS_FOOTNOTES( bf )				&&
	  ( rowNode->biTreeType == DOCinBODY		||
	    rowNode->biTreeType == DOCinENDNOTE	)		&&
	  docLayoutFootnotesForColumn( &lpBelowNotes, lpHere, bf,
							belowText, lj )	)
	{ LDEB(1); return -1;	}

    /*  2  */
    docLayoutToNextColumn( lpHere, bf, rowNode, lj );

    /*  3  */
    /*  Do not use rp->rpIsTableHeader as it gives trouble when header rows
     *  are not contiguous at the top of the table.  */
    if  ( rowNode->biNumberInParent >= rowNode->biRowPastHeaderRow	&&
	  rowNode->biRowPastHeaderRow > rowNode->biRowTableFirst	)
	{
	docLayoutRowSkipHeaderHeight( lpHere, rowNode, atTopOfRow );
	}

    /*  4  */
    docLayoutCalculateRowTopInset( &inset, lc->lcDocument, rowNode, atRowTop );
    lpHere->lpPageYTwips += inset;

    return 0;
    }

static int docRowToNextColumn(	LayoutPosition *	lpHere,
				struct BufferItem *	rowNode,
				const LayoutJob *	lj,
				int			atTopOfRow,
				BlockFrame *		bf,
				ParagraphLayoutJob *	rowPlj )
    {
    int				col;

    ParagraphLayoutJob *	plj;

    LayoutPosition		lpBefore;

    lpBefore= *lpHere;

    if  ( docRowLayoutToNextColumn( rowNode, lpHere, lj, atTopOfRow, bf ) )
	{ LDEB(atTopOfRow); return -1;	}

    /*  5  */
    plj= rowPlj->pljPos.pspChildren;
    for ( col= 0; col < rowNode->biChildCount; plj++, col++ )
	{
	const struct BufferItem *	cellNode= rowNode->biChildren[col];
	int				advanced;
	int				advanceAnyway= 0;

	if  ( docLayoutStripDone( &(plj->pljPos), plj ) )
	    { continue;	}

	if  ( cellNode->biChildren[plj->pljPos0.pspChild]->biLevel ==
								DOClevROW )
	    { continue;	}

	docCommitStripLayout( &advanced, advanceAnyway, plj,
				lpBefore.lpPage, lpBefore.lpColumn, cellNode );
	}

    return 0;
    }

/************************************************************************/
/* 									*/
/*  Calculate the layout of a nested table row.				*/
/* 									*/
/*  1)  If the row does not fit in the current page frame, the		*/
/*	the situation is hopeless: Proceed on this page: The row will	*/
/*	be split.							*/
/* 									*/
/************************************************************************/

int docLayoutRowInStrip(	int *				pStopCode,
				ParagraphLayoutPosition *	rowPlp,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				struct BufferItem *		rowNode )
    {
    int			stopCode= FORMATstopREADY;
    int			toNextColumn= 0;
    int			someAtHead= 0;
    const int		precededByHeader= 0;

    if  ( ! rowPlp->pspChildAdvanced )
	{
	rowNode->biTopPosition= rowPlp->plpPos;

	if  ( docLayoutStartRow( &toNextColumn, rowNode, precededByHeader, lj,
					&(rowPlp->plpPos), bf, rowPlp ) )
	    { LDEB(rowNode->biChildCount); return -1;	}

	/*  1  */
	if  ( toNextColumn )
	    { /*LDEB(toNextColumn);*/ toNextColumn= 0;	}
	}

    toNextColumn= 0;
    if  ( docLayoutRowPageStrip( &(rowPlp->plpPos),
					    &toNextColumn, &someAtHead,
					    rowNode, rowPlp, bf, lj ) )
	{ LDEB(1); return -1;	}

    if  ( toNextColumn )
	{
	stopCode= FORMATstopBLOCK_FULL;
	*pStopCode= FORMATstopBLOCK_FULL;
	return stopCode;
	}
    else{
	docStripLayoutNextChild( rowPlp );
	stopCode= FORMATstopREADY;
	*pStopCode= FORMATstopREADY;

	docLayoutFinishNodeLayout( (int *)0, rowNode, lj, &(rowPlp->plpPos) );

	return stopCode;
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a table row.				*/
/*									*/
/*  NOTE  Table headers that appear by themselves as the last table row	*/
/*	on the page should have been pushed to the next page. Some	*/
/*	thought reveals however that the result on the formatting of	*/
/*	the rest of the document is not altered by moving the header	*/
/*	row to the next page. For that reason, and to keep the formatter*/
/*	simple, we simply ignore the situation. To compensate, the	*/
/*	drawing code simply skips the row in its home position and the	*/
/*	visible result is the same.					*/
/*									*/
/*  1)  Sanity check against crashes.					*/
/*	This fix is a thourough memory leak. It is hardly ever		*/
/*	activated and it prevents crashes with incorrect rtf files.	*/
/*  2)  Allocate memory to monitor the progress of the layout of the	*/
/*	cells in the row.						*/
/*  3)  Initialize and place as much row content on the current page	*/
/*	as possible.							*/
/*  3a) As an optimization: It does not make sense to start a row that	*/
/*	has a minimum height on a page where it does not fit.		*/
/*	The test is not sufficient: Footnotes in the row can use the	*/
/*	space in such a way that on our way, we will discover that we	*/
/*	have no space left.						*/
/*  4)  Some of the content of the row did not fit on the current page.	*/
/*	Find out whether the row must be restarted on the next page.	*/
/*	(This does not make sense if the top of the row already is at	*/
/*	the top of the page.)						*/
/*  5)  When there are cells that remain empty on this page.. restart	*/
/*	on the next page.						*/
/*  6)  Go to the next page.						*/
/*  7)  If there is a reason to, reinitialize the row layout job. That	*/
/*	moves the whole row to the next page.				*/
/*  8)  Continue or restart (7) on the next page.			*/
/*  9)  As long as necessary, move to the next page and place content	*/
/*	there. (The exceptions above only apply for the first page, as	*/
/*	it is clearly impossible to keep the contents of a table higher	*/
/*	than one page on one page)					*/
/*									*/
/*  Measurements with MS-Word reveal the following:			*/
/*  if  trrh <  0, then the height of the row is -trrh.			*/
/*  if  trrh >= 0, then the height of the row is the maximum of the	*/
/*	height of the contents and the value of trrh. Contrary to the	*/
/*	exact height case, the width of the horizontal border is	*/
/*	added to the height of the row.					*/
/*  In the situation where two rows are on top of eachother, the	*/
/*  fattest border between them is used with a preference for the top	*/
/*  border of the bottom row if they have the same witdh.		*/
/*									*/
/************************************************************************/

int docLayoutRowNode(	int *				pStopCode,
			LayoutPosition *		lpBelow,
			const LayoutPosition *		lpTop,
			struct BufferItem *		rowNode,
			BlockFrame *			bf,
			int				inNewFrame,
			int				startInThisColumn,
			const LayoutJob *		lj )
    {
    int				stopCode= FORMATstopREADY;
    int				rval= 0;
    int				toNextColumn;
    int				someAtHead= 0;

    ParagraphLayoutJob		rowPlj;

    LayoutPosition		lpHere;

    const RowProperties *	rp= rowNode->biRowProperties;
    int				precededByHeader;

    /*  1  */
    if  ( ! docIsRowNode( rowNode ) )
	{ LLDEB(rowNode->biNumberInParent,rp->rpCellCount); return -1; }

    if  ( rp->rpCellCount < rowNode->biChildCount )
	{
	LLDEB(rp->rpCellCount,rowNode->biChildCount);
	docListNode( 0, rowNode, 0 );
	rowNode->biChildCount= rp->rpCellCount;
	}

    {
    const int		line= 0;
    const int		part= 0;

    docInitParagraphLayoutJob( &rowPlj );
    docBeginParagraphLayoutProgress( &rowPlj, lj,
				    rowNode->biNumberInParent,
				    line, part, rowNode->biNumberInParent+ 1,
				    lpTop );
    }

    lpHere= *lpTop;

    /*  2,3  */
    toNextColumn= 0;

    precededByHeader=
	    rowNode->biNumberInParent > rowNode->biRowPastHeaderRow	&&
	    rowNode->biRowPastHeaderRow > rowNode->biRowTableFirst	&&
	    inNewFrame;

    /*  3a  */
    if  ( docLayoutStartRow( &toNextColumn, rowNode, precededByHeader,
					lj, &lpHere, bf, &(rowPlj.pljPos) ) )
	{ LDEB(rowNode->biChildCount); rval= -1; goto ready;	}

    if  ( toNextColumn )
	{
	if  ( startInThisColumn )
	    { stopCode= FORMATstopFRAME_FULL; goto ready; }
	}
    else{
	if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  4  */
    if  ( toNextColumn					&&
	  ! rowNode->biTopPosition.lpAtTopOfColumn	)
	{
	int	isOnePage= RP_IS_ONE_PAGE( rp );
	int	noSpaceToStart= ! rowPlj.pljPos.pspChildAdvanced || someAtHead;

	/*  5, 7  */
	if  ( isOnePage || noSpaceToStart )
	    {
	    const int	atTopOfRow= 1;

	    if  ( startInThisColumn )
		{ stopCode= FORMATstopFRAME_FULL; goto ready; }

	    /*  6  */
	    if  ( docRowToNextColumn( &lpHere, rowNode, lj,
						atTopOfRow, bf, &rowPlj ) )
		{ LDEB(toNextColumn); rval= -1; goto ready;	}

	    rowNode->biTopPosition= lpHere;

	    /*  Do not use rp->rpIsTableHeader as it gives trouble when header 
	     *  rows are not contiguous at the top of the table.  */
	    if  ( rowNode->biNumberInParent < rowNode->biRowPastHeaderRow ||
		  rowNode->biRowPastHeaderRow <= rowNode->biRowTableFirst )
		{ rowNode->biRowAboveHeaderPosition= rowNode->biTopPosition; }

	    if  ( docLayoutStartRowCells( rowNode, lj,
					    &lpHere, &(rowPlj.pljPos) ) )
		{ LDEB(rowNode->biChildCount); rval= -1; goto ready;	}

	    toNextColumn= 0; /* irrelevant here */

	    /*  8  */
	    if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    /* Restart did not help to keep the row on one page */
	    if  ( toNextColumn && ! noSpaceToStart )
		{
		lpHere= *lpTop;
		toNextColumn= 0; /* irrelevant here */

		rowNode->biTopPosition= lpHere;

		if  ( docLayoutStartRow( &toNextColumn, rowNode,
					precededByHeader,
					lj, &lpHere, bf, &(rowPlj.pljPos) ) )
		    { LDEB(rowNode->biChildCount); rval= -1; goto ready; }

		if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn,
			    &someAtHead, rowNode, &(rowPlj.pljPos), bf, lj ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
	    }
	}

    /*  9  */
    while( toNextColumn )
	{
	const int	atTopOfRow= 0;

	if  ( docRowToNextColumn( &lpHere, rowNode, lj,
						atTopOfRow, bf, &rowPlj ) )
	    { LDEB(toNextColumn); rval= -1; goto ready;	}

	if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanParagraphLayoutJob( &rowPlj );

    if  ( ! rval )
	{
	*pStopCode= stopCode;
	*lpBelow= lpHere;
	}

    return rval;
    }

