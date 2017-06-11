/************************************************************************/
/*									*/
/*  Front end calls: I.e: From the user interface.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appAppFront.h"
#   include	"appDocument.h"
#   include	<guiNamedImages.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Count changed documents						*/
/*									*/
/************************************************************************/

static int appCountChangedDocuments(	EditApplication *	ea )
    {
    int		i;
    int		changedCount= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    { changedCount++;	}
	}

    return changedCount;
    }

/************************************************************************/
/*									*/
/*  Quit the application.						*/
/*									*/
/*  1)  Close the documents in reverse order to avoid the subscript of	*/
/*	unhandeled documents to change.					*/
/*									*/
/************************************************************************/

void appExitApplication(	EditApplication *	ea )
    {
    int		i;

    for ( i= ea->eaOpenCount- 1; i >= 0; i-- )
	{ appCloseDocument( ea->eaOpenDocuments[i] );	}

    guiCleanNamedImages( ea->eaNamedImages, ea->eaNamedImageCount );

    guiDestroyShellWidget( ea->eaToplevel.atTopWidget );

    exit( 0 ); LDEB(1); return;
    }

static void appAskCloseDocuments(	APP_WIDGET		option,
					EditApplication *	ea )
    {
    int		i;

    /*  1  */
    for ( i= ea->eaOpenCount- 1; i >= 0; i-- )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    {
	    appMakeDocVisible( ea, ed );
	    appRunReallyCloseDialog( option, ed );
	    }
	}

    return;
    }

void appQuitApplication(	APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea )
    {
    int				rcc;
    int				changedCount= 0;

    changedCount= appCountChangedDocuments( ea );
    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    if  ( changedCount > 1 )
	{
	rcc= appRunReallyQuitDialog( option, relative, ea );

	switch( rcc )
	    {
	    case AQDrespCANCEL:
	    case AQDrespCLOSED:
		return;
	    case AQDrespNO:
		appExitApplication( ea );
		LDEB(1); return;

	    default:
		LDEB(rcc); return;
	    case AQDrespYES:
		break;
	    }
	}

    appAskCloseDocuments( option, ea );

    changedCount= appCountChangedDocuments( ea );
    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    return;
    }

APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appQuitApplication( option, ea->eaToplevel.atTopWidget, ea );
    }

/************************************************************************/
/*									*/
/*  Open a new document.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, e )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    const MemoryBuffer *	filename= (const MemoryBuffer *)0;

    if  ( appNewDocument( ea, filename ) )
	{ LDEB(1); }

    return;
    }

void appRenameDocumentOptions(	EditApplication *	ea,
				EditDocument *		ed,
				const MemoryBuffer *	title )
    {
    int		i;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRenameWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
								ed, title );
	}

    appRenameWindowsOption( ea->eaWinMenu, ed, title );

    return;
    }

/************************************************************************/
/*									*/
/*  Remember whether a document has been changed.			*/
/*									*/
/************************************************************************/

static void appRefreshWindowsItemState( EditApplication *	ea,
					EditDocument *		ed,
					int			changed	)
    {
    int		i;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appSetWindowsItemState( ea->eaOpenDocuments[i]->edWindowMenu,
							    ed, changed );
	}

    appSetWindowsItemState( ea->eaWinMenu, ed, changed );
    }

/************************************************************************/
/*									*/
/*  Make a document the current Document.				*/
/*									*/
/************************************************************************/

static void appHighlightCurrentDocument( EditApplication *	ea,
					EditDocument *		ed,
					int			current )
    {
    guiEnableWidget( ed->edMenuBar, current != 0 );
    return;
    }

void appSetCurrentDocument(	EditApplication *	ea,
				EditDocument *		ed	)
    {
    if  ( ea->eaCurrentDocument )
	{ appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 0 ); }

    ea->eaCurrentDocument= ed;

    appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 1 );
    }

/************************************************************************/
/*									*/
/*  Remember whether a document has been changed.			*/
/*									*/
/************************************************************************/

void appDocumentChanged(	EditDocument *		ed,
				int			changed	)
    {
    changed= changed != 0;

    if  ( ed->edHasBeenChanged != changed )
	{
	appRefreshWindowsItemState( ed->edApplication, ed, changed );
	ed->edHasBeenChanged= changed;
	}
    }

/************************************************************************/
/*									*/
/*  Count the number of visible documents.				*/
/*									*/
/************************************************************************/

void appDocVisible(	EditApplication *	ea,
			EditDocument *		ed,
			int			visible	)
    {
    int	from;

    if  ( visible )
	{ from= ea->eaVisibleDocumentCount++;	}
    else{ from= ea->eaVisibleDocumentCount--;	}

    if  ( ea->eaVisibleDocumentCount == 0 )
	{ appShowShellWidget( ea, ea->eaToplevel.atTopWidget ); }
    else{
	if  ( ! ea->eaMainVisibleAsAbout )
	    { guiHideShellWidget( ea->eaToplevel.atTopWidget );	}
	}

    if  ( ea->eaVisibleDocumentCountChanged )
	{
	(*ea->eaVisibleDocumentCountChanged)
				    ( ea, from, ea->eaVisibleDocumentCount );
	}
    }

/************************************************************************/
/*									*/
/*  Remove a reference to a document.					*/
/*									*/
/************************************************************************/

void appRemoveDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    int		i;
    int		count= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRemoveWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu, ed );
	ea->eaOpenDocuments[count++]= ea->eaOpenDocuments[i];
	}

    appRemoveWindowsOption( ea->eaWinMenu, ed );

    ea->eaOpenCount= count;

    if  ( ea->eaCurrentDocument == ed )
	{ ea->eaCurrentDocument= (EditDocument *)0;	}

    if  ( ed->edIsVisible )
	{
	ed->edIsVisible= 0;
	appDocVisible( ea, ed, ed->edIsVisible );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Remember a reference to a document.					*/
/*									*/
/************************************************************************/

void appSetDocument(	EditApplication *	ea,
			EditDocument *		newEd )
    {
    EditDocument **	fresh;
    int			i;

    fresh= (EditDocument **)realloc( ea->eaOpenDocuments,
			( ea->eaOpenCount+ 1 )* sizeof(EditDocument *) );
    if  ( ! fresh )
	{ XDEB(fresh); return;	}

    fresh[ea->eaOpenCount]= newEd;
    ea->eaOpenDocuments= fresh;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	appAppSetWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
					    newEd, &(newEd->edTitle) );

	appAppSetWindowsOption( newEd->edWindowMenu,
					ea->eaOpenDocuments[i],
					&(ea->eaOpenDocuments[i]->edTitle) );
	}

    appAppSetWindowsOption( ea->eaWinMenu, newEd, &(newEd->edTitle) );

    ea->eaOpenCount++;

    return;
    }

/************************************************************************/

static int appMakeNamedImageInColumn(	EditApplication *	ea,
					APP_WIDGET *		pImage,
					APP_WIDGET		column,
					const char *		imageName )
    {
    struct NamedImage *	ni;

    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
							    imageName, &ni ) )
	{ SDEB(imageName); return -1;	}

    guiMakeImageInColumn( pImage, column, ni );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the application main widget.					*/
/*									*/
/************************************************************************/

int appFinishApplicationWindow(	EditApplication *	ea )
    {
    APP_WIDGET			pictureLabel= (APP_WIDGET)0;
    APP_WIDGET			label;

    guiInsertColumnInWindow( &(ea->eaMainWindow),
					    ea->eaToplevel.atTopWidget );

    guiInsertMenubarInColumn( &(ea->eaMenuBar), ea->eaMainWindow );

    if  ( ea->eaAppFileMenu )
	{
	ea->eaFileMenu= appMakeMenu( &(ea->eaFileMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			ea->eaAppFileMenu, (void *)ea );
	}

    if  ( ea->eaAppWinMenu )
	{
	ea->eaWinMenu= appMakeMenu( &(ea->eaWinMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			ea->eaAppWinMenu, (void *)ea );
	}

    if  ( ea->eaMakePrivateApplicationMenus )
	{ (*ea->eaMakePrivateApplicationMenus)( ea, ea->eaMenuBar ); }

    if  ( ea->eaAppHelpMenu )
	{
	ea->eaHelpMenu= appMakeMenu( &(ea->eaHelpMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			ea->eaAppHelpMenu, (void *)ea );
	}

#	if USE_MOTIF
	/*  Work around BUG in LessTif	*/
	{
	Dimension		high;

	XtVaGetValues( ea->eaMenuBar,
				XmNheight,	&high,
				NULL );
	if  ( high < 8 )
	    {
	    XtWidgetGeometry	preferred;

	    XtQueryGeometry( ea->eaMenuBar, (XtWidgetGeometry *)0, &preferred );

	    high= preferred.height+ preferred.border_width;
	    XtVaSetValues( ea->eaMenuBar,
				XmNheight,	high,
				NULL );
	    }
	}
#	endif

    if  ( ea->eaMainPicture					&&
	  appMakeNamedImageInColumn( ea, &pictureLabel,
			  ea->eaMainWindow, ea->eaMainPicture )	)
	{ SDEB(ea->eaMainPicture);	}

    if  ( ea->eaNameAndVersion )
	{
	char *		ident= (char *)0;


	if  ( 0 && ea->eaPlatformCompiled )
	    {
	    ident= malloc( strlen( ea->eaNameAndVersion )+ 3+
					strlen( ea->eaPlatformCompiled )+ 1 );
	    if  ( ! ident )
		{ XDEB(ident); }
	    else{
		sprintf( ident, "%s - %s",
				ea->eaNameAndVersion, ea->eaPlatformCompiled );
		}
	    }
	else{
	    ident= strdup( ea->eaNameAndVersion );
	    }

	if  ( ! ident )
	    { XDEB(ident);	}
	else{
	    guiMakeLabelInColumn( &label, ea->eaMainWindow, ident );

	    free( ident );
	    }
	}

    if  ( ea->eaReference )
	{
	guiMakeLabelInColumn( &label, ea->eaMainWindow,
						    ea->eaReference );
	}

    if  ( ea->eaMainIcon )
	{
	appSetNamedWindowIcon( ea, ea->eaToplevel.atTopWidget,
							    ea->eaMainIcon );
	}

    return 0;
    }

#   endif
