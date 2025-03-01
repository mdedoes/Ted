/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docTreeNode.h>
#   include	<docBlockOrnaments.h>
#   include	<docBorderProperties.h>
#   include	"docRowLayout.h"
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<geoRectangle.h>
#   include	<docAttributes.h>
#   include	<docNodeTree.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Decide what top border to use for a table cell (If any)		*/
/*									*/
/*  1)  Get the table row above the current row.			*/
/*  2)  If a cell has a bottom border, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  3)  By default use the cells top border.				*/
/*  4)  Use the table top border at the top of a page.			*/
/*									*/
/************************************************************************/

static const BorderProperties * docGetCellTopBorder(
				int *				pBpTopNr,
				int *				pUseAbove,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				col,
				int				atRowTop )
    {
    const struct BufferItem *	parentNode= rowNode->biParent;
    const RowProperties *	rp= rowNode->biRowProperties;
    const struct BufferItem *	cellNode= rowNode->biChildren[col];
    const CellProperties *	cp= cellNode->biCellProperties;

    const struct BufferItem *	rowNodeAbove= (const struct BufferItem *)0;

    const CellProperties *	aboveCpC= (const CellProperties *)0;
    int				useAbove= 0;

    const BorderProperties *	bpAbove= (const BorderProperties *)0;
    const BorderProperties *	bpTop= (const BorderProperties *)0;
    int				aboveNr= -1;
    int				topNr;

    /*  1  */
    if  ( rowNode->biNumberInParent > rowNode->biRowTableFirst )
	{
	if  ( rowNode->biNumberInParent <= 0 )
	    {
	    LLDEB(rowNode->biNumberInParent,rowNode->biRowTableFirst);
	    SDEB(docTreeTypeStr(rowNode->biTreeType));
	    }
	else{
	    rowNodeAbove= parentNode->biChildren[rowNode->biNumberInParent- 1];
	    }
	}

    if  ( rowNodeAbove )
	{
	int			aboveCol;
	const RowProperties *	rpAbove= rowNodeAbove->biRowProperties;

	aboveCol= docGetMatchingCell( rowNodeAbove, cellNode );
	if  ( aboveCol >= 0 && aboveCol < rpAbove->rpCellCount )
	    { aboveCpC= rpAbove->rpCells+ aboveCol;	}
	}

    /*  2  */
    if  ( aboveCpC							&&
	  aboveCpC->cpBottomBorderNumber != cp->cpTopBorderNumber	)
	{
	aboveNr= aboveCpC->cpBottomBorderNumber;
	bpAbove= docGetBorderPropertiesByNumber( bd, aboveNr );

	if  ( ! atRowTop && DOCisBORDER( bpAbove ) )
	    { useAbove= 1;	}
	}

    /*  3  */
    topNr= cp->cpTopBorderNumber;
    bpTop= docGetBorderPropertiesByNumber( bd, topNr );

    /*  2  */
    if  ( useAbove )
	{
	bpTop= bpAbove;
	topNr= aboveNr;
	}

    /*  4  */
    if  ( rowNode->biNumberInParent > rowNode->biRowTableFirst	&&
          ! atRowTop						)
	{
	topNr= rp->rpTopBorderNumber;
	bpTop= docGetBorderPropertiesByNumber( bd, topNr );
	useAbove= 0;
	}

    *pBpTopNr= topNr;
    *pUseAbove= useAbove;

    return bpTop;
    }

/************************************************************************/
/*									*/
/*  Decide what bottom border to use for a table cell (If any)		*/
/*									*/
/*  0)  Paranoia checks: Bugs that I actually once programmed.		*/
/*  1)  Get the table row below the current row.			*/
/*  2)  By default use the cells bottom border.				*/
/*  3)  If a cell has a bottom boder, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  4)  Use the table bottom border at the bottom of a page.		*/
/*									*/
/************************************************************************/

const BorderProperties * docGetCellBottomBorder(
				int *				pBpBottomNr,
				int *				pUseBelow,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				col,
				int				atRowBottom )
    {
    const struct BufferItem *	parentNode= rowNode->biParent;
    const struct BufferItem *	cellNode= rowNode->biChildren[col];

    const RowProperties * 	rp= rowNode->biRowProperties;
    const CellProperties *	cp= cellNode->biCellProperties;

    const struct BufferItem *	rowNodeBelow= (const struct BufferItem *)0;

    const CellProperties *	belowCpC= (const CellProperties *)0;
    int				useBelow= 0;

    const BorderProperties *	bpBelow= (const BorderProperties *)0;
    const BorderProperties  *	bpBottom= (const BorderProperties *)0;
    int				bottomNr;

    /*  0  */
    if  ( col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount);	}
    if  ( rowNode->biRowTablePast < 0				||
	  rowNode->biRowTablePast > rowNode->biParent->biChildCount	)
	{ LLDEB(rowNode->biRowTablePast,rowNode->biParent->biChildCount); }

    /*  1  */
    if  ( rowNode->biNumberInParent < rowNode->biRowTablePast- 1 )
	{ rowNodeBelow= parentNode->biChildren[rowNode->biNumberInParent+ 1]; }

    if  ( rowNodeBelow )
	{
	int			belowCol;
	const RowProperties *	rpBelow= rowNodeBelow->biRowProperties;

	belowCol= docGetMatchingCell( rowNodeBelow, cellNode );
	if  ( belowCol >= 0 )
	    { belowCpC= rpBelow->rpCells+ belowCol;	}
	}

    /*  2  */
    bottomNr= cp->cpBottomBorderNumber;
    bpBottom= docGetBorderPropertiesByNumber( bd, bottomNr );

    /*  3  */
    if  ( belowCpC )
	{
	int		belowNr;

	belowNr= belowCpC->cpBottomBorderNumber;
	bpBelow= docGetBorderPropertiesByNumber( bd, belowNr );

	if  ( ! DOCisBORDER( bpBottom )		&&
	      belowCpC				&&
	      DOCisBORDER( bpBelow )		)
	    {
	    useBelow= 1;
	    bottomNr= belowNr;
	    }
	}

    /*  4  */
    if  ( rowNode->biNumberInParent < rowNode->biRowTablePast- 1	&&
	  ! atRowBottom						)
	{
	bottomNr= rp->rpBottomBorderNumber;
	bpBottom= docGetBorderPropertiesByNumber( bd, bottomNr );
	useBelow= 0;
	}

    *pBpBottomNr= bottomNr;
    *pUseBelow= useBelow;

    return bpBottom;
    }

/************************************************************************/
/*									*/
/*  Get the left border of the cell. 'Left' is in terms of the cell	*/
/*  properties. So in right-to-left rows, the result is drawn on the	*/
/*  right hand side of the cell.					*/
/*									*/
/************************************************************************/

static int docGetCellLeftBorder( const BorderProperties **	pBpLeft,
				int *				bpLeftNr,
				const struct BufferDocument *	bd,
				const RowProperties *		rp,
				int				col )
    {
    int				rval= 0;
    const CellProperties *	cp= rp->rpCells+ col;

    int				leftHasBorder= 0;
    int				hasLeftBorder= 0;

    const BorderProperties *	bpLeft= (const BorderProperties *)0;

    /*  6  */
    if  ( col > 0							&&
	  docBorderNumberIsBorder( bd, cp[-1].cpRightBorderNumber )	)
	{ leftHasBorder= 1;	}

    bpLeft= docGetBorderPropertiesByNumber( bd, cp->cpLeftBorderNumber );
    hasLeftBorder= DOCisBORDER( bpLeft );

    if  ( leftHasBorder						&&
	  cp[-1].cpRightBorderNumber != cp->cpLeftBorderNumber	)
	{
	bpLeft= docGetBorderPropertiesByNumber( bd,
					    cp[-1].cpRightBorderNumber );

	*bpLeftNr= cp[-1].cpRightBorderNumber;
	}
    else{
	/*  9  */
	if  ( hasLeftBorder )
	    {
	    *bpLeftNr= cp->cpLeftBorderNumber;
	    rval= 1;
	    }
	}

    *pBpLeft= bpLeft;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the right border of the cell. 'Right' is in terms of the cell	*/
/*  properties. So in right-to-left rows, the result is drawn on the	*/
/*  left hand side of the cell.						*/
/*									*/
/************************************************************************/

static int docGetCellRightBorder( const BorderProperties **	pBpRight,
				int *				bpRightNr,
				const struct BufferDocument *	bd,
				const RowProperties *		rp,
				int				col )
    {
    int				rval= 0;
    const CellProperties *	cp= rp->rpCells+ col;

    int				drawCellRight= 1;

    int				rightHasBorder= 0;
    int				hasRightBorder= 0;

    const BorderProperties *	bpRight= (const BorderProperties *)0;

    if  ( col < rp->rpCellCount- 1 )
	{
	int			nextCol= col+ 1;
	const CellProperties *	nextCp= rp->rpCells+ nextCol;

	while( nextCol < rp->rpCellCount			&&
	       nextCp->cpHorizontalMerge == CELLmergeFOLLOW	)
	    { nextCol++; nextCp++;	}

	if  ( nextCol < rp->rpCellCount )
	    { drawCellRight= 0;	}
	}

    /*  6  */
    if  ( col < rp->rpCellCount- 1					&&
	  docBorderNumberIsBorder( bd, cp[1].cpLeftBorderNumber )	)
	{ rightHasBorder= 1;	}

    bpRight= docGetBorderPropertiesByNumber( bd, cp->cpRightBorderNumber );
    hasRightBorder= DOCisBORDER( bpRight );

    if  ( ! hasRightBorder && rightHasBorder )
	{
	bpRight= docGetBorderPropertiesByNumber( bd,
						cp[1].cpLeftBorderNumber );
	*bpRightNr= cp[1].cpLeftBorderNumber;
	}

    /*  10  */
    if  ( drawCellRight && hasRightBorder )
	{
	*bpRightNr= cp->cpRightBorderNumber;
	rval= 1;
	}

    *pBpRight= bpRight;
    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the left border of the cell. 'Left' is in geometric terms.	*/
/*  So in right-to-left rows, the result is based on the right border	*/
/*  of the cell properties.						*/
/*									*/
/************************************************************************/

static int docCellLeftBorderDrawn(
				const BorderProperties **	pBpLeft,
				int *				pBpLeftNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const struct BufferDocument *	bd,
				const RowProperties *		rp,
				int				col,
				int				drawTableGrid )
    {
    int				rval= 0;

    int				bpLeftNr= -1;
    const BorderProperties *	bpLeft= (const BorderProperties *)0;

    if  ( rp->rpRToL )
	{
	rval= docGetCellRightBorder( &bpLeft, &bpLeftNr, bd, rp, col );
	if  ( rval < 0 )
	    { LDEB(rval); return 0;	}
	}
    else{
	rval= docGetCellLeftBorder( &bpLeft, &bpLeftNr, bd, rp, col );
	if  ( rval < 0 )
	    { LDEB(rval); return 0;	}
	}

    if  ( bpLeft )
	{
	int		thick= 0;
	int		space;

	thick= docBorderThick( &space, bpLeft );

	drOutside->drX0= drCell->drX0+ 1- thick/ 2;
	drInside->drX0= drOutside->drX0+ thick;
	}

    if  ( rval )
	{ *pProp= ORNdrawLEFT_BORDER;	}
    else{
	if  ( drawTableGrid )
	    { *pProp= ORNdrawLEFT_GRID; rval= 1;	}
	}

    *pBpLeftNr= bpLeftNr;
    *pBpLeft= bpLeft;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the right border of the cell. 'Right' is in geometric terms.	*/
/*  So in right-to-left rows, the result is based on the left border	*/
/*  of the cell properties.						*/
/*									*/
/************************************************************************/

static int docCellRightBorderDrawn(
				const BorderProperties **	pBpRight,
				int *				pBpRightNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const struct BufferDocument *	bd,
				const RowProperties * 		rp,
				int				col,
				int				drawTableGrid )
    {
    int				rval= 0;

    int				bpRightNr= -1;
    const BorderProperties *	bpRight= (const BorderProperties *)0;

    if  ( rp->rpRToL )
	{
	rval= docGetCellLeftBorder( &bpRight, &bpRightNr, bd, rp, col );
	if  ( rval < 0 )
	    { LDEB(rval); return 0;	}
	}
    else{
	rval= docGetCellRightBorder( &bpRight, &bpRightNr, bd, rp, col );
	if  ( rval < 0 )
	    { LDEB(rval); return 0;	}
	}

    /*  10  */
    if  ( rval )
	{
	int		thick;
	int		space;

	*pProp= ORNdrawRIGHT_BORDER;

	thick= docBorderThick( &space, bpRight );
	drInside->drX1= drCell->drX1- thick/ 2;
	drOutside->drX1= drInside->drX1+ thick;
	}
    else{
	/*  7  */
	if  ( drawTableGrid )
	    { *pProp= ORNdrawRIGHT_GRID; rval= 1;	}
	}

    *pBpRightNr= bpRightNr;
    *pBpRight= bpRight;

    return rval;
    }

static int docCellTopBorderDrawn( const BorderProperties **	pBpTop,
				int *				bpTopNr,
				int *				pProp,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				col,
				int				atRowTop,
				int				drawTableGrid )
    {
    int				rval= 0;
    int				useAbove= 0;

    const BorderProperties *	bpTop= (const BorderProperties *)0;

    bpTop= docGetCellTopBorder( bpTopNr, &useAbove, bd,
					    rowNode, col, atRowTop );

    if  ( useAbove )
	{
	int	thick;
	int	space;

	thick= docBorderThick( &space, bpTop );

	drInside->drY0= drCell->drY0+ thick;
	}

    /*  8  */
    if  ( ! useAbove && DOCisBORDER( bpTop ) )
	{
	int	thick;
	int	space;

	*pProp= ORNdrawTOP_BORDER; rval= 1;

	thick= docBorderThick( &space, bpTop );

	/* border inside cell */
	drInside->drY0= drCell->drY0+ 1+ thick;
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! DOCisBORDER( bpTop )		&&
	      ! useAbove			)
	    {
	    *pProp= ORNdrawTOP_GRID; rval= 1;
	    /* inside cell, width determined by screen drawer */
	    }
	}

    *pBpTop= bpTop;
    return rval;
    }

static int docCellBottomBorderDrawn(
				const BorderProperties **	pBpBottom,
				int *				bpBottomNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				const DocumentRectangle *	drCell,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				col,
				int				atRowBottom,
				int				drawTableGrid )
    {
    int				rval= 0;

    int				thick;
    int				space;

    int				useBelow= 0;

    const BorderProperties *	bpBottom= (const BorderProperties *)0;

    bpBottom= docGetCellBottomBorder( bpBottomNr, &useBelow,
					bd, rowNode, col, atRowBottom );

    /*  11  */
    if  ( DOCisBORDER( bpBottom ) )
	{
	*pProp= ORNdrawBOTTOM_BORDER; rval= 1;

	thick= docBorderThick( &space, bpBottom );
	drOutside->drY1= drCell->drY1+ thick;
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! DOCisBORDER( bpBottom )		&&
	      ! useBelow			)
	    { *pProp= ORNdrawBOTTOM_GRID; rval= 1;	}
	}

    *pBpBottom= bpBottom;
    return rval;
    }

/************************************************************************/
/*									*/
/*  Retrieve the borders and shading of a cell.				*/
/*									*/
/*  3)  Is this cell shaded?						*/
/*  6)  If a cell has a right boder, and the next one a left border,	*/
/*	MS-Word only draws the right border of the leftmost cell.	*/
/*	( The same applies in the vertical direction. )			*/
/*  7)  Base the decisions on whether to draw a table grid on the same	*/
/*	criteria.							*/
/*  8)  Draw cell top border.						*/
/*  9)  Draw cell left border.						*/
/* 10)  Draw cell right border.						*/
/* 11)  Draw cell bottom border.					*/
/*									*/
/* Experimentation with MS-Word reveals:				*/
/* a)	The the top border is drawn inside the cell and that the	*/
/*	formatter allocates space for it.				*/
/* b)	That the left and right borders are half in the cell. Typically	*/
/*	covering part of the horizontal gap that the table reserves.	*/
/* c)	That the bottom border is completely below the cell. So inside	*/
/*	the table it comes at the same location as the top border of	*/
/*	the next row.							*/
/*									*/
/************************************************************************/

void docGetCellOrnaments(
			BlockOrnaments *		cellOrnaments,
			DocumentRectangle *		drOutside,
			DocumentRectangle *		drInside,
			const DocumentRectangle *	drCell,
			const struct BufferDocument *	bd,
			const struct BufferItem *	rowNode,
			int				col,
			int				atRowTop,
			int				atRowBottom,
			int				drawTableGrid )
    {
    const RowProperties * 	rp= rowNode->biRowProperties;
    const CellProperties *	cp= rp->rpCells+ col;
    int				shadingNumber;

    int				prop= 0;

    *drOutside= *drCell;
    *drInside= *drCell;

    /*  3  */
    shadingNumber= cp->cpShadingNumber;
    if  ( shadingNumber == 0 )
	{ shadingNumber= rp->rpShadingNumber;	}

    if  ( shadingNumber != 0 )
	{
	cellOrnaments->boShading= docGetItemShadingByNumber( bd,
							    shadingNumber );

	PROPmaskADD( &(cellOrnaments->boPropMask), ORNdrawSHADE );
	}

    if  ( docCellTopBorderDrawn( &(cellOrnaments->boTopBorder),
		    &(cellOrnaments->boTopBorderNumber),
		    &prop, drInside, drCell,
		    bd, rowNode, col, atRowTop, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellLeftBorderDrawn( &(cellOrnaments->boLeftBorder),
		&(cellOrnaments->boLeftBorderNumber),
		&prop, drOutside, drInside, drCell,
		bd, rp, col, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellRightBorderDrawn( &(cellOrnaments->boRightBorder),
		&(cellOrnaments->boRightBorderNumber),
		&prop, drOutside, drInside, drCell,
		bd, rp, col, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellBottomBorderDrawn( &(cellOrnaments->boBottomBorder),
		    &(cellOrnaments->boBottomBorderNumber),
		    &prop, drOutside, drCell,
		    bd, rowNode, col, atRowBottom, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate layout insets for a (table) row.				*/
/*									*/
/*  This routine is also used to calculate the inset that is used to	*/
/*  accomodate space for the bottom border of the previous row. This	*/
/*  inset is not only used in real rows, but also in the immediate	*/
/*  successor of a real row.						*/
/*									*/
/*  1)  Reserve space for the top border of all cells.			*/
/*									*/
/************************************************************************/

void docLayoutCalculateRowTopInset(
				int *				pInset,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				atRowTop )
    {
    int				col;

    *pInset= 0;

    /*  1  */
    for ( col= 0; col < rowNode->biChildCount; col++ )
	{
	/*const*/ struct BufferItem *	cellNode= rowNode->biChildren[col];
	int				useAbove= 0;
	const BorderProperties *	bpTop;
	int				topNr;

	/* TODO: What is wrong in the RTF parser? */
	if  ( ! cellNode->biCellProperties )
	    {
	    XDEB(cellNode->biCellProperties);
	    LDEB(1);docListNode(bd,0,rowNode,0);
	    cellNode->biCellProperties= rowNode->biRowProperties->rpCells+ col;
	    }

	if  ( docIsRowNode( rowNode )			&&
	      CELL_MERGED( cellNode->biCellProperties )	)
	    { continue;	}

	bpTop= docGetCellTopBorder( &topNr,
				&useAbove, bd, rowNode, col, atRowTop );

	docStretchInsetForBorder( pInset, bpTop );
	}

    return;
    }

/**
 * Is a cell a plain cell? I.E. It gives no visual clue about being a cell.
 * Then probably, it is only used for formatting purposes. If this is the 
 * case for all cells in the table, then we might not want to mark the
 * table as a real table in the generated PDF.
 */
int docCellPropertiesArePlain(
			const struct BufferDocument *	bd,
			const CellProperties *		cp )
    {
    return ! cp->cpIsRowHeader &&
	   ! docBorderNumberIsBorder( bd, cp->cpTopBorderNumber ) &&
	   ! docBorderNumberIsBorder( bd, cp->cpLeftBorderNumber ) &&
	   ! docBorderNumberIsBorder( bd, cp->cpRightBorderNumber ) &&
	   ! docBorderNumberIsBorder( bd, cp->cpBottomBorderNumber ) &&
	   ! docShadingNumberIsShading( bd, cp->cpShadingNumber ) ;
    }

/**
 * Is a row a plain row? I.E. It gives no visual clue about being a row.
 * Then probably, it is only used for formatting purposes. If this is the 
 * case for all rows in the table, then we might not want to mark the
 * table as a real table in the generated PDF.
 */
int docRowPropertiesArePlain(
			const struct BufferDocument *	bd,
			const RowProperties *		rp )
    {
    int		col;

    if  ( rp->rpIsTableHeader ||
	  docBorderNumberIsBorder( bd, rp->rpTopBorderNumber ) ||
	  docBorderNumberIsBorder( bd, rp->rpLeftBorderNumber ) ||
	  docBorderNumberIsBorder( bd, rp->rpRightBorderNumber ) ||
	  docBorderNumberIsBorder( bd, rp->rpBottomBorderNumber ) ||
	  docShadingNumberIsShading( bd, rp->rpShadingNumber ) )
	{ return 0;	}

    for ( col= 0; col < rp->rpCellCount; col++ )
	{
	const CellProperties *	cp= rp->rpCells+ col;

	if  ( ! docCellPropertiesArePlain( bd, cp ) )
	    { return 0;	}
	}

    return 1;
    }
