#   include	"appToolsConfig.h"

#   if USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>

#   include	<appEditApplication.h>
#   include	<guiBase.h>
#   include	<guiWidgets.h>
#   include	<guiWidgetsImpl.h>
#   include	"appVerticalTool.h"
#   include	<appGuiApplication.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				EditApplication *	ea,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    Widget		shell;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    MwmHints		hints;

    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;

    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	|
			MWM_FUNC_CLOSE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;

    if  ( userResizable )
	{
	hints.functions |=	MWM_FUNC_RESIZE;
	hints.decorations |=	MWM_DECOR_RESIZEH;
	}

    ac=0;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;

    /*
    if  ( iconPixmap )
	{ XtSetArg( al[ac], XmNiconPixmap,	iconPixmap ); ac++; }
    */

#   ifdef USE_X11_R5
    shell= XtAppCreateShell( ea->eaApplicationName, WIDGET_NAME,
			    applicationShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   else
    shell= XtAppCreateShell( ea->eaApplicationName, WIDGET_NAME,
			    topLevelShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   endif

    XtAddEventHandler( shell, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    guiSetDialogTitle( shell, option, ea->eaApplicationName );

    guiSetCloseCallback( shell, closeCallback, through );

    ac= 0;
    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( shell, WIDGET_NAME, al, ac );

    XtManageChild( paned );

    appShowShellWidget( ea, shell );
    appSetNamedWindowIcon( ea, shell, ea->eaMainIcon );

    *pShell= shell, *pPaned= paned; return;
    }

#   endif
