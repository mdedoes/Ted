/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_NUMBER_TREE_H
#   define	DOC_LIST_NUMBER_TREE_H

typedef struct ListNumberTreeNode
    {
				/**
				 *  The paragraph number in the document tree
				 *  that this node refers to.
				 */
    int				lntnParaNr;
				/**
				 *  The number of leaves in the offspring of
				 *  this node. This is to optimize calculating 
				 *  numbers.
				 */
    int				lntnLeafCount;
				/**
				 *  The array of children.
				 */
    struct ListNumberTreeNode *	lntnChildren;
				/**
				 *  The number of children.
				 */
    int				lntnChildCount;
				/**
				 *  != 0 if and only if this node is a leaf.
				 */
    unsigned char		lntnIsLeaf;
    } ListNumberTreeNode;

typedef struct ListNumberTrees
    {
    ListNumberTreeNode *	lntTrees;
    int				lntTreeCount;
    } ListNumberTrees;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListNumberTreeNode( ListNumberTreeNode *	lntn );
extern void docCleanListNumberTreeNode(	ListNumberTreeNode *	lntn );

extern void docInitListNumberTrees(	ListNumberTrees *	lnt );
extern void docCleanListNumberTrees(	ListNumberTrees *	lnt );
extern int docClaimListNumberTrees(	ListNumberTrees *	lnt,
					int			count );

extern void docShiftListNodeReferences(	ListNumberTreeNode *	root,
					int			paraFrom,
					int			paraShift );

extern void docShiftListTreeReferences(	ListNumberTrees *	lnt,
					int			paraFrom,
					int			paraShift );

extern ListNumberTreeNode * docGetListNumberTree(
					ListNumberTrees *	lnt,
					int			ls );

extern int docListNumberTreeInsertParagraph(
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesInsertParagraph(
					ListNumberTrees *	lnt,
					int			ls,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreeDeleteParagraph(
					ListNumberTreeNode *	root,
					int			paraNr );

extern int docListNumberTreesDeleteParagraph(
					ListNumberTrees *	lnt,
					int			ls,
					int			paraNr );

extern int docListNumberTreeGetNumberPath(	
					int *			numberPath,
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreeGetPrevPath(int *			numberPath,
					int *			pLevel,
					ListNumberTreeNode *	root,
					int			paraNr );

#   endif
