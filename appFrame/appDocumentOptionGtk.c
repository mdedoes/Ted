/************************************************************************/
/*									*/
/*  Gtk specific connection between the documents and their 'Window'	*/
/*  options.								*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_GTK

#   include	<stddef.h>

#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	<guiMenuItem.h>

#   include	<appDebugon.h>

static const char APP_DOCUMENT[]= "APP_DOCUMENT";

/************************************************************************/
/*									*/
/*  Management of the 'Windows' menu options.				*/
/*									*/
/************************************************************************/

void appSetWindowsItemState(	APP_WIDGET	menu,
				EditDocument *	ed,
				int		changed )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_get_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= g_object_get_data( G_OBJECT( child ), APP_DOCUMENT );

	if  ( voided == (void *)ed )
	    { guiSetToggleItemState( child, changed ); }

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

void appRemoveWindowsOption(	APP_WIDGET		menu,
				EditDocument *		oldEd )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_get_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= g_object_get_data( G_OBJECT( child ), APP_DOCUMENT );

	if  ( voided == (void *)oldEd )
	    { gtk_widget_destroy( child );	}

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

void appRenameWindowsOption(		APP_WIDGET		menu,
					EditDocument *		ed,
					const MemoryBuffer *	title )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_get_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= g_object_get_data( G_OBJECT( child ), APP_DOCUMENT );

	if  ( voided == (void *)ed )
	    {
	    guiSetToggleItemLabel( child,
				utilMemoryBufferGetString( title ) );
	    }

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

void appAppSetWindowsOption(	APP_WIDGET		menu,
				EditDocument *		ed,
				const MemoryBuffer *	title )
    {
    APP_WIDGET		item;
    AppMenuItem		ami;

    if  ( ed->edHasBeenChanged )
	{ ami.amiItemType= ITEMtyTOGGLE_ON;	}
    else{ ami.amiItemType= ITEMtyTOGGLE_OFF;	}

    ami.amiText= utilMemoryBufferGetString( title );
    ami.amiKey= (char *)0;
    ami.amiKeyText= (char *)0;
    ami.amiCallback= appDocToFront;

    item= guiSetToggleMenuItem( menu, &(ed->edToplevel), &ami, (void *)ed );

    g_object_set_data( G_OBJECT( item ), APP_DOCUMENT, item );
    }

#   endif
