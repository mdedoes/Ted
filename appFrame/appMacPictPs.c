#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMacPict.h>
#   include	<utilPs.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

# if 0
#    define	PICTLOG(x)	(x)
# else
#    define	PICTLOG(x)	/*nothing*/
# endif

int appMacPictPlayFilePs( SimpleOutputStream *		sos,
			SimpleInputStream *		sis,
			const char *			afmDirectory,
			int				useFilters,
			int				indexedImages,
			int				mapMode,
			int				xWinExt,
			int				yWinExt,
			int				twipsWide,
			int				twipsHigh )
    {
LDEB(0); return 0;
    }

/************************************************************************/
/*									*/
/*  List the fonts in a windows metafile.				*/
/*									*/
/*  The geometry parameters are absolutely irrelevant, but required by	*/
/*  appMetaInitDeviceContext().						*/
/*									*/
/************************************************************************/

int appMacPictListFontsPs( PostScriptFaceList *	psfl,
			SimpleInputStream *	sis,
			const char *		afmDirectory,
			const char *		prefix,
			int			mapMode,
			int			xWinExt,
			int			yWinExt,
			int			twipsWide,
			int			twipsHigh )
    {
LDEB(0); return 0;
    }
