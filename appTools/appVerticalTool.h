/************************************************************************/
/*									*/
/*  Make a vertical tool shell.						*/
/*									*/
/************************************************************************/

#   ifndef	APP_VERTICAL_TOOL_H
#   define	APP_VERTICAL_TOOL_H

#   include	<guiBase.h>

struct EditApplication;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMakeVerticalTool( APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				struct EditApplication *	ea,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through );

#   endif
