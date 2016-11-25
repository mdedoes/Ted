/************************************************************************/
/*									*/
/*  A surface to draw upon. (Implementation)				*/
/*									*/
/************************************************************************/

#   include <drawDrawingSurface.h>
#   include "drawImpl.h"
#   include "drawColorsXft.h"
#   include "drawColors.h"

struct DrawingSurface
    {
#   if DRAW_X11 || DRAW_GDK || DRAW_XFT

    AppColors *			dsColors;

    APP_POINT *			dsPoints;
    int				dsPointCount;

#   endif

#   if DRAW_X11
    GC				dsGc;
    Drawable			dsDrawable;
    Display *			dsDisplay;
    int				dsScreen;
    Atom			dsCurrentFID;
#   endif

#   if DRAW_GDK
    GdkGC *			dsGc;
    GdkDrawable *		dsDrawable;
#   endif

#   if DRAW_XFT
    XftDraw *			dsXftDrawable;
    AppXftColorList		dsXftColorList;
#   endif

    unsigned char		dsIsImage;
    NumberedPropertiesList	dsScreenFontAdmin;

#   if DRAW_CAIRO
				/**
				 *  The Window of a surface that draws on a 
				 *  window
				 */
    GdkWindow *			dsWindow;
				/**
				 *  The surface that we draw upon.
				 *  (For image type surfaces)
				 */
    cairo_surface_t *		dsSurface;
    int				dsCurrentScreenFont;
				/**
				 *  Do not allocate all the time.
				 */
    cairo_font_options_t *	dsFontOptions;
				/**
				 *  Only set while we are actually drawing!
				 */
    cairo_t *			dsCairoContext;
#   endif

#   if DRAW_X11 || DRAW_GDK
    unsigned char		dsAvoidFontconfig;
#   endif
    };

/************************************************************************/
/*									*/
/*  Utility functions.							*/
/*									*/
/************************************************************************/

extern void drawInitDrawingSurface(	DrawingSurface	ds );
