/************************************************************************/
/*									*/
/*  Manage row properties.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	"docRowProperties.h"
#   include	"docCellProperties.h"
#   include	"docDocumentAttributeMap.h"
#   include	"docPropVal.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/

static const int DocRowIntProperties[]=
{
    RPpropGAP_WIDTH,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,
    RPpropALIGNMENT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPprop_KEEPFOLLOW,
    RPpropAUTOFIT,
    RPpropRTOL,
    RPpropTRW_WIDTH,
    RPpropTRFTS_WIDTH,
    RPpropTRSPDL,
    RPpropTRSPDR,
    RPpropTRSPDT,
    RPpropTRSPDB,
    RPpropTRSPDFL,
    RPpropTRSPDFR,
    RPpropTRSPDFT,
    RPpropTRSPDFB,
    RPpropTRPADDL,
    RPpropTRPADDR,
    RPpropTRPADDT,
    RPpropTRPADDB,
    RPpropTRPADDFL,
    RPpropTRPADDFR,
    RPpropTRPADDFT,
    RPpropTRPADDFB,
    RPpropTRW_WIDTHB,
    RPpropTRW_WIDTHA,
    RPpropTRFTS_WIDTHB,
    RPpropTRFTS_WIDTHA,
    RPpropTRFLAT,
};

static const int DocRowIntPropertyCount=
				sizeof(DocRowIntProperties)/sizeof(int);

static const int DocRowBorderProperties[]=
{
    RPpropTOP_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropHORIZ_BORDER,
    RPpropVERT_BORDER,
};

static const int DocRowBorderPropertyCount=
				sizeof(DocRowBorderProperties)/sizeof(int);

/************************************************************************/
/*									*/
/*  Initialise row properties.						*/
/*									*/
/************************************************************************/

void docCleanRowProperties(	RowProperties *	rp )
    {
    if  ( rp->rpCells )
	{ free( rp->rpCells );	}
    }

void docInitRowProperties(	RowProperties *	rp )
    {
    rp->rpCellCount= 0;
    rp->rpCells= (struct CellProperties *)0;

    rp->rpHalfGapWidthTwips= 0;
    rp->rpLeftIndentTwips= 0;
    rp->rpHeightTwips= 0;

    /**/
    rp->rpRowNumber= -1;
    rp->rpRowBandNumber= -1;
    rp->rpRowStyle= -1;

    rp->rpTopBorderNumber= 0;
    rp->rpBottomBorderNumber= 0;
    rp->rpLeftBorderNumber= 0;
    rp->rpRightBorderNumber= 0;
    rp->rpHorizontalBorderNumber= 0;
    rp->rpVerticalBorderNumber= 0;

    rp->rpShadingNumber= 0;
    rp->rpFrameNumber= 0;

    rp->rpAlignment= DOCthaLEFT;

    rp->rpIsTableHeader= 0;
    rp->rpKeepOnOnePage= 0;
    rp->rp_Keepfollow= 0;
    rp->rpAutofit= 0;
    rp->rpRToL= 0;

    /**/
    rp->rpPreferredWidth= 0;

    /**/
    rp->rpTopCellSpacing= 0;
    rp->rpLeftCellSpacing= 0;
    rp->rpRightCellSpacing= 0;
    rp->rpBottomCellSpacing= 0;

    /**/
    rp->rpLeftCellPadding= 0;
    rp->rpRightCellPadding= 0;
    rp->rpTopCellPadding= 0;
    rp->rpBottomCellPadding= 0;

    /**/
    rp->rpCellWidthBefore= 0;
    rp->rpCellWidthAfter= 0;

    rp->rpFlatInPDF= 0;

    rp->rpAuthor= -1;

    rp->rpPreferredWidthUnit= TRautoNONE;

    rp->rpTopCellSpacingUnit= TRautoNONE;
    rp->rpLeftCellSpacingUnit= TRautoNONE;
    rp->rpRightCellSpacingUnit= TRautoNONE;
    rp->rpBottomCellSpacingUnit= TRautoNONE;

    rp->rpTopCellPaddingUnit= TRautoNONE;
    rp->rpLeftCellPaddingUnit= TRautoNONE;
    rp->rpRightCellPaddingUnit= TRautoNONE;
    rp->rpBottomCellPaddingUnit= TRautoNONE;

    rp->rpCellWidthBeforeUnit= TRautoNONE;
    rp->rpCellWidthAfterUnit= TRautoNONE;

    rp->rpAutoformatBorders= 0;
    rp->rpAutoformatShading= 0;
    rp->rpAutoformatFont= 0;
    rp->rpAutoformatColor= 0;
    rp->rpAutoformatApplyBestFit= 0;
    rp->rpAutoformatFirstRow= 0;
    rp->rpAutoformatLastRow= 0;
    rp->rpAutoformatFirstColumn= 0;
    rp->rpAutoformatLastColumn= 0;

    rp->rpIsLastRow= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the description of a column into those for a row.		*/
/*									*/
/*  If we are asked to shift the columns to the right, shift them by	*/
/*  the width of the new column. I.E. Its new right hand position minus	*/
/*  the left hand position of the first cell that it will push to the	*/
/*  right. This value is not influenced by the operation.		*/
/*									*/
/************************************************************************/

int docInsertRowColumn(	RowProperties *			rp,
			int				col,
			const CellProperties *		cp,
			const DocumentAttributeMap *	dam )
    {
    int			i;
    CellProperties *	fresh;

    CellProperties	sav= *cp; /* cp might be in same row */

    if  ( col > rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    fresh= (CellProperties *)realloc( rp->rpCells,
			    (rp->rpCellCount+ 1)* sizeof(CellProperties) );
    if  ( ! fresh )
	{ LXDEB(rp->rpCellCount,fresh); return -1;	}
    rp->rpCells= fresh;

    if  ( col < 0 )
	{ col= rp->rpCellCount;	}

    for ( i= rp->rpCellCount; i > col; i-- )
	{
	docCopyCellProperties( fresh+ i, fresh+ i- 1,
					    (const DocumentAttributeMap *)0 );
	}

    docInitCellProperties( fresh+ col );
    docCopyCellProperties( fresh+ col, &sav, dam );

    rp->rpCellCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a column wider (or narrower).					*/
/*									*/
/************************************************************************/

int docRowPropertiesMakeColWider(	RowProperties *		rp,
					int			col,
					int			wider )
    {
    CellProperties *	cp;

    if  ( col < 0 || col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    cp= rp->rpCells+ col;
    cp->cpWide += wider;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the left and right hand hand side coordinates.		*/
/*									*/
/************************************************************************/

int docRowGetColumnX(		int *			pX0,
				int *			pX1,
				int *			pX11,
				int *			pColspan,
				const RowProperties *	rp,
				int			col )
    {
    int		x0= rp->rpLeftIndentTwips;
    int		x1;
    int		x11;
    int		colspan= 1;
    int		i;

    if  ( col < 0 || col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    for ( i= 0; i < col; i++ )
	{ x0 += rp->rpCells[i].cpWide;	}

    x1= x0+ rp->rpCells[col].cpWide;
    x11= x1;

    if  ( rp->rpCells[col].cpHorizontalMerge == CELLmergeHEAD )
	{
	for ( i= col+ 1; i < rp->rpCellCount; i++ )
	    {
	    if  ( rp->rpCells[i].cpHorizontalMerge != CELLmergeFOLLOW )
		{ break;	}

	    x11 += rp->rpCells[i].cpWide;
	    colspan++;
	    }
	}

    if  ( pX0 )
	{ *pX0= x0;	}
    if  ( pX1 )
	{ *pX1= x1;	}
    if  ( pX11 )
	{ *pX11= x11;	}
    if  ( pColspan )
	{ *pColspan= colspan;	}

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
    int		col;

    if  ( col0+ count > rp->rpCellCount )
	{ LLLDEB(col0,count,rp->rpCellCount); return -1;	}

    rp->rpCellCount -= count;

    for ( col= col0; col < rp->rpCellCount; col++ )
	{
	/*docCleanCellProperties( &(rp->rpCells[col]) );*/
	rp->rpCells[col]= rp->rpCells[col+ count];
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare row properties.						*/
/*									*/
/************************************************************************/

int docRowPropertyDifference(	PropertyMask *			pRpDifMask,
				PropertyMask *			pCpDifMask,
				const RowProperties *		rp1,
				const PropertyMask *		rpCmpMask,
				const PropertyMask *		cpCmpMask,
				const RowProperties *		rp2,
				const int			colTo,
				const int			colCount,
				const DocumentAttributeMap *	dam )
    {
    int				p;
    const int			colUpto= colTo+ colCount;

    PropertyMask	rpDifMask;
    PropertyMask	cpDifMask;

    utilPropMaskClear( &rpDifMask );
    utilPropMaskClear( &cpDifMask );

    if  ( colTo < 0 || colTo > rp1->rpCellCount )
	{ LDEB(colTo); return -1;	}
    if  ( colCount > rp2->rpCellCount )
	{ LLDEB(colCount,rp2->rpCellCount); return -1; }

    if  ( ! PROPmaskISSET( rpCmpMask, RPprop_CELL_PROPS ) )
	{ cpCmpMask= (const PropertyMask *)0;	}
    else{
	if  ( ! cpCmpMask )
	    { XDEB(cpCmpMask); return -1;	}
	}
    if  ( cpCmpMask && utilPropMaskIsEmpty( cpCmpMask ) )
	{ cpCmpMask= (const PropertyMask *)0;	}

    if  ( PROPmaskISSET( rpCmpMask, RPprop_CELL_COUNT )	)
	{
	if  ( rp1->rpCellCount != colUpto )
	    { PROPmaskADD( &rpDifMask, RPprop_CELL_COUNT );	}
	}
    else{
	if  ( colUpto > rp1->rpCellCount )
	    { LLDEB(colUpto,rp1->rpCellCount); return -1;	}
	}

    if  ( cpCmpMask )
	{
	int			col;
	const CellProperties *	cp1;
	const CellProperties *	cp2;

	int			upto= colUpto;

	/* Do not compare where ther is no overlap: sure to be different */
	if  ( upto > rp1->rpCellCount )
	    { upto=  rp1->rpCellCount;	}

	cp1= rp1->rpCells+ colTo;
	cp2= rp2->rpCells;
	for ( col= colTo; col < upto; cp1++, cp2++, col++ )
	    {
	    PropertyMask	cp_DifMask;

	    utilPropMaskClear( &cp_DifMask );

	    docCellPropertyDifference( &cp_DifMask, cp1, cpCmpMask, cp2, dam );

	    if  ( ! utilPropMaskIsEmpty( &cp_DifMask ) )
		{
		PROPmaskADD( &rpDifMask, RPprop_CELL_PROPS );
		utilPropMaskOr( &cpDifMask, &cpDifMask, &cp_DifMask );
		}
	    }
	}

    for ( p= 0; p < DocRowIntPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];

	if  ( PROPmaskISSET( rpCmpMask, prop ) )
	    {
	    int		val1= docGetRowProperty( rp1, prop );
	    int		val2= docGetRowProperty( rp2, prop );

	    if  ( val1 != val2 )
		{ PROPmaskADD( &rpDifMask, prop );	}
	    }
	}

    for ( p= 0; p < DocRowBorderPropertyCount; p++ )
	{
	int	prop= DocRowBorderProperties[p];

	if  ( PROPmaskISSET( rpCmpMask, prop ) )
	    {
	    int		val1= docGetRowProperty( rp1, prop );
	    int		val2= docGetRowProperty( rp2, prop );

	    if  ( val2 >= 0 && dam && dam->damBorderMap )
		{ val2= dam->damBorderMap[val2];	}

	    if  ( val1 != val2 )
		{ PROPmaskADD( &rpDifMask, prop );	}
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpCmpMask, RPpropSHADING ) )
	{
	int	fromNumber= rp2->rpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( rp1->rpShadingNumber != fromNumber )
	    { PROPmaskADD( &rpDifMask, RPpropSHADING );	}
	}

    if  ( PROPmaskISSET( rpCmpMask, RPpropFRAME ) )
	{
	int	fromNumber= rp2->rpFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( rp1->rpFrameNumber != fromNumber )
	    { PROPmaskADD( &rpDifMask, RPpropFRAME );	}
	}

    if  ( pRpDifMask )
	{ utilPropMaskOr( pRpDifMask, pRpDifMask, &rpDifMask );	}
    if  ( pCpDifMask )
	{ utilPropMaskOr( pCpDifMask, pCpDifMask, &cpDifMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change row properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdRowProperties(	PropertyMask *			pRpDoneMask,
				PropertyMask *			pCpDoneMask,
				RowProperties *			rpTo,
				const PropertyMask *		rpSetMask,
				const PropertyMask *		cpSetMask,
				const RowProperties *		rpSet,
				const int			colTo,
				const int			colCount,
				const DocumentAttributeMap *	dam )
    {
    int				p;
    const int			colUpto= colTo+ colCount;
    const int			oldUpto= rpTo->rpCellCount;

    PropertyMask	rpDoneMask;
    PropertyMask	cpDoneMask;

    utilPropMaskClear( &rpDoneMask );
    utilPropMaskClear( &cpDoneMask );

    if  ( colTo < 0 || colTo > rpTo->rpCellCount )
	{ LDEB(colTo); return -1;	}

    if  ( ! PROPmaskISSET( rpSetMask, RPprop_CELL_PROPS ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    else{
	if  ( ! cpSetMask )
	    { XDEB(cpSetMask); return -1;	}
	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}

    if  ( PROPmaskISSET( rpSetMask, RPprop_CELL_COUNT )	)
	{
	if  ( rpTo->rpCellCount != colUpto )
	    {
	    if  ( rpTo->rpCellCount > colUpto )
		{ rpTo->rpCellCount=  colUpto;	}

	    if  ( rpTo->rpCellCount < colUpto )
		{
		CellProperties *	fresh;

		if  ( colCount > rpSet->rpCellCount )
		    { LLDEB(colCount,rpSet->rpCellCount); return -1; }

		fresh= realloc( rpTo->rpCells,
					    colUpto* sizeof(CellProperties) );
		if  ( ! fresh )
		    { LXDEB(colUpto,fresh); return -1;	}
		rpTo->rpCells= fresh;

		while( rpTo->rpCellCount < colUpto )
		    {
		    rpTo->rpCells[rpTo->rpCellCount]=
				    rpSet->rpCells[rpTo->rpCellCount- colTo];
		    rpTo->rpCellCount++;
		    }
		}

	    PROPmaskADD( &rpDoneMask, RPprop_CELL_COUNT );
	    }
	}
    else{
	if  ( colUpto > rpTo->rpCellCount )
	    { LLDEB(colUpto,rpTo->rpCellCount); return -1;	}
	}

    if  ( cpSetMask )
	{
	int			col;
	CellProperties *	cpTo;
	const CellProperties *	cpSet;

	int			upto= colUpto;

	if  ( colCount > rpSet->rpCellCount )
	    { LLDEB(colCount,rpSet->rpCellCount); return -1; }

	/* Do not compare what we just copied: sure to be equal */
	if  ( upto > oldUpto )
	    { upto=  oldUpto;	}

	cpTo= rpTo->rpCells+ colTo;
	cpSet= rpSet->rpCells;
	for ( col= colTo; col < upto; cpTo++, cpSet++, col++ )
	    {
	    PropertyMask	cp_DoneMask;

	    utilPropMaskClear( &cp_DoneMask );

	    if  ( docUpdCellProperties( &cp_DoneMask, cpTo,
						cpSetMask, cpSet, dam ) )
		{ LDEB(1); return -1;	}

	    if  ( ! utilPropMaskIsEmpty( &cp_DoneMask ) )
		{
		PROPmaskADD( &rpDoneMask, RPprop_CELL_PROPS );
		utilPropMaskOr( &cpDoneMask, &cpDoneMask, &cp_DoneMask );
		}
	    }
	}

    for ( p= 0; p < DocRowIntPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];

	if  ( PROPmaskISSET( rpSetMask, prop ) )
	    {
	    int		valt= docGetRowProperty( rpTo, prop );
	    int		valf= docGetRowProperty( rpSet, prop );

	    if  ( valt != valf )
		{
		if  ( docSetRowProperty( rpTo, prop, valf ) )
		    { LLDEB(prop,valf); return -1;	}

		PROPmaskADD( &rpDoneMask, prop );
		}
	    }
	}

    for ( p= 0; p < DocRowBorderPropertyCount; p++ )
	{
	int	prop= DocRowBorderProperties[p];

	if  ( PROPmaskISSET( rpSetMask, prop ) )
	    {
	    int		valt= docGetRowProperty( rpTo, prop );
	    int		valf= docGetRowProperty( rpSet, prop );

	    if  ( valf >= 0 && dam && dam->damBorderMap )
		{ valf= dam->damBorderMap[valf];	}

	    if  ( valt != valf )
		{
		if  ( docSetRowProperty( rpTo, prop, valf ) )
		    { LLDEB(prop,valf); return -1;	}

		PROPmaskADD( &rpDoneMask, prop );
		}
	    }
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropSHADING ) )
	{
	int	fromNumber= rpSet->rpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( rpTo->rpShadingNumber != fromNumber )
	    {
	    rpTo->rpShadingNumber= fromNumber;
	    PROPmaskADD( &rpDoneMask, RPpropSHADING );
	    }
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropFRAME ) )
	{
	int	fromNumber= rpSet->rpFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( rpTo->rpFrameNumber != fromNumber )
	    {
	    rpTo->rpFrameNumber= fromNumber;
	    PROPmaskADD( &rpDoneMask, RPpropFRAME );
	    }
	}

    if  ( pRpDoneMask )
	{ utilPropMaskOr( pRpDoneMask, pRpDoneMask, &rpDoneMask );	}
    if  ( pCpDoneMask )
	{ utilPropMaskOr( pCpDoneMask, pCpDoneMask, &cpDoneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy row properties. As this routine is not called very often,	*/
/*  use part of the 'update' mechanism.					*/
/*									*/
/************************************************************************/

int docCopyRowProperties(	RowProperties *			rpTo,
				const RowProperties *		rpFrom,
				const DocumentAttributeMap *	dam )
    {
    int			col;
    int			p;

    CellProperties *	fresh;

    fresh= realloc( rpTo->rpCells,
			    rpFrom->rpCellCount* sizeof(CellProperties) );
    if  ( ! fresh )
	{ LXDEB(rpFrom->rpCellCount,fresh); return -1;	}
    rpTo->rpCells= fresh;
    rpTo->rpCellCount= rpFrom->rpCellCount;

    for ( col= 0; col < rpFrom->rpCellCount; col++ )
	{ fresh[col]= rpFrom->rpCells[col];	}

    for ( p= 0; p < DocRowIntPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];
	int	val= docGetRowProperty( rpFrom, prop );

	if  ( docSetRowProperty( rpTo, prop, val ) )
	    { LLDEB(prop,val);	}
	}

    for ( p= 0; p < DocRowBorderPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];
	int	val= docGetRowProperty( rpFrom, prop );

	if  ( docSetRowProperty( rpTo, prop, val ) )
	    { LLDEB(prop,val);	}
	}

    rpTo->rpShadingNumber= rpFrom->rpShadingNumber;
    rpTo->rpFrameNumber= rpFrom->rpFrameNumber;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a kind of 'default' row layout: Evenly distribute the		*/
/*  available width over the columns.					*/
/*									*/
/************************************************************************/

int docEqualWidthColumns(		RowProperties *		rp,
					int			columns,
					int			wide,
					int			fontHalfPoints )
    {
    int				rval= 0;
    int				col;

    CellProperties		cp;

    docInitCellProperties( &cp );
    cp.cpWide= wide/ columns;

    rp->rpHalfGapWidthTwips= 5* fontHalfPoints;
    rp->rpLeftIndentTwips=  -5* fontHalfPoints;

    for ( col= 0; col < columns; col++ )
	{
	if  ( docInsertRowColumn( rp, col, &cp,
					(const DocumentAttributeMap *)0 ) )
	    { LDEB(col); rval= -1; goto ready;	}
	}

  ready:

    /*docCleanCellProperties( &cp );*/

    return rval;
    }

int docSetRowProperty(		RowProperties *		rp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case RPpropGAP_WIDTH:
	    rp->rpHalfGapWidthTwips= arg;
	    break;

	case RPpropLEFT_INDENT:
	    rp->rpLeftIndentTwips= arg;
	    break;

	case RPpropHEIGHT:
	    rp->rpHeightTwips= arg;
	    break;

	case RPpropTOP_BORDER:
	    rp->rpTopBorderNumber= arg;
	    break;
	case RPpropBOTTOM_BORDER:
	    rp->rpBottomBorderNumber= arg;
	    break;
	case RPpropLEFT_BORDER:
	    rp->rpLeftBorderNumber= arg;
	    break;
	case RPpropRIGHT_BORDER:
	    rp->rpRightBorderNumber= arg;
	    break;
	case RPpropHORIZ_BORDER:
	    rp->rpHorizontalBorderNumber= arg;
	    break;
	case RPpropVERT_BORDER:
	    rp->rpVerticalBorderNumber= arg;
	    break;

	case RPpropSHADING:
	    rp->rpShadingNumber= arg;
	    break;
	case RPpropFRAME:
	    rp->rpFrameNumber= arg;
	    break;

	/**/
	case RPpropALIGNMENT:
	    rp->rpAlignment= arg;
	    break;

	case RPpropIS_TABLE_HEADER:
	    rp->rpIsTableHeader= ( arg != 0 );
	    break;
	case RPpropKEEP_ON_ONE_PAGE:
	    rp->rpKeepOnOnePage= ( arg != 0 );
	    break;
	case RPprop_KEEPFOLLOW:
	    rp->rp_Keepfollow= ( arg != 0 );
	    break;
	case RPpropAUTOFIT:
	    rp->rpAutofit= ( arg != 0 );
	    break;
	case RPpropRTOL:
	    rp->rpRToL= ( arg != 0 );
	    break;

	/**/
	case RPpropTRW_WIDTH:
	    rp->rpPreferredWidth= arg;
	    break;
	case RPpropTRFTS_WIDTH:
	    rp->rpPreferredWidthUnit= arg;
	    break;

	/**/
	case RPpropTRSPDL:
	    rp->rpLeftCellSpacing= arg;
	    break;
	case RPpropTRSPDR:
	    rp->rpRightCellSpacing= arg;
	    break;
	case RPpropTRSPDT:
	    rp->rpTopCellSpacing= arg;
	    break;
	case RPpropTRSPDB:
	    rp->rpBottomCellSpacing= arg;
	    break;

	case RPpropTRSPDFL:
	    rp->rpLeftCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFR:
	    rp->rpRightCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFT:
	    rp->rpTopCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFB:
	    rp->rpBottomCellSpacingUnit= arg;
	    break;

	/**/
	case RPpropTRPADDL:
	    rp->rpLeftCellPadding= arg;
	    break;
	case RPpropTRPADDR:
	    rp->rpRightCellPadding= arg;
	    break;
	case RPpropTRPADDT:
	    rp->rpTopCellPadding= arg;
	    break;
	case RPpropTRPADDB:
	    rp->rpBottomCellPadding= arg;
	    break;

	case RPpropTRPADDFL:
	    rp->rpLeftCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFR:
	    rp->rpRightCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFT:
	    rp->rpTopCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFB:
	    rp->rpBottomCellPaddingUnit= arg;
	    break;

	/**/
	case RPpropTRW_WIDTHB:
	    rp->rpCellWidthBefore= arg;
	    break;
	case RPpropTRW_WIDTHA:
	    rp->rpCellWidthAfter= arg;
	    break;

	case RPpropTRFTS_WIDTHB:
	    rp->rpCellWidthBeforeUnit= arg;
	    break;
	case RPpropTRFTS_WIDTHA:
	    rp->rpCellWidthAfterUnit= arg;
	    break;

	case RPpropAUTOFORMAT_BORDERS:
	    rp->rpAutoformatBorders= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_SHADING:
	    rp->rpAutoformatShading= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FONT:
	    rp->rpAutoformatFont= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_COLOR:
	    rp->rpAutoformatColor= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_APPLY_BEST_FIT:
	    rp->rpAutoformatApplyBestFit= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_ROW:
	    rp->rpAutoformatFirstRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_ROW:
	    rp->rpAutoformatLastRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_COLUMN:
	    rp->rpAutoformatFirstColumn= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_COLUMN:
	    rp->rpAutoformatLastColumn= ( arg != 0 );
	    break;

	case RPpropROW_NUMBER:
	    rp->rpRowNumber= arg;
	    break;
	case RPpropROW_BAND_NUMBER:
	    rp->rpRowBandNumber= arg;
	    break;
	case RPpropROW_STYLE:
	    rp->rpRowStyle= arg;
	    break;

	case RPpropIS_LAST_ROW:
	    rp->rpIsLastRow= ( arg != 0 );
	    break;

	case RPpropTRAUTH:
	    rp->rpAuthor= arg;
	    break;

	case RPpropTRFLAT:
	    rp->rpFlatInPDF= arg != 0;
	    break;

	case RPprop_CELL_COUNT:
	case RPprop_CELL_PROPS:
	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetRowProperty(		const RowProperties *	rp,
				int			prop )
    {
    switch( prop )
	{
	case RPpropGAP_WIDTH:
	    return rp->rpHalfGapWidthTwips;

	case RPpropLEFT_INDENT:
	    return rp->rpLeftIndentTwips;

	case RPpropHEIGHT:
	    return rp->rpHeightTwips;

	case RPpropTOP_BORDER:
	    return rp->rpTopBorderNumber;
	case RPpropBOTTOM_BORDER:
	    return rp->rpBottomBorderNumber;
	case RPpropLEFT_BORDER:
	    return rp->rpLeftBorderNumber;
	case RPpropRIGHT_BORDER:
	    return rp->rpRightBorderNumber;
	case RPpropHORIZ_BORDER:
	    return rp->rpHorizontalBorderNumber;
	case RPpropVERT_BORDER:
	    return rp->rpVerticalBorderNumber;

	case RPpropSHADING:
	    return rp->rpShadingNumber;
	case RPpropFRAME:
	    return rp->rpFrameNumber;

	/**/
	case RPpropALIGNMENT:
	    return rp->rpAlignment;

	case RPpropIS_TABLE_HEADER:
	    return rp->rpIsTableHeader;
	case RPpropKEEP_ON_ONE_PAGE:
	    return rp->rpKeepOnOnePage;
	case RPprop_KEEPFOLLOW:
	    return rp->rp_Keepfollow;
	case RPpropAUTOFIT:
	    return rp->rpAutofit;
	case RPpropRTOL:
	    return rp->rpRToL;

	/**/
	case RPpropTRW_WIDTH:
	    return rp->rpPreferredWidth;
	case RPpropTRFTS_WIDTH:
	    return rp->rpPreferredWidthUnit;

	/**/
	case RPpropTRSPDL:
	    return rp->rpLeftCellSpacing;
	case RPpropTRSPDR:
	    return rp->rpRightCellSpacing;
	case RPpropTRSPDT:
	    return rp->rpTopCellSpacing;
	case RPpropTRSPDB:
	    return rp->rpBottomCellSpacing;

	case RPpropTRSPDFL:
	    return rp->rpLeftCellSpacingUnit;
	case RPpropTRSPDFR:
	    return rp->rpRightCellSpacingUnit;
	case RPpropTRSPDFT:
	    return rp->rpTopCellSpacingUnit;
	case RPpropTRSPDFB:
	    return rp->rpBottomCellSpacingUnit;

	/**/
	case RPpropTRPADDL:
	    return rp->rpLeftCellPadding;
	case RPpropTRPADDR:
	    return rp->rpRightCellPadding;
	case RPpropTRPADDT:
	    return rp->rpTopCellPadding;
	case RPpropTRPADDB:
	    return rp->rpBottomCellPadding;

	case RPpropTRPADDFL:
	    return rp->rpLeftCellPaddingUnit;
	case RPpropTRPADDFR:
	    return rp->rpRightCellPaddingUnit;
	case RPpropTRPADDFT:
	    return rp->rpTopCellPaddingUnit;
	case RPpropTRPADDFB:
	    return rp->rpBottomCellPaddingUnit;

	/**/
	case RPpropTRW_WIDTHB:
	    return rp->rpCellWidthBefore;
	case RPpropTRW_WIDTHA:
	    return rp->rpCellWidthAfter;

	case RPpropTRFTS_WIDTHB:
	    return rp->rpCellWidthBeforeUnit;
	case RPpropTRFTS_WIDTHA:
	    return rp->rpCellWidthAfterUnit;

	case RPpropAUTOFORMAT_BORDERS:
	    return rp->rpAutoformatBorders;
	case RPpropAUTOFORMAT_SHADING:
	    return rp->rpAutoformatShading;
	case RPpropAUTOFORMAT_FONT:
	    return rp->rpAutoformatFont;
	case RPpropAUTOFORMAT_COLOR:
	    return rp->rpAutoformatColor;
	case RPpropAUTOFORMAT_APPLY_BEST_FIT:
	    return rp->rpAutoformatApplyBestFit;
	case RPpropAUTOFORMAT_FIRST_ROW:
	    return rp->rpAutoformatFirstRow;
	case RPpropAUTOFORMAT_LAST_ROW:
	    return rp->rpAutoformatLastRow;
	case RPpropAUTOFORMAT_FIRST_COLUMN:
	    return rp->rpAutoformatFirstColumn;
	case RPpropAUTOFORMAT_LAST_COLUMN:
	    return rp->rpAutoformatLastColumn;

	case RPpropROW_NUMBER:
	    return rp->rpRowNumber;
	case RPpropROW_BAND_NUMBER:
	    return rp->rpRowBandNumber;
	case RPpropROW_STYLE:
	    return rp->rpRowStyle;

	case RPpropIS_LAST_ROW:
	    return rp->rpIsLastRow;

	case RPpropTRAUTH:
	    return rp->rpAuthor;

	case RPpropTRFLAT:
	    return rp->rpFlatInPDF;

	case RPprop_CELL_COUNT:
	    return rp->rpCellCount;

	case RPprop_CELL_PROPS:
	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

void docRowPropertiesSetCellDefaults(
				CellProperties *		cp,
				const RowProperties *		rp )
    {
    cp->cpTopPadding= rp->rpTopCellPadding;
    cp->cpLeftPadding= rp->rpLeftCellPadding;
    cp->cpRightPadding= rp->rpRightCellPadding;
    cp->cpBottomPadding= rp->rpBottomCellPadding;

    cp->cpTopPaddingUnit= rp->rpTopCellPaddingUnit;
    cp->cpLeftPaddingUnit= rp->rpLeftCellPaddingUnit;
    cp->cpRightPaddingUnit= rp->rpRightCellPaddingUnit;
    cp->cpBottomPaddingUnit= rp->rpBottomCellPaddingUnit;

    return;
    }
