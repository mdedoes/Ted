/************************************************************************/
/*									*/
/*  Print images to SVG.						*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<docDraw.h>
#   include	<docDrawLine.h>
#   include	"docSvgDrawImpl.h"
#   include	<drawMetafileSvg.h>
#   include	<drawImageSvg.h>
#   include	<docObjectProperties.h>
#   include	<docMetafileObject.h>
#   include	<docShape.h>
#   include	<bmObjectReader.h>
#   include	<docObjectIo.h>
#   include	<docObject.h>
#   include	<sioGeneral.h>
#   include	<svgWriter.h>
#   include	<docLayoutObject.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Emit a metafile.							*/
/*									*/
/************************************************************************/

static int docSvgDrawMetafile(	SvgWriter *			sw,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				int				objectKind,
				const LayoutContext *		lc,
				const DocumentRectangle *	drDest )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				rval= 0;

    ObjectReader		or;

    MetafilePlayer		mp;
    MetafileWriteSvg		playMetafile;

    DocumentRectangle		drSrc;

    bmInitObjectReader( &or );

    switch( objectKind )
	{
	case DOCokPICTWMETAFILE:
	    playMetafile= appMetaPlayWmfSvg;
	    break;

	case DOCokPICTEMFBLIP:
	    playMetafile= appMetaPlayEmfSvg;
	    break;

	case DOCokMACPICT:
	    playMetafile= appMacPictPlayFileSvg;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	default:
	    LDEB(pip->pipType); goto ready;
	}

    if  ( bmOpenObjectReader( &or, mb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docObjectGetSourceRect( &drSrc, pip );

    docSetMetafilePlayer( &mp, or.orSisHex, lc, pip, 0, 0 );

    sioOutPutString( "<svg ", xw->xwSos );
    svgWriteRectangleAttributes( sw, drDest );
    svgWriteViewBoxAttribute( sw, "viewBox", &drSrc );
    sioOutPutString( ">", xw->xwSos );
    xmlNewLine( xw );

    if  ( (*playMetafile)( sw, &mp ) )
	{ LDEB(1);	}

    sioOutPutString( "</svg>", xw->xwSos );
    xmlNewLine( xw );

  ready:

    bmCleanObjectReader( &or );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int docSvgDrawRasterImage( SvgWriter *			sw,
				DrawingContext *		dc,
				InsertedObject *		io,
				int				objectKind,
				const MemoryBuffer *		mb,
				const DocumentRectangle *	drDest )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);
    int				cropped= docPictureIsCropped( pip );

    if  ( ! cropped && objectKind == DOCokPICTJPEGBLIP )
	{
	if  ( ! drawRasterImageSvgFromData( sw, "image/jpeg", mb, drDest ) )
	    { return 1;	}
	}

    if  ( ! cropped && objectKind == DOCokPICTPNGBLIP )
	{
	if  ( ! drawRasterImageSvgFromData( sw, "image/png", mb, drDest ) )
	    { return 1;	}
	}

    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObject( io ) )
	    { XDEB(io->ioRasterImage.riBytes);	}
	}

    if  ( io->ioRasterImage.riBytes )
	{
	DocumentRectangle	drSel;

	docObjectGetCropRect( &drSel, pip, &(io->ioRasterImage.riDescription) );

	if  ( ! drawRasterImageSvg( sw, &(io->ioRasterImage), &drSel, drDest ) )
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int docSvgDrawShapeRaster( SvgWriter *			sw,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const DocumentRectangle *	drDest )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    int				cropped= docPictureIsCropped( pip );
    const MemoryBuffer *	mb= &(ds->dsPictureData);

    if  ( ! cropped && pip->pipType == DOCokPICTJPEGBLIP )
	{
	if  ( ! drawRasterImageSvgFromData( sw, "image/jpeg", mb, drDest ) )
	    { return 1;	}
	}

    if  ( ! cropped && pip->pipType == DOCokPICTPNGBLIP )
	{
	if  ( ! drawRasterImageSvgFromData( sw, "image/png", mb, drDest ) )
	    { return 1;	}
	}

    if  ( ! ds->dsRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObjectData( pip->pipType,
				&(ds->dsRasterImage), &(ds->dsPictureData) ) )
	    { XDEB(ds->dsRasterImage.riBytes);	}
	}

    if  ( ds->dsRasterImage.riBytes )
	{
	DocumentRectangle	drSel;

	docObjectGetCropRect( &drSel, pip,
				    &(ds->dsRasterImage.riDescription) );

	if  ( ! drawRasterImageSvg( sw, &(ds->dsRasterImage), &drSel, drDest ) )
	    { return 1;	}
	}

    return 0;
    }

int docSvgDrawShapeImage(	SvgWriter *			sw,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const DocumentRectangle *	drTwips,
				const AffineTransform2D *	at )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	    if  ( docSvgDrawMetafile( sw, pip, &(ds->dsPictureData),
						pip->pipType, lc, drTwips ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( docSvgDrawShapeRaster( sw, dc, ds, drTwips ) < 0 )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(pip->pipType); return 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save images and embedded objects to SVG.				*/
/*									*/
/************************************************************************/

int docSvgDrawInlineObject(	const DrawTextLine *		dtl,
				int				part,
				InsertedObject *		io,
				const DocumentRectangle *	drTwips,
				const LayoutPosition *		baseLine )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;

    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    switch( io->ioKind )
	{
	int		done;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:

	    if  ( docSvgDrawMetafile( sw, &(io->ioPictureProperties),
						    &(io->ioObjectData),
						    io->ioKind, lc, drTwips ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    return 1;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:

	    done= docSvgDrawRasterImage( sw, dc, io,
				    io->ioKind, &(io->ioObjectData), drTwips );
	    return done;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		||
		  io->ioResultKind == DOCokMACPICT		)
		{
		if  ( docSvgDrawMetafile( sw, &(io->ioPictureProperties),
					    &(io->ioResultData),
					    io->ioResultKind, lc, drTwips ) )
		    { LDEB(1); break;	}

		dc->dcCurrentTextAttributeSet= 0;
		dc->dcCurrentColorSet= 0;
		return 1;
		}

	    if  ( io->ioResultKind == DOCokPICTJPEGBLIP	||
	    	  io->ioResultKind == DOCokPICTPNGBLIP	)
		{
		done= docSvgDrawRasterImage( sw, dc, io,
			    io->ioResultKind, &(io->ioResultData), drTwips );
		return done;
		}

	    break;

	case DOCokDRAWING_SHAPE:
	    /*  Done in a separate loop from generic drawing code */
	    return 1;

	case DOCokEPS_FILE:
	default:
	    LDEB(io->ioKind); return 0;
	}

    LDEB(1); return 1;
    }


