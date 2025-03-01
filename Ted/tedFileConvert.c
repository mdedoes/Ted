/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>
#   include	<ctype.h>

#   include	<psBuildConfigFiles.h>
#   include	<sioFileio.h>
#   include	<sioStdout.h>
#   include	<drawMatchFont.h>
#   include	<appSystem.h>
#   include	<docCopyNode.h>
#   include	<docFontsDocuments.h>
#   include	<docJsonReport.h>
#   include	"tedFileConvert.h"
#   include	"tedDocument.h"
#   include	<appEditApplication.h>
#   include	<sioGeneral.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Miscelaneous file conversion calls.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * tedFileConvertOpenOut(
					    const char *	nameOut )
    {
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    if  ( ! strcmp( nameOut, "-" ) )
	{
	sosOut= sioOutStdoutOpen();
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut);	}
	}
    else{
	sosOut= sioOutFileioOpenS( nameOut );
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut);	}
	}

    return sosOut;
    }

# if 0

static int tedFileConvert(
		    EditApplication *	ea,
		    const char *	nameIn,
		    const char *	nameOut,
		    int (*cvf)(	EditApplication *	ea,
				SimpleOutputStream *	_sosOut,
				const char *		_nameIn,
				SimpleInputStream *	_sisIn ) )
    {
    int				rval= 0;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;
    SimpleInputStream *		sisIn= (SimpleInputStream *)0;

    if  ( ! strcmp( nameIn, "-" ) )
	{
	sisIn= sioInStdinOpen();
	if  ( ! sisIn )
	    { SXDEB(nameIn,sisIn); rval= -1; goto ready;	}
	}
    else{
	sisIn= sioInFileioOpenS( nameIn );
	if  ( ! sisIn )
	    { SXDEB(nameIn,sisIn); rval= -1; goto ready;	}
	}

    sosOut= tedFileConvertOpenOut( nameOut );
    if  ( ! sosOut )
	{ SXDEB(nameOut,sosOut); rval= -1; goto ready;	}

    if  ( (*cvf)( ea, sosOut, nameIn, sisIn ) )
	{ SSDEB(nameOut,nameIn); rval= -1; goto ready;	}

  ready:

    if  ( sosOut )
	{ sioOutClose( sosOut );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

# endif

int tedTtfToAfm(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    int				rval= 2;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    MemoryBuffer		psDir;

    utilInitMemoryBuffer( &psDir );

    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); rval= 1; goto ready;	}

    if  ( utilMemoryBufferSetString( &psDir, PSSCRIPT_DIR ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sosOut= tedFileConvertOpenOut( argv[1] );
    if  ( ! sosOut )
	{ SXDEB(argv[1],sosOut); rval= -1; goto ready;	}

    if  ( psFontFileToAfm( sosOut, ea->eaUseKerningInt < 0, argv[0], &psDir ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &psDir );

    if  ( sosOut )
	{ sioOutClose( sosOut );	}

    return rval;
    }

int tedAfmToGSFontmap(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    int				rval= 2;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); rval= 1; goto ready;	}

    sosOut= tedFileConvertOpenOut( argv[1] );
    if  ( ! sosOut )
	{ SXDEB(argv[1],sosOut); rval= -1; goto ready;	}

    if  ( psAfmToGSFontmap( sosOut, argv[0] ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready;	}

  ready:

    if  ( sosOut )
	{ sioOutClose( sosOut );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build an afm directory file for a particular font file.		*/
/*									*/
/************************************************************************/

int tedAfmForFontFiles(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    int			ret= argc;

    PostScriptFontList	psfl;
    MemoryBuffer	afmDir;
    MemoryBuffer	psDir;

    utilInitMemoryBuffer( &afmDir );
    utilInitMemoryBuffer( &psDir );

    psInitPostScriptFontList( &psfl );

    if  ( ! ea->eaAfmDirectory )
	{ XDEB(ea->eaAfmDirectory); ret= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &afmDir, ea->eaAfmDirectory ) )
	{ LDEB(1); ret= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &psDir, PSSCRIPT_DIR ) )
	{ LDEB(1); ret= -1; goto ready;	}

    if  ( psAfmForFontFiles( &psfl, ea->eaUseKerningInt < 0, argc, argv,
							&afmDir, &psDir ) )
	{
	SSSDEB(call,ea->eaAfmDirectory,PSSCRIPT_DIR);
	ret= -1; goto ready;
	}

#   if 0 && FONTS_X11
    if  ( ea->eaToplevel.atTopWidget				&&
	  appFindX11Fonts( ea->eaToplevel.atTopWidget, &psfl )	)
	{ XDEB(ea->eaToplevel.atTopWidget); ret= -1; goto ready;	}
#   endif

    if  ( psSaveAfms( &psfl, ea->eaUseKerningInt < 0, &afmDir ) )
	{ SDEB(ea->eaAfmDirectory); ret= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &afmDir );
    utilCleanMemoryBuffer( &psDir );

    psCleanPostScriptFontList( &psfl );

    return ret;
    }

int tedFontsDocuments(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    int				rval= 1;

    MemoryBuffer		templDir;
    MemoryBuffer		outDir;
    MemoryBuffer		scriptRelative;
    MemoryBuffer		scriptAbsoluteFrom;
    MemoryBuffer		scriptAbsoluteTo;

    const int			relativeIsFile= 0;

    utilInitMemoryBuffer( &templDir );
    utilInitMemoryBuffer( &outDir );
    utilInitMemoryBuffer( &scriptRelative );
    utilInitMemoryBuffer( &scriptAbsoluteFrom );
    utilInitMemoryBuffer( &scriptAbsoluteTo );

    if  ( argc < 1 )
	{ LDEB(argc); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &templDir, FONT_TPL_DIR ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &outDir, argv[0] ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &scriptRelative, "checkfontsdocs.sh" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( fileAbsoluteName( &scriptAbsoluteFrom,
			    &scriptRelative, relativeIsFile, &templDir ) < 0 )
	{ SDEB(utilMemoryBufferGetString(&templDir)); rval= -1; goto ready; }
    if  ( fileAbsoluteName( &scriptAbsoluteTo,
			    &scriptRelative, relativeIsFile, &outDir ) < 0 )
	{ SDEB(utilMemoryBufferGetString(&outDir)); rval= -1; goto ready; }

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); rval= -1; goto ready;	}

    if  ( drawGetDeferredFontMetricsForList( &(ea->eaPostScriptFontList) ) )
	{ SDEB(ea->eaAfmDirectory); rval= -1; /*goto ready;*/	}

    if  ( fileTestDirectory( &outDir )		&&
	  fileMakeDirectories( &outDir )	)
	{ SDEB(utilMemoryBufferGetString(&outDir)); rval= -1; goto ready; }

    if  ( appCopyFile( &scriptAbsoluteTo, &scriptAbsoluteFrom ) )
	{ SDEB(argv[0]); rval= -1; goto ready;	}

    if  ( docFontsDocuments( &(ea->eaPostScriptFontList), ea->eaLocale,
						    &templDir, &outDir ) )
	{ SDEB(argv[0]); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &templDir );
    utilCleanMemoryBuffer( &outDir );
    utilCleanMemoryBuffer( &scriptRelative );
    utilCleanMemoryBuffer( &scriptAbsoluteFrom );
    utilCleanMemoryBuffer( &scriptAbsoluteTo );

    return rval;
    }

static int tedFileConvertSaveOutput(
				MemoryBuffer *			fileName,
				EditApplication *		ea,
				TedDocument *			tdOut,
				const char *			outName,
				int				formatOut )
    {
    if  ( utilMemoryBufferSetString( fileName, outName ) )
	{ SDEB(outName); return -1;	}

    if  ( appSaveDocumentOutput( ea, tdOut, formatOut, fileName ) )
	{ SDEB(outName); return -1;	}

    return 0;
    }

static int tedConcatenateX(	EditApplication *		ea,
				int				argc,
				char **				argv,
				int				omitSectBreak )
    {
    int			rval= argc;
    int			arg;

    int			stdinCount= 0;

    TedDocument *	tdIn= (TedDocument *)0;
    TedDocument *	tdOut= (TedDocument *)0;
    int			formatIn= -1;
    int			formatOut= -1;

    MemoryBuffer	fileName;

    utilInitMemoryBuffer( &fileName );

    if  ( argc < 2 )
	{ LLDEB(2,argc); rval= -1; goto ready;	}

    for ( arg= 0; arg < argc- 1; arg++ )
	{
	int		suggestStdin= 0;

	if  ( utilMemoryBufferSetString( &fileName, argv[arg] ) )
	    { LSDEB(arg,argv[arg]); rval= -1; goto ready;	}

	tdIn= (TedDocument *)appOpenDocumentInput( ea, &formatIn,
				    &suggestStdin, stdinCount > 0, &fileName );
	if  ( ! tdIn )
	    { SXDEB(argv[arg],tdIn); rval= -1; goto ready;	}

	if  ( suggestStdin )
	    { stdinCount++;	}

	if  ( ! tdOut )
	    { tdOut= tdIn; formatOut= formatIn;	}
	else{
	    if  ( docAppendDocument( tdOut->tdDocument,
					    tdIn->tdDocument, omitSectBreak ) )
		{ SDEB(argv[arg]); rval= -1; goto ready;	}

	    tedFreeDocument( tdIn, formatIn );
	    }

	tdIn= (TedDocument *)0; formatIn= -1;
	}

    if  ( tedFileConvertSaveOutput( &fileName, ea, tdOut,
						    argv[arg], formatOut ) )
	{ LSDEB(arg,argv[arg]); rval= -1; goto ready;	}

  ready:

    if  ( tdIn )
	{ tedFreeDocument( tdIn, formatIn );	}
    if  ( tdOut )
	{ tedFreeDocument( tdOut, formatOut );	}

    utilCleanMemoryBuffer( &fileName );

    return rval;
    }

int tedConcatenate(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    const int	omitSectBreak= 0;

    return tedConcatenateX( ea, argc, argv, omitSectBreak );
    }

int tedConcatenateText(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    const int	omitSectBreak= 1;

    return tedConcatenateX( ea, argc, argv, omitSectBreak );
    }

int tedMergeJson(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    int			rval= argc;

    TedDocument *	tdIn= (TedDocument *)0;
    TedDocument *	tdOut= (TedDocument *)0;
    int			formatIn= -1;
    int			formatOut= -1;

    MemoryBuffer	fileName;

    int			stdinCount= 0;
    int			suggestStdin= 0;

    utilInitMemoryBuffer( &fileName );

    if  ( argc != 3 )
	{ SLDEB(call,argc); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &fileName, argv[0] ) )
	{ SDEB(argv[0]); rval= -1; goto ready;	}

    tdIn= (TedDocument *)appOpenDocumentInput( ea, &formatIn,
				    &suggestStdin, stdinCount > 0, &fileName );
    if  ( ! tdIn )
	{ SXDEB(argv[0],tdIn); rval= -1; goto ready;	}

    if  ( suggestStdin )
	{ stdinCount++;	}

    tdOut= tdIn; formatOut= formatIn;
    tdIn= (TedDocument *)0; formatIn= -1;

    if  ( docSubstituteJsonProperties( argv[1], ea->eaLocale,
							tdOut->tdDocument ) )
	{ SSDEB(argv[0],argv[1]); rval= -1; goto ready;	}

    if  ( tedFileConvertSaveOutput( &fileName, ea, tdOut,
						    argv[2], formatOut ) )
	{ SDEB(argv[2]); rval= -1; goto ready;	}

  ready:

    if  ( tdIn )
	{ tedFreeDocument( tdIn, formatIn );	}
    if  ( tdOut )
	{ tedFreeDocument( tdOut, formatOut );	}

    utilCleanMemoryBuffer( &fileName );

    return rval;
    }

