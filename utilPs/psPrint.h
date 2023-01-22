#   ifndef	UTIL_PS_H
#   define	UTIL_PS_H

#   include	<geoRectangle.h>
#   include	<geoAffineTransform.h>
#   include	"psNup.h"
#   include	<utilMemoryBuffer.h>

struct SimpleOutputStream;
struct SimpleInputStream;
struct AfmFontInfo;
struct PostScriptFontList;
struct PostScriptTypeList;
struct TextAttribute;
struct RGB8Color;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  The state of a printing job.					*/
/*									*/
/************************************************************************/

struct StructItem;

typedef struct PrintingState
    {
				/**
				 * The output stream that receives the
				 * actual PostScript that we emit
				 */
    struct SimpleOutputStream *	psSos;

				/**
				 * The number (counting from 0) in the
				 * document of the page that received
				 * something visible. Empty pages are
				 * only counted when a subsequent page
				 * receives visible content.
				 */
    int				psLastPageMarked;
    int				psLastSheetMarked;
    int				psPagesPrinted;
    int				psSheetsPrinted;
    int				psSheetsStarted;

    const struct PostScriptFontList *	psPostScriptFontList;
				/**
				 *  The font that is currently set.
				 *  It is cached to avoid setting the
				 *  same font over-and-over again.
				 */
    const struct AfmFontInfo *		psCurrentFontInfo;

    PrintGeometry		psPrintGeometry;
    NupSchema			psNupSchema;
    int				psRotateSheetGrid;

    AffineTransform2D		psCurrentTransform;
    const char *		psOrientation;

    DocumentRectangle		psSheetBoundingBox;

    int				psInsideLink;
    int				psLinkParticulesDone;
    int				psLinkRectLeft;

    MemoryBuffer		psLinkFile;
    MemoryBuffer		psLinkMark;
    MemoryBuffer		psLinkTitle;

    unsigned char		psUsePostScriptFilters;
    unsigned char		psUsePostScriptIndexedImages;
    unsigned char		ps7Bits;

				/**
				 * If set, we emit PDF marks that tag
				 * pieces of the document in a way that tries
				 * to be compliant with ISO 14289-1 (PDF/UA).
				 */
    unsigned char		psTagDocumentStructure;
				/**
				 * The number of marked content items in
				 * the document as a whole. We create a
				 * unique named dictionary for each StructElem
				 * in the document. This is used to generate
				 * the unique names.
				 */
    int				psDocContentMarkCount;
				/**
				 * The number of marked content items on
				 * the current page. As we produce a content
				 * stream per page, and marked content ID's
				 * (MCID) must be unique within the stream,
				 * we use this to produce a contiguous series
				 * of ids. See ISO 32000-1:2008, 14.7.4.2.
				 * (second bullet).
				 */
    int				psPageContentMarkCount;
				/**
				 * The (potential) first ID of marked content
				 * on the current page. psPageFirstMarkId is
				 * only meaningful if there are marks on the
				 * page.
				 */
    int				psPageFirstMarkId;

				/**
				 * We keep a stack of StructItem's to produce
				 * a document structure that is similar to the
				 * HTML structure that corresponds to the document.
				 * In a few places, this is also used in tricks to
				 * conform to the requirements of marked PDF.
				 * E.G. to avoid nesting marked content and to
				 * avoid inserting Artifacts inside marked content.
				 *
				 * NOTE 2 in ISO 32000-1:2008, 14.6.1 implicitly requires
				 * marked content to be inside a content stream. As we have a
				 * content stream per page, the marked content must not span
				 * pages. So, at a page boundary we must pop the whole stack.
				 * If necessary, we must push unfinished nodes in the
				 * document hierarchy onto the stack on the next page again.
				 */
    struct StructItem *		psCurrentStructItem;
    } PrintingState;

typedef struct StructItem
    {
    MemoryBuffer	siDictionaryName;
    struct StructItem *	siParent;
    const char *	siStructureType;
    int			siIsLeaf;
    } StructItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void psStartDSCDocument(
			const PrintingState *		ps,
			const struct PostScriptTypeList *	psfl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference );

extern void psRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage );

extern int psSetNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter );

extern int psPrintString(	struct SimpleOutputStream *	sos,
				const char *			s,
				int				len,
				int				sevenBits,
				int				utf8 );

extern int psPrintPdfMarkStringValue(
				PrintingState *			ps,
				const struct MemoryBuffer *	mb );

extern int psPrintStringValue(		PrintingState *		ps,
					const char *		s,
					int			len );

extern int psMoveShowString(		PrintingState *		ps,
					const char *		s,
					int			len,
					int			x,
					int			y );

extern int psShowString(		PrintingState *		ps,
					const char *		s,
					int			len );

extern int psSetFont(		PrintingState *		ps,
				const struct AfmFontInfo *	afi,
				const struct TextAttribute *	ta );

extern void psInitPrintingState(	PrintingState *	ps );
extern void psCleanPrintingState(	PrintingState *	ps );

extern int psSetRgb8Color(	PrintingState *		ps,
				const struct RGB8Color *	rgb8 );

extern int psStartPage(		PrintingState *		ps,
				int			documentPage );

extern int psFinishPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psAbortPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psSelectFontProcedures(
				struct SimpleOutputStream *		sos,
				const struct PostScriptTypeList *	psfl,
				int				allFonts );

extern int psIncludeFonts(	struct SimpleOutputStream *		sos,
				const struct PostScriptTypeList *	psfl );

extern void psStartEpsFile(	struct SimpleOutputStream *	sos,
				const char *			creator,
				const char *			title,
				int				pointsWide,
				int				pointsHigh );

extern void psFinishEpsFile(	struct SimpleOutputStream *	sos );

extern void psSetPdfmarkEmulation(		struct SimpleOutputStream *	sos );
extern void psImageQualityDistillerparams(	struct SimpleOutputStream *	sos );
extern void psSetRectfillEmulation(		struct SimpleOutputStream *	sos );
extern void psSetSelectfontEmulation(		struct SimpleOutputStream *	sos );
extern void psSetDefinePageFontImplementation(	struct SimpleOutputStream *	sos );
extern void psSetUtf8ShowImplementation(	struct SimpleOutputStream *	sos );
extern void psSetMvsImplementation(		struct SimpleOutputStream *	sos );
extern void psSetPatternImplementation(	struct SimpleOutputStream *	sos,
					double			shadingMesh );

extern void psDefineProcedure(	struct SimpleOutputStream *	sos,
				const char **			lines,
				int				count );

extern void psDefineEpsProcs(	struct SimpleOutputStream *	sos );

extern void psBeginEpsObject(	struct SimpleOutputStream *	sos,
				const struct DocumentRectangle * drTo,
				const struct DocumentRectangle * drBBox,
				const struct MemoryBuffer *	fileName );

extern void psEndEpsObject(	struct SimpleOutputStream *	sos );

extern int psDestPdfmark(	PrintingState *			ps,
				int				lineTop,
				const struct MemoryBuffer *	mbRef );

extern int psEmitDestPdfmark(	struct SimpleOutputStream *	sos,
				const AffineTransform2D *	at,
				int				lineTop,
				const struct MemoryBuffer *	mbRef );

extern void psTransformMatrix(		struct SimpleOutputStream *		sos,
					const AffineTransform2D *	at );

extern void psFillRectangle(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psRectanglePath(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psFlushLink(		PrintingState *		ps,
					int			x1,
					int			lineTop,
					int			lineHeight );

extern void psSourcePdfmark(	struct SimpleOutputStream *	sos,
				const DocumentRectangle *	drLink,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	markName );

extern void psGotoPdfmark(	struct SimpleOutputStream *	sos,
				const DocumentRectangle *	drLink,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	markName,
				const struct MemoryBuffer *	title );

extern int psPageModePdfmark(	struct SimpleOutputStream *	sos,
				const char *			pageMode );

extern int psSaveEpsFile(	struct SimpleOutputStream *	sos,
				DocumentRectangle *		drBBox,
				const struct MemoryBuffer *	filename );

extern int psIncludeEpsFile(	struct SimpleOutputStream *	sos,
				struct SimpleInputStream *	sis );


extern void psDscListFontNames(	struct SimpleOutputStream *		sos,
				int				embedFonts,
				const struct PostScriptTypeList *	pstl );

extern void psSetFontName(	char *				fontName,
				const struct AfmFontInfo *	afi );

extern int psSetToPdfCommand(	struct MemoryBuffer *		command,
				int				pageTwipsWide,
				int				pageTwipsHigh,
				const struct MemoryBuffer *	filename );

extern int psSetTestToPdfCommand( struct MemoryBuffer *		command );

extern int psOutlinePdfmark(	PrintingState *			ps,
				int				childCount,
				int				closed,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	markName );

extern int psSetPageProperty(	PrintingState *			ps,
				const char *			key,
				const char *			value );

extern int psSetCatalogProperty( PrintingState *		ps,
				const char *			key,
				const char *			value );

extern int psPdfMarkPosition(	PrintingState *			ps,
				const char *			structureType,
				int				contentId );

extern int psPdfBeginMarkedContent( PrintingState *		ps,
				const char *			structureType,
				int				contentId );

extern int psPdfBeginArtifact(	PrintingState *			ps,
				const char *			typeName,
				const char *			subtypeName,
				int				contentId );

extern int psPdfBeginFigure(	PrintingState *			ps,
				const struct MemoryBuffer *	altText,
				int				contentId );

extern int psPdfEndMarkedContent( PrintingState *		ps );

extern int psPdfmarkAppendContentToReadingOrder(
				PrintingState *			ps,
				const char *			structureType,
				int				contentId );

extern int psPdfmarkMarkedDocumentSetup(
				PrintingState *			ps,
				const char *			localeTag );

extern int psPdfmarkMarkedDocumentTrailer(
				PrintingState *			ps );

extern int psPdfmarkMarkedPageSetup(
				PrintingState *			ps,
				int				page );

extern int psPdfmarkFinishMarkedPage(
				PrintingState *			ps,
				int				page );

extern int psNewContentId(	PrintingState *			ps );

#   endif	/*  UTIL_PS_H  */
