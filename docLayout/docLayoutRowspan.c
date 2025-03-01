/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*  Setup rowspan related administration.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docStripLayoutJob.h"
#   include	<docTreeNode.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the top of group of vertically merged cells.			*/
/*									*/
/*  1)  Look at the table rows above the cell in reverse order.		*/
/*  2)  Find a matching cell.						*/
/*									*/
/************************************************************************/

static void docLayoutTopOfRowspan(
				ParagraphLayoutJob *		plj,
				struct BufferItem * const	cellNode,
				const struct BufferItem *	rowNode )
    {
    int		topRow;
    int		row= rowNode->biNumberInParent;
    int		col= cellNode->biNumberInParent;

    /*  1  */
    for ( topRow= rowNode->biNumberInParent- 1;
	  topRow >= rowNode->biRowTableFirst;
	  topRow-- )
	{
	int			topCol;
	const struct BufferItem *	topRowNode;
	const RowProperties *	topRp;
	const CellProperties *	topCp;

	/*  2  */
	topRowNode= rowNode->biParent->biChildren[topRow];
	topRp= topRowNode->biRowProperties;

	topCol= docGetMatchingCell( topRowNode, cellNode );
	if  ( topCol < 0 )
	    { LDEB(topCol); continue;	}

	topCp= topRp->rpCells+ topCol;
	switch( topCp->cpVerticalMerge )
	    {
	    case CELLmergeFOLLOW:
		continue;

	    case CELLmergeHEAD:
		{
		const struct BufferItem *	topCellNode= topRowNode->biChildren[topCol];

		cellNode->biCellMergedCellTopRow= topRow;
		cellNode->biCellMergedCellTopCol= topCol;
		plj->cljMergedCellBelowPosion= topCellNode->biBelowPosition;
		}
		break;

	    default:
		LDEB(topCp->cpVerticalMerge);
		break;
	    }

	break;
	}

    if  ( cellNode->biCellMergedCellTopRow < 0 )
	{ LLLDEB(row,col,cellNode->biCellMergedCellTopRow);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a row in the document.			*/
/*									*/
/*  3)  Columns with a colspan do not push the bottom of THIS row down.	*/
/*	Remember this.							*/
/*  4)  But in a cell with a rowspan, the top cell of the cell might	*/
/*	push the bottom down. Look for it.				*/
/*									*/
/************************************************************************/

void docRowLayoutRowspanAdmin(	struct BufferItem *		cellNode,
				const struct BufferItem *	rowNode,
				const struct BufferItem *	nextRowNode,
				int				nextRow,
				ParagraphLayoutJob *		plj )

    {
    const CellProperties * const	cp= cellNode->biCellProperties;

    if  ( nextRow < rowNode->biRowTablePast )
	{
	int			nextCol;

	nextCol= docGetMatchingCell( nextRowNode, cellNode );
	if  ( nextCol >= 0 )
	    {
	    const RowProperties *	nextRp;
	    const CellProperties *	nextCp;

	    nextRp= nextRowNode->biRowProperties;
	    nextCp= &(nextRp->rpCells[nextCol]);

	    /*  3  */
	    if  ( cp->cpVerticalMerge == CELLmergeHEAD		&&
		  nextCp->cpVerticalMerge == CELLmergeFOLLOW	)
		{
		cellNode->biCellRowspan= docGetCellBottom(
					    (LayoutPosition *)0, cellNode );
		}

	    /*  4  */
	    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW	&&
		  nextCp->cpVerticalMerge != CELLmergeFOLLOW	)
		{ docLayoutTopOfRowspan( plj, cellNode, rowNode ); }
	    }
	else{
	    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
		{ docLayoutTopOfRowspan( plj, cellNode, rowNode ); }
	    }
	}
    else{
	/*  4  */
	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	    { docLayoutTopOfRowspan( plj, cellNode, rowNode );	}
	}
    }

