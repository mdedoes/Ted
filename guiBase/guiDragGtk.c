#   include	"guiBaseConfig.h"

#   include	<stdio.h>

#   include	"guiBase.h"

#   include	<appDebugon.h>

#   if USE_GTK

/************************************************************************/
/*									*/
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    APP_EVENT_HANDLER_T		dlUpHandler;
    APP_EVENT_HANDLER_T		dlMoveHandler;
    int				dlTimerInterval;
    APP_DRAG_TIMER_CALLBACK	dlTimerHandler;
    void *			dlThrough;

    guint			dlIntervalId;

    guint			dlUpId;
    guint			dlDownId;
    guint			dlMoveId;

    GtkWidget *			dlWidget;
    GdkDevice *			dlDevice;
    } DragLoop;

static void guiDragMouseUp(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlIntervalId )
	{
	g_source_remove( dl->dlIntervalId );
	dl->dlIntervalId= 0;
	}

    if  ( event && dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, event, dl->dlThrough );	}

    gtk_main_quit();
    }

static void guiDragMouseMove(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( ! ( event->motion.state & GDK_BUTTON1_MASK ) )
	{
	/* really happens: XXDEB(event->motion.state,GDK_BUTTON1_MASK); */
	guiDragMouseUp( w, event, vdl );
	return;
	}

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, event, dl->dlThrough );	}
    }

static int guiDragTick(	void *		vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    gint		winX;
    gint		winY;
    GdkModifierType	mask= 0;

#   if GTK_MAJOR_VERSION < 3
    gdk_window_get_pointer( gtk_widget_get_window( dl->dlWidget ),
							&winX, &winY, &mask );
#   else
    gdk_window_get_device_position( gtk_widget_get_window( dl->dlWidget ),
							dl->dlDevice,
							&winX, &winY, &mask );
#   endif

    if  ( ! ( mask & GDK_BUTTON1_MASK ) )
	{
	/* really happens: XXDEB(mask,GDK_BUTTON1_MASK); */
	guiDragMouseUp( dl->dlWidget, (APP_EVENT *)0, vdl );
	return 0;
	}

    if  ( dl->dlTimerHandler )
	{ (*dl->dlTimerHandler)( dl->dlThrough, winX, winY );	}

    if  ( dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= g_timeout_add( dl->dlTimerInterval,
						    guiDragTick, (void *)dl );
	}

    return 0;
    }

void guiRunDragLoop(	APP_WIDGET		w,
			APP_CONTEXT		context,
			GdkEvent *		downEvent,
			APP_EVENT_HANDLER_T	upHandler,
			APP_EVENT_HANDLER_T	moveHandler,
			int			timerInterval,
			APP_DRAG_TIMER_CALLBACK	timerHandler,
			void *			through )
    {
    DragLoop		dl;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;

    dl.dlIntervalId= 0;
    dl.dlWidget= w;
    /*
    dl.dlDevice= gdk_event_get_device( downEvent );
    */
    dl.dlDevice= downEvent->button.device;

    if  ( dl.dlTimerInterval == 0 )
	{ dl.dlTimerInterval= 200;	}

    gtk_widget_add_events( w,
			GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK );

    dl.dlUpId= g_signal_connect( G_OBJECT( w ),
					    "button_release_event",
					    (GCallback)guiDragMouseUp,
					    (void *)&dl );
    dl.dlDownId= g_signal_connect( G_OBJECT( w ),
					    "button_press_event",
					    (GCallback)guiDragMouseUp,
					    (void *)&dl );
    dl.dlMoveId= g_signal_connect( G_OBJECT( w ),
					    "motion_notify_event",
					    (GCallback)guiDragMouseMove,
					    (void *)&dl );

    if  ( dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= g_timeout_add( dl.dlTimerInterval,
						guiDragTick, (void *)&dl );
	}

    /* Almost hangs X
    gdk_pointer_grab( w->window, FALSE, GDK_BUTTON1_MOTION_MASK,
					    (GdkWindow *)0,
					    (GdkCursor *)0,
					    GDK_CURRENT_TIME );
    */

    gtk_main();

    /*
    gdk_pointer_ungrab( GDK_CURRENT_TIME );
    */

    if  ( dl.dlIntervalId )
	{ g_source_remove( dl.dlIntervalId );	}

    g_signal_handler_disconnect( G_OBJECT( w ), dl.dlUpId );
    g_signal_handler_disconnect( G_OBJECT( w ), dl.dlDownId );
    g_signal_handler_disconnect( G_OBJECT( w ), dl.dlMoveId );

    return;
    }

#   endif
