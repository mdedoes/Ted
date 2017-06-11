/************************************************************************/
/*									*/
/*  Actions in the GTK interface that cannot be done without using	*/
/*  plain old X11, or some knowledge of the relationship between GTK	*/
/*  and X11.								*/
/*									*/
/*  The author is aware of the fact that he should not have done this.	*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   if USE_GTK
#   define	USE_GDK_DEPRECATED
#   include	"guiBase.h"
#   endif /* GTK */

#   if DRAW_GDK

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"drawUtilGdk.h"
#   include	"drawImpl.h"
#   include	"drawColorsXft.h"

#   include	<appDebugon.h>

#   include	<gdk/gdkx.h>

void drawGdkSetXFillRule(	GdkGC *		gc,
				int		rule )
    {
#   if GTK_MAJOR_VERSION < 2
    GdkGCPrivate *	private= (GdkGCPrivate*)gc;
    GC			xgc= private->xgc;
#   else
    GC			xgc= gdk_x11_gc_get_xgc( gc );
#   endif


    switch( rule )
	{
	case GDK_EVEN_ODD_RULE:	rule= EvenOddRule; break;
	case GDK_WINDING_RULE:	rule= WindingRule; break;
	default:
	    LDEB(rule); rule= EvenOddRule; break;
	}

    XSetFillRule( GDK_DISPLAY(), xgc, rule );
    }

# if DRAW_XFT

XftDraw * drawGtkXftDrawCreate(	GdkDrawable *		drawable,
				AppXftColorList *	axcl )
    {
    GdkVisual *		gdk_vis= gdk_visual_get_system();
    GdkColormap *	gdk_cmap= gdk_colormap_get_system();

    Drawable		x_drawable;
    XftDraw *		xftDraw;

    if  ( ! drawable )
	{ XDEB(drawable); return (XftDraw *)0;	}

    axcl->axclDisplay= GDK_WINDOW_XDISPLAY( drawable );

#   if GTK_MAJOR_VERSION >= 2

    x_drawable= GDK_WINDOW_XID( drawable );
    axcl->axclVisual= gdk_x11_visual_get_xvisual( gdk_vis );
    axcl->axclColormap= gdk_x11_colormap_get_xcolormap( gdk_cmap );

#   else

    x_drawable= GDK_WINDOW_XWINDOW( drawable );
    axcl->axclVisual= GDK_VISUAL_XVISUAL( gdk_vis );
    axcl->axclColormap= GDK_COLORMAP_XCOLORMAP( gdk_cmap );

#   endif

    xftDraw= XftDrawCreate( axcl->axclDisplay, x_drawable,
				    axcl->axclVisual, axcl->axclColormap );

    if  ( ! xftDraw )
	{ XDEB(xftDraw);	}

    return xftDraw;
    }

# endif

#   endif
