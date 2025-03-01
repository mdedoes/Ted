/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>
#   include	<utilArgToX.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedAppResources.h"
#   include	"tedDocument.h"
#   include	<appFileChooser.h>
#   include	<docTreeNode.h>
#   include	<utilFileExtension.h>
#   include	<appInspector.h>
#   include	<docFileExtensions.h>
#   include	<docDocumentProperties.h>
#   include	<appEditApplication.h>
#   include	<appGuiApplication.h>
#   include	<appEditDocument.h>
#   include	<docBuf.h>
#   include	<docDocument.h>
#   include	<appPrintDialog.h>
#   include	<bitmap.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine application specific default settings.			*/
/*									*/
/*  2)	Determine default for show table grid.				*/
/*									*/
/************************************************************************/

void tedDetermineDefaultSettings(	TedAppResources *	tar )
    {
    char *	past;

    /*  2  */
    if  ( appDetermineBoolean( &(tar->tarShowTableGridInt),
					    tar->tarShowTableGridString ) )
	{ SDEB(tar->tarShowTableGridString);	}

    /*  3  */
    if  ( tar->tarShadingMeshTwipsString	&&
	  tar->tarShadingMeshPointsDouble <= 0	)
	{
	long meshTwips= strtol( tar->tarShadingMeshTwipsString, &past, 10 );

	if  ( meshTwips <= 0 || meshTwips > 400 )
	    { SFDEB(tar->tarShadingMeshTwipsString,meshTwips); }

	tar->tarShadingMeshPointsDouble= meshTwips/ 20.0;
	}

    tar->tarPageGapMM= 4;
    if  ( tar->tarPageGapMMString )
	{ utilArgToInt( &(tar->tarPageGapMM), tar->tarPageGapMMString ); }

    if  ( appDetermineBoolean( &(tar->tarLenientRtfInt),
					    tar->tarLenientRtfString ) )
	{ SDEB(tar->tarLenientRtfString);	}

    if  ( appDetermineBoolean( &(tar->tarTraceEditsInt),
					    tar->tarTraceEditsString ) )
	{ SDEB(tar->tarTraceEditsString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfOutlineInt),
					    tar->tarPdfOutlineString ) )
	{ SDEB(tar->tarPdfOutlineString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfMarkContentInt),
					    tar->tarPdfMarkContentString ) )
	{ SDEB(tar->tarPdfMarkContentString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfOmitContentMarksInt),
					    tar->tarPdfOmitContentMarksString ) )
	{ SDEB(tar->tarPdfOmitContentMarksString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfDeclareUACompliantInt),
					    tar->tarPdfDeclareUACompliantString ) )
	{ SDEB(tar->tarPdfDeclareUACompliantString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfMarkInvisibleAsArtifactInt),
					    tar->tarPdfMarkInvisibleAsArtifactString ) )
	{ SDEB(tar->tarPdfMarkInvisibleAsArtifactString);	}

    if  ( appDetermineBoolean( &(tar->tarPdfFlattenPlainTablesInt),
					    tar->tarPdfFlattenPlainTablesString ) )
	{ SDEB(tar->tarPdfFlattenPlainTablesString);	}

    if  ( appDetermineBoolean( &(tar->tarDrawParticulesSeparatelyInt),
					    tar->tarDrawParticulesSeparatelyString ) )
	{ SDEB(tar->tarDrawParticulesSeparatelyString);	}

    if  ( appDetermineBoolean( &(tar->tarOverridePaperSizeInt),
					    tar->tarOverridePaperSizeString ) )
	{ SDEB(tar->tarOverridePaperSizeString);	}

    if  ( appDetermineBoolean( &(tar->tarInlineHtmlImagesInt),
					    tar->tarInlineHtmlImagesString ) )
	{ SDEB(tar->tarInlineHtmlImagesString);	}

    if  ( appDetermineBoolean( &(tar->tarHonourSpecialSectBreaksInt),
				    tar->tarHonourSpecialSectBreaksString ) )
	{ SDEB(tar->tarHonourSpecialSectBreaksString);	}

    return;
    }

/************************************************************************/

static int	tedBuildOpenImageExtensions(	TedAppResources *	tar )
    {
    if  ( tar->tarOpenImageExtensionCount == 0 )
	{
	AppFileExtension *	afe;

	if  ( bmMakeFileExtensions( &(tar->tarOpenImageExtensions),
					&(tar->tarOpenImageExtensionCount) ) )
	    { LDEB(1); return -1;	}

	afe= (AppFileExtension *) realloc( tar->tarOpenImageExtensions,
				    (tar->tarOpenImageExtensionCount+ 1)*
				    sizeof(AppFileExtension) );
	if  ( ! afe )
	    { LXDEB(tar->tarOpenImageExtensionCount,afe); return -1; }
	tar->tarOpenImageExtensions= afe;

	afe += tar->tarOpenImageExtensionCount;

	afe->afeId= "wmfFile";
	afe->afeFilter= "*.wmf";
	afe->afeDescription= "Windows Metafile ( *.wmf )";
	afe->afeExtension= "wmf";
	afe->afeUseFlags= APPFILE_CAN_OPEN;

	tar->tarOpenImageExtensionCount++;
	}

    return 0;
    }

/************************************************************************/

static int tedInsertDocumentFile(
				EditDocument *			ed,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				const struct MemoryBuffer *	filename )
    {
    int			rval= 0;

    const int		complain= 1;
    const int		suggestStdin= 0;
    int			formatHint= -1;
    int			format;

    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    struct BufferDocument *	bd= (struct BufferDocument *)0;

    formatHint= utilDocumentGetOpenFormat( (int *)0,
		    ea->eaFileExtensions, ea->eaFileExtensionCount,
		    filename, formatHint );

    if  ( tedOpenDocumentFile( (unsigned char *)0, &format, &bd, ea,
				    suggestStdin, formatHint, filename,
				    complain, relative, option ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( format )
	{
	case TEDdockindRTF:
	    if  ( tedIncludeRtfDocument( ed, bd, td->tdTraced ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    break;

	case TEDdockindTEXT_SAVE_WIDE:
	case TEDdockindTEXT_SAVE_FOLDED:
	    if  ( tedIncludePlainDocument( ed, bd, td->tdTraced ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    break;

	default:
	    LDEB(format); rval= -1; goto ready;
	}
	
  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Callback for the 'Insert File' menu option.				*/
/*  1)  Test whether the file is an image file: If so include it as an	*/
/*	image.								*/
/*  2)  Otherwise, include it as a document.				*/
/*									*/
/************************************************************************/

static int tedInsertFile(	EditApplication *	ea,
				void *			voided,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const MemoryBuffer *	filename )
    {
    int				rval= 0;
    EditDocument *		ed= (EditDocument *)voided;

    int				format;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    /*  1  */
    format= utilDocumentGetOpenFormat( (int *)0,
			tar->tarOpenImageExtensions,
			tar->tarOpenImageExtensionCount, filename, -1 );

    if  ( format >= 0 )
	{
	/*  1  */
	rval= tedDocInsertImageFile( ed, relative, option, filename );
	if  ( rval )
	    { LDEB(rval);	}
	}
    else{
	/*  2  */
	if  ( tedInsertDocumentFile( ed, relative, option, filename ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
	    
  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Callback for the 'Insert File' menu option.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertFile, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); return;	}

    if  ( tedBuildOpenImageExtensions( tar ) )
	{ LDEB(1);	}

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			    ea->eaFileExtensionCount, ea->eaFileExtensions,
			    ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			    tedInsertFile, ea, voided );

    return;
    }

APP_MENU_CALLBACK_H( tedDocFilePrint, printOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int			pageCount= bd->bdBody.dtRoot->biBelowPosition.lpPage+ 1;
    int			firstPage= -1;
    int			lastPage= -1;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const DocumentGeometry *	dgDoc= &(bd->bdProperties->dpGeometry);

    if  ( ! tedGetSelection( &ds, &sg, &sd,
				(struct DocumentTree **)0,
				(struct BufferItem **)0, ed ) )
	{
	firstPage= sg.sgHead.pgTopPosition.lpPage;
	lastPage= sg.sgTail.pgTopPosition.lpPage;
	}

    appRunPrintDialog( ed, dgDoc, pageCount, firstPage, lastPage,
					printOption, "print" );

    tedRedoDocumentLayout( ed );
    }

/************************************************************************/
/*									*/
/*  Run a file chooser to insert an image.				*/
/*									*/
/************************************************************************/

static int tedInsertImageFileFromChooser(
			EditApplication *	ea,
			void *			voided,
			APP_WIDGET		relative,
			APP_WIDGET		option,
			const MemoryBuffer *	filename )
    {
    return tedDocInsertImageFile( (EditDocument *)voided,
					    relative, option, filename );
    }

APP_MENU_CALLBACK_H( tedDocInsertImage, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); return;	}

    if  ( tedBuildOpenImageExtensions( tar ) )
	{ LDEB(1); return;	}

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			tar->tarOpenImageExtensionCount,
			tar->tarOpenImageExtensions,
			(char *)0, (MemoryBuffer *)0,
			tedInsertImageFileFromChooser, ea, (void *)ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Hide options that do not belong in the manual window.		*/
/*									*/
/************************************************************************/

static void tedMakeDocumentToHelp(	EditDocument *	ed )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		visible= 0;

    guiShowMenuOption( td->tdFileOpenOption, visible );
    guiShowMenuOption( td->tdFileSaveAsOption, visible );
    guiShowMenuOption( td->tdFileUnlockOption, visible ); /* Already hidden */
    guiShowMenuOption( td->tdFileRecoverOption, visible ); /* Already hidden */
    guiShowMenuOption( td->tdFilePropertiesOption, visible );
    guiShowMenuOption( td->tdFileQuitSeparator, visible );
    guiShowMenuOption( td->tdFileQuitOption, visible );

    /* Oops wrong calls */
    guiShowMenuOption( td->tdFontMenuButton, visible );
    guiShowMenuOption( td->tdTableMenuButton, visible );
    guiShowMenuOption( td->tdFormatMenuButton, visible );
    guiShowMenuOption( td->tdToolsMenuButton, visible );

    guiShowMenuOption( td->tdToolsFormatToolOption, visible );

    /* Oops wrong call */
    guiShowMenuOption( ed->edHelpMenuButton, visible );

    td->tdDrawTableGrid= visible;

    return;
    }

/************************************************************************/
/*									*/
/*  Display the online manual.						*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static APP_DESTROY_CALLBACK_H( tedManualDestroyed, w, voidtar )
    {
    TedAppResources *	tar= (TedAppResources *)voidtar;

    tar->tarManualDocument= (EditDocument *)0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void tedManual(		APP_WIDGET		option,
			EditApplication *	ea,
			APP_WIDGET		relative )
    {
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    const int		readOnly= 1;

    MemoryBuffer	file;

    utilInitMemoryBuffer( &file );

    if  ( tar->tarManualDocument )
	{ appMakeDocVisible( ea, tar->tarManualDocument );	}
    else{
	if  ( utilMemoryBufferSetString( &file, tar->tarAppHelpFileName ) )
	    { LDEB(1); goto ready;	}

	tar->tarManualDocument= appOpenDocument( ea, relative,
						    option, readOnly, &file );

	if  ( ! tar->tarManualDocument )
	    {
	    SXDEB(tar->tarAppHelpFileName,tar->tarManualDocument);
	    goto ready;
	    }

	tedMakeDocumentToHelp( tar->tarManualDocument );

	guiSetDestroyCallback( tar->tarManualDocument->edToplevel.atTopWidget,
					tedManualDestroyed, (void *)tar );
	}

  ready:

    utilCleanMemoryBuffer( &file );

    return;
    }

/************************************************************************/
/*									*/
/*  Turn tools on/off depending on the number of visible documents.	*/
/*									*/
/************************************************************************/

void tedVisibleDocumentCountChanged(	EditApplication *	ea,
					int			from,
					int			to )
    {
    if  ( from == 0 && to > 0 )
	{
	if  ( ea->eaInspector )
	    { appEnableInspector( ea->eaInspector, 1 ); }
	}

    if  ( from > 0 && to == 0 )
	{
	if  ( ea->eaInspector )
	    { appEnableInspector( ea->eaInspector, 0 ); }
	}

    return;
    }

