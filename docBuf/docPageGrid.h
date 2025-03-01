/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PAGE_GRID_H
#   define	DOC_PAGE_GRID_H

struct LayoutPosition;
struct BufferDocument;
struct BlockFrame;
struct ParagraphFrame;
struct FrameProperties;
struct BufferItem;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docBlockFrameTwips(	struct BlockFrame *		bf,
				const struct BufferItem *	node,
				struct BufferDocument *		bd,
				int				page,
				int				column );

void docSectionBlockFrameTwips(	struct BlockFrame *		bf,
				const struct BufferItem *	node,
				const struct BufferItem *	bodySectNode,
				struct BufferDocument *		bd,
				int				page,
				int				column );

extern void docCellFrameTwips(	struct ParagraphFrame *		pf,
				const struct BlockFrame *	bf,
				const struct BufferItem *	cellNode );

extern void docCellRectangleTwips(
				struct DocumentRectangle *	drCell,
				const struct BlockFrame *	bf,
				const struct BufferItem *	cellNode );

extern void docLayoutSectColumnTop(
				struct LayoutPosition *		lpTop,
				struct BlockFrame *		bf,
				struct BufferItem *		bodySectNode,
				struct BufferDocument *		bd );

extern void docParaBlockFrameTwips(
				struct BlockFrame *		bf,
				struct BufferItem *		paraNode,
				const struct BufferItem *	bodySectNode,
				struct BufferDocument *		bd,
				const struct LayoutPosition *	lpHere );

extern void docParagraphFrameTwips(
				struct ParagraphFrame *		pf,
				const struct BlockFrame *	bf,
				const struct BufferItem * 	paraNode );

extern void docLayoutSetTextFrame(
				struct BlockFrame *		bfTextFrame,
				const struct LayoutPosition *	lpRef,
				const struct BlockFrame *	bfRef,
				const struct FrameProperties *	fp,
				int				frameHighVal );

extern void docLayoutFrameX(	struct BlockFrame *		bfTextFrame,
				int				xRefProp,
				int				xPosProp,
				int				xPosVal,
				int				layoutInCell,
				const struct ParagraphFrame *	pfRef,
				const struct BlockFrame *	bfRef,
				int				xChar,
				int				frameWide );

extern void docLayoutFrameY(	struct BlockFrame *		bfTextFrame,
				int				yRefProp,
				int				yPosProp,
				int				yPosVal,
				const struct LayoutPosition *	lpLineTop,
				const struct LayoutPosition *	lpParaTop,
				const struct BlockFrame *	bfRef,
				int				frameHighProp,
				int				frameHighVal );

#   endif	/*  DOC_PAGE_GRID_H  */
