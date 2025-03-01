/************************************************************************/
/*									*/
/*  Save the images in a document and include references in the	HTML	*/
/*  file.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	<drawMetafileSvg.h>
#   include	<docLayoutObject.h>
#   include	"docHtmlWriteDrawingShape.h"
#   include	"docHtmlWriteMetafile.h"

#   include	<sioGeneral.h>
#   include	<sioUtil.h>

#   include	<docBuf.h>
#   include	<docShape.h>
#   include	"docHtmlWriteImpl.h"

#   include	<docObjectProperties.h>
#   include	<sioHexedMemory.h>
#   include	<docObjectIo.h>
#   include	<docObject.h>
#   include	<docShapeGeometry.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static void docHtmlSetSaveAsMetaSvg(
				int *				pUseDataUrl,
				bmWriteBitmap *			pWriteRaster,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pUseDataUrl= 0;
    *pWriteRaster= (bmWriteBitmap)0;
    *pMimeType= "image/svg+xml";
    *pExt= "svg";
    return;
    }

static void docHtmlSetSaveAsRasterFromData(
				const HtmlWritingContext *	hwc,
				int *				pUseDataUrl,
				MetafileWriteSvg *		pWriteSvg )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    *pUseDataUrl= hws->hwsInlineImages;
    *pWriteSvg= (MetafileWriteSvg)0;
    return;
    }

static void docHtmlSetSaveAsJfif(
				bmWriteBitmap *			pWriteRaster,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pWriteRaster= bmJpegWriteJfif;
    *pMimeType= "image/jpeg";
    *pExt= "jpg";
    return;
    }

static void docHtmlSetSaveAsPng(
				bmWriteBitmap *			pWriteRaster,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pWriteRaster= bmPngWritePng;
    *pMimeType= "image/png";
    *pExt= "png";
    return;
    }

int docHtmlObjectSaveHow(	const HtmlWritingContext *	hwc,
				int *				pType,
				int *				pUseDataUrl,
				bmWriteBitmap *			pWriteRaster,
				MetafileWriteSvg *		pWriteSvg,
				const char **			pMimeType,
				const char **			pExt,
				const MemoryBuffer **		pObjectData,
				const InsertedObject *		io )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    const RasterImage *		ri= &(io->ioRasterImage);

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMetaPlayWmfSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTEMFBLIP:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMetaPlayEmfSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokMACPICT:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMacPictPlayFileSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTJPEGBLIP:
	    docHtmlSetSaveAsRasterFromData( hwc, pUseDataUrl, pWriteSvg );
	    docHtmlSetSaveAsJfif( pWriteRaster, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTPNGBLIP:
	    docHtmlSetSaveAsRasterFromData( hwc, pUseDataUrl, pWriteSvg );
	    docHtmlSetSaveAsPng( pWriteRaster, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokDRAWING_SHAPE:
	    *pType= io->ioKind;
	    *pUseDataUrl= 0;
	    *pWriteRaster= (bmWriteBitmap)0;
	    *pWriteSvg= (MetafileWriteSvg)0;
	    *pMimeType= "image/svg+xml";
	    *pExt= "svg";
	    *pObjectData= (const MemoryBuffer *)0;
	    return 0;

	case DOCokOLEOBJECT:
	    switch( io->ioResultKind )
		{
		case DOCokPICTWMETAFILE:
		    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMetaPlayWmfSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTEMFBLIP:
		    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMetaPlayEmfSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokMACPICT:
		    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteRaster, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMacPictPlayFileSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTJPEGBLIP:
		    docHtmlSetSaveAsRasterFromData(
					    hwc, pUseDataUrl, pWriteSvg );
		    docHtmlSetSaveAsJfif( pWriteRaster, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTPNGBLIP:
		    docHtmlSetSaveAsRasterFromData(
					    hwc, pUseDataUrl, pWriteSvg );
		    docHtmlSetSaveAsPng( pWriteRaster, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pObjectData= &(io->ioResultData);
		    return 0;
		}

	    break;
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ return 1;	}

#   if  USE_PNG
    if  ( ri->riDescription.bdColorEncoding == BMcoRGB8PALETTE		&&
	  bmCanSaveAsContentType( &(ri->riDescription), "image/png" )	)
	{
	*pType= DOCokPICTPNGBLIP;
	*pUseDataUrl= hws->hwsInlineImages;
	*pWriteRaster= bmPngWritePng;
	*pWriteSvg= (MetafileWriteSvg)0;
	*pMimeType= "image/png";
	*pExt= "png";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

#   if  USE_GIF
    if  ( bmCanSaveAsContentType( &(ri->riDescription), "image/gif" ) )
	{
	*pType= DOCokGIF_FILE;
	*pUseDataUrl= hws->hwsInlineImages;
	*pWriteRaster= bmGifWriteGif;
	*pWriteSvg= (MetafileWriteSvg)0;
	*pMimeType= "image/gif";
	*pExt= "gif";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

    if  ( bmCanSaveAsContentType( &(ri->riDescription), "image/jpeg" ) )
	{
	*pType= DOCokPICTJPEGBLIP;
	*pUseDataUrl= hws->hwsInlineImages;
	*pWriteRaster= bmJpegWriteJfif;
	*pWriteSvg= (MetafileWriteSvg)0;
	*pMimeType= "image/jpeg";
	*pExt= "jpg";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Emit the 'img' element for an image.				*/
/*									*/
/************************************************************************/

static int docHtmlStartImgElement(
				HtmlWritingContext *		hwc,
				const struct BufferItem *	paraNode,
				const InsertedObject *		io,
				int				wide,
				int				high )
    {
    int				rval= 0;

    docHtmlPutString( "<img", hwc );

    docHtmlWriteIntAttribute( hwc, "width", wide );
    docHtmlWriteIntAttribute( hwc, "height", high );

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	char	style[200+1];

	style[0]= '\0';

	docHtmlSetShapeStyle( style, hwc, paraNode, io );

	if  ( style[0] )
	    { docHtmlWriteStringAttribute( hwc, "style", style );	}
	}

    if  ( utilMemoryBufferIsEmpty( &(io->ioAltText) ) )
	{ docHtmlWriteStringAttribute( hwc, "alt", "<IMG>" );	}
    else{
	docHtmlWriteStringAttribute( hwc, "alt",
			utilMemoryBufferGetString( &(io->ioAltText) ) );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Determine the size of a raster image that is included in the html	*/
/*  file.								*/
/*									*/
/************************************************************************/

static int docHtmlRasterImageSize(	int *			pWide,
					int *			pHigh,
					InsertedObject *	io )
    {
    const BitmapDescription *		bd;

    int					wide;
    int					high;
    int					d;

    wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* io->ioTwipsWide )/100 );
    high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* io->ioTwipsHigh )/100 );

    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObject( io ) )
	    { XDEB(io->ioRasterImage.riBytes); return 1;	}
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ return 1;	}
    bd= &(io->ioRasterImage.riDescription);

    if  ( wide < 1 ) { wide= 1;	}
    if  ( high < 1 ) { high= 1;	}

    d= ( 100* bd->bdPixelsWide- 100* wide )/ bd->bdPixelsWide;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ wide= bd->bdPixelsWide;	}

    d= ( 100* bd->bdPixelsHigh- 100* high )/ bd->bdPixelsHigh;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ high= bd->bdPixelsHigh;	}

    *pWide= wide; *pHigh= high; return 0;
    }

/************************************************************************/
/*									*/
/*  Save the reference to an image.					*/
/*									*/
/*  Usually, this is an 'img' element. For images that are better	*/
/*  rendered in SVG, we use an SVG object.				*/
/*									*/
/************************************************************************/

int docHtmlSaveInsertedObject(	int *				pDone,
				HtmlWritingContext *		hwc,
				int				n,
				const struct BufferItem *	paraNode,
				InsertedObject *		io )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    int				rval= 0;
    const char *		contentType;
    const char *		ext;
    int				type;
    int				useDataUrl;

    const MemoryBuffer *	objectData= (const MemoryBuffer *)0;
    bmWriteBitmap		writeRaster= (bmWriteBitmap)0;
    MetafileWriteSvg		playMetafile= (MetafileWriteSvg)0;

    int				fixed= 0;

    MemoryBuffer		src;

    int				wide= 0;
    int				high= 0;

    utilInitMemoryBuffer( &src );

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
				&writeRaster, &playMetafile,
				&contentType, &ext, &objectData, io ) )
	{ *pDone= 0; rval= 1; goto ready;	}

    if  ( docCheckObjectLayout(	&fixed, io ) )
	{ LDEB(1);	}

    if  ( ! useDataUrl )
	{
	if  ( ! hws->hwsGetImageSrc					||
	      (*hws->hwsGetImageSrc)( &src, hwc, n, io, ext ) < 0	)
	    { LXSDEB(n,hws->hwsGetImageSrc,ext); rval= -1; goto ready;	}
	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    {
	    DocumentRectangle	drTwips;

	    docPlaceRootShapeRect( &drTwips,
			    &(io->ioDrawingShape->dsShapeProperties), 0, 0 );

	    if  ( hws->hwsInlineImages )
		{
		const int		includeSvgDeclaration= 0;
		SimpleOutputStream * 	sosImage;

		sosImage= hwc->hwcXmlWriter.xwSos;

		if  ( docHtmlSaveDrawingShapeToSvgElement( hwc, io,
					includeSvgDeclaration, sosImage ) )
		    { LDEB(1); rval= -1; goto ready;	}

		*pDone= 1; rval= 0; goto ready;
		}
	    else{
		wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* io->ioTwipsWide )/100 );
		high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* io->ioTwipsHigh )/100 );
		}
	    }
	    break;

    	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	    if  ( hws->hwsInlineImages )
		{
		if  ( ! useDataUrl )
		    {
		    if  ( docHtmlPlayMetafileToSvgElement( hwc,
						playMetafile, io, objectData ) )
			{ LDEB(1); rval= -1; goto ready;	}

		    *pDone= 1; rval= 0; goto ready;
		    }
		}
	    else{
		wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* io->ioTwipsWide )/100 );
		high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* io->ioTwipsHigh )/100 );
		}

	    break;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokGIF_FILE:
	    {
	    if  ( docHtmlRasterImageSize( &wide, &high, io ) )
		{ LDEB(1); *pDone= 0; rval= 1; goto ready;	}
	    }
	    break;

	default:
	    LDEB(type); *pDone= 0; rval= 1; goto ready;
	}

    docHtmlStartImgElement( hwc, paraNode, io, wide, high );

    if  ( useDataUrl )
	{
	switch( type )
	    {
	    case DOCokPICTPNGBLIP:
	    case DOCokPICTJPEGBLIP:
	    case DOCokGIF_FILE:

		if  ( xmlWriteDataUrl( &(hwc->hwcXmlWriter), "src",
						contentType, objectData ) )
		    { SDEB(contentType); rval= -1; goto ready;	}

		break;

	    case DOCokPICTWMETAFILE:
	    case DOCokPICTEMFBLIP:
	    case DOCokMACPICT:

		if  ( docHtmlPlayMetafileToSvgDataUrl( hwc,
				playMetafile, contentType, io, objectData ) )
		    { LDEB(1); rval= -1; goto ready;	}

		break;

	    case DOCokDRAWING_SHAPE:

		if  ( docHtmlSaveDrawingShapeToSvgDataUrl(
							hwc, contentType, io ) )
		    { LDEB(1); rval= -1; goto ready;	}

		break;

	    default:
		LDEB(type); *pDone= 0; rval= 1; goto ready;
	    }
	}
    else{
	if  ( ! hws->hwsInlineImages )
	    {
	    docHtmlWriteStringAttribute( hwc, "src",
					utilMemoryBufferGetString( &src ) );
	    }
	}

    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    hwc->hwcImageCount++;
    *pDone= 1;

  ready:

    utilCleanMemoryBuffer( &src );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save from object data.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveObjectDataToStream(
					HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const MemoryBuffer *	objectData,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    if  ( ! hws->hwsOpenImageStream )
	{ XDEB(hws->hwsOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hws->hwsOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    sisData= sioInHexedMemoryOpen( objectData );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    if  ( sioCopyStream( sosImage, sisData ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save the data of the images in the document.			*/
/*  This is for the images that were not directly embedded in the HTML.	*/
/*									*/
/************************************************************************/

static int docHtmlSaveRasterBytesToStream(
					HtmlWritingContext *	hwc,
					bmWriteBitmap		writeRaster,
					int			n,
					InsertedObject *	io,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    if  ( ! hws->hwsOpenImageStream )
	{ XDEB(hws->hwsOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hws->hwsOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( (*writeRaster)( &(io->ioRasterImage.riDescription),
					io->ioRasterImage.riBytes, sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

static int docHtmlSaveImageFile(	int		n,
					void *		vio,
					void *		vhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)vhwc;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    InsertedObject *		io= (InsertedObject *)vio;

    int				type;
    int				useDataUrl;
    const char *		contentType;
    const char *		ext;

    int				fixed= 0;

    const MemoryBuffer *	objectData= (const MemoryBuffer *)0;
    bmWriteBitmap		writeRaster= (bmWriteBitmap)0;
    MetafileWriteSvg		playMetafile= (MetafileWriteSvg)0;

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
			&writeRaster, &playMetafile,
			&contentType, &ext, &objectData, io )	)
	{ return 0;	}

    if  ( docCheckObjectLayout( &fixed, io ) )
	{ LDEB(1);	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    if  ( hws->hwsInlineImages )
		{ return 0;	}

	    if  ( docHtmlSaveDrawingShapeToSvgStream( hwc, n, io,
							contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:

	    if  ( hws->hwsInlineImages )
		{ return 0;	}

	    if  ( docHtmlPlayMetafileToSvgStream( hwc, n, io, objectData,
					    playMetafile, contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( ! useDataUrl && objectData )
		{
		if  ( docHtmlSaveObjectDataToStream( hwc,
					n, io, objectData, contentType, ext ) )
		    { SDEB(contentType); return -1;	}

		return 0;
		}
	    break;

	default:
	    break;
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ XDEB(io->ioRasterImage.riBytes); return 0;	}

    if  ( writeRaster )
	{
	if  ( docHtmlSaveRasterBytesToStream( hwc, writeRaster,
						n, io, contentType, ext ) )
	    { SDEB(contentType); return -1;	}

	return 0;
	}
    else{
	LLDEB(n,type); return 0;
	}
    }

/************************************************************************/
/*									*/
/*  Save the content of images that have not been included in line	*/
/*									*/
/************************************************************************/

int docHtmlSaveImageFiles(	HtmlWritingContext *		hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					    docHtmlSaveImageFile, (void *)hwc );

    return 0;
    }

