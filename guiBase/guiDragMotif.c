#   include	"guiBaseConfig.h"

#   if USE_MOTIF

#   include	"guiBase.h"

#   include	<X11/Xatom.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    XtAppContext		dlContext;

    XtEventHandler		dlUpHandler;
    XtEventHandler		dlMoveHandler;
    int				dlTimerInterval;
    APP_DRAG_TIMER_CALLBACK	dlTimerHandler;
    void *			dlThrough;

    int				dlHalted;
    XtIntervalId		dlIntervalId;
    Widget			dlWidget;
    } DragLoop;

static void guiDragTick(	void *		vdl,
				XtIntervalId *	xii )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlTimerHandler )
	{
	Window		root;
	Window		child;
	int		rootX;
	int		rootY;
	int		winX;
	int		winY;
	unsigned int	mask;

	XQueryPointer( XtDisplay( dl->dlWidget ), XtWindow( dl->dlWidget ),
				    &root, &child,
				    &rootX, &rootY, &winX, &winY, &mask );

	(*dl->dlTimerHandler)( dl->dlThrough, winX, winY );
	}

    if  ( dl->dlTimerHandler && dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= XtAppAddTimeOut( dl->dlContext,
			    dl->dlTimerInterval, guiDragTick, (void *)dl );
	}

    return;
    }

static void guiDragMouseUp(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, dl->dlThrough, event, pRefused );	}

    dl->dlHalted= 1;
    }

/************************************************************************/
/*									*/
/*  Handle a movement event, but as an optimization ignore it if the	*/
/*  next even is a similar one: Apparently we are running after the	*/
/*  mouse.								*/
/*									*/
/************************************************************************/

static void guiDragMouseMove(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;
    Display *		display= XtDisplay( w );

    if  ( QLength( display ) > 0 )
	{
	XEvent			nextEvent;

	XPeekEvent( display, &nextEvent );

	if  ( nextEvent.type == event->type			&&
	      nextEvent.xmotion.window == event->xmotion.window	)
	    { return;	}
	}

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, dl->dlThrough, event, pRefused );	}
    }

void guiRunDragLoop(	APP_WIDGET		w,
			APP_CONTEXT		context,
			APP_EVENT *		downEvent,
			APP_EVENT_HANDLER_T	upHandler,
			APP_EVENT_HANDLER_T	moveHandler,
			int			timerInterval,
			APP_DRAG_TIMER_CALLBACK	timerHandler,
			void *			through )
    {
    DragLoop		dl;

    dl.dlContext= context;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;
    dl.dlWidget= w;

    dl.dlHalted= 0;
    dl.dlIntervalId= (XtIntervalId)0;

    if  ( dl.dlTimerHandler && dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= XtAppAddTimeOut( dl.dlContext,
				dl.dlTimerInterval, guiDragTick, (void *)&dl );
	}

    XtInsertEventHandler( w, ButtonReleaseMask, False,
				    guiDragMouseUp, (void *)&dl, XtListHead );
    XtInsertEventHandler( w, Button1MotionMask, False,
				    guiDragMouseMove, (void *)&dl, XtListHead );

    while( ! dl.dlHalted )
	{ XtAppProcessEvent( dl.dlContext, XtIMAll );	}

    if  ( dl.dlIntervalId )
	{ XtRemoveTimeOut( dl.dlIntervalId );	}

    XtRemoveEventHandler( w, ButtonReleaseMask, False,
					    guiDragMouseUp, (void *)&dl );
    XtRemoveEventHandler( w, Button1MotionMask, False,
					    guiDragMouseMove, (void *)&dl );
    return;
    }

#   endif
