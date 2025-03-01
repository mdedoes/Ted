/************************************************************************/
/*									*/
/*  Callbacks to the application object.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_APP_FRONT_H
#   define	APP_APP_FRONT_H

#   include	<guiBase.h>

#   if ! USE_HEADLESS

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, e );

extern APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e );
extern APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, e );

extern APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea );

extern APP_EVENT_HANDLER_H( appAppGotPasteCall, w, voidea, event );

# ifdef __cplusplus
    }
# endif

#   endif

#   endif
