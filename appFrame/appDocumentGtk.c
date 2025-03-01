/************************************************************************/
/*									*/
/*  Gtk specific document configuration.				*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_GTK

#   include	<stddef.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocument.h"
#   include	"appDocFront.h"
#   include	<guiDrawingWidget.h>
#   include	<guiWidgetsGtk.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Event handler to keep track of resize events for the document.	*/
/*  Both resize events of the shel widget and the document widget are	*/
/*  handeled to keep track of the difference in size.			*/
/*									*/
/*  The difference in size is used to set the maximum size of the	*/
/*  shell widget in the window manager.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appDocTopConfigure, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( event->type == GDK_MAP )
	{ ed->edMapped= 1;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a document window.						*/
/*									*/
/*  1)  Allocate a document.						*/
/*  3)  Make a shell to contain everything.				*/
/*  4)  Make the window pane.						*/
/*									*/
/************************************************************************/

static int appDocMakeMainWindow(	EditDocument *		ed )
    {
    int			rval= 0;
    EditApplication *	ea= ed->edApplication;

    const int		spacing= 0;

    MemoryBuffer	fullTitle;
    MemoryBuffer	iconName;

    utilInitMemoryBuffer( &fullTitle );
    utilInitMemoryBuffer( &iconName );

    if  ( appFormatDocumentTitle( &fullTitle, &iconName, ea, &(ed->edTitle) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ed->edToplevel.atTopWidget= gtk_window_new( GTK_WINDOW_TOPLEVEL );

    gtk_window_set_title( GTK_WINDOW( ed->edToplevel.atTopWidget ),
				    utilMemoryBufferGetString( &fullTitle ) );

    gtk_window_set_position( GTK_WINDOW( ed->edToplevel.atTopWidget ),
						GTK_WIN_POS_CENTER );
    /*
    gtk_window_set_policy( GTK_WINDOW( ed->edToplevel.atTopWidget ), 
						    TRUE, TRUE, FALSE );
    */
    gtk_window_set_resizable( GTK_WINDOW( ed->edToplevel.atTopWidget ), TRUE );

    gtk_widget_realize( ed->edToplevel.atTopWidget );

    appSetNamedWindowIcon( ea, ed->edToplevel.atTopWidget, ea->eaMainIcon );

#   if  GTK_MAJOR_VERSION < 3
    gdk_window_set_icon_name(
		    gtk_widget_get_window( ed->edToplevel.atTopWidget ),
		    utilMemoryBufferGetString( &iconName ) );
#   else
    gtk_window_set_icon_name(
			    GTK_WINDOW( ed->edToplevel.atTopWidget ),
			    utilMemoryBufferGetString( &iconName ) );
#   endif

    guiSetCloseCallback( ed->edToplevel.atTopWidget,
					appDocFileCloseCallback, (void *)ed );

    guiSetDestroyCallback( ed->edToplevel.atTopWidget,
					appDestroyEditDocument, (void *)ed );

#   if GTK_MAJOR_VERSION < 3
    ed->edMainWindow= gtk_vbox_new( FALSE, spacing );
#   else
    ed->edMainWindow= gtk_box_new( GTK_ORIENTATION_VERTICAL, spacing );
#   endif
    gtk_container_add( GTK_CONTAINER( ed->edToplevel.atTopWidget ),
							ed->edMainWindow );
    gtk_widget_show( ed->edMainWindow );

    ed->edToplevel.atAccelGroup= gtk_accel_group_new();

#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( ed->edToplevel.atAccelGroup,
				    G_OBJECT( ed->edToplevel.atTopWidget ) );
#   else
    gtk_window_add_accel_group( GTK_WINDOW( ed->edToplevel.atTopWidget ),
					    ed->edToplevel.atAccelGroup );
#   endif

    ed->edMenuBar= gtk_menu_bar_new();
    gtk_widget_show( ed->edMenuBar );
    gtk_box_pack_start( GTK_BOX( ed->edMainWindow ),
					    ed->edMenuBar, FALSE, TRUE, 0 );

    appDocFillMenu( ed );

  ready:

    utilCleanMemoryBuffer( &fullTitle );
    utilCleanMemoryBuffer( &iconName );

    return rval;
    }

static int appDocMakeScrolledWindow(	EditDocument *		ed )
    {
    ed->edScrolledWindow= guiGtkMakeGrid( 2, 2, FALSE );

    gtk_widget_show( ed->edScrolledWindow );
    gtk_box_pack_start( GTK_BOX( ed->edMainWindow ),
				    ed->edScrolledWindow, TRUE, TRUE, 0 );

    /*****/

#   if GTK_MAJOR_VERSION < 3
    ed->edVerticalScrollbar= gtk_vscrollbar_new( NULL );
#   else
    ed->edVerticalScrollbar=
	    gtk_scrollbar_new( GTK_ORIENTATION_VERTICAL, NULL );
#   endif
    ed->edVerticalAdjustment=
	    gtk_range_get_adjustment( GTK_RANGE( ed->edVerticalScrollbar ) );

    guiGtkAttachChildInGrid( ed->edScrolledWindow, ed->edVerticalScrollbar,
			1, 1, /* col, colspan */
			0, 1, /* row, rowspan */
			FALSE, TRUE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edVerticalScrollbar );

    /*****/

#   if GTK_MAJOR_VERSION < 3
    ed->edHorizontalScrollbar= gtk_hscrollbar_new( NULL );
#   else
    ed->edHorizontalScrollbar=
			gtk_scrollbar_new( GTK_ORIENTATION_HORIZONTAL, NULL );
#   endif
    ed->edHorizontalAdjustment=
	    gtk_range_get_adjustment( GTK_RANGE( ed->edHorizontalScrollbar ) );

    guiGtkAttachChildInGrid( ed->edScrolledWindow, ed->edHorizontalScrollbar,
			0, 1, /* col, colspan */
			1, 1, /* row, rowspan */
			TRUE, FALSE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edHorizontalScrollbar );

    return 0;
    }

int appMakeDocumentWidget(	EditDocument *		ed )
    {
    appDocumentRulerWidth( ed );

    /*  2  */
    ed->edWorkWidget= guiGtkMakeGrid( 3, 3, FALSE );

    guiGtkAttachChildInGrid( ed->edScrolledWindow, ed->edWorkWidget,
			0, 1, /* col, colspan */
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edWorkWidget );

    ed->edTopRulerWidget= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( ed->edWorkWidget, ed->edTopRulerWidget,
			0, 3, /* col, colspan */
			0, 1, /* row, rowspan */
			TRUE, FALSE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edTopRulerWidget );

    ed->edBottomRulerWidget= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( ed->edWorkWidget, ed->edBottomRulerWidget,
			0, 3, /* col, colspan */
			2, 1, /* row, rowspan */
			TRUE, FALSE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edBottomRulerWidget );

    ed->edLeftRulerWidget= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( ed->edWorkWidget, ed->edLeftRulerWidget,
			0, 1, /* col, colspan */
			1, 1, /* row, rowspan */
			FALSE, TRUE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edLeftRulerWidget );

    ed->edRightRulerWidget= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( ed->edWorkWidget, ed->edRightRulerWidget,
			2, 1, /* col, colspan */
			1, 1, /* row, rowspan */
			FALSE, TRUE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edRightRulerWidget );

    ed->edDocumentWidget.dwWidget= gtk_drawing_area_new();

    guiGtkAttachChildInGrid( ed->edWorkWidget, ed->edDocumentWidget.dwWidget,
			1, 1, /* col, colspan */
			1, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			0, 0 );

    gtk_widget_show( ed->edDocumentWidget.dwWidget );

#   if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
    GTK_WIDGET_SET_FLAGS( ed->edDocumentWidget.dwWidget, GTK_CAN_FOCUS );
#   else
    gtk_widget_set_can_focus( ed->edDocumentWidget.dwWidget, TRUE );
#   endif
    gtk_window_set_focus( GTK_WINDOW( ed->edToplevel.atTopWidget ),
					    ed->edDocumentWidget.dwWidget );

    return 0;
    }

static int appDocMakeToolbar(	EditDocument *		ed )
    {
# if 0
#   if GTK_MAJOR_VERSION < 3
    ed->edToolbar= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    ed->edToolbar= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    gtk_container_add( GTK_CONTAINER( ed->edMainWindow ), ed->edToolbar );

    gtk_widget_set_name( ed->edToolbar, "tedInspector");

    gtk_widget_show( ed->edToolbar );
/*
Disables arrow keys for one reason or another
LDEB(1);gtk_widget_set_can_focus(ed->edToolbar,1);
*/

# endif
    return 0;
    }

int appFinishDocumentWindow(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    DocumentWidget *	dw= &(ed->edDocumentWidget);

    /*  3  */
    if  ( appDocMakeMainWindow( ed )			||
	  appDocMakeToolbar( ed )			||
	  appDocMakeScrolledWindow( ed )		||
	  (*ea->eaMakeDocumentWidget)( ed )		)
	{ LDEB(1); return -1; }

    g_signal_connect( G_OBJECT( dw->dwWidget ),
				    "configure_event",
				    (GCallback)appDocConfigure,
				    (void *)ed );

    g_signal_connect( G_OBJECT( ed->edToplevel.atTopWidget ), 
				    "map_event",
				    (GCallback)appDocTopConfigure,
				    (void *)ed );

    g_signal_connect( G_OBJECT( dw->dwWidget ),
				    "selection_clear_event",
				    (GCallback)appDocCopyPasteHandler,
				    (void *)ed );

    g_signal_connect( G_OBJECT( dw->dwWidget ),
				    "selection_received",
				    (GCallback)appDocGotPasteReplyGtk,
				    (void *)ed );

    g_signal_connect( G_OBJECT( dw->dwWidget ),
				    "selection_get",
				    (GCallback)appDocReplyToCopyRequest,
				    (void *)ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish the setup of a document window.				*/
/*									*/
/*  2)  The drawing area does not deliver focus out events so install	*/
/*	the focus event handler on the top level widget.		*/
/*									*/
/************************************************************************/

int appFinishDocumentSetup(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    DocumentWidget *	dw= &(ed->edDocumentWidget);

    guiDrawSetRedrawHandler( dw->dwWidget, appDocExposeHandler, (void *)ed );

    if  ( ea->eaObserveFocus )
	{
	/*  2  */
#	if 1
	guiSetFocusChangeHandler( ed->edToplevel.atTopWidget,
					    ea->eaObserveFocus, (void *)ed );
#	else
	guiSetFocusChangeHandler( dw->dwWidget,
					    ea->eaObserveFocus, (void *)ed );
#	endif
	}

    if  ( ea->eaDocumentMouseHandler )
	{
	guiDrawSetButtonPressHandler( dw->dwWidget,
				    ea->eaDocumentMouseHandler, (void *)ed );

	gtk_widget_add_events( dw->dwWidget, GDK_POINTER_MOTION_MASK );
	}

    if  ( ea->eaDocumentScrollHandler )
	{
	guiDrawSetScrollHandler( dw->dwWidget,
				    ea->eaDocumentScrollHandler, (void *)ed );
	}

    dw->dwOwner= (void *)ed;
    dw->dwGotString= ea->eaDocGotString;
    dw->dwGotKey= ea->eaDocGotKey;

    g_signal_connect( G_OBJECT( ed->edVerticalAdjustment ),
			    "value_changed", 
			    (GCallback)appDocVerticalScrollbarCallback,
			    (void *)ed );

    g_signal_connect( G_OBJECT( ed->edHorizontalAdjustment ),
			    "value_changed", 
			    (GCallback)appDocHorizontalScrollbarCallback,
			    (void *)ed );

    guiDocumentSetInputContext( ea->eaInputMethod, dw );

    return 0;
    }

void appDocSetVerticalScrollbarValues(
				EditDocument *		ed,
				int			minimum,
				int			maximum,
				int			value,
				int			sliderSize )
    {
    gtk_adjustment_set_lower( ed->edVerticalAdjustment, minimum );
    gtk_adjustment_set_upper( ed->edVerticalAdjustment, maximum );
    gtk_adjustment_set_value( ed->edVerticalAdjustment, value );
    gtk_adjustment_set_page_size( ed->edVerticalAdjustment, sliderSize );
    gtk_adjustment_set_page_increment( ed->edVerticalAdjustment,
			( 9* sliderSize+ 9 )/10 );
    gtk_adjustment_set_step_increment( ed->edVerticalAdjustment,
			( sliderSize+ SCROLL_BAR_STEP- 1 )/ SCROLL_BAR_STEP );

    gtk_adjustment_changed( ed->edVerticalAdjustment );

    return;
    }

void appDocSetHorizontalScrollbarValues(
				EditDocument *		ed,
				int			minimum,
				int			maximum,
				int			value,
				int			sliderSize )
    {
    gtk_adjustment_set_lower( ed->edHorizontalAdjustment, minimum );
    gtk_adjustment_set_upper( ed->edHorizontalAdjustment, maximum );
    gtk_adjustment_set_value( ed->edHorizontalAdjustment, value );
    gtk_adjustment_set_page_size( ed->edHorizontalAdjustment, sliderSize );

    /* Otherwise, the contols at the end are inactive: */
    gtk_adjustment_set_page_increment( ed->edHorizontalAdjustment,
			( 9* sliderSize+ 9 )/10 );
    gtk_adjustment_set_step_increment( ed->edHorizontalAdjustment,
			( sliderSize+ SCROLL_BAR_STEP- 1 )/ SCROLL_BAR_STEP );

    gtk_adjustment_changed( ed->edHorizontalAdjustment );
    }

#   endif
