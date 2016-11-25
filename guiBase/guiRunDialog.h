/************************************************************************/
/*									*/
/*  Run (modal) dialogs.						*/
/*									*/
/************************************************************************/

#   ifndef	GUI_RUN_DIALOG_H
#   define	GUI_RUN_DIALOG_H

#   include	"guiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/


extern void guiMakeVerticalDialog(	AppDialog *		ad,
					APP_WIDGET *		pPaned,
					APP_WIDGET		topLevel,
					APP_CLOSE_CALLBACK_T	closeCallback,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through );

extern void guiRunDialog( 		AppDialog *		ad,
					int			initial,
					APP_CONTEXT		appContext );

extern void guiBreakDialog( 		AppDialog *		ad,
					int			response );

extern void guiShowDialog(		APP_CONTEXT		appContext,
					AppDialog *		ad,
					APP_WIDGET		relative );

extern void guiHideDialog(		AppDialog *		ad );

# ifdef __cplusplus
    }
# endif

#   endif
