/************************************************************************/
/*									*/
/*  Text Editor Buffer structure.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BUF_H
#   define	DOC_BUF_H

#   include	<docStyleSheet.h>
#   include	<docDocumentFieldList.h>
#   include	"docDrawingShapeList.h"
#   include	<docInsertedObjectList.h>
#   include	"docDocumentTree.h"

struct BufferDocument;
struct BufferItem;
struct DocumentNote;
struct DocumentPropertyLists;
struct LayoutPosition;
struct TextAttribute;
struct SectionProperties;
struct CellProperties;
struct TableRectangle;
struct DocumentPosition;
struct DocumentSelection;
struct SelectionScope;
struct PostScriptFontList;
struct DocumentProperties;
struct ListOverride;
struct DocumentGeometry;
struct PropertyMask;
struct DocumentList;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  The document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct NotesList
    {
    struct DocumentNote **	nlNotes;
    int				nlNoteCount;
    } NotesList;

typedef struct BufferDocument
    {
				/**
				 *  Numbered properties of the document.
				 *  In some cases, the lists are shared 
				 *  between documents.
				 */
    struct DocumentPropertyLists *	bdPropertyLists;

				/**
				 *  The body of the document. This is the 
				 *  main document tree. Page headers and 
				 *  page footers are attached to the sections.
				 *  
				 *  Notes are kept in bdNotesList. The 
				 *  separators between the notes and ordinary
				 *  text are declared below.
				 */
    DocumentTree		bdBody;
    struct DocumentProperties *	bdProperties;

    InsertedObjectList		bdObjectList;
    DocumentStyleSheet		bdStyleSheet;
    DocumentFieldList		bdFieldList;
    DrawingShapeList		bdShapeList;

				/**
				 *  The notes in this document. References 
				 *  from the contents of the various document 
				 *  trees to this list always pass through 
				 *  the field list of that tree.
				 *
				 *  In practice, only the document body holds
				 *  notes. The implementation of notes 
				 *  implcitly relies on this assumption.
				 */
    NotesList			bdNotesList;

				/**
				 *  The Identifiers of the sequence 
				 *  fields in this document. It is map from 
				 *  the identifier to an int. The value of 
				 *  the int is only relevant inside the loop 
				 *  that renumbers the SEQ fields of a tree 
				 *  in the document.
				 */
    void *			bdSeqFieldIdentifiers;

				/**
				 *  The text that appears between the 
				 *  body text and the first footnote on the 
				 *  page. The layout of this tree is 
				 *  determined by the section properties of 
				 *  the first footnote of the page.
				 */
    DocumentTree		bdFtnsep;

				/**
				 *  The text that appears between the 
				 *  body text and continued footnotes from
				 *  the previous page.
				 */
    DocumentTree		bdFtnsepc;

				/**
				 *  The text that appears after the footnote
				 *  contents on the page when the notes are
				 *  continued on the next page.
				 */
    DocumentTree		bdFtncn;

				/**
				 *  The text that appears between the 
				 *  body text and the first endnote on the 
				 *  page. The layout of this tree is 
				 *  determined by the section properties of 
				 *  the first endnote of the page.
				 */
    DocumentTree		bdAftnsep;

				/**
				 *  The text that appears between the 
				 *  body text and continued endnotes from
				 *  the previous page.
				 */
    DocumentTree		bdAftnsepc;

				/**
				 *  The text that appears after the endnote
				 *  contents on the page when the notes are
				 *  continued on the next page.
				 */
    DocumentTree		bdAftncn;

				/**
				 *  Locale id for the document as a whole;
				 *  It is derived from the document properties 
				 *  and/or the operating system locale.
				 */
    int				bdLocaleId;
    } BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern BufferDocument * docNewFile(
				struct TextAttribute *		taDefault,
				const char *			defaultFontName,
				int				defaultFontSize,
				const struct PostScriptFontList *	psfl,
				const struct DocumentGeometry * 	dg );

extern void docFreeDocument( BufferDocument *	bd );
extern BufferDocument * docNewDocument( const BufferDocument *	bdRef );

extern void docDeleteDocumentTree(	BufferDocument *	bd,
					DocumentTree *		dt );

extern void docEraseDocumentTree(	BufferDocument *	bd,
					DocumentTree *		dt );

extern void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const struct BufferItem *	cellNode );

extern int docGetCellBottom(	struct LayoutPosition *		lpBottom,
				const struct BufferItem *	cellNode );

extern int docWhatPagesForHeaderFooter(
				const struct DocumentProperties *	dp,
				const struct SectionProperties *	sp,
				int				treeType );

extern struct BufferItem * docGetSelectionRoot(
			struct DocumentTree **			pTree,
			struct BufferItem **			pBodySectNode,
			struct BufferDocument *			bd,
			const struct DocumentSelection *	ds );

extern int docGetHeaderFooter(	DocumentTree **			pTree,
				struct BufferItem **		pBodySectNode,
				const struct DocumentPosition *	dp,
				BufferDocument *		bd,
				int				treeType );

extern int docInquireHeadersFooters(
				int *				pDocHasHeaders,
				int *				pDocHasFooters,
				const struct BufferDocument *	bd );

extern int docMakeDocumentTree(	BufferDocument *			bd,
				DocumentTree *				dt,
				const struct SelectionScope *		ss,
				const struct SectionProperties *	sp );

extern struct BufferItem * docStartDocumentTree(
				BufferDocument *		bd,
				DocumentTree *			dt,
				int				treeType,
				const struct BufferItem *	bodyNode,
				int				ownerNumber,
				int				txtAttrNr );

extern struct BufferItem * docMakeTreeRoot(
				BufferDocument *		bd,
				DocumentTree *			dt,
				const struct BufferItem *	bodyNode,
				int				ownerNumber,
				int				treeType );

extern int docMergeDocumentLists(
				int **				pFontMap,
				int **				pListStyleMap,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const int *			colorMap,
				const int *			rulerMap );

extern int docMergeColorTables(	int **				pColorMap,
				BufferDocument *		bdTo,
				const BufferDocument *		bdFrom );

extern int docFindBookmarkInDocument(
				struct DocumentSelection *	dsInside,
				int *				pHeadPart,
				int *				pTailPart,
				const BufferDocument *		bd,
				const struct MemoryBuffer *	markName );

extern DocumentTree * docSectionHeaderFooter(
				const struct BufferItem *	bodySectNode,
				unsigned char *			pApplies,
				const struct DocumentProperties *	dp,
				int				treeType );

extern int docSectionHasHeaderFooter(
				const struct BufferItem *	bodySectNode,
				unsigned char  *		pApplies,
				const struct DocumentProperties *	dp,
				int				treeType );

struct DocumentTree * docDocumentNoteSeparator(
				BufferDocument *		bd,
				int				which );

extern int docGetTreeOfNode(	DocumentTree **			pTree,
				struct BufferItem **		pBodySectNode,
				BufferDocument *		bd,
				struct BufferItem *		node );

extern int docGetRootOfSelectionScope(	struct DocumentTree **	pTree,
					struct BufferItem **	pBodySectNode,
					struct BufferDocument *	bd,
					const struct SelectionScope *	ss );

extern int docSectionHeaderFooterFirstPage(
				int *				pUsedInDocument,
				const struct BufferItem *	bodySectNode,
				int				treeType,
				const struct DocumentProperties *	dp );

extern void docSetTreeTypeOfNode(	struct BufferItem *		node,
					int			treeType );

extern void docDelimitTables(		struct BufferItem *	parentNode,
					int			recursively );

extern int docGetListOfParagraph(	struct ListOverride **	pLo,
					struct DocumentList **	pDl,
					int			ls,
					const BufferDocument *	bd );

extern int docRemoveParagraphFromList(
				struct BufferItem *		paraNode,
				DocumentTree *			dt,
				const BufferDocument *		bd );

extern int docFindListOfSelection(
				int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const struct DocumentSelection *	ds,
				DocumentTree *			tree,
				BufferDocument *		bd );

extern int docApplyListRuler(	const struct DocumentList *	dl,
				const struct ListOverride *	lo,
				BufferDocument *		bd );

extern int docNextSimilarTree(	struct DocumentPosition *	dp,
				struct DocumentTree **		pTree,
				BufferDocument *		bd );

extern int docPrevSimilarTree(	struct DocumentPosition *	dp,
				struct DocumentTree **		pTree,
				BufferDocument *		bd );

extern int docAdaptParagraphToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const BufferDocument *		bd );

extern int docGetMatchingCell(	const struct BufferItem *	rowNode,
				const struct BufferItem *	cellNode );

extern int docFindSetPattern(		void **			pProg,
					const char *		pattern,
					int			useRegex,
					int			asWord,
					int			caseSensitive );

extern int docGetCharsUsed(		BufferDocument *	bd );

extern const char * docGetEncodingName(
				struct BufferDocument *		bd,
				struct TextAttribute *		ta,
				int				charset );

extern int docChangeParticuleAttributes( int *			pChanged,
				struct PropertyMask *		pTaAllMask,
				BufferDocument *		bd,
				struct BufferItem *		paraNode,
				int				part,
				int				partUpto,
				const struct TextAttribute *	taSet,
				const struct PropertyMask *	taSetMask );

extern int docNodeAtExtremity(	int *				pAtExtremity,
				const struct BufferItem *	parentNode,
				const struct BufferItem *	paraNode,
				int				after );

extern int docHeaderFooterPage(	const BufferDocument *		bd,
				const struct BufferItem *	bodySectNode,
				int				currentPage,
				int				treeType );

extern void docInvalidateSectHeaderFooterLayout(
				struct BufferItem *		sectNode );

extern void docInvalidateParagraphLayout( struct BufferItem *	paraNode );
extern void docInvalidateNodeLayout(	struct BufferItem *	node );

extern void docOverridePaperSize(	struct BufferDocument *		bd,
					const struct DocumentGeometry *	dgFrom );

extern void docGetCellRectangleProperties(
				struct CellProperties *		cpTo,
				const struct BufferItem *	rowNode,
				const struct TableRectangle *		tr,
				const struct DocumentAttributeMap *	dam );

extern int docGetSelectionAttributes(
				struct TextAttribute *		ta,
				struct PropertyMask *		pUpdMask,
				struct BufferDocument *		bd,
				const struct DocumentSelection * ds );

#   endif
