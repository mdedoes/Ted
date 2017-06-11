/************************************************************************/
/*									*/
/*  Application, Main module.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<locale.h>
#   include	<signal.h>

#   include	<appSystem.h>
#   include	<utilLocale.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appEditDocument.h"
#   include	"appFileChooser.h"
#   include	"appPaper.h"
#   include	"appGuiResource.h"

#   if USE_MOTIF
#       include	"appAppFront.h"
#   endif

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Use the contents of /etc/papersize as the default papersize.	*/
/*									*/
/************************************************************************/

static void appDefaultPapersize(	EditApplication *	ea )
    {
    const char *	defsz= utilPaperDefaultSize();

    if  ( defsz )
	{
	if  ( appSetSystemProperty( ea, "paper", defsz ) )
	    { SDEB(defsz);	}
	}

    return;
    }

/************************************************************************/

static int appSetCommandLineProperties(	EditApplication *	ea,
					int			argc,
					char *			argv[] )
    {
    int		arg;
    int		argTo= 1;

    for ( arg= 1; arg < argc; arg++ )
	{
	if  ( arg+ 3 <= argc				&&
	      ! strcmp( argv[arg], "--setProperty" )	)
	    {
	    if  ( appSetUserProperty( ea, argv[arg+ 1], argv[arg+ 2] ) )
		{ SSDEB(argv[arg+ 1], argv[arg+ 2]); return -1;	}

	    arg += 2; continue;
	    }

	if  ( ! strncmp( argv[arg], "--", 2 ) )
	    {
	    char *	eq= strchr( argv[arg], '=' );

	    if  ( eq )
		{
		int	res;

		*eq= '\0';
		res= appSetUserProperty( ea, argv[arg]+ 2, eq+ 1 );
		*eq= '=';

		if  ( res )
		    { SDEB(argv[arg]); return -1;	}

		continue;
		}
	    }

	argv[argTo++]= argv[arg];
	}

    return argTo;
    }

/************************************************************************/

static void appConfigureLocale(	EditApplication *	ea )
    {
    static SimpleLocale	sl;

    utilInitSimpleLocale( &sl );
    if  ( utilGetSimpleLocale( &sl ) )
	{ LDEB(1);	}

    /* Does not really make sense: simply interferes with the locale
    if  ( utilSetSimpleLocale( &sl,
			ea->eaDecimalPoint, 
			ea->eaDigitSeparator,
			ea->eaListSeparator ) )
	{ SSSDEB(ea->eaDecimalPoint,ea->eaDigitSeparator,ea->eaListSeparator); }
    */

    ea->eaLocale= &sl;
    }

/************************************************************************/
/*									*/
/*  Generic main() procedure.						*/
/*									*/
/*  1)  Connect to the GUI system.					*/
/*  2)  Retrieve resource values and derive some application wide	*/
/*	settings from them.						*/
/*  3)  Do not die too easily.						*/
/*	And try to be immune for shells that make you sensitive for	*/
/*	signals that an X11 application does not want to know about.	*/
/*									*/
/************************************************************************/

int appMain(	EditApplication *	ea,
		int			argc,
		char *			argv[] )
    {
    int			rval= 0;

    int			arg;
    int			res;
    int			didSpecial= 0;
    char *		prog;
    char *		locale= (char *)0;

    MemoryBuffer	filename;
    MemoryBuffer	absolute;
    MemoryBuffer	ext;

    const char *	overrideProperties= (const char *)0;

    utilInitMemoryBuffer( &filename );
    utilInitMemoryBuffer( &absolute );
    utilInitMemoryBuffer( &ext );

    locale= setlocale( LC_ALL, "" );
    locale= setlocale( LC_MESSAGES, "" );
    if  ( locale )
	{
	char *	s= locale;
	while( isalpha( *s ) || *s == '_' )
	    { s++;	}

	if  ( s > locale )
	    {
	    int		len= s- locale;
	    char *	localeTag;

	    localeTag= malloc( len+ 1 );
	    if  ( ! localeTag )
		{ LXDEB(len,localeTag); return -1;	}

	    strncpy( localeTag, locale, len )[len]= '\0';

	    ea->eaLocaleTag= localeTag;
	    }
	}

    appDefaultPapersize( ea );

    res= appReadUserProperties( ea );
    if  ( res )
	{ LDEB(res); rval= 1; goto ready;	}

    res= appReadSystemProperties( ea );
    if  ( res )
	{ LDEB(res); rval= 1; goto ready;	}

    res= appSetCommandLineProperties( ea, argc, argv );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    argc= res;

    overrideProperties= appGetProperty( ea, "overrideProperties" );
    if  ( overrideProperties )
	{
	if  ( utilMemoryBufferSetString( &filename, overrideProperties ) )
	    { LDEB(1); rval= 1; goto ready;	}

	if  ( appReadOverrideProperties( ea, &filename ) )
	    { SDEB(overrideProperties); rval= 1; goto ready;	}
	}

    geoInitDocumentGeometry( &(ea->eaDefaultDocumentGeometry) );

    /*  b  */
    appGetApplicationResourceValues( ea );

    appConfigureLocale( ea );

    prog= argv[0];
    res= appMainHandleSpecialCalls( ea, "--", 1, prog, argc- 1, argv+ 1 );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    if  ( res > 0 )
	{ didSpecial= 1; argc -= res; argv += res; argv[0]= prog;	}

    if  ( argc <= 1 && didSpecial )
	{ goto ready;	}

    /*  1  */
    if  ( appGuiInitApplication( ea, &argc, &argv ) )
	{ LDEB(1); rval= 1; goto ready;	}

#   if ! USE_HEADLESS

    if  ( ea->eaToplevel.atTopWidget )
	{ appGetPixelsPerTwip( ea );	}

    if  ( appFinishApplicationWindow( ea ) )
	{ LDEB(1); rval= -1; goto ready;	}

    appAllocateCopyPasteTargetAtoms( ea );

#   if USE_MOTIF
    if  ( ea->eaAppSelectionTypeCount > 0 )
	{
	XtAddEventHandler( ea->eaToplevel.atTopWidget,
			PropertyChangeMask, True, appAppGotPasteCall, ea );
	}
#   endif
#   endif

    res= appMainHandleSpecialCalls( ea, "++", 0, prog, argc- 1, argv+ 1 );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    if  ( res > 0 )
	{ didSpecial= 1; argc -= res; argv += res; argv[0]= prog;	}

    if  ( argc <= 1 && didSpecial )
	{ goto ready;	}

    /*  3  */
    (void) signal( SIGHUP, SIG_IGN );
    (void) signal( SIGINT, SIG_IGN );
#   ifdef SIGWINCH
    (void) signal( SIGWINCH, SIG_IGN );
#   endif

    for ( arg= 1; arg < argc; arg++ )
	{
	if  ( utilMemoryBufferSetString( &filename, argv[arg] ) )
	    { LDEB(1); rval= 1; goto ready;	}

#	if ! USE_HEADLESS
	if  ( ! fileTestDirectory( &filename ) )
	    {
	    appRunOpenChooser( (APP_WIDGET)0, ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, &filename,
			appChooserOpenDocument, ea, (void *)ea );

	    continue;
	    }
#	endif

	if  ( appMainOpenDocument( ea, argc > 2, &filename ) )
	    { SDEB(argv[arg]); rval= -1;	}
	}

#   if ! USE_HEADLESS
    if  ( ea->eaVisibleDocumentCount > 0 )
	{ guiHideShellWidget( ea->eaToplevel.atTopWidget );	}
    else{
	if  ( argc > 1 )
	    { goto ready;	}

	appShowShellWidget( ea, ea->eaToplevel.atTopWidget );
	}
#   endif

#   if USE_MOTIF
    XtAppMainLoop( ea->eaContext );
#   endif

#   if USE_GTK
    gtk_main();
#   endif

  ready:

    utilCleanMemoryBuffer( &filename );
    utilCleanMemoryBuffer( &absolute );
    utilCleanMemoryBuffer( &ext );

    return rval;
    }
