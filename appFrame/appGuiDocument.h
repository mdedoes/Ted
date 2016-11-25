/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_DOCUMENT_H
#   define	APP_GUI_DOCUMENT_H

#   include	<guiBase.h>

#   if ! USE_HEADLESS

struct EditDocument;
struct MemoryBuffer;
struct PrintGeometry;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appRunReallyCloseDialog(	APP_WIDGET		option,
					struct EditDocument *	ed );

extern void appDocPrintToFile(	APP_WIDGET			option,
				APP_WIDGET			panel,
				struct EditDocument *		ed,
				const struct PrintGeometry *	pg );

# if USE_GTK

extern void appDocGotPasteReplyGtk(	GtkWidget *		w,
					GtkSelectionData *	gsd,
					guint			time,
					void *			voided );

extern void appAppGotPasteReplyGtk(	GtkWidget *		w,
					GtkSelectionData *	gsd,
					guint			time,
					void *			voidea );
# endif

extern void appScrollToPosition( int *				pScrolledX,
				int *				pScrolledY,
				struct EditDocument *		ed,
				int				x,
				int				y );

extern void appDocFillMenu(	struct EditDocument *		ed );

extern void appSetWindowsItemState(	APP_WIDGET	menu,
					struct EditDocument *	ed,
					int		changed );

extern void appRemoveWindowsOption(	APP_WIDGET		menu,
					struct EditDocument *	oldEd );

extern void appRenameWindowsOption(	APP_WIDGET		menu,
					struct EditDocument *	ed,
					const struct MemoryBuffer *	title );

extern void appAppSetWindowsOption(	APP_WIDGET		menu,
					struct EditDocument *	ed,
					const struct MemoryBuffer *	title );

extern void appDocExposeRectangle(
				const struct EditDocument *	ed,
				const struct DocumentRectangle *	drChanged,
				int				scrolledX,
				int				scrolledY );

extern void appDocScrollToY(		struct EditDocument *		ed,
					int				y );
extern void appDocScrollToX(		struct EditDocument *		ed,
					int				x );

extern int appDocSaveDocument(	struct EditDocument *	ed,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			format,
				const struct MemoryBuffer *	filename );

#   endif

#   endif
