/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_NUMBER_TREE_H
#   define	DOC_LIST_NUMBER_TREE_H

struct ListNumberTreeNode;

typedef struct ListNumberTrees
    {
    struct ListNumberTreeNode *	lntTrees;
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
					int			ls,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesDeleteParagraph(
					ListNumberTrees *	lnt,
					int			ls,
					int			paraNr );

extern int docListNumberTreesGetNumberPath(	
					int *			numberPath,
					ListNumberTrees *	lnt,
					int			ls,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesGetRootOffset(	
					int *			pRootOffset,
					ListNumberTrees *	lnt,
					int			ls,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreesPrevNumberPath(	
					int *			numberPath,
					int *			pLevel,
					ListNumberTrees *	lnt,
					int			ls,
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
