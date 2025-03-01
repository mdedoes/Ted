#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<sioFileio.h>
#   include	<sioFd.h>
#   include	<sioStdout.h>
#   include	<sioPipe.h>
#   include	<sioMD5.h>
#   include	<utilMD5.h>
#   include	<docHtmlWrite.h>
#   include	<appSystem.h>
#   include	<psPrint.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"

#   include	<docPsPrint.h>
#   include	<docSvgDraw.h>
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docMarkdownReadWrite.h>
#   include	<docField.h>
#   include	<docRecalculateTocField.h>
#   include	<docRtfTrace.h>
#   include	<docEditCommand.h>

#   include	<appPrintJob.h>
#   include	<docFileExtensions.h>
#   include	<docRecalculateIncludeTextField.h>
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<appEditApplication.h>
#   include	<appPrintDocument.h>
#   include	<sioGeneral.h>
#   include	<layoutContext.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recalculate all fields in the document. (If needed)			*/
/*									*/
/************************************************************************/

static int tedRefreshRecalculateFields(	int *			pReachedBottom,
					RecalculateFields *	rf,
					TedDocument *		td,
					const LayoutContext *	lc )
    {
    struct BufferDocument *		bd= td->tdDocument;
    const int			page= 0;

    rf->rfDocument= bd;
    rf->rfUpdateFlags= FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoDOC_INFO;
    rf->rfFieldsUpdated= 0;
    rf->rfLocale= lc->lcLocale;

    rf->rfBodySectNode= bd->bdBody.dtRoot->biChildren[0];

    if  ( docRecalculateIncludeTextFields( rf ) )
	{ LDEB(1); return -1;	}
    if  ( docRecalculateTocFields( rf ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf, &(bd->bdBody),
				    bd->bdBody.dtRoot->biChildren[0], page ) )
	{ LDEB(1); return -1;	}

    if  ( rf->rfFieldsUpdated > 0			&&
	  tedLayoutDocumentBody( pReachedBottom, lc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a document to PostScript.					*/
/*									*/
/************************************************************************/

int tedPreparePrint(	const PrintJob *		pj,
			const PrintGeometry *		pg )
    {
    int				rval= 0;

    EditApplication *		ea= pj->pjApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)pj->pjPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    DocumentProperties *	dp= bd->bdProperties;

    time_t			now;

    RecalculateFields		rf;
    LayoutContext		lc;

    MemoryBuffer		fontDir;

    utilInitMemoryBuffer( &fontDir );

    docInitRecalculateFields( &rf );

    if  ( ea->eaFontDirectory						&&
	  utilMemoryBufferSetString( &fontDir, ea->eaFontDirectory )	)
	{ XDEB(ea->eaFontDirectory); rval= -1; goto ready;	}

    now= time( (time_t *)0 );
    dp->dpPrintim= *localtime( &now );

    layoutInitContext( &lc );
    tedSetDocumentLayoutContext( &lc, pj->pjDrawingSurface,
				&(ea->eaPostScriptFontList),
				tar->tarHonourSpecialSectBreaksInt, td );

    if  ( tedRefreshRecalculateFields( (int *)0, &rf, td, &lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRecalculateFields( &rf );

    utilCleanMemoryBuffer( &fontDir );

    return rval;
    }

int tedPrintDocument(	SimpleOutputStream *		sos,
			const PrintJob *		pj,
			const PrintGeometry *		pg )
    {
    int				rval= 0;

    EditApplication *		ea= pj->pjApplication;
    TedDocument *		td= (TedDocument *)pj->pjPrivateData;

    LayoutContext		lc;

    double			shadingMesh= -1;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    int				emitOutline= tar->tarPdfOutlineInt > 0;
    int				markContent= tar->tarPdfMarkContentInt > 0;
    int				omitContentMarks= tar->tarPdfOmitContentMarksInt > 0;
    int				markInvisibleAsArtifact= tar->tarPdfMarkInvisibleAsArtifactInt > 0;
    int				flattenPlainTables= tar->tarPdfFlattenPlainTablesInt > 0;
    int				drawParticulesSeparately= tar->tarDrawParticulesSeparatelyInt > 0;
    int				declareUACompliant= tar->tarPdfDeclareUACompliantInt > 0;

    MemoryBuffer		fontDir;

    utilInitMemoryBuffer( &fontDir );

    if  ( ea->eaFontDirectory						&&
	  utilMemoryBufferSetString( &fontDir, ea->eaFontDirectory )	)
	{ XDEB(ea->eaFontDirectory); rval= -1; goto ready;	}

    layoutInitContext( &lc );
    tedSetDocumentLayoutContext( &lc, pj->pjDrawingSurface,
				&(ea->eaPostScriptFontList),
				tar->tarHonourSpecialSectBreaksInt, td );

    if  ( docPsPrintDocument( sos, pj->pjTitle,
				    ea->eaApplicationName,
				    ea->eaReference,
				    &fontDir,
				    shadingMesh,
				    emitOutline,
				    markContent,
				    omitContentMarks,
				    declareUACompliant,
				    markInvisibleAsArtifact,
				    flattenPlainTables,
				    drawParticulesSeparately,
				    &lc, pg ) )
	{ LDEB(1); rval= -1;	}

  ready:

    utilCleanMemoryBuffer( &fontDir );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save a document to SVG.						*/
/*									*/
/************************************************************************/

static int tedSaveSvg(	SimpleOutputStream *		sos,
			EditApplication *		ea,
			TedDocument *			td,
			DrawingSurface			ds )
    {
    int				rval= 0;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    RecalculateFields		rf;
    LayoutContext		lc;

    docInitRecalculateFields( &rf );
    layoutInitContext( &lc );

    tedSetDocumentLayoutContext( &lc, ds,
				&(ea->eaPostScriptFontList),
				tar->tarHonourSpecialSectBreaksInt, td );

    if  ( tedRefreshRecalculateFields( (int *)0, &rf, td, &lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSvgDrawDocument( sos, ea->eaApplicationName,
					    ea->eaReference, &lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRecalculateFields( &rf );

    return rval;
    }

/************************************************************************/

static SimpleOutputStream * tedOpenSaveOutput(
				const DocumentProperties *	dp,
				int				format,
				int				suggestStdout,
				const MemoryBuffer *		filename )
    {
    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    SimpleOutputStream *	rval= (SimpleOutputStream *)0;
    MemoryBuffer		pdfCommand;

    utilInitMemoryBuffer( &pdfCommand );

    if  ( format == TEDdockindPDF )
	{
	const int	wide= dp->dpGeometry.dgPageWideTwips;
	const int	high= dp->dpGeometry.dgPageHighTwips;

	if  ( suggestStdout )
	    {
	    if  ( psSetToPdfCommand( &pdfCommand, wide, high,
						(const MemoryBuffer *)0 ) )
		{ LDEB(1); goto ready;	}
	    }
	else{
	    if  ( psSetToPdfCommand( &pdfCommand, wide, high, filename ) )
		{ LDEB(1); goto ready;	}
	    }

	sosDoc= sioOutPipeOpen( &pdfCommand );
	if  ( ! sosDoc )
	    { /* SXDEB(filename,sosDoc); */ goto ready;	}
	}
    else{
	if  ( suggestStdout )
	    {
	    sosDoc= sioOutStdoutOpen();
	    if  ( ! sosDoc )
		{ /* SXDEB(filename,sosDoc); */ goto ready;	}
	    }
	else{
	    sosDoc= sioOutFileioOpen( filename );
	    if  ( ! sosDoc )
		{ /* SXDEB(filename,sosDoc); */ goto ready;	}
	    }
	}

    rval= sosDoc; sosDoc= (SimpleOutputStream *)0; /* steal */

  ready:

    utilCleanMemoryBuffer( &pdfCommand );

    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}

    return rval;
    }

/************************************************************************/

static int tedLayoutForSaveFormatted(
				RecalculateFields *		rf,
				LayoutContext *			lc,
				DrawingSurface			ds,
				const EditApplication *		ea,
				TedDocument *			td,
				int				format )
    {
    int				rval= 0;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    struct BufferDocument *	bd= td->tdDocument;
    DocumentProperties *	dp= bd->bdProperties;

    docInitRecalculateFields( rf );
    layoutInitContext( lc );

    tedSetDocumentLayoutContext( lc, ds,
			    &(ea->eaPostScriptFontList),
			    tar->tarHonourSpecialSectBreaksInt, td );
    lc->lcLocale= td->tdLocale;
    rf->rfLocale= lc->lcLocale;

    if  ( ! ds )
	{
	DocumentRectangle	drScreenIgnored;
	DocumentRectangle	drVisibleIgnored;

	if  ( tedFirstRecalculateFields( rf, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( tedLayoutDocument( ea, &drScreenIgnored, &drVisibleIgnored,
				    td, format, (DrawingSurface)0,
				    &(ea->eaPostScriptFontList),
				    &(dp->dpGeometry) ) )
	    { LDEB(format); rval= -1; goto ready;	}
	}

  ready:

    docCleanRecalculateFields( rf );

    return rval;
    }

int tedSaveDocumentToStream(	EditApplication *	ea,
				DrawingSurface		ds,
				SimpleOutputStream *	sos,
				TedDocument *		td,
				int			format,
				const MemoryBuffer *	documentTitle,
				const MemoryBuffer *	filename )
    {
    struct BufferDocument *	bd= td->tdDocument;

    const struct DocumentSelection * const ds0 =
				(const struct DocumentSelection *)0;

    switch( format )
	{
	case TEDdockindRTF:
	    if  ( docRtfSaveDocument( sos, bd, ds0, 0 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindTEXT_OPEN:
	    LDEB(format);
	    /*FALLTHROUGH*/

	case TEDdockindTEXT_SAVE_WIDE:
	    if  ( docPlainSaveDocument( sos, bd, ds0, 0 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindTEXT_SAVE_FOLDED:
	    if  ( docPlainSaveDocument( sos, bd, ds0, 1 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindMARKDOWN:
	    if  ( docMarkdownSaveDocument( sos, bd, ds0 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindHTML_FILES:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;
	    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;
	    int			inlineHtmlImages= tar->tarInlineHtmlImagesInt;

	    if  ( tedLayoutForSaveFormatted( &rf, &lc, ds, ea, td, format ) )
		{ SDEB(utilMemoryBufferGetString(documentTitle)); return -1; }

	    if  ( docHtmlSaveDocument( sos, bd, filename, &lc,
						    inlineHtmlImages > 0 ) )
		{ LDEB(1); return -1;	}
	    }
	    break;

	case TEDdockindEML:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    if  ( tedLayoutForSaveFormatted( &rf, &lc, ds, ea, td, format ) )
		{ SDEB(utilMemoryBufferGetString(documentTitle)); return -1; }

	    if  ( docEmlSaveDocument( sos, bd, &lc ) )
		{ LDEB(format); return -1;	}
	    }
	    break;

	case TEDdockindEPUB:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    if  ( tedLayoutForSaveFormatted( &rf, &lc, ds, ea, td, format ) )
		{ SDEB(utilMemoryBufferGetString(documentTitle)); return -1; }

	    if  ( docEpubSaveDocument( sos, bd, &lc ) )
		{ LDEB(format); return -1;	}
	    }
	    break;

	case TEDdockindPS:
	case TEDdockindPDF:
	    if  ( appSaveToPs( ea, ds, sos, td, documentTitle, format ) )
		{ LDEB(format); return -1;	}
	    break;

	case TEDdockindSVG:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    if  ( tedLayoutForSaveFormatted( &rf, &lc, ds, ea, td, format ) )
		{ SDEB(utilMemoryBufferGetString(documentTitle)); return -1; }

	    if  ( tedSaveSvg( sos, ea, td, ds ) )
		{ SDEB(utilMemoryBufferGetString(documentTitle)); return -1; }
	    }

	    break;

	default:
	    LDEB(format); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a document.							*/
/*									*/
/************************************************************************/

int tedSaveDocument(	EditApplication *	ea,
			DrawingSurface		ds,
			const void *		privateData,
			int			format,
			const MemoryBuffer *	documentTitle,
			int			suggestStdout,
			const MemoryBuffer *	filename,
			int			isDocName )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)privateData;
    EditTrace *			etOld= &(td->tdEditTrace);
    struct BufferDocument *	bd= td->tdDocument;
    DocumentProperties *	dp= bd->bdProperties;

    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosMD5= (SimpleOutputStream *)0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    unsigned char		digest[MD5_DIGEST_SIZE_BYTES];
    int				saveDigest= 0;

    EditTrace			etNew;
    int				renameTrace= 0;

    int				fdTrace= FDerrOTHER;

    docInitEditTrace( &etNew );

    if  ( isDocName )
	{
	if  ( docEditTraceSetDocumentName( &etNew, filename,
						    TedTraceExtension ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( etOld->etTraceStatus == TRACING_RELATIVE )
	    {
	    if  ( ! utilEqualMemoryBuffer( &(etNew.etTraceFileName),
						&(etOld->etTraceFileName ) ) )
		{ renameTrace= 1;	}
	    }

	if  ( etOld->etTraceStatus == TRACING_TEMP	||
	      etOld->etTraceStatus == TRACING_ANON	)
	    { renameTrace= 1;	}

	if  ( renameTrace )
	    {
	    fdTrace= docEditTraceTryRelative( &etNew, filename,
							TedTraceExtension );

	    if  ( fdTrace < 0 && etNew.etTraceStatus == TRACING_NO )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    if  ( fdTrace < 0 && etNew.etTraceStatus == TRACING_EXIST )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    }
	}

    sosDoc= tedOpenSaveOutput( dp, format, suggestStdout, filename );
    if  ( ! sosDoc )
	{ /* SXDEB(filename,sosDoc); */ rval= -1; goto ready;	}

    sos= sosDoc;
    if  ( isDocName && td->tdTraced )
	{
	sosMD5= sioOutMD5Open( digest, sosDoc );
	if  ( ! sosMD5 )
	    { XDEB(sosMD5); rval= -1; goto ready;	}

	sos= sosMD5;
	saveDigest= 1;
	}

    if  ( suggestStdout && format == TEDdockindHTML_FILES )
	{ filename= (const MemoryBuffer *)0;	}

    if  ( format == TEDdockindRTF )
	{
	time_t		now= time( (time_t *)0 );

	dp->dpRevtim= *localtime( &now );
	}

    if  ( tedSaveDocumentToStream( ea, ds, sos, td, format,
						    documentTitle, filename ) )
	{ LDEB(format); rval= -1; goto ready;	}

    /*  flush to finalize digest */
    if  ( sosMD5 )
	{
	sioOutClose( sosMD5 ); sosMD5= (SimpleOutputStream *)0;

	utilMD5ToBase64( etNew.etBaseMD5Digest64, digest );
	}

    if  ( renameTrace							&&
	  etNew.etTraceStatus == TRACING_ACCES				&&
	  ! strcmp( etOld->etBaseMD5Digest64, etNew.etBaseMD5Digest64 )	)
	{ renameTrace= 0;	}

    if  ( renameTrace )
	{
	const int	restart= 0;
	const int	exclusive= 0;

	if  ( etNew.etTraceStatus == TRACING_ACCES )
	    {
	    fdTrace= docEditTraceTryTemp( &etNew, TedTraceExtension );

	    if  ( fdTrace < 0 )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    }

	sioFdCloseFile( etNew.etTraceFileHandle ); etNew.etTraceFileHandle= -1;
	sioFdCloseFile( etOld->etTraceFileHandle ); etOld->etTraceFileHandle= -1;

	if  ( appRenameFile( &(etNew.etTraceFileName),
						&(etOld->etTraceFileName) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(etOld->etTraceFileName),
						&(etNew.etTraceFileName) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	etOld->etTraceStatus= etNew.etTraceStatus;

	/*  Name just changed */
	if  ( docEditTraceOpenTrace( etOld, restart, exclusive ) < 0 )
	    {
	    SDEB(utilMemoryBufferGetString(&(etOld->etTraceFileName)));
	    rval= -1; goto ready;
	    }
	}

    if  ( saveDigest )
	{
	utilMD5ToBase64( etOld->etBaseMD5Digest64, digest );

	if  ( docRtfTraceVersion( EDITcmd_SAVE, filename,
				etOld->etBaseMD5Digest64, &(dp->dpRevtim),
				etOld, (struct BufferDocument *)0 ) )
	    { LDEB(saveDigest); rval= -1; goto ready;	}

	etOld->etBase= etOld->etIndex- 1;
	etOld->etTyping= TYPING_NO;
	}

    if  ( isDocName )
	{ etOld->etIsRecovery= 0;	}

  ready:

    if  ( sosMD5 )
	{ sioOutClose( sosMD5 );	}
    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}
    
    docCleanEditTrace( &etNew );

    return rval;
    }

