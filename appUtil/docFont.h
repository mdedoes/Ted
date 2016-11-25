/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_H
#   define	DOC_FONT_H

#   include	<utilPropMask.h>
#   include	<utilTextAttribute.h>
#   include	<utilFontEncoding.h>

/************************************************************************/
/*									*/
/*  Font Administration in terms of 'Document Fonts'			*/
/*									*/
/*  1)  A document font is characterized by an item in the font list	*/
/*	in an offce document. The main key is the name of the font.	*/
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
    char *			dfFamilyStyle;	/*  fnil, fswiss ..	*/
    char *			dfName;		/*  Helvetica,		*/
    char *			dfAltName;	/*  \\falt in rtf	*/
    short int			dfDocFontNumber;/*  f0, f1 ...	*/

    short int			dfDocFamilyIndex;
    short int			dfEncodingSet;

    short int			dfCharset;	/*  fcharsetN		*/
    short int			dfCodepage;	/*  cpgN		*/

    short int			dfPsFamilyNumber;
    short int			dfPsFaceNumber[FONTface_COUNT];

    unsigned char		dfPitch;	/*  fprqN		*/
    unsigned char		dfUsed;		/*  2			*/
    char			dfPanose[FONTlenPANOSE+1];
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

    DFprop_COUNT
    } DocumentFontProperty;

typedef struct DocumentFontFamily
    {
    char *		dffFamilyName;
    int			dffFontForEncoding[ENCODINGps_COUNT];
    } DocumentFontFamily;

typedef struct DocumentFontList
    {
    int				dflFontCount;
    DocumentFont *		dflFonts;
    int				dflFamilyCount;
    DocumentFontFamily *	dflFamilies;
    } DocumentFontList;

typedef enum DocumentFontStyle
    {
    DFstyleFNIL= 0,
    DFstyleFROMAN,
    DFstyleFSWISS,
    DFstyleFMODERN,
    DFstyleFSCRIPT,
    DFstyleFDECOR,
    DFstyleFTECH,

    DFstyle_COUNT
    } DocumentFontStyle;

#define FACE_INDEX( isS, isB ) ( 2*( (isS) != 0 )+ ( (isB) != 0 ) )
#define FACE_BOLD( idx ) ( (idx) % 2 != 0 )
#define FACE_SLANTED( idx ) ( ((idx)/2) % 2 != 0 )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentFont(	DocumentFont *		df );

extern void docCleanDocumentFont(	DocumentFont *		df );

extern int docCopyDocumentFont(		DocumentFont *		to,
					const DocumentFont *	from );

extern void docInitFontList(		DocumentFontList *	dfl );

extern void docCleanFontList(		DocumentFontList *	dfl );

extern int docCopyFontList(		DocumentFontList *		to,
					const DocumentFontList *	from );

extern int utilFontFamilyStyle( const char * fontFamilyName );

extern DocumentFont *	docInsertFont(	DocumentFontList *		dfl,
					int				n,
					const DocumentFont *		df );

extern int utilFontCompareFaces(	const void *	veft1,
					const void *	veft2 );

extern int docGetFontByName(	DocumentFontList *	dfl,
				const char *		fontFamilyName,
				int			encoding );

extern int docMergeFontIntoFontlist(
				DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom );

extern int docFontSetFamilyStyle(	DocumentFont *	df,
					int		style );

extern int docFontSetFamilyName(	DocumentFont *	df,
					const char *	name );

extern int docFontSetAltName(		DocumentFont *	df,
					const char *	name );

#   endif	/*  DOC_FONT_H	*/
