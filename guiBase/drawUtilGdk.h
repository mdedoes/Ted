/************************************************************************/
/*									*/
/*  Gtk/Gdk implementation utility functions. Always use via		*/
/*  drawUtilImpl.h							*/
/*									*/
/************************************************************************/

#   if USE_GTK /* { */

#   include	"guiBase.h"
#   include	"drawImpl.h"
#   include	"guiImageImpl.h"

struct RasterImage;
struct AppColors;
struct AppXftColorList;
struct DocumentRectangle;

# if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 22
#	define gdk_visual_get_visual_type( v ) ((v)->type)
#	define gdk_visual_get_depth( v ) ((v)->depth)
# endif

/************************************************************************/
/*									*/
/*  Utility Routines							*/
/*									*/
/************************************************************************/

extern int drawUtilGdkMakeImage(
			APP_IMAGE **			pPimage,
			int				toWide,
			int				toHigh,
			struct AppColors *		ac,
			const struct RasterImage *	ri,
			const struct DocumentRectangle * drSrc );

# if DRAW_XFT
    extern XftDraw * drawGtkXftDrawCreate(
					GdkDrawable *		drawable,
					struct AppXftColorList * axcl );
# endif

# if DRAW_GDK
    extern void drawGdkSetXFillRule(	GdkGC *			gc,
					int			rule );
# endif

extern struct AppColors * drawGetColorsGdk( void );

extern void guiCollectExposuresGtk(	struct DocumentRectangle *	drClip,
					GdkWindow *			window,
					APP_EVENT *			event );

#   endif	/* } USE_GTK */
