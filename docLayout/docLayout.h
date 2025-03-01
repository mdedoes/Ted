/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_H
#   define	DOC_LAYOUT_H

struct BufferItem;
struct BufferDocument;
struct DocumentTree;
struct TextAttribute;
struct ParagraphLayoutJob;
struct LayoutJob;
struct DocumentPosition;
struct DocumentSelection;
struct PositionGeometry;
struct AfmFontInfo;
struct DocumentRectangle;
struct TextLine;
struct DocumentNote;
struct DrawingShape;
struct FrameProperties;
struct EditRange;
struct ParagraphFrame;
struct LayoutPosition;
struct ParagraphLayoutPosition;
struct BlockOrigin;
struct BlockFrame;
struct NotesReservation;
struct LineLayoutJob;
struct LayoutContext;

/************************************************************************/
/*									*/
/*  Pass through information for a layout job.				*/
/*									*/
/************************************************************************/

typedef int (*SCREEN_LAYOUT_LINE)( 
				struct TextLine *		tl,
				struct LineLayoutJob *		llj );

typedef int (*START_SCREEN_PARAGRAPH)(
				struct BufferItem *		node,
				const struct ParagraphFrame *	pf,
				const struct LayoutContext *	lc );

/************************************************************************/
/*									*/
/*  A Layout Job.							*/
/*									*/
/************************************************************************/

typedef struct LayoutJob
    {
				/**
				 * Receives the rectangle that is to be 
				 * redrawn on the screen. Coordinates are 
				 * in pixels, but relative to the document as 
				 * a whole. ( Not dependent on the position of 
				 * the scrollbars. ) In non interactive layout 
				 * jobs this is a NULL pointer.
				 */
    struct DocumentRectangle *	ljChangedRectanglePixels;

				/**
				 * The node that initiated the layout process.
				 */
    struct BufferItem *		ljChangedNode;

				/**
				 * The context of this layout action. It holds
				 * the document and methods to lookup fonts
				 * etc.
				 */
    const struct LayoutContext * ljContext;

				/**
				 * Set to true when the layout job reaches 
				 * the bottom of the document. I.E. there is
				 * no content after the job that has not been
				 * touched by the action.
				 *
				 * This is to optimize screen drawing.
				 */
    int				ljReachedDocumentBottom;

				/**
				 * The page on which we are balancing the 
				 * columns of a section. This is used during 
				 * the binary search for the best height of 
				 * the columns.
				 */
    int				ljBalancePage;
				/**
				 * The y1 value (column height) that we use
				 * in the search for the best column height
				 * while balancing the columns of a section.
				 */
    int				ljBalanceY1;

				/**
				 * The section in the body of the document that
				 * determines the page setup for this layout 
				 * job. It is the current section for nodes in 
				 * the body of the document. For headers and 
				 * footers it is the section that owns them.
				 */
    const struct BufferItem *	ljBodySectNode;

				/**
				 * Indirection for screen calculations when 
				 * laying out the document.
				 */
    START_SCREEN_PARAGRAPH	ljStartScreenParagraph;
    SCREEN_LAYOUT_LINE		ljScreenLayoutLine;
    } LayoutJob;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitLayoutJob(		LayoutJob *	lj );
extern void docCleanLayoutJob(		LayoutJob *	lj );

extern int docLayoutParagraphLineExtents(
				int *				pFontSize,
				const struct LayoutContext *	lc,
				struct BufferItem *		node );

extern int docLayoutNodeAndParents(	struct BufferItem *	node,
					LayoutJob *		lj );

extern int docAdjustParaLayout(		struct BufferItem *	paraNode,
					int			afterReplace,
					int			stroffFrom,
					int			stroffShift,
					int			stroffUpto,
					LayoutJob *		lj );

extern void docLayoutColumnTop(	struct LayoutPosition *		lpTop,
				struct BlockFrame *		bf,
				struct BufferItem *		bodySectNode,
				const LayoutJob *		lj );

extern int docLayoutStripChildren(
				int *				pStopCode,
				struct ParagraphLayoutJob *	plj,
				struct BlockFrame *		bf,
				struct BufferItem *		cellNode );

extern int docLayoutStackedStrip( struct BufferItem *		cellNode,
				struct BlockFrame *		bf,
				struct ParagraphLayoutJob *	plj );

extern void docLayoutToNextColumn(	struct LayoutPosition *		lpTop,
					struct BlockFrame *		bf,
					struct BufferItem *		node,
					const LayoutJob *		lj );

extern void docLayoutToNextPage(	struct LayoutPosition *		lpTop,
					struct BlockFrame *		bf,
					struct BufferItem *		node,
					const LayoutJob *		lj );

extern int docLayoutToFirstColumn(	struct LayoutPosition *		lpTop,
					struct BlockFrame *		bf,
					struct BufferItem *		node,
					const LayoutJob *		lj );

extern int docLayoutToEvenPage(		struct LayoutPosition *		lpTop,
					struct BlockFrame *		bf,
					struct BufferItem *		node,
					const LayoutJob *		lj );

extern int docLayoutToOddPage(		struct LayoutPosition *		lpTop,
					struct BlockFrame *		bf,
					struct BufferItem *		node,
					const LayoutJob *		lj );

extern int docLayoutRowNode(
			int *				pStopCode,
			struct LayoutPosition *		lpBelow,
			const struct LayoutPosition *	lpTop,
			struct BufferItem *		rowNode,
			struct BlockFrame *		bf,
			int				inNewFrame,
			int				stayInThisColumn,
			const LayoutJob *		lj );

extern void docCommitStripLayout(
				int *				pAdvanced,
				int				advanceAnyway,
				struct ParagraphLayoutJob *	plj,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode );

extern void docFindStripLayoutOrigin(
				struct ParagraphLayoutJob *	plj,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode );

extern void docLayoutBlockFrame( struct BlockFrame *		bf,
				struct BufferItem *		node,
				const LayoutJob *		lj,
				int				page,
				int				column );

extern int docLayoutFootnotesForColumn(
				struct LayoutPosition *		lpBelowNotes,
				const struct LayoutPosition *	lpBelowText,
				const struct BlockFrame *	refBf,
				int				belowText,
				const LayoutJob *		refLj );

extern int docLayoutCollectParaFootnoteHeight(
			struct NotesReservation *	nr,
			int				referringPage,
			int				referringColumn,
			struct BufferDocument *		bd,
			const struct BufferItem *	bodySectNode,
			const struct BufferItem *	paraNode,
			int				partFrom,
			int				partUpto );

extern int docCollectFootnotesFromColumn(
			struct BlockFrame *			bf,
			const struct DocumentPosition *	dpHere,
			int				partHere,
			struct BufferDocument *		bd,
			int				referringPage,
			int				referringColumn );

extern int docNoteSeparatorRectangle(
				struct DocumentRectangle *	drExtern,
				struct DocumentTree **		pEiNoteSep,
				int *				pY0Twips,
				const struct DocumentNote *	dnFirstNote,
				int				treeType,
				const struct BufferItem *	bodySectNode,
				const struct LayoutContext *	lc );

extern int docGetBoxAroundTree(
				struct DocumentRectangle *	dr,
				const struct DocumentTree *	ei,
				int				justUsed,
				int				page,
				int				column,
				const struct LayoutContext *	lc );

extern int docTreePrelayout(	struct DocumentTree *		ei,
				const struct BufferItem *	bodySectNode,
				LayoutJob *			lj );

extern int docLayoutNodeImplementation(
				struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		node,
				struct BlockFrame *		bf,
				LayoutJob *			lj );

extern int docSectHeaderFooterPrelayout( struct BufferItem *	sectNode,
					LayoutJob *		lj );

extern int docLayoutEndnotesForSection(
				struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				int				sect,
				struct BlockFrame *			bf,
				LayoutJob *			lj );

extern int docLayoutEndnotesForDocument(
				struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BlockFrame *			bf,
				LayoutJob *			lj );

extern int docLayoutParaLines(	int *				pStopCode,
				int				isRedo,
				const struct ParagraphFrame *	pf,
				struct LayoutPosition *		lpHere,
				int *				pLine,
				struct BlockFrame *			bf,
				const LayoutJob *		lj,
				struct BufferItem *		paraNode,
				int				part );

extern int docLayoutSectChildren( struct LayoutPosition *	lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		sectNode,
				int				from,
				struct BlockFrame *			bf,
				LayoutJob *			lj );

extern int docLayoutSectNode(	struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		sectNode,
				struct BlockFrame *			bf,
				LayoutJob *			lj );

extern int docLayoutTableSlice( struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		parentNode,
				int				from,
				int				upto,
				struct BlockFrame *		bf,
				LayoutJob *			lj );

extern void docResetExternalTreeLayout(
				struct BufferDocument *		bd );

extern void docDrawingShapeInvalidateTextLayout(
				struct DrawingShape *		ds );

extern void docShapePageY(	struct LayoutPosition *	lpShapeTop,
				struct LayoutPosition *	lpBelowShape,
				struct BlockFrame *		bfShape,
				const struct DrawingShape *	ds,
				const struct BufferItem *	paraNode,
				const struct LayoutPosition *	lpLineTop,
				const struct BlockFrame *	bf );

extern void docShapePageRectangle(
				struct LayoutPosition *	lpShapeTop,
				struct LayoutPosition *	lpBelowShape,
				int *			pX0,
				int *			pX1,
				const struct DrawingShape *	ds,
				const struct BufferItem *	paraNode,
				const struct LayoutPosition *	lpLineTop,
				const struct ParagraphFrame *	pfRef,
				const struct BlockFrame *	bfRef,
				int			xChar );

extern int docLayoutInvalidateRange(
			struct DocumentSelection *	dsLayout,
			struct BufferDocument *		bd,
			const struct DocumentTree *	ei,
			struct EditRange *		er );

extern void docRedoParaStripLayout(
				const LayoutJob *		lj,
				struct BlockFrame *		bf,
				const struct LayoutPosition *	lpFrom,
				struct BufferItem *		cellNode,
				int				paraFrom,
				int				paraUpto );

extern int docSectNotesPrelayout(
				int				sect,
				const struct BufferItem *	bodySectNode,
				LayoutJob *			lj );

extern void docLayoutSetNodeBottom(
				int *				pChanged,
				struct BufferItem *		node,
				const struct LayoutPosition *	lp,
				const struct LayoutContext *	lc,
				struct DocumentRectangle *	drChanged );

extern void docLayoutSetBottomPosition(
			int *					pChanged,
			struct LayoutPosition *			lpBelow,
			const struct LayoutPosition *		lp,
			const struct LayoutContext *		lc,
			struct DocumentRectangle *		drChanged );

extern int docLayoutGetInitialFrame(
			struct BlockFrame *			bf,
			const LayoutJob *			lj,
			const struct LayoutPosition *		lpHere,
			struct BufferItem *			node );

extern void docLayoutStartNodeLayout(
			struct BufferItem *			node,
			const LayoutJob *			lj,
			const struct LayoutPosition *		lpHere );

extern void docLayoutFinishNodeLayout(
			int *					pChanged,
			struct BufferItem *			node,
			const LayoutJob *			lj,
			const struct LayoutPosition *		lpHere );

extern int docLayoutFinishSectNode(
			struct LayoutPosition *			lpBelow,
			const struct LayoutPosition *		lpTop,
			struct BufferItem *			sectNode,
			struct BlockFrame *			bf,
			LayoutJob *				lj );

extern const struct AfmFontInfo * docLayoutGetAfi(
			int *					pTextAttrNr,
			const struct TextAttribute **		pTa,
			unsigned char *				pLineFlags,
			const struct LayoutContext *		lc,
			const struct BufferItem *		paraNode,
			int					part );

extern int docLayoutTextExtents(
			struct DocumentRectangle *		drText,
			const struct TextAttribute *		ta,
			const struct AfmFontInfo *		afi,
			const char *				from,
			int					len );

extern int docLayoutTextExtentsInParticule(
			struct DocumentRectangle *		drText,
			const struct LayoutContext *		lc,
			const struct BufferItem *		paraNode,
			int					part,
			const char *				from,
			int					len );

extern int docLayoutStartParagraph(
			const LayoutJob *		lj,
			int *				pStopCode,
			struct BufferItem *		paraNode,
			const struct BlockFrame *		bf,
			struct ParagraphLayoutPosition *	plp );

extern void docLayoutFinishFrame(
			const struct FrameProperties *		fp,
			struct BlockFrame *			bfTextFrame,
			const struct BlockFrame *		bfFlow,
			const LayoutJob *			lj,
			const struct ParagraphLayoutPosition *	plpFlow,
			struct BufferItem *			cellNode,
			int					paraFrom,
			int					paraUpto );

extern int docLayoutParagraphInStrip(
			int *					pStopCode,
			struct ParagraphLayoutPosition *	plp,
			struct BlockFrame *			bf,
			const LayoutJob *			lj,
			int					cellTopInset,
			int					isRedo,
			struct BufferItem *			paraNode );

extern int docLayoutRowInStrip(
			int *					pStopCode,
			struct ParagraphLayoutPosition *	plp,
			struct BlockFrame *			bf,
			const LayoutJob *			lj,
			struct BufferItem *			paraNode );

extern int docAdjustParentLayout(
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		node,
				struct BlockFrame *		bf,
				LayoutJob *			lj );

extern int docLayoutCellNode(	struct LayoutPosition *		lpBelow,
				const struct LayoutPosition *	lpTop,
				struct BufferItem *		cellNode,
				int				from,
				struct BlockFrame *		bf,
				LayoutJob *			lj );

extern void docRowLayoutRowspanAdmin(
				struct BufferItem *		cellNode,
				const struct BufferItem *	rowNode,
				const struct BufferItem *	nextRowNode,
				int				nextRow,
				struct ParagraphLayoutJob *	plj );

extern void docCellStripFrame(	struct BufferItem *		cellNode,
				const struct BlockFrame *	bf,
				struct ParagraphLayoutJob *	plj );

extern void docLineTextRectangleTwips(
				struct DocumentRectangle *	drLineTwips,
				const struct LayoutPosition *	lpTop,
				const struct LayoutPosition *	lpBottom,
				const struct TextLine *		tl,
				const struct ParagraphFrame *	pf );

extern void docLineFrameRectanglePixels(
				struct DocumentRectangle *	drLineFramePix,
				const struct LayoutPosition *	lpTop,
				const struct LayoutPosition *	lpBottom,
				const struct TextLine *		tl,
				const struct BlockOrigin *	bo,
				const struct ParagraphFrame *	pf,
				const struct LayoutContext *	lc );

extern void docGetPixelRect(	struct DocumentRectangle *	drPixels,
				const struct LayoutContext *	lc,
				const struct DocumentRectangle * drTwips,
				int				page );

extern void docGetPixelRectForPos(
				struct DocumentRectangle *	drPixels,
				const struct LayoutContext *	lc,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	lpTop,
				const struct LayoutPosition *	lpBottom );

extern void docGetPageRectPixels(
				struct DocumentRectangle *	drPixels,
				const struct LayoutContext *	lc,
				int				page );

extern void docGetPixelRectangleForPages(
				struct DocumentRectangle *	drPixels,
				const struct LayoutContext *	lc,
				int				page0,
				int				page1 );

extern void docPixelRectangleForPositions(
				struct DocumentRectangle *	drPixels,
				const struct PositionGeometry *	pgB,
				const struct PositionGeometry *	pgE,
				const struct LayoutContext *	lc );

extern int docLayoutScapsScreenFont(
				const struct LayoutContext *	lc,
				const struct TextAttribute *	ta );

extern void docNodeRectangle(	struct DocumentRectangle *	drPixels,
				const struct BufferItem *	node,
				const struct LayoutContext *	lc,
				const struct BlockOrigin *	bo );

extern int docGetFirstSectionOnPage(	struct BufferDocument *	bd,
					int			page );

extern int docFindHeaderFooterForY(
				struct DocumentTree **		pTree,
				struct BufferItem **		pSelSectNode,
				int *				pColumn,
				const struct LayoutContext *	lc,
				struct BufferItem *		bodySectNode,
				struct BufferDocument *		bd,
				int				page,
				int				docY );

extern void docPageRectsPixels(	struct DocumentRectangle *	drOutside,
				struct DocumentRectangle *	drInside,
				double				xfac,
				const struct BufferItem *	bodySectNode,
				const struct BufferDocument *	bd );

extern int docAdjustLayoutToChangedTree(
				struct DocumentTree *		tree,
				struct BufferItem *		bodyNode,
				LayoutJob *			lj );

extern void docLayoutAdjustFrame( struct BlockFrame *		bf,
				const struct BufferItem *	node );

extern int docLayoutSelectedRoot( const struct LayoutContext *	lc,
				struct DocumentTree *		treeFound,
				struct BufferItem *		rootNodeFound,
				struct BufferItem *		bodySectNodeSet,
				int				page,
				int				column );

extern int docLayoutLineXOfPosition(
				int *				pDocXPixels,
				int *				pDocXTwips,
				const struct DocumentPosition *	dp,
				const struct LayoutContext *	lc,
				const struct ParagraphFrame *	pf,
				struct BufferItem *		paraNode,
				struct TextLine *		tl );

extern const struct AfmFontInfo * docDocLayoutGetFontInfo(
				const struct LayoutContext *	lc,
				const struct TextAttribute *	ta );

#   endif	/*  DOC_LAYOUT_H  */
