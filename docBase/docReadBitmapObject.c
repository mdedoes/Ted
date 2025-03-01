/************************************************************************/
/*									*/
/*  Obtain the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<stdio.h>

#   include	<bmio.h>
#   include	<sioFileio.h>
#   include	<sioHexedMemory.h>

#   include	"docObject.h"
#   include	"docObjectIo.h"
#   include	"docObjectProperties.h"
#   include	<bmObjectReader.h>
#   include	<drawMetafile.h>
#   include	<sioGeneral.h>
#   include	<sioUtil.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a raster image file.						*/
/*									*/
/************************************************************************/

int docReadRasterObject(	InsertedObject *	io,
				const MemoryBuffer *	filename )
    {
    int				rval= 0;
    int				res;
    RasterImage			ri;
    const char *		contentType= "?";
    int				copyAsFile= 0;
    int				includedAsRaster= 0;

    PictureProperties *		pip= &(io->ioPictureProperties);

    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;
    SimpleInputStream *		sisFile= (SimpleInputStream *)0;

    bmInitRasterImage( &ri );

    res= bmRead( filename,
		&(ri.riBytes), &(ri.riDescription), &(ri.riFormat) );
    if  ( res )
	{
	SLDEB(utilMemoryBufferGetString(filename),res);
	rval= -1; goto ready;
	}

    if  ( ri.riFormat >= 0 )
	{ contentType= bmGetContentTypeOfFormat( ri.riFormat );	}

    io->ioKind= DOCokPICTPNGBLIP;
    io->ioInline= 1;

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
							&(ri.riDescription) );

    pip->pipTwipsWide= io->ioTwipsWide;
    pip->pipTwipsHigh= io->ioTwipsHigh;

    io->ioResultData.mbSize= 0;
    sosData= sioOutHexedMemoryOpen( &(io->ioObjectData) );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    if  ( ! strcmp( contentType, "image/jpeg" ) )
	{
	io->ioKind= DOCokPICTJPEGBLIP;
	pip->pipType= DOCokPICTJPEGBLIP;
	copyAsFile= 1;
	}

    if  ( ! strcmp( contentType, "image/png" ) )
	{
	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	copyAsFile= 1;
	}

    if  ( copyAsFile )
	{
	sisFile= sioInFileioOpen( filename );
	if  ( ! sisFile )
	    { XDEB(sisFile); rval= -1; goto ready; }

	if  ( sioCopyStream( sosData, sisFile ) )
	    { LDEB(1); rval= -1; goto ready; }

	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster						&&
	  bmCanSaveAsContentType( &(ri.riDescription), "image/png" )	)
	{
	if  ( bmPngWritePng( &(ri.riDescription), ri.riBytes, sosData ) )
	    { LDEB(1); rval= -1; goto ready;	}

	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster )
	{
	if  ( bmWmfWriteWmf( &(ri.riDescription), ri.riBytes, sosData ) )
	    { LDEB(1); rval= -1; goto ready;	}

	pip->pipMetafileIsRaster= 1;
	pip->pipMetafileBitmapBpp= ri.riDescription.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	pip->pipType= DOCokPICTWMETAFILE;
	pip->pipMapMode= 8;
	}

  ready:

    bmCleanRasterImage( &ri );

    if  ( sisFile )
	{ sioInClose( sisFile );	}
    if  ( sosData )
	{ sioOutClose( sosData );	}

    return rval;
    }

int docReadRasterSize(		InsertedObject *	io )
    {
    int				rval= 0;
    PictureProperties *		pip= &(io->ioPictureProperties);

    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObject( io ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
					&(io->ioRasterImage.riDescription) );

    pip->pipTwipsWide= io->ioTwipsWide;
    pip->pipTwipsHigh= io->ioTwipsHigh;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

int docGetRasterImageForObjectData(	int			kind,
					RasterImage *		ri,
					const MemoryBuffer *	mb )
    {
    switch( kind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( bmReadRasterObject( ri, appMetaPlayWmfImg, "wmf", mb ) )
		{ LDEB(kind); return -1;	}
	    break;

	case DOCokPICTPNGBLIP:
	    if  ( bmReadRasterObject( ri, bmPngReadPng, "png", mb )	)
		{ LDEB(kind); return -1;	}
	    break;

	case DOCokPICTJPEGBLIP:
	    if  ( bmReadRasterObject( ri, bmJpegReadJfif, "jpg", mb )	)
		{ LDEB(kind); return -1;	}

	    break;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(kind); return -1;
	}

    return 0;
    }

int docGetRasterImageForObject(	InsertedObject *	io )
    {
    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioRasterImage.riBytes				&&
		  docGetRasterImageForObjectData( io->ioKind,
			&(io->ioRasterImage), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioRasterImage.riBytes )
		{ io->ioPictureProperties.pipMetafileIsRaster= 1;	}

	    break;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioRasterImage.riBytes				&&
		  docGetRasterImageForObjectData( io->ioKind,
			&(io->ioRasterImage), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    break;

	case DOCokOLEOBJECT:
	    switch( io->ioResultKind )
		{
		case DOCokPICTWMETAFILE:
		    if  ( ! io->ioRasterImage.riBytes			&&
			  docGetRasterImageForObjectData( io->ioResultKind,
				&(io->ioRasterImage), &(io->ioResultData) ) )
			{ LDEB(1); return 0;	}

		    if  ( io->ioRasterImage.riBytes )
			{ io->ioPictureProperties.pipMetafileIsRaster= 1; }

		    return 0;

		case DOCokPICTPNGBLIP:
		case DOCokPICTJPEGBLIP:
		    if  ( ! io->ioRasterImage.riBytes			&&
			  docGetRasterImageForObjectData( io->ioResultKind,
				&(io->ioRasterImage), &(io->ioResultData) ) )
			{ LDEB(1); return 0;	}

		    return 0;

		default:
		    LLDEB(io->ioKind,io->ioResultKind); return 0;
		}
	    return 0;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

/************************************************************************/

int docSaveRasterBytesToObject(	InsertedObject *	io,
				RasterImage *		ri )
    {
    int				rval= 0;

    PictureProperties *		pip;

    MemoryBuffer		mb;
    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;

    int				includedAsRaster= 0;

    const char *		contentType="?";

    utilInitMemoryBuffer( &mb );

    pip= &(io->ioPictureProperties);

    sosData= sioOutHexedMemoryOpen( &mb );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    if  ( ri->riFormat >= 0 )
	{ contentType= bmGetContentTypeOfFormat( ri->riFormat );	}

    if  ( ! includedAsRaster			&&
	  ! strcmp( contentType, "image/png" )	)
	{
	if  ( bmPngWritePng( &(ri->riDescription), ri->riBytes, sosData ) )
	    { LDEB(1); rval= -1; goto ready;	}

	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster			&&
	  ! strcmp( contentType, "image/jpeg" )	)
	{
	if  ( bmJpegWriteJfif( &ri->riDescription, ri->riBytes, sosData ) )
	    { LDEB(1); rval= -1; goto ready;	}

	io->ioKind= DOCokPICTJPEGBLIP;
	pip->pipType= DOCokPICTJPEGBLIP;
	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster )
	{
	if  ( bmWmfWriteWmf( &(ri->riDescription), ri->riBytes, sosData ) )
	    { LDEB(1); rval= -1; goto ready;	}

	pip->pipMetafileIsRaster= 1;
	pip->pipMetafileBitmapBpp= ri->riDescription.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	pip->pipType= DOCokPICTWMETAFILE;
	pip->pipMapMode= 8;
	}

    sioOutClose( sosData ); sosData= (SimpleOutputStream *)0;

    bmImageSizeTwips( &(pip->pipTwipsWide), &(pip->pipTwipsHigh),
							&(ri->riDescription) );
    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    io->ioObjectData= mb; /* steal */
    utilInitMemoryBuffer( &mb );

    bmCleanRasterImage( &(io->ioRasterImage) );
    io->ioRasterImage= *ri; /* steal */
    bmInitRasterImage( ri );
    bmFreeRasterImage( ri );

  ready:

    utilCleanMemoryBuffer( &mb );

    if  ( sosData )
	{ sioOutClose( sosData );	}

    return rval;
    }

