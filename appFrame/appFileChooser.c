/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appSystem.h>
#   include	<sioFileio.h>

#   include	"appEditApplication.h"
#   include	"appPrintDocument.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocFront.h"
#   include	"appPrintJob.h"

#   include	"appFileChooser.h"
#   include	"appQuestion.h"
#   include	"appGuiResource.h"
#   include	<utilFileExtension.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Error messages.							*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_ChooserResourceTable[]=
    {
	APP_RESOURCE( "chooserNoFilename",
		    offsetof(AppFileChooserResources,acrNoFilenameMessage),
		    "Please Give a Name" ),
	APP_RESOURCE( "chooserIsDirectory",
		    offsetof(AppFileChooserResources,acrIsDirecoryMessage),
		    "Is a directory" ),
	APP_RESOURCE( "chooserNotWritable",
		    offsetof(AppFileChooserResources,acrNotWritableMessage),
		    "You have no permission to write this file." ),
	APP_RESOURCE( "chooserNotReadable",
		    offsetof(AppFileChooserResources,acrNotReadableMessage),
		    "You have no permission to read this file." ),
	APP_RESOURCE( "chooserOverwrite",
		    offsetof(AppFileChooserResources,acrOverwriteMessage),
		    "Do you want to overwrite this file?" ),
	APP_RESOURCE( "chooserNoSuchDir",
		    offsetof(AppFileChooserResources,acrNoSuchDirMessage),
		    "This directory does not exist." ),

	APP_RESOURCE( "chooserPsFiles",
		    offsetof(AppFileChooserResources,acrPostScriptFiles),
		    "PostScript ( *.ps )" ),
    };

/************************************************************************/
/*									*/
/*  Retrieve the configurable texts of a file chooser.			*/
/*									*/
/************************************************************************/

void appFileChooserGetTexts(	EditApplication *		ea,
				AppChooserInformation *		aci )
    {
    static int				gotResources= 0;
    static AppFileChooserResources	APP_ChooserResourceValues;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea,
				    (void *)&APP_ChooserResourceValues,
				    APP_ChooserResourceTable,
				    sizeof(APP_ChooserResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    aci->aciResources= &APP_ChooserResourceValues;

    return;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name of a file that is to be opened.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForOpen(
				const AppChooserInformation *	aci,
				const MemoryBuffer *		filename )
    {
    EditApplication *		ea= aci->aciApplication;
    APP_WIDGET			option= aci->aciOption;

    int				fileExists= 0;
    int				fileReadable= 0;
    int				isDir= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    fileExists= fileTestFileExists( filename ) == 0;

    if  ( fileExists )
	{ fileReadable= fileTestFileReadable( filename ) == 0; }
    else{ isDir= fileTestDirectory( filename ) == 0;		}

    if  ( isDir )
	{
	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, filename, acr->acrIsDirecoryMessage );
	return -1;
	}

    if  ( ! fileExists )
	{
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, filename, afmr->afmrNoSuchFileMessage );

	return -1;
	}

    if  ( ! fileReadable )
	{
	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, filename, acr->acrNotReadableMessage );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Partially validate the file name of a file chooser: if if has the	*/
/*  wrong extension.. set the rigth one.				*/
/*									*/
/*  Return:  0:  Have saved an acceptable file name.			*/
/*  Return:  1:  Not an acceptable file name.				*/
/*  Return: -1:  Failure.						*/
/*									*/
/************************************************************************/

int appChooserTestFilename(	const AppChooserInformation *	aci,
				const MemoryBuffer *		filename )
    {
    int					rval= 0;
    struct EditApplication *		ea= aci->aciApplication;
    APP_WIDGET				option= aci->aciOption;

    const AppFileChooserResources *	acr= aci->aciResources;

    MemoryBuffer			relative;

    utilInitMemoryBuffer( &relative );

    if  ( fileGetRelativeName( &relative, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &relative ) )
	{
	appQuestionRunErrorDialog( ea, aci->aciRelativeTo, option,
						acr->acrNoFilenameMessage );

	rval= 1; goto ready;
	}

  ready:

    utilCleanMemoryBuffer( &relative );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name for a file to save a document to.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForWrite(
				const AppChooserInformation *	aci,
				const MemoryBuffer *		fileName )
    {
    int				isDir= 0;
    int				fileExists= 0;
    int				dirExists= 0;
    int				fileWritable= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    const char *	filename= utilMemoryBufferGetString( fileName );

    fileExists= fileTestFileExists( fileName ) == 0;

    if  ( fileExists )
	{ fileWritable= fileTestFileWritable( fileName ) == 0; }
    else{ isDir= fileTestDirectory( fileName ) == 0;		}

    if  ( ! fileExists && ! isDir )
	{
	MemoryBuffer	dir;

	utilInitMemoryBuffer( &dir );

	if  ( fileDirectoryOfFileName( &dir, fileName ) )
	    { LDEB(1); return ACIrespFAILURE;	}

	if  ( ! utilMemoryBufferIsEmpty( &dir ) )
	    { dirExists= fileTestDirectory( &dir ) == 0;	}

	utilCleanMemoryBuffer( &dir );
	}

    if  ( isDir )
	{
	appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrIsDirecoryMessage );
	return ACIrespNONE;
	}

    if  ( fileExists )
	{
	if  ( ! fileWritable )
	    {
	    appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrNotWritableMessage );
	    return ACIrespNONE;
	    }
	else{
	    return appFileChooserConfirmOverWrite( aci, filename );
	    }
	}
    else{
	if  ( ! dirExists )
	    {
	    char *	slash= strrchr( filename+ 1, '/' );

	    if  ( slash )
		{
		*slash= '\0';

		appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrNoSuchDirMessage );
		return ACIrespNONE;
		}
	    }
	}

    return ACIrespSAVE;
    }

static int appDocPrintDocument(	EditDocument *		ed,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			format,
				const MemoryBuffer *	filename )
    {
    SimpleOutputStream *	sos;

    PrintJob *			pj= (PrintJob *)through;

    sos= sioOutFileioOpen( filename );

    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    appCallPrintFunction( sos, pj );

    sioOutClose( sos );

    return 0;
    }

void appDocPrintToFile(	APP_WIDGET			option,
			APP_WIDGET			panel,
			EditDocument *			ed,
			const struct PrintGeometry *	pg )
    {
    EditApplication *		ea= ed->edApplication;
    PrintJob			pj;

    if  ( ! ea->eaPrintDocument )
	{ XDEB(ea->eaPrintDocument); return;	}

    appPrintJobForEditDocument( &pj, ed, pg );

    appRunPrintToFileChooser( option, panel, appDocPrintDocument, ed, &pj );
    }

/************************************************************************/
/*									*/
/*  Callbacks from the file menu: Save a document and remember name and	*/
/*  type=format of the output file.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    if  ( ! ea->eaSaveDocument )
	{ XDEB(ea->eaSaveDocument); return;	}

    appRunSaveChooser( option, ed->edToplevel.atTopWidget,
			    appDocSaveDocument, ed, ed->edPrivateData );
    }

/************************************************************************/
/*									*/
/*  Callback from the file-open dialog.					*/
/*									*/
/************************************************************************/

int appChooserOpenDocument(	EditApplication *	ea,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const MemoryBuffer *	filename )
    {
    const int	readOnly= 0;

    if  ( ! appOpenDocument( ea, relative, option, readOnly, filename ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the extension of the file name because a different format	*/
/*  was chosen.								*/
/*									*/
/*  Work with a copy because of Motifs peculiarities.			*/
/*									*/
/************************************************************************/

int appChooserChangeFilenameExtension(
				const AppChooserInformation *	aci,
				MemoryBuffer *			filename,
				int				newFormat )
    {
    int				rval= 1;
    const AppFileExtension *	afe= aci->aciExtensions;

    MemoryBuffer		oldExtension;

    int				oldFormat= aci->aciFormat;

    utilInitMemoryBuffer( &oldExtension );

    if  ( ! utilMemoryBufferIsEmpty( filename ) )
	{
	if  ( fileGetFileExtension( &oldExtension, filename ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ! utilMemoryBufferIsEmpty( &oldExtension )		&&
	      oldFormat >= 0						&&
	      afe[oldFormat].afeExtension				&&
	      afe[newFormat].afeExtension				&&
	      utilMemoryBufferEqualsString( &oldExtension,
					  afe[oldFormat].afeExtension )	)
	    {
	    const char *	newExtension= afe[newFormat].afeExtension;

	    if  ( fileChangeExtension( filename, newExtension ) )
		{ LDEB(1); goto ready;	}
	    }
	}

  ready:

    utilCleanMemoryBuffer( &oldExtension );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Determine a name that is suitable to print the document.		*/
/*									*/
/************************************************************************/

int appPrintChooserFileName(	AppChooserInformation *	aci,
				MemoryBuffer *		changed,
				EditDocument *		ed,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    EditApplication *	ea= ed->edApplication;

    int			format= -1;
    int			suggestStdout= 0;
    int			fileExists;

    const char * const	ps= "ps";

    MemoryBuffer	ext;

    utilInitMemoryBuffer( &ext );

    if  ( utilCopyMemoryBuffer( changed, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    format= appDocumentGetSaveFormat( &suggestStdout, ea, changed,
				ed->edPrivateData,
				APPFILE_CAN_SAVE, aci->aciFormat );

    fileExists= ! fileTestFileExists( changed );

    if  ( format == aci->aciFormat )
	{
	if  ( fileChangeExtension( changed, ps ) )
	    { LDEB(aci->aciFormat); rval= -1; goto ready;	}
	}
    else{
	if  ( ! fileExists )
	    {
	    if  ( fileGetFileExtension( &ext, changed ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( ! utilMemoryBufferEqualsString( &ext, ps ) )
		{
		if  ( fileAddExtension( changed, ps ) )
		    { LDEB(aci->aciFormat); rval= -1; goto ready;	}
		}
	    }
	}

  ready:

    utilCleanMemoryBuffer( &ext );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Print the document.							*/
/*									*/
/************************************************************************/

int appSaveChooserPrint(	AppChooserInformation *	aci,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    const int		format= -1;

    int			resp;

    MemoryBuffer	changed;

    utilInitMemoryBuffer( &changed );

    if  ( appPrintChooserFileName( aci, &changed,
					    aci->aciDocument, filename ) )
	{ LDEB(1); rval= ACIrespFAILURE; goto ready;	}

    resp= appFileChooserTestNameForWrite( aci, &changed );

    if  ( resp == ACIrespSAVE )
	{
	if  ( (*aci->aciSaveDocument)( aci->aciDocument, aci->aciThrough,
				    aci->aciRelativeTo, aci->aciOption,
				    format, &changed ) )
	    { LDEB(format); rval= ACIrespFAILURE; goto ready;	}
	}

    rval= resp;

  ready:

    utilCleanMemoryBuffer( &changed );

    return rval;
    }


/************************************************************************/
/*									*/
/*  Save the document.							*/
/*									*/
/*  Try to add an extension to the name. [But only do so if the file	*/
/*  does not exist.							*/
/*									*/
/************************************************************************/

int appSaveChooserSave(		AppChooserInformation *	aci,
				APP_WIDGET		relative,
				EditDocument *		ed,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    EditApplication *	ea= ed->edApplication;

    int			format= -1;
    int			resp;
    int			suggestStdout= 0;
    int			fileExists;

    MemoryBuffer	changed;
    MemoryBuffer	ext;

    utilInitMemoryBuffer( &changed );
    utilInitMemoryBuffer( &ext );

    if  ( utilCopyMemoryBuffer( &changed, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    format= appDocumentGetSaveFormat( &suggestStdout, ea, &changed,
				ed->edPrivateData,
				APPFILE_CAN_SAVE, aci->aciFormat );

    fileExists= ! fileTestFileExists( &changed );

    if  ( format >= 0 && format < ea->eaFileExtensionCount )
	{
	if  ( fileGetFileExtension( &ext, &changed ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ! fileExists && utilMemoryBufferIsEmpty( &ext ) )
	    {
	    if  ( fileAddExtension( &changed,
			ea->eaFileExtensions[format].afeExtension ) )
		{ LDEB(format); rval= -1; goto ready;	}
	    }
	}
    else{
	if  ( aci->aciFormat >= 0 && aci->aciFormat < ea->eaFileExtensionCount )
	    {
	    if  ( ! fileExists )
		{
		if  ( fileAddExtension( &changed,
			ea->eaFileExtensions[aci->aciFormat].afeExtension ) )
		    { LDEB(aci->aciFormat); rval= -1; goto ready;	}
		}

	    format= aci->aciFormat;
	    }
	}

    resp= appFileChooserTestNameForWrite( aci, &changed );

    if  ( resp == ACIrespSAVE )
	{
	if  ( (*aci->aciSaveDocument)( ed, aci->aciThrough,
				    aci->aciRelativeTo, aci->aciOption,
				    format, &changed ) )
	    { LDEB(format); rval= -1; goto ready;	}
	}

    rval= resp;

  ready:

    utilCleanMemoryBuffer( &changed );
    utilCleanMemoryBuffer( &ext );

    return rval;
    }

#   endif
