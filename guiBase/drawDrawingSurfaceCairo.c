/************************************************************************/
/*									*/
/*  Drawing surface CAIRO implementation.				*/
/*									*/
/************************************************************************/

#   include "guiBaseConfig.h"

#   include	"guiBase.h"

#   if DRAW_CAIRO

#   include "guiWidgetDrawingSurface.h"
#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawTextImplX11.h"
#   include "drawScreenFontAdmin.h"
#   include "drawUtilGdk.h"
#   include "drawImplXft.h"
#   include "drawImpl.h"
#   include "drawFontFc.h"
#   include <bitmap.h>
#   include <bmRender.h>
#   include <geoRectangle.h>
#   include <geo2DInteger.h>

#   include <appDebugon.h>

#   if FONTS_CAIRO
#	if ! defined(CAIRO_HAS_FT_FONT) || ! CAIRO_HAS_FT_FONT
	CAIRO_HAS_FT_FONT is not available
#	else
#	    include	<cairo/cairo-ft.h>
#	endif
#   endif

/************************************************************************/

static DrawingSurface drawMakeChildDrawingSurface(
				DrawingSurface			parent )
    {
    DrawingSurface	ds= malloc(sizeof(struct DrawingSurface));

    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

    if  ( parent->dsIsImage || ! parent->dsWindow )
	{
	LXDEB(parent->dsIsImage,parent->dsWindow);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    ds->dsFontOptions= cairo_font_options_copy( parent->dsFontOptions );

    return ds;
    }

/************************************************************************/

static cairo_surface_t * drawMakeImageSurface(
				const DocumentRectangle *	drDest,
				const struct RasterImage *	ri,
				const DocumentRectangle *	drSrc )
    {
    cairo_surface_t *		image= (cairo_surface_t *)0;
    const BitmapDescription *	bd= &(ri->riDescription);

    cairo_format_t		format= CAIRO_FORMAT_INVALID;

    RasterImage			riOut;
    BitmapDescription *		bdOut= &(riOut.riDescription);

    ColorAllocator		ca;

    const int			bitmapUnit= 0;
    const int			swapBitmapBytes= 0;
    const int			swapBitmapBits= 0;
    const int			dither= 0;

    int				stride;
    unsigned char *		bytes= (unsigned char *)0;

    bmInitRasterImage( &riOut );
    bmInitColorAllocator( &ca );

    bdOut->bdPixelsWide= drDest->drX1- drDest->drX0+ 1;
    bdOut->bdPixelsHigh= drDest->drY1- drDest->drY0+ 1;

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB:
	case BMcoRGB8PALETTE:
	    bdOut->bdBitsPerSample= 8;
	    bdOut->bdColorEncoding= BMcoRGB;

	    if  ( bd->bdHasAlpha )
		{
		format= CAIRO_FORMAT_ARGB32;
		bdOut->bdSamplesPerPixel= 4;
		bdOut->bdHasAlpha= 1;
		}
	    else{
		format= CAIRO_FORMAT_RGB24;
		bdOut->bdSamplesPerPixel= 3;
		bdOut->bdHasAlpha= 0;
		}

	    break;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bd->bdHasAlpha )
		{ LDEB(bd->bdHasAlpha); goto ready;	}
	    if  ( bd->bdBitsPerPixel == 1 )
		{
		format= CAIRO_FORMAT_A1;

		bdOut->bdBitsPerSample= 1;
		bdOut->bdSamplesPerPixel= 1;
		bdOut->bdColorEncoding= BMcoBLACKWHITE;
		}
	    else{
		format= CAIRO_FORMAT_A8;

		bdOut->bdBitsPerSample= 8;
		bdOut->bdSamplesPerPixel= 1;
		bdOut->bdColorEncoding= BMcoBLACKWHITE;
		}

	    break;

	default:
	    LDEB(bd->bdColorEncoding); goto ready;
	}

    if  ( bmCalculateSizes( bdOut ) )
	{ LDEB(1); goto ready;	}
    if  ( bmAllocateBuffer( &riOut ) )
	{ LDEB(bdOut->bdBufferLength); goto ready;	}

    if  ( bmSetColorAllocatorForImage( &ca, bdOut ) )
	{ LDEB(1); goto ready;	}

    stride= cairo_format_stride_for_width( format, bdOut->bdPixelsWide );

    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
			    dither, riOut.riBytes, bdOut, ri, drSrc ) )
	{ LDEB(1); goto ready; }

    bytes= malloc( stride* bdOut->bdPixelsHigh );
    if  ( ! bytes )
	{ XDEB(bytes); goto ready;	}

    switch( format )
	{
	case CAIRO_FORMAT_RGB24:
	case CAIRO_FORMAT_ARGB32:
	    if  ( bmToNativeARGB32( bytes, riOut.riBytes, bdOut, stride ) )
		{ LLDEB(format,stride); goto ready;	}

	    break;

	case CAIRO_FORMAT_A1:
	    if  ( bmToNativeA1( bytes, riOut.riBytes, bdOut, stride ) )
		{ LLDEB(format,stride); goto ready;	}

	    break;

	default:
	    LDEB(format); goto ready;
	}

    image= cairo_image_surface_create_for_data( bytes, format,
			bdOut->bdPixelsWide, bdOut->bdPixelsHigh, stride );
    if  ( ! image )
	{ XDEB(image); goto ready;	}

    bytes= (unsigned char *)0; /* stolen */

  ready:

    if  ( bytes )
	{ free(bytes);	}

    bmCleanRasterImage( &riOut );
    bmCleanColorAllocator( &ca );

    return image;
    }

/************************************************************************/
/*									*/
/*  Make a drawing surface the can be used to draw an image or a	*/
/*  pattern on the parent. Typically, this is first filled and then	*/
/*  used to draw on the parent in an expose, or repeatedly to draw the	*/
/*  pattern on the parent.						*/
/*									*/
/************************************************************************/

DrawingSurface drawMakeDrawingSurfaceForParent(
				DrawingSurface			parent,
				int				wide,
				int				high )
    {
    DrawingSurface	ds= drawMakeChildDrawingSurface( parent );

    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    ds->dsSurface= gdk_window_create_similar_surface( parent->dsWindow,
					    CAIRO_CONTENT_COLOR, wide, high );
    if  ( ! ds->dsSurface )
	{
	XDEB(ds->dsSurface);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    ds->dsCairoContext= cairo_create( ds->dsSurface );

    return ds;
    }

/************************************************************************/

DrawingSurface drawMakeDrawingSurfaceForImageAndParent(
				DrawingSurface			parent,
				const struct RasterImage *	ri,
				const DocumentRectangle *	drSrc,
				int				wide,
				int				high )
    {
    DocumentRectangle	drDest;
    DrawingSurface	ds= drawMakeChildDrawingSurface( parent );

    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    drDest.drX0= 0;
    drDest.drY0= 0;
    drDest.drX1= wide- 1;
    drDest.drY1= high- 1;

    ds->dsSurface= drawMakeImageSurface( &drDest, ri, drSrc );
    if  ( ! ds->dsSurface )
	{
	XDEB(ds->dsSurface);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    return ds;
    }

/************************************************************************/

int drawRasterImage(		DrawingSurface			ds,
				const DocumentRectangle *	drDest,
				const struct RasterImage *	ri,
				const DocumentRectangle *	drSrc )
    {
    int			rval= 0;

    cairo_surface_t *		image= (cairo_surface_t *)0;

    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); rval= -1; goto ready;	}

    image= drawMakeImageSurface( drDest, ri, drSrc );
    if  ( ! image )
	{ XDEB(image); rval= -1; goto ready;	}

    cairo_save( ds->dsCairoContext );
    cairo_set_source_surface( ds->dsCairoContext, image,
						drDest->drX0, drDest->drY0 );
    cairo_paint( ds->dsCairoContext );
    cairo_restore( ds->dsCairoContext );

    cairo_surface_destroy( image );

  ready:

    return rval;
    }

/************************************************************************/

void drawChildSurface(		DrawingSurface			ds,
				const DrawingSurface		child,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drChild )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return;	}
    if  ( ! child->dsSurface )
	{ XDEB(ds->dsSurface); return;	}

    cairo_save( ds->dsCairoContext );
    cairo_set_source_surface ( ds->dsCairoContext, child->dsSurface,
							    xDest, yDest );
    cairo_paint( ds->dsCairoContext );
    cairo_restore( ds->dsCairoContext );

    return;
    }

/************************************************************************/
/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/************************************************************************/

int drawSetForegroundColor(	DrawingSurface		ds,
				const RGB8Color *	rgb8 )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    cairo_set_source_rgb( ds->dsCairoContext,
				rgb8->rgb8Red/ 255.0,
				rgb8->rgb8Green/ 255.0,
				rgb8->rgb8Blue/ 255.0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Destroy a drawing surface. Only free resources allocated by the	*/
/*  DrawingSurface itself.						*/
/*									*/
/************************************************************************/

void drawFreeDrawingSurface(	DrawingSurface		ds )
    {
    utilCleanNumberedPropertiesList( &(ds->dsScreenFontAdmin) );

    if  ( ds->dsFontOptions )
	{
	cairo_font_options_destroy( ds->dsFontOptions );
	}

    if  ( ds->dsSurface )
	{
	if  ( ds->dsCairoContext )
	    { cairo_destroy( ds->dsCairoContext );	}

	cairo_surface_destroy( ds->dsSurface );
	}

    free( ds );
    }

/************************************************************************/
/*									*/
/*  Fill a rectangle.							*/
/*									*/
/************************************************************************/

void drawFillRectangle(	DrawingSurface			ds,
			const DocumentRectangle *	dr )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return;	}

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, dr->drX0, dr->drY0 );

    cairo_line_to( ds->dsCairoContext, dr->drX1+ 1, dr->drY0 );
    cairo_line_to( ds->dsCairoContext, dr->drX1+ 1, dr->drY1+ 1 );
    cairo_line_to( ds->dsCairoContext, dr->drX0, dr->drY1+ 1 );
    cairo_line_to( ds->dsCairoContext, dr->drX0, dr->drY0 );

    cairo_close_path( ds->dsCairoContext );
    cairo_fill( ds->dsCairoContext );
    }

void drawRectangle(	DrawingSurface			ds,
			const DocumentRectangle *	dr )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return;	}

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, dr->drX0, dr->drY0 );

    cairo_line_to( ds->dsCairoContext, dr->drX1, dr->drY0 );
    cairo_line_to( ds->dsCairoContext, dr->drX1, dr->drY1 );
    cairo_line_to( ds->dsCairoContext, dr->drX0, dr->drY1 );
    cairo_line_to( ds->dsCairoContext, dr->drX0, dr->drY0 );

    cairo_close_path( ds->dsCairoContext );
    cairo_stroke( ds->dsCairoContext );
    }

/************************************************************************/
/*									*/
/*  Draw a line.							*/
/*									*/
/************************************************************************/

int drawLine(		DrawingSurface			ds,
			int				x0,
			int				y0,
			int				x1,
			int				y1 )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, x0, y0 );
    cairo_line_to( ds->dsCairoContext, x1, y0 );

    cairo_stroke( ds->dsCairoContext );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a series of lines.						*/
/*									*/
/************************************************************************/

int drawLines(		DrawingSurface			ds,
			const Point2DI *		points,
			int				pointCount,
			int				close )
    {
    int		i;

    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, points[0].x, points[0].y );

    for ( i= 1; i < pointCount; i++ )
	{
	cairo_line_to( ds->dsCairoContext, points[i].x, points[i].y );
	}

    if  ( close )
	{ cairo_close_path( ds->dsCairoContext );	}

    cairo_stroke( ds->dsCairoContext );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a polygon.							*/
/*									*/
/************************************************************************/

int drawFillPolygon(	DrawingSurface			ds,
			const Point2DI *		points,
			int				pointCount )
    {
    int		i;

    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, points[0].x, points[0].y );

    for ( i= 1; i < pointCount; i++ )
	{
	cairo_line_to( ds->dsCairoContext, points[i].x, points[i].y );
	}

    cairo_close_path( ds->dsCairoContext );

    cairo_fill( ds->dsCairoContext );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set line attributes.						*/
/*									*/
/************************************************************************/

# if 0
static const int LineStyleMap[LineStyle_Count]=
    {
    CAIRO_LINE_STYLE_SOLID,
    CAIRO_LINE_STYLE_ON_OFF_DASH,
    CAIRO_LINE_STYLE_DOUBLE_DASH,
    };
# endif

static const int CapStyleMap[LineCap_Count]=
    {
    CAIRO_LINE_CAP_BUTT,	/* CAIRO_LINE_CAP_NOT_LAST, */
    CAIRO_LINE_CAP_BUTT,
    CAIRO_LINE_CAP_ROUND,
    CAIRO_LINE_CAP_SQUARE
    };

static const int JoinStyleMap[LineJoin_Count]=
    {
    CAIRO_LINE_JOIN_MITER,
    CAIRO_LINE_JOIN_ROUND,
    CAIRO_LINE_JOIN_BEVEL
    };

int drawSetLineAttributes(	DrawingSurface		ds,
				int			lineWidth,
				int			lineStyle,
				int			capStyle,
				int			joinStyle,
				const unsigned char *	dashList,
				int			dashCount )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    cairo_set_line_width( ds->dsCairoContext, lineWidth );
    cairo_set_line_cap( ds->dsCairoContext, CapStyleMap[capStyle] );
    cairo_set_line_join( ds->dsCairoContext, JoinStyleMap[joinStyle] );

    return 0;
    }

void drawArc(			DrawingSurface		ds,
				const Arc2DI *		arc )
    {
LDEB(1);
    }

void drawFillArc(		DrawingSurface		ds,
				const Arc2DI *		arc )
    {
LDEB(1);
    }

/************************************************************************/

cairo_scaled_font_t * drawOpenScreenFontCairo(
						const DrawingSurface	ds,
						DrawScreenFont *	dsf )
    {
    cairo_font_face_t *		cairoFace;
    const int			loadFlags= 0;

    cairo_scaled_font_t *	scaledFace;

    cairo_matrix_t		fm;
    cairo_matrix_t		ctm;

    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return (cairo_scaled_font_t *)0;	}
    if  ( ! dsf->dsfFT_Face )
	{ XDEB(dsf->dsfFT_Face); return (cairo_scaled_font_t *)0;	}

    cairoFace= cairo_ft_font_face_create_for_ft_face(
					    dsf->dsfFT_Face, loadFlags );
    if  ( ! cairoFace )
	{ XDEB(cairoFace); return (cairo_scaled_font_t *)0;	}

    cairo_matrix_init( &fm,
		    dsf->dsfSizePixels, 0, 0, dsf->dsfSizePixels, 0, 0 );
    cairo_get_matrix( ds->dsCairoContext, &ctm );

    scaledFace= cairo_scaled_font_create( cairoFace, &fm, &ctm,
						    ds->dsFontOptions );
    if  ( ! scaledFace )
	{ XDEB(scaledFace); return (cairo_scaled_font_t *)0;	}

    return scaledFace;
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

int drawString(		DrawingSurface			ds,
			int				x0,
			int				y0,
			int				screenFont,
			const char *			s,
			int				len )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return -1;	}

    if  ( ds->dsCurrentScreenFont != screenFont )
	{
	const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
	DrawScreenFont *		dsf;
	cairo_scaled_font_t *		scaledFace;

	dsf= drawGetScreenFontByNumber( npl, screenFont );
	if  ( ! dsf )
	    { LXDEB(screenFont,dsf); return -1;	}

	scaledFace= drawOpenScreenFontCairo( ds, dsf );
	if  ( ! scaledFace )
	    { XDEB(scaledFace); return -1;	}

	cairo_set_scaled_font( ds->dsCairoContext, scaledFace );
	cairo_scaled_font_destroy( scaledFace );
	}

	cairo_move_to( ds->dsCairoContext, x0, y0 );

	/* HACK */
	if  ( s[len] )
	    {
	    char sav= s[len]; ((char *)s)[len]= '\0';
	    cairo_show_text( ds->dsCairoContext, s );
	    ((char *)s)[len]= sav;
	    }
	else{
	    cairo_show_text( ds->dsCairoContext, s );
	    }

    return 0;
    }

int drawOpenScreenFont(		DrawingSurface			ds,
				const struct AfmFontInfo *	afi,
				int				pixelSize,
				const IndexSet *		unicodesWanted )
    {
    DrawScreenFont *	dsf;
    int			fresh;
    int			screenFont;

    screenFont= drawGetScreenFont( &dsf, &fresh, &(ds->dsScreenFontAdmin),
					    unicodesWanted, afi, pixelSize );
    if  ( screenFont < 0 )
	{ LDEB(screenFont); return -1;	}

    if  ( fresh )
	{
	if  ( drawFontOpenScreenFont( dsf, 0 ) )
	    { LDEB(1); return -1;	}
	}

    return screenFont;
    }

void drawNoClipping(		DrawingSurface		ds )
    {
    /* NOTE: Not on the base surface of the window! */

    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return;	}

    cairo_reset_clip( ds->dsCairoContext );

    return;
    }

void drawSetClipRect(	DrawingSurface			ds,
			const DocumentRectangle *	drClip )
    {
    if  ( ! ds->dsCairoContext )
	{ XDEB(ds->dsCairoContext); return;	}

    cairo_reset_clip( ds->dsCairoContext );

    cairo_new_path( ds->dsCairoContext );
    cairo_move_to( ds->dsCairoContext, drClip->drX0, drClip->drY0 );

    cairo_line_to( ds->dsCairoContext, drClip->drX1, drClip->drY0 );
    cairo_line_to( ds->dsCairoContext, drClip->drX1, drClip->drY1 );
    cairo_line_to( ds->dsCairoContext, drClip->drX0, drClip->drY1 );
    cairo_line_to( ds->dsCairoContext, drClip->drX0, drClip->drY0 );

    cairo_close_path( ds->dsCairoContext );
    cairo_clip( ds->dsCairoContext );

    return;
    }


void drawMoveArea(		DrawingSurface			ds,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drSrc )
    {
    cairo_rectangle_int_t	rect;
    cairo_region_t *		region;

    if  ( ! ds->dsWindow )
	{ XDEB(ds->dsWindow); return;	}

    rect.x= drSrc->drX0;
    rect.y= drSrc->drY0;
    rect.width= drSrc->drX1- drSrc->drX0+ 1;
    rect.height= drSrc->drY1- drSrc->drY0+ 1;

    region= cairo_region_create_rectangle( &rect );

    gdk_window_move_region( ds->dsWindow, region,
				    xDest- drSrc->drX0,
				    yDest- drSrc->drY0 );

    cairo_region_destroy( region );
    }

#   endif
