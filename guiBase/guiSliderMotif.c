#   include	"guiBaseConfig.h"

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   if USE_MOTIF

#   include	<Xm/Scale.h>

void guiSetSlider(		APP_WIDGET	slider,
				int		minVal,
				int		maxVal,
				int		curVal )
    {
    XtVaSetValues( slider,
	    XmNminimum,		minVal,
	    XmNmaximum,		maxVal,
	    XmNvalue,		curVal,
	    NULL );
    }

void guiSetSliderValue(		APP_WIDGET	slider,
					int		curVal )
    {
    XtVaSetValues( slider,
	    XmNvalue,		curVal,
	    NULL );
    }

int guiGetValueFromSliderCallback(	int *		pValue,
					APP_WIDGET	w,
					void *		e )
    {
    XmScaleCallbackStruct *	scs= (XmScaleCallbackStruct *)e;

    *pValue= scs->value; return 0;
    }

void guiMakeSliderInRow(	APP_WIDGET *		pSlider,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through )
    {
    Arg		al[20];
    int		ac= 0;

    APP_WIDGET	scale;

    ac= 0;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan );
									ac++;

    XtSetArg( al[ac], XmNorientation,		XmHORIZONTAL ); ac++;

    scale= XmCreateScale( row, WIDGET_NAME, al, ac );

    XtManageChild( scale );

    *pSlider= scale; return;
    }

#   endif
