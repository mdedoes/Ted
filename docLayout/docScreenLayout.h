/************************************************************************/
/*									*/
/*  Header file for geometry/formatting/drawing related functionality	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_LAYOUT_H
#   define	DOC_SCREEN_LAYOUT_H

struct LayoutContext;
struct LayoutJob;
struct DocumentTree;
struct BufferItem;
struct DocumentRectangle;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docOpenScreenFont(	
			const struct LayoutContext *		lc,
			int					textAttrNr );

extern int docStartScreenLayoutForTree(
				struct LayoutJob *		lj,
				struct DocumentTree *		tree,
				int				page,
				int				column );

extern int docScreenLayoutNode(	int *				pReachedBottom,
				struct BufferItem *		node,
				const struct LayoutContext *	lc,
				struct DocumentRectangle *	drChanged );

extern int docScreenLayoutDocumentBody(
				int *				pReachedBottom,
				struct BufferDocument *		bd,
				const struct LayoutContext *	lc );

extern void docSetScreenLayoutFunctions(	struct LayoutJob *	lj );

#   endif	/*  DOC_SCREEN_LAYOUT_H	*/
