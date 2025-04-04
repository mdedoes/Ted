#   include	"guiBaseConfig.h"

#   include	"guiBase.h"

#   if USE_MOTIF

#   include	"guiRunDialog.h"

#   include	"guiWidgets.h"

#   include	<Xm/Form.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/DialogS.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  1)  Force keyboard focus to a modal dialog.				*/
/*									*/
/*  2)  Remember te initial size of a shell as its minimum and its	*/
/*	maximum size. This event handler can be used to prevent the	*/
/*	user from resizing a dialog. E.G. because KDE does not listen	*/
/*	to the window manager hints about this. On the other hand some	*/
/*	window managers (mwm on vms!) cause this event handler to be	*/
/*	called too early. That results in dialogs that are MUCH too	*/
/*	small.								*/
/*									*/
/************************************************************************/

/*  1  */
static void appDialogSetFocus(		Widget			w,
					XtPointer		voidqc,
					XEvent *		event,
					Boolean *		pRefused )
    {
    if  ( event->type == MapNotify )
	{
	guiMotifSetFocusToWindow( w );

	/*
	XtRemoveEventHandler( w, StructureNotifyMask, False,
						appDialogSetFocus, voidqc );
	*/
	}

    *pRefused= 1; return;
    }

#   define FIX_DIALOG_SIZE 0
#   if FIX_DIALOG_SIZE
/*  2  */
static void appFixDialogSize(	Widget			w,
				void *			through,
				XEvent *		event,
				Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNmaxWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			XmNmaxHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appFixDialogSize, through );

    *pRefused= 1;

    return;
    }
#   endif

/************************************************************************/
/*									*/
/*  Make a dialog with a vertical organisation.				*/
/*									*/
/************************************************************************/

void guiMakeVerticalDialog(	AppDialog *		ad,
				Widget *		pPaned,
				APP_WIDGET		topLevel,
				APP_CLOSE_CALLBACK_T	closeCallback,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    Widget		shell;
    Widget		dialog;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    MwmHints		hints;

    hints.flags=	MWM_HINTS_FUNCTIONS	|
			MWM_HINTS_DECORATIONS	;
    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	;
    if  ( closeCallback )
	{ hints.functions |= MWM_FUNC_CLOSE;	}

    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;

    ac= 0;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    if  ( hints.flags & MWM_HINTS_FUNCTIONS )
	{ XtSetArg( al[ac], XmNmwmFunctions,	hints.functions ); ac++; }
    if  ( hints.flags & MWM_HINTS_DECORATIONS )
	{ XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++; }

    shell= XmCreateDialogShell( topLevel, WIDGET_NAME, al, ac );

    if  ( closeCallback )
	{
	guiSetCloseCallback( shell, closeCallback, through );
	}

    if  ( destroyCallback )
	{
	XtAddCallback( shell, XmNdestroyCallback,
						destroyCallback, through );
	}

#   if FIX_DIALOG_SIZE
    See Above
    XtAddEventHandler( shell, StructureNotifyMask, False,
						appFixDialogSize, through );
#   endif

    ac= 0;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    /* Does not work due to motif bug:
    XtSetArg( al[ac], XmNdialogStyle,		XmDIALOG_APPLICATION_MODAL );
									ac++;
    */

    dialog= XmCreateForm( shell, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( dialog, WIDGET_NAME, al, ac );

    XtManageChild( paned );

    XtAddEventHandler( shell, StructureNotifyMask, False,
					    appDialogSetFocus, (void *)0 );

    ad->adTopWidget= shell; ad->adDialog= dialog;
    *pPaned= paned;
    return;
    }

void guiRunDialog(			AppDialog *		ad,
					int			initial,
					APP_CONTEXT		appContext )
    {
#   if 0
    APP_WIDGET		defaultButton= (APP_WIDGET)0;

    XtVaGetValues( qc->qcDialog.adDialog,
			XmNdefaultButton,	&defaultButton,
			NULL );

    if  ( defaultButton )
	{ guiFocusToWidget( defaultButton ); }
#   endif

    XtAddGrab( ad->adTopWidget, True, False );

    ad->adResponse= initial;
    ad->adContinue= 1;

    while( ad->adContinue			&&
	   XtIsRealized( ad->adTopWidget )	)
	{
	XtAppProcessEvent( appContext, XtIMAll );
	}

    XtRemoveGrab( ad->adTopWidget );

    return;
    }

void guiBreakDialog(			AppDialog *		ad,
					int			response )
    {
    ad->adResponse= response;
    ad->adContinue= 0;
    }

void guiSetDefaultButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    XtVaSetValues( ad->adDialog,
			XmNdefaultButton,	button,
			NULL );
    }

void guiSetCancelButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    XtVaSetValues( ad->adDialog,
			XmNcancelButton,	button,
			NULL );
    }

static void appDialogRelative(	Widget		relative,
				Widget		dialog )
    {
    Dimension		x;
    Dimension		y;
    Dimension		width;
    Dimension		height;

    XtVaGetValues( relative,
			    XmNx,	&x,
			    XmNy,	&y,
			    XmNwidth,	&width,
			    XmNheight,	&height,
			    NULL );

    XtVaSetValues( dialog,
			    XmNx,		x+ width/5,
			    XmNy,		y+ height/ 5,
			    NULL );

    XtVaSetValues( XtParent( dialog ),
			    XmNtransientFor,	relative,
			    XmNx,		x+ width/5,
			    XmNy,		y+ height/ 5,
			    NULL );

    }

static void appRelativeMapCallback(	Widget		w,
					XtPointer	voidRelative,
					XtPointer	voidcbs )
    {
    Widget		relative= (Widget)voidRelative;

    appDialogRelative( relative, w );

    XtRemoveCallback( w, XmNmapCallback, appRelativeMapCallback, voidRelative );
    }

/************************************************************************/
/*									*/
/*  Show a shell widget. Because of the RIDICULOUS and ABSURD situation	*/
/*  the you cannot rely on a shell window to be visible after we tell	*/
/*  it to become visible, we handle a series of events in a loop until	*/
/*  it becomes visible.							*/
/*									*/
/************************************************************************/

void guiShowDialog(			APP_CONTEXT		appContext,
					AppDialog *		ad,
					Widget			relative )
    {
    if  ( relative )
	{
	XtAddCallback( ad->adDialog, XmNmapCallback,
				appRelativeMapCallback, (void *)relative );
	}

    XtManageChild( ad->adDialog );
    XSync( XtDisplay( ad->adDialog ), False );

    if  ( relative )
	{ appDialogRelative( relative, ad->adDialog );	}

    guiMotifWaitForWindow( XtParent( ad->adDialog ), appContext );
    }

void guiHideDialog(			AppDialog *		ad )
    {
    XtUnmanageChild( ad->adDialog );

    XtVaSetValues( ad->adTopWidget,
			    XmNtransientFor,	(Widget)0,
			    NULL );
    }

#   endif
