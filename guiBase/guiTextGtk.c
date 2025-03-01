#   include	"guiBaseConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"

#   include	<appDebugon.h>

#   if USE_GTK

void guiMakeTextInRow(		APP_WIDGET *		pText,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		text= gtk_entry_new();

    gtk_widget_set_name(GTK_WIDGET (text), "tedRowText");

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    guiGtkAttachChildInGrid( row, text,
			column, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( text );

    /*
    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );
    */
    g_object_set( G_OBJECT( text ),
			"editable", textEnabled != 0,
			NULL );

    if  ( ! textEnabled )
	{ gtk_widget_set_sensitive( text, FALSE );	}

    *pText= text;
    }

void guiMakeTextInHBox(		APP_WIDGET *		pText,
				APP_WIDGET		hbox,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		text= gtk_entry_new();

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    gtk_box_pack_start( GTK_BOX( hbox ), text, FALSE, FALSE, 0 );

    gtk_widget_show( text );

    /*
    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );
    */
    g_object_set( G_OBJECT( text ),
			"editable", textEnabled != 0,
			NULL );

    if  ( ! textEnabled )
	{ gtk_widget_set_sensitive( text, FALSE );	}

    *pText= text;
    }


/************************************************************************/
/*									*/
/*  Make a text widget that is contained in a column of widgets		*/
/*									*/
/************************************************************************/

void guiMakeTextInColumn(	APP_WIDGET *	pText,
				APP_WIDGET	column,
				int		textColumns,
				int		textEnabled )
    {
    APP_WIDGET		text;

    text= gtk_entry_new();

    gtk_widget_set_name(GTK_WIDGET (text), "tedColumnText");

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    gtk_box_pack_start( GTK_BOX( column ), text, FALSE, TRUE, 0 );

    /*
    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );
    */
    g_object_set( G_OBJECT( text ),
			"editable", textEnabled != 0,
			NULL );

    gtk_widget_show( text );

    *pText= text;
    }

void guiRefuseTextValue(	APP_WIDGET		text )
    {
    gtk_editable_select_region( GTK_EDITABLE( text ), 0, 
			strlen( gtk_entry_get_text( GTK_ENTRY( text ) ) ) );

    gtk_widget_grab_focus( text );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void guiStringToText(		APP_WIDGET	text,
				const char *	s )
    {
    gtk_entry_set_text( GTK_ENTRY( text ), s );

    return;
    }

/************************************************************************/
/*									*/
/*  Select text in a text entry box.					*/
/*									*/
/************************************************************************/

void guiTextSelectContents(		APP_WIDGET	w,
					int		from,
					int		upto )
    {
    gtk_editable_select_region( GTK_EDITABLE(w), from, upto );
    return;
    }

/************************************************************************/
/*									*/
/*  Turn a text widget on or off.					*/
/*									*/
/************************************************************************/

void guiEnableText(		APP_WIDGET	text,
				int		enabled )
    {
    gtk_widget_set_sensitive( text, enabled != 0 );
    /*
    gtk_entry_set_editable( GTK_ENTRY( text ), enabled != 0 );
    */
    g_object_set( G_OBJECT( text ),
			"editable", enabled != 0,
			NULL );
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve strings from text widgets.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	guiStringFromText(	APP_WIDGET	text )
    {
    return strdup( gtk_entry_get_text( GTK_ENTRY( text ) ) );
    }

/*  2  */
void guiFreeStringFromTextWidget(	char *		s )
    {
    free( s );
    }

void guiSetTypingCallbackForText(	APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through )
    {
    g_signal_connect( G_OBJECT( text ), "changed",
					(GCallback)callBack, through );

    return;
    }

void guiSetGotValueCallbackForText(
				APP_WIDGET			text,
				APP_TXACTIVATE_CALLBACK_T	callBack,
				void *				through )
    {
    g_signal_connect( G_OBJECT( text ), "activate",
					(GCallback)callBack, through );

    return;
    }

#   endif
