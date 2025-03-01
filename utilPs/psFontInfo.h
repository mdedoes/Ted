/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_INFO_H
#   define	PS_FONT_INFO_H

#   include	<geoRectangle.h>
#   include	<utilIndexSet.h>
#   include	<utilIndexMapping.h>
#   include	<utilMemoryBuffer.h>

struct AfmFontInfo;

#   define	PSflagDEFER_METRICS	0x01
#   define	PSflagIGNORE_KERNING	0x02

/************************************************************************/
/*									*/
/*  Information extracted from an AFM file. (Or directly from a font)	*/
/*									*/
/*  The afm file format suggests that a font is a mapping from glyph	*/
/*  to code point while both type1 fonts and TTF fonts actually work	*/
/*  the other way: They have a collection of glyphs and the encoding	*/
/*  maps a code point to the glyph.					*/
/*									*/
/*  Bounding boxes are vertically oriented in the numeric orientation	*/
/*  of the numbers. I.E: Bottom:drY0, Top:drY1.				*/
/*									*/
/************************************************************************/

typedef struct AfmKerningPair
    {
    short int		akpPosition;
    short int		akpXVec;
    } AfmKerningPair;

typedef struct AfmCodeList
    {
    unsigned short int *	aclCodes;
    int				aclCodeCount;
    } AfmCodeList;

typedef struct AfmCharMetric
    {
			    /**
			     *  Potentially empty array of code points 
			     *  associated to this glyph. This is because it 
			     *  is possible to map more than one code point 
			     *  in the default encoding to the same glyph.
			     *  Only code values < 256 are saved to the 
			     *  afm file.
			     */
    AfmCodeList		acmDefaultCodeList;
    AfmCodeList		acmUnicodeCodeList;

    short int		acmWX;
    DocumentRectangle	acmBBox;
    char *		acmN;
			    /**
			     *  Index of the glyph in the font.
			     */
    short int		acmGlyphIndex;

    AfmKerningPair *	acmKernPairs;
    short int		acmKernPairCount;
    } AfmCharMetric;

typedef int (*ResolveFontMetrics)(	struct AfmFontInfo *	afi );

typedef struct AfmFontInfo
    {
			    /**
			     *  Must be unique in the font list. Use the 
			     *  index in the list.
			     */
    int			afiFaceNumber;

			    /**
			     *  The (technical) font name. This is the name 
			     *  that we use to refer to the font in PostScript.
			     */
    char *		afiFontName;

			    /**
			     *  The (visible) font name. This is the name 
			     *  that we show to the user and that we use 
			     *  to match external (RTF) font names to the font.
			     */
    char *		afiFullName;
    char *		afiFamilyName;
    char *		afiFamilyName_Orig;

			    /**
			     *  The style of the font: Somenting like
			     *  'Bold Oblique'. We try find it by comparing 
			     *  the full name with the family name. If that 
			     *  fails, we use afiWeightStr. The string is 
			     *  owned by another field: Do not free!
			     */
    char *		afiStyle;

			    /**
			     *  The weight of the font. (Bold)
			     */
    char *		afiWeightStr;
    short int		afiWeightInt;

			    /**
			     *  The width of the font. (Narrow, Condensed)
			     */
    char *		afiWidthStr;
    short int		afiWidthInt;

			    /**
			     *  The slant of the font.
			     */
    double		afiItalicAngle;
    double		afiTanItalicAngle;

			    /**
			     *  If true, all characters have the same width.
			     */
    unsigned char	afiIsFixedPitch;

			    /**
			     *  The copyright notice. Simply used to move it
			     *  around between font files and the metrics file.
			     */
    char *		afiNotice;
			    /**
			     *  The version string of the font. Simply used to
			     *  move it around between font files and the 
			     *  metrics file.
			     */
    char *		afiVersion;
			    /**
			     *  The font vendor name. Simply used to move it
			     *  around between font files and the metrics file.
			     */
    char *		afiVendor;

			    /**
			     *  The divisor that relates the metrics to the 
			     *  M-square. 1000 for PS fonts, 2048 for TTF fonts.
			     */
    int			afiUnitsPerEm;

    DocumentRectangle	afiFontBBox;
    int			afiUnderlinePosition;
    int			afiUnderlineThickness;
    int			afiCapHeight;
    int			afiXHeight;
    int			afiAscender;
    int			afiDescender;

    unsigned char	afiIgnoreKerning;

			    /**
			     *  Glyph Metrics information: Must be known 
			     *  as soon as text is formatted.
			     */
    int			afiMetricCount;
    AfmCharMetric **	afiMetrics;

			    /**
			     *  Map glyph names to glyph metrics.
			     */
    void *		afiNameToMetric;
    int			afiFallbackGlyph;

			    /**
			     *  Encoding Scheme as it is found in the 
			     *  file: More or less meningless.
			     */
    char *		afiEncodingScheme;
			    /**
			     *  Character Set as it is found in the 
			     *  file: More or less meningless.
			     */
    char *		afiCharacterSet;

			    /**
			     *  True if no encoding could be derived from 
			     *  the glyph names or from the encoding in 
			     *  the TTF file.
			     */
    unsigned char	afiFontSpecificEncoding;

			    /**
			     *  Resource name as it is found in the 
			     *  file. Meanongless here, merely used to 
			     *  move information around between font files 
			     *  and metric files.
			     */
    char *		afiResourceName;

    MemoryBuffer	afiFontFileName;
    int			afiFontFileIndex;

    IndexSet		afiUnicodesProvided;

			    /**
			     *  For a (partially?) unicode supporting font,
			     *  the mapping from unicode to glyph is stored 
			     *  here. [ We know that it should have been 
			     *  the other way round, but we do not know how 
			     *  to deal with that knowlege.
			     *
			     *  For a font with FontSpecific encoding,
			     *  the mapping is from the font specific 
			     *  code point to the glyph.
			     */
    IndexMapping	afiCodeToGlyphMapping;

			    /**
			     *  If the metrics information was retrieved 
			     *  from an AFM file, the name of the file 
			     *  is stored here. It is used to retrieve 
			     *  deferred metric information.
			     */
    MemoryBuffer	afiAfmFileName;

			    /**
			     *  True if and only if loading full font 
			     *  metrics was postponed for efficiency reasons.
			     */
    unsigned char	afiMetricsDeferred;

			    /**
			     *  If loading metrics has been deferred: 
			     *  call this function to obtain them later 
			     *  on.
			     */
    ResolveFontMetrics	afiResolveMetrics;

			    /********************************************/
			    /*  Hacked screen related functionality.	*/
			    /********************************************/
			    /**
			     *  To operate in absolutely archaic environments,
			     *  Ted keeps track of X11 fonts that match a font.
			     *  The administration is kept here.
			     */
    char **		afiX11Fonts;
    int			afiX11FontCount;
    } AfmFontInfo;

# define psFontHasUnicode( a, u ) \
		utilIndexSetContains( &((a)->afiUnicodesProvided), (u) )

/************************************************************************/
/*									*/
/*  Face information.							*/
/*									*/
/*  Used to map a users idea of fonts to implementation fonts.		*/
/*  Used by the font chooser to select fonts.				*/
/*									*/
/*  1)  Set width defines. The values math those in fontconfig.		*/
/*  2)  Set weight defines. The values math those in fontconfig.	*/
/*									*/
/************************************************************************/

/*  1  */
#   define	FONTwidthULTRACONDENSED		50
#   define	FONTwidthEXTRACONDENSED		63
#   define	FONTwidthCONDENSED		75
#   define	FONTwidthSEMICONDENSED		87
#   define	FONTwidthNORMAL			100
#   define	FONTwidthSEMIEXPANDED		113
#   define	FONTwidthEXPANDED		125
#   define	FONTwidthEXTRAEXPANDED		150
#   define	FONTwidthULTRAEXPANDED		200

#   define	FONTwidth_MIN			FONTwidthULTRACONDENSED
#   define	FONTwidth_MAX			FONTwidthULTRAEXPANDED

/*  2  */
#   define	FONTweightTHIN			0
#   define	FONTweightEXTRALIGHT		40
#   define	FONTweightLIGHT			50
#   define	FONTweightBOOK			75
#   define	FONTweightREGULAR		80
#   define	FONTweightMEDIUM		100
#   define	FONTweightDEMIBOLD		180
#   define	FONTweightBOLD			200
#   define	FONTweightEXTRABOLD		205
#   define	FONTweightBLACK			210

#   define	FONTweightULTRALIGHT		FONTweightEXTRALIGHT
#   define	FONTweightNORMAL		FONTweightREGULAR
#   define	FONTweightSEMIBOLD		FONTweightDEMIBOLD
#   define	FONTweightULTRABOLD		FONTweightEXTRABOLD
#   define	FONTweightHEAVY			FONTweightBLACK

#   define	FONTweight_MIN			FONTweightTHIN
#   define	FONTweight_MAX			FONTweightBLACK

/************************************************************************/

# define FONT_IS_BOLD( afi ) ((afi)->afiWeightInt >= FONTweightDEMIBOLD )
# define FONT_IS_SLANTED( afi ) ((afi)->afiItalicAngle < -1.0 )

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psGetCodeToGlyphMapping(	AfmFontInfo *		afi );

extern void psInitAfmFontInfo(		AfmFontInfo *		afi );
extern void psCleanAfmFontInfo(		AfmFontInfo *		afi );
extern void psFreeAfmFontInfo(		AfmFontInfo *		afi );
extern void psRemoveMetricsFromInfo(	AfmFontInfo *		afi );

extern int psFindAlternate(		const AfmFontInfo *	afi,
					const char *		glyphName );

extern int psFontInfoAddMetric(		AfmFontInfo *			afi,
					int				C,
					int				WX,
					const char *			N,
					const DocumentRectangle *	abb );

extern int psFontInfoGetGlyphIndex(	const AfmFontInfo *	afi,
					const char *		glyphName );

extern int psFontInfoSetGlyphName(	AfmFontInfo *		afi,
					int			glyphIndex,
					const char *		glyphName );

extern const char * psFontInfoGetGlyphName(
					const AfmFontInfo *	afi,
					int			glyphIndex );

extern int psFontInfoSetGlyphCode(	AfmFontInfo *		afi,
					int			glyphIndex,
					int			c );

extern int psFontInfoSetGlyphUnicode(	AfmFontInfo *		afi,
					int			glyphIndex,
					int			c );

extern int psAddX11FontToInfo(		AfmFontInfo *		afi,
					const char *		x11Font );

extern int psMapVisibleNamesToBase35(	AfmFontInfo *		afi );

#   endif
