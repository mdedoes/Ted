/************************************************************************/
/*									*/
/*  Option menus.							*/
/*									*/
/************************************************************************/

#   include	"guiBase.h"

#   ifndef	GUI_OPTMENU_H
#   define	GUI_OPTMENU_H

typedef void (*OptionmenuCallback)(	int		n,
					void *		through );

#   if USE_GTK

typedef struct AppOptionmenu
    {
    APP_WIDGET			aomInplace;
    APP_WIDGET			aomPulldown;

    OptionmenuCallback		aomCallback;
    void *			aomTarget;
    } AppOptionmenu;

#   endif

#   if USE_MOTIF

typedef struct AppOptionmenu
    {
    APP_WIDGET			aomInplace;
    APP_WIDGET			aomPulldown;

    OptionmenuCallback		aomCallback;
    void *			aomTarget;

    int				aomOptionsVisible;
    } AppOptionmenu;

#   endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   if ! USE_HEADLESS

extern void guiMakeOptionmenuInColumn(	AppOptionmenu *		aom,
					APP_WIDGET		parent,
					OptionmenuCallback	callBack,
					void *			through );

extern void guiMakeOptionmenuInRow(	AppOptionmenu *		aom,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					OptionmenuCallback	callBack,
					void *			through );

extern void guiOptionmenuRefreshWidth(	AppOptionmenu *		aom );

extern void guiEmptyOptionmenu(		AppOptionmenu *		aom );

extern void guiEnableOptionmenu(	AppOptionmenu *		aom,
					int			sensitive );

extern void guiAddItemToOptionmenu(	AppOptionmenu *		aom,
					const char *		label );

extern void guiDeleteItemFromOptionmenu( AppOptionmenu *	aom,
					int			pos );

extern void guiReplaceValueInOptionmenu( AppOptionmenu *	aom,
					int			position,
					const char *		value );

extern void guiOptionMenuEnablePosition( AppOptionmenu *	aom,
					int			pos,
					int			enabled );

extern void guiSetOptionmenu(		AppOptionmenu *		aom,
					int			pos );

extern void guiInitOptionmenu(		AppOptionmenu *		aom );

#   endif

#   endif		/* GUI_OPTMENU_H */
