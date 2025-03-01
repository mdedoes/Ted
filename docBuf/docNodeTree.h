/************************************************************************/
/*									*/
/*  Manipulation of the node tree and navigation over it.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_NODE_TREE_H
#   define	DOC_NODE_TREE_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct RowProperties;
struct SelectionScope;
struct LayoutPosition;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docNumberOfParagraph( const struct BufferItem *	paraNode );

extern struct BufferItem * docGetParagraphByNumber(
				const struct DocumentTree *	tree,
				int				n );

extern void docFreeNode(	struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				struct BufferItem *		node );

extern struct BufferItem * docInsertNode(
				const struct BufferDocument *	bd,
				struct BufferItem *		parent,
				int				n,
				int				level );

extern struct BufferItem * docAppendParagraph(
				struct BufferDocument *		bd,
				struct BufferItem *		node,
				int				txtAttrNr );

extern void docDeleteNodes(		struct BufferDocument *	bd,
					struct DocumentTree *	tree,
					struct BufferItem *	node,
					int			first,
					int			count );

extern int docSplitGroupNode(		struct BufferDocument *	bd,
					struct BufferItem **	pNewNode,
					struct BufferItem *	oldNode,
					int			n );

extern int docSplitGroupNodeAtLevel(	struct BufferDocument *	bd,
					struct BufferItem **	pBeforeNode,
					struct BufferItem **	pAfterNode,
					struct BufferItem *	splitNode,
					int			n,
					int			level );

extern int docMergeGroupNodes(		struct BufferDocument *	bd,
					struct BufferItem *	to,
					struct BufferItem *	from );

extern struct BufferItem * docNextParagraph(	struct BufferItem *	node );
extern struct BufferItem * docPrevParagraph(	struct BufferItem *	node );

extern struct BufferItem * docNextSection(	struct BufferItem *	node );
extern struct BufferItem * docPrevSection(	struct BufferItem *	node );

extern void docDeleteNode(		struct BufferDocument *		bd,
					struct DocumentTree *		tree,
					struct BufferItem *		node );

extern void docInitNode( struct BufferItem *		node,
			struct BufferItem *		parent,
			const struct BufferDocument *	bd,
			int				numberInParent,
			int				level,
			int				treeType );

extern struct BufferItem * docInsertRowNode(
				struct BufferDocument *		bd,
				struct BufferItem *		sectNode,
				int				n,
				const struct RowProperties *	rp,
				int				textAttrNr );

extern struct BufferItem * docGetCellNode(	struct BufferItem *	node );
extern struct BufferItem * docGetRowNode(	struct BufferItem *	node );
extern struct BufferItem * docGetRowLevelNode(	struct BufferItem *	node );
extern struct BufferItem * docGetSectNode(	struct BufferItem *	node );

extern int docRowNesting(		const struct BufferItem *	node );
extern int docTableNesting(		const struct BufferItem *	node );
extern void docSetParaTableNesting(	struct BufferItem *	paraNode,
					const struct BufferDocument *	bd );

extern int docValidChildLevel(		int		parentLevel,
					int		childLevel );

extern struct BufferItem * docMakeNode( void );

extern struct BufferItem * docGetBodySectNode(
				struct BufferItem *		node,
				const struct BufferDocument *	bd );

extern struct BufferItem * docGetCommonParent(
				struct BufferItem *		paraNode1,
				struct BufferItem *		paraNode2 );

extern struct BufferItem * docGetBodySectNodeOfScope(
				const struct SelectionScope *	ss,
				const struct BufferDocument *	bd );

extern void docSetTreeTypeOfNode(	struct BufferItem *	node,
					int			treeType );

extern void docInvalidateSectHeaderFooterLayout(
				struct BufferItem *		sectNode );

extern void docInvalidateParagraphLayout( struct BufferItem *	paraNode );

extern void docInvalidateNodeLayout(	struct BufferItem *	node );

extern int docNodeAtExtremity(	int *				pAtExtremity,
				const struct BufferItem *	parentNode,
				const struct BufferItem *	paraNode,
				int				after );

extern int docGetMatchingCell(	const struct BufferItem *	rowNode,
				const struct BufferItem *	cellNode );

extern void docTableDetermineCellspans(
				int *				pRowspan,
				int *				pColspan,
				const struct BufferItem *	cellNode );

extern int docGetCellBottom(	struct LayoutPosition *		lpBottom,
				const struct BufferItem *	cellNode );

extern void docDelimitTables(		struct BufferItem *	parentNode,
					int			recursively );

#   endif
