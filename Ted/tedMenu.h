#   ifndef	TED_MENU_H
#   define	TED_MENU_H

#   include	<guiMenuItem.h>

struct EditApplication;
struct EditDocument;

extern AppMenu	TED_AppFileMenu;
extern AppMenu	TED_AppWinMenu;
extern AppMenu	TED_DocFileMenu;
extern AppMenu	TED_DocEditMenu;
extern AppMenu	TED_DocWinMenu;
extern AppMenu	TED_DocHelpMenu;
extern AppMenu	TED_AppHelpMenu;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

void tedMakePrivateDocumentMenus(
				struct EditApplication *	ea,
				struct EditDocument *		ed,
				APP_WIDGET			menubar );

#   endif	/*  TED_MENU_H	*/
