/************************************************************************/
/*									*/
/*  Motif implementation utility functions. Always use via		*/
/*  drawUtilImpl.h							*/
/*									*/
/************************************************************************/

#   if USE_MOTIF /* { */

#   include	"guiBase.h"
#   include	<X11/Xlib.h>

struct RasterImage;
struct DocumentRectangle;
struct AppColors;

extern const char * const APP_VisualClasses[];

/************************************************************************/
/*									*/
/*  Utility Routines							*/
/*									*/
/************************************************************************/

extern int drawUtilMotifMakeImage(
			Display *			display,
			int				screen,
			XImage **			pPimage,
			int				toWide,
			int				toHigh,
			struct AppColors *		ac,
			const struct RasterImage *	ri,
			const struct DocumentRectangle * drSel );

extern struct AppColors *	drawGetColorsMotif( Display *	display );

#   endif	/* } USE_MOTIF */
