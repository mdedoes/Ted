/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/*  GTK specific code. This code is based on original code by Andrea	*/
/*  Frome.								*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_GTK

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appSystem.h>

#   include	"appEditApplication.h"
#   include	"appAppFront.h"
#   include	"appDocFront.h"
#   include	"appEditDocument.h"
#   include	"appQuestion.h"
#   include	"appFileChooser.h"
#   include	<guiWidgetsGtk.h>

#   include	<utilFileExtension.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get/Set the file name.						*/
/*									*/
/************************************************************************/

int appChooserImplGetFilename(	const AppChooserInformation *	aci,
				MemoryBuffer *			filename )
    {
    GtkFileChooser *	gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    const gchar *	fnm;

    fnm= gtk_file_chooser_get_filename( gfs );
    if  ( fnm )
	{
	if  ( utilMemoryBufferSetString( filename, fnm ) )
	    { LDEB(1); return -1;	}

	g_free( (gpointer)fnm );
	}
    else{
	utilEmptyMemoryBuffer( filename );
	}

    return 0;
    }

int appChooserImplSetFilename(	AppChooserInformation *	aci,
				const MemoryBuffer *	filename )
    {
    GtkFileChooser *	gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    gtk_file_chooser_set_filename( gfs,
		    (const gchar *)utilMemoryBufferGetString( filename ) );
    gtk_file_chooser_set_current_name( gfs,
		    (const gchar *)utilMemoryBufferGetString( filename ) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cleanup when a chooser is destroyed.				*/
/*									*/
/************************************************************************/

static void appChooserDestroyed(APP_WIDGET	fileChooser,
				GdkEvent *	event,
				void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    utilCleanMemoryBuffer( &(aci->aciFilename) );

    free( aci );
    }

static void appInitAci(		AppChooserInformation *	aci )
    {
    guiInitOptionmenu( &(aci->aciFilterOptionmenu) );

    aci->aciApplication= (EditApplication *)0;
    aci->aciDocument= (EditDocument *)0;
    utilInitMemoryBuffer( &(aci->aciFilename) );
    aci->aciThrough= (void *)0;
    aci->aciOpenDocument= (APP_OPEN_DOCUMENT)0;
    aci->aciExtensions= (struct AppFileExtension *)0;
    aci->aciExtensionCount= 0;
    aci->aciOption= (APP_WIDGET)0;
    aci->aciRelativeTo= (APP_WIDGET)0;
    aci->aciFormat= -1;

    aci->aciResources= (const AppFileChooserResources *)0;
    }

static void appChooserSetDirectory(	const MemoryBuffer *	filename,
					GtkFileChooser *	gfs )
    {
    MemoryBuffer	dir;

    utilInitMemoryBuffer( &dir );

    if  ( fileDirectoryOfFileName( &dir, filename ) )
	{ LDEB(1); return;	}

    if  ( ! utilMemoryBufferIsEmpty( &dir ) )
	{
	gtk_file_chooser_set_current_folder( gfs,
					utilMemoryBufferGetString( &dir ) );
	}

    utilCleanMemoryBuffer( &dir );

    return;
    }

static GtkFileFilter * appFileChooserImplBuildFilter(
				const AppFileExtension *	afe )
    {
    GtkFileFilter *	newFilter= (GtkFileFilter *)0;

    newFilter= gtk_file_filter_new();
    gtk_file_filter_add_pattern( newFilter, afe->afeFilter );
    gtk_file_filter_set_name( newFilter, afe->afeDescription );

    return newFilter;
    }

static void appFileChooserImplChangeFilter(	AppChooserInformation *	aci,
						int			format )
    {
    GtkFileChooser *		gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    const AppFileExtension *	afe= aci->aciExtensions+ format;
    GtkFileFilter *		newFilter= appFileChooserImplBuildFilter( afe );

    gtk_file_chooser_set_filter( gfs, newFilter );
    }

/************************************************************************/
/*									*/
/*  Make a chooser.							*/
/*									*/
/************************************************************************/

static int appMakeFileChooser(	AppChooserInformation **	pAci,
				GtkFileChooserAction		action,
				const gchar *			actionText,
				APP_WIDGET			relative,
				EditApplication *		ea,
				APP_WIDGET			option )
    {
    AppChooserInformation *	aci;
    const char *		title= "<@v@>";

    while( relative )
	{
	if  ( GTK_IS_WINDOW( relative ) )
	    { break;	}

	relative= gtk_widget_get_parent( relative );
	}

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    if  ( option )
	{
	/* needs not to be freed for GTK */
	title= guiGetTextFromMenuOption( option );
	}

    aci->aciWidget= gtk_file_chooser_dialog_new( title,
					GTK_WINDOW( relative ),
					action,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					actionText, GTK_RESPONSE_ACCEPT,
					NULL);

    g_signal_connect( G_OBJECT(  aci->aciWidget ), "destroy_event",
			(GCallback)( appChooserDestroyed ), (void *)aci );

    aci->aciApplication= ea;

    if  ( option )
	{
	guiSetDialogTitle( aci->aciWidget, option, ea->eaApplicationName );
	}

    *pAci= aci;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Run some kind of a file chooser.					*/
/*									*/
/************************************************************************/

static int appRunFileChooser(	EditApplication *		ea,
				int				acceptResponse,
				AppChooserInformation *		aci,
				APP_WIDGET			relative )
    {
    int		rval= ACIrespNONE;

#   ifdef GTK_FILE_CHOOSER

    int response= gtk_dialog_run( GTK_DIALOG( aci->aciWidget ) );
    switch( response )
	{
	case GTK_RESPONSE_ACCEPT:
	case GTK_RESPONSE_OK:
	    rval= acceptResponse;
	    break;

	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_DELETE_EVENT:
	    rval= ACIrespCANCEL;
	    break;

	default:
	    LDEB(response);
	    rval= ACIrespFAILURE;
	    break;
	}
#   else
    utilEmptyMemoryBuffer( &(aci->aciFilename) );

    appGuiShowDialog( ea, &(aci->aciDialog), relative );

    appGuiRunDialog( &(aci->aciDialog), ACIrespNONE, ea );

    appGuiHideDialog( &(aci->aciDialog) );
#   endif

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

static void appFileChooserOpenFile(	AppChooserInformation *	aci,
					const MemoryBuffer *	filename,
					APP_OPEN_DOCUMENT	openDocument,
					void *			through )
    {
    int ret= appFileChooserTestNameForOpen( aci, filename );
    if  ( ret )
	{ LDEB(1);	}

    if  ( ! ret && (*openDocument)( aci->aciApplication,
					through, aci->aciRelativeTo,
					aci->aciOption, filename ) )
	{ LDEB(1);	}

    return;
    }

static void appFileChooserOpenFiles(	AppChooserInformation *	aci,
					GtkFileChooser *	gfs,
					APP_OPEN_DOCUMENT	openDocument,
					void *			through )
    {
    GSList *	filenames;
    GSList *	fn;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    fn= filenames= gtk_file_chooser_get_filenames( gfs );
    if  ( filenames )
	{
	while( fn )
	    {
	    gchar *		fnm= (gchar *)(fn->data);

	    if  ( utilMemoryBufferSetString( &filename, fnm ) )
		{ LDEB(1); goto ready;	}

	    appFileChooserOpenFile( aci, &filename, openDocument, through );

	    g_free( fn->data );
	    fn= fn->next;
	    }

	g_slist_free( filenames );
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return;
    }

void appRunOpenChooser( APP_WIDGET			option,
			APP_WIDGET			relative,
			int				extensionCount,
			AppFileExtension *		extensions,
			const char *			defaultFilter,
			const MemoryBuffer *		dir,
			APP_OPEN_DOCUMENT		openDocument,
			EditApplication *		ea,
			void *				through )
    {
    int				response;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

    int				withFilter= extensionCount > 0;

    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_OPEN,
				    GTK_STOCK_OPEN, relative, ea, option ) )
	{ LDEB(1); return;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    gtk_file_chooser_set_select_multiple( gfs, TRUE );

    if  ( withFilter )
	{
	AppFileExtension *	afe;
	int			i;

	GtkFileFilter *		defaultFileFilter= (GtkFileFilter *)0;

	if  ( appFileFilterGetDescriptions( ea, extensions, extensionCount ) )
	    { LDEB(extensionCount);	}

	afe= extensions;
	for ( i= 0; i < extensionCount; afe++, i++ )
	    {
	    GtkFileFilter *	fileFilter;

	    if  ( ! FILE_CHOOSER_CAN_OPEN( afe ) )
		{ continue;	}

	    fileFilter= appFileChooserImplBuildFilter( afe );

	    gtk_file_chooser_add_filter( gfs, fileFilter );

	    if  ( ! defaultFileFilter )
		{ defaultFileFilter= fileFilter;	}
	    if  ( defaultFilter && ! strcmp( afe->afeFilter, defaultFilter ) )
		{ defaultFileFilter= fileFilter;	}
	    }

	if  ( defaultFileFilter )
	    { gtk_file_chooser_set_filter( gfs, defaultFileFilter );	}
	}

    if  ( dir && ! utilMemoryBufferIsEmpty( dir ) )
	{
	gtk_file_chooser_set_current_folder( gfs,
					utilMemoryBufferGetString( dir ) );
	}

    aci->aciApplication= ea;
    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciThrough= through;
    aci->aciOpenDocument= openDocument;

    response= appRunFileChooser( ea, ACIrespOPEN, aci, relative );
    switch( response )
	{
	case ACIrespOPEN:
	    appFileChooserOpenFiles( aci, gfs, openDocument, through );
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

    return;
    }

APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appRunOpenChooser( GTK_WIDGET( option ), ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appRunOpenChooser( GTK_WIDGET( option ), ed->edToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

static void appFileFormatChosen(	int		formatChosen,
					void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    MemoryBuffer		fileSelected;

    utilInitMemoryBuffer( &fileSelected );

    appChooserImplGetFilename( aci, &fileSelected );

    appFileChooserImplChangeFilter( aci, formatChosen );
    appChooserChangeFilenameExtension( aci, &fileSelected, formatChosen );

    appChooserImplSetFilename( aci, &fileSelected );

    aci->aciFormat= formatChosen;

    utilCleanMemoryBuffer( &fileSelected );

    return;
    }

/************************************************************************/
/*									*/
/*  Build the option menu with filters.					*/
/*									*/
/************************************************************************/

static void appFillSaveFilter(	AppChooserInformation *	aci,
				unsigned int		useFlags,
				EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    AppOptionmenu *		aom= &(aci->aciFilterOptionmenu);

    AppFileExtension *		afe;
    int				i;

    int				defaultFilterIndex= -1;

    if  ( appFileFilterGetDescriptions( ea,
			ea->eaFileExtensions, ea->eaFileExtensionCount ) )
	{ LDEB(ea->eaFileExtensionCount);	}

    aci->aciExtensions= ea->eaFileExtensions;
    aci->aciExtensionCount= ea->eaFileExtensionCount;

    afe= ea->eaFileExtensions;
    for ( i= 0; i < ea->eaFileExtensionCount; afe++, i++ )
	{
	guiAddItemToOptionmenu( aom, afe->afeDescription );

	if  ( appDocumentTestCanSave( ea, afe, ed->edPrivateData,
							    useFlags, i ) )
	    {
	    const int	visible= 0;

	    guiOptionMenuEnablePosition( aom, i, visible );
	    continue;
	    }

	if  ( defaultFilterIndex < 0 || i == ed->edFormat )
	    { defaultFilterIndex= i;	}
	}

    if  ( defaultFilterIndex >= 0 )
	{
	appFileChooserImplChangeFilter( aci, defaultFilterIndex );
	guiSetOptionmenu( aom, defaultFilterIndex );
	aci->aciFormat= defaultFilterIndex;
	}

    return;
    }

int appRunSaveChooser(	APP_WIDGET		option,
			APP_WIDGET		relative,
			APP_SAVE_DOCUMENT	saveDocument,
			EditDocument *		ed,
			void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    int				response= ACIrespNONE;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

    MemoryBuffer		filename;

    utilInitMemoryBuffer( &filename );

    {
    int				withFilter= ea->eaFileExtensionCount > 0;

    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_SAVE,
				    GTK_STOCK_SAVE, relative, ea, option ) )
	{ response= ACIrespFAILURE; goto ready;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    if  ( withFilter )
	{
	AppOptionmenu *	aom= &(aci->aciFilterOptionmenu);

	guiMakeOptionmenuGtk( &(aci->aciFilterOptionmenu),
					appFileFormatChosen, (void *)aci );

	gtk_file_chooser_set_extra_widget( gfs, aom->aomInplace );

	appFillSaveFilter( aci, APPFILE_CAN_SAVE, ed );
	}

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	appChooserImplSetFilename( aci, &(ed->edFilename) );
	appChooserSetDirectory( &(ed->edFilename), gfs );
	}

    gtk_file_chooser_set_do_overwrite_confirmation( gfs, TRUE );
    }

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciThrough= through;
    aci->aciSaveDocument= saveDocument;

    response= appRunFileChooser( ea, ACIrespSAVE, aci, relative );

    if  ( appChooserImplGetFilename( aci, &filename ) )
	{ LDEB(1); response= ACIrespFAILURE;	}

    switch( response )
	{
	case ACIrespSAVE:
	    appSaveChooserSave( aci, relative, ed, &filename );
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

  ready:

    utilCleanMemoryBuffer( &filename );

    return response;
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for print to file.					*/
/*									*/
/************************************************************************/

int appRunPrintToFileChooser(	APP_WIDGET		option,
				APP_WIDGET		relative,
				APP_SAVE_DOCUMENT	printDocument,
				EditDocument *		ed,
				void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    int				response= ACIrespNONE;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

    MemoryBuffer		filename;

    utilInitMemoryBuffer( &filename );

    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_SAVE,
				    GTK_STOCK_SAVE, relative, ea, option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciApplication= ed->edApplication;
    aci->aciThrough= through;
    aci->aciSaveDocument= printDocument;

    aci->aciExtensions= ea->eaFileExtensions;
    aci->aciExtensionCount= ea->eaFileExtensionCount;
    aci->aciFormat= ed->edFormat;

    {
    GtkFileFilter *		fileFilter= (GtkFileFilter *)0;

    fileFilter= gtk_file_filter_new();
    gtk_file_filter_add_pattern( fileFilter, "*.ps" );
    gtk_file_filter_set_name( fileFilter, "PostScript" );
    gtk_file_chooser_set_filter( gfs, fileFilter );
    }

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	if  ( appPrintChooserFileName( aci, &filename,
					aci->aciDocument, &(ed->edFilename) ) )
	    { LDEB(1); goto ready;	}

	appChooserImplSetFilename( aci, &filename );
	appChooserSetDirectory( &filename, gfs );
	}

    gtk_file_chooser_set_do_overwrite_confirmation( gfs, TRUE );

    response= appRunFileChooser( ea, ACIrespSAVE, aci, relative );

    switch( response )
	{
	case ACIrespSAVE:
	    if  ( appChooserImplGetFilename( aci, &filename ) )
		{ LDEB(1); response= ACIrespFAILURE;	}
	    else{
		appSaveChooserPrint( aci, &filename );
		}
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

  ready:

    utilCleanMemoryBuffer( &filename );

    return response;
    }

int appFileChooserConfirmOverWrite(	const AppChooserInformation *	aci,
					const char *		filename )
    {
    GtkFileChooser *	gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    int			rcc;

    /*  Do not ask if the GTK chooser already did */
    if  ( gtk_file_chooser_get_do_overwrite_confirmation( gfs ) )
	{ return ACIrespSAVE; }

    rcc= appQuestionRunSubjectYesNoCancelDialog(
			aci->aciApplication, aci->aciRelativeTo, aci->aciOption,
			filename, aci->aciResources->acrOverwriteMessage );

    switch( rcc )
	{
	case AQDrespYES:
	    return ACIrespSAVE;
	case AQDrespNO:
	    return ACIrespNONE;
	default:
	    LDEB(rcc);
	    /*FALLTHROUGH*/
	case AQDrespCANCEL:
	case AQDrespCLOSED:
	    return ACIrespCANCEL;
	}
    }

#   endif
