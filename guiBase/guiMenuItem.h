/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_MENU_ITEM_H
#   define	APP_MENU_ITEM_H

#   include	"guiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

#   if ! USE_HEADLESS

/************************************************************************/
/*									*/
/*  A menu, menu item in an application.				*/
/*									*/
/************************************************************************/

typedef enum MenuItemType
    {
    ITEMtyOPTION= 0,
    ITEMtyTOGGLE_OFF,
    ITEMtyTOGGLE_ON,
    ITEMtySEPARATOR
    } MenuItemType;

typedef struct AppMenuItem
    {
    const char *	amiTextResName;
    const char *	amiText;

    const char *	amiKeyResName;
    const char *	amiKey;

    const char *	amiKeyTextResName;
    const char *	amiKeyText;

    MenuItemType	amiItemType;
    APP_MENU_CALLBACK_T	amiCallback;
    APP_WIDGET		amiOptionWidget;
    } AppMenuItem;

typedef struct AppMenu
    {
    const char *		amText;
    const char *		amTextResName;

    const char *		amMnemonic;
    const char *		amMnemonicResName;

    unsigned char		amIsHelp;

    struct AppMenuItem *	amItems;
    int				amItemCount;

    int				amGotResources;
    } AppMenu;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern APP_WIDGET guiMakeMenuInParent(	APP_WIDGET *		pButton,
					AppToplevel *		at,
					APP_WIDGET		menuBar,
					const AppMenu *		am );

extern APP_WIDGET guiSetMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					struct AppMenuItem *	ami,
					void *			target );

extern APP_WIDGET guiSetMenuSeparator(	APP_WIDGET		menu,
					AppToplevel *		at,
					struct AppMenuItem *	ami,
					void *			target );

extern APP_WIDGET guiSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					struct AppMenuItem *	ami,
					void *			target );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif
