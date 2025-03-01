#   include	"guiBaseConfig.h"

#   if USE_MOTIF

#   include	"guiBase.h"
#   include	"guiWidgets.h"

#   include	<X11/Xatom.h>
#   include	<Xm/ToggleB.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Toggle button (checkbox) related functionality.			*/
/*									*/
/************************************************************************/

APP_WIDGET guiMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			col,
				int			colspan )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	toggle;

    if  ( ! text )
	{ text= "";	}

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    ac= guiMotifSetInRowConstraints( al, ac, col, colspan );

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNmarginTop,		TBrmargT ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    toggle= XmCreateToggleButton( row, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    XmStringFree( labelString );

    XtManageChild( toggle );

    return toggle;
    }

void guiMakeColumnToggle(	Widget *		pToggle,
				Widget			column,
				XtCallbackProc		callback,
				void *			through,
				const char *		labelText,
				int			set )
    {
    Widget		toggle;

    XmString		labelString= (XmString)0;

    Arg			al[20];
    int			ac= 0;

    if  ( labelText )
	{ labelString= XmStringCreateLocalized( (char *)labelText ); }

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNset,			set ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		TBcmargT ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		TBcmargH ); ac++;

    if  ( labelString )
	{ XtSetArg( al[ac], XmNlabelString,	labelString ); ac++; }

    toggle= XmCreateToggleButton( column, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    if  ( labelString )
	{ XmStringFree( labelString );	}

    XtManageChild( toggle );

    *pToggle= toggle;
    }

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void guiSetToggleLabel(		APP_WIDGET		toggle,
				const char *		text )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)text );
    XtVaSetValues( toggle,
			XmNlabelString,	labelString,
			NULL );

    XmStringFree( labelString );
    }

void guiSetToggleState(	APP_WIDGET		toggle,
			int			set )
    {
    XmToggleButtonSetState( toggle, set != 0, False );
    }

int guiGetToggleState(	APP_WIDGET		toggle )
    {
    return XmToggleButtonGetState( toggle ) != 0;
    }

int guiGetToggleStateFromCallbackMotif( void *	voidcbs )
    {
    XmToggleButtonCallbackStruct *	cbs;

    cbs= (XmToggleButtonCallbackStruct *)voidcbs;

    return cbs->set;
    }

#   endif
