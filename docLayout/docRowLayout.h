/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_LAYOUT_H
#   define	DOC_ROW_LAYOUT_H

struct BorderProperties;
struct BlockOrnaments;
struct BufferDocument;
struct DocumentRectangle;
struct BufferItem;

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

#   endif	/*  DOC_ROW_LAYOUT_H  */
