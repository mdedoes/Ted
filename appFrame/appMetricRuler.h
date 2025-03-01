/************************************************************************/
/*									*/
/*  Definitions relating to rulers.					*/
/*									*/
/************************************************************************/

#   include	<guiBase.h>
#   include	<drawDrawingSurface.h>

struct EditApplication;
struct EditDocument;
struct PostScriptFontList;
struct DocumentRectangle;
struct RulerData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * appMakeMetricRuler(
			int				sizeAcross,
			double				magnifiedPixelsPerTwip,
			double				magnification,
			const struct PostScriptFontList *	psfl,

			int				minUnused,
			int				maxUnused,

			int				documentC0,
			int				documentC1,
			int				visibleC0,
			int				visibleC1,

			int				whatUnit );

extern void appFreeMetricRuler(	void *		voidmr );

extern APP_REDRAW_HANDLER_H( appRedrawHorizontalMetricRuler, w, voidmr, event );
extern APP_REDRAW_HANDLER_H( appRedrawVerticalMetricRuler, w, voidmr, event );

extern APP_EVENT_HANDLER_H( appHorizontalRulerConfigure, w, voidmr, event );
extern APP_EVENT_HANDLER_H( appVerticalRulerConfigure, w, voidmr, event );

extern void appHorizontalRulerTrackMouse( int *		pX0pixels,
					int *			pX1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					struct EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					struct EditDocument *		ed );

extern void  appVerticalRulerTrackMouse( int *			pY0pixels,
					int *			pY1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					struct EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					struct EditDocument *		ed );

extern void appScrollHorMetricRuler(	void *			voidmr,
					APP_WIDGET		w,
					int			d );

extern void appScrollVertMetricRuler(	void *			voidmr,
					APP_WIDGET		w,
					int			d );

extern void appSetMetricRulerRange(	void *			voidmr,
					APP_WIDGET		w,
					int			docFullV1,
					int			docVisV0,
					int			docVisV1 );

extern void appSetMetricRulerValues(	APP_WIDGET	w,
					void *		voidmr,
					int		v0,
					int		v1 );

extern void appSetMetricRulerMagnification(	APP_WIDGET	w,
					void *		voidmr,
					double		magnifiedPixelsPerTwip,
					double		magnification );

extern void appAdaptMetricRuler(	APP_WIDGET	w,
					void *		voidmr,
					int		documentC1 );

extern void appDocSetMetricTopRuler(	struct EditDocument *	ed,
					int		topRulerHeight,
					double		magnification,
					int		unitInt,
					APP_EVENT_HANDLER_T	mouseDown );

extern void appDocSetMetricLeftRuler(	struct EditDocument *	ed,
					int		topRulerHeight,
					double		magnification,
					int		unitInt,
					APP_EVENT_HANDLER_T	mouseDown );

extern void appDrawRectangleSelection(
				struct DocumentRectangle *	drClip,
				struct DocumentRectangle *	drSelected,
				DrawingSurface			ds,
				const struct DocumentRectangle * drScreen,
				int				ox,
				int				oy );

extern void appRemoveRectangleSelection(
				const struct DocumentRectangle * drVisible,
				struct DocumentRectangle *	drSelected,
				APP_WIDGET			w );

extern void appSetHorRectangleSelection(
				struct DocumentRectangle *	drVisible,
				struct DocumentRectangle *	drSelected,
				int				x0Screen,
				int				x1Screen,
				APP_WIDGET			w );

extern void appSetVertRectangleSelection(
				struct DocumentRectangle *	drVisible,
				struct DocumentRectangle *	drSelected,
				int				y0Screen,
				int				y1Screen,
				APP_WIDGET			w );

extern void appMetricRulerDrawHair(	struct EditDocument *		ed );

extern void appMetricRulerExposeValue(	struct EditDocument *          ed );
extern void appMetricRulerChangeValue(	struct EditDocument *          ed,
					int			newValue );

extern void appHorizontalRulerDrawMark(	struct RulerData *	rd,
					int			fontBottom,
					int			tagY0,
					int			tagY1,
					int			tickY0,
					int			tickY1,
					int			iitem,
					int			tick,
					double			units,
					int			ox );
