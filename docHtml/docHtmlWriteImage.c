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

#   include	<appDebugon.h>

/************************************************************************/

int docHtmlObjectSaveHow(	const HtmlWritingContext *	hwc,
				int *				pType,
				int *				pUseDataUrl,
				bmWriteBitmap *			pWriteBitmap,
				const char **			pMimeType,
				const char **			pExt,
				const MemoryBuffer **		pObjectData,
				const InsertedObject *		io )
    {
    const RasterImage *		abi= &(io->ioRasterImage);

    if  ( io->ioKind == DOCokPICTWMETAFILE	||
	  io->ioKind == DOCokPICTEMFBLIP	||
	  io->ioKind == DOCokMACPICT		)
	{
	*pType= io->ioKind;
	*pUseDataUrl= 0;
	*pWriteBitmap= (bmWriteBitmap)0;
	*pMimeType= "image/svg+xml";
	*pExt= "svg";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokPICTJPEGBLIP )
	{
	*pType= io->ioKind;
	*pUseDataUrl= hwc->hwcInlineImages;
	*pWriteBitmap= bmJpegWriteJfif;
	*pMimeType= "image/jpeg";
	*pExt= "jpg";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokPICTPNGBLIP )
	{
	*pType= io->ioKind;
	*pUseDataUrl= hwc->hwcInlineImages;
	*pWriteBitmap= bmPngWritePng;
	*pMimeType= "image/png";
	*pExt= "png";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT )
	{
	if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
	      io->ioResultKind == DOCokPICTEMFBLIP	||
	      io->ioResultKind == DOCokMACPICT		)
	    {
	    *pType= io->ioResultKind;
	    *pUseDataUrl= 0;
	    *pMimeType= "image/svg+xml";
	    *pExt= "svg";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }

	if  ( io->ioResultKind == DOCokPICTJPEGBLIP )
	    {
	    *pType= io->ioResultKind;
	    *pUseDataUrl= hwc->hwcInlineImages;
	    *pWriteBitmap= bmJpegWriteJfif;
	    *pMimeType= "image/jpeg";
	    *pExt= "jpg";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }

	if  ( io->ioResultKind == DOCokPICTPNGBLIP )
	    {
	    *pType= io->ioResultKind;
	    *pUseDataUrl= hwc->hwcInlineImages;
	    *pWriteBitmap= bmPngWritePng;
	    *pMimeType= "image/png";
	    *pExt= "png";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	*pType= io->ioKind;
	*pUseDataUrl= 0;
	*pMimeType= "image/svg+xml";
	*pExt= "svg";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
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
	*pMimeType= "image/jpeg";
	*pExt= "jpg";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}

    return 1;
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
    int				wide;
    int				high;

    const char *		contentType;
    const char *		ext;
    int				type;
    int				useDataUrl;
    int				asObject= 0;

    const MemoryBuffer *	mb= (const MemoryBuffer *)0;
    bmWriteBitmap		writeBitmap= (bmWriteBitmap)0;

    int				fixed= 0;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
				&writeBitmap, &contentType, &ext, &mb, io ) )
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

	    wide= drTwips.drX1- drTwips.drX0+ 1;
	    high= drTwips.drY1- drTwips.drY0+ 1;

	    docHtmlStartObject( hwc, paraNode, io, wide, high, contentType, &src );
	    asObject= 1;
	    }
	    break;

    	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	    wide= ( io->ioScaleXSet* io->ioTwipsWide )/100;
	    high= ( io->ioScaleYSet* io->ioTwipsHigh )/100;

	    docHtmlStartObject( hwc, paraNode, io, wide, high, contentType, &src );
	    asObject= 1;
	    break;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokGIF_FILE:
	    if  ( docHtmlRasterImageSize( &wide, &high, io ) )
		{ LDEB(1); *pDone= 0; rval= 1; goto ready;	}
	    break;

	default:
	    LDEB(type); *pDone= 0; rval= 1; goto ready;
	}

    docHtmlStartImgElement( hwc, paraNode, io, wide, high );

    if  ( useDataUrl )
	{
	XmlWriter *	xw= &(hwc->hwcXmlWriter);

	xmlStartDataUrl( xw, "src", contentType );
	}
    else{
	docHtmlWriteStringAttribute( hwc, "src",
					utilMemoryBufferGetString( &src ) );
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

    utilCleanMemoryBuffer( &src );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveDrawingShape(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;

    DocumentRectangle		drDest;
    int				pixelsWide;
    int				pixelsHigh;
    int				page;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    struct DocumentTree *		ei= (struct DocumentTree *)0;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;

    DrawingShape *		ds= io->ioDrawingShape;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    pixelsWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    pixelsHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);
    page= io->ioY0Position.lpPage;

    if  ( docGetRootOfSelectionScope( &ei, &bodySectNode,
					hwc->hwcLayoutContext->lcDocument,
					&(ds->dsSelectionScope) ) )
	{ LDEB(1);	}

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docSvgSaveShapeObject( sosImage,
				    page, pixelsWide, pixelsHigh,
				    io, bodySectNode, hwc->hwcLayoutContext ) )
	{ LDEB(1); return -1;	}

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

static int docHtmlPlayMetafileToSvg(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const MemoryBuffer *	mb,
					MetafileWriteSvg	writeSvg,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;

    const PictureProperties *	pip= &(io->ioPictureProperties);

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    ObjectReader		or;

    MetafilePlayer		mp;
    SvgWriter			sw;

    DocumentRectangle		drDest;

    svgInitSvgWriter( &sw );
    bmInitObjectReader( &or );

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( bmOpenObjectReader( &or, mb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sw.swXmlWriter.xwSos= sosImage;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    docObjectGetSourceRect( &(sw.swViewBox), pip );
    sw.swWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    sw.swHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);

    docSetMetafilePlayer( &mp, or.orSisHex, hwc->hwcLayoutContext, pip,
						    sw.swWide, sw.swHigh );

    svgStartDocument( &sw );

    if  ( (*writeSvg)( &sw, &mp ) )
	{ LSDEB(n,contentType); rval= -1; goto ready;	}

    svgFinishDocument( &sw );

  ready:

    bmCleanObjectReader( &or );

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save from object data.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveObjectData(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const MemoryBuffer *	mb,
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

    if  ( bmOpenObjectReader( &or, mb ) )
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

static int docHtmlSaveRasterBytes(	HtmlWritingContext *	hwc,
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

static int docHtmlSaveImageFile( int n, void * vio, void * vhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)vhwc;
    InsertedObject *		io= (InsertedObject *)vio;

    int				type;
    int				useDataUrl;
    const char *		contentType;
    const char *		ext;

    int				fixed= 0;

    const MemoryBuffer *	mb= (const MemoryBuffer *)0;
    bmWriteBitmap		writeBitmap= (bmWriteBitmap)0;

    if  ( docHtmlObjectSaveHow( hwc, &type, &useDataUrl,
			&writeBitmap, &contentType, &ext, &mb, io )	||
	  useDataUrl							)
	{ return 0;	}

    if  ( docCheckObjectLayout( &fixed, io ) )
	{ LDEB(1);	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    if  ( docHtmlSaveDrawingShape( hwc, n, io, contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTWMETAFILE:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMetaPlayWmfSvg, contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMetaPlayEmfSvg, contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokMACPICT:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMacPictPlayFileSvg, contentType, ext ) )
		{ SDEB(contentType); return -1;	}

	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( mb )
		{
		if  ( docHtmlSaveObjectData( hwc,
					n, io, mb, contentType, ext ) )
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
	if  ( docHtmlSaveRasterBytes( hwc, writeBitmap,
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

