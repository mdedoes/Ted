/************************************************************************/
/*									*/
/*  Application, Invoke special calls.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"appEditApplication.h"
#   include	"appPrintDocument.h"
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/

static int app_version(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaNameAndVersion );
    return 0;
    }

static int app_platform(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaPlatformCompiled );
    return 0;
    }

static int app_build(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaHostDateCompiled );
    return 0;
    }

static int app_fullVersion(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s %s %s %s\n",
		    ea->eaNameAndVersion,
		    ea->eaOptionalComponents,
		    ea->eaPlatformCompiled,
		    ea->eaHostDateCompiled );

    return 0;
    }

static int app_saveTo(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    int		rval= 2;

    MemoryBuffer	a0;
    MemoryBuffer	a1;

    utilInitMemoryBuffer( &a0 );
    utilInitMemoryBuffer( &a1 );

    if  ( argc < 2 )
	{ SLDEB(call,argc); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &a0, argv[0] ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &a1, argv[1] ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appFileConvert( ea, &a0, &a1 ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &a0 );
    utilCleanMemoryBuffer( &a1 );

    return rval;
    }

static int app_printToFile(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToFile( ea, argv[0], argv[1], (const char *)0 ) )
	{ SSSDEB(call,argv[0],argv[1]); return -1; }

    return 2;
    }

static int app_printToFilePaper( EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 3 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToFile( ea, argv[0], argv[1], argv[2] ) )
	{ SSSDEB(call,argv[0],argv[2]); return -1; }

    return 3;
    }

static int app_print( 		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], (const char *)0, (const char *)0 ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 1;
    }

static int app_printPaper( 	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], (const char *)0, argv[1] ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 2;
    }

static int app_printToPrinter( 	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], argv[1], (const char *)0 ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 2;
    }

static int app_printToPrinterPaper(
				EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 3 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], argv[1], argv[2] ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 3;
    }

static const SpecialCall AppSpecialCalls[]=
{
    { "version",		app_version,			},
    { "platform",		app_platform,			},
    { "build",			app_build,			},
    { "fullVersion",		app_fullVersion,		},
    { "saveTo",			app_saveTo,			},
    { "printToFile",		app_printToFile,		},
    { "printToFilePaper",	app_printToFilePaper,		},
    { "print",			app_print,			},
    { "printPaper",		app_printPaper,			},
    { "printToPrinter",		app_printToPrinter,		},
    { "printToPrinterPaper",	app_printToPrinterPaper,	},
};

static const int AppSpecialCallCount= sizeof(AppSpecialCalls)/sizeof(SpecialCall);

/************************************************************************/
/*									*/
/*  Handle Special Calls.						*/
/*									*/
/*  1)  It is not necessary to initialize TedApplication to convert	*/
/*	files. Intercept conversion calls to prevent the program from	*/
/*	becoming interactive. NOTE that the calls differ from the calls	*/
/*	that initialize the application: They begin with -- instead of	*/
/*	++ . To override configurable options, the ++Something calls	*/
/*	should be used.							*/
/*									*/
/************************************************************************/

static const SpecialCall * appGetSpecialCall(
				const SpecialCall *	calls,
				int			callCount,
				const char *		call )
    {
    int		c;

    for ( c= 0; c < callCount; calls++, c++ )
	{
	if  ( ! strcmp( call, calls->scCall ) )
	    { return calls;	}
	}

    return (SpecialCall *)0;
    }

int appMainHandleSpecialCalls(	EditApplication *	ea,
				const char *		prefix,
				int			getResources,
				const char *		prog,
				int			argc,
				char *			argv[] )
    {
    int			prefixLength= strlen( prefix );
    int			done= 0;

    if  ( argc < 1 )
	{ return 0;	}

    while( done < argc )
	{
	const char *		call;
	const SpecialCall *	sc;
	int			args;
	int			off;

	if  ( strncmp( argv[done], prefix, prefixLength ) )
	    { break;	}

	call= argv[done]+ prefixLength;
	sc= appGetSpecialCall( ea->eaSpecialCalls,
					    ea->eaSpecialCallCount, call );
	if  ( ! sc )
	    {
	    sc= appGetSpecialCall( AppSpecialCalls,
					    AppSpecialCallCount, call );
	    }
	if  ( ! sc )
	    { break;	}

	if  ( getResources )
	    {
	    appGetApplicationResourceValues( ea );
	    getResources= 0;
	    }

	off= done+ 1;
	args= (*sc->scExecuteCall)( ea, prog, call, argc- off, argv+ off );
	if  ( args < 0 )
	    { SLDEB(argv[1+done],args); return -1;	}
	done += 1+ args;
	}

    return done;
    }

