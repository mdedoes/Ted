/************************************************************************/
/*									*/
/*  Document Layout of table rows and table cells.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_LAYOUT_H
#   define	DOC_ROW_LAYOUT_H

struct BorderProperties;
struct BlockOrnaments;
struct BufferDocument;
struct DocumentRectangle;
struct BufferItem;
struct ParagraphLayoutPosition;
struct LayoutJob;
struct BlockFrame;
struct LayoutPosition;
struct ParagraphLayoutJob;
struct CellProperties;
struct RowProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docGetCellOrnaments(
			struct BlockOrnaments *		cellOrnaments,
			struct DocumentRectangle *	drOutside,
			struct DocumentRectangle *	drInside,
			const struct DocumentRectangle * drCell,
			const struct BufferDocument *	bd,
			const struct BufferItem *	rowNode,
			int				col,
			int				atRowTop,
			int				atRowBottom,
			int				drawTableGrid );

extern void docLayoutCalculateRowTopInset(
				int *				pInset,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				atRowTop );

extern const struct BorderProperties * docGetCellBottomBorder(
				int *				pBpBottomNr,
				int *				pUseBelow,
				const struct BufferDocument *	bd,
				const struct BufferItem *	rowNode,
				int				col,
				int				atRowBottom );

extern int docLayoutRowValignCells(
				struct BufferItem *			rowNode,
				struct BlockFrame *			bf,
				const struct LayoutPosition *		lpBelow,
				const struct ParagraphLayoutPosition *	rowPlp,
				const struct LayoutJob *		lj );

extern void docLayoutFinishCell( struct LayoutPosition *	lpHere,
				struct LayoutPosition *		lpBelowAllCells,
				struct BufferItem *		cellNode,
				const struct CellProperties *	cp,
				struct ParagraphLayoutJob *	plj );

extern int docLayoutCellPageStrip(
			struct LayoutPosition *		lpHere,
			struct LayoutPosition *		lpBottom,
			struct LayoutPosition *		lpBelowAllCells,
			int *				pContinueInNextFrame,
			struct BufferItem *		cellNode,
			const struct CellProperties *	cp,
			struct BlockFrame *		bf,
			struct ParagraphLayoutJob *	plj );

extern void docLayoutStartCell(	struct BufferItem *		cellNode,
				const struct LayoutJob *	lj,
				const struct LayoutPosition *	lpHere,
				struct ParagraphLayoutJob *	cellPlj );

extern void docCellToNextColumn( struct LayoutPosition *	lpBefore,
				const struct BufferItem *	cellNode,
				struct ParagraphLayoutJob *	plj );

extern int docCellPropertiesArePlain(
			const struct BufferDocument *	bd,
			const struct CellProperties *	cp );

extern int docRowPropertiesArePlain(
			const struct BufferDocument *	bd,
			const struct RowProperties *	rp );

#   endif	/*  DOC_ROW_LAYOUT_H  */
