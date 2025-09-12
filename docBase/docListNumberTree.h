/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_NUMBER_TREE_H
#   define	DOC_LIST_NUMBER_TREE_H

struct ListNumberTreeNode;

/**
 *  Keep track of the indices (numbers) of paragraphs that belong to some
 *  kind of list.
 */
typedef struct ListNumberTrees
    {
				/**
				 * The trees per list. The index of the tree
				 * is the index of list in the document.
				 * (NOT the number of the list override)
				 */
    struct ListNumberTreeNode *	lntTrees;
				/** The number of trees */
    int				lntTreeCount;
    } ListNumberTrees;

typedef struct ListNumberTree
    {
    struct ListNumberTreeNode *	lntRoot;
    } ListNumberTree;

typedef int (*LIST_TREE_FUNC)(	int		ilvl,
				void *		through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListNumberTrees(	ListNumberTrees *	lnt );
extern void docCleanListNumberTrees(	ListNumberTrees *	lnt );
extern int docClaimListNumberTrees(	ListNumberTrees *	lnt,
					int			count );

extern void docInitListNumberTree(	ListNumberTree *	lnt );
extern void docCleanListNumberTree(	ListNumberTree *	lnt );

extern void docShiftListTreesReferences( ListNumberTrees *	lnt,
					int			paraFrom,
					int			paraShift );

extern int docListNumberTreesInsertParagraph(
					ListNumberTrees *	lnt,
					int			listIndex,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesDeleteParagraph(
					ListNumberTrees *	lnt,
					int			listIndex,
					int			paraNr );

extern int docListNumberTreesGetNumberPath(	
					int *			numberPath,
					ListNumberTrees *	lnt,
					int			listIndex,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesGetRootOffset(	
					int *			pRootOffset,
					ListNumberTrees *	lnt,
					int			listIndex,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesPrevNumberPath(	
					int *			numberPath,
					int *			pLevel,
					ListNumberTrees *	lnt,
					int			listIndex,
					int			paraNr );

extern int docListNumberTreeInsertParagraph(
					ListNumberTree *	lnt,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreeDeleteParagraph(
					ListNumberTree *	lnt,
					int			paraNr );

extern int docListNumberTreeForAll(	ListNumberTree *	lnt,
					LIST_TREE_FUNC		forOne,
					void *			through );

extern void docShiftListTreeReferences(	ListNumberTree *	lnt,
					int			paraFrom,
					int			paraShift );

extern int docListNumberTreeIsEmpty(	ListNumberTree *	lnt );
#   endif
