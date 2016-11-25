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

#   include	"tedApp.h"

#   include	<psTtf.h>
#   include	<psBuildConfigFiles.h>
#   include	<sioFileio.h>
#   include	<sioStdin.h>
#   include	<sioStdout.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Miscelaneous file conversion calls.					*/
/*									*/
/************************************************************************/

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

    if  ( ! strcmp( nameOut, "-" ) )
	{
	sosOut= sioOutStdoutOpen();
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut); rval= -1; goto ready;	}
	}
    else{
	sosOut= sioOutFileioOpenS( nameOut );
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut); rval= -1; goto ready;	}
	}

    if  ( (*cvf)( ea, sosOut, nameIn, sisIn ) )
	{ SSDEB(nameOut,nameIn); rval= -1; goto ready;	}

  ready:

    if  ( sosOut )
	{ sioOutClose( sosOut );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

static int ted_TtfToAfm(	EditApplication *	ea,
				SimpleOutputStream *	sosOut,
				const char *		nameIn,
				SimpleInputStream *	sisIn )
    {
    return psTtfToAfm( sosOut, ea->eaUseKerning < 0, nameIn, sisIn );
    }

int tedTtfToAfm(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); return 1;	}

    if  ( tedFileConvert( ea, argv[0], argv[1], ted_TtfToAfm ) )
	{ SSSDEB(call,argv[0],argv[1]); return -1;	}

    return 2;
    }

static int ted_AfmToGSFontmap(	EditApplication *	ea,
				SimpleOutputStream *	sosOut,
				const char *		nameIn,
				SimpleInputStream *	sisIn )
    {
    return psAfmToGSFontmap( sosOut, nameIn, sisIn );
    }

int tedAfmToGSFontmap(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); return 1;	}

    if  ( tedFileConvert( ea, argv[0], argv[1], ted_AfmToGSFontmap ) )
	{ SSSDEB(call,argv[0],argv[1]); return -1;	}

    return 2;
    }
