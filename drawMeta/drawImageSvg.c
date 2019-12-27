#   include	"drawMetaConfig.h"

#   include	<stddef.h>

#   include	"drawImageSvg.h"
#   include	<bmio.h>
#   include	<svgWriter.h>
#   include	<bitmap.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start an image.							*/
/*									*/
/************************************************************************/

static int drawSvgStartImage(	SvgWriter *			sw,
				const DocumentRectangle *	drDest )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);

    xmlPutString( "<image ", xw );

    xmlWriteStringAttribute( xw, "overflow", "visible" );
    xmlWriteStringAttribute( xw, "preserveAspectRatio", "none" );
    svgWriteRectangleAttributes( sw, drDest );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a raster image in the SVG output.				*/
/*									*/
/************************************************************************/

int drawRasterImageSvg(		SvgWriter *			sw,
				const RasterImage *		riSrc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    {
    int			rval= 0;
    XmlWriter *		xw= &(sw->swXmlWriter);
    const RasterImage *	ri= riSrc;

    RasterImage		riDest;

    SimpleOutputStream *	sosBase64= (SimpleOutputStream *)0;

    bmInitRasterImage( &riDest );

    if  ( drSrc )
	{
	if  ( drSrc->drX0 != 0						||
	      drSrc->drY0 != 0						||
	      drSrc->drX1 != riSrc->riDescription.bdPixelsWide- 1	||
	      drSrc->drY1 != riSrc->riDescription.bdPixelsHigh- 1	)
	    {
	    if  ( bmSelect( &riDest, riSrc, drSrc ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    ri= &riDest;
	    }
	}

    drawSvgStartImage( sw, drDest );
    sosBase64= xmlStartDataUrl( xw, "xlink:href", "image/png" );
    if  ( ! sosBase64 )
	{ XDEB(sosBase64); rval= -1; goto ready;	}

    if  ( bmPngWritePng( &(ri->riDescription), ri->riBytes, sosBase64 ) )
	{ XDEB(sosBase64); rval= -1; goto ready;	}
    xmlFinishDataUrl( xw, sosBase64 ); sosBase64= (SimpleOutputStream *)0;

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

  ready:

    bmCleanRasterImage( &riDest );

    if  ( sosBase64 )
	{ sioOutClose( sosBase64 );	}

    return rval;
    }


/************************************************************************/
/*									*/
/*  Insert an image from data in the SVG output.			*/
/*									*/
/************************************************************************/

int drawRasterImageSvgFromData(	SvgWriter *			sw,
				const char *			contentType,
				const struct MemoryBuffer *	mb,
				const DocumentRectangle *	drDest )
    {
    int				rval= 0;
    XmlWriter *			xw= &(sw->swXmlWriter);

    drawSvgStartImage( sw, drDest );

    xmlWriteDataUrl( xw, "xlink:href", contentType, mb );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return rval;
    }

