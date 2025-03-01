/************************************************************************/
/*									*/
/*  Definitions relating to rulers.					*/
/*									*/
/************************************************************************/

#   include	<guiBase.h>

struct TabStopList;
struct EditDocument;
struct RGB8Color;
struct PostScriptFontList;
struct ColumnSeparator;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedDocSetLeftRuler(		struct EditDocument *	ed );
extern int tedDocSetRightRuler(		struct EditDocument *	ed );
extern int tedDocSetTopRuler(		struct EditDocument *	ed );
extern int tedDocSetBottomRuler(	struct EditDocument *	ed );

extern void tedVerticalRulerSetBackground( void *		voidrd,
					const struct RGB8Color *	back );
extern void tedTopRulerSetBackground(	void *			voidttr,
					const struct RGB8Color *	back );
extern void tedBottomRulerSetBackground( void *			voidtbr,
					const struct RGB8Color *	back );

extern void * tedMakeTopRuler(
			int				height,
			double				magnifiedPixelsPerTwip,
			double				magnification,
			const struct PostScriptFontList *	psfl,

			int				leftRulerWidth,
			int				rightRulerWidth,

			int				docX0,
			int				docX1,
			int				marginC0,
			int				marginC1,
			int				docVisibleX0,
			int				docVisibleX1,

			int				whatUnit );

extern void * tedMakeBottomRuler(
			const struct PostScriptFontList *	psfl,
			int				height,
			int				leftRulerWidth,
			int				rightRulerWidth,
			const char *			pageFormat );

extern void tedFreeVerticalRuler(	void *		voidrd );
extern void tedFreeTopRuler(		void *		voidttr );
extern void tedFreeBottomRuler(		void *		voidtlr );

extern APP_REDRAW_HANDLER_H( tedRedrawTopRuler, w, voidttr, event );
extern APP_REDRAW_HANDLER_H( tedRedrawBottomRuler, w, voidtlr, event );

extern void tedScrollTopRuler(		void *			voidttr,
					APP_WIDGET		w,
					int			d );

extern void tedSetTopRulerRange(	void *			voidttr,
					APP_WIDGET		w,
					int			docFullX1,
					int			docVisX0,
					int			docVisX1 );

extern void tedAdaptTopRuler(	void *			voidttr,
				APP_WIDGET		w,
				int			docX0,
				int			docX1,
				int			marginC0,
				int			marginC1,
				int			firstIndent,
				int			leftIndent,
				int			rightIndent,
				int			paraFrameX0,
				int			paraFrameX1,
				const struct TabStopList *	tslSet );

extern int tedTopRulerFindMouse(	int *			pIndex,
					int			mouseX,
					int			mouseY,
					struct EditDocument *		ed );

extern int tedTopRulerTrackMouse( 	int *			pFirstIndent,
					int *			pLeftIndent,
					int *			pRightIndent,
					struct TabStopList *	tsl,
					int *			pCsCount,
					struct ColumnSeparator **	pCs,
					APP_WIDGET		w,
					APP_EVENT *		downEvent,
					struct EditDocument *		ed );

extern APP_EVENT_HANDLER_H( tedTopRulerConfigure, w, voidttr, event );
extern APP_EVENT_HANDLER_H( tedBottomRulerConfigure, w, voidtlr, event );

extern int tedSetRulerColumns(	APP_WIDGET		w,
				void *			voidttr,
				int			bfX0Pixels,
				int			bfX1Pixels,
				const struct ColumnSeparator *	cs,
				int			csCount );

extern void tedSetBottomRulerValues(
				void *			voidtbr,
				APP_WIDGET		w,
				int			page,
				int			pages,
				int			section,
				int			sections );

