/************************************************************************/
/*									*/
/*  Select positions in a document, based on their geometric position.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SELECT_LAYOUT_H
#   define	DOC_SELECT_LAYOUT_H

struct DocumentPosition;
struct BufferDocument;
struct BufferItem;
struct BlockOrigin;
struct LayoutPosition;

extern int docGetTopOfColumn(	struct DocumentPosition *	dp,
				int *				pLineTop,
				int *				pPartTop,
				struct BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetFirstInColumnForNode(
				struct DocumentPosition *	dp,
				int *				pLineTop,
				int *				pPartTop,
				struct BufferItem *		node,
				int				page,
				int				column );

extern int docGetBottomOfColumn(struct DocumentPosition *	dp,
				int *				pPartBot,
				struct BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetLastInColumnForNode(
				struct DocumentPosition *	dp,
				int *				pLineBot,
				int *				pPartBot,
				struct BufferItem *		node,
				int				page,
				int				column );

extern int docPositionNearHeadFoot( struct DocumentPosition *	dpNew,
				int				treeType,
				struct BufferItem *		bodySectNode,
				struct BufferDocument *		bd,
				int				page );

extern int docParaFindLastLineInFrame(
				const struct BufferItem *	paraNode,
				int				lineFrom,
				const struct LayoutPosition *	lpThisFrame,
				const struct BlockOrigin *	bo );

extern int docParaFindFirstLineInFrame(
				const struct BufferItem *	paraNode,
				int				lineFrom,
				const struct LayoutPosition *	lpThisFrame,
				const struct BlockOrigin *	bo );

#   endif	/*  DOC_SELECT_LAYOUT_H  */
