/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "guiBaseConfig.h"

#   if USE_GTK

#   define	USE_GDK_DEPRECATED

#   include "guiWidgetDrawingSurface.h"
#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawUtilGdk.h"
#   include "drawImpl.h"
#   include "drawDrawingSurfacePrivate.h"
#   include "guiDrawingWidget.h"
#   include <geoRectangle.h>

#   include <appDebugon.h>

void guiExposeDrawingWidget(	APP_WIDGET	nativeWidget )
    {
    GdkRectangle	rect;
    GdkWindow *		win= gtk_widget_get_window( nativeWidget );

    if  ( ! win )
	{
	gtk_widget_realize( nativeWidget );
	win= gtk_widget_get_window( nativeWidget );
	}

    rect.x= 0;
    rect.y= 0;
    rect.width= 0;
    rect.height= 0;

#   if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 22

    gdk_drawable_get_size( nativeWidget->window,
				    &(rect.width), &(rect.height) );

#   else

    rect.width=  gdk_window_get_width( win );
    rect.height= gdk_window_get_height( win );

#   endif

    gdk_window_invalidate_rect( win, &rect, TRUE );
    /*
    gtk_widget_queue_draw_area( nativeWidget, x, y, wide, high );
    */

    return;
    }

void guiExposeDrawingWidgetRectangle(
				APP_WIDGET			nativeWidget,
				const DocumentRectangle *	drExpose )
    {
    int			x= drExpose->drX0;
    int			y= drExpose->drY0;
    int			wide= drExpose->drX1- drExpose->drX0+ 1;
    int			high= drExpose->drY1- drExpose->drY0+ 1;

    GdkWindow *		win= gtk_widget_get_window( nativeWidget );

    if  ( ! win )
	{
	gtk_widget_realize( nativeWidget );
	win= gtk_widget_get_window( nativeWidget );
	}


#   if GTK_MAJOR_VERSION < 2
    gdk_window_clear_area_e( win, x, y, wide, high );
#   else

    {
    GdkRectangle	rect;

    rect.x= x;
    rect.y= y;
    rect.width= wide;
    rect.height= high;

    gdk_window_invalidate_rect( win, &rect, TRUE );
    /*
    gtk_widget_queue_draw_area( nativeWidget, x, y, wide, high );
    */
    }

#   endif

    return;
    }

#   if DRAW_GDK
#   define	LOG_EXPOSES	0

void guiCollectExposuresGtk(	DocumentRectangle *	drClip,
				GdkWindow *		window,
				APP_EVENT *		event )
    {
    drClip->drX0= event->expose.area.x;
    drClip->drY0= event->expose.area.y;
    drClip->drX1= event->expose.area.x+ event->expose.area.width;
    drClip->drY1= event->expose.area.y+ event->expose.area.height;

#   if LOG_EXPOSES
    appDebug( "======: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    while( TRUE )
	{
	GdkEvent *		nxEvent;
	DocumentRectangle	drMore;

	nxEvent= gdk_event_peek();
	if  ( ! nxEvent )
	    { break;	}

	if  ( nxEvent->type != GDK_EXPOSE	||
	      nxEvent->any.window != window	)
	    { gdk_event_free( nxEvent ); break;	}

	gdk_event_free( nxEvent );

	nxEvent= gdk_event_get();
	if  ( ! nxEvent )
	    { XDEB(nxEvent); break;	}

	drMore.drX0= nxEvent->expose.area.x;
	drMore.drY0= nxEvent->expose.area.y;
	drMore.drX1= nxEvent->expose.area.x+ nxEvent->expose.area.width;
	drMore.drY1= nxEvent->expose.area.y+ nxEvent->expose.area.height;

	gdk_event_free( nxEvent );

#	if LOG_EXPOSES
	appDebug( "++++++: [%4d+%4d]x[%4d+%4d]\n",
			drMore.drX0,
			drMore.drX1- drMore.drX0+ 1,
			drMore.drY0,
			drMore.drY1- drMore.drY0+ 1 );
#	endif

	geoUnionRectangle( drClip, drClip, &drMore );

#	if LOG_EXPOSES
	appDebug( "......: [%4d+%4d]x[%4d+%4d]\n",
			drClip->drX0,
			drClip->drX1- drClip->drX0+ 1,
			drClip->drY0,
			drClip->drY1- drClip->drY0+ 1 );
#	endif
	}

    return;
    }

void guiStartDrawing(	DocumentRectangle *	drClip,
			DrawingSurface		ds,
			APP_WIDGET		nativeWidget,
			APP_DRAW_EVENT *	event )
    {
    GdkWindow *		win= gtk_widget_get_window( nativeWidget );

    if  ( ! win )
	{
	gtk_widget_realize( nativeWidget );
	win= gtk_widget_get_window( nativeWidget );
	}

    guiCollectExposuresGtk( drClip, win, event );
    drawSetClipRect( ds, drClip );

    return;
    }

#   endif

#   if DRAW_CAIRO

void guiStartDrawing(	DocumentRectangle *	drClip,
			DrawingSurface		ds,
			APP_WIDGET		nativeWidget,
			APP_DRAW_EVENT *	event )
    {
    GdkWindow *		win= gtk_widget_get_window( nativeWidget );

    double		x0;
    double		y0;
    double		x1;
    double		y1;

    if  ( ! win )
	{
	gtk_widget_realize( nativeWidget );
	win= gtk_widget_get_window( nativeWidget );
	}

    ds->dsCairoContext= event;
    ds->dsCurrentScreenFont= -1;

    cairo_clip_extents( event, &x0, &y0, &x1, &y1 );

    drClip->drX0= x0;
    drClip->drY0= y0;
    drClip->drX1= x1;
    drClip->drY1= y1;

    return;
    }

#   endif

/************************************************************************/
/*									*/
/*  Make a drawing surface for a screen widget.				*/
/*									*/
/************************************************************************/

DrawingSurface guiDrawingSurfaceForNativeWidget(
					APP_WIDGET	nativeWidget,
					int		avoidFontconfig )
    {
    DrawingSurface	ds= malloc(sizeof(struct DrawingSurface));
    GdkWindow *		win= gtk_widget_get_window( nativeWidget );

    if  ( ! win )
	{
	gtk_widget_realize( nativeWidget );
	win= gtk_widget_get_window( nativeWidget );
	}

    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

#   if DRAW_GDK
    ds->dsColors= drawGetColorsGdk();
    if  ( ! ds->dsColors )
	{
	XDEB(ds->dsColors);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    ds->dsDrawable= win;
    ds->dsAvoidFontconfig= avoidFontconfig;

    ds->dsGc= gdk_gc_new( ds->dsDrawable );
    if  ( ! ds->dsGc )
	{
	XDEB(ds->dsGc);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}
#   endif

#   if DRAW_XFT
    if  ( ! ds->dsAvoidFontconfig )
	{
	ds->dsXftDrawable= drawGtkXftDrawCreate(
				ds->dsDrawable, &(ds->dsXftColorList) );
	if  ( ! ds->dsXftDrawable )
	    { XDEB(ds->dsXftDrawable);	}

#	if GTK_MAJOR_VERSION >= 2
	if  ( ds->dsXftDrawable )
	    { gtk_widget_set_double_buffered( nativeWidget, FALSE ); }
#	endif

	}
#   endif

#   if DRAW_CAIRO
    ds->dsWindow= win;

    ds->dsFontOptions= cairo_font_options_create();
    cairo_font_options_set_antialias( ds->dsFontOptions, CAIRO_ANTIALIAS_GOOD );
#   endif

    return ds;
    }

/************************************************************************/
/*									*/
/*  Get the font of a label widget.					*/
/*									*/
/************************************************************************/

int guiGetLabelFont(	DrawingSurface			ds,
			const struct PostScriptFontList *	psfl,
			APP_WIDGET			nativeLabel )
    {
#   if GTK_MAJOR_VERSION >= 2
    GtkWidget *		labelw= gtk_bin_get_child( GTK_BIN( nativeLabel ) );
#   else
    GtkWidget *		labelw= GTK_BIN( nativeLabel )->child;
#   endif
    const int		fontSizeHintPixels= -1;

    if  ( ! GTK_IS_BIN( nativeLabel ) )
	{ XDEB(nativeLabel);	}

    return drawOpenDefaultFont( ds, psfl, labelw, fontSizeHintPixels );
    }

/************************************************************************/
/*									*/
/*  Get colors.								*/
/*									*/
/************************************************************************/

void guiGetForegroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    GtkStyle *		gs= gtk_widget_get_style( nativeWidget );
    const GdkColor *	xc= &(gs->fg[GTK_STATE_NORMAL]);

    rgb8->rgb8Red= xc->red/ 256;
    rgb8->rgb8Green= xc->green/ 256;
    rgb8->rgb8Blue= xc->blue/ 256;
    rgb8->rgb8Alpha= 255;

    return;
    }

void guiGetBackgroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    GtkStyle *		gs= gtk_widget_get_style( nativeWidget );
    const GdkColor *	xc= &(gs->bg[GTK_STATE_NORMAL]);

    rgb8->rgb8Red= xc->red/ 256;
    rgb8->rgb8Green= xc->green/ 256;
    rgb8->rgb8Blue= xc->blue/ 256;
    rgb8->rgb8Alpha= 255;

    return;
    }

void guiGetTopShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetBackgroundColor( rgb8, nativeWidget );

    utilGetTopShadowColor( rgb8, rgb8 );

    return;
    }

void guiGetBottomShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetBackgroundColor( rgb8, nativeWidget );

    utilGetBottomShadowColor( rgb8, rgb8 );

    return;
    }

#   endif
