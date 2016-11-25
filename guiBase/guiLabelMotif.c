#   include	"guiBaseConfig.h"

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   if USE_MOTIF

#   include	<Xm/Label.h>

/************************************************************************/
/*									*/
/*  Make a row with a label and a text widget.				*/
/*									*/
/************************************************************************/

void guiMakeLabelInRow(		APP_WIDGET *		pLabel,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				const char *		labelText )
    {
    Widget		label;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)labelText );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    label= XmCreateLabel( row, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    XtManageChild( label );

    *pLabel= label;
    }

void guiMakeLabelInHBox(	APP_WIDGET *		pLabel,
				APP_WIDGET		hbox,
				const char *		labelText )
    {
    Widget		label;

    XmString		labelString;

    WidgetList		children;
    Cardinal		childCount= 0;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)labelText );

    XtVaGetValues( hbox,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    if  ( childCount == 0 )
	{
	XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
	XtSetArg( al[ac], XmNleftOffset,	0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
	XtSetArg( al[ac], XmNleftOffset,	0 ); ac++;
	XtSetArg( al[ac], XmNleftWidget,	children[childCount- 1] ); ac++;
	}

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		6 ); ac++;

    label= XmCreateLabel( hbox, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    XtManageChild( label );

    *pLabel= label;
    }


void guiMakeLabelInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				const char *		labelText )
    {
    Widget		label;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)labelText );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    label= XmCreateLabel( column, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    XtManageChild( label );

    guiMotifTurnOfSashTraversal( column );

    *pLabel= label; return;
    }

void guiChangeLabelText(	APP_WIDGET	labelWidget,
				const char *	label )
    {
    XmString			labelString;

    labelString= XmStringCreateLocalized( (char *)label );

    XtVaSetValues( labelWidget,
			XmNlabelString,		labelString,
			NULL );

    XmStringFree( labelString );
    
    return;
    }

#   endif
