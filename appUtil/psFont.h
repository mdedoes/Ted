/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_H
#   define	PS_FONT_H

#   include	<docFont.h>
#   include	<utilFontEncoding.h>

/************************************************************************/
/*									*/
/*  Charsets/Encodings supported by a font.				*/
/*									*/
/************************************************************************/

typedef struct SupportedCharset
    {
    int		scSupported;
    int		scCodeToGlyphMapping[256];
    } SupportedCharset;

/************************************************************************/
/*									*/
/*  Information extracted fron an AFM file.				*/
/*									*/
/************************************************************************/

typedef struct AfmKerningPair
    {
    int		akpPosition;
    int		akpXVec;
    } AfmKerningPair;

typedef struct AfmBBox
    {
    int		abbLeft;
    int		abbBottom;
    int		abbRight;
    int		abbTop;
    } AfmBBox;

typedef struct AfmCharMetric
    {
    int			acmC;
    int			acmWX;
    AfmBBox		acmBBox;
    char *		acmN;
    AfmKerningPair *	acmKernPairs;
    int			acmKernPairCount;
    } AfmCharMetric;

typedef struct AfmFontInfo
    {
    char *		afiFontName;
    char *		afiFullName;
    char *		afiFamilyName;
    char *		afiWeight;
    double		afiItalicAngle;
    double		afiTanItalicAngle;
    unsigned char	afiIsFixedPitch;
    AfmBBox		afiFontBBox;
    double		afiUnderlinePosition;
    double		afiUnderlineThickness;
    int			afiCapHeight;
    int			afiXHeight;
    int			afiAscender;
    int			afiDescender;

    char *		afiEncodingScheme;
    char *		afiCharacterSet;

    int			afiMetricCount;
    AfmCharMetric *	afiMetrics;

    SupportedCharset	afiSupportedCharsets[ENCODINGps_COUNT];
    } AfmFontInfo;

/************************************************************************/
/*									*/
/*  Font information.							*/
/*									*/
/*  Used to map a users idea of fonts to implementation fonts.		*/
/*  Used by the font chooser to select fonts.				*/
/*									*/
/*  1)  Set width defines. The values math those in paragraph 3.2.37	*/
/*	of the X11R5 logical font description convention.		*/
/*	[ The differences in value are more interesting than the values	*/
/*	themselves. ]							*/
/*  1)  Set weight defines. The values math those in paragraph 3.2.38	*/
/*	of the X11R5 logical font description convention.		*/
/*	[ The differences in value are more interesting than the values	*/
/*	themselves. ]							*/
/*									*/
/************************************************************************/

/*  1  */
#   define	FONTwidth_UNDEFINED		0

#   define	FONTwidth_MIN			FONTwidthULTRA_CONDENSED
#   define	FONTwidthULTRA_CONDENSED	10
#   define	FONTwidthEXTRA_CONDENSED	20
#   define	FONTwidthCONDENSED		30
#   define	FONTwidthSEMI_CONDENSED		40
#   define	FONTwidthMEDIUM			50
#   define	FONTwidthSEMI_EXPANDED		60
#   define	FONTwidthEXPANDED		70
#   define	FONTwidthEXTRA_EXPANDED		80
#   define	FONTwidthULTRA_EXPANDED		90
#   define	FONTwidth_MAX			FONTwidthULTRA_EXPANDED

/*  2  */
#   define	FONTweight_UNDEFINED		0

#   define	FONTweight_MIN			FONTweightULTRA_LIGHT
#   define	FONTweightULTRA_LIGHT		10  /* = Thin */
#   define	FONTweightEXTRA_LIGHT		20
#   define	FONTweightLIGHT			30
#   define	FONTweightSEMI_LIGHT		40
#   define	FONTweightMEDIUM		50
#   define	FONTweightSEMI_BOLD		60
#   define	FONTweightBOLD			70
#   define	FONTweightEXTRA_BOLD		80
#   define	FONTweightULTRA_BOLD		90  /* = Heavy = Black */
#   define	FONTweight_MAX			FONTweightULTRA_BOLD

typedef struct AppFontTypeface
    {
    char *			aftFaceName;
    int *			aftSizes;
    int				aftSizeCount;
    unsigned int		aftIsBold:1;
    unsigned int		aftIsSlanted:1;
    unsigned int		aftIsScalable:1;
    unsigned int		aftIsFixedWidth:1;
    void *			aftPrintingData;
    char *			aftXQueryFormat;
    int				aftWidth;
    int				aftWeight;
    int				aftDefaultEncoding;
    } AppFontTypeface;

typedef struct AppFontFamily
    {
    char *			affFontFamilyName;
    char *			affFontFamilyText;
    AppFontTypeface *		affFaces;
    int				affFaceCount;
    unsigned int		affHasFixedWidth:1;
    unsigned int		affHasProportionalWidth:1;

    int				affDefaultEncoding;
    SupportedCharset		affSupportedCharsets[ENCODINGps_COUNT];
    } AppFontFamily;

/************************************************************************/
/*									*/
/*  Permissible aliasses for glyph names.				*/
/*									*/
/************************************************************************/

typedef struct AlternateGlyphName
    {
    const char *	agnStandardName;
    const char *	agnAlternateName;
    } AlternateGlyphName;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void docInitFontFamily(		AppFontFamily *		aff );
extern void docInitFontTypeface(	AppFontTypeface *	aft );

extern const AlternateGlyphName	PS_AlternateNames[];

extern int psCalculateStringExtents(	AfmBBox *		abb,
					const unsigned char *	s,
					int			len,
					int			twipsSize,
					int			withKerning,
					int			encoding,
					const AfmFontInfo *	afi );

extern int psFontCatalog(	const char *		afmDirectory,
				AppFontFamily **	pFamilies,
				int *			pCount );


extern int psGetFontEncodings(	AfmFontInfo *	afi );

extern void utilFontFaceDistance(	int *			pDifCount,
					double *		pDistance,
					const AppFontTypeface *	aft1,
					const AppFontTypeface *	aft2 );

extern int utilFontGetPsFont(	int *			pFamily,
				int *			pFace,
				AppFontFamily **	pAff,
				AppFontTypeface **	pAft,
				int *			pFontEncoding,
				const char *		afmDirectory,
				const DocumentFont *	df,
				const TextAttribute *	ta );

#   endif
