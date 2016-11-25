#   ifndef	DRAW_COLORS_H
#   define	DRAW_COLORS_H

#   include	"guiBase.h"
#   include	<bmcolor.h>

# ifdef __cplusplus
extern "C"
    {
# endif

#   if ! USE_HEADLESS

/************************************************************************/
/*									*/
/*  Administration on colors.						*/
/*									*/
/************************************************************************/

typedef struct AppColors
    {
    ColorAllocator	acAllocator;

#   if		USE_MOTIF
    Display *		acDisplay;
    Colormap		acColormap;
    int			acVisualClass;
#   endif

#   if			DRAW_GDK
    GdkColormap *	acColormap;
    int			acVisualClass;
#   endif
    } AppColors;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void drawInitColors(	AppColors *		ac );

extern int drawColorRgb(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int drawColorFindRgb(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int drawColorNamed(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				const char *		name );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif	/*  DRAW_COLORS_H	*/
