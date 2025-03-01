/************************************************************************/
/*									*/
/*  A widget in the screen GUI. (Implementation)			*/
/*									*/
/************************************************************************/

#   include "guiBase.h"

struct RGB8Color;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

#   if ! USE_HEADLESS

extern void guiExposeDrawingWidget(	APP_WIDGET	nativeWidget );
extern void guiExposeDrawingWidgetRectangle(
				APP_WIDGET			nativeWidget,
				const struct DocumentRectangle *	dr );

extern void guiGetForegroundColor(	struct RGB8Color *	rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetBackgroundColor(	struct RGB8Color *	rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetTopShadowColor(	struct RGB8Color *	rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetBottomShadowColor(	struct RGB8Color *	rgb8,
					APP_WIDGET		nativeWidget );

extern int guiDrawGetSizeOfWidget(		int *		pWide,
						int *		pHigh,
						APP_WIDGET	w );

extern int guiDrawGetSizeFromConfigureEvent(	int *		pWide,
						int *		pHigh,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern int guiGetCoordinatesFromMouseButtonEvent(
					int *			pX,
					int *			pY,
					int *			pButton,
					int *			pUpDown,
					int *			pSeq,
					unsigned int *		pKeyState,
					const APP_WIDGET	w,
					const APP_EVENT *	event );

extern int guiGetCoordinatesFromMouseMoveEvent( int *		pX,
						int *		pY,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern void guiDrawSetRedrawHandler(	APP_WIDGET		w,
					APP_REDRAW_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetConfigureHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetButtonPressHandler( APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetScrollHandler( 	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern int guiDrawGetInoutFromFocusEvent(	int *		pInOut,
						APP_WIDGET	w,
						APP_EVENT *	event );

#   endif

