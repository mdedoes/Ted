/************************************************************************/
/*									*/
/*  A widget in the screen GUI. (Implementation)			*/
/*									*/
/************************************************************************/

#   include "guiBase.h"
#   include <drawDrawingSurface.h>

struct PostScriptFontList;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

#   if ! USE_HEADLESS

extern DrawingSurface guiDrawingSurfaceForNativeWidget(
				APP_WIDGET		nativeWidget,
				int			avoidFontconfig );

extern int guiGetLabelFont(
			DrawingSurface				ds,
			const struct PostScriptFontList *	psfl,
			APP_WIDGET				nativeLabel );

extern void drawMoveArea(	DrawingSurface			ds,
				int				xDest,
				int				yDest,
				const struct DocumentRectangle *	drSrc );

extern void guiStartDrawing(	struct DocumentRectangle *	drClip,
				DrawingSurface			ds,
				APP_WIDGET			nativeWidget,
				APP_DRAW_EVENT *		exposeEvent );

#   endif
