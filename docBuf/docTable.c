/************************************************************************/
/*									*/
/*  Delimit tables and table rectangles.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docTableRectangle.h"
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	"docSelect.h"
#   include	<docDocumentAttributeMap.h>
#   include	<docParaProperties.h>
#   include	<docEditRange.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the range of rows for a 'table'				*/
/*									*/
/************************************************************************/

int docDelimitTable(	const struct BufferItem *	paraNode,
			struct BufferItem **		pParentNode,
			int *				pCol,
			int *				pRow0,
			int *				pRow,
			int *				pRow1 )
    {
    struct BufferItem *	rowNode;
    struct BufferItem *	parentNode;
    int			col;

    if  ( paraNode->biLevel != DOClevPARA			||
	  paraNode->biParaProperties->ppTableNesting == 0	)
	{ /* LLDEB(paraNode->biLevel,paraNode->biParaInTable); */ return -1; }

    rowNode= docGetRowNode( (struct BufferItem *)paraNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); return -1;	}
    parentNode= rowNode->biParent;
    col= paraNode->biParent->biNumberInParent;

    if  ( pParentNode )
	{ *pParentNode= parentNode;		}
    if  ( pCol )
	{ *pCol= col;				}
    if  ( pRow0 )
	{ *pRow0= rowNode->biRowTableFirst;	}
    if  ( pRow )
	{ *pRow= rowNode->biNumberInParent;	}
    if  ( pRow1 )
	{ *pRow1= rowNode->biRowTablePast- 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the spans of a table cell.				*/
/*									*/
/************************************************************************/

int docGetCellBottom(		LayoutPosition *		lpBottom,
				const struct BufferItem *	cellNode )
    {
    const struct BufferItem *	rowNode= cellNode->biParent;
    const struct BufferItem *	parentNode= rowNode->biParent;

    int				row0= rowNode->biNumberInParent;
    int				rowp= rowNode->biRowTablePast;
    const CellProperties *	cp= cellNode->biCellProperties;

    const LayoutPosition *	lpBelow= &(rowNode->biBelowPosition);

    int				rowspan= 1;

    if  ( ! cp )
	{ XDEB(cp); return 1; }

    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	{ rowspan= 0;	}
    else{
	if  ( cp->cpVerticalMerge == CELLmergeHEAD )
	    {
	    int		nRow;

	    for ( nRow= row0+ 1; nRow < rowp; nRow++ )
		{
		const struct BufferItem *	nRowNode= parentNode->biChildren[nRow];
		const RowProperties *	nRowRp= nRowNode->biRowProperties;
		int			nCol;

		nCol= docGetMatchingCell( nRowNode, cellNode );
		if  ( nCol < 0 )
		    { break;	}

		if  ( nRowRp->rpCells[nCol].cpVerticalMerge != CELLmergeFOLLOW )
		    { break;	}

		lpBelow= &(nRowNode->biBelowPosition);
		rowspan++;
		}
	    }
	}

    if  ( lpBottom )
	{ *lpBottom= *lpBelow;	}

    return rowspan;
    }

/************************************************************************/
/*									*/
/*  Determine the spans of a table cell.				*/
/*									*/
/************************************************************************/

void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const struct BufferItem *	cellNode )
    {
    int				colspan= cellNode->biCellColspan;
    int				rowspan= 1;

    rowspan= docGetCellBottom( (LayoutPosition *)0, cellNode );

    if  ( pRowspan )
	{ *pRowspan= rowspan;	}
    if  ( pColspan )
	{ *pColspan= colspan;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangular area as it is selected in a table.	*/
/*									*/
/*  This funtion fails if the whole selection is not inside ONE table.	*/
/*									*/
/*  1)  Find the table environment of the beginning of the selection.	*/
/*  2)  Find the table environment of the end of the selection.		*/
/*  3)  Different section -> Different table.				*/
/*  4)  Different begin or different end -> Different table.		*/
/*  5)  Paranoia checks.						*/
/*  6)  Delimit rectangle.						*/
/*  7)  If the selection is inside one cell determine its		*/
/*	colspan/rowspan.						*/
/*									*/
/************************************************************************/

int docGetTableRectangle(	TableRectangle *		tr,
				const DocumentSelection *	ds )
    {
    struct BufferItem *	parentNode0;
    struct BufferItem *	parentNode1;

    TableRectangle	trHead;
    TableRectangle	trTail;

    int			wholeSingleCell= 0;

    /*  1  */
    if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode0,
						&(trHead.trCol0),
						&(trHead.trRow00),
						&(trHead.trRow0),
						&(trHead.trRow11) ) )
	{ /* LDEB(1); */ return 1;	}

    /*  2  */
    if  ( docDelimitTable( ds->dsTail.dpNode, &parentNode1,
						&(trTail.trCol0),
						&(trTail.trRow00),
						&(trTail.trRow0),
						&(trTail.trRow11) ) )
	{ /* LDEB(1); */ return 1;	}

    /*  3  */
    if  ( parentNode0 != parentNode1 )
	{ /* XXDEB(parentNode0,parentNode1); */ return 1;	}

    /*  4  */
    if  ( trHead.trRow00 != trTail.trRow00	||
	  trHead.trRow11 != trTail.trRow11	)
	{ return 1;	}

    /*  5  */
    if  ( trHead.trCol0 > trTail.trCol0 )
	{ return 1;	}
    if  ( trHead.trRow0 > trTail.trRow0 )
	{ return 1;	}

    /*  6  */
    tr->trCol0= trHead.trCol0;
    tr->trCol1= trTail.trCol0;
    tr->trCol11= parentNode0->biChildren[trHead.trRow0]->biChildCount- 1;

    tr->trRow00= trHead.trRow00;
    tr->trRow0= trHead.trRow0;
    tr->trRow1= trTail.trRow0;
    tr->trRow11= trHead.trRow11;
    tr->trPastHeaderRow= parentNode0->biChildren[trHead.trRow0]->biRowPastHeaderRow;

    tr->trCellColspan= 0;
    tr->trCellRowspan= 0;

    tr->trIsSingleCell= (	tr->trRow0 == tr->trRow1	&&
				tr->trCol0 == tr->trCol1	);

    /*  7  */
    if  ( tr->trIsSingleCell )
	{
	BufferItem *	rowNode0= parentNode0->biChildren[trHead.trRow0];
	BufferItem *	cellNode0= rowNode0->biChildren[tr->trCol0];

	DocumentSelection	dsCell;

	docSetNodeSelection( &dsCell, cellNode0 );
	wholeSingleCell= docSameSelection( &dsCell, ds );

	docTableDetermineCellspans(
			&(tr->trCellRowspan), &(tr->trCellColspan), cellNode0 );
	}

    tr->trIsColumnSlice= 0;
    if  ( tr->trRow0 == tr->trRow00 && tr->trRow1 == tr->trRow11 )
	{
	tr->trIsColumnSlice= ! tr->trIsSingleCell || wholeSingleCell;
	}

    tr->trIsRowSlice= 0;
    if  ( tr->trCol0 == 0 && tr->trCol1 == tr->trCol11 )
	{
	tr->trIsRowSlice= ! tr->trIsSingleCell || wholeSingleCell;
	}

    tr->trIsTableSlice= tr->trIsColumnSlice || tr->trIsRowSlice;
    tr->trIsWholeTable= tr->trIsColumnSlice && tr->trIsRowSlice;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit the tables in a section.					*/
/*									*/
/************************************************************************/

void docDelimitTables(	struct BufferItem *	parentNode,
			int			recursively )
    {
    int				n;

    if  ( ! parentNode )
	{ XDEB(parentNode); return;	}

    n= 0;
    while( n < parentNode->biChildCount )
	{
	struct BufferItem *	firstNode;

	int			first;
	int			past;

	int			pastHeaderRow= -1;

	firstNode= parentNode->biChildren[n];
	if  ( ! docIsRowNode( firstNode ) )
	    {
	    if  ( recursively && firstNode->biLevel != DOClevPARA )
		{ docDelimitTables( firstNode, recursively );	}

	    if  ( firstNode->biLevel == DOClevROW )
		{
		firstNode->biRowTableFirst= -1;
		firstNode->biRowTablePast= -1;
		firstNode->biRowPastHeaderRow= -1;
		}

	    n++; continue;
	    }

	if  ( n >= parentNode->biChildCount )
	    { break;	}

	first= n;
	past= n;
	pastHeaderRow= n;

	while( past < parentNode->biChildCount )
	    {
	    struct BufferItem *		rowNode= parentNode->biChildren[past];
	    const RowProperties *	rp;

	    if  ( ! docIsRowNode( rowNode ) )
		{ break;	}

	    rp= rowNode->biRowProperties;
	    if  ( rp->rpIsTableHeader )
		{ pastHeaderRow= past+ 1;	}

	    past++; 
	    }

	if  ( past == n )
	    { LLDEB(past,n);	}

	while( n < past )
	    {
	    struct BufferItem *		rowNode= parentNode->biChildren[n];

	    if  ( recursively )
		{ docDelimitTables( rowNode, recursively );	}

	    rowNode->biRowTableFirst= first;
	    rowNode->biRowTablePast= past;
	    rowNode->biRowPastHeaderRow= pastHeaderRow;

	    n++;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the cell with left and right coordinates as given.		*/
/*									*/
/************************************************************************/

int docGetMatchingCell(		const struct BufferItem *	rowNode,
				const struct BufferItem *	refCellNode )
    {
    int				col;

    if  ( rowNode->biLevel != DOClevROW )
	{ SDEB(docLevelStr(rowNode->biLevel)); return -1;	}

    /*  2  */
    for ( col= 0; col < rowNode->biChildCount; col++ )
	{
	struct BufferItem *	cellNode= rowNode->biChildren[col];
	const CellProperties *	cp= cellNode->biCellProperties;

	if  ( ! cp )
	    { LXDEB(col,cp); continue; }

	if  ( cp->cpHorizontalMerge == CELLmergeFOLLOW )
	    { continue;	}

	if  ( cellNode->biCellHeadX == refCellNode->biCellHeadX	&&
	      cellNode->biCellTailX == refCellNode->biCellTailX )
	    { return col;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Limit a selection that ends in a merged cell to the cell that it	*/
/*  started in.								*/
/*									*/
/************************************************************************/

void docAvoidMergedCellTail(	DocumentSelection *	dsNew,
				EditRange *		er )
    {
    struct BufferItem *	cellNode= docGetCellNode( dsNew->dsTail.dpNode );

    if  ( cellNode )
	{
	const CellProperties *	cp= cellNode->biCellProperties;

	if  ( cp && CELL_MERGED( cp ) )
	    {
	    cellNode= docGetCellNode( dsNew->dsHead.dpNode );

	    if  ( docSetNodeSelection( dsNew, cellNode ) )
		{ LDEB(1);				}
	    else{ docSetEditRange( er, dsNew );	}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save cell properties that are representative for the cells in a	*/
/*  table rectangle. Typically, that is the properties of the		*/
/*  upper-left cell. For a bigger rectangle, the borders are adapted	*/
/*  to those on the outside of the rectangle.				*/
/*									*/
/************************************************************************/

void docGetCellRectangleProperties(	CellProperties *		cpTo,
					const struct BufferItem *	rowNode,
					const TableRectangle *		tr,
					const DocumentAttributeMap *	dam )
    {
    const RowProperties *	rp= rowNode->biRowProperties;

    if  ( docCopyCellProperties( cpTo, &(rp->rpCells[tr->trCol0]), dam ) )
	{ LDEB(1); return;	}

    if  ( tr->trCol1 > tr->trCol0 )
	{
	int	fromNumber= rp->rpCells[tr->trCol1].cpRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	cpTo->cpRightBorderNumber= fromNumber;
	}

    if  ( tr->trRow1 > tr->trRow0 )
	{
	const struct BufferItem *	rowNode1;
	const RowProperties *	rp1;
	int 			fromNumber;

	rowNode1= rowNode->biParent->biChildren[tr->trRow1];
	rp1= rowNode1->biRowProperties;

	fromNumber= rp1->rpCells[tr->trCol0].cpBottomBorderNumber;
	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	cpTo->cpBottomBorderNumber= fromNumber;
	}

    return;
    }

