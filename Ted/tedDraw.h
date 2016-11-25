/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  guiExposeDrawingWidget().						*/
/*									*/
/************************************************************************/

#   ifndef	TED_DRAW_H
#   define	TED_DRAW_H

struct EditDocument;
struct TedDocument;
struct EditApplication;
struct DocumentRectangle;

#   define	LOG_REDRAWS	0
#   define	BLINK_IBAR	1

#   if		LOG_REDRAWS
#	undef	BLINK_IBAR
#	define	BLINK_IBAR	0
#   endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedStartCursorBlink(	struct EditDocument *		ed );
extern void tedStopCursorBlink(		struct EditDocument *		ed );

extern void tedHideObjectWindows(	struct EditDocument *		ed );
extern void tedDestroyObjectWindows(	struct TedDocument *		ed );

extern void tedCleanCursorBlink(	struct TedDocument *		td );

extern int tedMakeDocumentCursor(	struct EditApplication *	ea );

extern void tedDrawRectangle(	struct EditDocument *		ed,
				struct DocumentRectangle *	drClip );

#   endif	/*  TED_DRAW_H	*/
