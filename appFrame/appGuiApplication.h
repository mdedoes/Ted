/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_APPLICATION_H
#   define	APP_GUI_APPLICATION_H

#   include	<guiBase.h>

#   if ! USE_HEADLESS

struct EditApplication;
struct AppMenu;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct EditDocument * appOpenDocumentFile(
				struct EditApplication *	ea,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				int				suggestStdin,
				int				formatHint,
				const struct MemoryBuffer *	filename );

extern struct EditDocument * appOpenDocument(
				struct EditApplication *	ea,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				const struct MemoryBuffer *	filename );

extern APP_MENU_CALLBACK_H( appDocToFront, option, voided, e );

void appQuitApplication(	APP_WIDGET			option,
				APP_WIDGET			relative,
				struct EditApplication *	ea );

extern int appRunReallyQuitDialog(
				APP_WIDGET			option,
				APP_WIDGET			relative,
				struct EditApplication *	ea );

extern void appShowDefaultsEditor( APP_WIDGET			prefOption,
				void *				voidea );

extern void appReportSaveFailure(
				struct EditApplication *	ea,
				APP_WIDGET			option,
				APP_WIDGET			relative,
				const struct MemoryBuffer *	filename );

extern void appShowShellWidget(	struct EditApplication *	ea,
				APP_WIDGET			shell );

extern APP_WIDGET appMakeMenu(	APP_WIDGET *			pButton,
				AppToplevel *			at,
				struct EditApplication *	ea,
				APP_WIDGET			parent,
				struct AppMenu *		am,
				void *				through );

extern void appSetNamedWindowIcon(
				struct EditApplication *	ea,
				APP_WIDGET			win,
				const char *			iconName );

extern int appFinishApplicationWindow(	struct EditApplication *	ea );

# endif

#   endif
