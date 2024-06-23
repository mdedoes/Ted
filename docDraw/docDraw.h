#   ifndef	DOC_DRAW_H
#   define	DOC_DRAW_H

#   include	<textAttribute.h>
#   include	<utilColor.h>
#   include	<docLayoutDocumentTree.h>

struct DrawingContext;
struct InsertedObject;
struct DocumentField;
struct TextAttribute;
struct ParagraphFrame;
struct BlockOrnaments;
struct ParagraphDrawingStrip;
struct BlockOrigin;
struct DrawTextLine;
struct LayoutPosition;
struct BlockFrame;
struct DocumentRectangle;
struct AffineTransform2D;
struct DocumentGeometry;
struct TextRun;
struct TextLine;
struct LayoutContext;
struct DocumentSelection;

/************************************************************************/
/*									*/
/*  The different resources needed for drawing.				*/
/*									*/
/************************************************************************/

typedef int (*DRAW_ORNAMENTS)(
			const struct BlockOrnaments *		ornaments,
			int					page,
			const struct DocumentRectangle *	drOutside,
			const struct DocumentRectangle *	drInside,
			void *					through,
			struct DrawingContext *			dc );

typedef int (*FINISH_PAGE)(	void *				through,
				struct DrawingContext *		dc,
				struct BufferItem *		bodySectNode,
				int				page,
				int				asLast );

typedef int (*START_PAGE)(	void *				through,
				const struct DocumentGeometry *	dg,
				struct DrawingContext *		dc,
				const char *			why,
				int				page );

typedef int (*SET_COLOR_RGB)(	struct DrawingContext *		dc,
				void *				through,
				const RGB8Color *		rgb8 );

typedef int (*SET_FONT)(	struct DrawingContext *		dc,
				void *				through,
				int				textAttrNr,
				const struct TextAttribute *	ta );

typedef int (*DRAW_SHAPE)(	const struct DocumentRectangle *drOutside,
				const struct AffineTransform2D *atOutside,
				int				page,
				struct DrawingShape *		ds,
				struct DrawingContext *		dc,
				void *				through );

typedef int (*DRAW_INLINE_OBJECT)(
				const struct DrawTextLine *	dtl,
				int				part,
				struct InsertedObject *		io,
				const struct DocumentRectangle * drTwips,
				const struct LayoutPosition *	baseLine );

typedef int (*DRAW_TAB)(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

typedef int (*DRAW_FTNSEP)(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

typedef int (*DRAW_UNDERLINE)(	const struct DrawTextLine *	dtl,
				int				part,
				int				upto,
				int				direction,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

typedef int (*DRAW_STRIKETHROUGH)(
				const struct DrawTextLine *	dtl,
				int				part,
				int				upto,
				int				direction,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

typedef int (*START_FIELD)(	const struct DrawTextLine *	dtl,
				int				part,
				int				x0Twips,
				const struct DocumentField *	df );

typedef int (*FINISH_FIELD)(	const struct DrawTextLine *	dtl,
				int				part,
				int				x1Twips,
				const struct DocumentField *	df );

typedef int (*DRAW_TEXT_RUN)(	const struct TextRun *		tr,
				int				x0Twips,
				int				x1Twips,
				const struct DrawTextLine *	dtl,
				const struct LayoutPosition *	baseLine,
				const char *			outputString );

			/**
			 *  Start to draw a text line. The real drawing 
			 *  is done by the call-backs.
			 *
			 *  Return  0: On success
			 *  Return  1: If the line must not be drawn
			 *  Return -1: On error
			 */
typedef int (*START_TEXT_LINE)(
			struct DrawTextLine *			dtl,
			int					x0Twips );

			/**
			 *  Finish to draw a text line. The real drawing 
			 *  has been done by the call-backs.
			 */
typedef int (*FINISH_TEXT_LINE)(
			const struct DrawTextLine *		dtl,
			int					x1Twips );

			/**
			 *  Keep track of the document hierarchy while 
			 *  drawing. This is to make it possble to tag the 
			 *  structural elements in the output. This is used 
			 *  to support the ISO 14289-1 (PDF/UA) requirements.
			 *  (We use PdfMarks for that). These two are invoked
			 *  around the actual drawing of the node as a whole.
			 *  Nodes that span multiple pages are started and finished
			 *  for each page that they contribute to.
			 *
			 *  @param through The specific drawing/printing environment
			 *  @param dc The drawing context
			 *  @param repeated (boolean) true if and only if this is a repeated
			 *	instance of the node. In practice: A repeated table header.
			 *  @param pageBreak true if and only if the node is (re)started 
			 *	because of a page break. This happens if the node spans multiple
			 *	pages and inside tables. There the node might not be split, but 
			 *	the table that holds the row is split.
			 *  @param The node to start/finish.
			 */
typedef int (*START_NODE)(	void *				through,
				struct DrawingContext *		dc,
				int				repeated,
				int				pageBreak,
				const struct BufferItem *	node );

typedef int (*FINISH_NODE)(	void *				through,
				struct DrawingContext *		dc,
				int				repeated,
				int				pageBreak,
				const struct BufferItem *	node );

			/**
			 *  Keep track of the document hierarchy while 
			 *  drawing. As nodes may be split by page breaks,
			 *  at some places finer control is needed. These 
			 *  two are invoked around the actual drawing of 
			 *  the lines on a single page of of the document.
			 */
typedef int (*START_LINES)(	void *				through,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				firstLine,
				const struct DocumentSelection * ds );

typedef int (*FINISH_LINES)(	void *				through,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				lastLine,
				const struct DocumentSelection * ds );

			/**
			 *  Keep track of the document hierarchy while 
			 *  drawing. These two are invoked around the 
			 *  drawing of separate document trees like the 
			 *  document body, the page headers/footers,
			 *  foot/end notes etc.
			 */
typedef int (*START_TREE)(	void *				through,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree );

typedef int (*FINISH_TREE)(	void *				through,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree );


typedef struct DrawingContext
    {
				/**
				 *  The document that we are currently drawing
				 */
    struct BufferDocument *	dcDocument;

				/**
				 *  The document tree that we are currently drawing
				 */
    const struct DocumentTree *	dcTree;

				/**
				 *  Draw text particules separately: Normally,
				 *  spans in a line with the same text attributes 
				 *  are drawn as one string. This has the advantage, 
				 *  that the actual implementation takes care of 
				 *  most of the geometry calculations. However, in 
				 *  certain cases the result is better if we draw the
				 *  _words_ separately.
				 */
    int				dcDrawParticulesSeparately;

				/**
				 *  Is the current text attribute set to a value that 
				 *  we can rely upon? (Font, Size)
				 */
    int				dcCurrentTextAttributeSet;
				/**
				 *  The current text attribute. If we want to draw in
				 *  this font, with this size, we do not have to set it
				 *  again.
				 */
    TextAttribute		dcCurrentTextAttribute;

				/**
				 *  Is the current color set to a value that 
				 *  we can rely upon?
				 */
    int				dcCurrentColorSet;
				/**
				 *  The current color. If we want to draw in
				 *  this color, we do not have to set it again.
				 */
    RGB8Color			dcCurrentColor;

    const struct LayoutContext * dcLayoutContext;

				/**
				 *  The section in the document to which 
				 *  the node that we are currently drawing 
				 *  belongs.
				 */
    const struct BufferItem *	dcBodySectNode;

				/**
				 *  The rectangle that is to be redrawn. 
				 *  If dcClipRect == NULL, draw the whole 
				 *  document.
				 *
				 *  The rectangle is in pixels and in 
				 *  document coordinates. (Shift it to 
				 *  compensate for scrolling before you 
				 *  pass it to any drawing routine.)
				 */
    const struct DocumentRectangle * dcClipRect;

				/**
				 *  The selection that is to be drawn 
				 *  as 'selected'. This field is only 
				 *  used in special invocations to draw 
				 *  the selection.
				 */
    const struct DocumentSelection * dcSelection;

				/**
				 *  Position flags of the head of the 
				 *  selection. Only used in conjunction with
				 *  dcSelection.
				 */
    int				dcSelectionHeadPositionFlags;

				/**
				 *  Position flags of the tail of the 
				 *  selection. Only used in conjunction with
				 *  dcSelection.
				 */
    int				dcSelectionTailPositionFlags;

				/**
				 *  Thrue.. If and only if the selection is
				 *  a table rectangle. Only used in conjunction 
				 *  with dcSelection.
				 */
    int				dcSelectionIsTableRectangle;


				/**
				 *  The first page to be drawn
				 */
    int				dcFirstPage;

				/**
				 *  The last page to be drawn
				 */
    int				dcLastPage;

				/**
				 *  Draw content of other document trees 
				 *  such as headers/footers/footnotes/endnotes
				 *  on the page if and only if
				 *  dcDrawOtherTrees != 0.
				 */
    int				dcDrawOtherTrees;

				/**
				 *  Postpone drawing headers and footers until 
				 *  after the body content of the page has been
				 *  drawn.
				 */
    unsigned char		dcPostponeHeadersFooters;

				/**
				 *  As an optimization, we determine whether 
				 *  the document has headers and footers 
				 *  before we start drawing a page range.
				 */
    unsigned char		dcDocHasPageHeaders;

				/**
				 *  As an optimization, we determine whether 
				 *  the document has headers and footers 
				 *  before we start drawing a page range.
				 */
    unsigned char		dcDocHasPageFooters;

				/**
				 *  Draw the table grid on screen?
				 */
    unsigned char		dcDrawTableGrid;

    SET_COLOR_RGB		dcSetColorRgb;
    SET_FONT			dcSetFont;
    DRAW_SHAPE			dcDrawShape;
    DRAW_INLINE_OBJECT		dcDrawInlineObject;
    START_FIELD			dcStartField;
    FINISH_FIELD		dcFinishField;
    DRAW_TAB			dcDrawTab;
    DRAW_FTNSEP			dcDrawFtnsep;
    DRAW_UNDERLINE		dcDrawUnderline;
    DRAW_STRIKETHROUGH		dcDrawStrikethrough;
    DRAW_TEXT_RUN		dcDrawTextRun;

    START_TEXT_LINE		dcStartTextLine;
    FINISH_TEXT_LINE		dcFinishTextLine;

    DRAW_ORNAMENTS		dcDrawOrnaments;	/*  background	*/

    FINISH_PAGE			dcFinishPage;
    START_PAGE			dcStartPage;
    START_TREE_LAYOUT		dcStartTreeLayout;

    START_NODE			dcStartNode;
    FINISH_NODE			dcFinishNode;

    START_LINES			dcStartLines;
    FINISH_LINES		dcFinishLines;

    START_TREE			dcStartTree;
    FINISH_TREE			dcFinishTree;
    } DrawingContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDrawingContext(	DrawingContext *	dc );

extern void docResetDrawingContextState( DrawingContext *	dc );

extern int docDrawNode(		struct LayoutPosition *		lpBelow,
				struct BufferItem *		node,
				void *				through,
				DrawingContext *		dc );

extern int docDrawGroupNode(	struct LayoutPosition *		lpBelow,
				const struct BufferItem *	node,
				void *				through,
				DrawingContext *		dc,
				const struct BlockOrigin *	bo );

extern int docDrawPageRange(	DrawingContext *		dc,
				void *				through );

extern int docDrawPageHeader(	struct BufferItem *		sectNode,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawPageFooter(	struct BufferItem *		sectNode,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawShapesForPage(	void *			through,
					DrawingContext *	dc,
					int			belowText,
					int			page );

extern int docDrawFootnotesForColumn(
				int				page,
				int				column,
				void *				through,
				DrawingContext *		dc );

extern int docDrawEndnotesForSection(	struct LayoutPosition *	lpBelow,
					int			sect,
					void *			through,
					DrawingContext *	dc );

extern int docDrawEndnotesForDocument(	struct LayoutPosition *	lpBelow,
					void *			through,
					DrawingContext *	dc );

extern int docDrawToColumnOfNode( struct BufferItem *		prevNode,
				struct BufferItem *		thisNode,
				void *				through,
				struct LayoutPosition *		lpHere,
				DrawingContext *		dc,
				const struct BlockOrigin *	bo );

extern void docDrawSetColorRgb(		DrawingContext *	dc,
					void *			through,
					const RGB8Color *	rgb8 );

extern void docDrawSetColorNumber(	DrawingContext *	dc,
					void *			through,
					int			colorNumber );

extern void docDrawSetFont(	DrawingContext *		dc,
				void *				through,
				int				textAttrNr,
				const struct TextAttribute *	ta );

extern int docDrawShapeGetFill(	int *				pFill,
				RGB8Color *			rgb8Fill,
				const struct DrawingShape *	ds,
				DrawingContext *		dc,
				void *				through );

extern int docDrawShapeGetLine(	int *				pLine,
				RGB8Color *			rgb8Stroke,
				const struct DrawingShape *	ds,
				DrawingContext *		dc,
				void *				through );

extern int docDrawShape(	DrawingContext *		dc,
				void *				through,
				const struct BufferItem *	bodySectNode,
				const struct DocumentRectangle * drTwips,
				const struct InsertedObject *	io );

extern int docDrawRowNode(	struct BufferItem *		rowNode,
				void *				through,
				DrawingContext *		dc,
				const struct BlockOrigin *	bo );

extern int docDrawTableHeader(	int *				pHigh,
				const struct BufferItem *	rowNode,
				const struct BlockFrame *	bfRef,
				void *				through,
				DrawingContext *		dc,
				const struct LayoutPosition *	lpHeader );

extern int docDrawParagraphStrip( void *			through,
				struct ParagraphDrawingStrip *	pds,
				struct BufferItem *		paraNode,
				int				countAfter,
				const struct LayoutPosition *	lpShadeTop,
				int				lineFrom,
				const struct ParagraphFrame *	pf,
				DrawingContext *		dc,
				const struct LayoutPosition *	lpThisFrame,
				const struct BlockOrigin *	bo );

extern int docDrawTextLine(	struct BufferItem *		paraNode,
				struct TextLine *		tl,
				const struct ParagraphFrame *	pf,
				const struct DocumentRectangle * drLineFramePixels,
				void *				through,
				DrawingContext *		dc,
				const struct BlockOrigin *	bo );

extern int docDrawToNextColumn(	struct BufferItem *		thisBodyNode,
				struct BufferItem *		nextBodyNode,
				void *				through,
				struct LayoutPosition *		lpHere,
				struct BlockFrame *		bf,
				DrawingContext *		dc,
				const char *			why );

extern int docDrawParaNode(	struct LayoutPosition *		lpBelow,
				struct BufferItem *		paraNode,
				void *				through,
				DrawingContext *		dc,
				const struct BlockOrigin *	bo );

extern int docDrawShapeText(	const struct DocumentRectangle * drHere,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				struct DrawingShape *		ds,
				DrawingContext *		dc,
				void *				through );

#   endif
