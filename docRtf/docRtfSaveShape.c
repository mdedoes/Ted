/************************************************************************/
/*									*/
/*  Save a shape to RTF							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>
#   include	<string.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	"docRtfFlags.h"
#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	"docRtfFindProperty.h"
#   include	<utilPropMask.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<sioGeneral.h>
#   include	<docShapeProp.h>
#   include	<docObjectProperties.h>
#   include	<sioHexedMemory.h>
#   include	<geoUnits.h>
#   include	<geo2DInteger.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docRtfSaveShapeProperties(
				RtfWriter *			rw,
				const struct ShapeProperties *	sp )
    {
    ShapeProperties	spRef;

    PropertyMask	spCmpMask;
    PropertyMask	spDifMask;

    utilPropMaskClear( &spCmpMask );
    utilPropMaskFill( &spCmpMask, SHPprop_COUNT );
    utilPropMaskClear( &spDifMask );

    docInitShapeProperties( &spRef );
    docShapePropertyDifference( &spDifMask, sp, &spCmpMask, &spRef );

    /* Save these anyway */
    PROPmaskADD( &spDifMask, SHPpropWR );
    PROPmaskADD( &spDifMask, SHPpropWRK );

    if  ( docRtfWriteAllItemProperties( rw, RTCscopeROOT_SHAPE,
				(void *)sp,
				(RtfGetIntProperty)docGetShapeProperty,
				&spDifMask, SHPprop_COUNT ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRtfStartShapeProperty(
				RtfWriter *			rw,
				const char *			sn )
    {
    docRtfWriteDestinationBegin( rw, "sp" );
    docRtfWriteDestinationBegin( rw, "sn" );

    docRtfWriteDocEncodedString( rw, sn, strlen( sn ) );

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationBegin( rw, "sv" );

    return 0;
    }

static int docRtfSaveShapeProperty( RtfWriter *		rw,
			    int				prop,
			    long			value )
    {
    const int			scope= RTCscopeSHAPE;
    const RtfControlWord *	rcw;
    char			scratch[30];

    rcw= docRtfFindShapeProperty( scope, prop, 0 );
    if  ( ! rcw )
	{ LLXDEB(scope,prop,rcw); return -1;	}

    if  ( docRtfStartShapeProperty( rw, rcw->rcwWord ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    sprintf( scratch, "%ld", value );
    docRtfWriteDocEncodedString( rw, scratch, strlen( scratch ) );

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

static int docRtfSaveShapeVertices(
				RtfWriter *			rw,
				const char *			sn,
				const Point2DI *		points,
				int				pointCount )
    {
    int		i;
    char	scratch[30+1];

    if  ( docRtfStartShapeProperty( rw, sn ) )
	{ SDEB(sn); return -1;	}

    sprintf( scratch, "8;%d", pointCount );
    docRtfWriteDocEncodedString( rw, scratch, strlen(scratch) );

    for ( i= 0; i < pointCount; i++ )
	{
	sprintf( scratch, ";(%d,%d)", points[i].x, points[i].y );
	docRtfWriteDocEncodedString( rw, scratch, strlen(scratch) );
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

static int docRtfSaveShapeNumbers(
				RtfWriter *			rw,
				const char *			sn,
				const int *			numbers,
				int				numberCount )
    {
    int		i;
    char	scratch[30+1];
    int		size= 2;

    if  ( docRtfStartShapeProperty( rw, sn ) )
	{ SDEB(sn); return -1;	}

    for ( i= 0; i < numberCount; i++ )
	{
	if  ( numbers[i] >= 65536 )
	    { size= 4; break;	}
	}

    sprintf( scratch, "%d;%d", size, numberCount );
    docRtfWriteDocEncodedString( rw, scratch, strlen(scratch) );

    for ( i= 0; i < numberCount; i++ )
	{
	sprintf( scratch, ";%d", numbers[i] );
	docRtfWriteDocEncodedString( rw, scratch, strlen(scratch) );
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

int docRtfSaveShapeString(	RtfWriter *		rw,
				const char *		sn,
				const MemoryBuffer *	mb )
    {
    if  ( docRtfStartShapeProperty( rw, sn ) )
	{ SDEB(sn); return -1;	}

    if  ( docRtfWriteDocEncodedString( rw,
				(const char *)(mb->mbBytes), mb->mbSize ) )
	{ SLDEB(sn,mb->mbSize); return -1;	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

static int docRtfSaveShapeDrawing(
				RtfWriter *			rw,
				const struct ShapeDrawing *	sd )
    {
    int			rval= 0;
    ShapeDrawing	sdRef;
    int			prop;

    docInitShapeDrawing( &sdRef );
    docInitShapeDrawingAllocated( &sdRef );

    if  ( docRtfSaveShapeProperty( rw, DSHPprop_shapeType,
		    docGetShapeDrawingProperty( sd, DSHPprop_shapeType ) ) )
	{ LDEB(DSHPprop_shapeType);	}

    for ( prop= DSHPprop__FIRST_NUMERIC; prop < DSHPprop__COUNT; prop++ )
	{
	long	val;
	long	ref;

	if  ( prop == DSHPprop_shapeType )
	    { continue;	}

	val= docGetShapeDrawingProperty( sd, prop );
	ref= docGetShapeDrawingProperty( &sdRef, prop );

	if  ( val != ref )
	    {
	    if  ( docRtfSaveShapeProperty( rw, prop, val ) )
		{ LLDEB(prop,val); rval= -1; goto ready;	}
	    }
	}

    if  ( sd->sdVertexCount > 0 )
	{
	if  ( docRtfSaveShapeVertices( rw, "pVerticies",
					sd->sdVertices,
					sd->sdVertexCount ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sd->sdWrapPolygonVertexCount > 0 )
	{
	if  ( docRtfSaveShapeVertices( rw, "pWrapPolygonVertices",
					sd->sdWrapPolygonVertices,
					sd->sdWrapPolygonVertexCount ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sd->sdSegmentInfoCount > 0 )
	{
	if  ( docRtfSaveShapeNumbers( rw, "pSegmentInfo",
					sd->sdSegmentInfos,
					sd->sdSegmentInfoCount ) )
	    { LDEB(1); return -1;	}
	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_wzName) )			&&
	  docRtfSaveShapeString( rw, "wzName", &(sd->sd_wzName) )	)
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_wzDescription) )		&&
	  docRtfSaveShapeString( rw, "wzDescription",
					&(sd->sd_wzDescription) )	)
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_gtextUNICODE) )		&&
	  docRtfSaveShapeString( rw, "gtextUNICODE",
					&(sd->sd_gtextUNICODE) )	)
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_gtextFont) )		&&
	  docRtfSaveShapeString( rw, "gtextFont", &(sd->sd_gtextFont) )	)
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_pibName) )		&&
	  docRtfSaveShapeString( rw, "pibName", &(sd->sd_pibName) )	)
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(sd->sd_fillBlipName) )		&&
	  docRtfSaveShapeString( rw, "fillBlipName",
					&(sd->sd_fillBlipName) )	)
	{ LDEB(1); return -1;	}

  ready:
    
    docCleanShapeDrawing( &sdRef );

    return rval;
    }

static int docRtfSaveBehindShapeImage(
				RtfWriter *			rw,
				const DrawingShape *		ds )
    {
    int				rval= 0;
    const DocumentRectangle *	drShape= &(ds->dsShapeProperties.spRect);

    RasterImage			ri;
    PictureProperties		pip;
    MemoryBuffer		mbImage;

    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;

    bmInitRasterImage( &ri );
    docInitPictureProperties( &pip );
    utilInitMemoryBuffer( &mbImage );

    if  ( bmTransparentImage( &ri, BMcoRGB8PALETTE, 1, 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    pip.pipType= DOCokPICTPNGBLIP;
    pip.pipTwipsWide= drShape->drX1- drShape->drX0;
    pip.pipTwipsHigh= drShape->drY1- drShape->drY0;
    /* Only for windows bitmaps:
    pip.pipBmBitsPerPixel= ri.riDescription.bdBitsPerPixel;
    pip.pipBmPlanes= 1;
    pip.pipBmBytesPerRow= ri.riDescription.bdBytesPerRow;
    */

    /* Get resolution approximately right */
    ri.riDescription.bdUnit= BMunM;
    ri.riDescription.bdXResolution=
	    ( TWIPS_PER_M* ri.riDescription.bdPixelsWide ) / pip.pipTwipsWide;
    ri.riDescription.bdYResolution=
	    ( TWIPS_PER_M* ri.riDescription.bdPixelsHigh ) / pip.pipTwipsHigh;

    sosData= sioOutHexedMemoryOpen( &mbImage );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    if  ( bmPngWritePng( &(ri.riDescription), ri.riBytes, sosData ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutClose( sosData ); sosData= (SimpleOutputStream *)0;

    if  ( docRtfSavePicture( rw, &pip, &(ds->dsDrawing.sd_wzDescription), &mbImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sosData )
	{ sioOutClose( sosData );	}

    utilInitMemoryBuffer( &mbImage );
    /*docCleanPictureProperties( &pip );*/
    bmCleanRasterImage( &ri );

    return rval;
    }

int docRtfSaveDrawingShape(	RtfWriter *		rw,
				struct DrawingShape *	ds )
    {
    int		ch;

    const int	evenIfAbsent= 0;
    const int	forcePar= 0;

    if  ( ds->dsDrawing.sdShapeType == SHPtyGROUP )
	{ docRtfWriteDestinationBegin( rw, "shpgrp" );	}
    else{ docRtfWriteDestinationBegin( rw, "shp" );	}

    docRtfWriteDestinationBegin( rw, "*\\shpinst" );

    if  ( ! ds->dsIsChildShape )
	{
	if  ( docRtfSaveShapeProperties( rw, &(ds->dsShapeProperties) ) )
	    { LDEB(ds->dsIsChildShape); return -1;	}
	docRtfWriteNextLine( rw );
	}

    if  ( docRtfSaveShapeDrawing( rw, &(ds->dsDrawing) ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(ds->dsPictureData) ) )
	{
	docRtfStartShapeProperty( rw, "pib" );

	if  ( docRtfSavePicture( rw, &(ds->dsPictureProperties),
						    &(ds->dsDrawing.sd_wzDescription),
						    &(ds->dsPictureData ) ) )
	    { LDEB(1); return -1;	}

	docRtfWriteDestinationEnd( rw );
	docRtfWriteDestinationEnd( rw );
	docRtfWriteNextLine( rw );
	}

    if  ( docRtfSaveDocumentTree( rw, "shptxt", &(ds->dsDocumentTree),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    for ( ch= 0; ch < ds->dsChildCount; ch++ )
	{
	if  ( docRtfSaveDrawingShape( rw, ds->dsChildren[ch] ) )
	    { LDEB(ch); return -1;	}
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );

    if  ( ds->dsDrawing.sd_fPseudoInline )
	{
	if  ( docRtfSaveBehindShapeImage( rw, ds ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }
