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
    const struct AfmFontInfo *	psCurrentFontInfo;

    PrintGeometry		psPrintGeometry;
    NupSchema			psNupSchema;

				/**
				 *  As shading is done in device coordinates and not in 
				 *  user coordinate space, shading must be rotated with 
				 *  the contents of the page.
				 *
				 *  The same applies for the nup schema.
				 */
    int				psRotateSheetGrid;

    AffineTransform2D		psCurrentTransform;
    const char *		psOrientation;

    DocumentRectangle		psSheetBoundingBox;

				/**
				 *  Non-zero while we are emitting 
				 *  the content of a hyperlink
				 */
    int				psInsideLink;

				/**
				 *  Non-zero while we are emitting 
				 *  the content of list label. (Bullet)
				 */
    int				psInsideListLabel;

				/**
				 *  The number of text particules that
				 *  we emitted inside the current hyperlink.
				 */
    int				psLinkParticulesDone;
				/**
				 *  The x coordinate where we started to emit
				 *  the current hyperlink. We use this to mark 
				 *  the click rectangle of the hyperlink.
				 */
    int				psLinkRectLeft;

				/**
				 *  The 'file' part of the current hyperlink
				 *  destination. It refers to the destination document
				 *  as a while.
				 */
    MemoryBuffer		psLinkFile;

				/**
				 *  The bookmark part of the current hyperlink.
				 *  It refers to a location inside the document.
				 *  This is the equivalent of the HTML fragment identifier.
				 */
    MemoryBuffer		psLinkMark;

				/**
				 *  The title for the current hyperlink.
				 *  Currently, it is only used for notes.
				 */
    MemoryBuffer		psLinkTitle;

				/**
				 *  This is the name of the annotation dictionary.
				 *  that implements the current (hyper)link.
				 *  We start an annotation at the beginning of the 
				 *  link end we end it at the end of the link.
				 *  When the link spans multiple lines, we end the 
				 *  annotation at the end of the line and we start a new
				 *  one on the next line.
				 */
    MemoryBuffer		psAnnotationDictionaryName;

    unsigned char		psUsePostScriptFilters;
    unsigned char		psUsePostScriptIndexedImages;

				/**
				 * If set, we emit PDF marks that tag
				 * pieces of the document in a way that tries
				 * to be compliant with ISO 14289-1 (PDF/UA).
				 */
    unsigned char		psTagDocumentStructure;

				/**
				 * Omit the marks that mark the pieces in the 
				 * content stream. In this way, the most difficult
				 * work in generating PostScript that can be converted 
				 * to PDF/AU is done. Without marking the content, the 
				 * the resulting document is NOT PDF/UA compliant however.
				 * This is only relevant in combination with psTagDocumentStructure.
				 * We want to retain the original name of psTagDocumentStructure.
				 * This makes the name a bit silly.
				 */

    unsigned char		psOmitContentMarks;

				/**
				 * Insert XMP metadata that tells the document is PDF/UA compliant.
				 * As this is done through a GhostScript specific pdfmark,
				 * this must be explicitly enabled.
				 *
				 * Note that with psOmitContentMarks, the document is NOT Pdf/UA
				 * compliant.
				 */
    unsigned char		psDeclareUACompliant;


				/**
				 * In combination with psTagDocumentStructure!=0,
				 * we mark invisible text as an artifact.
				 */
    unsigned char		psMarkInvisibleAsArtifact;

				/**
				 * In combination with psTagDocumentStructure!=0,
				 * If this flag is set, tables without any borders or shading
				 * will not be marked as tables. (Nor will the rows, cells or headers.)
				 */
    unsigned char		psFlattenPlainTables;

				/**
				 * Conform to 4.2.6.2 In https://pdfa.org/resource/tagged-pdf-best-practice-guide-syntax/
				 * and emit repeated table headers in an artifact.
				 */
    unsigned char		psRepeatTableHeadersAsArtifact;

				/**
				 *  Keep track of whether we are in an /Artifact in the
				 *  creation of a PDF structure. Only relevant in combination
				 *  with psTagDocumentStructure != 0.
				 *  psInArtifact is managed by incrementing/decrementing. So
				 *  it behaves like a stack. We try not to bother code that 
				 *  is unrelated with the details of this. So usually, we 
				 *  simply simply check for psInArtifact != 0.
				 */
    int				psInArtifact;

				/**
				 * The number of dictionaries that we created.
				 * This is used to get unique dictionary names.
				 * For the sake of understandability, the names
				 * hold a little mode than the unique number.
				 */
    int				psDictionaryNameCount;

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
				 * The number of annotations on the current page.
				 * this is used to give them a sequence number.
				 */
    int				psPageAnnotationCount;

				/**
				 * The number of pages/annotations in the document.
				 * This is used to give them a sequence number.
				 */
    int				psDocNumberTreeItemCount;

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
			    /**
			     *  The name of the StructItem dictionary
			     */
    MemoryBuffer	siDictionaryName;

			    /**
			     *  The name of the array with children of
			     *  this StructItem. It is only used in 
			     *  StructItems that implement a group.
			     *  This is referenced as the /K member.
			     */
    MemoryBuffer	siChildArrayName;

			    /**
			     *  For links, this is the name of the
			     *  corresponding annotation dictionary.
			     */
    MemoryBuffer	siAnnotationDictionaryName;

			    /**
			     *  The parent of this StructItem
			     */
    struct StructItem *	siParent;

			    /**
			     *  The type of structure
			     */
    const char *	siStructureType;

			    /**
			     *  The content id (MCID) of the marked 
			     *  content that matches this StructItem
			     *  Only used in leaves.
			     */
    int			siContentId;
			    /**
			     *  True if this StructItem is a leaf in the tree.
			     */
    int			siIsLeaf;
			    /**
			     *  True if this StructItem is a textual (inline)
			     *  leaf in the tree. E.G. a /Span or a /Link 
			     *  StructItem.
			     */
    int			siIsInline;

			    /**
			     *  Indicates that the language of this StructItem is
			     *  explicitly given.
			     */
    const char *	siLanguageTag;
    } StructItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char PS_PDF_UA_XMP[];

extern void psPdfInitStructItem(
			StructItem *			si );
extern void psPdfCleanStructItem(
			StructItem *			si );

extern void psPdfPopStructItem(	PrintingState * 	ps );

extern int psPdfPushStructItem(	PrintingState *	ps,
				StructItem *	structItem );

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
				struct SimpleOutputStream *	sos,
				const struct MemoryBuffer *	mb );

extern int psPrintStringValue(	struct SimpleOutputStream *	sos,
				const char *			s,
				int				len,
				int				utf8 );

extern int psMoveShowString(		PrintingState *		ps,
					const char *		s,
					int			len,
					int			x,
					int			y );

extern int psShowString(		PrintingState *		ps,
					const char *		s,
					int			len );

extern int psSetFont(		PrintingState *			ps,
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

extern void psFlushLink(	PrintingState *		ps,
				int			x1,
				int			lineTop,
				int			lineHeight );

extern void psSetLinkRectangle(	PrintingState *		ps,
				int			x1Twips,
				int			lineTop,
				int			lineHeight,
				const char *		annotationDictionaryName );

extern int psPdfmarkDefineAnnotationDictionary(
				PrintingState *			ps,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName,
				const MemoryBuffer *		contents,
				const char *			annotationDictionaryName );

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
				int				contentId,
				const MemoryBuffer *		extraProperties );

extern int psPdfBeginMarkedContentActualText(
				PrintingState *			ps,
				const char *			structureType,
				int				contentId,
				const struct MemoryBuffer *	actualText );

extern int psPdfBeginMarkedFigure(
			PrintingState *				ps,
			const char *				structureType,
			int					contentId,
			const struct DocumentRectangle *	drTwips,
			const struct MemoryBuffer *		altText );

extern int psPdfBeginTypedArtifact(
				PrintingState *			ps,
				const char *			typeName,
				const char *			subtypeName );

extern int psPdfEndMarkedContent( PrintingState *		ps,
				const char *			structureType );

extern int psPdfmarkAppendMarkedLeaf(
				PrintingState *			ps,
				StructItem *			structItem,
				const struct MemoryBuffer *	structureAttributes );

extern int psPdfmarkAppendMarkedGroup(
				PrintingState *			ps,
				StructItem *			structItem,
				const struct MemoryBuffer *	structureAttributes );

extern int psPdfmarkAppendMarkedLink(
				PrintingState *			ps,
				StructItem *			structItem );

extern int psPdfmarkAppendMarkedIllustration(
			PrintingState *				ps,
			StructItem *				structItem,
			const struct DocumentRectangle *	drTwips,
			const struct MemoryBuffer *		altText );

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

extern struct StructItem * psPdfLeafStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine );

extern struct StructItem * psPdfAnnotatedStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine );

extern struct StructItem * psPdfGroupStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			contentId );

extern int psPdfMarkSetActualText(
				PrintingState *		ps,
				struct StructItem *	structItem,
				const struct MemoryBuffer * mbActualText );

extern int psPrintPdfmarkTextEntry(
				struct SimpleOutputStream *	sos,
				const char *			key,
				const struct MemoryBuffer *	text );

#   endif	/*  UTIL_PS_H  */
