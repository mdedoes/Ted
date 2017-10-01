/************************************************************************/
/*									*/
/*  Save the images in a document and include references in the	HTML	*/
/*  file.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	<drawMetafileSvg.h>
#   include	<docMetafileObject.h>
#   include	<docLayoutObject.h>
#   include	"docSvgDrawImpl.h"

#   include	<sioGeneral.h>
#   include	<sioUtil.h>
#   include	<sioMemory.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docShape.h>
#   include	"docHtmlWriteImpl.h"

#   include	<docObjectProperties.h>
#   include	<bmObjectReader.h>
#   include	<docObjectIo.h>
#   include	<docObject.h>
#   include	<svgWriter.h>
#   include	<layoutContext.h>
#   include	<docShapeGeometry.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static void docHtmlSetSaveAsMetaSvg(
				int *				pUseDataUrl,
				bmWriteBitmap *			pWriteBitmap,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pUseDataUrl= 0;
    *pWriteBitmap= (bmWriteBitmap)0;
    *pMimeType= "image/svg+xml";
    *pExt= "svg";
    return;
    }

static void docHtmlSetSaveAsRasterFromData(
				const HtmlWritingContext *	hwc,
				int *				pUseDataUrl,
				MetafileWriteSvg *		pWriteSvg )
    {
    *pUseDataUrl= hwc->hwcInlineImages;
    *pWriteSvg= (MetafileWriteSvg)0;
    return;
    }

static void docHtmlSetSaveAsJfif(
				bmWriteBitmap *			pWriteBitmap,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pWriteBitmap= bmJpegWriteJfif;
    *pMimeType= "image/jpeg";
    *pExt= "jpg";
    return;
    }

static void docHtmlSetSaveAsPng(
				bmWriteBitmap *			pWriteBitmap,
				const char **			pMimeType,
				const char **			pExt )
    {
    *pWriteBitmap= bmPngWritePng;
    *pMimeType= "image/png";
    *pExt= "png";
    return;
    }

int docHtmlObjectSaveHow(	const HtmlWritingContext *	hwc,
				int *				pType,
				int *				pUseDataUrl,
				bmWriteBitmap *			pWriteBitmap,
				MetafileWriteSvg *		pWriteSvg,
				const char **			pMimeType,
				const char **			pExt,
				const MemoryBuffer **		pObjectData,
				const InsertedObject *		io )
    {
    const RasterImage *		abi= &(io->ioRasterImage);

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMetaPlayWmfSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTEMFBLIP:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMetaPlayEmfSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokMACPICT:
	    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pWriteSvg= appMacPictPlayFileSvg;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTJPEGBLIP:
	    docHtmlSetSaveAsRasterFromData( hwc, pUseDataUrl, pWriteSvg );
	    docHtmlSetSaveAsJfif( pWriteBitmap, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokPICTPNGBLIP:
	    docHtmlSetSaveAsRasterFromData( hwc, pUseDataUrl, pWriteSvg );
	    docHtmlSetSaveAsPng( pWriteBitmap, pMimeType, pExt );
	    *pType= io->ioKind;
	    *pObjectData= &(io->ioObjectData);
	    return 0;

	case DOCokDRAWING_SHAPE:
	    *pType= io->ioKind;
	    *pUseDataUrl= 0;
	    *pWriteBitmap= (bmWriteBitmap)0;
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
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMetaPlayWmfSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTEMFBLIP:
		    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMetaPlayEmfSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokMACPICT:
		    docHtmlSetSaveAsMetaSvg(
				pUseDataUrl, pWriteBitmap, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pWriteSvg= appMacPictPlayFileSvg;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTJPEGBLIP:
		    docHtmlSetSaveAsRasterFromData(
					    hwc, pUseDataUrl, pWriteSvg );
		    docHtmlSetSaveAsJfif( pWriteBitmap, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pObjectData= &(io->ioResultData);
		    return 0;

		case DOCokPICTPNGBLIP:
		    docHtmlSetSaveAsRasterFromData(
					    hwc, pUseDataUrl, pWriteSvg );
		    docHtmlSetSaveAsPng( pWriteBitmap, pMimeType, pExt );
		    *pType= io->ioResultKind;
		    *pObjectData= &(io->ioResultData);
		    return 0;
		}

	    break;
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ return 1;	}

#   if  USE_PNG
    if  ( abi->riDescription.bdColorEncoding == BMcoRGB8PALETTE		&&
	  bmCanSaveAsContentType( &(abi->riDescription), "image/png" )	)
	{
	*pType= DOCokPICTPNGBLIP;
	*pUseDataUrl= hwc->hwcInlineImages;
	*pWriteBitmap= bmPngWritePng;
	*pWriteSvg= (MetafileWriteSvg)0;
	*pMimeType= "image/png";
	*pExt= "png";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

#   if  USE_GIF
    if  ( bmCanSaveAsContentType( &(abi->riDescription), "image/gif" ) )
	{
	*pType= DOCokGIF_FILE;
	*pUseDataUrl= hwc->hwcInlineImages;
	*pWriteBitmap= bmGifWriteGif;
	*pWriteSvg= (MetafileWriteSvg)0;
	*pMimeType= "image/gif";
	*pExt= "gif";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

    if  ( bmCanSaveAsContentType( &(abi->riDescription), "image/jpeg" ) )
	{
	*pType= DOCokPICTJPEGBLIP;
	*pUseDataUrl= hwc->hwcInlineImages;
	*pWriteBitmap= bmJpegWriteJfif;
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
/*  Save metafile to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlPlayMetafileToSvg(
				HtmlWritingContext *	hwc,
				InsertedObject *	io,
				int			includeSvgDeclaration,
				const MemoryBuffer *	objectData,
				MetafileWriteSvg	writeSvg,
				SimpleOutputStream *	sosImage )
    {
    int				rval= 0;

    const PictureProperties *	pip= &(io->ioPictureProperties);

    DocumentRectangle		drDest;

    ObjectReader		or;
    SvgWriter			sw;
    MetafilePlayer		mp;

    svgInitSvgWriter( &sw );
    bmInitObjectReader( &or );

    if  ( bmOpenObjectReader( &or, objectData ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sw.swXmlWriter.xwSos= sosImage;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    docObjectGetSourceRect( &(sw.swViewBox), pip );
    sw.swWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    sw.swHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);

    docSetMetafilePlayer( &mp, or.orSisHex, hwc->hwcLayoutContext, pip,
						    sw.swWide, sw.swHigh );

    svgStartDocument( &sw, includeSvgDeclaration );

    if  ( (*writeSvg)( &sw, &mp ) )
	{ LDEB(1); rval= -1; goto ready;	}

    svgFinishDocument( &sw );

  ready:

    bmCleanObjectReader( &or );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveDrawingShapeToSvg(
				HtmlWritingContext *	hwc,
				InsertedObject *	io,
				int			includeSvgDeclaration,
				SimpleOutputStream *	sosImage )
    {
    int				rval= 0;

    DocumentRectangle		drDest;
    int				pixelsWide;
    int				pixelsHigh;
    int				page;

    struct DocumentTree *	tree= (struct DocumentTree *)0;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;

    DrawingShape *		ds= io->ioDrawingShape;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    pixelsWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    pixelsHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);
    page= io->ioY0Position.lpPage;

    if  ( docGetRootOfSelectionScope( &tree, &bodySectNode,
					hwc->hwcLayoutContext->lcDocument,
					&(ds->dsSelectionScope) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSvgSaveShapeObject( sosImage, includeSvgDeclaration,
				    page, pixelsWide, pixelsHigh,
				    io, bodySectNode, hwc->hwcLayoutContext ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/

static void docHtmlSetShapeLeft(	char *			style )
    {
    strcat( style, "float: left;" );
    strcat( style, "clear: left;" );
    strcat( style, "margin: 5px 5px 5px 0;" );
    }

static void docHtmlSetShapeRight(	char *			style )
    {
    strcat( style, "float: right;" );
    strcat( style, "clear: right;" );
    strcat( style, "margin: 5px 0 5px 5px;" );
    }

static void docHtmlSetShapeStyle(
			char *				style,
			HtmlWritingContext *		hwc,
			const struct BufferItem *	paraNode,
			const InsertedObject *		io )
    {
    const DrawingShape *	ds= io->ioDrawingShape;
    const ShapeProperties *	sp= &(ds->dsShapeProperties);

    switch( sp->spWrapStyle )
	{
	case SHPswtTOPBOTTOM:
	    strcat( style, "clear: both;" );
	    break;

	case SHPswtAROUND:
	case SHPswtTIGHTLY:
	    switch( sp->spWrapSide )
		{
		case SHPswsBOTH:
		case SHPswsLARGEST:
		    {
		    int	px0;
		    int	px1;

		    docParagraphFrameTwips( &(hwc->hwcParagraphFrame),
					&(hwc->hwcBlockFrame), paraNode );

		    px0= hwc->hwcParagraphFrame.pfParaContentRect.drX0;
		    px1= hwc->hwcParagraphFrame.pfParaContentRect.drX1;

		    if  ( sp->spRect.drX0- px0 >= px1- sp->spRect.drX1 )
			{ docHtmlSetShapeRight( style );	}
		    else{ docHtmlSetShapeLeft( style );		}
		    }

		    break;

		case SHPswsLEFT:
		    docHtmlSetShapeRight( style );
		    break;

		case SHPswsRIGHT:
		    docHtmlSetShapeLeft( style );
		    break;

		default:
		    LLDEB(sp->spWrapStyle,sp->spWrapSide);
		    break;
		}
	    break;

	case SHPswtNONE:
	case SHPswtTHROUGH:
	    LDEB(sp->spWrapStyle);
	    break;

	default:
	    LDEB(sp->spWrapStyle);
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Start an 'object' in the sense of the HTML specification.		*/
/*									*/
/************************************************************************/

static void docHtmlStartObject(	HtmlWritingContext *		hwc,
				const struct BufferItem *	paraNode,
				const InsertedObject *		io,
				int				wide,
				int				high,
				const char *			contentType,
				const MemoryBuffer *		src )
    {
    wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* wide )/100 );
    high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* high )/100 );

    docHtmlPutString( "<object", hwc );

    docHtmlWriteStringAttribute( hwc, "type", contentType );
    docHtmlWriteStringAttribute( hwc, "data", utilMemoryBufferGetString( src ) );

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

    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );
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

    docHtmlWriteStringAttribute( hwc, "alt", "<IMG>" );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Determine the size of a raster image thta is included in the html	*/
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
/*  Same the reference to an image.					*/
/*									*/
/*  Usually, this is an 'img' element. For images that are better	*/
/*  rendered in SVG, we use an SVG object.				*/
/*									*/
/************************************************************************/

int docHtmlSaveImgElement(	int *				pDone,
				HtmlWritingContext *		hwc,
				int				n,
				const struct BufferItem *	paraNode,
				InsertedObject *		io )
    {
    int				rval= 0;
    const char *		contentType;
    const char *		ext;
    int				type;
    int				useDataUrl;
    int				asObject= 0;

    const MemoryBuffer *	objectData= (const MemoryBuffer *)0;
    bmWriteBitmap		writeBitmap= (bmWriteBitmap)0;
    MetafileWriteSvg		writeSvg= (MetafileWriteSvg)0;

    int				fixed= 0;

    MemoryBuffer		src;

    SimpleInputStream *		sisMem= (SimpleInputStream *)0;

    int				wide;
    int				high;


    utilInitMemoryBuffer( &src );

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
				&writeBitmap, &writeSvg,
				&contentType, &ext, &objectData, io ) )
	{ *pDone= 0; rval= 1; goto ready;	}

    if  ( docCheckObjectLayout(	&fixed, io ) )
	{ LDEB(1);	}

    if  ( ! useDataUrl )
	{
	if  ( ! hwc->hwcGetImageSrc					||
	      (*hwc->hwcGetImageSrc)( &src, hwc, n, io, ext ) < 0	)
	    { LXSDEB(n,hwc->hwcGetImageSrc,ext); rval= -1; goto ready;	}
	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    {
	    DocumentRectangle	drTwips;

	    docPlaceRootShapeRect( &drTwips,
			    &(io->ioDrawingShape->dsShapeProperties), 0, 0 );

	    if  ( hwc->hwcInlineImages )
		{
		const int		includeSvgDeclaration= 0;
		SimpleOutputStream * 	sosImage;

		sosImage= hwc->hwcXmlWriter.xwSos;

		if  ( docHtmlSaveDrawingShapeToSvg( hwc, io,
					includeSvgDeclaration, sosImage ) )
		    { LDEB(1); rval= -1; goto ready;	}

		*pDone= 1; rval= 0; goto ready;
		}
	    else{
		wide= drTwips.drX1- drTwips.drX0+ 1;
		high= drTwips.drY1- drTwips.drY0+ 1;

		docHtmlStartObject( hwc, paraNode, io,
					wide, high, contentType, &src );
		asObject= 1;
		}
	    }
	    break;

    	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	    if  ( hwc->hwcInlineImages )
		{
		const int		includeSvgDeclaration= 0;
		SimpleOutputStream * 	sosImage;

		sosImage= hwc->hwcXmlWriter.xwSos;

		if  ( docHtmlPlayMetafileToSvg( hwc, io, includeSvgDeclaration,
					    objectData, writeSvg, sosImage ) )
		    { LSDEB(n,contentType); rval= -1; goto ready;	}

		*pDone= 1; rval= 0; goto ready;
		}
	    else{
		wide= ( io->ioScaleXSet* io->ioTwipsWide )/100;
		high= ( io->ioScaleYSet* io->ioTwipsHigh )/100;

		docHtmlStartObject( hwc, paraNode, io,
					    wide, high, contentType, &src );
		asObject= 1;
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
	XmlWriter *	xw= &(hwc->hwcXmlWriter);

	switch( type )
	    {
	    case DOCokPICTPNGBLIP:
	    case DOCokPICTJPEGBLIP:
	    case DOCokGIF_FILE:
		xmlStartDataUrl( xw, "src", contentType );

		sisMem= sioInMemoryOpen( objectData );
		if  ( ! sisMem )
		    { XDEB(sisMem); *pDone= 0; rval= -1; goto ready;        }

		if  ( sioCopyStream( xw->xwSos, sisMem ) )
		    { LDEB(1); rval= -1; goto ready;	}

		sioInClose( sisMem ); sisMem= (SimpleInputStream *)0;

		docHtmlPutString( "\"", hwc );

		break;

	    default:
		LDEB(type); *pDone= 0; rval= 1; goto ready;
	    }
	}
    else{
	if  ( ! hwc->hwcInlineImages )
	    {
	    docHtmlWriteStringAttribute( hwc, "src",
					utilMemoryBufferGetString( &src ) );
	    }
	}

    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    if  ( asObject )
	{
	docHtmlPutString( "</object>", hwc );
	docHtmlNewLine( hwc );
	}

    hwc->hwcImageCount++;
    *pDone= 1;

  ready:

    if  ( sisMem )
	{ sioInClose( sisMem );	}

    utilCleanMemoryBuffer( &src );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveDrawingShapeToSvgStream(
					HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    const int			includeSvgDeclaration= 1;

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docHtmlSaveDrawingShapeToSvg( hwc, io, includeSvgDeclaration,
				    sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save metafile to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlPlayMetafileToSvgStream(
				HtmlWritingContext *	hwc,
				int			n,
				InsertedObject *	io,
				const MemoryBuffer *	objectData,
				MetafileWriteSvg	writeSvg,
				const char *		contentType,
				const char *		ext )
    {
    int				rval= 0;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			includeSvgDeclaration= 1;

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ LSXDEB(n,contentType,sosImage); rval= -1; goto ready; }

    if  ( docHtmlPlayMetafileToSvg( hwc, io, includeSvgDeclaration,
					    objectData, writeSvg, sosImage ) )
	{ LSDEB(n,contentType); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

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

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    ObjectReader		or;

    bmInitObjectReader( &or );

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( bmOpenObjectReader( &or, objectData ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( sioCopyStream( sosImage, or.orSisHex ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    bmCleanObjectReader( &or );

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
					bmWriteBitmap		writeBitmap,
					int			n,
					InsertedObject *	io,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( (*writeBitmap)( &(io->ioRasterImage.riDescription),
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
    InsertedObject *		io= (InsertedObject *)vio;

    int				type;
    int				useDataUrl;
    const char *		contentType;
    const char *		ext;

    int				fixed= 0;

    const MemoryBuffer *	objectData= (const MemoryBuffer *)0;
    bmWriteBitmap		writeBitmap= (bmWriteBitmap)0;
    MetafileWriteSvg		writeSvg= (MetafileWriteSvg)0;

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
			&writeBitmap, &writeSvg,
			&contentType, &ext, &objectData, io )	)
	{ return 0;	}

    if  ( docCheckObjectLayout( &fixed, io ) )
	{ LDEB(1);	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    if  ( hwc->hwcInlineImages )
		{ return 0;	}

	    if  ( docHtmlSaveDrawingShapeToSvgStream( hwc, n, io,
							contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:

	    if  ( hwc->hwcInlineImages )
		{ return 0;	}

	    if  ( docHtmlPlayMetafileToSvgStream( hwc, n, io, objectData,
					    writeSvg, contentType, ext ) )
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

    if  ( writeBitmap )
	{
	if  ( docHtmlSaveRasterBytesToStream( hwc, writeBitmap,
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
    struct BufferDocument *	bd= hwc->hwcDocument;

    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					    docHtmlSaveImageFile, (void *)hwc );

    return 0;
    }

