/************************************************************************/
/*									*/
/*  Callbacks to the application object.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_DOC_FRONT_H
#   define	APP_DOC_FRONT_H

#   include	<guiBase.h>
# ifdef __cplusplus
extern "C"
    {
# endif

struct AppSelectionTargetType;
struct EditDocument;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern APP_EVENT_HANDLER_H( appScrollEventHandler, w, voided, scrollEvent );

extern APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, e );

extern APP_MENU_CALLBACK_H( appDocFileNew, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileSave, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileClose, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileQuit, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditCopy, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditCut, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditSelAll, option, voided, e );

extern APP_CLOSE_CALLBACK_H( appDocFileCloseCallback, w, voided );

extern APP_MENU_CALLBACK_H( appDocAbout, option, voided, e );

extern APP_EVENT_HANDLER_H( appDocCopyPasteHandler, w, voided, event );


extern APP_SCROLLBAR_CALLBACK_H( appDocHorizontalScrollbarCallback, bar, voided, e );
extern APP_SCROLLBAR_CALLBACK_H( appDocVerticalScrollbarCallback, bar, voided, e );

extern APP_REDRAW_HANDLER_H( appDocExposeHandler, w, d, exposeEvent );

extern APP_DESTROY_CALLBACK_H( appDestroyEditDocument, w, voided );


extern APP_EVENT_HANDLER_H( appDocConfigure, w, voided, event );

extern APP_GIVE_COPY( appDocReplyToCopyRequest, w, gsd, voided );

extern int appDocReleaseSelection(	struct EditDocument *	ed,
					const char *		selection );

extern int appDocOwnSelection(	struct EditDocument *		ed,
				const char *			selection,
				struct AppSelectionTargetType *	targets,
				int				targetCount );

extern int appDocAskForPaste(	struct EditDocument *	ed,
				const char *		selection );

extern void appScrollToRectangle( int *				pScrolledX,
				int *				pScrolledY,
				struct EditDocument *		ed,
				const struct DocumentRectangle *	dr );

# ifdef __cplusplus
    }
# endif

#   endif
