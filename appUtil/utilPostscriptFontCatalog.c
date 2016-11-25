#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<locale.h>

#   include	"docFont.h"
#   include	"psFont.h"
#   include	"appSystem.h"

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  A primitive table driven parser for AFM files.			*/
/*									*/
/************************************************************************/

#   define	AFMlenLINE	256

typedef struct AfmKeyword
    {
    char *	akString;
    int		(*akConsumer)(	FILE *			f,
				int			valPos,
				char *			input,
				AfmFontInfo *		afi );
    } AfmKeyword;

static int psAfmConsumeLines(	FILE *			f,
				char *			input,
				AfmFontInfo *		afi,
				AfmKeyword *		ak,
				int			keywordCount );

static int psAfmIgnore(			FILE *, int, char *,AfmFontInfo *);
static int psAfmFontMetrics(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmFontName(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmFullName(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmFamilyName(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmWeight(			FILE *,	int, char *,AfmFontInfo *);
static int psAfmFontBBox(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmEncodingScheme(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmMappingScheme(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmCharacterSet(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmCharacters(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmIsFixedV(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmCapHeight(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmXHeight(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmAscender(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmDescender(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartDirection(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmUnderlinePosition(	FILE *,	int, char *,AfmFontInfo *);
static int psAfmUnderlineThickness(	FILE *,	int, char *,AfmFontInfo *);
static int psAfmItalicAngle(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmCharWidth(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmIsFixedPitch(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartCharMetrics(	FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartKernData(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartComposites(	FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartTrackKern(		FILE *,	int, char *,AfmFontInfo *);
static int psAfmStartKernPairs(		FILE *,	int, char *,AfmFontInfo *);

# define psAfmStdHW psAfmIgnore
# define psAfmStdVW psAfmIgnore

static AfmKeyword	psAfmFileKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartFontMetrics",	psAfmFontMetrics,	},
    };

static AfmKeyword	psAfmMetricsKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "FontName",		psAfmFontName,		},
	{ "FullName",		psAfmFullName,		},
	{ "FamilyName",		psAfmFamilyName,	},
	{ "Weight",		psAfmWeight,		},
	{ "FontBBox",		psAfmFontBBox,		},
	{ "Version",		psAfmIgnore,		},
	{ "Notice",		psAfmIgnore,		},
	{ "EncodingScheme",	psAfmEncodingScheme,	},
	{ "MappingScheme",	psAfmMappingScheme,	},
	{ "EscChar",		psAfmIgnore,		},
	{ "CharacterSet",	psAfmCharacterSet,	},
	{ "Characters",		psAfmCharacters,	},
	{ "IsBaseFont",		psAfmIgnore,		},
	{ "VVector",		psAfmIgnore,		},
	{ "IsFixedV",		psAfmIsFixedV,		},
	{ "CapHeight",		psAfmCapHeight,		},
	{ "XHeight",		psAfmXHeight,		},
	{ "Ascender",		psAfmAscender,		},
	{ "Descender",		psAfmDescender,		},
	{ "StartDirection",	psAfmStartDirection,	},
	{ "UnderlinePosition",	psAfmUnderlinePosition,	},
	{ "UnderlineThickness",	psAfmUnderlineThickness,},
	{ "ItalicAngle",	psAfmItalicAngle,	},
	{ "CharWidth",		psAfmCharWidth,		},
	{ "IsFixedPitch",	psAfmIsFixedPitch,	},
	{ "StartCharMetrics",	psAfmStartCharMetrics,	},
	{ "StartKernData",	psAfmStartKernData,	},
	{ "StartComposites",	psAfmStartComposites,	},
	{ "StdHW",		psAfmStdHW,		},
	{ "StdVW",		psAfmStdVW,		},

	{ "StartKernPairs",	psAfmStartKernPairs,	},
				/****************************************/
				/*  Should not appear directly in the	*/
				/*  file but embedded between		*/
				/*  "StartKernData" and "EndKernData".	*/
				/*  As some messy afm files forget the	*/
				/*  embeding of the kerning data in a	*/
				/*  separate section, the information	*/
				/*  is accepted here as well.		*/
				/****************************************/

	{ "EndFontMetrics",	0,			},
    };

/************************************************************************/
/*									*/
/*  Initialise an AfmFontInfo						*/
/*									*/
/************************************************************************/

static void psInitAfmFontInfo(	AfmFontInfo *	afi )
    {
    afi->afiFontName= (char *)0;
    afi->afiFullName= (char *)0;
    afi->afiFamilyName= (char *)0;
    afi->afiWeight= (char *)0;

    afi->afiItalicAngle= 0.0;
    afi->afiTanItalicAngle= 0.0;

    afi->afiIsFixedPitch= 0;
    afi->afiFontBBox.abbLeft= 0;
    afi->afiFontBBox.abbBottom= 0;
    afi->afiFontBBox.abbRight= 0;
    afi->afiFontBBox.abbTop= 0;
    afi->afiUnderlinePosition= 0;
    afi->afiUnderlineThickness= 0;
    afi->afiEncodingScheme= (char *)0;
    afi->afiCapHeight= 0;
    afi->afiXHeight= 0;
    afi->afiAscender= 0;
    afi->afiDescender= 0;
    afi->afiCharacterSet= (char *)0;

    afi->afiMetricCount= 0;
    afi->afiMetrics= (AfmCharMetric *)0;

    return;
    }

static void psCleanAfmFontInfo(	AfmFontInfo *	afi )
    {
    if  ( afi->afiFontName )
	{ free( afi->afiFontName );	}
    if  ( afi->afiFullName )
	{ free( afi->afiFullName );	}
    if  ( afi->afiFamilyName )
	{ free( afi->afiFamilyName );	}
    if  ( afi->afiWeight )
	{ free( afi->afiWeight );	}
    if  ( afi->afiEncodingScheme )
	{ free( afi->afiEncodingScheme );}
    if  ( afi->afiCharacterSet )
	{ free( afi->afiCharacterSet );}

    if  ( afi->afiMetrics )
	{ free( afi->afiMetrics );}

    return;
    }

static int psGetWeight(		int *			pWeight,
				const char *		weight )
    {
    if  ( ! strcmp( weight, "Extra Light" ) )
	{ *pWeight= FONTweightEXTRA_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "Light" ) )
	{ *pWeight= FONTweightLIGHT; return 0;	}

    if  ( ! strcmp( weight, "Book" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "Regular" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "Normal" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "Medium" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "Roman" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}

    if  ( ! strcmp( weight, "Semibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "Demibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "Demi" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}

    if  ( ! strcmp( weight, "Bold" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}

    if  ( ! strcmp( weight, "Extrabold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}

    if  ( ! strcmp( weight, "Black" ) )
	{ *pWeight= FONTweightULTRA_BOLD; return 0;	}

    SDEB(weight); return -1;
    }

static void psFontWidthFromName(	int *		pWidth,
					const char *	name )
    {
    const char *	s= name;

    while( s[0] == ' ' )
	{ s++;	}

    while( *s )
	{
	if  ( ! strncmp( s, "Narrow", 6 )		&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    { *pWidth= FONTwidthCONDENSED; return;	}

	if  ( ! strncmp( s, "Extra Condensed", 15 )	&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    { *pWidth= FONTwidthEXTRA_CONDENSED; return;	}
	
	if  ( ! strncmp( s, "Condensed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    { *pWidth= FONTwidthCONDENSED; return;	}
	if  ( ! strncmp( s, "Compressed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    { *pWidth= FONTwidthCONDENSED; return;	}
	if  ( ! strncmp( s, "Compact", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    { *pWidth= FONTwidthCONDENSED; return;	}
	
	if  ( ! strncmp( s, "Extended", 8 )	&&
	      ( s[8] == '\0' || s[8] == ' ' )	)
	    { *pWidth= FONTwidthEXPANDED; return;	}

	while( s[0] && s[0] != ' ' )
	    { s++;	}
	while( s[0] == ' ' )
	    { s++;	}
	}
    }

static void psFillFaceFromInfo(		AppFontTypeface *	aft,
					const AfmFontInfo *	afi )
    {
    int		i;

    if  ( afi->afiWeight					&&
	  psGetWeight( &(aft->aftWeight), afi->afiWeight )	)
	{ SDEB(afi->afiWeight);	}

    if  ( aft->aftWeight > FONTweightMEDIUM )
	{ aft->aftIsBold= 1;	}
    else{ aft->aftIsBold= 0;	}

    if  ( afi->afiItalicAngle < -1.0 )
	{ aft->aftIsSlanted= 1; }
    else{ aft->aftIsSlanted= 0;	}

    psFontWidthFromName( &(aft->aftWidth), afi->afiFullName );

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{
	if  ( afi->afiSupportedCharsets[i].scSupported  )
	    { aft->aftDefaultEncoding= i; break;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Compare routine for sorting the faces of a font.			*/
/*									*/
/*  Serves two purposes:						*/
/*									*/
/*  1)  Present them in a predictable order in the font chooser.	*/
/*  2)  Makes it easier to present width variants as different		*/
/*	families.							*/
/*									*/
/************************************************************************/

static int psFontCompareFaces(	const AppFontTypeface *		aft1,
				const AppFontTypeface *		aft2,
				const AfmFontInfo *		afi1,
				const AfmFontInfo *		afi2 )
    {
    int				cmp;

    cmp= strcmp( afi1->afiFamilyName, afi2->afiFamilyName );
    if  ( cmp != 0 )
	{ return cmp;	}

    return utilFontCompareFaces( (void *)aft1, (void *)aft2 );
    }

static int psFontCompareTypeInfos(	const void *	voidafi1,
					const void *	voidafi2 )
    {
    const AfmFontInfo *	afi1= (AfmFontInfo *)voidafi1;
    const AfmFontInfo *	afi2= (AfmFontInfo *)voidafi2;

    AppFontTypeface	aft1;
    AppFontTypeface	aft2;

    docInitFontTypeface( &aft1 );
    docInitFontTypeface( &aft2 );

    psFillFaceFromInfo( &aft1, afi1 );
    psFillFaceFromInfo( &aft2, afi2 );

    return psFontCompareFaces( &aft1, &aft2, afi1, afi2 );
    }

/************************************************************************/
/*									*/
/*  Get a line of input. (Essentially fgets())				*/
/*									*/
/************************************************************************/

static char *	psGetInputLine(	char *		s,
				int *		pLen,
				int		size,
				FILE *		stream )
    {
    s= fgets( s, size, stream );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0 )
	    {
	    if  ( s[lineLength-1] == '\n' )
		{
		s[--lineLength]= '\0';

		if  ( lineLength > 0		&&
		      s[lineLength-1] == '\r'	)
		    { s[--lineLength]= '\0'; }
		}
	    }

	*pLen= lineLength;
	}

    return s;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of postscript fonts for the font chooser.		*/
/*									*/
/*  The code assumes that afmDirectory is some kind of a resource, that	*/
/*  does not vary over different calls to the routine.			*/
/*									*/
/*  A:  Consume one file.						*/
/*									*/
/************************************************************************/

#   define	FILEL	400

static AfmFontInfo *	UtilFontPsInfos= (AfmFontInfo *)0;
static int		UtilFontPsInfoCount= 0;


/*  A  */
static int psGotAfmFile(	const char *	filename,
				void *		through )
    {
    char		input[AFMlenLINE+1];
    FILE *		afmFile;
    int			res;

    AfmFontInfo *	afi;

    int			len= strlen( filename );

    if  ( len == 0 )
	{ LDEB(len); return -1;	}

    afi= (AfmFontInfo *)realloc( UtilFontPsInfos,
			    (UtilFontPsInfoCount+ 1)* sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}
    UtilFontPsInfos= afi;

    afi += UtilFontPsInfoCount;
    psInitAfmFontInfo( afi );

    afmFile= fopen( filename, "r" );
    if  ( ! afmFile )
	{ SXDEB(filename,afmFile); psCleanAfmFontInfo( afi ); return 0; }

    res= psAfmConsumeLines( afmFile, input, afi, psAfmFileKeywords,
			sizeof(psAfmFileKeywords)/sizeof(AfmKeyword) );

    fclose( afmFile );

    if  ( res < 0 )
	{ SLDEB(filename,res); psCleanAfmFontInfo( afi ); return 0; }

    if  ( psGetFontEncodings( afi ) )
	{
	SSDEB(filename,afi->afiFullName);
	SDEB(afi->afiEncodingScheme);
	psCleanAfmFontInfo( afi ); return 0;
	}

    if  ( ! afi->afiFamilyName )
	{ XDEB(afi->afiFamilyName); return -1;	}

    UtilFontPsInfoCount++; return 0;
    }

/************************************************************************/
/*									*/
/*  Set the faces of a font family.					*/
/*									*/
/*  1)  Some hacked fonts have a hyphen here.				*/
/*  2)  The face name is the tail of the full name.			*/
/*									*/
/************************************************************************/

static int appPsSetFontFaces(	AppFontFamily *		aff,
				int			count,
				const AfmFontInfo *	afi )
    {
    AppFontTypeface *	aft;
    int			face;

    aft= realloc( aff->affFaces, count* sizeof( AppFontTypeface ) );
    if  ( ! aft )
	{ LXDEB(count,aft); return -1;	}
    aff->affFaces= aft;

    for ( face= 0; face < count; aft++, afi++, face++ )
	{
	int		fl;

	docInitFontTypeface( aft );
	psFillFaceFromInfo( aft, afi );

	aft->aftPrintingData= (void *)afi;
	aft->aftFaceName= afi->afiFullName;

	aff->affFontFamilyName= afi->afiFamilyName;
	aff->affFontFamilyText= afi->afiFamilyName;

	if  ( aft->aftIsFixedWidth )
	    {
	    if  ( aff->affHasProportionalWidth )
		{ SDEB(afi->afiFullName);	}
	    aff->affHasFixedWidth= 1;
	    }
	else{
	    if  ( aff->affHasFixedWidth )
		{ SDEB(afi->afiFullName);	}
	    aff->affHasProportionalWidth= 1;
	    }

	fl= strlen( afi->afiFamilyName );

	/*  1  */
	if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	      afi->afiFullName[fl] == '-'				)
	    { afi->afiFullName[fl]=   ' ';	}

	/*  2  */
	if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	      ( afi->afiFullName[fl] == ' '		||
		afi->afiFullName[fl] == '\0'	)	)
	    {
	    aft->aftFaceName= afi->afiFullName+ fl;
	    while( aft->aftFaceName[0] == ' ' )
		{ aft->aftFaceName++;	}
	    if  ( ! aft->aftFaceName[0] )
		{ aft->aftFaceName= afi->afiWeight;	}
	    }
	else{
	    SSDEB(afi->afiFamilyName,afi->afiFullName);
	    if  ( afi->afiWeight )
		{ aft->aftFaceName= afi->afiWeight;	}
	    else{ aft->aftFaceName= afi->afiFullName;	}
	    }
	}

    aff->affFaceCount= count;

    if  ( aff->affFaceCount > 0 )
	{
	int			enc;

	qsort( aff->affFaces, aff->affFaceCount,
			    sizeof(AppFontTypeface), utilFontCompareFaces );

	aft= aff->affFaces;
	for ( face= 0; face < aff->affFaceCount; aft++, face++ )
	    {
	    AfmFontInfo *	afiFace= (AfmFontInfo *)aft->aftPrintingData;

	    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
		{
		if  ( face == 0 )
		    {
		    aff->affSupportedCharsets[enc]= 
					    afiFace->afiSupportedCharsets[enc];
		    }
		else{
		    if  ( ! afiFace->afiSupportedCharsets[enc].scSupported )
			{ aff->affSupportedCharsets[enc].scSupported= 0; }
		    }
		}
	    }

	aff->affDefaultEncoding= aff->affFaces[0].aftDefaultEncoding;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of all the fonts for which an AFM file is found.	*/
/*									*/
/*  1)  Collect the information from all AFM files.			*/
/*	NOTE: Afm files are in the "C" locale.				*/
/*  2)  Count families.							*/
/*	NOTE: Width variants of the same family are treated like	*/
/*	different families here.					*/
/*  3)  Allocate space for the families.				*/
/*  4)  Divide the faces over the families.				*/
/*									*/
/************************************************************************/

static AppFontFamily *	EditFontPsFamilies= (AppFontFamily *)0;
static int		EditFontPsFamilyCount= 0;

int psFontCatalog(	const char *		afmDirectory,
			AppFontFamily **	pFamilies,
			int *			pCount		)
    {
    FILE *			xfontDir;
    int				lineLength;

    int				j;
    int				from;

    int				fam;
    int				face;

    AppFontFamily *		aff;
    AppFontTypeface *		aft;
    AfmFontInfo *		afi;

    char			scratch[FILEL+ 1];

#   ifdef __VMS
    const char * const		afmExtension= "AFM";
#   else
    const char * const		afmExtension= "afm";
#   endif

    if  ( EditFontPsFamilyCount > 0 )
	{
	*pFamilies= EditFontPsFamilies;
	*pCount= EditFontPsFamilyCount;
	return 0;
	}

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    j= appForAllFiles( afmDirectory, afmExtension, (void *)0, psGotAfmFile );

    setlocale( LC_NUMERIC, "" );

    if  ( j )
	{ SDEB(afmDirectory); return -1;	}

    qsort( UtilFontPsInfos, UtilFontPsInfoCount,
			    sizeof(AfmFontInfo), psFontCompareTypeInfos );

    /*  2  */
    fam= 1;
    for ( j= 1; j < UtilFontPsInfoCount; j++ )
	{
	AppFontTypeface	aft1;
	AppFontTypeface	aft2;

	docInitFontTypeface( &aft1 );
	docInitFontTypeface( &aft2 );

	psFillFaceFromInfo( &aft1, &(UtilFontPsInfos[j+ 0]) );
	psFillFaceFromInfo( &aft2, &(UtilFontPsInfos[j- 1]) );

	if  ( strcmp( UtilFontPsInfos[j].afiFamilyName,
				UtilFontPsInfos[j- 1].afiFamilyName )	)
	    { fam++;	}
	}

    /*  3  */
    aff= realloc( EditFontPsFamilies, fam* sizeof(AppFontFamily) );
    if  ( ! aff )
	{ LXDEB(fam,aff); return -1;	}
    EditFontPsFamilies= aff;

    aff= EditFontPsFamilies;
    for ( j= 0; j < fam; aff++, j++ )
	{ docInitFontFamily( aff );	}

    /*  2  */
    from= 0; fam= 0;
    for ( j= 1; j < UtilFontPsInfoCount; j++ )
	{
	AppFontTypeface	aft1;
	AppFontTypeface	aft2;

	docInitFontTypeface( &aft1 );
	docInitFontTypeface( &aft2 );

	psFillFaceFromInfo( &aft1, &(UtilFontPsInfos[j+ 0]) );
	psFillFaceFromInfo( &aft2, &(UtilFontPsInfos[j- 1]) );

	if  ( ! strcmp( UtilFontPsInfos[j].afiFamilyName,
				UtilFontPsInfos[j- 1].afiFamilyName )	)
	    { continue;	}

	if  ( appPsSetFontFaces( EditFontPsFamilies+ fam,
					    j- from, UtilFontPsInfos+ from ) )
	    { LDEB(j- from); return -1;	}

	from= j; fam++;
	}

    if  ( appPsSetFontFaces( EditFontPsFamilies+ fam,
					    j- from, UtilFontPsInfos+ from ) )
	{ LDEB(j- from); return -1;	}

    EditFontPsFamilyCount= fam+ 1;

    sprintf( scratch, "%s/xfonts.dir", afmDirectory );
    xfontDir= fopen( scratch, "r" );
    if  ( xfontDir )
	{
	while( psGetInputLine( scratch, &lineLength, FILEL, xfontDir ) )
	    {
	    char *	s= scratch;

	    while( *s && ! isspace( *s ) )
		{ s++; }

	    if  ( ! *s )
		{ SDEB(scratch); continue;	}

	    *(s++)= '\0';
	    while( isspace( *s ) )
		{ s++;	}
	    if  ( ! *s )
		{ SDEB(scratch); continue;	}

	    aff= EditFontPsFamilies; aft= (AppFontTypeface *)0;
	    for ( fam= 0; fam < EditFontPsFamilyCount; aff++, fam++ )
		{
		aft= aff->affFaces;
		for ( face= 0; face < aff->affFaceCount; aft++, face++ )
		    {
		    afi= (AfmFontInfo *)aft->aftPrintingData;

		    if  ( ! strcmp( afi->afiFontName, scratch )	)
			{ break;	}
		    }

		if  ( face < aff->affFaceCount )
		    { break;	}
		}

	    if  ( fam < EditFontPsFamilyCount )
		{
		j= strlen( s );
		while( j > 1 && ( s[j-1] == '\n' || isspace( s[j-1] ) ) )
		    { j--; s[j]= '\0';	}

		aft->aftXQueryFormat= (char *)malloc( j+ 1 );
		if  ( ! aft->aftXQueryFormat )
		    { XDEB(aft->aftXQueryFormat); return -1;	}
		strcpy( aft->aftXQueryFormat, s );
		}
	    }

	fclose( xfontDir );
	}

    *pFamilies= EditFontPsFamilies;
    *pCount= EditFontPsFamilyCount;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume a series of lines.						*/
/*									*/
/************************************************************************/

static int psAfmConsumeLines(	FILE *		f,
				char *		input,
				AfmFontInfo *	afi,
				AfmKeyword *	keywords,
				int		keywordCount )
    {
    for (;;)
	{
	int		k;
	int		b;
	int		p;
	AfmKeyword *	ak;

	if  ( ! psGetInputLine( input, &p, AFMlenLINE, f ) )
	    { return 1;	}

	b= 0;
	while( input[b] &&   isspace( input[b] ) )
	    { b++;	}

	if  ( ! input[b] )
	    { continue;	}

	p= b;
	while( input[p] && ! isspace( input[p] ) )
	    { p++;	}
	if  ( input[p] )
	    { input[p++]= '\0';	}

	while( isspace( input[p] ) )
	    { p++;	}

	ak= keywords;
	for ( k= 0; k < keywordCount; ak++, k++ )
	    {
	    if  ( ! strcmp( input+ b, ak->akString ) )
		{ break;	}
	    }

	if  ( k >= keywordCount )
	    { SDEB(input+ b); return -1;	}
	if  ( ! ak->akConsumer )
	    { return 0;	}

	if  ( (*ak->akConsumer)( f, p, input, afi ) )
	    { SDEB(input+ b); return -1;	}
	}
    }

/************************************************************************/
/*									*/
/*  Ignore an input line.						*/
/*									*/
/************************************************************************/
static int	psAfmIgnore(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Consume font metrics.						*/
/*									*/
/*  1)  Not interested in the version.					*/
/*									*/
/************************************************************************/

static int psAfmFontMetrics(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    {
    return psAfmConsumeLines( f, input, afi, psAfmMetricsKeywords,
			    sizeof(psAfmMetricsKeywords)/sizeof(AfmKeyword) );
    }

/************************************************************************/
/*									*/
/*  Extract various types of values.					*/
/*									*/
/************************************************************************/

static int psAfmSaveString(	char **		pTarget,
				char *		source )
    {
    *pTarget= (char *)malloc( strlen( source )+ 1 );

    if  ( ! *pTarget )
	{ XDEB(*pTarget); return -1;	}

    strcpy( *pTarget, source );
    return 0;
    }

static int psAfmGetNumber(	double *	pNumber,
				char *		input,
				int		valPos )
    {
    char *	s= input+ valPos;
    char *	past;
    double	d;

    d= strtod( s, &past );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}

    *pNumber= d;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract various strings.						*/
/*									*/
/************************************************************************/

static int psAfmFontName(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFontName), input+ valPos ); }

static int psAfmFullName(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFullName), input+ valPos ); }

static int psAfmFamilyName(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiFamilyName), input+ valPos ); }

static int	psAfmWeight(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiWeight), input+ valPos ); }

static int	psAfmEncodingScheme(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiEncodingScheme), input+ valPos ); }

/************************************************************************/
/*									*/
/*  Unimplemented..							*/
/*									*/
/************************************************************************/

static int	psAfmBBox(	int		valPos,
				char *		input,
				AfmBBox *	abb )
    {
    char *	s= input+ valPos;
    char *	past;
    double	left, bottom, right, top;

    int		done= 0;

    left= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    bottom= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    right= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    top= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    abb->abbLeft= (int)left;
    abb->abbBottom= (int)bottom;
    abb->abbRight= (int)right;
    abb->abbTop= (int)top;

    return done;
    }

static int psAfmFontBBox(	FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    {
    AfmBBox	abb;
    int		res= psAfmBBox( valPos, input, &abb );
    char *	s;

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    s= input+ valPos+ res;
    while( isspace( *s ) )
	{ s++;	}
    if  ( *s )
	{ SSDEB(input,input+ valPos+ res); return -1;	}

    afi->afiFontBBox= abb;

    return 0;
    }

static int	psAfmMappingScheme(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { LDEB(1); return -1; }

static int	psAfmCharacterSet(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiCharacterSet), input+ valPos ); }

static int	psAfmCharacters(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    int		count;
    char *	past;
    char *	s= input+ valPos;

    count= strtol( s, &past, 10 );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}

    return 0;
    }

static int	psAfmIsFixedV(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmStartDirection(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmCapHeight(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiCapHeight= (int)d; return 0;
    }

static int	psAfmXHeight(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiXHeight= (int)d; return 0;
    }

static int	psAfmAscender(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiAscender= (int)d; return 0;
    }

static int	psAfmDescender(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiDescender= (int)d; return 0;
    }

static int	psAfmUnderlinePosition(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiUnderlinePosition= (int)d; return 0;
    }

static int	psAfmUnderlineThickness(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    afi->afiUnderlineThickness= (int)d; return 0;
    }

static int	psAfmItalicAngle(	FILE *		f,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    if  ( psAfmGetNumber( &(afi->afiItalicAngle), input, valPos ) )
	{ return -1;	}

    afi->afiTanItalicAngle= tan( ( M_PI* afi->afiItalicAngle ) / 180  );

    return 0;
    }

static int	psAfmCharWidth(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    LDEB(1); return -1;
    }

static int	psAfmIsFixedPitch(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    if  ( ! strcmp( input+ valPos, "false" ) )
	{ afi->afiIsFixedPitch= 0; return 0;	}
    if  ( ! strcmp( input+ valPos, "true" ) )
	{ afi->afiIsFixedPitch= 1; return 0;	}

    SSDEB(input,input+ valPos); return -1;
    }

static int	psAfmStartCharMetrics(	FILE *		f,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, f ) )
	{
	AfmCharMetric *	acm;
	char *		b;
	char *		p;

	int		C= -1;
	double		WX= 0.0;
	char *		N= NULL;
	AfmBBox		abb;

	abb.abbLeft= abb.abbRight= abb.abbTop= abb.abbBottom= 0;

	if  ( ! strncmp( input, "EndCharMetrics", 14 ) )
	    {
	    b= input+ 14;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	b= input;
	while( isspace( *b ) )
	    { b++;	}

	while( *b )
	    {
	    p= b; while( *p && ! isspace( *p ) )
		{ p++;	}
	    if  ( isspace( *p ) )
		{ *(p++)= '\0';	}

	    if  ( ! strcmp( b, "C" ) )
		{
		b= p;
		C= strtol( b, &p, 10 );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "CX" ) )
		{
		b= p;
		C= strtol( b, &p, 16 );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p != ';' )
		    { SDEB(b); return -1;	}
		b= p+ 1; continue;
		}

	    if  ( ! strcmp( b, "WX" ) )
		{
		b= p;
		WX= strtod( b, &p );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "N" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		N= (char *)malloc( p- b+ 1 );
		if  ( ! N )
		    { XDEB(N); return -1;	}
		strncpy( N, b, p- b )[p- b]= '\0';
		goto nextToken;
		}

	    if  ( ! strcmp( b, "L" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    if  ( ! strcmp( b, "B" ) )
		{
		int	done;

		b= p;
		done= psAfmBBox( 0, b, &abb );

		if  ( done < 0 )
		    { SLDEB(b,done); return -1;	}

		p= b+ done; goto nextToken;
		}

	    SSDEB(b,N); return -1;

	    nextToken:
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p != ';' )
		    { SDEB(b); return -1;	}
		b= p+ 1;
		while( isspace( *b ) )
		    { b++;	}
		continue;
	    }

	acm= (AfmCharMetric *)realloc( afi->afiMetrics,
		    ( afi->afiMetricCount+ 1 )* sizeof( AfmCharMetric ) );
	if  ( ! acm )
	    { LXDEB(C,acm); return -1;	}
	afi->afiMetrics= acm;

	acm += afi->afiMetricCount++;

	acm->acmC= C;
	acm->acmWX= (int)WX;
	acm->acmN= N;
	acm->acmKernPairs= (AfmKerningPair *)0;
	acm->acmKernPairCount= 0;
	acm->acmBBox= abb;
	}

    LDEB(AFMlenLINE); return -1;
    }

static int	psAfmStartKernPairs(	FILE *		f,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, f ) )
	{
	AfmCharMetric *		acm;
	AfmKerningPair *	akp;
	char *			b;
	char *			p;

	char *	N1= (char *)0;
	char *	N2= (char *)0;
	double	XVec= 0.0;

	int	pos1;
	int	pos2;

	if  ( ! strncmp( input, "EndKernPairs", 12 ) )
	    {
	    b= input+ 12;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	b= input;
	while( isspace( *b ) )
	    { b++;	}

	if  ( ! *b || *b == '\n' )
	    { continue;	}

	while( *b )
	    {
	    p= b; while( *p && ! isspace( *p ) )
		{ p++;	}
	    if  ( isspace( *p ) )
		{ *(p++)= '\0';	}

	    if  ( ! strcmp( b, "KPX" ) )
		{
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}
		N1= b;
		b= p; while( isspace( *b ) )
		    { b++;	}
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( b == p )
		    { SDEB(b); return -1;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}
		N2= b;
		b= p;
		XVec= strtod( b, &p );
		if  ( b == p )
		    { SDEB(b); return -1;	}
		goto nextToken;
		}

	    SDEB(b); return -1;

	    nextToken:
		while( isspace( *p ) )
		    { p++;	}
		if  ( *p && *p != ';' )
		    { SDEB(b); return -1;	}
		if  ( *p == ';' )
		    { b= p+ 1; }
		else{ b= p   ; }

		while( isspace( *b ) )
		    { b++;	}
		continue;
	    }

	acm= afi->afiMetrics;
	for ( pos2= 0; pos2 < afi->afiMetricCount; acm++, pos2++ )
	    {
	    if  ( acm->acmN && ! strcmp( acm->acmN, N2 ) )
		{ break;	}
	    }

	acm= afi->afiMetrics;
	for ( pos1= 0; pos1 < afi->afiMetricCount; acm++, pos1++ )
	    {
	    if  ( acm->acmN && ! strcmp( acm->acmN, N1 ) )
		{ break;	}
	    }

	if  ( pos1 >= afi->afiMetricCount || pos2 >= afi->afiMetricCount )
	    { LLDEB(pos1,pos2); return -1;	}

	akp= (AfmKerningPair *)realloc( acm->acmKernPairs,
			( acm->acmKernPairCount+ 1 )* sizeof(AfmKerningPair) );
	if  ( ! akp )
	    { LXDEB(acm->acmKernPairCount,akp); return -1;	}
	acm->acmKernPairs= akp;

	akp += acm->acmKernPairCount++;
	akp->akpPosition= pos2;
	akp->akpXVec= (int)XVec;
	}

    LDEB(AFMlenLINE); return -1;
    }

static AfmKeyword	psAfmKernDataKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartKernPairs",	psAfmStartKernPairs,	},
	{ "StartTrackKern",	psAfmStartTrackKern,	},
	{ "EndKernData",	0,			},
    };

static int	psAfmStartKernData(	FILE *	f,
					int	valPos,
					char *	input,
					AfmFontInfo *	afi )
    {
    return psAfmConsumeLines( f, input, afi, psAfmKernDataKeywords,
		    sizeof(psAfmKernDataKeywords)/sizeof(AfmKeyword) );
    }

static int	psAfmStartComposites(	FILE *		f,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, f ) )
	{
	if  ( ! strncmp( input, "EndComposites", 13 ) )
	    { break;					}
	}

    return 0;
    }

static int	psAfmStartTrackKern(	FILE *		f,
					int		valPos,
					char *		input,
					AfmFontInfo *	afi )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, f ) )
	{
	if  ( ! strncmp( input, "EndTrackKern", 13 ) )
	    { break;					}
	}

    return 0;
    }

