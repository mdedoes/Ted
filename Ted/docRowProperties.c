/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a BufferItem.						*/
/*									*/
/************************************************************************/

void docCleanInitRowProperties(	RowProperties *	rp )
    {
    docCleanRowProperties( rp );
    docInitRowProperties( rp );
    }

void docCleanRowProperties(	RowProperties *	rp )
    {
    if  ( rp->rpCells )
	{ free( rp->rpCells );	}
    }

void docInitRowProperties(	RowProperties *	rp )
    {
    rp->rpCellCount= 0;
    rp->rpCells= (CellProperties *)0;

    rp->rpHalfGapWidthTwips= 0;
    rp->rpHalfGapWidthPixels= 0;

    rp->rpHeightTwips= 0;

    rp->rpLeftIndentTwips= 0;
    rp->rpLeftIndentPixels= 0;

    rp->rpHasTableParagraphs= 0;

    rp->rpIsTableHeader= 0;
    rp->rpKeepOnOnePage= 0;
    rp->rpAutofit= 0;

    docInitBorderProperties( &rp->rpTopBorder );
    docInitBorderProperties( &rp->rpBottomBorder );
    docInitBorderProperties( &rp->rpLeftBorder );
    docInitBorderProperties( &rp->rpRightBorder );
    docInitBorderProperties( &rp->rpHorizontalBorder );
    docInitBorderProperties( &rp->rpVerticalBorder );

    /**/
    rp->rpPreferredWidth= 0;
    rp->rpPreferredWidthUnit= TRautoNONE;

    /**/
    rp->rpLeftDefaultCellSpacing= 0;
    rp->rpRightDefaultCellSpacing= 0;
    rp->rpTopDefaultCellSpacing= 0;
    rp->rpBottomDefaultCellSpacing= 0;

    rp->rpLeftDefaultCellSpacingUnit= TRautoNONE;
    rp->rpRightDefaultCellSpacingUnit= TRautoNONE;
    rp->rpTopDefaultCellSpacingUnit= TRautoNONE;
    rp->rpBottomDefaultCellSpacingUnit= TRautoNONE;

    /**/
    rp->rpLeftDefaultCellMargin= 0;
    rp->rpRightDefaultCellMargin= 0;
    rp->rpTopDefaultCellMargin= 0;
    rp->rpBottomDefaultCellMargin= 0;

    rp->rpLeftDefaultCellMarginUnit= TRautoNONE;
    rp->rpRightDefaultCellMarginUnit= TRautoNONE;
    rp->rpTopDefaultCellMarginUnit= TRautoNONE;
    rp->rpBottomDefaultCellMarginUnit= TRautoNONE;

    /**/
    rp->rpCellWidthBefore= 0;
    rp->rpCellWidthAfter= 0;

    rp->rpCellWidthBeforeUnit= TRautoNONE;
    rp->rpCellWidthAfterUnit= TRautoNONE;

    return;
    }

int docInsertRowColumn(	RowProperties *			rp,
			int				n,
			const CellProperties *		cp,
			const int *			colorMap )
    {
    CellProperties *	fresh;

    fresh= (CellProperties *)realloc( rp->rpCells,
			(rp->rpCellCount+ 1)* sizeof(CellProperties) );
    if  ( ! fresh )
	{ LXDEB(rp->rpCellCount,fresh); return -1;	}
    rp->rpCells= fresh;

    if  ( n < 0 )
	{ n= rp->rpCellCount;	}
    else{
	int		i;

	for ( i= rp->rpCellCount; i > n; i-- )
	    { docCopyCellProperties( fresh+ i, fresh+ i- 1, colorMap );	}
	}

    docInitCellProperties( fresh+ n );
    docCopyCellProperties( fresh+ n, cp, colorMap );

    rp->rpCellCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a number of columns from row properties.			*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRow(	RowProperties *		rp,
				int			col0,
				int			count )
    {
    int		col1= col0+ count- 1;
    int		left;
    int		wide;
    int		col;

    if  ( col0+ count > rp->rpCellCount )
	{ LLLDEB(col0,count,rp->rpCellCount); return -1;	}


    if  ( col0 == 0 )
	{ left= rp->rpLeftIndentTwips;				}
    else{ left= rp->rpCells[col0-1].cpRightBoundaryTwips;	}

    wide= rp->rpCells[col1].cpRightBoundaryTwips- left;

    rp->rpCellCount -= count;

    for ( col= col0; col < rp->rpCellCount; col++ )
	{
	docCleanCellProperties( &(rp->rpCells[col]) );
	rp->rpCells[col]= rp->rpCells[col+ count];
	rp->rpCells[col].cpRightBoundaryTwips -= wide;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy row properties. As this routine is not called very often,	*/
/*  just call the 'update' routine.					*/
/*									*/
/************************************************************************/

int docCopyRowProperties(	RowProperties *		rpTo,
				const RowProperties *	rpFrom,
				const int *		colorMap )
    {
    PropertyMask		rpDoneMask;
    PropertyMask		rpSetMask;

    PROPmaskCLEAR( &rpDoneMask );
    PROPmaskCLEAR( &rpSetMask );

    PROPmaskFILL( &rpSetMask, RPprop_COUNT );

    if  ( docUpdRowProperties( &rpDoneMask, rpTo,
					    &rpSetMask, rpFrom, colorMap ) )
	{ LDEB(1); return -1;	}

    rpTo->rpHasTableParagraphs= rpFrom->rpHasTableParagraphs;

    return 0;
    }

/************************************************************************/
/*									*/
/*  1)  Are the columns in two RowProperties 'the same' (Do they	*/
/*	align?)								*/
/*  2)  All internal borders equal?					*/
/*  3)  All column properties identical?				*/
/*									*/
/************************************************************************/

/*  1  */
int docAlignedColumns(	const RowProperties *	rp1,
			const RowProperties *	rp2 )
    {
    CellProperties *	cp1;
    CellProperties *	cp2;
    int			i;

    if  ( rp1->rpHasTableParagraphs != rp2->rpHasTableParagraphs )
	{ return 0;	}

    if  ( rp1->rpCellCount != rp2->rpCellCount )
	{ return 0;	}

    if  ( rp1->rpHalfGapWidthTwips != rp2->rpHalfGapWidthTwips )
	{ return 0;	}

    if  ( rp1->rpLeftIndentTwips != rp2->rpLeftIndentTwips )
	{ return 0;	}

    cp1= rp1->rpCells;
    cp2= rp2->rpCells;
    for ( i= 0; i < rp1->rpCellCount; cp2++, cp1++, i++ )
	{
	if  ( cp1->cpRightBoundaryTwips != cp2->cpRightBoundaryTwips )
	    { return 0;	}
	}

    return 1;
    }

/*  2  */
int docEqualRowBorders(	const RowProperties *	rp1,
			const RowProperties *	rp2 )
    {
    PropertyMask	rpChgMask;
    PropertyMask	rpUpdMask;

    const int * const	colorMap= (const int *)0;

    PROPmaskCLEAR( &rpUpdMask );
    PROPmaskADD( &rpUpdMask, RPpropTOP_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropBOTTOM_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropLEFT_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropRIGHT_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropHORIZ_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropVERT_BORDER );
    PROPmaskCLEAR( &rpChgMask );

    docRowPropertyDifference( &rpChgMask, rp1, rp2, &rpUpdMask, colorMap );

    if  ( ! utilPropMaskIsEmpty( &rpChgMask ) )
	{ return 0;	}

    return 1;
    }

/*  3  */
int docEqualRows(	const RowProperties *	rp1,
			const RowProperties *	rp2 )
    {
    PropertyMask	rpChgMask;
    PropertyMask	rpUpdMask;

    const int * const	colorMap= (const int *)0;

    PROPmaskFILL( &rpUpdMask, RPprop_COUNT );
    PROPmaskCLEAR( &rpChgMask );

    docRowPropertyDifference( &rpChgMask, rp1, rp2, &rpUpdMask, colorMap );

    if  ( ! utilPropMaskIsEmpty( &rpChgMask ) )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Change row properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdRowProperties(	PropertyMask *			pRpDonePask,
				RowProperties *			rpTo,
				const PropertyMask *		rpUpdMask,
				const RowProperties *		rpFrom,
				const int *			colorMap )
    {
    PropertyMask		rpDoneMask;

    PROPmaskCLEAR( &rpDoneMask );

    if  ( PROPmaskISSET( rpUpdMask, RPpropCOLUMNS ) )
	{
	int			col;
	CellProperties *	cpTo;
	const CellProperties *	cpFrom;

	while( rpTo->rpCellCount > rpFrom->rpCellCount )
	    {
	    docCleanCellProperties( &(rpTo->rpCells[rpTo->rpCellCount-1]) );
	    PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );
	    rpTo->rpCellCount--;
	    }

	cpTo= rpTo->rpCells;
	cpFrom= rpFrom->rpCells;
	for ( col= 0; col < rpTo->rpCellCount; cpTo++, cpFrom++, col++ )
	    {
	    PropertyMask	cpDoneMask;
	    PropertyMask	cpUpdMask;

	    PROPmaskFILL( &cpUpdMask, CLprop_COUNT );
	    PROPmaskCLEAR( &cpDoneMask );

	    if  ( docUpdCellProperties( &cpDoneMask, cpTo,
					    &cpUpdMask, cpFrom, colorMap ) )
		{ LDEB(1); return -1;	}

	    if  ( ! utilPropMaskIsEmpty( &cpDoneMask ) )
		{ PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );	}
	    }

	while( rpTo->rpCellCount < rpFrom->rpCellCount )
	    {
	    if  ( docInsertRowColumn( rpTo, rpTo->rpCellCount,
							cpFrom, colorMap ) )
		{ LDEB(rpTo->rpCellCount); return -1;	}

	    PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );
	    cpFrom++;
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropGAP_WIDTH ) )
	{
	if  ( rpTo->rpHalfGapWidthTwips != rpFrom->rpHalfGapWidthTwips )
	    {
	    rpTo->rpHalfGapWidthTwips= rpFrom->rpHalfGapWidthTwips;
	    PROPmaskADD( &rpDoneMask, RPpropGAP_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_INDENT ) )
	{
	if  ( rpTo->rpLeftIndentTwips != rpFrom->rpLeftIndentTwips )
	    {
	    rpTo->rpLeftIndentTwips= rpFrom->rpLeftIndentTwips;
	    PROPmaskADD( &rpDoneMask, RPpropLEFT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHEIGHT ) )
	{
	if  ( rpTo->rpHeightTwips != rpFrom->rpHeightTwips )
	    {
	    rpTo->rpHeightTwips= rpFrom->rpHeightTwips;
	    PROPmaskADD( &rpDoneMask, RPpropHEIGHT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpTopBorder),
				    &(rpFrom->rpTopBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpTopBorder),
				    &(rpFrom->rpTopBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpBottomBorder),
				    &(rpFrom->rpBottomBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpBottomBorder),
				    &(rpFrom->rpBottomBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpLeftBorder),
				    &(rpFrom->rpLeftBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpLeftBorder),
				    &(rpFrom->rpLeftBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpRightBorder),
				    &(rpFrom->rpRightBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpRightBorder),
				    &(rpFrom->rpRightBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHORIZ_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpHorizontalBorder),
				    &(rpFrom->rpHorizontalBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpHorizontalBorder),
				    &(rpFrom->rpHorizontalBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropHORIZ_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropVERT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpVerticalBorder),
				    &(rpFrom->rpVerticalBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpVerticalBorder),
				    &(rpFrom->rpVerticalBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropVERT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropALIGNMENT ) )
	{
	if  ( rpTo->rpAlignment != rpFrom->rpAlignment )
	    {
	    rpTo->rpAlignment= rpFrom->rpAlignment;
	    PROPmaskADD( &rpDoneMask, RPpropALIGNMENT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropIS_TABLE_HEADER ) )
	{
	if  ( rpTo->rpIsTableHeader != rpFrom->rpIsTableHeader )
	    {
	    rpTo->rpIsTableHeader= rpFrom->rpIsTableHeader;
	    PROPmaskADD( &rpDoneMask, RPpropIS_TABLE_HEADER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_ON_ONE_PAGE ) )
	{
	if  ( rpTo->rpKeepOnOnePage != rpFrom->rpKeepOnOnePage )
	    {
	    rpTo->rpKeepOnOnePage= rpFrom->rpKeepOnOnePage;
	    PROPmaskADD( &rpDoneMask, RPpropKEEP_ON_ONE_PAGE );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropAUTOFIT ) )
	{
	if  ( rpTo->rpAutofit != rpFrom->rpAutofit )
	    {
	    rpTo->rpAutofit= rpFrom->rpAutofit;
	    PROPmaskADD( &rpDoneMask, RPpropAUTOFIT );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTH ) )
	{
	if  ( rpTo->rpPreferredWidth != rpFrom->rpPreferredWidth )
	    {
	    rpTo->rpPreferredWidth= rpFrom->rpPreferredWidth;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTH ) )
	{
	if  ( rpTo->rpPreferredWidthUnit != rpFrom->rpPreferredWidthUnit )
	    {
	    rpTo->rpPreferredWidthUnit= rpFrom->rpPreferredWidthUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTH );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDL ) )
	{
	if  ( rpTo->rpLeftDefaultCellSpacing !=
				      rpFrom->rpLeftDefaultCellSpacing )
	    {
	    rpTo->rpLeftDefaultCellSpacing= rpFrom->rpLeftDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDR ) )
	{
	if  ( rpTo->rpRightDefaultCellSpacing !=
				    rpFrom->rpRightDefaultCellSpacing )
	    {
	    rpTo->rpRightDefaultCellSpacing= rpFrom->rpRightDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDT ) )
	{
	if  ( rpTo->rpTopDefaultCellSpacing !=
				    rpFrom->rpTopDefaultCellSpacing )
	    {
	    rpTo->rpTopDefaultCellSpacing= rpFrom->rpTopDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDB ) )
	{
	if  ( rpTo->rpBottomDefaultCellSpacing !=
				    rpFrom->rpBottomDefaultCellSpacing )
	    {
	    rpTo->rpBottomDefaultCellSpacing= rpFrom->rpBottomDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFL ) )
	{
	if  ( rpTo->rpLeftDefaultCellSpacingUnit !=
				      rpFrom->rpLeftDefaultCellSpacingUnit )
	    {
	    rpTo->rpLeftDefaultCellSpacingUnit=
				    rpFrom->rpLeftDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFR ) )
	{
	if  ( rpTo->rpRightDefaultCellSpacingUnit !=
				    rpFrom->rpRightDefaultCellSpacingUnit )
	    {
	    rpTo->rpRightDefaultCellSpacingUnit=
				    rpFrom->rpRightDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFT ) )
	{
	if  ( rpTo->rpTopDefaultCellSpacingUnit !=
				    rpFrom->rpTopDefaultCellSpacingUnit )
	    {
	    rpTo->rpTopDefaultCellSpacingUnit=
				    rpFrom->rpTopDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFB ) )
	{
	if  ( rpTo->rpBottomDefaultCellSpacingUnit !=
				    rpFrom->rpBottomDefaultCellSpacingUnit )
	    {
	    rpTo->rpBottomDefaultCellSpacingUnit=
				    rpFrom->rpBottomDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDL ) )
	{
	if  ( rpTo->rpLeftDefaultCellMargin !=
				      rpFrom->rpLeftDefaultCellMargin )
	    {
	    rpTo->rpLeftDefaultCellMargin= rpFrom->rpLeftDefaultCellMargin;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDR ) )
	{
	if  ( rpTo->rpRightDefaultCellMargin !=
				    rpFrom->rpRightDefaultCellMargin )
	    {
	    rpTo->rpRightDefaultCellMargin= rpFrom->rpRightDefaultCellMargin;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDT ) )
	{
	if  ( rpTo->rpTopDefaultCellMargin !=
				    rpFrom->rpTopDefaultCellMargin )
	    {
	    rpTo->rpTopDefaultCellMargin= rpFrom->rpTopDefaultCellMargin;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDB ) )
	{
	if  ( rpTo->rpBottomDefaultCellMargin !=
				    rpFrom->rpBottomDefaultCellMargin )
	    {
	    rpTo->rpBottomDefaultCellMargin= rpFrom->rpBottomDefaultCellMargin;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFL ) )
	{
	if  ( rpTo->rpLeftDefaultCellMarginUnit !=
				      rpFrom->rpLeftDefaultCellMarginUnit )
	    {
	    rpTo->rpLeftDefaultCellMarginUnit=
				    rpFrom->rpLeftDefaultCellMarginUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFR ) )
	{
	if  ( rpTo->rpRightDefaultCellMarginUnit !=
				    rpFrom->rpRightDefaultCellMarginUnit )
	    {
	    rpTo->rpRightDefaultCellMarginUnit=
				    rpFrom->rpRightDefaultCellMarginUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFT ) )
	{
	if  ( rpTo->rpTopDefaultCellMarginUnit !=
				    rpFrom->rpTopDefaultCellMarginUnit )
	    {
	    rpTo->rpTopDefaultCellMarginUnit=
				    rpFrom->rpTopDefaultCellMarginUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFB ) )
	{
	if  ( rpTo->rpBottomDefaultCellMarginUnit !=
				    rpFrom->rpBottomDefaultCellMarginUnit )
	    {
	    rpTo->rpBottomDefaultCellMarginUnit=
				    rpFrom->rpBottomDefaultCellMarginUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHB ) )
	{
	if  ( rpTo->rpCellWidthBefore != rpFrom->rpCellWidthBefore )
	    {
	    rpTo->rpCellWidthBefore= rpFrom->rpCellWidthBefore;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTHB );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHA ) )
	{
	if  ( rpTo->rpCellWidthAfter != rpFrom->rpCellWidthAfter )
	    {
	    rpTo->rpCellWidthAfter= rpFrom->rpCellWidthAfter;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTHA );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHB ) )
	{
	if  ( rpTo->rpCellWidthBeforeUnit != rpFrom->rpCellWidthBeforeUnit )
	    {
	    rpTo->rpCellWidthBeforeUnit= rpFrom->rpCellWidthBeforeUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTHB );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHA ) )
	{
	if  ( rpTo->rpCellWidthAfterUnit != rpFrom->rpCellWidthAfterUnit )
	    {
	    rpTo->rpCellWidthAfterUnit= rpFrom->rpCellWidthAfterUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTHA );
	    }
	}

    /**/

    *pRpDonePask= rpDoneMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Compare row properties.						*/
/*									*/
/************************************************************************/

void docRowPropertyDifference(	PropertyMask *			pRpChgPask,
				const RowProperties *		rp1,
				const RowProperties *		rp2,
				const PropertyMask *		rpUpdMask,
				const int *			colorMap )
    {
    PropertyMask		rpChgMask;

    PROPmaskCLEAR( &rpChgMask );

    if  ( PROPmaskISSET( rpUpdMask, RPpropCOLUMNS ) )
	{
	if  ( rp1->rpCellCount != rp2->rpCellCount )
	    { PROPmaskADD( &rpChgMask, RPpropCOLUMNS );	}
	else{
	    int				col;
	    const CellProperties *	cp1;
	    const CellProperties *	cp2;

	    cp1= rp1->rpCells;
	    cp2= rp2->rpCells;
	    for ( col= 0; col < rp1->rpCellCount; cp1++, cp2++, col++ )
		{
		PropertyMask	cpChgMask;
		PropertyMask	cpUpdMask;

		PROPmaskFILL( &cpUpdMask, CLprop_COUNT );
		PROPmaskCLEAR( &cpChgMask );

		docCellPropertyDifference( &cpChgMask, cp1,
						    cp2, &cpUpdMask, colorMap );

		if  ( ! utilPropMaskIsEmpty( &cpChgMask ) )
		    {
		    PROPmaskADD( &rpChgMask, RPpropCOLUMNS );
		    break;
		    }
		}
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropGAP_WIDTH ) )
	{
	if  ( rp1->rpHalfGapWidthTwips != rp2->rpHalfGapWidthTwips )
	    { PROPmaskADD( &rpChgMask, RPpropGAP_WIDTH );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_INDENT ) )
	{
	if  ( rp1->rpLeftIndentTwips != rp2->rpLeftIndentTwips )
	    { PROPmaskADD( &rpChgMask, RPpropLEFT_INDENT );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHEIGHT ) )
	{
	if  ( rp1->rpHeightTwips != rp2->rpHeightTwips )
	    { PROPmaskADD( &rpChgMask, RPpropHEIGHT );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpTopBorder), &(rp2->rpTopBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropTOP_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpBottomBorder),
						&(rp2->rpBottomBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropBOTTOM_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpLeftBorder),
						&(rp2->rpLeftBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropLEFT_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpRightBorder),
						&(rp2->rpRightBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropRIGHT_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHORIZ_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpHorizontalBorder),
						&(rp2->rpHorizontalBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropHORIZ_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropVERT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpVerticalBorder),
						&(rp2->rpVerticalBorder), colorMap ) )
	    { PROPmaskADD( &rpChgMask, RPpropVERT_BORDER );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropALIGNMENT ) )
	{
	if  ( rp1->rpAlignment != rp2->rpAlignment )
	    { PROPmaskADD( &rpChgMask, RPpropALIGNMENT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropIS_TABLE_HEADER ) )
	{
	if  ( rp1->rpIsTableHeader != rp2->rpIsTableHeader )
	    { PROPmaskADD( &rpChgMask, RPpropIS_TABLE_HEADER ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_ON_ONE_PAGE ) )
	{
	if  ( rp1->rpKeepOnOnePage != rp2->rpKeepOnOnePage )
	    { PROPmaskADD( &rpChgMask, RPpropKEEP_ON_ONE_PAGE ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropAUTOFIT ) )
	{
	if  ( rp1->rpAutofit != rp2->rpAutofit )
	    { PROPmaskADD( &rpChgMask, RPpropAUTOFIT ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTH ) )
	{
	if  ( rp1->rpPreferredWidth != rp2->rpPreferredWidth )
	    { PROPmaskADD( &rpChgMask, RPpropTRW_WIDTH );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTH ) )
	{
	if  ( rp1->rpPreferredWidthUnit != rp2->rpPreferredWidthUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRFTS_WIDTH );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDL ) )
	{
	if  ( rp1->rpLeftDefaultCellSpacing !=
				      rp2->rpLeftDefaultCellSpacing )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDR ) )
	{
	if  ( rp1->rpRightDefaultCellSpacing !=
				    rp2->rpRightDefaultCellSpacing )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDT ) )
	{
	if  ( rp1->rpTopDefaultCellSpacing !=
				    rp2->rpTopDefaultCellSpacing )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDB ) )
	{
	if  ( rp1->rpBottomDefaultCellSpacing !=
				    rp2->rpBottomDefaultCellSpacing )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFL ) )
	{
	if  ( rp1->rpLeftDefaultCellSpacingUnit !=
				      rp2->rpLeftDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDFL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFR ) )
	{
	if  ( rp1->rpRightDefaultCellSpacingUnit !=
				    rp2->rpRightDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDFR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFT ) )
	{
	if  ( rp1->rpTopDefaultCellSpacingUnit !=
				    rp2->rpTopDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDFT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFB ) )
	{
	if  ( rp1->rpBottomDefaultCellSpacingUnit !=
				    rp2->rpBottomDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRSPDFB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDL ) )
	{
	if  ( rp1->rpLeftDefaultCellMargin !=
				      rp2->rpLeftDefaultCellMargin )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDR ) )
	{
	if  ( rp1->rpRightDefaultCellMargin !=
				    rp2->rpRightDefaultCellMargin )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDT ) )
	{
	if  ( rp1->rpTopDefaultCellMargin !=
				    rp2->rpTopDefaultCellMargin )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDB ) )
	{
	if  ( rp1->rpBottomDefaultCellMargin !=
				    rp2->rpBottomDefaultCellMargin )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFL ) )
	{
	if  ( rp1->rpLeftDefaultCellMarginUnit !=
				      rp2->rpLeftDefaultCellMarginUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDFL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFR ) )
	{
	if  ( rp1->rpRightDefaultCellMarginUnit !=
				    rp2->rpRightDefaultCellMarginUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDFR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFT ) )
	{
	if  ( rp1->rpTopDefaultCellMarginUnit !=
				    rp2->rpTopDefaultCellMarginUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDFT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFB ) )
	{
	if  ( rp1->rpBottomDefaultCellMarginUnit !=
				    rp2->rpBottomDefaultCellMarginUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRPADDFB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHB ) )
	{
	if  ( rp1->rpCellWidthBefore != rp2->rpCellWidthBefore )
	    { PROPmaskADD( &rpChgMask, RPpropTRW_WIDTHB );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHA ) )
	{
	if  ( rp1->rpCellWidthAfter != rp2->rpCellWidthAfter )
	    { PROPmaskADD( &rpChgMask, RPpropTRW_WIDTHA );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHB ) )
	{
	if  ( rp1->rpCellWidthBeforeUnit != rp2->rpCellWidthBeforeUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRFTS_WIDTHB );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHA ) )
	{
	if  ( rp1->rpCellWidthAfterUnit != rp2->rpCellWidthAfterUnit )
	    { PROPmaskADD( &rpChgMask, RPpropTRFTS_WIDTHA );	}
	}

    /**/

    *pRpChgPask= rpChgMask; return;
    }

/************************************************************************/
/*									*/
/*  Update borders for a certain number of columns.			*/
/*									*/
/************************************************************************/

void docRowSetTopBorderInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpTopBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetBottomBorderInCols(RowProperties *		rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpBottomBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetLeftBorderInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpLeftBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetRightBorderInCols(RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpRightBorder),
							bpSetMask, bpSet );
	}

    return;
    }

void docRowSetShadingInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		isSetMask,
				const ItemShading *		isSet )
    {
    CellProperties *		cp;
    int				col;

    PropertyMask		isDoneMask;

    const int * const		colorMap= (const int *)0;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateItemShading( &isDoneMask, &(cp->cpShading),
						isSetMask, isSet, colorMap );
	}

    return;
    }

