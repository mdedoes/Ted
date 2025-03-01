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
#   include	<docCellProperties.h>
#   include	<docBlockFrame.h>
#   include	<docNodeTree.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Finish cell layout.							*/
/*									*/
/************************************************************************/

void docLayoutFinishCell(	LayoutPosition *	lpHere,
				LayoutPosition *	lpBelowAllCells,
				BufferItem *		cellNode,
				const CellProperties *	cp,
				ParagraphLayoutJob *	plj )
    {
    ParagraphLayoutPosition *	plp= &(plj->pljPos);

    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW	&&
	  cellNode->biCellMergedCellTopRow >= 0		)
	{
	docLayoutPushBottomDown( lpBelowAllCells,
				    &(plj->cljMergedCellBelowPosion) );
	docLayoutPushBottomDown( lpHere,
				    &(plj->cljMergedCellBelowPosion) );
	return;
	}

    if  ( CELL_MERGED( cp ) )
	{ return;	}

    cellNode->biBelowPosition= plp->plpPos;

    docLayoutPushBottomDown( lpBelowAllCells, &(plp->plpPos) );

    if  ( cellNode->biCellRowspan <= 1 )
	{ docLayoutPushBottomDown( lpHere, &(plp->plpPos) ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a table cell as fits on the current page.		*/
/*									*/
/*  3)  Cells in a row span or a colspan play no role...		*/
/*  4)  This column was already exhausted on a previous page..		*/
/*  5)  Format paragraphs inside this cell.				*/
/*  6)  Remember how far we have descended on the page.			*/
/*  7)  Anything left over that must be moved to the next page?		*/
/*  7a) Distinguish between the situation where a fixed height row has	*/
/*	reached its capacity and that where the page is full.		*/
/*									*/
/************************************************************************/

int docLayoutCellPageStrip(	LayoutPosition *	lpHere,
				LayoutPosition *	lpBottom,
				LayoutPosition *	lpBelowAllCells,
				int *			pContinueInNextFrame,
				BufferItem *		cellNode,
				const CellProperties *	cp,
				BlockFrame *		bf,
				ParagraphLayoutJob *	plj )
    {
    ParagraphLayoutPosition *	plp= &(plj->pljPos);
    int				stopCode= FORMATstopREADY;

    /*  3  */
    if  ( CELL_MERGED( cp ) )
	{ return 0;	}

    /*  4  */
    if  ( docLayoutStripDone( plp, plj ) )
	{ return 0;	}

    /*  5  */
    plp->plpPos= *lpHere;

    docCellStripFrame( cellNode, bf, plj );

    if  ( docLayoutStripChildren( &stopCode, plj, bf, cellNode ) )
	{ LDEB(1); return -1;	}

    /*  6  */
    docLayoutPushBottomDown( lpBelowAllCells, &(plp->plpPos) );
    if  ( cellNode->biCellRowspan <= 1 )
	{ docLayoutPushBottomDown( lpBottom, &(plp->plpPos) );	}

    /*  7,7a  */
    if  ( cellNode->biTreeType == DOCinBODY		&&
	  stopCode == FORMATstopBLOCK_FULL		)
	{ *pContinueInNextFrame= 1;	}

    return 0;
    }

/************************************************************************/

void docLayoutStartCell(	BufferItem *		cellNode,
				const LayoutJob *	lj,
				const LayoutPosition *	lpHere,
				ParagraphLayoutJob *	cellPlj )
    {
    const int		para= 0;
    const int		line= 0;
    const int		part= 0;

    const int		recursively= 0;

    docDelimitTables( cellNode, recursively );

    cellNode->biCellTopInset= 0;

    docBeginParagraphLayoutProgress( cellPlj, lj,
					para, line, part,
					cellNode->biChildCount, lpHere );

    cellNode->biTopPosition= *lpHere;

    return;
    }

void docCellToNextColumn(	LayoutPosition *	lpBefore,
				const BufferItem *	cellNode,
				ParagraphLayoutJob *	plj )
    {
    int			advanced;
    const int		advanceAnyway= 0;

    if  ( docLayoutStripDone( &(plj->pljPos), plj ) )
	{ return;	}

    if  ( cellNode->biChildren[plj->pljPos0.pspChild]->biLevel ==
							    DOClevROW )
	{ return;	}

    docCommitStripLayout( &advanced, advanceAnyway, plj,
						    lpBefore, cellNode );

    return;
    }

