/************************************************************************/
/*									*/
/*  Document Properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_PROPERTIES_H
#   define	DOC_DOCUMENT_PROPERTIES_H

#   include	<time.h>

#   include	<utilPropMask.h>

#   include	<utilDocumentGeometry.h>
#   include	<appUnit.h>
#   include	<docFont.h>
#   include	<bmcolor.h>

#   include	"docListTable.h"
#   include	"docListOverrideTable.h"

#   define	DPfetFOOT_ONLY		0
#   define	DPfetEND_ONLY		1
#   define	DPfetFOOT_END_BOTH	2

typedef enum NotesPosition
    {
    DPftnPOS_SECT_END= 0,
    DPftnPOS_DOC_END,
    DPftnPOS_BELOW_TEXT,
    DPftnPOS_PAGE_BOTTOM,

    DPftnPOS__COUNT
    } NotesPosition;

typedef enum NotesRestart
    {
    DPftnRST_CONTINUOUS= 0,
    DPftnRST_PER_SECTION,
    DPftnRST_PER_PAGE,

    DPftnRST__COUNT
    } NotesRestart;

typedef enum NoteNumberStyle
    {
    DPftnNAR= 0,
    DPftnNALC,
    DPftnNAUC,
    DPftnNRLC,
    DPftnNRUC,
    DPftnNCHI,

    DPftn_NCOUNT
    } NoteNumberStyle;

typedef enum DocumentProperty
    {
    DPpropNONE= -1,

    DPpropSTART_PAGE= DGprop_COUNT,
    DPpropFACING_PAGES,

    DPpropWIDOWCTRL,

    DPpropNOTETYPES,	/*  \fetN	*/

    DPpropFOOTNOTE_STARTNR,
    DPpropFOOTNOTE_POSITION,
    DPpropFOOTNOTE_RESTART,
    DPpropFOOTNOTE_STYLE,

    DPpropENDNOTE_STARTNR,
    DPpropENDNOTE_POSITION,
    DPpropENDNOTE_RESTART,
    DPpropENDNOTE_STYLE,

    DPpropDEFTAB,
    DPpropDEFF,
    DPpropDOC_CHARSET,
    DPpropANSICPG,

    DPpropTITLE,
    DPpropSUBJECT,
    DPpropKEYWORDS,
    DPpropCOMMENT,
    DPpropDOCCOMM,
    DPpropAUTHOR,
    DPpropHLINKBASE,

    DPpropCREATIM,
    DPpropREVTIM,
    DPpropPRINTIM,

    DPpropLISTTABLE,
    DPpropLISTOVERRIDETABLE,

    DPprop_COUNT
    } DocumentProperty;

typedef struct NotesProperties
    {
    int			npStartNumber;
    unsigned int	npPosition:2;
    unsigned int	npRestart:2;
    unsigned int	npNumberStyle:3;
    } NotesProperties;

typedef struct DocumentProperties
    {
    int			dpContainsTables;
    int			dpTabIntervalTwips;
    int			dpDefaultColor;
    int			dpDefaultFont;
    int			dpDocumentCharset; /* ansi, mac, pc, pca */
    int			dpAnsiCodepage;

    int			dpStartPageNumber;

    unsigned int	dpHasFacingPages:1;
    unsigned int	dpWidowControl:1;

    unsigned int	dpFootEndNoteType:2;
    NotesProperties	dpFootnoteProperties;
    NotesProperties	dpEndnoteProperties;

    DocumentGeometry	dpGeometry;
    DocumentFontList	dpFontList;

    DocumentListTable	dpListTable;
    ListOverrideTable	dpListOverrideTable;

    RGB8Color *		dpColors;
    int			dpColorCount;

    unsigned char **	dpAuthors;
    int			dpAuthorCount;

    struct tm		dpCreatim;
    struct tm		dpRevtim;
    struct tm		dpPrintim;

    unsigned char *	dpTitle;
    unsigned char *	dpSubject;
    unsigned char *	dpKeywords;
    unsigned char *	dpComment;
    unsigned char *	dpDoccomm;
    unsigned char *	dpAuthor;
    unsigned char *	dpHlinkbase;

    unsigned char *	dpFilename;
    } DocumentProperties;

#   define	docHasDocumentInfo( dp )		\
		( (dp)->dpTitle			||	\
		  (dp)->dpSubject		||	\
		  (dp)->dpKeywords		||	\
		  (dp)->dpComment		||	\
		  (dp)->dpDoccomm		||	\
		  (dp)->dpHlinkbase		||	\
		  (dp)->dpAuthor		||	\
		  (dp)->dpCreatim.tm_mday != 0	||	\
		  (dp)->dpRevtim.tm_mday != 0	||	\
		  (dp)->dpPrintim.tm_mday != 0	)

#   define	DOCcolorBITS		8
#   define	DOCcolorMAXCOUNT	(1<<DOCcolorBITS)

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
				PropertyMask *			pChgPask,
				DocumentProperties *		dp,
				const PropertyMask *		updMask,
				const DocumentProperties *	dpNew );

extern int docPropertiesSetFilename(	DocumentProperties *	dp,
					const char *		filename );

extern int docAllocateDocumentColor(	DocumentProperties *	dp,
					const RGB8Color *	rgb8 );

#   endif	/*  DOC_DOCUMENT_PROPERTIES_H  */
