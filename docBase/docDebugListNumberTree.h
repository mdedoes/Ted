/************************************************************************/
/*									*/
/*  Lists: Diagnostic routines.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_LIST_NUMBER_TREE_H
#   define	DOC_DEBUG_LIST_NUMBER_TREE_H

struct ListNumberTreeNode;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docListListNumberNode(	const struct ListNumberTreeNode *	lntn );
extern int docCheckListNumberNode(	const struct ListNumberTreeNode *	lntn );

extern int docListNumberTreeLogPath(	struct ListNumberTreeNode **	path,
					int *			nums,
					int			level );

extern int docListNumberTreeFindParagraph(
					int *			pLevel,
					struct ListNumberTreeNode **	path,
					int *			nums,
					struct ListNumberTreeNode *	root,
					int			paraNr );

#   endif	/*	DOC_DEBUG_LIST_NUMBER_TREE_H	*/
