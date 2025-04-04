/************************************************************************/
/*									*/
/*  Document Properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_PROPERTIES_H
#   define	DOC_DOCUMENT_PROPERTIES_H

#   include	<time.h>

#   include	<geoDocumentPageSetup.h>
#   include	<utilMemoryBuffer.h>

#   include	"docNotesProperties.h"

struct PropertyMask;
struct DocumentFontList;
struct ListAdmin;
struct DocumentAttributeMap;
struct RGB8Color;

typedef enum DocumentProperty
    {
    DPpropNONE= -1,

    DPpropSTART_PAGE= DGprop_COUNT,

    DPpropFACING_PAGES,
    DPpropGUTTER_HORIZONTAL,
    DPpropWIDOWCTRL,
    DPpropTWO_ON_ONE,
    DPpropDOCTEMP,
    DPpropRTOL,

    DPpropNOTETYPES,	/*  \fetN	*/

    DPpropFOOTNOTE_STARTNR,
    DPpropFOOTNOTE_JUSTIFICATION,
    DPpropFOOTNOTE_PLACEMENT,
    DPpropFOOTNOTE_RESTART,
    DPpropFOOTNOTE_STYLE,

    DPpropENDNOTE_STARTNR,
    DPpropENDNOTE_JUSTIFICATION,
    DPpropENDNOTE_PLACEMENT,
    DPpropENDNOTE_RESTART,
    DPpropENDNOTE_STYLE,

    DPpropDEFTAB,
    DPpropDOC_CHARSET,
    DPpropANSICPG,
    DPpropDEFLANG,

    DPpropDEFF,
    DPpropSTSHFDBCH,
    DPpropSTSHFLOCH,
    DPpropSTSHFHICH,
    DPpropSTSHFBI,

    DPpropGENERATOR,
    DPpropTITLE,
    DPpropSUBJECT,
    DPpropKEYWORDS,
    DPpropDOCCOMM,
    DPpropAUTHOR,
    DPpropCOMPANY,
    DPpropHLINKBASE,

    DPpropCREATIM,
    DPpropREVTIM,
    DPpropPRINTIM,

    DPpropTOP_BORDER,
    DPpropBOTTOM_BORDER,
    DPpropLEFT_BORDER,
    DPpropRIGHT_BORDER,
    DPpropHEAD_BORDER,
    DPpropFOOT_BORDER,

    DPpropFONTTABLE,
    DPpropCOLORTABLE,
    DPpropSTYLESHEET,
    DPpropLISTTABLE,
    DPpropLISTOVERRIDETABLE,
    DPpropUSERPROPS,

    DPprop_COUNT,

    DPprop_INFO,
    DPprop_REVTBL,

    DPprop_OPERATOR,
    DPprop_VERSION,
    DPprop_EDMINS,
    DPprop_NOFPAGES,
    DPprop_NOFWORDS,
    DPprop_NOFCHARS,
    DPprop_NOFCHARSWS,
    DPprop_VERN,

				/****************************************/
    DPprop_IGNORED,		/*  Various document properties that	*/
				/*  are ignored, but accepted to avoid	*/
				/*  warnings.				*/
				/****************************************/

    DPprop_FULL_COUNT
    } DocumentProperty;

typedef struct DocumentProperties
    {
    DocumentGeometry	dpGeometry;

    int			dpTabIntervalTwips;
    int			dpDefaultColor;
    int			dpDocumentCharset; /* ansi, mac, pc, pca */
    int			dpAnsiCodepage;
    int			dpDefaultLocaleId;

    int			dpDefaultFont;
    int			dpDefaultFontDbch;
    int			dpDefaultFontLoch;
    int			dpDefaultFontHich;
    int			dpDefaultFontBi;

    int			dpStartPageNumber;

			/**
			 *  This flag controls two things:
			 *  - Use different page headers and footers 
			 *    on odd and on even pages.
			 *  - Obey the break property of sections that 
			 *    start on odd or on even pages.
			 */
    unsigned char	dpHasFacingPages;
    unsigned char	dpGutterHorizontal;
    unsigned char	dpWidowControl;
    unsigned char	dpTwoOnOne;

			/**
			 *  The document is a template: Invite the user 
			 *  to save it under a different name by forgetting 
			 *  the file name. (And that it is a template.)
			 */
    unsigned char	dpIsDocumentTemplate;

			/**
			 * 0: This document will have English-style
			 *	pagination (the default)
			 *    Text in this document will be displayed from
			 *	left to right unless overridden by a more
			 *	specific control (the default)
			 * 1: This document will be formatted to have
			 * 	Arabic-style pagination
			 *    Text in this document will be displayed from
			 *	right to left unless overridden by a more
			 *	specific control
			 */
    unsigned char	dpRToL;

    unsigned char		dpFootEndNoteType;
    FootEndNotesProperties	dpNotesProps;

			/*  Owned by the document */
    struct DocumentFontList *	dpFontList;
    struct ListAdmin *		dpListAdmin;
    struct ColorPalette *	dpColorPalette;

    struct tm		dpCreatim;
    struct tm		dpRevtim;
    struct tm		dpPrintim;

    MemoryBuffer	dpGeneratorRead;
    MemoryBuffer	dpGeneratorWrite;
    MemoryBuffer	dpTitle;
    MemoryBuffer	dpSubject;
    MemoryBuffer	dpKeywords;
    MemoryBuffer	dpDoccomm;
    MemoryBuffer	dpAuthor;
    MemoryBuffer	dpCompany;
    MemoryBuffer	dpHlinkbase;
    MemoryBuffer	dpFilename;

    int			dpTopBorderNumber;
    int			dpBottomBorderNumber;
    int			dpLeftBorderNumber;
    int			dpRightBorderNumber;
    int			dpHeadBorderNumber;
    int			dpFootBorderNumber;

			/**
			 *   Only used in Ted's own copy paste to distinguish
			 *   between a copy/paste that just holds a text 
			 *   fragment and one that consists of whole paragraphs
			 */
    unsigned char	dpHasOpenEnd;
			/**
			 *   Only used in Ted's own copy paste to know that
			 *   a pasted document contains tables.
			 */
    unsigned char	dpContainsTables;
    } DocumentProperties;

#   define	docHasDocumentInfo( dp )		\
		( (dp)->dpTitle.mbSize > 0	||	\
		  (dp)->dpSubject.mbSize > 0	||	\
		  (dp)->dpKeywords.mbSize > 0	||	\
		  (dp)->dpDoccomm.mbSize > 0	||	\
		  (dp)->dpHlinkbase.mbSize > 0	||	\
		  (dp)->dpAuthor.mbSize > 0	||	\
		  (dp)->dpCompany.mbSize > 0	||	\
		  (dp)->dpCreatim.tm_mday != 0	||	\
		  (dp)->dpRevtim.tm_mday != 0	||	\
		  (dp)->dpPrintim.tm_mday != 0	)

#   define	DOCcolorBITS		8
#   define	DOCcolorMAXCOUNT	(1<<DOCcolorBITS)

extern const int DOCdocNOTE_PROP_MAP[FEPprop_COUNT];

extern const int * const DOCdocFOOTNOTE_PROP_MAP;
extern const int * const DOCdocENDNOTE_PROP_MAP;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentProperties(	DocumentProperties *		dp );

extern void docCleanDocumentProperties(	DocumentProperties *		dp );

extern int docCopyDocumentProperties(	DocumentProperties *		to,
					const DocumentProperties *	from );

extern int docUpdDocumentProperties(
			struct PropertyMask *			pDoneMask,
			DocumentProperties *			dpTo,
			const struct PropertyMask *		dpSetMask,
			const DocumentProperties *		dpFrom,
			const struct DocumentAttributeMap *	dam );

extern void docDocumentPropertyDifference(
				struct PropertyMask *		pDifMask,
				const DocumentProperties *	dp1,
				const struct PropertyMask *	cmpMask,
				const DocumentProperties *	dp2 );

extern int docPropertiesSetFilename(	DocumentProperties *	dp,
					const MemoryBuffer *	filename );

extern int docAllocateDocumentColor(
				DocumentProperties *		dp,
				const struct RGB8Color *	rgb8 );

extern int docSetDocumentProperty(	DocumentProperties *	dp,
					int			prop,
					int			arg );

extern int docGetDocumentProperty(	const DocumentProperties *	dp,
					int				prop );

extern struct tm * docGetDocumentPropertyTime(
					DocumentProperties *	dp,
					int			prop );

extern MemoryBuffer * docGetDocumentPropertyBuffer(
					DocumentProperties *	dp,
					int			prop );

extern int docSetDocumentPropertyString( DocumentProperties *	dp,
					int			prop,
					const char *		val,
					int			vallen );

extern int docSetDocumentPropertyTime(	DocumentProperties *	dp,
					int			prop,
					const struct tm *	val );

extern void docFillDocFootnoteMask(		struct PropertyMask *	dpMask );
extern void docFillDocEndnoteMask(		struct PropertyMask *	dpMask );
extern void docFillDocNotesMask(		struct PropertyMask *	dpMask );

#   endif	/*  DOC_DOCUMENT_PROPERTIES_H  */
