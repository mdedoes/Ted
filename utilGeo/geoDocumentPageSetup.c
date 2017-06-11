#   include	"utilGeoConfig.h"

#   include	"geoDocumentPageSetup.h"
#   include	"geoRectangle.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set default document geometry.					*/
/*									*/
/************************************************************************/

void geoInitDocumentGeometry(	DocumentGeometry *	dg )
    {
    dg->dgPageWideTwips= 12240;
    dg->dgPageHighTwips= 15840;

    dg->dgMargins.roLeftOffset= 1800;
    dg->dgMargins.roTopOffset= 1440;
    dg->dgMargins.roRightOffset= 1800;
    dg->dgMargins.roBottomOffset= 1440;

    dg->dgHeaderPositionTwips= 720;
    dg->dgFooterPositionTwips= 720;

    dg->dgGutterTwips= 0;
    dg->dgMirrorMargins= 0;
    }

/************************************************************************/
/*									*/
/*  Copy a document Geometry.						*/
/*									*/
/************************************************************************/

void geoUpdDocumentGeometry(	PropertyMask *			dpDoneMask,
				DocumentGeometry *		dgTo,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet )
    {
    PropertyMask		doneMask;

    utilPropMaskClear( &doneMask );

    if  ( PROPmaskISSET( dgSetMask, DGpropPAGE_WIDTH ) )
	{
	if  ( dgTo->dgPageWideTwips != dgSet->dgPageWideTwips )
	    {
	    dgTo->dgPageWideTwips= dgSet->dgPageWideTwips;
	    PROPmaskADD( &doneMask, DGpropPAGE_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropPAGE_HEIGHT ) )
	{
	if  ( dgTo->dgPageHighTwips != dgSet->dgPageHighTwips )
	    {
	    dgTo->dgPageHighTwips= dgSet->dgPageHighTwips;
	    PROPmaskADD( &doneMask, DGpropPAGE_HEIGHT );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropLEFT_MARGIN ) )
	{
	if  ( dgTo->dgMargins.roLeftOffset != dgSet->dgMargins.roLeftOffset )
	    {
	    dgTo->dgMargins.roLeftOffset= dgSet->dgMargins.roLeftOffset;
	    PROPmaskADD( &doneMask, DGpropLEFT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropRIGHT_MARGIN ) )
	{
	if  ( dgTo->dgMargins.roRightOffset != dgSet->dgMargins.roRightOffset )
	    {
	    dgTo->dgMargins.roRightOffset= dgSet->dgMargins.roRightOffset;
	    PROPmaskADD( &doneMask, DGpropRIGHT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropTOP_MARGIN ) )
	{
	if  ( dgTo->dgMargins.roTopOffset != dgSet->dgMargins.roTopOffset )
	    {
	    dgTo->dgMargins.roTopOffset= dgSet->dgMargins.roTopOffset;
	    PROPmaskADD( &doneMask, DGpropTOP_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropBOTTOM_MARGIN ) )
	{
	if  ( dgTo->dgMargins.roBottomOffset != dgSet->dgMargins.roBottomOffset )
	    {
	    dgTo->dgMargins.roBottomOffset= dgSet->dgMargins.roBottomOffset;
	    PROPmaskADD( &doneMask, DGpropBOTTOM_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropHEADER_POSITION ) )
	{
	if  ( dgTo->dgHeaderPositionTwips != dgSet->dgHeaderPositionTwips )
	    {
	    dgTo->dgHeaderPositionTwips= dgSet->dgHeaderPositionTwips;
	    PROPmaskADD( &doneMask, DGpropHEADER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropFOOTER_POSITION ) )
	{
	if  ( dgTo->dgFooterPositionTwips != dgSet->dgFooterPositionTwips )
	    {
	    dgTo->dgFooterPositionTwips= dgSet->dgFooterPositionTwips;
	    PROPmaskADD( &doneMask, DGpropFOOTER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropGUTTER ) )
	{
	if  ( dgTo->dgGutterTwips != dgSet->dgGutterTwips )
	    {
	    dgTo->dgGutterTwips= dgSet->dgGutterTwips;
	    PROPmaskADD( &doneMask, DGpropGUTTER );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropMARGMIR ) )
	{
	if  ( dgTo->dgMirrorMargins != dgSet->dgMirrorMargins )
	    {
	    dgTo->dgMirrorMargins= dgSet->dgMirrorMargins;
	    PROPmaskADD( &doneMask, DGpropMARGMIR );
	    }
	}

    if  ( dpDoneMask )
	{ utilPropMaskOr( dpDoneMask, dpDoneMask, &doneMask );	}

    return;
    }

void geoDocumentGeometryGetPageRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= 0;
    dr->drX1= dg->dgPageWideTwips;
    dr->drY0= 0;
    dr->drY1= dg->dgPageHighTwips;
    }

void geoDocumentGeometryGetBodyRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgMargins.roLeftOffset;
    dr->drX1= dg->dgPageWideTwips- dg->dgMargins.roRightOffset;
    dr->drY0= dg->dgMargins.roTopOffset;
    dr->drY1= dg->dgPageHighTwips- dg->dgMargins.roBottomOffset;
    }

void geoDocumentGeometryGetHeaderRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgMargins.roLeftOffset;
    dr->drX1= dg->dgPageWideTwips- dg->dgMargins.roRightOffset;
    dr->drY0= dg->dgHeaderPositionTwips;
    dr->drY1= dg->dgMargins.roTopOffset;
    }

void geoDocumentGeometryGetFooterRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgMargins.roLeftOffset;
    dr->drX1= dg->dgPageWideTwips- dg->dgMargins.roRightOffset;
    dr->drY0= dg->dgPageHighTwips- dg->dgMargins.roBottomOffset;
    dr->drY1= dg->dgPageHighTwips- dg->dgFooterPositionTwips;
    }

void geoDocumentGeometryGetPageBoundingBox(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg,
				int				hasHeader,
				int				hasFooter )
    {
    geoDocumentGeometryGetBodyRect( dr, dg );

    if  ( hasHeader )
	{
	DocumentRectangle	drHead;

	geoDocumentGeometryGetHeaderRect( &drHead, dg );
	geoUnionRectangle( dr, dr, &drHead );
	}

    if  ( hasFooter )
	{
	DocumentRectangle	drFoot;

	geoDocumentGeometryGetFooterRect( &drFoot, dg );
	geoUnionRectangle( dr, dr, &drFoot );
	}
    }

void geoOverridePaperSize(	DocumentGeometry *		dgTo,
				const DocumentGeometry *	dgFrom )
    {
    if  ( dgTo->dgPageWideTwips > dgTo->dgPageHighTwips		&&
	  dgFrom->dgPageHighTwips > dgFrom->dgPageWideTwips	)
	{
	dgTo->dgPageWideTwips= dgFrom->dgPageHighTwips;
	dgTo->dgPageHighTwips= dgFrom->dgPageWideTwips;
	}
    else{
	dgTo->dgPageWideTwips= dgFrom->dgPageWideTwips;
	dgTo->dgPageHighTwips= dgFrom->dgPageHighTwips;
	}
    }

