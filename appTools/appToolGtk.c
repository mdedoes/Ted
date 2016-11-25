#   include	"appToolsConfig.h"

#   if USE_GTK

#   include	<appEditApplication.h>
#   include	<appGuiApplication.h>
#   include	"appVerticalTool.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pColumn,
				EditApplication *	ea,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    GtkWidget *		shell;
    GtkWidget *		column;

    shell= gtk_window_new( GTK_WINDOW_TOPLEVEL );

    guiSetCloseCallback( shell, closeCallback, through );

    guiSetDialogTitle( shell, option, ea->eaApplicationName );

#   if GTK_MAJOR_VERSION < 3
    column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    column= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    gtk_container_set_border_width( GTK_CONTAINER( shell ), WINDOW_BORDER_GTK );
    gtk_container_add( GTK_CONTAINER( shell ), column );

    gtk_widget_show( column );
    gtk_widget_realize( shell );

    appSetNamedWindowIcon( ea, shell, ea->eaMainIcon );

    *pShell= shell;
    *pColumn= column;
    return;
    }

#   endif
