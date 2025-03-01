/************************************************************************/
/*									*/
/*  Document window application independent functionality.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appSystem.h>
#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocument.h"
#   include	"appQuestion.h"
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

#   define	LOG_REDRAWS	0

void appReportSaveFailure(	EditApplication *	ea,
				APP_WIDGET		option,
				APP_WIDGET		relative,
				const MemoryBuffer *	filename )
    {
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    if  ( fileTestFileWritable( filename ) )
	{
	appQuestionRunFilenameErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNotWritable );
	}
    else{
	appQuestionRunFilenameErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNoAccess );
	}
    }

/************************************************************************/
/*									*/
/*  Callback for the option in the files menu.				*/
/*									*/
/************************************************************************/

void appMakeDocVisible(	EditApplication *	ea,
			EditDocument *		ed )
    {
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    if  ( ! ed->edIsVisible )
	{
	ed->edIsVisible= 1;

	appDocVisible( ea, ed, ed->edIsVisible );
	}
    }

APP_MENU_CALLBACK_H( appDocToFront, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    appMakeDocVisible( ea, ed );

    guiSetToggleItemState( option, ed->edHasBeenChanged );

    return;
    }

int appSetDocumentFilename(	EditDocument *		ed,
				const MemoryBuffer *	filename )
    {
    if  ( filename )
	{
	if  ( utilCopyMemoryBuffer( &(ed->edFilename), filename ) )
	    { LDEB(1); return -1;	}
	}
    else{ utilEmptyMemoryBuffer( &(ed->edFilename) );	}

    return 0;
    }

int appFormatDocumentTitle(	MemoryBuffer *		windowTitle,
				MemoryBuffer *		iconTitle,
				EditApplication *	ea,
				const MemoryBuffer *	title )
    {
    int			rval= 0;

    MemoryBuffer	baseName;

    utilInitMemoryBuffer( &baseName );

    if  ( fileGetRelativeName( &baseName, title ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( windowTitle, ea->eaApplicationName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferAppendString( windowTitle, ": " ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryAppendBuffer( windowTitle, title ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( iconTitle, ea->eaApplicationName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferAppendString( iconTitle, ": " ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryAppendBuffer( iconTitle, &baseName ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &baseName );

    return rval;
    }

int appSetDocumentTitle(	EditDocument *		ed,
				const MemoryBuffer *	title )
    {
    int				rval= 0;
    EditApplication *		ea= ed->edApplication;

    MemoryBuffer		fullTitle;
    MemoryBuffer		iconName;

    utilInitMemoryBuffer( &fullTitle );
    utilInitMemoryBuffer( &iconName );

    if  ( utilCopyMemoryBuffer( &(ed->edTitle), title ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appFormatDocumentTitle( &fullTitle, &iconName, ea, &(ed->edTitle) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    guiSetShellTitle( ed->edToplevel.atTopWidget, &fullTitle );
    guiSetIconTitle( ed->edToplevel.atTopWidget, &iconName );

    appRenameDocumentOptions( ed->edApplication, ed, &(ed->edTitle) );

  ready:

    utilCleanMemoryBuffer( &fullTitle );
    utilCleanMemoryBuffer( &iconName );

    return rval;
    }


/************************************************************************/
/*									*/
/*  Free all resources allocated for a document.			*/
/*									*/
/************************************************************************/

static void appCleanDocumentWidget(	DocumentWidget *	dw )
    {
#   if USE_MOTIF
    if  ( dw->dwInputContext )
	{ XDestroyIC( dw->dwInputContext ); }
#   endif

#   if (USE_GTK) && defined(GTK_TYPE_IM_CONTEXT)
    if  ( dw->dwInputContext )
	{ g_object_unref( dw->dwInputContext ); }
#   endif
    }

static void appCleanDocument(		EditApplication *	ea,
					EditDocument *		ed )
    {
    if  ( ed->edLeftRuler )
	{ (*ea->eaFreeLeftRuler)( ed->edLeftRuler ); }
    if  ( ed->edTopRuler )
	{ (*ea->eaFreeTopRuler)( ed->edTopRuler ); }
    if  ( ed->edRightRuler )
	{ (*ea->eaFreeRightRuler)( ed->edRightRuler ); }
    if  ( ed->edBottomRuler )
	{ (*ea->eaFreeBottomRuler)( ed->edBottomRuler ); }

    utilCleanMemoryBuffer( &(ed->edFilename) );
    utilCleanMemoryBuffer( &(ed->edTitle) );

    appCleanDocumentWidget( &(ed->edDocumentWidget) );
    if  ( ed->edDrawingSurface )
	{ drawFreeDrawingSurface( ed->edDrawingSurface ); }

    if  ( ed->edInspector )
	{
XDEB(ed->edInspector);
	}

    return;
    }

static void appFreeDocument(		EditApplication *	ea,
					EditDocument *		ed )
    {
    appCleanDocument( ea, ed );
    free( ed );

    return;
    }

static void appInitDocumentWidget(	DocumentWidget *	dw )
    {
    dw->dwWidget= (APP_WIDGET)0;

#   if USE_MOTIF
    dw->dwInputContext= (XIC)0;
#   endif

#   if USE_GTK
#	ifdef GTK_TYPE_IM_CONTEXT
	dw->dwInputContext= (GtkIMContext *)0;
#	else
	dw->dwInputContext= 0;
#	endif
#   endif
    }

static void appInitEditDocument(	EditApplication *	ea,
					EditDocument *		ed )
    {
    ed->edApplication= ea;

    utilInitMemoryBuffer( &(ed->edFilename) );
    utilInitMemoryBuffer( &(ed->edTitle) );

    ed->edFormat= -1;
    ed->edFileReadOnly= 0;

    ed->edMenuBar= (APP_WIDGET)0;
    ed->edMainWindow= (APP_WIDGET)0;
	ed->edFileMenu= (APP_WIDGET)0;
	ed->edFileMenuButton= (APP_WIDGET)0;
	    ed->edFileCloseOption= (APP_WIDGET)0;
	ed->edEditMenu= (APP_WIDGET)0;
	ed->edEditMenuButton= (APP_WIDGET)0;
	ed->edWindowMenu= (APP_WIDGET)0;
	ed->edWindowMenuButton= (APP_WIDGET)0;
	ed->edHelpMenu= (APP_WIDGET)0;
	ed->edHelpMenuButton= (APP_WIDGET)0;

    ed->edToolbar= (APP_WIDGET)0;
    ed->edScrolledWindow= (APP_WIDGET)0;
	ed->edVerticalScrollbar= (APP_WIDGET)0;
	ed->edHorizontalScrollbar= (APP_WIDGET)0;
	ed->edWorkWidget= (APP_WIDGET)0;
	appInitDocumentWidget( &(ed->edDocumentWidget) );
	ed->edDrawingSurface= (DrawingSurface)0;

	ed->edLeftRulerWidget= (APP_WIDGET)0;
	ed->edTopRulerWidget= (APP_WIDGET)0;
	ed->edRightRulerWidget= (APP_WIDGET)0;
	ed->edBottomRulerWidget= (APP_WIDGET)0;

	ed->edLeftRuler= (void *)0;
	ed->edTopRuler= (void *)0;
	ed->edRightRuler= (void *)0;
	ed->edBottomRuler= (void *)0;

    ed->edLeftRulerWidePixels= 0;
    ed->edTopRulerHighPixels= 0;
    ed->edRightRulerWidePixels= 0;
    ed->edBottomRulerHighPixels= 0;

    ed->edHasBeenChanged= 0;
    ed->edIsReadonly= 0;
    ed->edIsVisible= 0;

    ed->edMapped= 0;
    ed->edNotYetDrawn= 1;
    ed->edInProgrammaticChange= 0;

    appInitSelectRectangle( &(ed->edSelectRectangle) );

    ed->edInspector= (struct AppInspector *)0;
    return;
    }

int appMakeDocumentWindow(		EditDocument **		pEd,
					EditApplication *	ea,
					int			readOnly,
					const MemoryBuffer *	title,
					const MemoryBuffer *	filename )
    {
    int				rval= 0;
    EditDocument *		ed= (EditDocument *)0;

    MemoryBuffer		scratch;

    utilInitMemoryBuffer( &scratch );

    if  ( ! title || utilMemoryBufferIsEmpty( title ) )
	{
	static int			count= 1;

	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	utilMemoryBufferPrintf( &scratch,
				    afmr->afmrNamelessTitleFormat, count++ );
	title= &scratch;
	}

    /*  1  */
    ed= (EditDocument *)malloc( sizeof(EditDocument) );
    if  ( ! ed )
	{ XDEB(ed); rval= -1; goto ready;	}

    appInitEditDocument( ea, ed );

    ea->eaNextDocumentId++; /* Never 0 */
    ed->edDocumentId= ea->eaNextDocumentId;

    if  ( filename && utilCopyMemoryBuffer( &(ed->edFilename), filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ed->edIsReadonly= readOnly;
    if  ( utilCopyMemoryBuffer( &(ed->edTitle), title ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ea->eaMakePrivateData	)
	{
	ed->edPrivateData= (*ea->eaMakePrivateData)( ea );
	if  ( ! ed->edPrivateData )
	    { XDEB(ed->edPrivateData); rval= -1; goto ready; }
	}

    if  ( appFinishDocumentWindow( ed ) )
	{ LDEB(1); rval= -1; goto ready; }

    *pEd= ed; ed= (EditDocument *)0; /* steal */

  ready:

    utilCleanMemoryBuffer( &scratch );

    if  ( ed )
	{ appFreeDocument( ea, ed ); }

    return rval;
    }

int appSetupDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    DocumentWidget *		dw= &(ed->edDocumentWidget);

    int				wide;
    int				high;

#   if USE_MOTIF
    XtRealizeWidget( ed->edToplevel.atTopWidget );
#   endif

#   if USE_GTK
    gtk_widget_realize( ed->edToplevel.atTopWidget );
#   endif

    ed->edDrawingSurface= guiDrawingSurfaceForNativeWidget(
					    ed->edDocumentWidget.dwWidget,
					    ea->eaAvoidFontconfigInt > 0 );
    if  ( ! ed->edDrawingSurface )
	{ PDEB(ed->edDrawingSurface); appFreeDocument( ea, ed ); return -1; }

    if  ( (*ea->eaLayoutDocument)( ea, &(ed->edFullRect),
					&(ed->edVisibleRect),
					ed->edPrivateData, ed->edFormat,
					ed->edDrawingSurface,
					&(ea->eaPostScriptFontList),
					&(ea->eaDefaultDocumentGeometry) ) )
	{ LDEB(1); return -1; }

    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0;
    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0;

    if  ( wide > ( 4* ea->eaScreenPixelsWide )/ 5 )
	{ wide=  ( 4* ea->eaScreenPixelsWide )/ 5; }
    if  ( high > ( 4* ea->eaScreenPixelsHigh )/ 5 )
	{ high=  ( 4* ea->eaScreenPixelsHigh )/ 5; }

#   if USE_MOTIF
    {
    Dimension docW= -1, docH= -1;
    Dimension topW= -1, topH= -1;

    XtVaSetValues( dw->dwWidget,
			    XmNborderWidth,		0,
			    XmNshadowThickness,		0,
			    XmNwidth,			wide,
			    XmNheight,			high,
			    NULL );

    /*
     * Cope with the fact that Motif performs the geometry calculations
     * when the widget is realised (And that we need to realize the widget 
     * before we perform the layout of the document.)
     */

    XtVaGetValues( dw->dwWidget,
			    XmNwidth,			&docW,
			    XmNheight,			&docH,
			    NULL );
    XtVaGetValues( ed->edToplevel.atTopWidget,
			    XmNwidth,			&topW,
			    XmNheight,			&topH,
			    NULL );

    if  ( docW != wide && docH != high && topW > docW && topH > docH )
	{
	XtVaSetValues( ed->edToplevel.atTopWidget,
			    XmNwidth,			wide+ ( topW- docW ),
			    XmNheight,			high+ ( topH- docH ),
			    NULL );
	}
    }
#   endif

#   if USE_GTK
    gtk_widget_set_size_request( ed->edTopRulerWidget,
					ed->edLeftRulerWidePixels+
					wide+
					ed->edRightRulerWidePixels,
					ed->edTopRulerHighPixels );
    gtk_widget_set_size_request( ed->edLeftRulerWidget,
					ed->edLeftRulerWidePixels, high );


    gtk_widget_set_size_request( dw->dwWidget, wide, high );

    gtk_widget_set_size_request( ed->edRightRulerWidget,
					    ed->edRightRulerWidePixels, high );
    gtk_widget_set_size_request( ed->edBottomRulerWidget,
					ed->edLeftRulerWidePixels+
					wide+
					ed->edRightRulerWidePixels,
					ed->edBottomRulerHighPixels );

    /* Unused!
    gtk_object_set_user_data( G_OBJECT( dw->dwWidget ), (void *)ed );
    */
#   endif

    if  ( ea->eaSetTopRuler		&&
	  (*ea->eaSetTopRuler)( ed )	)
	{ LDEB(1);	}

    if  ( ea->eaSetLeftRuler		&&
	  (*ea->eaSetLeftRuler)( ed )	)
	{ LDEB(1);	}

    if  ( ea->eaSetRightRuler		&&
	  (*ea->eaSetRightRuler)( ed )	)
	{ LDEB(1);	}

    if  ( ea->eaSetBottomRuler		&&
	  (*ea->eaSetBottomRuler)( ed )	)
	{ LDEB(1);	}

    guiGetBackgroundColor( &(ed->edBackgroundColor), dw->dwWidget );

    appDocSetScrollbarValues( ed );

    ed->edHasBeenChanged= 0;  /*  Can be changed in eaFinishDocumentSetup */

    if  ( ea->eaFinishDocumentSetup && (*ea->eaFinishDocumentSetup)( ed ) )
	{ LDEB(1); return -1; }

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    ed->edIsVisible= 1;

    appDocVisible( ea, ed, ed->edIsVisible );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a a document.							*/
/*									*/
/************************************************************************/

void appCloseDocument(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    appRemoveDocument( ea, ed );

    /* Is done by the widget destroy callback for GTK: */
#   if USE_MOTIF
    if  ( ed->edPrivateData )
	{
	(*ea->eaFreeDocument)( ed->edPrivateData, ed->edFormat );
	ed->edPrivateData= (void *)0;
	}
#   endif

    appCleanDocument( ea, ed );
    guiDestroyShellWidget( ed->edToplevel.atTopWidget );

    /* Is done by the widget destroy callback that is called asynchronuously */
#   if 0
    free( ed );
#   endif

    if  ( ea->eaOpenCount == 0 && ! ea->eaMainVisibleAsAbout  )
	{ appExitApplication( ea );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the menu of a document window.					*/
/*									*/
/************************************************************************/

void appDocFillMenu(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    if  ( ea->eaDocFileMenu )
	{
	ed->edFileMenu= appMakeMenu( &(ed->edFileMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    ea->eaDocFileMenu, (void *)ed );
	}

    if  ( ea->eaDocEditMenu )
	{
	ed->edEditMenu= appMakeMenu( &(ed->edEditMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    ea->eaDocEditMenu, (void *)ed );
	}

    if  ( ea->eaMakePrivateDocumentMenus )
	{ (*ea->eaMakePrivateDocumentMenus)( ea, ed, ed->edMenuBar ); }

    if  ( ea->eaDocWindowMenu )
	{
	ed->edWindowMenu= appMakeMenu( &(ed->edWindowMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    ea->eaDocWindowMenu, (void *)ed );
	}

    if  ( ea->eaDocHelpMenu )
	{
	ed->edHelpMenu= appMakeMenu( &(ed->edHelpMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    ea->eaDocHelpMenu, (void *)ed );
	}
    }

/************************************************************************/
/*									*/
/*  Cause a rectangle to be redrawn.					*/
/*									*/
/*  Cause the smallest rectangle that contains the selection to be	*/
/*  redrawn.								*/
/*									*/
/*  drChanged is a rectangle in the document (pixel) coordinate system.	*/
/*  it is shifted to accomodate for the scrollbars.			*/
/*									*/
/************************************************************************/

void appDocExposeRectangle(	const EditDocument *		ed,
				const DocumentRectangle *	drChanged,
				int				scrolledX,
				int				scrolledY )
    {
    DocumentRectangle		drExpose;
    DocumentRectangle		drScrolled;

    const DocumentRectangle *	drVisible= &(ed->edVisibleRect);

    if  ( ! drChanged )
	{ guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget ); return; }

    drScrolled= *drChanged;
    geoShiftRectangle( &drScrolled, -scrolledX, -scrolledY );

    if  ( scrolledX != 0 )
	{
	drScrolled.drY0= drVisible->drY0;
	drScrolled.drY1= drVisible->drY1;
	}

    if  ( scrolledY != 0 )
	{
	drScrolled.drX0= drVisible->drX0;
	drScrolled.drX1= drVisible->drX1;
	}

    geoUnionRectangle( &drScrolled, &drScrolled, drChanged );

    drExpose= *drVisible;

    if  ( geoIntersectRectangle( &drExpose, &drExpose, &drScrolled ) )
	{
	geoShiftRectangle( &drExpose, -drVisible->drX0, -drVisible->drY0 );

	if  ( drExpose.drX0 < 0 )
	    { drExpose.drX0=  0;	}
	if  ( drExpose.drY0 < 0 )
	    { drExpose.drY0=  0;	}

#	if LOG_REDRAWS
	geoLogRectangle( "CLEAR!", &drExpose );
#	endif

	if  ( drExpose.drX1 >= drExpose.drX0 && drExpose.drY1 >= drExpose.drY0 )
	    {
	    guiExposeDrawingWidgetRectangle( ed->edDocumentWidget.dwWidget,
								    &drExpose );
	    }
	}
    }

void appInitSelectRectangle(	SelectRectangle *	sr )
    {
    sr->srDirection= DOCselNONE;

    geoInitRectangle( &(sr->srSelected) );
    geoInitRectangle( &(sr->srLTM) );
    geoInitRectangle( &(sr->srRBM) );
    }

#   endif
