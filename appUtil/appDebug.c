/************************************************************************/
/*									*/
/*  Debugging output.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdarg.h>
#   include	<appDebug.h>

static FILE *	APP_DebugFile;
const int appDebugZero = 0;

int appDebug(	const char *	format,
		... )
    {
    va_list	ap;
    int		ret;

    FILE *	debugFile= APP_DebugFile;

    if  ( ! debugFile )
	{ debugFile= stderr;	}

    va_start( ap, format );

    ret= vfprintf( debugFile, format, ap );

    va_end( ap );

    fflush( debugFile );

    return ret;
    }

int appDebugSetFile(	const char *	filename,
			const char *	mode )
    {
    FILE *	debugFile= (FILE *)0;

    if  ( filename )
	{
	debugFile= fopen( filename, mode );

	if  ( ! debugFile )
	    { SSXDEB(filename,mode,debugFile); return -1;	}
	}

    if  ( APP_DebugFile )
	{ fclose( APP_DebugFile ); APP_DebugFile= (FILE *)0;	}

    APP_DebugFile= debugFile;

    return 0;
    }

# ifdef __GNUC__

#   include <execinfo.h>
#   include <stdlib.h>

void appDebugStackTrace(	const char *	file,
				int		line )
    {
    void *	frames[30];
    size_t	count;
    char **	names;
    int		frame;

    count = backtrace (frames, sizeof(frames)/sizeof(void *));
    names = backtrace_symbols (frames, count);

    appDebug( "from %s(%d) %d frames:\n", file, line, (int)count );

    for ( frame = 0; frame < count; frame++ )
	{ appDebug( "%6d: %s(..)\n", frame, names[frame] );	}

    free (names);
    }

# else

void appDebugStackTrace(	const char *	file,
				int		line )
    {
    appDebug( "from %s(%d) Can only print stack with GCC!\n", file, line );
    }

# endif
