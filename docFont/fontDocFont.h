/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_H
#   define	DOC_FONT_H

#   include	<utilIndexMapping.h>
#   include	<utilIndexSet.h>
#   include	<utilMemoryBuffer.h>
#   include	<textOfficeCharset.h>

struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Font Administration in terms of 'Document Fonts'			*/
/*									*/
/*  1)  A document font is characterized by an item in the font list	*/
/*	in an office document. The main key is the name of the font.	*/
/*	It is possible that two fonts with the same name exist. In that	*/
/*	case, the character set helps to distinguish between them.	*/
/*									*/
/*  2)  The administration distinguishes between fonts that are 'used'	*/
/*	and those that are not. For efficient PostScript production,	*/
/*	the distincion might be too coarse: Many fonts that have been	*/
/*	used count as used. The purpose of the administration is to	*/
/*	decide what fonts to save in a document. Typically the set of	*/
/*	fonts that count as used consistes of the fonts that were in	*/
/*	the font list of a document when it was opened plus the ones	*/
/*	that have been assigned to a stretch of text during editing.	*/
/*									*/
/*  10)	If the character set is given, (dfCharset) it is used to find	*/
/*	how the text is encoded. The actual index in the array of	*/
/*	office character sets is determined through a lookup. Otherwise	*/
/*	the index is derived from the code page (dfCodepage) or guessed	*/
/*	from the matching postscript font.				*/
/*									*/
/************************************************************************/

					/*  fprqN			*/
#   define FONTpitchDEFAULT		0
#   define FONTpitchFIXED		1
#   define FONTpitchVARIABLE		2

#   define FONTlenPANOSE		20

typedef enum FontFaceIndex
    {
    FONTfaceREGULAR= 0,
    FONTfaceBOLD,
    FONTfaceSLANTED,
    FONTfaceBOLD_SLANTED,

    FONTface_COUNT
    } FontFaceIndex;

typedef struct DocumentFont
    {
				    /**
				     *  Helvetica, Times..
				     */
    MemoryBuffer		dfName;

				    /**
				     *  \\falt in rtf
				     */
    MemoryBuffer		dfAltName;

				    /**
				     *  f0, f1, f2,,,,,
				     */
    short int			dfDocFontNumber;

				    /**
				     *  fnil, fswiss ..
				     */
    unsigned char		dfStyleInt;

				    /**
				     *  fprqN
				     */
    unsigned char		dfPitch;

    char			dfPanose[FONTlenPANOSE+1];

				    /**
				     *  (PostScript) font faces that 
				     *  are matched to the font for 
				     *  plain,bold,italic,bold-italic.
				     */
    struct AfmFontInfo *	dfPsFontInfo[FONTface_COUNT];

				    /**
				     *  Mostly used while reading/
				     *  opening/writing a document:
				     */
    IndexSet			dfUnicodesUsed;
    IndexMapping		dfUnicodeToCharset;

				    /**
				     *  Keep track uf the encodings that
				     *  we need to write text in this font.
				     *  The administration is built 
				     *  immediately before the document 
				     *  is saved.
				     *
				     *  It is also abused to keep track 
				     *  of  the encodings of the font 
				     *  while reading the document.
				     */
    int				dfCharsetIdxUsed[CHARSETidx_COUNT];
    int				dfCharsetIdxUsedCount;

				    /**
				     *  True, if and only if the font 
				     *  appears in the source document with
				     *  a font specific encoding.
				     */
    unsigned char		dfFontSpecificEncoding;

				    /**
				     *  \\ftnil or \\fttruetype
				     *  The type of embedded font.
				     */
    unsigned char		dfEmbeddedType;
#					define FONTtypeNONE	0
#					define FONTtypeNIL	1
#					define FONTtypeTTF	2
#					define FONTtype_COUNT	3

				    /**
				     *  {\\fontemb\\CDATA
				     *  Embedded Open Type (EOT) data
				     *  see http://www.w3.org/Submission/2008/SUBM-EOT-20080305/
				     *  for the format of the contents.
				     */
    MemoryBuffer		dfFontBytes;

				    /**
				     *  {\\fontemb{\\fontfile
				     *  The font file name
				     */
    MemoryBuffer		dfFontFile;

    } DocumentFont;

typedef enum DocumentFontProperty
    {
    DFpropFAMILY_STYLE= 0,
    DFpropNAME,
    DFpropALT_NAME,
    DFpropCHARSET,
    DFpropCODEPAGE,
    DFpropPITCH,
    DFpropPANOSE,

    DFpropFONT_TYPE,
    DFpropFONT_FILE,
    DFpropFONT_BYTES,

    DFprop_COUNT
    } DocumentFontProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void fontInitDocumentFont(	DocumentFont *		df );

extern void fontCleanDocumentFont(	DocumentFont *		df );

extern int fontCopyDocumentFont(	DocumentFont *		to,
					const DocumentFont *	from );

extern int fontSetFamilyStyle(		DocumentFont *	df,
					int		style );

extern int fontSetFamilyName(		DocumentFont *	df,
					const char *	name );

extern int fontSetAltName(		DocumentFont *	df,
					const char *	name );

extern void fontCopyFaceMatches(	DocumentFont *		dfTo,
					const DocumentFont *	dfFrom );

extern int fontFindLegacyEncodings(	DocumentFont *		df );

extern int fontRemoveCharsetFromFontName( DocumentFont *	df,
					int			charset );

extern int cssFontFamilyIndicator(	char *			target,
					int			maxSize,
					int			styleInt,
					const struct MemoryBuffer *	familyName );

#   endif	/*  DOC_FONT_H	*/
