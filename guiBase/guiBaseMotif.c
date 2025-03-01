#   include	"guiBaseConfig.h"

#   if USE_MOTIF

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"guiBase.h"
#   include	"guiWidgets.h"
#   include	"guiWidgetsImpl.h"
#   include	"guiImageImpl.h"

#   include	<utilMemoryBuffer.h>

#   include	<X11/Xatom.h>
#   include	<Xm/RowColumn.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/DrawnB.h>
#   include	<Xm/Separator.h>
#   include	<Xm/DrawingA.h>
#   include	<Xm/ScrollBar.h>
#   include	<Xm/AtomMgr.h>
#   include	<Xm/Protocols.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/*  1)  Unmanaging should be done by the Xt toolkit but is not always	*/
/*	done correctly. Doing it here does no harm anyway.		*/
/*									*/
/************************************************************************/

void guiEmptyParentWidget(	Widget		parent )
    {
    WidgetList		children;
    WidgetList		save;
    Cardinal		childCount;

    XtVaGetValues( parent,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    /*  1  */
    XtUnmanageChildren( children, childCount );

    save= (WidgetList)malloc( childCount* sizeof(Widget) );
    if  ( save )
	{
	int	i;

	for ( i= childCount- 1; i >= 0; i-- )
	    { save[i]= children[i];	}

	for ( i= childCount- 1; i >= 0; i-- )
	    { XtDestroyWidget( save[i] );	}

	free( save );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void guiSetDialogTitle(	APP_WIDGET		shell,
			Widget			option,
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

    XtVaSetValues( shell,
		    XmNtitle, title,
		    NULL );

    free( title );
    }

/************************************************************************/
/*									*/
/*  1)  Remember te initial size of a shell as its minimum size.	*/
/*									*/
/************************************************************************/

/*  1  */
void appSetSizeAsMinimum(	Widget			w,
				void *			through,
				XEvent *		event,
				Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    *pRefused= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Names of the X11 events.						*/
/*									*/
/************************************************************************/

const char * const APP_X11EventNames[]=
    {
    "0", "1", "KeyPress", "KeyRelease", "ButtonPress", "ButtonRelease",
    "MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn", "FocusOut",
    "KeymapNotify", "Expose", "GraphicsExpose", "NoExpose", "VisibilityNotify",
    "CreateNotify", "DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
    "ReparentNotify", "ConfigureNotify", "ConfigureRequest", "GravityNotify",
    "ResizeRequest", "CirculateNotify", "CirculateRequest", "PropertyNotify",
    "SelectionClear", "SelectionRequest", "SelectionNotify", "ColormapNotify",
    "ClientMessage", "MappingNotify"
    };


void guiMakeColumnDrawing(	Widget *		pButton,
				Widget			column,
				XtCallbackProc		exposeCallback,
				XtCallbackProc		pushedCallback,
				void *			through,
				int			width )
    {
    Widget		button;

    Arg			al[20];
    int			ac= 0;

    XmString		labelString= XmStringCreateLocalized( (char *)" " );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNwidth,			width ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		1 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		4 ); ac++;

    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    button= XmCreateDrawnButton( column, WIDGET_NAME, al, ac );

    if  ( exposeCallback )
	{
	XtAddCallback( button, XmNexposeCallback, exposeCallback, through );
	}

    if  ( pushedCallback )
	{
	XtAddCallback( button, XmNactivateCallback, pushedCallback, through );
	}

    XtManageChild( button );

    XmStringFree( labelString );

    *pButton= button;
    }

/************************************************************************/
/*									*/
/*  For debugging purposes: Give widgets a name of the form file(line)	*/
/*  for where they are made.						*/
/*									*/
/************************************************************************/

char * guiWidgetNameMotif(	char *	file,
				int	line )
    {
    static char **	names;
    static int		nameCount;

    char **		freshNames;
    char *		freshName;

    int			i;

    freshNames= (char **)realloc( names, ( nameCount+ 1 )* sizeof(char *) );
    if  ( ! freshNames )
	{ XDEB(freshNames); return file;	}
    names= freshNames;

    freshName= (char *)malloc( strlen( file )+ 11 );
    if  ( ! freshName )
	{ XDEB(freshName); return file;	}

    sprintf( freshName, "%s(%d)", file, line );

    for ( i= 0; i < nameCount; freshNames++, i++ )
	{
	if  ( ! strcmp( freshNames[0], freshName ) )
	    { free( freshName ); return freshNames[0];	}
	}

    names[nameCount++]= freshName;

    return freshName;
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void guiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	parent )
    {
    APP_WIDGET		column;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac],	XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac],	XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac],	XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac],	XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac],	XmNmarginHeight,	0 ); ac++;
    XtSetArg( al[ac],	XmNspacing,		0 ); ac++;

    column= XmCreatePanedWindow( parent, WIDGET_NAME, al, ac );

    XtManageChild( column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void guiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
					APP_WIDGET	parent )
    {
    Widget		menuBar;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,		True ); ac++;

    menuBar= XmCreateMenuBar( parent, WIDGET_NAME, al, ac );

    XtManageChild( menuBar );

    *pMenubar= menuBar; return;
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void guiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
    Arg			al[20];
    int			ac= 0;

    *pSeparator= XmCreateSeparator( column, WIDGET_NAME, al, ac );

    XtManageChild( *pSeparator );

    return;
    }

/************************************************************************/
/*									*/
/*  Enable/Disable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void guiEnableWidget(	APP_WIDGET		w,
			int			on_off )
    {
    XtSetSensitive( w, on_off != 0 );
    }

void guiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    APP_WIDGET		parent= XtParent( w );

    XtVaSetValues( w,	XmNmappedWhenManaged,	on_off != 0,
			NULL );

    if  ( parent && XmIsRowColumn( parent ) )
	{
	if  ( on_off )
	    { XtManageChild( w );	}
	else{ XtUnmanageChild( w );	}
	}
    }

/************************************************************************/
/*									*/
/*  Hide a shell widget. Because of the RIDICULOUS and ABSURD situation	*/
/*									*/
/************************************************************************/

void guiHideShellWidget(		APP_WIDGET		shell )
    {
    if  ( XtIsRealized( shell ) )
	{ XtUnmapWidget( shell );	}
    }

void guiDestroyShellWidget(		APP_WIDGET		shell )
    {
    XtDestroyWidget( shell );
    }

void guiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    XtVaGetValues( scrollbar,
				    XmNvalue,		pValue,
				    XmNsliderSize,	pSliderSize,
				    NULL );
    return;
    }

void guiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    const Boolean		notify= False;

    XmScrollBarSetValues( scrollbar, value, sliderSize, 0,0, notify );

    return;
    }

int guiGetScrollbarValueFromCallback( APP_WIDGET		scrollbar,
					void *			voidcbs )
    {
    XmScrollBarCallbackStruct *	cbs= (XmScrollBarCallbackStruct *)voidcbs;

    return cbs->value;
    }

/************************************************************************/
/*									*/
/*  Set the title of the window.					*/
/*									*/
/*  See http://standards.freedesktop.org/wm-spec/wm-spec-1.3.html	*/
/*  for a better way: set _NET_WM_NAME to an UTF-8 value.		*/
/*									*/
/************************************************************************/

void guiSetShellTitle(		APP_WIDGET		shell,
				const MemoryBuffer *	fullTitle )
    {
    XtVaSetValues( shell,
			XmNtitle,	utilMemoryBufferGetString( fullTitle ),
			NULL );

    return;
    }

void guiSetIconTitle(		APP_WIDGET		shell,
				const MemoryBuffer *	iconName )
    {
    XtVaSetValues( shell,
			XmNiconName,	utilMemoryBufferGetString( iconName ),
			NULL );

    return;
    }

/************************************************************************/
/*									*/
/*  Make drawing areas. Typically these are small drawings so it does	*/
/*  no harm to redraw the whole drawing area after a resize.		*/
/*									*/
/************************************************************************/

void guiMotifSendExposeForResize(	APP_WIDGET		w,
					void *			through,
					void *			voidcbs )
    {
    if  ( XtIsRealized( w ) )
	{
	XClearArea( XtDisplay( w ), XtWindow( w ), 0, 0, 0, 0, True );
	}
    }

void guiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				    APP_WIDGET			column,
				    int				wide,
				    int				high,
				    int				heightResizable,
				    APP_EVENT_HANDLER_T		redraw,
				    void *			through )
    {
    APP_WIDGET	drawing;

    Arg		al[20];
    int		ac;

    /*
    XtSetArg( al[ac],	XmNsensitive,		False ); ac++;
    */

    ac= 0;
    if  ( high > 0 )
	{
	XtSetArg( al[ac], XmNheight,		high ); ac++;
	}

    if  ( high <= 0 || heightResizable )
	{ XtSetArg( al[ac], XmNskipAdjust,	False ); ac++;	}
    else{ XtSetArg( al[ac], XmNskipAdjust,	True ); ac++;	}

    if  ( wide > 0 )
	{
	XtSetArg( al[ac], XmNwidth,		wide ); ac++;
	}
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNnavigationType,	XmNONE ); ac++;

    drawing= XmCreateDrawingArea( column, WIDGET_NAME, al, ac );

    if  ( redraw )
	{
	XtAddEventHandler( drawing, ExposureMask, False, redraw, through );
	}

    XtAddCallback( drawing, XmNresizeCallback, guiMotifSendExposeForResize,
								(void *)0 );

    XtManageChild( drawing );

    *pDrawing= drawing;
    }

void guiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, FocusChangeMask, False, handler, through );
    }

void guiSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	XtAddCallback( shell, XmNdestroyCallback,
						destroyCallback, through );
	}
    }

void guiSetCloseCallback(	APP_WIDGET		shell,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    static int triedCloseAtom;
    static Atom closeAtom;

    if  ( closeCallback && ! triedCloseAtom )
	{
	closeAtom= XmInternAtom( XtDisplay( shell ),
					(char *)"WM_DELETE_WINDOW", False );
	triedCloseAtom= 1;
	}

    if  ( closeCallback && closeAtom > 0 )
	{
	XtVaSetValues( shell,
			    XmNdeleteResponse,		XmDO_NOTHING,
			    NULL );

	XmAddWMProtocolCallback( shell, closeAtom, closeCallback, through );
	}
    }

void guiFocusToWidget(	APP_WIDGET		w )
    {
    XmProcessTraversal( w, XmTRAVERSE_CURRENT );
    }

/************************************************************************/
/*									*/
/*  Cope with stubborn fluxbox.						*/
/*									*/
/************************************************************************/

void guiMotifSetFocusToWindow(	APP_WIDGET		w )
    {
    XWindowAttributes	xwa;

    if  ( ! XtIsRealized( w ) )
	{ return;	}

    XGetWindowAttributes( XtDisplay( w ), XtWindow( w ), &xwa );

    if  ( xwa.map_state != IsViewable )
	{ return;	}

    XSetInputFocus( XtDisplay( w ), XtWindow( w ), RevertToNone, CurrentTime );
    }

void guiMotifWaitForWindow(	APP_WIDGET		shell,
				XtAppContext		context )
    {
    const int		eventCount= 350;
    int			i;

    for ( i= 0; i < eventCount; i++ )
	{
	if  ( XtIsRealized( shell ) )
	    {
	    XWindowAttributes	xwa;

	    XGetWindowAttributes( XtDisplay( shell ), XtWindow( shell ), &xwa );
	    if  ( xwa.map_state != IsUnmapped )
		{ break;	}
	    }

	XtAppProcessEvent( context, XtIMAll );
	}

    if  ( i >= eventCount )
	{ LDEB(i);	}

    return;
    }

# ifdef XP_PRINTNAME
# ifdef FAKE_XP

XPContext XpGetContext( Display * display )
    { return (XPContext)0; }

char *XpGetOneAttribute(	Display *	display,
				XPContext	context,
				XPAttributes	type,
				char * attribute_name )
    { return "XpGetOneAttribute"; }

Bool XpSetImageResolution(	Display * display,
				XPContext  print_context,
				int  image_res,
				int * prev_res_return )
    { *prev_res_return= 100; return False; }

Bool XpQueryExtension(	Display *display,
			int *event_base_return,
			int *error_base_return )
    { return False;	}

void XpStartPage(	Display *display,
			Window window )
    { return;	}

void XpEndPage(	Display *display )
    { return;	}

void XpEndJob(	Display *display )
    { return;	}

Status XpGetPageDimensions(	Display *display,
				XPContext print_context,
				unsigned short *width,
				unsigned short *height,
				XRectangle *reproducible_area )
    { return 0; }

Screen *XpGetScreenOfContext (	Display *display,
				XPContext print_context )
    { return (Screen *)0; }

Status XpGetDocumentData (	Display *data_display,
				XPContext context,
				XPSaveProc save_proc,
				XPFinishProc finish_proc,
				XPointer client_data )
    { return 0;	}

Status XpGetPdmStartParams (	Display *print_display,
				Window print_window,
				XPContext print_context,
				Display *video_display,
				Window video_window,
				Display **selection_display_return,
				Atom *selection_return,
				Atom *type_return,
				int *format_return,
				unsigned char **data_return,
				int *nelements_return )
    { return 0;	}

void XpSelectInput (	Display *display,
			XPContext context,
			unsigned long event_mask )
    { return;	}

# endif /* FAKE_XP */
# endif /* XP_PRINTNAME */

#   endif
