/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif.					*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"
#   include	"guiBase.h"

#   include	<stddef.h>

#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   if USE_MOTIF

int guiDrawGetSizeFromConfigureEvent(	int *		pWide,
					int *		pHigh,
					Widget		w,
					XEvent *	event )
    {
    XConfigureEvent *	cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return 1;	}

    *pWide= cevent->width; *pHigh= cevent->height; return 0;
    }
					
int guiGetCoordinatesFromMouseButtonEvent(
					int *			pX,
					int *			pY,
					int *			pButton,
					int *			pUpDown,
					int *			pSeq,
					unsigned int *		pKeyState,
					const APP_WIDGET	w,
					const XEvent *		event )
    {
    const XButtonEvent *	bevent= &(event->xbutton);

    static Widget		prevWidget;
    static Time			prevTime;
    static Time			lastTime;
    static long			multiClickInterval;

    int				seq= 1;

    if  ( multiClickInterval == 0 )
	{
	multiClickInterval= XtGetMultiClickTime( XtDisplay( w ) );
	if  ( multiClickInterval == 0 )
	    { multiClickInterval= 200;	}
	}

    switch( event->type )
	{
	case ButtonPress:
	    *pUpDown= +1;

	    if  ( w == prevWidget )
		{
		if  ( bevent->time != lastTime				&&
		      bevent->time- lastTime < multiClickInterval	)
		    {
		    seq++;
		    if  ( bevent->time- prevTime < 2* multiClickInterval )
			{ seq++; }
		    }

		if  ( bevent->time != lastTime )
		    { prevTime= lastTime; lastTime= bevent->time; }
		}
	    else{
		prevWidget= w;
		lastTime= bevent->time;
		prevTime= lastTime- 4* multiClickInterval;
		*pSeq= 1;
		}

	    *pKeyState= bevent->state;
	    break;

	case ButtonRelease:
	    *pUpDown= -1;
	    *pSeq= 1;
	    *pKeyState= bevent->state;
	    break;

	default:
	    LDEB(event->type); return 1;
	}

    *pSeq= seq;

    switch( bevent->button )
	{
	case Button1:	*pButton= 1; break;
	case Button2:	*pButton= 2; break;
	case Button3:	*pButton= 3; break;
	case Button4:	*pButton= 4; break;
	case Button5:	*pButton= 5; break;

	default:
	    LDEB(bevent->button); return 1;
	}

    *pX= bevent->x; *pY= bevent->y; return 0;
    }

int guiGetCoordinatesFromMouseMoveEvent(	int *		pX,
						int *		pY,
						Widget		w,
						XEvent *	event )
    {
    XMotionEvent *	mevent= &(event->xmotion);

    if  ( event->type != MotionNotify )
	{ return 1;	}

    *pX= mevent->x; *pY= mevent->y; return 0;
    }

void guiDrawSetRedrawHandler(	APP_WIDGET		w,
				APP_EVENT_HANDLER_T	handler,
				void *			through )
    {
    XtAddEventHandler( w, ExposureMask, False, handler, through );
    }

void guiDrawSetConfigureHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, StructureNotifyMask, False, handler, through );
    }

void guiDrawSetButtonPressHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, ButtonPressMask, False, handler, through );
    }

void guiDrawSetScrollHandler(		APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, ButtonPressMask, False, handler, through );
    }

int guiDrawGetInoutFromFocusEvent(	int *			pInOut,
					APP_WIDGET		w,
					APP_EVENT *		event )
    {
    switch( event->type )
	{
	case FocusIn:
	    *pInOut=  1; return 0;

	case FocusOut:
	    *pInOut= -1; return 0;

	default:
	    LDEB(event->type); return 1;
	}
    }

int guiDrawGetSizeOfWidget(		int *		pWide,
					int *		pHigh,
					APP_WIDGET	w )
    {
    Dimension		wide;
    Dimension		high;

    XtVaGetValues( w,	XmNwidth,	&wide,
			XmNheight,	&high,
			NULL );

    *pWide= wide; *pHigh= high; return 0;
    }

#   endif
