/************************************************************************/
/*									*/
/*  Forward declaration for calculating the layout of external trees.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_DOCUMENT_TREE_H
#   define	DOC_LAYOUT_DOCUMENT_TREE_H

struct LayoutJob;
struct LayoutContext;
struct DocumentTree;
struct BufferItem;
struct DrawingShape;
struct DocumentRectangle;
struct InsertedObject;

typedef int (*START_TREE_LAYOUT)(
				struct LayoutJob *		lj,
				struct DocumentTree *		tree,
				int				page,
				int				column );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docLayoutDocumentTree(
			struct DocumentTree *		tree,
			struct DocumentRectangle *	drChanged,
			int				page,
			int				column,
			int				y0Twips,
			const struct BufferItem *	bodySectNode,
			const struct LayoutContext *	lc,
			START_TREE_LAYOUT		startTreeLayout,
			int				adjustDocument );

extern int docShapeCheckTextLayout(
			struct DrawingShape *		ds,
			const struct DocumentRectangle * drTwips,
			struct DocumentRectangle *	drChanged,
			const struct BufferItem *	bodySectNode,
			const struct LayoutContext *	lc,
			START_TREE_LAYOUT		startTreeLayout );

extern int docShapePrelayout(
			struct InsertedObject *		io,
			const struct BufferItem *	bodySectNode,
			const struct LayoutContext *	lc );

extern int docCheckPageOfSelectedTree(
			int *				pChanged,
			struct BufferItem **		pBodySectNode,
			struct DocumentRectangle *	drChanged,
			struct DocumentTree *		selRootTree,
			const struct LayoutContext *	lc,
			START_TREE_LAYOUT		startTreeLayout );

#   endif	/*  DOC_LAYOUT_DOCUMENT_TREE_H  */
