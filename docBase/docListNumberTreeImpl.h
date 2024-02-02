/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_NUMBER_TREE_IMPL_H
#   define	DOC_LIST_NUMBER_TREE_IMPL_H

#   include	"docListNumberTree.h"

typedef struct ListNumberTreeNode
    {
				/**
				 *  The paragraph number in the document tree
				 *  that this node refers to. Internal nodes 
				 *  in the tree have lntnParaNr == -1.
				 *  Internal nodes are nodes that implement 
				 *  levels that are skipped in the indentation 
				 *  of the tree.
				 */
    int				lntnParaNr;
				/**
				 *  The array of children.
				 */
    struct ListNumberTreeNode **	lntnChildren;
				/**
				 *  The number of children.
				 */
    int				lntnChildCount;
    } ListNumberTreeNode;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern ListNumberTreeNode * docGetListNumberTree( ListNumberTrees *	lnt,
						int			ls );

extern void docInitListNumberTreeNode( ListNumberTreeNode *	lntn );
extern void docCleanListNumberTreeNode(	ListNumberTreeNode *	lntn );

extern void docShiftListNodeReferences(	ListNumberTreeNode *	root,
					int			paraFrom,
					int			paraShift );

extern int docListNumberTreeNodeInsertParagraph(
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreeNodeDeleteParagraph(
					ListNumberTreeNode *	root,
					int			paraNr );

extern int docListNumberNodeGetNumberPath(	
					int *			numberPath,
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr );

extern int docListNumberNodePrevNumberPath(
					int *			numberPath,
					int *			pLevel,
					ListNumberTreeNode *	root,
					int			paraNr );

extern int docListNumberNodeForAll(	ListNumberTreeNode *	root,
					LIST_TREE_FUNC		forOne,
					void *			through );

extern int docListNumberTreeFindParagraph(
					int *			pLevel,
					struct ListNumberTreeNode **	path,
					int *			nums,
					struct ListNumberTreeNode *	root,
					int			paraNr );

#   endif
