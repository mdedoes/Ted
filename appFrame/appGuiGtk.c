#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appAppFront.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

#   if USE_GTK

/************************************************************************/
/*									*/
/*  Make the initial connection to the gui environment.			*/
/*									*/
/************************************************************************/

int appGuiInitApplication(	EditApplication *	ea,
				int *			pArgc,
				char ***		pArgv )
    {
    int			argc= *pArgc;
    char **		argv= *pArgv;

    GtkWidget *		gWindow;
    GtkAccelGroup *	gAccelGroup;

#   if GTK_MAJOR_VERSION < 3
    const char *	path= CONFDIR "/ted.gtkrc-2.0";

    if  ( ea->eaStyleToolInt >= 0 )
	{ gtk_rc_add_default_file( path );	}
#   else
    {
# if 0
    GError *		error= (GError *)0;
    GtkStyleContext *	styleContext= (GtkStyleContext *)0;
    GtkCssProvider *	styleProvider= gtk_css_provider_new();

    gtk_css_provider_load_from_path( styleProvider, path, &error );
    if  ( error )
	{
XDEB(error);
	}

    gtk_style_context_add_provider( styleContext,
					GTK_STYLE_PROVIDER( styleProvider ),
					GTK_STYLE_PROVIDER_PRIORITY_USER- 1 );
# endif
    }
#   endif


    gtk_init( &argc, &argv );

    gWindow= gtk_window_new( GTK_WINDOW_TOPLEVEL );
    ea->eaToplevel.atTopWidget= gWindow;

    /* user can't minimize or maximize */
    /*
    gtk_window_set_policy( GTK_WINDOW( gWindow), FALSE, FALSE, FALSE);
    */
    gtk_window_set_resizable( GTK_WINDOW( gWindow ), FALSE );
    gtk_window_set_position( GTK_WINDOW( gWindow), GTK_WIN_POS_CENTER);

    guiSetCloseCallback( gWindow, appAppWmClose, (void*)ea );

    /* 
     * Facilities for making menu option accelerators.
     */
    gAccelGroup= gtk_accel_group_new();
    ea->eaToplevel.atAccelGroup= gAccelGroup;

#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( gAccelGroup, G_OBJECT( gWindow));
#   else
    gtk_window_add_accel_group( GTK_WINDOW( gWindow ), gAccelGroup );
#   endif

    /* 
     * Do this here so that we don't get an error when
     * we create the pixmap using the window.
     */
    gtk_widget_realize( gWindow );

    ea->eaScreenPixelsWide= gdk_screen_width();
    ea->eaScreenPixelsHigh= gdk_screen_height();

    *pArgc= argc;
    *pArgv= argv;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetPixelsPerTwip( EditApplication *	ea )
    {
#   if 0
    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= (double)gdk_screen_width()/ (double)gdk_screen_width_mm();
    verPixPerMM= (double)gdk_screen_height()/ (double)gdk_screen_height_mm();

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    ea->eaPixelsPerTwip= xfac;
#   else
    ea->eaPixelsPerTwip= 96.0/ ( 20* 72 );
#   endif

    return;
    }

void appShowShellWidget(		EditApplication *	ea,
					APP_WIDGET		shell )
    {
    gtk_widget_show( shell );
    gtk_window_present( GTK_WINDOW( shell ) );
    /*
    gdk_window_raise( shell->window );
    */
    }

#   endif
