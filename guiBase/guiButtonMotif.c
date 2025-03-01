#   include	"guiBaseConfig.h"

#   if USE_MOTIF

#   include	"guiWidgets.h"

#   include	<Xm/PushB.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a in a form containing a row of buttons.			*/
/*									*/
/************************************************************************/

void guiMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	button;

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    ac= guiMotifSetInRowConstraints( al, ac, position, colspan );

    if  ( showAsDefault )
	{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 1 ); ac++;
	XtSetArg( al[ac], XmNshowAsDefault, showAsDefault != 0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
	}

    button= XmCreatePushButton( row, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    if  ( showAsDefault )
	{
	XtVaSetValues( row,
			XmNdefaultButton,	button,
			NULL );
	}

    *pButton= button; return;
    }

void guiMakeImageButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				struct NamedImage *	ni,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan )
    {
    Drawable	pixmap;

    Arg		al[20];
    int		ac= 0;

    Widget	button;

    if  ( guiMotifGetImagePixmap( &pixmap, row, ni ) )
	{ LDEB(1); return;	}

    ac= 0;
    ac= guiMotifSetInRowConstraints( al, ac, position, colspan );

    XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
    XtSetArg( al[ac], XmNlabelType, XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNlabelPixmap, pixmap ); ac++;

    button= XmCreatePushButton( row, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    *pButton= button; return;
    }

void guiMakeButtonInColumn(	APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		buttonText,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault )
    {
    Widget		button;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)buttonText );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    if  ( showAsDefault )
	{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 1 ); ac++;
	XtSetArg( al[ac], XmNshowAsDefault, showAsDefault != 0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
	}

    button= XmCreatePushButton( column, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    if  ( showAsDefault )
	{
	XtVaSetValues( column,
			XmNdefaultButton,	button,
			NULL );
	}

    guiMotifTurnOfSashTraversal( column );

    *pButton= button; return;
    }

void guiMakeImageButtonInColumn( APP_WIDGET *		pButton,
				APP_WIDGET		column,
				struct NamedImage *	ni,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through )
    {
    Drawable		pixmap;
    Widget		button;

    Arg			al[20];
    int			ac= 0;

    if  ( guiMotifGetImagePixmap( &pixmap, column, ni ) )
	{ LDEB(1); return;	}

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
    XtSetArg( al[ac], XmNlabelType, XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNlabelPixmap, pixmap ); ac++;

    button= XmCreatePushButton( column, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    guiMotifTurnOfSashTraversal( column );

    *pButton= button; return;
    }

void guiChangeButtonText(	APP_WIDGET	button,
				const char *	label )
    {
    guiChangeLabelText( button, label );
    }

#   endif
