/************************************************************************/
/*									*/
/*  Translation of GUI Drawing primitives to the actual implementing	*/
/*  GUI system. As we do not have the pretentence to introduce our own	*/
/*  GUI layer, as much as possible is done through macros. (#defines)	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_BASE_H
#   define	APP_GUI_BASE_H

	    /* yes ALL! for dependencies in makefile */
#   include "guiBaseMotif.h"
#   include "guiBaseGtk.h"

/************************************************************************/
/*									*/
/*   Decide whether we use ugly hand-crafted dialogs or use standard	*/
/*   system ones.							*/
/*									*/
/************************************************************************/

# if USE_MOTIF
#	define USE_OWN_DIALOGS	1
# endif

# if USE_GTK
#   if GTK_MAJOR_VERSION < 2
#	define USE_GTK_DIALOGS	0
#	define USE_OWN_DIALOGS	1
#   else
#	define USE_GTK_DIALOGS	1
#	define USE_OWN_DIALOGS	0
#   endif
# else
#	define USE_GTK_DIALOGS	0
# endif

# if USE_HEADLESS
#	define USE_OWN_DIALOGS	0
# endif

/************************************************************************/
/*									*/
/*   Routine declarations.						*/
/*									*/
/************************************************************************/

# if ! USE_HEADLESS

extern int appKeysymX11ToUnicode(	unsigned int	keysym );

typedef void (*APP_DRAG_TIMER_CALLBACK)(	void *		through,
						int		mouseX,
						int		mouseY );

typedef void (*APP_FORGET_COPY)(	APP_WIDGET		w,
					void *			through,
					APP_EVENT *		event );

extern void guiRunDragLoop(	APP_WIDGET		w,
				APP_CONTEXT		context,
				APP_EVENT *		downEvent,
				APP_EVENT_HANDLER_T	upHandler,
				APP_EVENT_HANDLER_T	moveHandler,
				int			timerInterval,
				APP_DRAG_TIMER_CALLBACK	timerHandler,
				void *			through );

# endif

#   endif	/* APP_GUI_BASE_H */
