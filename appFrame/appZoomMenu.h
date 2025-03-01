/************************************************************************/
/*									*/
/*  Infrastructure to make a 'Zoom' document menu.			*/
/*									*/
/************************************************************************/

#   ifndef		APP_ZOOM_MENU_H
#   define		APP_ZOOM_MENU_H

#   include	<guiBase.h>

#   if ! USE_HEADLESS

struct EditDocument;
struct AppMenu;
struct DocumentRectangle;

#   define	ZOOM_COUNT	7

extern void appZoomChangeFactor( APP_WIDGET		option,
				APP_WIDGET		menu,
				APP_WIDGET *		options,
				int			optionCount,
				int *			pLogSqrt2Magnification,
				double *		pMagnification,
				struct DocumentRectangle *	drSelected );

extern void appUpdateZoomMenu(	APP_WIDGET	menu,
				APP_WIDGET *	options,
				int		optionCount,
				int		logs2mag );

void appMakeZoomMenu(		struct EditDocument *	ed,
				APP_WIDGET *		pZoomMenu,
				APP_WIDGET *		options,
				APP_WIDGET		menubar,
				int			logSqrt2Magnification,
				struct AppMenu *	am );

#   endif

#   endif	/*	APP_ZOOM_MENU_H	*/
