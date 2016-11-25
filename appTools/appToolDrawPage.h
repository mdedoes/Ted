/************************************************************************/
/*									*/
/*  Draw page diagrams on tool windows.					*/
/*									*/
/************************************************************************/

#   ifndef	GUI_DRAW_PAGE_H
#   define	GUI_DRAW_PAGE_H

#   include	<guiBase.h>
#   include	<drawDrawingSurface.h>

struct RGB8Color;
struct DocumentGeometry;
struct PrintGeometry;

# ifdef __cplusplus
extern "C"
    {
# endif

#   define	A3_MM_HIGH	420

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appDrawPageDiagram(	APP_WIDGET			w,
				DrawingSurface			ds,
				const struct RGB8Color	*	backColor,
				double				pixelsPerTwip,
				const struct DocumentGeometry *	dg );

extern void appDrawNupDiagram(	APP_WIDGET			w,
				DrawingSurface			ds,
				const struct RGB8Color	*	backColor,
				int				screenFont,
				double				pixelsPerTwip,
				const struct DocumentGeometry *	dgPage,
				const struct PrintGeometry *	pg );

extern APP_WIDGET appMakePageDrawing(	APP_WIDGET		parent,
					double			pixelsPerTwip,
					int			widgetHighMm,
					APP_REDRAW_HANDLER_T	redraw,
					void *			through );

# ifdef __cplusplus
    }
# endif

#   endif
