/************************************************************************/
/*									*/
/*  Simple io streams using X11 window properties.			*/
/*									*/
/************************************************************************/

#   include	<guiBase.h>

#   if ! USE_HEADLESS

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInOpenPaste(
					APP_WIDGET		w,
					APP_SELECTION_EVENT *	event );

extern struct SimpleOutputStream * sioOutOpenCopy(
					APP_WIDGET		w,
					APP_SELECTION_EVENT *	event );

# endif

