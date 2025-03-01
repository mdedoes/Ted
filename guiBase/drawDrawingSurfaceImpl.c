/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "guiBaseConfig.h"

#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawScreenFontAdmin.h"
#   include "drawDrawingSurfacePrivate.h"
#   include "drawImpl.h"

#   include <appDebugon.h>

#   if ! USE_HEADLESS

int drawFontImplementationName(
			MemoryBuffer *			target,
			const DrawingSurface		ds,
			int				screenFont )
    {
    return drawFontImplementationNameImpl( target,
					&(ds->dsScreenFontAdmin), screenFont );
    }

#   endif

void drawInitDrawingSurface(	DrawingSurface	ds )
    {
#   if DRAW_X11 || DRAW_GDK || DRAW_XFT
    ds->dsColors= (AppColors *)0;

    ds->dsPoints= (APP_POINT *)0;
    ds->dsPointCount= 0;
#   endif

    ds->dsIsImage= 0;

#   if DRAW_XFT
    appInitAppXftColorList( &(ds->dsXftColorList) );
    ds->dsXftDrawable= (XftDraw *)0;
#   endif

#   if DRAW_GDK
    ds->dsDrawable= (GdkDrawable *)0;
    ds->dsGc= (GdkGC *)0;
#   endif

#   if DRAW_X11
    ds->dsDrawable= (Drawable)0;
    ds->dsDisplay= (Display *)0;
    ds->dsScreen= 0;
    ds->dsGc= (GC)0;
#   endif

#   if DRAW_GDK || DRAW_X11
    ds->dsAvoidFontconfig= 0;
#   endif

#   if DRAW_CAIRO
    ds->dsWindow= (GdkWindow *)0;
    ds->dsSurface= (cairo_surface_t *)0;
    ds->dsCurrentScreenFont= -1;
    ds->dsFontOptions= (cairo_font_options_t *)0;
	    /*
	     *  Only set while we are actually drawing!
	     */
    ds->dsCairoContext= (cairo_t *)0;
#   endif

    drawStartScreenFontList( &(ds->dsScreenFontAdmin) );

    return;
    }

