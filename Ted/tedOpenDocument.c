#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioFileio.h>
#   include	<sioMemory.h>
#   include	<utilMD5.h>
#   include	<sioFd.h>
#   include	<appSystem.h>
#   include	<geoUnits.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"
#   include	"tedDocument.h"
#   include	<tedDocFront.h>

#   include	<docDebug.h>
#   include	<docPlainReadWrite.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<docEditCommand.h>
#   include	<docRtfTrace.h>
#   include	<fontMatchFont.h>

#   include	<docFileExtensions.h>
#   include	<utilFileExtension.h>

#   include	<appDocument.h>
#   include	<docEditStep.h>
#   include	<docDocumentProperties.h>
#   include	<textMsLocale.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<sioGeneral.h>
#   include	<docBuf.h>
#   include	<docDocument.h>

#   include	<appDebugon.h>

/************************************************************************/

int tedSetGenerator(		DocumentProperties *	dp,
				const EditApplication *	ea )
    {
    utilMemoryBufferPrintf( &(dp->dpGeneratorWrite),
		    "%s (%s);", ea->eaNameAndVersion, ea->eaReference );
    
    return 0;
    }

/************************************************************************/

static int tedSetDocumentDefaults(	TedDocument *		td,
					EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    double			pixPerMM;


    td->tdPixelsPerTwip= ea->eaMagnification* ea->eaPixelsPerTwip;
    pixPerMM= TWIPS_PER_MM* td->tdPixelsPerTwip;
    td->tdPageGapPixels= pixPerMM* tar->tarPageGapMM;

    /*  2  */
    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); return -1;	}

    return 0;
    }

/************************************************************************/

static struct BufferDocument *	tedMakeDocument(
					EditApplication *	ea )
    {
    const PostScriptFontList *	psfl= (const PostScriptFontList *)0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    struct BufferDocument *	rval= (struct BufferDocument *)0;
    DocumentProperties *	dp;

    TextAttribute		ta;
    time_t			now;

    int				lcId;

    if  ( ea->eaPreferBase35FontsInt <= 0 )
	{ psfl= &(ea->eaPostScriptFontList);	}

    bd= docNewFile( &ta, "Helvetica", 2* 12, psfl,
				    &(ea->eaDefaultDocumentGeometry) );
    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    dp= bd->bdProperties;

    tedSetGenerator( dp, ea );

    if  ( ea->eaAuthor )
	{
	if  ( utilMemoryBufferSetString( &(dp->dpAuthor), ea->eaAuthor ) )
	    { LDEB(1); goto ready; }
	}

    now= time( (time_t *)0 );
    dp->dpCreatim= *localtime( &now );
    dp->dpRevtim= dp->dpCreatim;

    lcId= textMatchConfiguredLocaleTag( ea->eaLocaleTag );
    if  ( lcId >= 0 )
	{
	bd->bdLocaleId= lcId;
	dp->dpDefaultLocaleId= lcId;
	}

    rval= bd; bd= (struct BufferDocument *)0; /* steal */

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

/************************************************************************/

static struct BufferDocument * tedTryHomeTemplate(
					EditApplication *	ea )
    {
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    struct BufferDocument *	rval= (struct BufferDocument *)0;

    MemoryBuffer	home;
    MemoryBuffer	rel;
    MemoryBuffer	dir;
    MemoryBuffer	templateName;

    const int		suggestStdin= 0;
    const int		formatHint= TEDdockindRTF;
    const int		complain= 0;
    APP_WIDGET		relative= (APP_WIDGET)0;
    APP_WIDGET		option= (APP_WIDGET)0;

    unsigned char *	digest= (unsigned char *)0;

    int			format= formatHint;

    utilInitMemoryBuffer( &home );
    utilInitMemoryBuffer( &rel );
    utilInitMemoryBuffer( &dir );
    utilInitMemoryBuffer( &templateName );

    if  ( fileHomeDirectory( &home ) < 0 )
	{ goto ready;	}
    if  ( utilMemoryBufferSetString( &rel, "Ted" ) < 0 )
	{ goto ready;	}
    if  ( fileAbsoluteName( &dir, &rel, 0, &home ) < 0 )
	{ goto ready;	}
    if  ( utilMemoryBufferSetString( &rel, "new.rtf" ) < 0 )
	{ goto ready;	}
    if  ( fileAbsoluteName( &templateName, &rel, 0, &dir ) < 0 )
	{ goto ready;	}

    if  ( fileTestFileExists( &templateName ) )
	{ goto ready;	}

    if  ( tedOpenDocumentFile( digest, &format, &bd, ea,
				    suggestStdin, formatHint, &templateName,
				    complain, relative, option ) )
	{ /*SDEB(filename);*/ goto ready;	}

    if  ( ea->eaAuthor )
	{
	DocumentProperties *	dp= bd->bdProperties;

	if  ( utilMemoryBufferSetString( &(dp->dpAuthor), ea->eaAuthor ) )
	    { LDEB(1); goto ready;	}
	}

    /*  Completely irrelevant here */
    bd->bdProperties->dpIsDocumentTemplate= 0;

    rval= bd; bd= (struct BufferDocument *)0; /* steal */

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}

    utilCleanMemoryBuffer( &home );
    utilCleanMemoryBuffer( &rel );
    utilCleanMemoryBuffer( &dir );
    utilCleanMemoryBuffer( &templateName );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the last editing command and the (last) position in the trace	*/
/*  where the current version of the document was saved.		*/
/*									*/
/************************************************************************/

typedef struct FindDigest
    {
    int				fdBase;
    int				fdLastEdit;
    int				fdFormat;
    EditTrace *			fdEditTrace;
    EditApplication *		fdApplication;
    MemoryBuffer		fdDocumentName;
    struct BufferDocument *	fdDocument;
    } FindDigest;

static void tedInitFindDigest(	FindDigest *	fd )
    {
    fd->fdBase= -1;
    fd->fdLastEdit= -1;
    fd->fdFormat= -1;
    fd->fdEditTrace= (EditTrace *)0;
    fd->fdApplication= (EditApplication *)0;

    utilInitMemoryBuffer( &(fd->fdDocumentName) );

    fd->fdDocument= (struct BufferDocument *)0;
    }

static void tedCleanFindDigest(	FindDigest *	fd )
    {
    utilCleanMemoryBuffer( &(fd->fdDocumentName) );

    if  ( fd->fdDocument )
	{ docFreeDocument( fd->fdDocument );	}
    }

static int docTraceFindDigest(	const TraceStep *	ts,
				EditStep *		es,
				int			step,
				void *			voidfd )
    {
    FindDigest *	fd= (FindDigest *)voidfd;

    if  ( es->esCommand < EDITcmd_EDIT_COUNT		&&
	  es->esCommand != EDITcmdEXTEND_REPLACE	)
	{ fd->fdLastEdit= step;	}

    if  ( es->esCommand == EDITcmd_NEW			&&
	  ! fd->fdEditTrace->etBaseMD5Digest64[0]	)
	{
	fd->fdBase= step;
	return 0;
	}

    if  ( es->esCommand == EDITcmd_OPEN		||
	  es->esCommand == EDITcmd_SAVE		)
	{
	if  ( utilMemoryBufferEqualsString(
				&(es->esNewDocProps.dpGeneratorRead),
				fd->fdEditTrace->etBaseMD5Digest64 ) )
	    { fd->fdBase= step; /* NO: look for the last one return 1; */ }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember a step in the trace. Look for the last save as well.	*/
/*									*/
/************************************************************************/

static int tedRemebmerTraceStep(	const TraceStep *	ts,
					EditStep *		es,
					int			step,
					void *			voidfd )
    {
    FindDigest *	fd= (FindDigest *)voidfd;
    TraceStep *		tsNew;

    if  ( docTraceFindDigest( ts, es, step, voidfd ) < 0 )
	{ LDEB(step); return -1;	}

    tsNew= utilPagedListClaimItem( &(fd->fdEditTrace->etTraceSteps), step );
    if  ( ! tsNew )
	{ LXDEB(step,tsNew); return -1;	}

    *tsNew= *ts;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Open a copy of the document and compare it to the checksum that we	*/
/*  expect.								*/
/*									*/
/************************************************************************/

static int tedCheckTracedDocument(	FindDigest *		fd,
					const EditStep *	es,
					int			step )
    {
    int				rval= 0;

    const int			complain= 0;
    const int			suggestStdin= 0;

    EditApplication *		ea= fd->fdApplication;
    const MemoryBuffer *	filename= &(es->esNewDocProps.dpHlinkbase);

    unsigned char		digest[MD5_DIGEST_SIZE_BYTES];
    int				format= -1;
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    format= utilDocumentGetOpenFormat( (int *)0,
		    ea->eaFileExtensions, ea->eaFileExtensionCount,
		    filename, format );

    if  ( ! tedOpenDocumentFile( digest, &format, &bd, ea,
			suggestStdin, format, filename,
			complain, (APP_WIDGET)0, (APP_WIDGET)0 ) )
	{
	char	digest64[MD5_DIGEST_SIZE_BASE64];

	utilMD5ToBase64( digest64, digest );

	if  ( utilMemoryBufferEqualsString(
			&(es->esNewDocProps.dpGeneratorRead), digest64 ) )
	    {
	    if  ( utilCopyMemoryBuffer( &(fd->fdDocumentName), filename ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( fd->fdDocument )
		{ docFreeDocument( fd->fdDocument );	}

	    fd->fdDocument= bd; bd= (struct BufferDocument *)0; /* steal */
	    fd->fdBase= step;
	    fd->fdFormat= format;
	    }
	}

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

static int tedTryNewDocument(	FindDigest *		fd,
				EditStep *		es,
				int			step )
    {
    int				rval= 0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    if  ( es->esSourceDocument			&&
	  es->esSourceDocument->bdBody.dtRoot	)
	{
	bd= es->esSourceDocument;
	es->esSourceDocument= (struct BufferDocument *)0; /* steal */
	}
    else{
	bd= tedMakeDocument( fd->fdApplication );
	if  ( ! bd )
	    { XDEB(bd); rval= -1; goto ready;	}
	}

    if  ( fd->fdDocument )
	{ docFreeDocument( fd->fdDocument );	}

    fd->fdDocument= bd; bd= (struct BufferDocument *)0; /* steal */
    fd->fdBase= step;

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Scan (and record) the trace and look for the last version that	*/
/*  still exists on the file system. (With the same checksum.)		*/
/*									*/
/************************************************************************/

static int tedFindDocument(	const TraceStep *	ts,
				EditStep *		es,
				int			step,
				void *			voidfd )
    {
    FindDigest *	fd= (FindDigest *)voidfd;

    if  ( tedRemebmerTraceStep( ts, es, step, voidfd ) < 0 )
	{ LDEB(step); return -1;	}

    switch( es->esCommand )
	{
	case EDITcmd_NEW:
	    if  ( tedTryNewDocument( fd, es, step ) )
		{ LDEB(step); return -1;	}
	    break;

	case EDITcmd_OPEN:
	case EDITcmd_SAVE:
	    if  ( tedCheckTracedDocument( fd, es, step ) )
		{ LDEB(step); return -1;	}
	    break;

	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scan the trace to find the base version of the document. If we	*/
/*  encounter a problem, simply report that no base version can be	*/
/*  found. Failing on a defective trace would make it impossible to	*/
/*  unlock the document by discarding it.				*/
/*									*/
/************************************************************************/

static int tedFindBaseVersion(	EditApplication *	ea,
				TedDocument *		td )
    {
    int			rval= 0;

    EditTrace *		et= &(td->tdEditTrace);
    SimpleInputStream *	sisTrace= (SimpleInputStream *)0;

    FindDigest		fd;

    tedInitFindDigest( &fd );

    fd.fdEditTrace= et;
    fd.fdApplication= ea;

    sisTrace= sioInFileioOpen( &(et->etTraceFileName) );
    if  ( ! sisTrace )
	{ XDEB(sisTrace); rval= -1; goto ready;	}

    if  ( docRtfScanEditTrace( sisTrace, docTraceFindDigest, (void *)&fd,
				    (const IndexMapping *)0, td->tdDocument ) )
	{ LDEB(1); fd.fdBase= -1;	}

    sioInClose( sisTrace ); sisTrace= (SimpleInputStream *)0;

    if  ( fd.fdBase >= 0 )
	{ et->etBase= fd.fdBase;	}

  ready:

    if  ( sisTrace )
	{ sioInClose( sisTrace );	}

    tedCleanFindDigest( &fd );

    return rval;
    }

/************************************************************************/

static int tedOpenTraceFile(	EditApplication *	ea,
				TedDocument *		td,
				int *			pFormat,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;

    EditTrace *		et= &(td->tdEditTrace);
    SimpleInputStream *	sisTrace= (SimpleInputStream *)0;

    const int		read= 1;
    const int		write= 1;
    const int		append= 0;
    const int		create= 0;
    const int		exclusive= 0;

    int			fdTrace= FDerrOTHER;
    FindDigest		fd;

    MemoryBuffer	mbScratch;
    IndexMapping	im;

    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    utilInitMemoryBuffer( &mbScratch );
    utilInitIndexMapping( &im );
    tedInitFindDigest( &fd );

    if  ( utilIndexMappingPut( &im, EDITcmd_NEW, EDITversionNEW ) )
	{ LDEB(EDITcmd_NEW); rval= -1; goto ready;	}

    fdTrace= sioFdOpenFile( filename, read, write, append, create, exclusive );
    if  ( fdTrace < 0 )
	{ LDEB(fdTrace); rval= -1; goto ready;	}

    fd.fdEditTrace= et;
    fd.fdApplication= ea;

    sisTrace= sioInFileioOpen( filename );
    if  ( ! sisTrace )
	{ XDEB(sisTrace); rval= -1; goto ready;	}

    if  ( docRtfScanEditTrace( sisTrace, tedFindDocument, (void *)&fd,
					&im, (struct BufferDocument *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioInClose( sisTrace ); sisTrace= (SimpleInputStream *)0;

    if  ( fd.fdBase >= 0 && fd.fdDocument )
	{
	if  ( utilCopyMemoryBuffer( &(et->etTraceFileName), filename ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(td->tdRecoveredName),
						&(fd.fdDocumentName) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	et->etTraceFileHandle= fdTrace; fdTrace= FDerrOTHER;
	et->etCount= et->etTraceSteps.plItemCount;
	et->etIndex= et->etBase= fd.fdBase;
	et->etIsRecovery= 1;

	/* steal */
	td->tdDocument= fd.fdDocument;
	fd.fdDocument= (struct BufferDocument *)0;

	*pFormat= fd.fdFormat;
	}
    else{
	int		longestPara;

	if  ( utilMemoryBufferSetString( &mbScratch, tar->tarCannotRecover ) )
	    { LDEB(1); rval= -1; goto ready;	}

	sisTrace= sioInMemoryOpen( &mbScratch );
	if  ( ! sisTrace )
	    { XDEB(sisTrace); rval= -1; goto ready;	}

	td->tdDocument= docPlainReadFile( sisTrace, &longestPara,
					    &(ea->eaDefaultDocumentGeometry) );
	sioInClose( sisTrace ); sisTrace= (SimpleInputStream *)0;
	if  ( ! td->tdDocument )
	    { XDEB(td->tdDocument); rval= -1; goto ready;	}

	tedSetGenerator( td->tdDocument->bdProperties, ea );

	*pFormat= TEDdockindTRACE;
	et->etTraceStatus= TRACING_EXIST;
	}

  ready:

    if  ( sisTrace )
	{ sioInClose( sisTrace );	}
    if  ( fdTrace >= 0 )
	{ sioFdCloseFile( fdTrace );	}

    tedCleanFindDigest( &fd );

    utilCleanMemoryBuffer( &mbScratch );
    utilCleanIndexMapping( &im );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/*  1)  Read the file.							*/
/*  2)  Get the list of fonts that are available on the machine.	*/
/*  3)  Add them to the font list of the document.			*/
/*									*/
/************************************************************************/

int tedOpenDocument(	EditApplication *	ea,
			void *			voidtd,
			int *			pFormat,
			APP_WIDGET		relative,
			APP_WIDGET		option,
			const int		readOnly,
			const int		suggestStdin,
			const int		formatHint,
			const MemoryBuffer *	filename )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)voidtd;
    EditTrace *			et= &(td->tdEditTrace);

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    unsigned char		digest_x[MD5_DIGEST_SIZE_BYTES];
    unsigned char *		digest= (unsigned char *)0;

    const int			complain= 1;
    int				traced;

    MemoryBuffer		ext;

    utilInitMemoryBuffer( &ext );

    tedDetermineDefaultSettings( tar );

    traced= tar->tarTraceEditsInt > 0;

    if  ( fileGetFileExtension( &ext, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &ext, TedTraceExtension ) )
	{
	if  ( tedOpenTraceFile( ea, td, pFormat, filename ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( traced && ! readOnly )
	    {
	    int		fdTrace;

	    if  ( suggestStdin )
		{
		fdTrace= docEditTraceTryAnon( et, TedTraceExtension );
		if  ( fdTrace < 0 )
		    { LDEB(fdTrace); rval= -1; goto ready;	}
		}
	    else{
		fdTrace= docEditTraceTryRelative( et,
					    filename, TedTraceExtension );

		if  ( fdTrace < 0 && et->etTraceStatus == TRACING_NO )
		    { LCDEB(fdTrace,et->etTraceStatus); rval= -1; goto ready; }

		digest= digest_x;
		}
	    }

	/*  1,2  */
	if  ( tedOpenDocumentFile( digest, pFormat, &(td->tdDocument), ea,
					suggestStdin, formatHint, filename,
					complain, relative, option ) )
	    { /*SDEB(filename);*/ rval= -1; goto ready;	}

	if  ( digest )
	    { utilMD5ToBase64( et->etBaseMD5Digest64, digest );	}
	}

    tedSetDocumentDefaults( td, ea );

    {
    DocumentProperties *	dp= td->tdDocument->bdProperties;

    /*  3  */
    if  ( ea->eaAvoidFontconfigInt <= 0		&&
	  ea->eaPreferBase35FontsInt >= 0	)
	{
	if  ( fontAddBase35FontsToDocList( dp->dpFontList ) )
	    { LDEB(35); rval= -1; goto ready; }
	}
    else{
	if  ( fontAddPsFontsToDocList( dp->dpFontList, 
						&(ea->eaPostScriptFontList) ) )
	    {
	    LDEB(ea->eaPostScriptFontList.psflFamilyCount);
	    rval= -1; goto ready;
	    }
	}
    }

    if  ( et->etTraceStatus == TRACING_ACCES )
	{
	int		fdTrace;

	fdTrace= docEditTraceTryTemp( et, TedTraceExtension );

	if  ( fdTrace < 0 && et->etTraceStatus == TRACING_NO )
	    { LCDEB(fdTrace,et->etTraceStatus); rval= -1; goto ready;	}
	}

    if  ( et->etTraceStatus == TRACING_EXIST && *pFormat != TEDdockindTRACE )
	{
	if  ( tedFindBaseVersion( ea, td ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditIsTraced( et->etTraceStatus ) )
	{
	const DocumentProperties *	dp= td->tdDocument->bdProperties;

	if  ( suggestStdin )
	    {
	    if  ( docRtfTraceVersion( EDITcmd_NEW, filename,
					(char *)0, &(dp->dpRevtim),
					et, td->tdDocument ) )
		{ XDEB(digest); rval= -1; goto ready;	}
	    }
	else{
	    if  ( docRtfTraceVersion( EDITcmd_OPEN, filename,
			    et->etBaseMD5Digest64, &(dp->dpRevtim),
			    et, (struct BufferDocument *)0 ) )
		{ XDEB(digest); rval= -1; goto ready;	}
	    }

	et->etBase= et->etIndex- 1;
	td->tdTraced= 1;
	}
    else{ td->tdTraced= 0;	}

  ready:

    if  ( rval )
	{
	docCleanEditTrace( et );
	docInitEditTrace( et );
	}

    utilCleanMemoryBuffer( &ext );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Recover a document.							*/
/*									*/
/************************************************************************/

int tedDocRecover(	EditDocument *		ed )
    {
    int			rval= 0;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    EditTrace *		et= &(td->tdEditTrace);

    const int		restart= 0;
    const int		exclusive= 0;

    SimpleInputStream *	sisTrace= (SimpleInputStream *)0;
    FindDigest		fd;

    tedInitFindDigest( &fd );

    if  ( et->etTraceStatus != TRACING_EXIST )
	{ CDEB(et->etTraceStatus); return -1;	}

    if  ( docEditTraceOpenTrace( et, restart, exclusive ) < 0 )
	{ LLDEB(restart,exclusive); rval= -1; goto ready;	}

    sisTrace= sioInFdOpen( et->etTraceFileHandle );
    if  ( ! sisTrace )
	{ LXDEB(et->etTraceFileHandle,sisTrace); rval= -1; goto ready;	}

    fd.fdEditTrace= et;
    fd.fdApplication= ed->edApplication;

    if  ( docRtfScanEditTrace( sisTrace, tedRemebmerTraceStep, (void *)&fd,
			    (const struct IndexMapping *)0, td->tdDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioInClose( sisTrace ); sisTrace= (SimpleInputStream *)0;

    if  ( fd.fdBase < 0 )
	{ LDEB(fd.fdBase); rval= -1; goto ready;	}

    et->etCount= et->etTraceSteps.plItemCount;
    et->etIndex= et->etBase= fd.fdBase;

    while( et->etIndex < fd.fdLastEdit )
	{
	if  ( tedDocRepeat( ed ) )
	    { LLDEB(et->etIndex,fd.fdLastEdit); rval= -1; goto ready;	}
	}

    td->tdTraced= 1;
    ed->edIsReadonly= 0;

    tedMakeDocumentReadonly( ed, ed->edIsReadonly );
    guiShowMenuOption( td->tdFileUnlockOption, 0 );
    guiShowMenuOption( td->tdFileRecoverOption, 0 );

    tedSetTracedChangedFlag( ed );

    tedAdaptToolsToSelection( ed );

  ready:

    if  ( sisTrace )
	{ sioInClose( sisTrace );	}

    tedCleanFindDigest( &fd );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Unlock a document that cannot be locked.				*/
/*  (Doing so while someone else is editing it is asking for trouble.)	*/
/*									*/
/************************************************************************/

int tedDocUnlock(	EditDocument *		ed )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    EditTrace *		et= &(td->tdEditTrace);

    if  ( et->etTraceStatus == TRACING_EXIST	&&
	  ed->edFormat == TEDdockindTRACE	)
	{
	if  ( fileRemoveFile( &(ed->edFilename) ) )
	    { LDEB(1); return -1;	}

	appCloseDocument( ed );
	return 0;
	}

    if  ( et->etTraceStatus == TRACING_EXIST )
	{
	const int	restart= 1;
	const int	exclusive= 0;

	if  ( docEditTraceOpenTrace( &(td->tdEditTrace), restart, exclusive ) )
	    { LLDEB(restart,exclusive); return -1;	}

	if  ( docRtfTraceVersion( EDITcmd_OPEN, &(ed->edFilename),
			    et->etBaseMD5Digest64,
			    &(td->tdDocument->bdProperties->dpRevtim),
			    et, (struct BufferDocument *)0 ) )
	    { LDEB(1); return -1;	}

	td->tdTraced= 1;
	et->etBase= et->etIndex- 1;
	}
    else{
	if  ( et->etIsRecovery )
	    { et->etIsRecovery= 0;	}
	else{
	    LLDEB(et->etTraceStatus,et->etIsRecovery);
	    return -1;
	    }
	}

    ed->edIsReadonly= 0;

    tedMakeDocumentReadonly( ed, ed->edIsReadonly );
    guiShowMenuOption( td->tdFileUnlockOption, 0 );
    guiShowMenuOption( td->tdFileRecoverOption, 0 );

    tedSetTracedChangedFlag( ed );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a new empty document.						*/
/*									*/
/************************************************************************/

int tedNewDocument(	EditDocument *		ed,
			const MemoryBuffer *	filename )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditTrace *			et= &(td->tdEditTrace);
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    int				traced;

    int				fdTrace= FDerrOTHER;

    MemoryBuffer		ext;
    MemoryBuffer		traceFileName;

    if  ( filename && utilMemoryBufferIsEmpty( filename ) )
	{ filename= (const MemoryBuffer *)0;	}

    utilInitMemoryBuffer( &ext );
    utilInitMemoryBuffer( &traceFileName );

    tedDetermineDefaultSettings( tar );

    traced= tar->tarTraceEditsInt > 0;

    if  ( tedSetDocumentDefaults( td, ea ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bd= tedTryHomeTemplate( ea );
    if  ( ! bd )
	{
	bd= tedMakeDocument( ea );
	if  ( ! bd )
	    { XDEB(bd); rval= -1; goto ready;	}
	}

    ed->edFormat= TEDdockindRTF; /* rtf */
    td->tdDocument= bd;

    if  ( filename )
	{
	if  ( fileGetFileExtension( &ext, filename ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilMemoryBufferEqualsString( &ext, "rtf" ) )
	    { ed->edFormat= TEDdockindRTF;	}
	if  ( utilMemoryBufferEqualsString( &ext, "txt" ) )
	    { ed->edFormat= TEDdockindTEXT_SAVE_FOLDED;	}

	if  ( traced )
	    {
	    fdTrace= docEditTraceTryRelative( et, filename, TedTraceExtension );

	    if  ( fdTrace < 0 )
		{
		if  ( fdTrace == FDerrEXIST )
		    {
		    int		traceFormat;

		    if  ( td->tdDocument )
			{
			docFreeDocument( td->tdDocument );
			td->tdDocument= (struct BufferDocument *)0;
			}

		    if  ( docEditTraceTraceFileName( &traceFileName,
						filename, TedTraceExtension ) )
			{ LDEB(1); goto ready;	}

		    if  ( tedOpenTraceFile( ea, td, &traceFormat,
							    &traceFileName ) )
			{ LDEB(1); rval= -1; goto ready;	}
		    }
		else{
		    LLDEB(fdTrace,FDerrEXIST); rval= -1; goto ready;
		    }
		}
	    }
	}
    else{
	if  ( traced )
	    {
	    fdTrace= docEditTraceTryAnon( et, TedTraceExtension );
	    if  ( fdTrace < 0 )
		{ LDEB(fdTrace); rval= -1; goto ready;	}
	    }
	}

    if  ( docEditIsTraced( et->etTraceStatus ) )
	{
	const DocumentProperties *	dp= td->tdDocument->bdProperties;

	if  ( docRtfTraceVersion( EDITcmd_NEW, filename,
					    (char *)0, &(dp->dpRevtim),
					    et, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	et->etBase= et->etIndex- 1;
	td->tdTraced= 1;
	}
    else{ td->tdTraced= 0;	}

  ready:

    utilCleanMemoryBuffer( &ext );
    utilCleanMemoryBuffer( &traceFileName );

    return rval;
    }

