/************************************************************************/
/*									*/
/*  Motif specific document configuration.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_MOTIF

#   include	<stddef.h>
#   include	<limits.h>

#   include	"appEditApplication.h"
#   include	"appEditDocument.h"
#   include	"appDocument.h"
#   include	"appDocFront.h"
#   include	"appGuiDocument.h"
#   include	<guiMenuItem.h>
#   include	<guiDrawingWidget.h>
#   include	<guiWidgetsImpl.h>
#   include	"appGuiApplication.h"

#   include	<appDebugon.h>

#   include	<Xm/Form.h>
#   include	<Xm/ScrolledW.h>
#   include	<Xm/DrawingA.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/ScrollBar.h>

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

static void appDocTopConfigure(	APP_WIDGET		w,
				void *			voided,
				XEvent *		event,
				Boolean *		pRefused )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( event->type == MapNotify )
	{
	ed->edMapped= 1;
	}

    if  ( event->type == ConfigureNotify )
	{
	XConfigureEvent *	cevent= (XConfigureEvent *)event;

	if  ( ! ed->edMapped && cevent->y < 0 )
	    {
	    XtVaSetValues( ed->edToplevel.atTopWidget,
				    XmNy,		40,
				    NULL );
	    }
	}

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

    Arg			al[20];
    int			ac;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );

    MemoryBuffer	fullTitle;
    MemoryBuffer	iconName;

    utilInitMemoryBuffer( &fullTitle );
    utilInitMemoryBuffer( &iconName );

    if  ( appFormatDocumentTitle( &fullTitle, &iconName, ea, &(ed->edTitle) ) )
	{
	SDEB(utilMemoryBufferGetString(&(ed->edTitle)));
	rval= -1; goto ready;
	}

    ac= 0;
    XtSetArg( al[ac], XmNtitle,			utilMemoryBufferGetString(
							&fullTitle ) ); ac++;
    XtSetArg( al[ac], XmNiconName,		utilMemoryBufferGetString(
							&iconName ) ); ac++;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmappedWhenManaged,	False ); ac++;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    /*
    XtSetArg( al[ac], XmNmaxWidth,		maxWidth ); ac++;
    XtSetArg( al[ac], XmNmaxHeight,		maxHeight ); ac++;
    */

#   ifdef USE_X11_R5
    ed->edToplevel.atTopWidget= XtAppCreateShell( ea->eaApplicationName, 
					    ea->eaDocumentWidgetName,
					    applicationShellWidgetClass,
					    display, al, ac );
#   else
    ed->edToplevel.atTopWidget= XtAppCreateShell( ea->eaApplicationName, 
					    WIDGET_NAME,
					    topLevelShellWidgetClass,
					    display, al, ac );
#   endif

    guiSetCloseCallback( ed->edToplevel.atTopWidget,
					appDocFileCloseCallback, (void *)ed );

    ac= 0;
    XtSetArg( al[ac], XmNsashWidth,	1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,	1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,	False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,	0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,	0 ); ac++;
    XtSetArg( al[ac], XmNspacing,	0 ); ac++;

    ed->edMainWindow= XmCreatePanedWindow(
			ed->edToplevel.atTopWidget, WIDGET_NAME, al, ac );

    guiInsertMenubarInColumn( &(ed->edMenuBar), ed->edMainWindow );

    appDocFillMenu( ed );

  ready:

    utilCleanMemoryBuffer( &fullTitle );
    utilCleanMemoryBuffer( &iconName );

    return rval;
    }

static int appDocMakeScrolledWindow(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    Arg			al[20];
    int			ac= 0;
    
    ac= 0;
    XtSetArg( al[ac], XmNscrollingPolicy,	XmAPPLICATION_DEFINED ); ac++;
    XtSetArg( al[ac], XmNscrollBarDisplayPolicy, XmSTATIC ); ac++;
    XtSetArg( al[ac], XmNhighlightThickness,	0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNpaneMaximum,		SHRT_MAX ); ac++;

    ed->edScrolledWindow=
	    XmCreateScrolledWindow( ed->edMainWindow, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNorientation,		XmVERTICAL ); ac++;
    XtSetArg( al[ac], XmNprocessingDirection,	XmMAX_ON_BOTTOM ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;
    ed->edVerticalScrollbar=
		XmCreateScrollBar( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edVerticalScrollbar, XmNvalueChangedCallback,
			    ea->eaDocVerticalScrollbarCallback, (void *)ed );
    XtAddCallback( ed->edVerticalScrollbar, XmNdragCallback,
			    ea->eaDocVerticalScrollbarCallback, (void *)ed );

    ac= 0;
    XtSetArg( al[ac], XmNorientation,		XmHORIZONTAL ); ac++;
    XtSetArg( al[ac], XmNprocessingDirection,	XmMAX_ON_RIGHT ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;
    ed->edHorizontalScrollbar=
		XmCreateScrollBar( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edHorizontalScrollbar, XmNvalueChangedCallback,
			    appDocHorizontalScrollbarCallback, (void *)ed );
    XtAddCallback( ed->edHorizontalScrollbar, XmNdragCallback,
			    appDocHorizontalScrollbarCallback, (void *)ed );

    guiMotifTurnOfSashTraversal( ed->edMainWindow );

    return 0;
    }

int appMakeDocumentWidget(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    Arg			al[20];
    int			ac= 0;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );
    int			screen= DefaultScreen( display );
    Pixel		blackPixel= BlackPixel( display, screen );

    appDocumentRulerWidth( ed );

    /*  2  */
    ed->edWorkWidget= XmCreateForm( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    /*  3  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	-ed->edTopRulerHighPixels ); ac++;

    ed->edTopRulerWidget=
		XmCreateDrawingArea( ed->edWorkWidget, WIDGET_NAME, al, ac );

    /*  4  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		-ed->edBottomRulerHighPixels ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edBottomRulerWidget=
		XmCreateDrawingArea( ed->edWorkWidget, WIDGET_NAME, al, ac );

    /*  5  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;

    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;

    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		-ed->edLeftRulerWidePixels ); ac++;

    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edLeftRulerWidget=
		XmCreateDrawingArea( ed->edWorkWidget, WIDGET_NAME, al, ac );

    /*  6  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		-ed->edRightRulerWidePixels ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edRightRulerWidget=
	    XmCreateDrawingArea( ed->edWorkWidget, WIDGET_NAME, al, ac );

    /*  7  */
    ac= 0;
    XtSetArg( al[ac],	XmNtraversalOn,		True ); ac++;

    /* NO: Postpone
    XtSetArg( al[ac],	XmNbackground,		whitePixel ); ac++;
    */
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNleftWidget,		ed->edLeftRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNrightWidget,		ed->edRightRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edDocumentWidget.dwWidget=
		XmCreateDrawingArea( ed->edWorkWidget, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edDocumentWidget.dwWidget,
		XmNdestroyCallback, appDestroyEditDocument, (void *)ed );

    XtManageChild( ed->edTopRulerWidget );
    XtManageChild( ed->edLeftRulerWidget );
    XtManageChild( ed->edRightRulerWidget );
    XtManageChild( ed->edBottomRulerWidget );
    XtManageChild( ed->edDocumentWidget.dwWidget );
    XtManageChild( ed->edWorkWidget );

    /* Not needed
    XmScrolledWindowSetAreas( ed->edScrolledWindow,
					ed->edHorizontalScrollbar,
					ed->edVerticalScrollbar,
					ed->edWorkWidget );
    */

    return 0;
    }

static int appDocMakeToolbar(	EditDocument *		ed )
    {
#   if 0
    Arg			al[20];
    int			ac= 0;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNseparatorOn,	False ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,	True ); ac++;
    XtSetArg( al[ac], XmNallowResize,	True ); ac++;

    ed->edToolbar= XmCreatePanedWindow( ed->edMainWindow, WIDGET_NAME, al, ac );
#   endif

    return 0;
    }

int appFinishDocumentWindow(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    /*  3  */
    if  ( appDocMakeMainWindow( ed )		||
	  appDocMakeToolbar( ed )		||
	  appDocMakeScrolledWindow( ed )	||
	  (*ea->eaMakeDocumentWidget)( ed )	)
	{ LDEB(1); return -1; }

    XtAddEventHandler( ed->edDocumentWidget.dwWidget,
					StructureNotifyMask, False,
					appDocConfigure, (void *)ed );
    XtAddEventHandler( ed->edToplevel.atTopWidget, StructureNotifyMask, False,
					appDocTopConfigure, (void *)ed );

    /*  4  */
    XtAddEventHandler( ed->edDocumentWidget.dwWidget,
		    PropertyChangeMask, True, appDocCopyPasteHandler, ed );

    XtManageChild( ed->edVerticalScrollbar );
    XtManageChild( ed->edHorizontalScrollbar );

    XtManageChild( ed->edDocumentWidget.dwWidget );
    XtManageChild( ed->edScrolledWindow );

    if  ( ed->edToolbar )
	{ XtManageChild( ed->edToolbar ); }

    XtManageChild( ed->edMainWindow );

    XtRealizeWidget( ed->edToplevel.atTopWidget );

    appSetNamedWindowIcon( ea, ed->edToplevel.atTopWidget, ea->eaMainIcon );

    return 0;
    }

int appFinishDocumentSetup(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    DocumentWidget *	dw= &(ed->edDocumentWidget);

    guiDrawSetRedrawHandler( dw->dwWidget,
					appDocExposeHandler, (void *)ed );

    if  ( ea->eaObserveFocus )
	{
	guiSetFocusChangeHandler( dw->dwWidget,
					    ea->eaObserveFocus, (void *)ed );
	}

    if  ( ea->eaDocumentMouseHandler )
	{
	guiDrawSetButtonPressHandler( dw->dwWidget,
				ea->eaDocumentMouseHandler, (void *)ed );
	}

    if  ( ea->eaDocumentScrollHandler )
	{
	guiDrawSetScrollHandler( dw->dwWidget,
				    ea->eaDocumentScrollHandler, (void *)ed );
	}

    dw->dwOwner= (void *)ed;
    dw->dwGotString= ea->eaDocGotString;
    dw->dwGotKey= ea->eaDocGotKey;

    if  ( ea->eaDocumentCursor )
	{
	XDefineCursor( XtDisplay( dw->dwWidget ),
				XtWindow( ed->edDocumentWidget.dwWidget ),
				ea->eaDocumentCursor );
	}

    guiDocumentSetInputContext( ea->eaInputMethod, dw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Management of the 'Windows' menu options.				*/
/*									*/
/************************************************************************/

void appSetWindowsItemState(	APP_WIDGET	menu,
				EditDocument *	ed,
				int		changed )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			j;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( j= childCount- 1; j >= 0; j-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[j],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)ed )
	    { guiSetToggleItemState( children[j], changed ); }
	}

    return;
    }

void appRemoveWindowsOption(	APP_WIDGET		menu,
				EditDocument *		oldEd )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			i;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( i= childCount- 1; i >= 0; i-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[i],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)oldEd )
	    { XtDestroyWidget( children[i] );	}
	}

    return;
    }

void appRenameWindowsOption(		APP_WIDGET		menu,
					EditDocument *		ed,
					const MemoryBuffer *	title )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			i;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( i= childCount- 1; i >= 0; i-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[i],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)ed )
	    {
	    guiSetToggleItemLabel( children[i],
					utilMemoryBufferGetString( title ) );
	    }
	}
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
    ami.amiCallback= (APP_MENU_CALLBACK_T)appDocToFront;

    item= guiSetToggleMenuItem( menu, &(ed->edToplevel), &ami, (void *)ed );

    XtVaSetValues( item,
		    XmNuserData,	(void *)ed,
		    NULL );
    }

void appDocSetVerticalScrollbarValues(
				EditDocument *		ed,
				int			minimum,
				int			maximum,
				int			value,
				int			sliderSize )
    {
    XtVaSetValues( ed->edVerticalScrollbar,
	XmNminimum,		minimum,
	XmNmaximum,		maximum,
	XmNvalue,		value,
	XmNsliderSize,		sliderSize,
	XmNpageIncrement,	( 9* sliderSize+ 9 )/10,
	XmNincrement,		( sliderSize+ SCROLL_BAR_STEP- 1 )/ SCROLL_BAR_STEP,
	NULL );

    return;
    }

void appDocSetHorizontalScrollbarValues(
				EditDocument *		ed,
				int			minimum,
				int			maximum,
				int			value,
				int			sliderSize )
    {
    XtVaSetValues( ed->edHorizontalScrollbar,
			XmNminimum,	minimum,
			XmNmaximum,	maximum,
			XmNvalue,	value,
			XmNsliderSize,	sliderSize,
			NULL );
    }

#   endif
