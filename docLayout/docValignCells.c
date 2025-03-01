/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	<docTreeNode.h>
#   include	"docRowLayout.h"
#   include	"docStripLayoutJob.h"
#   include	<docPropVal.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Shift cell contents of the cells in a row that are not top-aligned.	*/
/*									*/
/*  Experimentation with MS-Word shows the following behavior:		*/
/*									*/
/*  TOP    aligned cells behave as expected.				*/
/*  CENTER aligned cells with content that fits on the first page,	*/
/*	   vertically center the contents on the first page. If the	*/
/*	   content is higher than the space on the first page, the cell	*/
/*	   behaves like a top-aligned cell.				*/
/*  BOTTOM aligned cells with content that fits on the first page,	*/
/*	   bottom align the contents on the first page. If the content	*/
/*	   is higher than the space on the first page, the cell behaves	*/
/*	   like a top-aligned cell.					*/
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

if  ( cellNode->biChildCount > 0 )
{
    cellNode->biChildren[0]->biTopPosition.lpPageYTwips += cellNode->biCellTopInset;
}
	    docRedoParaStripLayout( lj, bf, &lpCellTop,
					    cellNode, paraFrom, paraUpto );
	    return;

	case DOCtvaBOTTOM:
	    cellNode->biCellTopInset= rowHigh- cellHigh;
	    lpCellTop.lpPageYTwips += cellNode->biCellTopInset;

if  ( cellNode->biChildCount > 0 )
{
    cellNode->biChildren[0]->biTopPosition.lpPageYTwips += cellNode->biCellTopInset;
}
	    docRedoParaStripLayout( lj, bf, &lpCellTop,
					    cellNode, paraFrom, paraUpto );
	    return;

	default:
	    LDEB(thisCp->cpValign);
	    return;
	}
    }

int docLayoutRowValignCells(	BufferItem *			rowNode,
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
	BufferItem *		cellNode= rowNode->biChildren[col];

	if  ( cellNode->biCellRowspan > 1 )
	    { continue;	}
	if  ( ! docLayoutStripDone( &(plj->pljPos), plj) )
	    { continue;	}


	if  ( docCompareLayoutPositionFrames( &(rowNode->biTopPosition),
							    lpBelow ) != 0 )
	    { continue;	}

	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	    {
	    if  ( cellNode->biCellMergedCellTopRow < 0 )
		{ continue;	}
	    else{
		BufferItem *	topRowNode;

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

