#   include	"guiBaseConfig.h"

#   include	<appDebugon.h>

#   if USE_GTK

#   include	<stdio.h>

#   include	<gdk/gdkkeysyms.h>
#   include	"guiKeysGtk.h"
#   include	"guiWidgets.h"
#   include	"guiRunDialog.h"

/************************************************************************/
/*									*/
/*  Make a dialog with a vertical organisation.				*/
/*									*/
/************************************************************************/

void guiMakeVerticalDialog(	AppDialog *		ad,
				APP_WIDGET *		pColumn,
				APP_WIDGET		topLevel,
				APP_CLOSE_CALLBACK_T	closeCallback,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    APP_WIDGET		shell;
    APP_WIDGET		column;

#   if  GTK_MAJOR_VERSION < 2
    const gboolean	homogeneous= FALSE;
    const gint		spacing= COLUMN_SPACING_GTK;
#   endif

#   if GTK_MAJOR_VERSION < 2
    shell= gtk_window_new( GTK_WINDOW_DIALOG );
#   else
    shell= gtk_dialog_new();
#   endif

    gtk_window_set_position( GTK_WINDOW( shell ), GTK_WIN_POS_CENTER );

    if  ( closeCallback )
	{
	g_signal_connect( G_OBJECT( shell ),
					    "delete_event",
					    (GCallback)closeCallback,
					    through );
	}

    if  ( destroyCallback )
	{
	g_signal_connect( G_OBJECT( shell ),
					    "destroy_event",
					    (GCallback)destroyCallback,
					    through );
	}

    gtk_container_set_border_width( GTK_CONTAINER( shell ), WINDOW_BORDER_GTK );

#   if  GTK_MAJOR_VERSION < 2
    column= gtk_vbox_new( homogeneous, spacing );

    gtk_container_add( GTK_CONTAINER( shell ), column );

    gtk_widget_show( column );
#   else
    column= gtk_bin_get_child( GTK_BIN( shell ) );
#   endif

    ad->adAccelGroup= gtk_accel_group_new();
#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( ad->adAccelGroup, G_OBJECT( shell ) );
#   else
    gtk_window_add_accel_group( GTK_WINDOW( shell ), ad->adAccelGroup );
#   endif

    ad->adTopWidget= shell; *pColumn= column;
    return;
    }

static void appGuiGtkDialogBreak(	GtkWidget *		w,
					GdkEvent *		event,
					void *			voidad )
    {
    AppDialog *		ad= (AppDialog *)voidad;

    ad->adCloseId= 0;

    gtk_main_quit();

    return;
    }

void guiRunDialog(			AppDialog *		ad,
					int			initial,
					APP_CONTEXT		appContext )
    {
    gtk_window_set_modal( GTK_WINDOW( ad->adTopWidget ), TRUE );

    ad->adResponse= initial;

    ad->adCloseId= g_signal_connect( G_OBJECT( ad->adTopWidget ),
					"delete_event",
					(GCallback)appGuiGtkDialogBreak,
					(void *)ad );

    gtk_main();

    if  ( ad->adCloseId )
	{
	g_signal_handler_disconnect( G_OBJECT( ad->adTopWidget ),
							ad->adCloseId );

	if  ( ad->adResponse == AQDrespCLOSED )
	    { guiHideDialog( ad );			}

	ad->adCloseId= 0;
	}

    if  ( ad->adResponse != AQDrespCLOSED )
	{ gtk_window_set_modal( GTK_WINDOW( ad->adTopWidget ), FALSE );	}

    return;
    }

void guiBreakDialog(			AppDialog *		ad,
					int			response )
    {
    ad->adResponse= response;
    gtk_main_quit();
    }

void guiSetDefaultButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    unsigned int	keyMask= 0;
    unsigned int	keyCode= KEY_Return;

    gtk_widget_add_accelerator( button, "clicked", ad->adAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
    }

void guiSetCancelButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    unsigned int	keyMask= 0;
    unsigned int	keyCode= KEY_Escape;

    gtk_widget_add_accelerator( button, "clicked", ad->adAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
    }

void guiShowDialog(			APP_CONTEXT		appContext,
					AppDialog *		ad,
					APP_WIDGET		relative )
    {
    gint		x;
    gint		y;
    gint		wide;
    gint		high;

    if  ( relative )
	{
	GdkWindow *	relWin= gtk_widget_get_window( relative );

	gtk_window_set_transient_for(
		    GTK_WINDOW( ad->adTopWidget ), GTK_WINDOW( relative ) );

	gtk_window_set_position( GTK_WINDOW( ad->adTopWidget ),
							GTK_WIN_POS_NONE );

	gtk_widget_realize( ad->adTopWidget );

	gdk_window_get_position( relWin, &x, &y );

#	if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 22

	gdk_window_get_size( relative->window, &wide, &high );

#	else

	wide= gdk_window_get_width( relWin );
	high= gdk_window_get_height( relWin );

#	endif

	gdk_window_move( gtk_widget_get_window( ad->adTopWidget ),
						    x+ wide/5, y+ high/ 5 );
	}
    else{
	gtk_window_set_position( GTK_WINDOW( ad->adTopWidget ),
							GTK_WIN_POS_CENTER );
	}

    gtk_widget_show( ad->adTopWidget );
    }

void guiHideDialog(			AppDialog *		ad )
    {
    gtk_widget_hide( ad->adTopWidget );

    gtk_window_set_transient_for(
			    GTK_WINDOW( ad->adTopWidget ), (GtkWindow *)0 );
    }

#   endif
