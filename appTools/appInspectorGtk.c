#   include	"appToolsConfig.h"

#   include	<stdio.h>

#   include	"appInspector.h"
#   include	"appInspectorSubject.h"

#   include	<appDebugon.h>

#   if USE_GTK

void appInspectorMakePageParent(	AppInspector *	ai )
    {
    GtkWidget *		notebook;

    notebook= gtk_notebook_new();

    gtk_notebook_set_show_tabs( GTK_NOTEBOOK( notebook ), FALSE );
    gtk_notebook_set_show_border( GTK_NOTEBOOK( notebook ), FALSE );

    gtk_widget_show( notebook );

    gtk_box_pack_start( GTK_BOX( ai->aiPaned ), notebook, FALSE, TRUE, 0 );

    ai->aiPageParent= notebook;

    gtk_widget_set_name(GTK_WIDGET (ai->aiPaned), "tedInspector");

    return;
    }

void appInspectorChoosePage(	AppInspector *		ai,
				int			andMenu,
				int			subject )
    {
    gtk_notebook_set_current_page( GTK_NOTEBOOK( ai->aiPageParent ), subject );

    if  ( andMenu )
	{ guiSetOptionmenu( &(ai->aiSubjectOptionmenu), subject );	}

    return;
    }

int appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
					AppInspector *	ai,
					const char *	label )
    {
#   if GTK_MAJOR_VERSION < 3
    GtkWidget *	page= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    GtkWidget *	page= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    gtk_notebook_append_page( GTK_NOTEBOOK( ai->aiPageParent ),
						    page, (GtkWidget *)0 );

    gtk_widget_show( page );

    guiAddItemToOptionmenu( &(ai->aiSubjectOptionmenu), label );

    *pPage= page; return 0;
    }

void appInspectorDeleteSubject(	AppInspector *		ai,
				int			subject )
    {
    GtkWidget *		page;

    page= gtk_notebook_get_nth_page( GTK_NOTEBOOK( ai->aiPageParent ),
								subject );

    gtk_widget_destroy( page );

    guiDeleteItemFromOptionmenu( &(ai->aiSubjectOptionmenu), subject );

    ai->aiSubjectCount--;
    while( subject < ai->aiSubjectCount )
	{ ai->aiSubjects[subject]= ai->aiSubjects[subject+ 1];	}

    return;
    }


void appInspectorEnablePage(	AppInspector *	ai,
				int		subject,
				int		enabled )
    {
    GtkWidget *		page;

    page= gtk_notebook_get_nth_page( GTK_NOTEBOOK( ai->aiPageParent ),
								subject );

    gtk_widget_set_sensitive( page, enabled != 0 );

    guiOptionMenuEnablePosition( &(ai->aiSubjectOptionmenu),
						    subject, enabled != 0 );

    return;
    }

#   endif /*  USE_GTK	*/
