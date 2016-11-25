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
#   include	"utilMatchFont.h"
#   include	"appSystem.h"
#   include	"utilT1Entries.h"

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  HACK: map some (URW) names to standard font names (ADOBE base 35).	*/
/*									*/
/************************************************************************/

typedef struct NameMap
    {
    const char *	nmFrom;
    const char *	nmTo;
    } NameMap;

static const NameMap PS_FamilyMap[]=
    {
	{ "Century Schoolbook L",	"New Century Schoolbook"	},
	{ "Nimbus Mono L",		"Courier"			},
	{ "Nimbus Roman No9 L",		"Times"				},
	{ "Nimbus Sans L",		"Helvetica"			},
	{ "Standard Symbols L",		"Symbol"			},
	{ "URW Antiqua T",		"New Century Schoolbook"	},
	{ "URW Bookman L",		"ITC Bookman"			},
	{ "URW Chancery L",		"ITC Zapf Chancery"		},
	{ "URW Gothic L",		"ITC Avant Garde Gothic"	},
	{ "URW Palladio L",		"Palatino"			},
	{ "Dingbats",			"ITC Zapf Dingbats"		},
    };

static const NameMap PS_FullMap[]=
    {
    { "URW Gothic L Book",  "ITC Avant Garde Gothic Book"	},
    { "URW Gothic L Book Oblique",
			    "ITC Avant Garde Gothic Book Oblique"	},
    { "URW Gothic L Demi",  "ITC Avant Garde Gothic Demi"	},
    { "URW Gothic L Demi Oblique",
			    "ITC Avant Garde Gothic Demi Oblique"	},
    { "URW Bookman L Light",
			    "ITC Bookman Light"	},
    { "URW Bookman L Light Italic",
			    "ITC Bookman Light Italic"	},
    { "URW Bookman L Demi Bold",
			    "ITC Bookman Demi"	},
    { "URW Bookman L Demi Bold Italic",
			    "ITC Bookman Demi Italic"	},
    { "Nimbus Mono L Regular",
			    "Courier"	},
    { "Nimbus Mono L Regular Oblique",
			    "Courier Oblique"	},
    { "Nimbus Mono L Bold", "Courier Bold"	},
    { "Nimbus Mono L Bold Oblique",
			    "Courier Bold Oblique"	},
    { "Nimbus Sans L Regular",
			    "Helvetica"	},
    { "Nimbus Sans L Regular Italic",
			    "Helvetica Oblique"	},
    { "Nimbus Sans L Bold", "Helvetica Bold"	},
    { "Nimbus Sans L Bold Italic",
			    "Helvetica Bold Oblique"	},
    { "Nimbus Sans L Regular Condensed",
			    "Helvetica Narrow"	},
    { "Nimbus Sans L Regular Condensed Italic" ,
			    "Helvetica Narrow Oblique"	},
    { "Nimbus Sans L Bold Condensed",
			    "Helvetica Narrow Bold"	},
    { "Nimbus Sans L Bold Condensed Italic",
			    "Helvetica Narrow Bold Oblique"	},
    { "Century Schoolbook L Roman",
			    "New Century Schoolbook Roman"	},
    { "Century Schoolbook L Italic",
			    "New Century Schoolbook Italic"	},
    { "Century Schoolbook L Bold",
			    "New Century Schoolbook Bold"	},
    { "Century Schoolbook L Bold Italic",
			    "New Century Schoolbook Bold Italic"	},
    { "URW Palladio L Roman",
			    "Palatino Roman"	},
    { "URW Palladio L Italic",
			    "Palatino Italic"	},
    { "URW Palladio L Bold",
			    "Palatino Bold"	},
    { "URW Palladio L Bold Italic",
			    "Palatino Bold Italic"	},
    { "Standard Symbols L", "Symbol"	},
    { "Nimbus Roman No9 L Regular",
			    "Times Roman"	},
    { "Nimbus Roman No9 L Regular Italic",
			    "Times Italic"	},
    { "Nimbus Roman No9 L Medium",
			    "Times Bold"	},
    { "Nimbus Roman No9 L Medium Italic",
			    "Times Bold Italic"	},
    { "URW Chancery L Medium Italic",
			    "ITC Zapf Chancery Medium Italic"	},
    { "Dingbats",	    "ITC Zapf Dingbats"	},
    };

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
static int psAfmNotice(			FILE *,	int, char *,AfmFontInfo *);
static int psAfmVersion(		FILE *,	int, char *,AfmFontInfo *);
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
	{ "Version",		psAfmVersion,		},
	{ "Notice",		psAfmNotice,		},
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

static void psFillFaceFromInfo(		AppFontTypeface *	aft,
					const AfmFontInfo *	afi )
    {
    int		i;

    int		widthStart;
    int		widthLength;

    int		weightStart;
    int		weightLength;

    if  ( afi->afiWeight )
	{
	if  ( ! utilFontWeightFromString( &(aft->aftWeight),
			    &weightStart, &weightLength, afi->afiWeight ) )
	    { SDEB(afi->afiWeight);	}
	}

    if  ( aft->aftWeight > FONTweightMEDIUM )
	{ aft->aftIsBold= 1;	}
    else{ aft->aftIsBold= 0;	}

    if  ( afi->afiItalicAngle < -1.0 )
	{ aft->aftIsSlanted= 1; }
    else{ aft->aftIsSlanted= 0;	}

    utilFontWidthFromString( &(aft->aftWidth),
				&widthStart, &widthLength, afi->afiFullName );

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
/*  3)  Remove a second carriage return. Some files really have this.	*/
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

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\n'	)
	    { s[--lineLength]= '\0'; 	}

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	/*  3  */
	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

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
/*  B:  Ignore errors in cerian hopeless files.				*/
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

    int			i;
    int			mappedFamily= -1;
    int			mappedFull= -1;

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
	{
	SXDEB(filename,afmFile);
	psCleanAfmFontInfo( afi ); return 0;
	}

    res= psAfmConsumeLines( afmFile, input, afi, psAfmFileKeywords,
				sizeof(psAfmFileKeywords)/sizeof(AfmKeyword) );

    fclose( afmFile );

    if  ( res < 0 )
	{ SLDEB(filename,res); psCleanAfmFontInfo( afi ); return 0; }

    if  ( psGetFontEncodings( afi ) )
	{
	int		l= strlen( filename );
	int		h;
	int		complain= 1;

	const char * const	hopeless[]=
	    {
	    "/ghostscript/fonts/fcyr.afm",
	    "/ghostscript/fonts/fcyri.afm",
	    };

	for ( h= 0; h < sizeof(hopeless)/sizeof(char *); h++ )
	    {
	    int		ll= strlen( hopeless[h] );

	    if  ( ll < l && ! strcmp( filename+ l- ll,  hopeless[h] ) )
		{ complain= 0; break;	}
	    }

	if  ( complain )
	    { SSSDEB(filename,afi->afiFullName,"SUPPORTS NO KNOWN ENCODINGS"); }

	psCleanAfmFontInfo( afi ); return 0;
	}

    if  ( ! afi->afiFamilyName )
	{
	SXDEB(filename,afi->afiFamilyName);
	psCleanAfmFontInfo( afi ); return 0;
	}

    for ( i= 0; i < sizeof(PS_FamilyMap)/sizeof(NameMap); i++ )
	{
	if  ( ! strcmp( afi->afiFamilyName, PS_FamilyMap[i].nmFrom ) )
	    { mappedFamily= i; break; }
	}

    for ( i= 0; i < sizeof(PS_FullMap)/sizeof(NameMap); i++ )
	{
	if  ( ! strcmp( afi->afiFullName, PS_FullMap[i].nmFrom ) )
	    { mappedFull= i; break; }
	}

    if  ( mappedFamily >= 0 && mappedFull >= 0 )
	{
	free( afi->afiFamilyName );
	afi->afiFamilyName= strdup( PS_FamilyMap[mappedFamily].nmTo );

	free( afi->afiFullName );
	afi->afiFullName= strdup( PS_FullMap[mappedFull].nmTo );
	}

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
				AfmFontInfo *		afi )
    {
    AppFontTypeface *	aft;
    int			face;

    int			width= -1;
    int			widthStart= -1;
    int			widthLength= 0;
    const char *	widthStartString= (const char *)0;

    if  ( count == 0 )
	{ LDEB(count); return 0;	}

    aft= realloc( aff->affFaces, count* sizeof( AppFontTypeface ) );
    if  ( ! aft )
	{ LXDEB(count,aft); return -1;	}
    aff->affFaces= aft;

    aff->affFontFamilyName_Orig= afi->afiFamilyName;
    aff->affFontFamilyName= afi->afiFamilyName;

    docInitFontTypeface( aft );
    psFillFaceFromInfo( aft, afi );

    aff->affWidth= aft->aftWidth;

    utilFontWidthFromString( &width, &widthStart, &widthLength,
							afi->afiFullName );
    if  ( widthStart >= 0 )
	{
	if  ( ! afi->afiWidth )
	    {
	    afi->afiWidth= malloc( widthLength+ 1 );
	    if  ( afi->afiWidth )
		{
		strncpy( afi->afiWidth,
			afi->afiFullName+ widthStart,
			widthLength )[widthLength]= '\0';
		}
	    }

	widthStartString= afi->afiFullName+ widthStart;
	}

    for ( face= 0; face < count; aft++, afi++, face++ )
	{
	int		fl;

	docInitFontTypeface( aft );
	psFillFaceFromInfo( aft, afi );

	aft->aftPrintingData= (void *)afi;
	aft->aftFaceName= afi->afiFullName;

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

    if  ( widthStart >= 0 )
	{
	int	len= strlen( aff->affFontFamilyName );
	char *	text= malloc( len+ 1+ widthLength+ 1 );

	if  ( text )
	    {
	    strcpy( text, aff->affFontFamilyName );
	    text[len]= ' ';
	    strncpy( text+ len+ 1, widthStartString, widthLength );
	    text[len+ 1+ widthLength]= '\0';

	    aff->affFontFamilyName= text;
	    }
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

int psFontCatalog(	PostScriptFontList *	psfl,
			const char *		afmDirectory )
    {
    int				j;
    int				from;

    int				fam;
    AppFontFamily *		aff;

    char			scratch[FILEL+ 1];

#   ifdef __VMS
    const char * const		afmExtension= "AFM";
#   else
    const char * const		afmExtension= "afm";
#   endif

    if  ( EditFontPsFamilyCount > 0 )
	{
	psfl->psflFamilies= EditFontPsFamilies;
	psfl->psflFamilyCount= EditFontPsFamilyCount;
	return 0;
	}

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    j= appForAllFiles( afmDirectory, afmExtension, (void *)0, psGotAfmFile );

    setlocale( LC_NUMERIC, "" );

    if  ( j )
	{ SDEB(afmDirectory); return -1;	}

    if  ( UtilFontPsInfoCount < 1 )
	{
	LDEB(UtilFontPsInfoCount);

	psfl->psflFamilies= EditFontPsFamilies;
	psfl->psflFamilyCount= EditFontPsFamilyCount;

	return 0;
	}

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
				UtilFontPsInfos[j- 1].afiFamilyName )	||
	      aft1.aftWidth != aft2.aftWidth				)
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
				UtilFontPsInfos[j- 1].afiFamilyName )	&&
	      aft1.aftWidth == aft2.aftWidth				)
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

    psfl->psflFamilies= EditFontPsFamilies;
    psfl->psflFamilyCount= EditFontPsFamilyCount;

    sprintf( scratch, "%s/xfonts.dir", afmDirectory );
    if  ( ! appTestFileExists( scratch ) )
	{
	const int	mapNames= 0;

	if  ( utilSetT1EntriesForFonts( scratch, mapNames, psfl ) )
	    { SDEB(scratch); return -1;	}
	}

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

static int psAfmNotice(		FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiNotice), input+ valPos ); }

static int psAfmVersion(		FILE *		f,
				int		valPos,
				char *		input,
				AfmFontInfo *	afi )
    { return psAfmSaveString( &(afi->afiVersion), input+ valPos ); }

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

