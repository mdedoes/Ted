/************************************************************************/
/*									*/
/*  Utility routines to build font configuration.			*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<locale.h>

#   include	<reg.h>

#   include	<sioFileio.h>
#   include	<sioPfb.h>
#   include	<sioPipe.h>
#   include	<appSystem.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilMemoryBufferPrintf.h>
#   include	"psTtf.h"
#   include	"psBuildConfigFiles.h"
#   include	"psReadWriteFontInfo.h"

#   include	<appDebugon.h>

#   ifdef __VMS
    static const char * const		afmExtension= "AFM";
    static const char * const		pfbExtension= "PFB";
    static const char * const		pfaExtension= "PFA";
#   else
    static const char * const		afmExtension= "afm";
    static const char * const		pfbExtension= "pfb";
    static const char * const		pfaExtension= "pfa";
#   endif
    static const char * const		ttfExtension= "ttf";
    static const char * const		TTFExtension= "TTF";

static const char * psFontNamePattern=
			"^/FontName[ \t]*/([^ \t]*)[ \t][ \t]*def";

static const char * PS_LocalAfmDir= "localfonts";

/************************************************************************/
/*									*/
/*  Find the font name for a Postcript type 1 font.			*/
/*									*/
/************************************************************************/

static char * psFontNameFromPf(	SimpleInputStream *	sisFont,
				regProg *		prog )
    {
    char *		rval= (char *)0;
    char		scratch[550+1];

    while( sioInGetString( scratch, sizeof(scratch)-1, sisFont ) )
	{
	ExpressionMatch		em;

	scratch[sizeof(scratch)-1]= '\0';

	if  ( regFindLeftToRight( &em, prog, scratch, 0, strlen( scratch ) ) )
	    {
	    int		from;
	    int		past;
	    int		l;

	    regGetMatch( &from, &past, &em, 0 );

	    l= past- from;

	    rval= (char *)malloc( l+ 1 );
	    if  ( ! rval )
		{ LXDEB(l,rval);					}
	    else{ strncpy( rval, scratch+ from, l )[l]= '\0'; }

	    break;
	    }
	}

    if  ( ! rval )
	{ XDEB(rval);	}

    return rval;
    }

static int psFillAfiForPf(	AfmFontInfo *		afi,
				int			afmFlags,
				SimpleInputStream *	sisFont,
				regProg *		prog,
				const MemoryBuffer *	filename,
				const char *		afmCommand )
    {
    int			rval= 0;
    char *		fontname= (char *)0;

    MemoryBuffer	command;

    SimpleInputStream *	sisCmd= (SimpleInputStream *)0;

    utilInitMemoryBuffer( &command );

    fontname= psFontNameFromPf( sisFont, prog );
    if  ( ! fontname )
	{ XDEB(fontname); rval= -1; goto ready;	}

    if  ( strlen( afmCommand )+ strlen( fontname ) > sizeof(command)- 1 )
	{ SDEB(fontname); rval= -1; goto ready;	}

    utilMemoryBufferPrintf( &command, afmCommand, fontname );

    sisCmd= sioInPipeOpen( &command );
    if  ( ! sisCmd )
	{ SXDEB(afmCommand,sisCmd); rval= -1; goto ready;	}

    rval= psAfmReadAfm( sisCmd, afi, afmFlags );
    if  ( rval  )
	{ SLDEB(afmCommand,rval); rval= -1; }

    if  ( utilCopyMemoryBuffer( &(afi->afiFontFileName), filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( psGetUnicodesFromGlyphNames( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psGetAlternateGlyphs( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psResolveFallbackGlyph( afi ) )
	{ SDEB(afi->afiFullName);	}

  ready:

    utilCleanMemoryBuffer( &command );

    if  ( sisCmd )
	{ sioInClose( sisCmd );	}
    if  ( fontname )
	{ free( fontname );	}

    return rval;
    }

static char * psFontNameFromPfa(	regProg *		prog,
					const MemoryBuffer *	filename )
    {
    char *		rval= (char *)0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); goto ready;	}

    rval= psFontNameFromPf( sisFile, prog );
    if  ( ! rval )
	{ XDEB(rval);		}

  ready:
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static int psFillAfiForPfa(	AfmFontInfo *		afi,
				int			afmFlags,
				regProg *		prog,
				const MemoryBuffer *	filename,
				const char *		afmCommand )
    {
    int			rval= 0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); rval= -1; goto ready;	}

    rval= psFillAfiForPf( afi, afmFlags, sisFile, prog, filename, afmCommand );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static char * psFontNameFromPfb(	regProg *		prog,
					const MemoryBuffer *	filename )
    {
    char *		rval= (char *)0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisFont= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); goto ready;	}

    sisFont= sioInPfbOpen( sisFile );
    if  ( ! sisFont )
	{ XDEB(sisFont); goto ready;	}

    rval= psFontNameFromPf( sisFont, prog );
    if  ( ! rval )
	{ XDEB(rval);		}

  ready:
    if  ( sisFont )
	{ sioInClose( sisFont );	}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static int psFillAfiForPfb(	AfmFontInfo *		afi,
				int			afmFlags,
				regProg *		prog,
				const MemoryBuffer *	filename,
				const char *		afmCommand )
    {
    int			rval= 0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisFont= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); rval= -1; goto ready;	}

    sisFont= sioInPfbOpen( sisFile );
    if  ( ! sisFont )
	{ XDEB(sisFont); rval= -1; goto ready;	}

    rval= psFillAfiForPf( afi, afmFlags, sisFont, prog, filename, afmCommand );
    if  ( rval )
	{ LDEB(rval);		}

  ready:
    if  ( sisFont )
	{ sioInClose( sisFont );	}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the font name for a TTF file. This is an expensive way. That	*/
/*  is not a problem as we rarely do this.				*/
/*									*/
/************************************************************************/

static int psFillAfiForTtf(	AfmFontInfo *		afi,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); rval= -1; goto ready;	}

    if  ( psTtfToAfi( afi, filename, sisFile ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static char * psFontNameFromTtf(	const MemoryBuffer *	filename )
    {
    char *		rval= (char *)0;
    AfmFontInfo		afi;

    psInitAfmFontInfo( &afi );

    if  ( psFillAfiForTtf( &afi, filename ) )
	{ LDEB(1); goto ready;	}

    rval= strdup( afi.afiFontName );
    if  ( ! rval )
	{ XDEB(rval);	}

  ready:
    psCleanAfmFontInfo( &afi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Scan a list of files and issue GhostScript compatible Fontmap lines	*/
/*  that can be added to a GhostScript Fontmap file. The purpose of	*/
/*  this function is to make the fonts known to GhostScript. This means	*/
/*  that we cannot use GhostScript yet.					*/
/*									*/
/************************************************************************/

int psFontmapForFiles(		SimpleOutputStream *	sosOut,
				int			fileCount,
				char **			fileNames )
    {
    int			rval= 0;
    int			f;

    regProg *		prog= (regProg *)0;
    char *		fontname= (char *)0;
    const int		options= 0;

    MemoryBuffer	extension;
    MemoryBuffer	filename;

    utilInitMemoryBuffer( &extension );
    utilInitMemoryBuffer( &filename );

    prog= regCompile( psFontNamePattern, options );
    if  ( ! prog )
	{ XDEB(prog); rval= -1; goto ready;	}

    for ( f= 0; f < fileCount; f++ )
	{
	if  ( utilMemoryBufferSetString( &filename, fileNames[f] ) )
	    { SDEB(fileNames[f]); rval= -1; goto ready;	}

	if  ( appFileGetFileExtension( &extension, &filename ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilMemoryBufferIsEmpty( &extension ) )
	    { rval= -1; continue;	}

	if  ( utilMemoryBufferEqualsString( &extension, pfaExtension ) )
	    { fontname= psFontNameFromPfa( prog, &filename );	}

	if  ( utilMemoryBufferEqualsString( &extension, pfbExtension ) )
	    { fontname= psFontNameFromPfb( prog, &filename );	}

	if  ( utilMemoryBufferEqualsString( &extension, ttfExtension ) ||
	      utilMemoryBufferEqualsString( &extension, TTFExtension ) )
	    { fontname= psFontNameFromTtf( &filename );		}

	if  ( fontname )
	    {
	    sioOutPrintf( sosOut, "/%s (%s) ;\n", fontname, fileNames[f] );
	    free( fontname ); fontname= (char *)0;
	    }
	else{ SDEB(fileNames[f]); rval= -1;	}
	}

  ready:

    if  ( fontname )
	{ free( fontname );	}
    if  ( prog )
	{ free( prog );	}

    utilCleanMemoryBuffer( &extension );
    utilCleanMemoryBuffer( &filename );

    return rval;
    }

/************************************************************************/

typedef struct AfmDirBuilder
    {
    MemoryBuffer		adbAfmDirectory;
    regProg *			adbFontNameProg;
    char			adbAfmCommand[300+1];
    PostScriptFontList *	adbPostScriptFontList;
    int				adbAfmFlags;
    } AfmDirBuilder;

static int psSaveAfmFile(	AfmFontInfo *		afi,
				AfmDirBuilder *		adb )
    {
    int				rval= 0;
    const int			relativeIsFile= 0;
    unsigned char *		s;

    MemoryBuffer		relative;

    utilInitMemoryBuffer( &relative );

    if  ( utilMemoryBufferSetString( &relative, afi->afiFontName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferAppendBytes( &relative,
				    (const unsigned char *)".afm", 4 ) )
	{ LDEB(4); rval= -1; goto ready;	}

    s= relative.mbBytes;
    while( *s )
	{
	if  ( isspace( *s ) )
	    { *s= '-';	}
	s++;
	}
    if  ( appAbsoluteName( &(afi->afiAfmFileName), &relative,
				relativeIsFile, &(adb->adbAfmDirectory) ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( psPostScriptFontListAddInfo( adb->adbPostScriptFontList, afi ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &relative );

    return rval;
    }

static int psAfmFromPfaFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;

    AfmFontInfo *	afi= (AfmFontInfo *)0;

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    if  ( psFillAfiForPfa( afi, adb->adbAfmFlags, adb->adbFontNameProg,
					filename, adb->adbAfmCommand ) )
	{ LDEB(1); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0; /* steal */

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psAfmFromPfbFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;

    AfmFontInfo *	afi= (AfmFontInfo *)0;

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    if  ( psFillAfiForPfb( afi, adb->adbAfmFlags, adb->adbFontNameProg,
					    filename, adb->adbAfmCommand ) )
	{ LDEB(1); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0; /* steal */

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psAfmFromTtfFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;

    AfmFontInfo *	afi= (AfmFontInfo *)0;

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    if  ( psFillAfiForTtf( afi, filename ) )
	{ LDEB(1); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0;

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

/************************************************************************/

static int psStartAfmDirBuilder(	AfmDirBuilder *		adb,
					PostScriptFontList *	psfl,
					int			ignoreKerning,
					const MemoryBuffer *	afmDirectory,
					const MemoryBuffer *	psDirectory )
    {
    int			rval= 0;
    const int		relativeIsFile= 0;
    const int		options= 0;

    const char *	afmBase= "gs -sthefont=%s -q %s/gsfontmetrics.ps -c quit";

    MemoryBuffer	local;

    utilInitMemoryBuffer( &local );

    adb->adbFontNameProg= (regProg *)0;
    adb->adbPostScriptFontList= psfl;
    adb->adbAfmFlags= 0;

    if  ( ignoreKerning )
	{ adb->adbAfmFlags |= PSflagIGNORE_KERNING;	}

    if  ( utilMemoryBufferSetString( &local, PS_LocalAfmDir ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appAbsoluteName( &(adb->adbAfmDirectory),
			&local, relativeIsFile, afmDirectory ) < 0 )
	{ SDEB(PS_LocalAfmDir); rval= -1; goto ready;	}

    adb->adbFontNameProg= regCompile( psFontNamePattern, options );
    if  ( ! adb->adbFontNameProg )
	{ XDEB(adb->adbFontNameProg); rval= -1; goto ready;	}

    if  ( psDirectory->mbSize+ strlen( afmBase ) >
					    sizeof(adb->adbAfmCommand)- 1 )
	{ LDEB(psDirectory->mbSize); rval= -1; goto ready;	}

    sprintf( adb->adbAfmCommand, afmBase, "%s",
				utilMemoryBufferGetString( psDirectory ) );

  ready:

    utilCleanMemoryBuffer( &local );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build an afm directory: Include all fonts that are on the		*/
/*  GhostScript library path.						*/
/*									*/
/************************************************************************/

int psGSLibAfmDirectory(	PostScriptFontList *	psfl,
				int			ignoreKerning,
				const MemoryBuffer *	afmDirectory,
				const MemoryBuffer *	psDirectory )
    {
    int				rval= 0;
    SimpleInputStream *		sisCmd= (SimpleInputStream *)0;

    AfmDirBuilder		adb;
    MemoryBuffer		dir;
    MemoryBuffer		libCommand;

    char			scratch[500+1];

    const char *	libBase= "gs -q %s/gslibpath.ps -c quit";

    utilInitMemoryBuffer( &dir );
    utilInitMemoryBuffer( &libCommand );

    if  ( psStartAfmDirBuilder( &adb, psfl, ignoreKerning,
					    afmDirectory, psDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilMemoryBufferPrintf( &libCommand, libBase,
				utilMemoryBufferGetString( psDirectory ) );

    sisCmd= sioInPipeOpen( &libCommand );
    if  ( ! sisCmd )
	{ SXDEB(libBase,sisCmd); rval= -1; goto ready;	}

    setlocale( LC_NUMERIC, "C" );

    while( sioInGetString( scratch, sizeof(scratch)- 1, sisCmd ) )
	{
	int		l;

	scratch[sizeof(scratch)- 1]= '\0';
	l= strlen( scratch );
	if  ( l < 2 )
	    { LDEB(l); rval= -1; goto ready;	}
	if  ( scratch[l-1] != '\n' )
	    { SDEB(scratch); rval= -1; goto ready;	}
	scratch[l-1]= '\0';

	if  ( utilMemoryBufferSetString( &dir, scratch ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ! appFileNameIsAbsolute( scratch ) )
	    { continue;	}
	if  ( appTestDirectory( &dir ) )
	    { continue;	}

	l= appForAllFiles( &dir, pfaExtension, &adb, psAfmFromPfaFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, pfbExtension, &adb, psAfmFromPfbFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, ttfExtension, &adb, psAfmFromTtfFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, TTFExtension, &adb, psAfmFromTtfFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	}

  ready:

    setlocale( LC_NUMERIC, "" );

    if  ( sisCmd )
	{ sioInClose( sisCmd );	}

    if  ( adb.adbFontNameProg )
	{ regFree( adb.adbFontNameProg );	}

    utilCleanMemoryBuffer( &dir );
    utilCleanMemoryBuffer( &libCommand );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build an afm directory file for a particular font file.		*/
/*									*/
/************************************************************************/

int psAfmForFontFiles(	PostScriptFontList *	psfl,
			int			ignoreKerning,
			int			fileCount,
			char **			fileNames,
			const MemoryBuffer *	afmDirectory,
			const MemoryBuffer *	psDirectory )
    {
    int				rval= 0;
    int				f;

    const int			relativeIsFile= 0;

    AfmDirBuilder		adb;
    MemoryBuffer		pwd;
    MemoryBuffer		filename;
    MemoryBuffer		file;
    MemoryBuffer		extension;

    utilInitMemoryBuffer( &pwd );
    utilInitMemoryBuffer( &filename );
    utilInitMemoryBuffer( &file );
    utilInitMemoryBuffer( &extension );

    if  ( psStartAfmDirBuilder( &adb, psfl, ignoreKerning,
						afmDirectory, psDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appCurrentDirectory( &pwd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    setlocale( LC_NUMERIC, "C" );

    for ( f= 0; f < fileCount; f++ )
	{
	if  ( utilMemoryBufferSetString( &filename, fileNames[f] ) )
	    { SDEB(fileNames[f]); rval= -1; goto ready;	}

	if  ( appFileGetFileExtension( &extension, &filename ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilMemoryBufferIsEmpty( &extension ) )
	    { rval= -1; continue;	}

	if  ( ! appFileNameIsAbsolute( fileNames[f] ) )
	    {
	    int		l;

	    l= appAbsoluteName( &file, &filename, relativeIsFile, &pwd );
	    if  ( l < 0 )
		{ LDEB(l); rval= -1; continue;	}
	    }
	else{
	    if  ( utilCopyMemoryBuffer( &file, &filename ) )
		{ LDEB(1);	}
	    }

	if  ( utilMemoryBufferEqualsString( &extension, pfaExtension ) )
	    {
	    if  ( psAfmFromPfaFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	if  ( utilMemoryBufferEqualsString( &extension, pfbExtension ) )
	    {
	    if  ( psAfmFromPfbFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	if  ( utilMemoryBufferEqualsString( &extension, ttfExtension ) ||
	      utilMemoryBufferEqualsString( &extension, TTFExtension ) )
	    {
	    if  ( psAfmFromTtfFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	SDEB(fileNames[f]); rval= -1; continue;
	}

  ready:

    setlocale( LC_NUMERIC, "" );

    if  ( adb.adbFontNameProg )
	{ regFree( adb.adbFontNameProg );	}

    utilCleanMemoryBuffer( &pwd );
    utilCleanMemoryBuffer( &filename );
    utilCleanMemoryBuffer( &file );
    utilCleanMemoryBuffer( &extension );

    return rval;
    }

int psAfmToGSFontmap(	SimpleOutputStream *	sosFontDir,
			const char *		afmFileName,
			SimpleInputStream *	sisAfm )
    {
    int			rval= 0;
    AfmFontInfo		afi;
    const char *	filename= "FontFileName";

    const int		afmFlags= PSflagDEFER_METRICS | PSflagIGNORE_KERNING;

    psInitAfmFontInfo( &afi );

    setlocale( LC_NUMERIC, "C" );

    if  ( psAfmReadAfm( sisAfm, &afi, afmFlags ) )
	{ SDEB(afmFileName); rval= -1; goto ready;	}

    if  ( ! utilMemoryBufferIsEmpty( &(afi.afiFontFileName) ) )
	{ filename= utilMemoryBufferGetString( &(afi.afiFontFileName) ); }

    sioOutPrintf( sosFontDir, "/%s (%s) ;\n", afi.afiFontName, filename );

  ready:

    setlocale( LC_NUMERIC, "" );

    psCleanAfmFontInfo( &afi );

    return rval;
    }
