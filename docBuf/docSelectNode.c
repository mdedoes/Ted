/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docSelect.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Implementation of a 'Select All' menu option.			*/
/*  Extend selection to the whole document tree.			*/
/*									*/
/************************************************************************/

int docSelectWholeBody(		DocumentSelection *	ds,
				struct BufferDocument *	bd )
    {
    struct BufferItem *		selRootNode= (struct BufferItem *)0;

    if  ( docSelectionIsSet( ds ) )
	{
	selRootNode= docGetSelectionRoot( (struct DocumentTree **)0,
					(struct BufferItem **)0, bd, ds );
	}

    if  ( ! selRootNode )
	{ selRootNode= bd->bdBody.dtRoot;	}

    while( selRootNode && selRootNode->biParent )
	{ selRootNode= selRootNode->biParent; }
    if  ( ! selRootNode )
	{ XDEB(selRootNode); return -1;	}

    if  ( docHeadPosition( &(ds->dsHead), selRootNode ) )
	{ LDEB(1); return -1;	}
    ds->dsHead.dpStroff= 0;
    if  ( docTailPosition( &(ds->dsTail), selRootNode ) )
	{ LDEB(1); return -1;	}
    ds->dsTail.dpStroff= docParaStrlen( ds->dsTail.dpNode );

    ds->dsAnchor= ds->dsHead;
    ds->dsDirection= 1;

    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeSection(	DocumentSelection *	ds,
				int			direction )
    {
    struct BufferItem *		sectNode0= docGetSectNode( ds->dsHead.dpNode );
    struct BufferItem *		sectNode1= docGetSectNode( ds->dsTail.dpNode );

    if  ( ! sectNode0 || ! sectNode1 )
	{ XXDEB(sectNode0,sectNode1); return -1;	}

    if  ( sectNode0->biParent != sectNode1->biParent )
	{ XXDEB(sectNode0->biParent,sectNode1->biParent); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), sectNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}

	sectNode0= sectNode1= docGetSectNode( ds->dsTail.dpNode );
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), sectNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}

	sectNode0= sectNode1= docGetSectNode( ds->dsHead.dpNode );
	}

    if  ( docHeadPosition( &(ds->dsHead), sectNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), sectNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole paragraphs.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeParagraph(	DocumentSelection *	ds,
				int			direction )
    {
    struct BufferItem *		paraNode0= ds->dsHead.dpNode;
    struct BufferItem *		paraNode1= ds->dsTail.dpNode;

    if  ( paraNode0->biParent != paraNode1->biParent )
	{ XXDEB(paraNode0->biParent,paraNode1->biParent); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), paraNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}

	paraNode0= paraNode1= ds->dsTail.dpNode;
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), paraNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}

	paraNode0= paraNode1= ds->dsHead.dpNode;
	}

    if  ( docHeadPosition( &(ds->dsHead), paraNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), paraNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole rows.				*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectRow(		DocumentSelection *	ds,
				int			direction,
				int			allColumns )
    {
    struct BufferItem *		cellNode0= docGetCellNode( ds->dsHead.dpNode );
    struct BufferItem *		cellNode1= docGetCellNode( ds->dsTail.dpNode );
    struct BufferItem *		rowNode0= docGetRowNode( cellNode0 );
    struct BufferItem *		rowNode1= docGetRowNode( cellNode1 );

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    if  ( direction != 0 )
	{
	int		row0, row1;

	if  ( docDelimitTable( ds->dsHead.dpNode, (struct BufferItem **)0,
					(int *)0, &row0, (int *)0, &row1 ) )
	    { LDEB(1); return -1;	}

	if  ( direction > 0 )
	    {
	    if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
		{ LDEB(1); return -1;	}
	    if  ( docNextPosition( &(ds->dsTail) ) )
		{ return 1;	}
	    rowNode1= docGetRowNode( ds->dsTail.dpNode );
	    if  ( ! rowNode1 || rowNode1->biNumberInParent > row1 )
		{ return 1;	}

	    if  ( docTailPosition( &(ds->dsHead), rowNode0 ) )
		{ LDEB(1); return -1;	}
	    if  ( docNextPosition( &(ds->dsHead) ) )
		{ return 1;	}
	    rowNode0= docGetRowNode( ds->dsHead.dpNode );
	    if  ( ! rowNode0 || rowNode0->biNumberInParent > row1 )
		{ return 1;	}
	    }

	if  ( direction < 0 )
	    {
	    if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
		{ LDEB(1); return -1;	}
	    if  ( docPrevPosition( &(ds->dsHead) ) )
		{ return 1;	}
	    rowNode0= docGetRowNode( ds->dsHead.dpNode );
	    if  ( ! rowNode0 || rowNode0->biNumberInParent < row0 )
		{ return 1;	}

	    if  ( docHeadPosition( &(ds->dsTail), rowNode1 ) )
		{ LDEB(1); return -1;	}
	    if  ( docPrevPosition( &(ds->dsTail) ) )
		{ return 1;	}
	    rowNode1= docGetRowNode( ds->dsTail.dpNode );
	    if  ( ! rowNode1 || rowNode1->biNumberInParent < row0 )
		{ return 1;	}
	    }
	}

    if  ( allColumns )
	{
	if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( cellNode0->biNumberInParent >= rowNode0->biChildCount ||
	      cellNode0->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	cellNode0= rowNode0->biChildren[cellNode0->biNumberInParent];

	if  ( cellNode1->biNumberInParent >= rowNode0->biChildCount ||
	      cellNode1->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	cellNode1= rowNode1->biChildren[cellNode1->biNumberInParent];

	if  ( docHeadPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docTailPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole cells/columns.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeCell(		DocumentSelection *	ds,
				int			direction,
				int			allRows )
    {
    struct BufferItem *	cellNode0= docGetCellNode( ds->dsHead.dpNode );
    struct BufferItem *	cellNode1= docGetCellNode( ds->dsTail.dpNode );
    struct BufferItem *	rowNode0;
    struct BufferItem *	rowNode1;
    struct BufferItem *	parentNode;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( ! cellNode0 || ! cellNode1 )
	{ XXDEB(cellNode0,cellNode1); return -1;	}

    rowNode0= docGetRowNode( cellNode0 );
    rowNode1= docGetRowNode( cellNode1 );

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    parentNode= rowNode0->biParent;

    if  ( allRows )
	{
	int		col;
	int		row, row0, row1;

	if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode,
						&col, &row0, &row, &row1 ) )
	    { LDEB(1); return -1;	}

	rowNode0= parentNode->biChildren[row0];
	rowNode1= parentNode->biChildren[row1];

	if  ( cellNode0->biNumberInParent >= rowNode0->biChildCount )
	    {
	    LLDEB(cellNode0->biNumberInParent,rowNode0->biChildCount);
	    return -1;
	    }
	cellNode0= rowNode0->biChildren[cellNode0->biNumberInParent];

	if  ( cellNode1->biNumberInParent >= rowNode1->biChildCount )
	    {
	    LLDEB(cellNode1->biNumberInParent,rowNode1->biChildCount);
	    return -1;
	    }
	cellNode1= rowNode1->biChildren[cellNode1->biNumberInParent];
	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}
	cellNode1= docGetCellNode( ds->dsTail.dpNode );
	if  ( ! cellNode1 || cellNode1->biParent != rowNode1 )
	    { return 1;	}
	if  ( cellNode1->biNumberInParent >= rowNode0->biChildCount )
	    { return 1;	}

	if  ( docTailPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsHead) ) )
	    { return 1;	}
	cellNode0= docGetCellNode( ds->dsHead.dpNode );
	if  ( ! cellNode0 || cellNode0->biParent != rowNode0 )
	    { return 1;	}
	if  ( cellNode0->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}
	cellNode0= docGetCellNode( ds->dsHead.dpNode );
	if  ( ! cellNode0 || cellNode0->biParent != rowNode0 )
	    { return 1;	}
	/*  No need to check against a wider row */

	if  ( docHeadPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsTail) ) )
	    { return 1;	}
	cellNode1= docGetCellNode( ds->dsTail.dpNode );
	if  ( ! cellNode1 || cellNode1->biParent != rowNode1 )
	    { return 1;	}
	/*  No need to check against a wider row */
	}

    if  ( docHeadPosition( &dpHead, cellNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail, cellNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( cellNode0 == cellNode1 )
	{
	int	rowspan= 1;
	int	colspan= 1;

	docTableDetermineCellspans( &rowspan, &colspan, cellNode0 );

	if  ( rowspan > 1 || colspan > 1 )
	    {
	    rowNode1= parentNode->biChildren[
				rowNode0->biNumberInParent+ rowspan- 1];
	    cellNode1= rowNode1->biChildren[
				cellNode0->biNumberInParent+ colspan- 1];

	    if  ( docTailPosition( &dpTail, cellNode1 ) )
		{ LDEB(1); return -1;	}
	    }
	}

    docSetRangeSelection( ds, &dpHead, &dpTail, direction );

    return 0;
    }

/************************************************************************/

int docSelectColumn(	DocumentSelection *	ds,
			int			col )
    {
    const int		direction= 0;
    const int		allRows= 1;

    struct BufferItem *	paraNode= ds->dsHead.dpNode;
    struct BufferItem *	rowNode= docGetRowNode( paraNode );

    if  ( ! rowNode )
	{ XDEB(rowNode); return -1;	}
    if  ( col < 0 || col >= rowNode->biChildCount )
	{ LLDEB(col,rowNode->biChildCount); return -1;	}

    if  ( docHeadPosition( &(ds->dsHead), rowNode->biChildren[col] ) )
	{ LDEB(col); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), rowNode->biChildren[col] ) )
	{ LDEB(col); return -1;	}

    if  ( docSelectWholeCell( ds, direction, allRows ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover the whole table.			*/
/*									*/
/************************************************************************/

int docSelectWholeTable(	DocumentSelection *	ds )
    {
    struct BufferItem *		rowNode0= docGetRowNode( ds->dsHead.dpNode );
    struct BufferItem *		rowNode1= docGetRowNode( ds->dsTail.dpNode );

    struct BufferItem *		parentNode;
    int				row0;
    int				row1;

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode, (int *)0,
						&row0, (int *)0, &row1 ) )
	{ LDEB(1); return -1;	}

    rowNode0= parentNode->biChildren[row0];
    rowNode1= parentNode->biChildren[row1];

    if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
	{ LDEB(1); return -1;	}

    ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;
    ds->dsCol0= ds->dsCol1= -1;

    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Select a table cell by row/column.					*/
/*									*/
/************************************************************************/

int docSelectTableCell(		DocumentSelection *	dsCell,
				struct BufferItem *	childNode,
				int			col,
				int			row )
    {
    struct BufferItem *	rowNode;

    rowNode= docGetRowNode( childNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); return -1;	}

    row += rowNode->biRowTableFirst;
    if  ( row < rowNode->biRowTableFirst	||
	  row >= rowNode->biRowTablePast	)
	{
	LLLDEB(rowNode->biRowTableFirst,row,rowNode->biRowTablePast);
	return -1;
	}
    rowNode= rowNode->biParent->biChildren[row];

    if  ( col < 0 || col >= rowNode->biChildCount )
	{ LLDEB(col,rowNode->biChildCount); return -1;	}

    if  ( docSetNodeSelection( dsCell, rowNode->biChildren[col] ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docSelectTableLeft(		DocumentSelection *		dsSlice,
				struct BufferItem *		childNode )
    {
    int			col;
    int			row0;
    int			row;
    int			row1;

    struct BufferItem *	parentNode;
    struct BufferItem *	rowNode;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( docDelimitTable( childNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    if  ( col == 0 )
	{ return 1;	}

    rowNode= parentNode->biChildren[row];

    if  ( docHeadPosition( &dpHead, rowNode->biChildren[0] ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail, rowNode->biChildren[col- 1] ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( dsSlice, &dpHead, &dpTail, 1 );

    return 0;
    }

int docSelectTableRight(	DocumentSelection *		dsSlice,
				struct BufferItem *		childNode )
    {
    int			col;
    int			row0;
    int			row;
    int			row1;

    struct BufferItem *	parentNode;
    struct BufferItem *	rowNode;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( docDelimitTable( childNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    rowNode= parentNode->biChildren[row];

    if  ( col == rowNode->biChildCount- 1 )
	{ return 1;	}

    if  ( docHeadPosition( &dpHead, rowNode->biChildren[col+ 1] ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail,
			    rowNode->biChildren[rowNode->biChildCount- 1] ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( dsSlice, &dpHead, &dpTail, 1 );

    return 0;
    }

static int docSelectTableOther(	DocumentSelection *		dsSlice,
				BufferItem *			cellNode,
				BufferItem *			headRowNode,
				BufferItem *			tailRowNode )
    {
    int			hcol;
    int			tcol;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    hcol= docGetMatchingCell( headRowNode, cellNode );
    tcol= docGetMatchingCell( tailRowNode, cellNode );

    if  ( hcol < 0 || tcol < 0 )
	{ LLDEB(hcol,tcol); return -1;	}

    if  ( docHeadPosition( &dpHead, headRowNode->biChildren[hcol] ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail, tailRowNode->biChildren[tcol] ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( dsSlice, &dpHead, &dpTail, 1 );

    return 0;
    }

int docSelectTableAbove(	DocumentSelection *		dsSlice,
				struct BufferItem *		childNode )
    {
    int			col;
    int			row0;
    int			row;
    int			row1;

    struct BufferItem *	parentNode;
    struct BufferItem *	rowNode;
    struct BufferItem *	cellNode;

    if  ( docDelimitTable( childNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    if  ( row <= row0 )
	{ return 1;	}

    rowNode= parentNode->biChildren[row];
    cellNode= rowNode->biChildren[col];

    if  ( docSelectTableOther( dsSlice, cellNode,
				    parentNode->biChildren[row0],
				    parentNode->biChildren[row- 1] ) )
	{ LLLDEB(col,row0,row- 1); return -1;	}

    return 0;
    }

int docSelectTableBelow(	DocumentSelection *		dsSlice,
				struct BufferItem *		childNode )
    {
    int			col;
    int			row0;
    int			row;
    int			row1;

    struct BufferItem *	parentNode;
    struct BufferItem *	rowNode;
    struct BufferItem *	cellNode;

    if  ( docDelimitTable( childNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    if  ( row >= row1 )
	{ return 1;	}

    rowNode= parentNode->biChildren[row];
    cellNode= rowNode->biChildren[col];

    if  ( docSelectTableOther( dsSlice, cellNode,
				    parentNode->biChildren[row0+ 1],
				    parentNode->biChildren[row1] ) )
	{ LLLDEB(col,row0+ 1,row1); return -1;	}

    return 0;
    }

int docSelectTableRect(	DocumentSelection *		dsRect,
			struct BufferItem *		childNode,
			int				col0Set,
			int				col1Set,
			int				row0Set,
			int				row1Set )
    {
    int			col;
    int			row0;
    int			row;
    int			row1;

    struct BufferItem *	parentNode;
    struct BufferItem *	rowNode;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( docDelimitTable( childNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    rowNode= parentNode->biChildren[row];

    if  ( row0Set == -1 && row1Set == -1 )
	{ row0Set= row; row1Set= row;	}
    else{
	row0Set += rowNode->biRowTableFirst;
	row1Set += rowNode->biRowTableFirst;
	}

    if  ( row0Set > row1Set			||
	  row0Set < rowNode->biRowTableFirst	||
	  row1Set >= rowNode->biRowTablePast	)
	{
	LLDEB(row0Set,rowNode->biRowTableFirst);
	LLDEB(row1Set,rowNode->biRowTablePast);
	return -1;
	}

    if  ( col0Set == -1 && col1Set == -1 )
	{
	col0Set= 0;
	col1Set= rowNode->biChildCount- 1;
	}

    if  ( col1Set < col0Set						||
	  col0Set < 0							||
	  col1Set < 0							||
	  col1Set >= parentNode->biChildren[row0Set]->biChildCount	||
	  col1Set >= parentNode->biChildren[row1Set]->biChildCount	)
	{
	LLLDEB(col0Set,row0Set,parentNode->biChildren[row0Set]->biChildCount);
	LLLDEB(col1Set,row1Set,parentNode->biChildren[row1Set]->biChildCount);
	return -1;
	}

    if  ( docHeadPosition( &dpHead,
			parentNode->biChildren[row0Set]->biChildren[col0Set] ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail,
			parentNode->biChildren[row1Set]->biChildren[col1Set] ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( dsRect, &dpHead, &dpTail, 1 );

    return 0;
    }
