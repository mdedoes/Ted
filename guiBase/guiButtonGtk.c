#   include	"guiBaseConfig.h"

#   if USE_GTK

#   include	<stdio.h>
#   include	"guiWidgets.h"
#   include	<appDebugon.h>
#   include	"guiWidgetsGtk.h"

static void guiGtkRemoveButtonDefault(	GtkWidget *	button )
    {
#   if  ( GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12 )
	GTK_WIDGET_UNSET_FLAGS( button, GTK_CAN_FOCUS );
	GTK_WIDGET_UNSET_FLAGS( button, GTK_CAN_DEFAULT );
#   else
	gtk_widget_set_can_focus( button, FALSE );
	gtk_widget_set_can_default( button, FALSE );
#   endif
    }

static GtkWidget * guiGtkMakeTextButton(
				const char *		text,
				int			showAsDefault )
    {
    GtkWidget *		button= gtk_button_new_with_label( (char *)text );

    gtk_button_set_relief ( GTK_BUTTON(button), GTK_RELIEF_HALF );

    if  ( ! showAsDefault )
	{ guiGtkRemoveButtonDefault( button );	}

    return button;
    }

static GtkWidget * guiGtkMakeImageButton(
				struct NamedImage *	ni,
				const char *		tooltip,
				int			showAsDefault )
    {
    GtkWidget *		button= gtk_button_new();
    APP_WIDGET		imageWidget;

    if  ( guiGtkMakeImageWidget( &imageWidget, ni ) )
	{
	XDEB(ni);
	return guiGtkMakeTextButton( tooltip, showAsDefault );
	}

    gtk_button_set_image( GTK_BUTTON( button ), imageWidget );

#   if (GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 12)
    gtk_widget_set_tooltip_text( button, tooltip );
#   endif

    return button;
    }

static void guiGtkAttachButtonInRow(
				APP_WIDGET		row,
				APP_WIDGET		button,
				int			position,
				int			colspan )
    {
    gtk_widget_set_name( GTK_WIDGET(button), "tedRowButton" );

    guiGtkAttachChildInGrid( row, button,
		    position, colspan,
		    0, 1, /* row, rowspan */
		    TRUE, TRUE, /* hexpand, vexpand */
		    ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( button );

    return;
    }

static void guiGtkAttachButtonInColumn(
				APP_WIDGET		column,
				APP_WIDGET		button )
    {
    const gboolean	expand= FALSE;
    const gboolean	fill= TRUE;
    const guint		padding= 0;

    gtk_widget_set_name(GTK_WIDGET (button), "tedColumnButton");

    gtk_box_pack_start( GTK_BOX( column ), button, expand, fill, padding );

    gtk_widget_show( button );
    }

void guiMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault )
    {
    GtkWidget *		button;

    button= guiGtkMakeTextButton( text, showAsDefault );

    guiGtkAttachButtonInRow( row, button, position, colspan );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( button ), "clicked",
					(GCallback)callback, through );
	}

    *pButton= button;

    return;
    }

void guiMakeButtonInColumn(	APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault )
    {
    GtkWidget *		button;

    button= guiGtkMakeTextButton( text, showAsDefault );

    guiGtkAttachButtonInColumn( button, column );

    gtk_widget_show( button );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( button ), "clicked",
					(GCallback)callback, through );
	}

    *pButton= button;

    return;
    }

void guiMakeImageButtonInColumn( APP_WIDGET *		pButton,
				APP_WIDGET		column,
				struct NamedImage *	ni,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through )
    {
    GtkWidget *		button;
    const int		showAsDefault= 0;

    button= guiGtkMakeImageButton( ni, tooltip, showAsDefault );

    guiGtkAttachButtonInColumn( button, column );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( button ), "clicked",
					(GCallback)callback, through );
	}

    *pButton= button;

    return;
    }

void guiMakeImageButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				struct NamedImage *	ni,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan )
    {
    GtkWidget *		button;
    const int		showAsDefault= 0;

    button= guiGtkMakeImageButton( ni, tooltip, showAsDefault );

    guiGtkAttachButtonInRow( row, button, position, colspan );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( button ), "clicked",
					(GCallback)callback, through );
	}

    *pButton= button;

    return;
    }

void guiChangeButtonText(	APP_WIDGET		button,
				const char *		label )
    {
    if  ( ! guiGtkSetChildLabel( button, (char *)label ) )
	{ SDEB((char *)label);	}
    }

#   endif
