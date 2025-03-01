/************************************************************************/
/*									*/
/*  Drawn pulldowns: For color chooser etc.				*/
/*									*/
/************************************************************************/

#   include	"guiWidgets.h"
#   include	<drawDrawingSurface.h>
#   include	<utilColor.h>

#   if ! USE_HEADLESS

struct AppDrawnPulldown;
typedef void (*AppDrawnPulldownPuldown)( void * through );

typedef struct AppDrawnPulldown
    {
    APP_WIDGET			adpInplaceDrawing;
    APP_WIDGET			adpPulldownShell;
    APP_WIDGET			adpPulldownDrawing;
    APP_EVENT_HANDLER_T		adpClickHandler;
    AppDrawnPulldownPuldown	adpPulldown;
    void *			adpThrough;
    int				adpMouseX;
    int				adpMouseY;
    unsigned char		adpEnabled;

    DrawingSurface		adpInplaceDrawingSurface;
    DrawingSurface		adpPulldownDrawingSurface;
    RGB8Color			adpInplaceBackgroundColor;

#   if USE_MOTIF
    Time			adpFirstDownTime;
#   endif
    } AppDrawnPulldown;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void guiMakeDrawnPulldownInColumn(
				AppDrawnPulldown *		adp,
				APP_REDRAW_HANDLER_T		redrawInplace,
				APP_REDRAW_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			column,
				void *				through );

extern void guiMakeDrawnPulldownInRow(
				AppDrawnPulldown *		adp,
				APP_REDRAW_HANDLER_T		redrawInplace,
				APP_REDRAW_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			row,
				int				column,
				int				colspan,
				void *				through );

extern void guiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
					int			sensitive );

extern void guiInitDrawnPulldown(	AppDrawnPulldown *	adp );
extern void guiCleanDrawnPulldown(	AppDrawnPulldown *	adp );

extern void guiSetDrawnPulldownHeight(
				    AppDrawnPulldown *	adp,
				    int			height );

extern void guiSetDrawnPulldownStrips(
				    AppDrawnPulldown *	adp,
				    int			strips );

extern int guiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent );

extern void guiDrawnPulldownDrawArrow(	const struct DocumentRectangle * drClip,
					int *			pRestWide,
					int *			pRestHigh,
					APP_WIDGET		w,
					AppDrawnPulldown *	adp );

extern int guiFinishDrawnPulldownInplace(	AppDrawnPulldown *	adp );
extern int guiFinishDrawnPulldownPulldown(	AppDrawnPulldown *	adp );

extern void guiExposeDrawnPulldownInplace(	AppDrawnPulldown *	adp );

#   endif
