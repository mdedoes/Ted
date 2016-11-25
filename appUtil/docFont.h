/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_H
#   define	DOC_FONT_H

#   include	<utilPropMask.h>
#   include	<utilTextAttribute.h>

/************************************************************************/
/*									*/
/*  Font Administration in terms of 'Document Fonts'			*/
/*									*/
/*  1)  A document font is characterized by an item in the font list	*/
/*	in an offce document. The main key is the name of the font.	*/
/*	It is possible that two fonts with the same name exist. In that	*/
/*	case, the character set helps to distinguish between them.	*/
/*									*/
/************************************************************************/

#   define FONTpitchDEFAULT		0
#   define FONTpitchFIXED		1
#   define FONTpitchVARIABLE		2

#   define FONTlenPANOSE		20

typedef struct DocumentFontInstance
    {
    TextAttribute	dfiAttribute;
    int			dfiPhysicalFont;
    int			dfiPsFaceNumber;
    } DocumentFontInstance;

typedef struct DocumentFont
    {
    char *			dfFamilyStyle;	/*  fnil, fswiss ..	*/
    char *			dfName;		/*  Helvetica,		*/
						/*  Helvetica Narrow.	*/
    char *			dfAltName;	/*  \\falt in rtf	*/
    int				dfDocFamilyNumber;/*  f0, f1 ...	*/

    int				dfPsFamilyNumber;

    int				dfInstanceCount;
    DocumentFontInstance *	dfInstances;

    int				dfCharset;	/*  fcharsetN		*/
    int				dfCodepage;	/*  cpgN		*/
    int				dfEncodingUsed;

    int				dfPitch;	/*  fprqN		*/
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

typedef struct DocumentFontList
    {
    int			dflCount;
    DocumentFont *	dflFonts;
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

/************************************************************************/
/*									*/
/*  Default attributes.							*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentFont(	DocumentFont *	df );

extern void docCleanDocumentFont(	DocumentFont *	df );

extern int docCopyDocumentFont(	DocumentFont *		to,
			const DocumentFont *	from );

extern void docInitFontList(	DocumentFontList *	dfl );

extern void docCleanFontList(	DocumentFontList *	dfl );

extern int docCopyFontList(	DocumentFontList *		to,
				const DocumentFontList *	from );

extern int utilFontFamilyStyle( const char * fontFamilyName );

extern DocumentFont *	docInsertFont(	DocumentFontList *	dfl,
					int			n,
					const DocumentFont *	df );

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
