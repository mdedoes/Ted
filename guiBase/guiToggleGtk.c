#   include	"guiBaseConfig.h"

#   if USE_GTK

#   include	<stdio.h>

#   include	"guiBase.h"
#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void guiSetToggleLabel(	APP_WIDGET		toggle,
				const char *		text )
    {
    if  ( ! guiGtkSetChildLabel( toggle, text ) )
	{ SDEB(text);	}
    }

void guiSetToggleState(	APP_WIDGET		toggle,
				int			set )
    {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( toggle ), set != 0 );
    }

int guiGetToggleState(	APP_WIDGET		toggle )
    {
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( toggle ) ) != 0;
    }

int guiGetToggleStateFromCallbackGtk( GtkWidget *	toggle )
    {
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( toggle ) ) != 0;
    }

APP_WIDGET guiMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			col,
				int			colspan )
    {
    GtkWidget *	toggle;

    if  ( text )
	{
	toggle= gtk_check_button_new_with_label( (char *)text );
	}
    else{
	toggle= gtk_check_button_new();
	}

    gtk_widget_set_name( GTK_WIDGET(toggle), "tedRowToggle" );

    guiGtkAttachChildInGrid( row, toggle,
			col, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( toggle );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( toggle ), "toggled",
					    (GCallback)callback, through );
	}

    return toggle;
    }

#   endif /* USE_GTK */
