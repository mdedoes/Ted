/************************************************************************/
/*									*/
/*  Levels in the Lists of the list table of a document.		*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_LEVEL_H
#   define	DOC_LIST_LEVEL_H

#   include	<utilPropMask.h>
#   include	<utilTextAttribute.h>

#   include	"docTabStop.h"
#   include	"docParaProperties.h"

#   define	DLmaxLEVELS	9

typedef enum ListLevelAlignment
    {
    DOCllaLEFT= 0,
    DOCllaCENTERED,
    DOCllaRIGHT,

    DOClla_COUNT
    } ListLevelAlignment;

typedef enum ListLevelFollow
    {
    DOCllfTAB= 0,
    DOCllfSPACE,
    DOCllfNONE,

    DOCllf_COUNT
    } ListLevelFollow;

typedef struct DocumentListLevel
    {
    int			dllStartAt;
    unsigned char	dllNumberStyle;
    unsigned char	dllJustification;
    unsigned char	dllFollow;
    unsigned char	dllPrevToDigits;
    unsigned char	dllNoRestart;
    int			dllPictureNumber:8;
    unsigned char	dllFontBias;

    unsigned char *	dllText;
    long		dllTemplateID;
    unsigned char *	dllNumbers;
    unsigned char	dllTextSize;
    unsigned char	dllNumberSize;

    unsigned char	dllFromOld;		/*  O  */
    unsigned char	dllUsePrevText;		/*  O  */
    unsigned char	dllUsePrevSpace;	/*  O  */
    int			dllIndent;		/*  O  */
    int			dllSpace;		/*  O  */

    TabStopList		dllTabStopList;
    int			dllLeftIndentTwips;
    int			dllFirstIndentTwips;
    PropertyMask	dllParaPropertyMask;

    TextAttribute	dllTextAttribute;
    PropertyMask	dllTextAttributeMask;
    } DocumentListLevel;

typedef enum ListLevelProperty
    {
    LLpropSTARTAT,
    LLpropSTYLE,
    LLpropJUSTIFY,
    LLpropFOLLOW,
    LLpropLEGAL,
    LLpropNORESTART,
    LLpropPICTURE,
    LLpropFBIAS,

    LLpropTEXT,
    LLpropTEMPLATEID,
    LLpropNUMBERS,

    LLpropOLD,
    LLpropPREV,
    LLpropPREVSPACE,
    LLpropINDENT,
    LLpropSPACE,

    LLprop_COUNT
    } ListLevelProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentListLevel(	DocumentListLevel *		dll );
extern void docCleanDocumentListLevel(	DocumentListLevel *		dll );

extern int docCopyDocumentListLevel(
				DocumentListLevel *		to,
				const DocumentListLevel *	from,
				const int *			fontMap,
				const int *			colorMap );

extern int documentListLevelSetStyle(
				DocumentListLevel *		dll,
				const PropertyMask *		paraMask,
				const ParagraphProperties *	pp,
				const PropertyMask *		textMask,
				const TextAttribute *		ta );

#   endif
