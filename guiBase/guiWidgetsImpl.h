/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_WIDGETS_IMPL_H
#   define	GUI_WIDGETS_IMPL_H

#   include	"guiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

#   if ! USE_HEADLESS

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void guiEmptyParentWidget(	APP_WIDGET		parent );

/************************************************************************/
/*									*/
/*  Event handler to set minimum size.					*/
/*									*/
/************************************************************************/

extern APP_EVENT_HANDLER_H( appSetSizeAsMinimum, w, through, event );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif
