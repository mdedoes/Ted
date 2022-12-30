#   include	"guiBaseConfig.h"
#   include	"guiBase.h"

#   if USE_GTK

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiWidgetsImpl.h"
#   include	"guiDrawingWidget.h"
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/************************************************************************/

static void guiCallGtkWidgetDestroy( GtkWidget * widget, void * null )
    {
    gtk_widget_destroy( widget );
    }

void guiEmptyParentWidget(	APP_WIDGET		parent )
    {
    gtk_container_foreach( GTK_CONTAINER( parent ),
				    guiCallGtkWidgetDestroy, NULL );
    /*
    gtk_container_foreach( GTK_CONTAINER( parent ),
				    guiCallGtkWidgetDestroy, NULL );
				    (GtkCallback)gtk_widget_destroy, NULL );
    */
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void guiInsertColumnInWindow(	APP_WIDGET *	pColumn,
				APP_WIDGET	shell )
    {
#   if GTK_MAJOR_VERSION < 3
    GtkWidget *		column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    GtkWidget *		column= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    gtk_widget_show( column );

    gtk_container_add( GTK_CONTAINER( shell ), column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void guiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
				APP_WIDGET	column )
    {
    GtkWidget *		menubar= gtk_menu_bar_new();

    gtk_widget_show( menubar );

    gtk_box_pack_start( GTK_BOX( column ), menubar, FALSE, TRUE, 0 );

    *pMenubar= menubar;
    return;
    }

/************************************************************************/
/*									*/
/*  Enable/Dispable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void guiEnableWidget(	APP_WIDGET		w,
			int			on_off )
    {
    gtk_widget_set_sensitive( w, on_off != 0 );
    }


void guiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    if  ( on_off )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void guiSetDialogTitle(	APP_WIDGET		shell,
			APP_WIDGET		option,
			const char *		applicationName )
    {
    char *	title;

    if  ( option )
	{
	char *		s;

	s= guiGetTextFromMenuOption( option );

	title= (char *)malloc( strlen( applicationName )+ 1+ strlen( s )+ 1 );
	sprintf( title, "%s %s", applicationName, s );

	guiFreeTextFromMenuOption( s );
	}
    else{ title= strdup( applicationName );	}

    gtk_window_set_title( GTK_WINDOW( shell ), title );

    free( title );
    }

void guiSetShellTitle(	APP_WIDGET		shell,
			const MemoryBuffer *	fullTitle )
    {
    gtk_window_set_title( GTK_WINDOW( shell ),
				    utilMemoryBufferGetString( fullTitle ) );

    return;
    }

void guiSetIconTitle(		APP_WIDGET		shell,
				const MemoryBuffer *	iconName )
    {
    GdkWindow *		win= gtk_widget_get_window( shell );

    /*
     * gtk_window_set_icon_name() Sets the name of a theme icon.. 
     * NOT the name displayed on the window icon via the ICCM 
     * WM_ICON_NAME property.
     */

    if  ( win )
	{
	gdk_window_set_icon_name( win,
				    utilMemoryBufferGetString( iconName ) );
	}

    return;
    }

void guiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    gtk_adjustment_set_value( adjustment, value );
    gtk_adjustment_set_page_size( adjustment, sliderSize );

    gtk_adjustment_changed( adjustment );

    return;
    }

void guiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    *pValue= gtk_adjustment_get_value( adjustment );
    *pSliderSize= gtk_adjustment_get_page_size( adjustment );

    return;
    }

void guiHideShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_hide( shell );
    }

void guiDestroyShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_destroy( shell );
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void guiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
#   if GTK_MAJOR_VERSION < 3
    GtkWidget *	separator= gtk_hseparator_new();
#   else
    GtkWidget *	separator= gtk_separator_new( GTK_ORIENTATION_HORIZONTAL );
#   endif

    gtk_box_pack_start( GTK_BOX( column ), separator, FALSE, TRUE, 0 );

    gtk_widget_show( separator );

    *pSeparator= separator;

    return;
    }

void guiMakeDrawingAreaInColumn(
				APP_WIDGET *		pDrawing,
				APP_WIDGET		column,
				int			wide,
				int			high,
				int			heightResizable,
				APP_REDRAW_HANDLER_T	redraw,
				void *			through )
    {
    GtkWidget *	drawing= gtk_drawing_area_new();
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    if  ( wide < 0 )
	{ wide= 0;	}
    if  ( high < 0 )
	{ high= 0;	}

    if  ( high <= 0 || heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), drawing, expand, fill, 0 );

    /*
    gtk_drawing_area_size( GTK_DRAWING_AREA( drawing ), wide, high );
    */
    gtk_widget_set_size_request( drawing, wide, high );

    gtk_widget_show( drawing );

    *pDrawing= drawing;

    if  ( redraw )
	{ guiDrawSetRedrawHandler( drawing, redraw, through );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a column itself.	*/
/*									*/
/************************************************************************/

static void appGuiGtkMakeFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pTable,
					APP_WIDGET		parent,
					APP_WIDGET		child,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;

    frame= gtk_frame_new( title );

    gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );

    gtk_box_pack_start( GTK_BOX( parent ), frame, FALSE, TRUE, 0 );

    gtk_widget_show( frame );

    table= guiGtkMakeGrid( 1, 1, TRUE );

    gtk_container_add( GTK_CONTAINER( frame ), table );

    gtk_widget_show( table );

    guiGtkAttachChildInGrid( table, child,
			0, 1, /* col, colspan */
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			FRAME_BORDER_GTK, FRAME_BORDER_GTK );

    gtk_widget_show( child );

    *pFrame= frame; *pTable= table; return;
    }

void guiMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		paned;

#   if GTK_MAJOR_VERSION < 3
    paned= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    paned= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, paned, title );

    *pFrame= frame; *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  Set the title of a frame.						*/
/*									*/
/************************************************************************/

int guiSetFrameTitle(		APP_WIDGET		frame,
				const char *		title )
    {
    gtk_frame_set_label( GTK_FRAME( frame ), (const char *)title );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a row itself.	*/
/*									*/
/************************************************************************/

void guiMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
				APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				int			columnCount,
				const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		row;

    row= guiGtkMakeGrid( 1, columnCount, TRUE );

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, row, title );

    *pFrame= frame; *pRow= row; return;
    }

int guiGetScrollbarValueFromCallbackGtk( APP_WIDGET		scrollbar )
    {
    GtkAdjustment *	adjustment= (GtkAdjustment *)scrollbar;

    return gtk_adjustment_get_value( adjustment );
    }

void guiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    gtk_widget_add_events( w, GDK_FOCUS_CHANGE_MASK );

    g_signal_connect( G_OBJECT( w ),
				    "focus_in_event",
				    (GCallback)handler, through );

    g_signal_connect( G_OBJECT( w ), "focus_out_event",
				    (GCallback)handler, through );
    }

void guiSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	/*
	g_signal_connect( G_OBJECT( shell ), "destroy_event",
				    (GCallback)destroyCallback, through );
	g_signal_connect( G_OBJECT( shell ), "destroy-event",
				    (GCallback)destroyCallback, through );
	*/
	g_signal_connect( G_OBJECT( shell ), "destroy",
				    (GCallback)destroyCallback, through );
	}
    }

void guiSetCloseCallback(	APP_WIDGET		shell,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    if  ( closeCallback )
	{
	g_signal_connect( G_OBJECT( shell ), "delete_event",
				    (GCallback)closeCallback, through );
	}
    }

int guiGtkSetChildLabel(	GtkWidget *		w,
				const char *		s )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{ gtk_label_set_text( GTK_LABEL( w ), s ); return 1;	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_get_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( guiGtkSetChildLabel( (GtkWidget *)gl->data, s ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

int guiGtkGetChildLabel(	const char **		pLabel,
				GtkWidget *		w )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{
	*pLabel= gtk_label_get_label( GTK_LABEL( w ) );
	return 1;
	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_get_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( guiGtkGetChildLabel( pLabel, (GtkWidget *)gl->data ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

void guiFocusToWidget(	APP_WIDGET		w )
    {
    gtk_widget_grab_focus( w );
    }

void guiGtkListTree(		int			indent,
				APP_WIDGET		w )
    {
    GType		type= G_TYPE_FROM_INSTANCE( w );
    const char *	typeName;
    const char *	instanceName= gtk_widget_get_name( w );

    GtkAllocation	alloc;

    gtk_widget_get_allocation( w, &alloc );

    typeName= g_type_name( type );
    appDebug( "%*s%s%s%s (%d..%d x %d..%d)\n", 2* indent, "",
				typeName,
				( instanceName?" ":"" ),
				( instanceName?instanceName:"" ),
				alloc.x, alloc.x+ alloc.width,
				alloc.y, alloc.y+ alloc.height );

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_get_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    guiGtkListTree( indent+ 1, (GtkWidget *)gl->data );

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return;
    }

# endif

