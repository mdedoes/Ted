#   include	"guiBaseConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiDrawnPulldown.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   if USE_GTK

/************************************************************************/
/*									*/
/*  Hide the popup of this drawn popdown.				*/
/*									*/
/************************************************************************/

static void guiDrawnPulldownPopdown(	APP_WIDGET	w )
    {
    if  ( gtk_widget_has_grab( w ) )
	{
	gtk_grab_remove( w );

#	if GTK_MAJOR_VERSION < 3
	gdk_pointer_ungrab( GDK_CURRENT_TIME );
#	endif
	}

    gtk_widget_hide( w );

    return;
    }

/************************************************************************/
/*									*/
/*  Display the popup that belongs to this drawn popdown.		*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( guiDrawnPulldownPulldown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;
    GdkEventButton *	bevent= &(mouseEvent->button);

    gint		screenX;
    gint		screenY;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    if  ( bevent->button != 1 )
	{ return;	}

    /******/

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ guiDrawnPulldownPopdown( w ); return;	}

    screenX= bevent->x_root;
    screenY= bevent->y_root;

    /*
    adp->adpFirstDownTime= bevent->time;
    */
    adp->adpMouseX= screenX;
    adp->adpMouseY= screenY;

    /******/

    gdk_window_get_origin( gtk_widget_get_window( adp->adpInplaceDrawing ),
							&screenX, &screenY );

#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_uposition( adp->adpPulldownShell, screenX, screenY );
#   else
    gtk_window_move( GTK_WINDOW( adp->adpPulldownShell ), screenX, screenY );
#   endif

    if  ( adp->adpPulldown )
	{ (*adp->adpPulldown)( adp->adpThrough );	}

    /* code borrowed from gtkcombo.c: */

    gtk_widget_show( adp->adpPulldownShell );
    gtk_widget_grab_focus( adp->adpPulldownShell );
    gtk_grab_add( adp->adpPulldownShell );

#   if GTK_MAJOR_VERSION < 3
    gdk_pointer_grab( gtk_widget_get_window( adp->adpPulldownShell ), TRUE,
				    GDK_BUTTON_PRESS_MASK | 
				    GDK_BUTTON_RELEASE_MASK,
				    NULL, NULL, GDK_CURRENT_TIME );
#   endif

    return;
    }

static APP_EVENT_HANDLER_H( guiDrawnPulldownMouseUpDown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;
    GdkEventButton *	bevent= &(mouseEvent->button);

    int			screenX;
    int			screenY;

    gint		wide;
    gint		high;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    /**/
    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ guiDrawnPulldownPopdown( w ); return;	}

    screenX= bevent->x_root;
    screenY= bevent->y_root;

    /*
    if  ( mouseEvent->xbutton.time == adp->adpFirstDownTime )
	{ return;	}
    */

    if  ( upDown < 0			&&
	  adp->adpMouseX == screenX	&&
	  adp->adpMouseY == screenY	)
	{ return;	}

#   if GTK_MAJOR_VERSION < 2
    wide= w->allocation.width;
    high= w->allocation.height;
#   else
    gtk_window_get_size( GTK_WINDOW( w ), &wide, &high );
#   endif

    /**/
    if  ( mouseX < 0		||
	  mouseY < 0		||
	  mouseX >= wide	||
	  mouseY >= high	)
	{ goto ready;	}

    if  ( adp->adpClickHandler )
	{
	(*adp->adpClickHandler)( w, mouseEvent, adp->adpThrough );
	}

  ready:

    guiDrawnPulldownPopdown( w );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the drawn pulldown. Attach the pulldown to the in place	*/
/*  widget and do administration.					*/
/*									*/
/************************************************************************/

static void guiFinishDrawnPulldown(
				AppDrawnPulldown *		adp,
				APP_REDRAW_HANDLER_T		redrawInplace,
				APP_REDRAW_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				void *				through )
    {
    GtkAllocation	size;

    adp->adpClickHandler= clickedPulldown;
    adp->adpPulldown= pullDown;
    adp->adpThrough= through;

    /******/

    if  ( redrawInplace )
	{
	guiDrawSetRedrawHandler( adp->adpInplaceDrawing, 
						redrawInplace, through );
	}

    /******/

    adp->adpPulldownShell= gtk_window_new( GTK_WINDOW_POPUP );

    adp->adpPulldownDrawing= gtk_drawing_area_new();

    gtk_container_add( GTK_CONTAINER( adp->adpPulldownShell ),
						    adp->adpPulldownDrawing );

    size.x= size.y= 0;
    size.width= size.height= 100;
    gtk_widget_set_allocation( adp->adpPulldownDrawing, &size );

    if  ( redrawPulldown )
	{
	guiDrawSetRedrawHandler( adp->adpPulldownDrawing,
						redrawPulldown, through );
	}

    /******/
    gtk_widget_add_events( adp->adpPulldownShell, 
			    GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_PRESS_MASK );

    g_signal_connect( G_OBJECT( adp->adpPulldownShell ),
				"button_press_event",
				(GCallback)guiDrawnPulldownMouseUpDown,
				(void *)adp );
    g_signal_connect( G_OBJECT( adp->adpPulldownShell ),
				"button_release_event",
				(GCallback)guiDrawnPulldownMouseUpDown,
				(void *)adp );

    gtk_widget_add_events( adp->adpInplaceDrawing, GDK_BUTTON_PRESS_MASK );
    g_signal_connect( G_OBJECT( adp->adpInplaceDrawing ),
				"button_press_event",
				(GCallback)guiDrawnPulldownPulldown,
				(void *)adp );

    /******/

    gtk_widget_show( adp->adpPulldownDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a pulldown where the values are drawings.. Not Text.		*/
/*									*/
/************************************************************************/

void guiMakeDrawnPulldownInColumn(
				AppDrawnPulldown *		adp,
				APP_REDRAW_HANDLER_T		redrawInplace,
				APP_REDRAW_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			column,
				void *				through )
    {
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    adp->adpInplaceDrawing= gtk_drawing_area_new();

    gtk_box_pack_start( GTK_BOX( column ), adp->adpInplaceDrawing,
							    expand, fill, 0 );

    gtk_widget_show( adp->adpInplaceDrawing );

    guiFinishDrawnPulldown( adp, redrawInplace, redrawPulldown,
					clickedPulldown, pullDown, through );

    return;
    }

void guiMakeDrawnPulldownInRow(	AppDrawnPulldown *		adp,
				APP_REDRAW_HANDLER_T		redrawInplace,
				APP_REDRAW_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			row,
				int				column,
				int				colspan,
				void *				through )
    {
    adp->adpInplaceDrawing= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( row, adp->adpInplaceDrawing,
			column, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			0, 0 );
			/*
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );
			*/

    gtk_widget_show( adp->adpInplaceDrawing );

    guiFinishDrawnPulldown( adp, redrawInplace, redrawPulldown,
					clickedPulldown, pullDown, through );

    return;
    }

void guiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
				int			enabled )
    {
    if  ( adp->adpEnabled != enabled )
	{
	adp->adpEnabled= enabled;
	gtk_widget_set_sensitive( adp->adpInplaceDrawing, enabled != 0 );

	guiExposeDrawnPulldownInplace( adp );
	}
    }

void guiSetDrawnPulldownHeight(	AppDrawnPulldown *	adp,
					int			height )
    {
    GtkAllocation	pullSize;
    GtkAllocation	inplaceSize;

    pullSize.x= pullSize.y= 0;

#   if GTK_MAJOR_VERSION < 2
    gtk_widget_get_allocation( adp->adpPulldownShell, &pullSize );
#   else
    gtk_window_get_size( GTK_WINDOW( adp->adpPulldownShell ),
					&pullSize.width, &pullSize.height );
#   endif

    gtk_widget_get_allocation( adp->adpInplaceDrawing, &inplaceSize );

    pullSize.height= height;
    pullSize.width= inplaceSize.width;

    gtk_window_set_default_size( GTK_WINDOW( adp->adpPulldownShell ),
					    pullSize.width, pullSize.height );
#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_usize( adp->adpPulldownShell,
					    pullSize.width, pullSize.height );
#   else
    gtk_window_resize( GTK_WINDOW( adp->adpPulldownShell ),
					    pullSize.width, pullSize.height );
#   endif

    return;
    }

void guiSetDrawnPulldownStrips(	AppDrawnPulldown *	adp,
					int			strips )
    {
    GtkAllocation	inplaceSize;

    gtk_widget_get_allocation( adp->adpInplaceDrawing, &inplaceSize );

    guiSetDrawnPulldownHeight( adp, strips* inplaceSize.height );

    return;
    }

int guiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent )
    {
    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    GtkAllocation	inplaceSize;

    gtk_widget_get_allocation( adp->adpInplaceDrawing, &inplaceSize );

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return -1;	}

    *pStrip= mouseY/ inplaceSize.height;
    return 0;
    }

#   endif
