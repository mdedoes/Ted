/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>

#   include	<appSystem.h>
#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocFront.h"
#   include	"appQuestion.h"
#   include	"appFileChooser.h"

#   include	<utilFileExtension.h>

#   include	<appDebugon.h>

#   define	LOG_REDRAWS	0

/************************************************************************/
/*									*/
/*  Callback for the file menu.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocFileSave, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    APP_WIDGET			relative= ed->edToplevel.atTopWidget;

    if  ( ! ea->eaSaveDocument )
	{ XDEB(ea->eaSaveDocument); return;	}

    if  ( utilMemoryBufferIsEmpty( &(ed->edFilename) )	||
	  ed->edFileReadOnly				||
	  ed->edFormat < 0				)
	{
	appRunSaveChooser( option, ed->edToplevel.atTopWidget,
			    appDocSaveDocument, ed, ed->edPrivateData );

	return;
	}

    if  ( ! appDocSaveDocument( ed, (void *)0, relative, option,
					    ed->edFormat, &(ed->edFilename) ) )
	{ appDocumentChanged( ed, 0 ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Open a a document.							*/
/*									*/
/*  1)  Allocate a document.						*/
/*  3)  Make a shell to contain everything.				*/
/*  4)  Make the window pane.						*/
/*									*/
/************************************************************************/

EditDocument * appOpenDocumentFile(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			readOnly,
					int			suggestStdin,
					int			formatHint,
					const MemoryBuffer *	filename )
    {
    EditDocument *		ed;
    int				fileReadonly= 0;
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    if  ( readOnly || suggestStdin )
	{ fileReadonly= 1;	}
    else{
	if  ( fileTestFileReadable( filename ) )
	    {
	    int			resp;

	    resp= appQuestionRunFilenameOkCancelDialog( ea, relative, option,
					    filename, afmr->afmrFileReadOnly );

	    if  ( resp != AQDrespOK )
		{ return (EditDocument *)0;	}

	    fileReadonly= 1;
	    }
	}

    /*  1,3,4  */
    if  ( suggestStdin )
	{
	/* as if from 'new' */
	if  ( appMakeDocumentWindow( &ed, ea, readOnly,
					(const MemoryBuffer *)0,
					(const MemoryBuffer *)0 ) )
	    { LDEB(1); return (EditDocument *)0;	}
	}
    else{
	if  ( appMakeDocumentWindow( &ed, ea, readOnly,
				    filename, filename ) )
	    { LDEB(1); return (EditDocument *)0;	}
	}

    ed->edFileReadOnly= fileReadonly;

    if  ( (*ea->eaOpenDocument)( ea, ed->edPrivateData, &(ed->edFormat),
				relative, option,
				readOnly, suggestStdin, formatHint, filename ) )
	{ return (EditDocument *)0; }

    if  ( appSetupDocument( ea, ed ) )
	{ LDEB(1); return (EditDocument *)0; }

    appSetDocument( ea, ed );

    return ed;
    }

EditDocument * appOpenDocument(	EditApplication *	ea,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			readOnly,
				const MemoryBuffer *	filename )
    {
    const int	suggestStdin= 0;
    int		formatHint= -1;

    formatHint= utilDocumentGetOpenFormat( (int *)0,
			ea->eaFileExtensions, ea->eaFileExtensionCount,
			filename, formatHint );

    return appOpenDocumentFile( ea, relative, option,
			    readOnly, suggestStdin, formatHint, filename );
    }

int appNewDocument(	EditApplication *	ea,
			const MemoryBuffer *	filename )
    {
    const int			readOnly= 0;
    const char *		title= (const char *)0;

    EditDocument *		ed;

    /*  1,3,4  */
    if  ( appMakeDocumentWindow( &ed, ea, readOnly, filename, filename ) )
	{ LDEB(1); return -1;	}

    if  ( (*ea->eaNewDocument)( ed, filename ) )
	{ SDEB(title); return -1; }

    if  ( appSetupDocument( ea, ed ) )
	{ SDEB(title); return -1; }

    appSetDocument( ea, ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callback from the 'Save As' action of the filechooser.		*/
/*									*/
/************************************************************************/

int appDocSaveDocument(	EditDocument *		ed,
			void *			through,
			APP_WIDGET		relative,
			APP_WIDGET		option,
			int			format,
			const MemoryBuffer *	filename )
    {
    EditApplication *	ea= ed->edApplication;
    const int		suggestStdout= 0;

    int			canOpen= appFileCanOpen( ea, format );
    int			isDocName= canOpen;

    if  ( (*ea->eaSaveDocument)( ea, ed->edDrawingSurface,
				ed->edPrivateData, format, &(ed->edTitle),
				suggestStdout, filename, isDocName ) )
	{
	appReportSaveFailure( ea, option, relative, filename );
	return -1;
	}

    if  ( canOpen )
	{
	appDocumentChanged( ed, 0 );
	ed->edFileReadOnly= 0;
	ed->edFormat= format;

	if  ( ! utilEqualMemoryBuffer( &(ed->edFilename), filename ) )
	    {
	    if  ( appSetDocumentFilename( ed, filename ) )
		{ LDEB(1);	}
	    if  ( appSetDocumentTitle( ed, filename ) )
		{ LDEB(1);	}
	    }
	}

    return 0;
    }

/************************************************************************/

static int appOpenExistingDocument(	EditApplication *	ea,
					int			readOnly,
					int			formatHint,
					const MemoryBuffer *	filename )
    {
    int				rval= 0;
    const MemoryBuffer * const	fileRelativeTo= (const MemoryBuffer *)0;
    int				relativeIsFile= 1;
    EditDocument *		ed;

    MemoryBuffer		absolute;

    const int			suggestStdin= 0;

    utilInitMemoryBuffer( &absolute );

    if  ( fileAbsoluteName( &absolute,
		    filename, relativeIsFile, fileRelativeTo ) < 0 )
	{
	SDEB(utilMemoryBufferGetString(filename));
	ed= appOpenDocumentFile( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				readOnly, suggestStdin, formatHint, filename );
	}
    else{
	ed= appOpenDocumentFile( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				readOnly, suggestStdin, formatHint, &absolute );
	}

    if  ( ! ed )
	{ XDEB(ed); rval= -1;	}

    utilCleanMemoryBuffer( &absolute );

    return rval;
    }

int appMainOpenDocument(		EditApplication *	ea,
					int			refuseStdin,
					const MemoryBuffer *	filename )
    {
    const int		readOnly= 0;

    int			openFormat= -1;
    int			suggestStdin= 0;

    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    openFormat= utilDocumentGetOpenFormat( &suggestStdin,
		    ea->eaFileExtensions, ea->eaFileExtensionCount,
		    filename, openFormat );

    if  ( fileTestFileExists( filename ) )
	{
	int		createNew= ea->eaCreateNewFromCommand;

	int		saveFormat= -1;
	int		suggestStdout= 0;

	if  ( ! refuseStdin && openFormat >= 0 && suggestStdin )
	    {
	    appOpenDocumentFile( ea,
			    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
			    readOnly, suggestStdin, openFormat, filename );

	    return 0;
	    }

	saveFormat= appDocumentGetSaveFormat( &suggestStdout, ea,
					    filename, (void *)0,
					    APPFILE_CAN_SAVE, saveFormat );
	if  ( saveFormat < 0 || suggestStdout )
	    { createNew= 0;	}

	if  ( createNew )
	    {
	    int	ynRes;

	    ynRes= appQuestionRunSubjectYesNoDialog( ea,
				ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				filename, afmr->afmrMakeItQuestion );

	    if  ( ynRes == AQDrespYES )
		{
		if  ( appNewDocument( ea, filename ) )
		    { SDEB(utilMemoryBufferGetString(filename));	}
		}
	    }
	else{
	    appQuestionRunFilenameErrorDialog( ea,
				ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				filename, afmr->afmrNoSuchFileMessage );
	    }
	}
    else{
	(void) appOpenExistingDocument( ea,
				    readOnly, openFormat, filename );
	}

    return 0;
    }

#   endif
