/************************************************************************/
/*									*/
/*  Describe what is to be done because a new selection is set in a	*/
/*  formatted document.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DESCRIBE_SET_SELECTION_H
#   define	DOC_DESCRIBE_SET_SELECTION_H

struct DocumentTree;
struct BufferItem;
struct DocumentRectangle;
struct LayoutContext;
struct LayoutContext;
struct BufferDocument;
struct DocumentSelection;

extern int docDescribeSetSelection(
			struct DocumentTree **		pTreeSet,
			struct BufferItem **		pBodySectNodeSet,
			struct DocumentRectangle *	drExternalSet,
			int *				pRedrawOldTree,
			int *				pRedrawNewTree,
			struct LayoutContext *		lc,
			struct BufferDocument *		bd,
			const struct DocumentSelection * dsOld,
			const struct DocumentSelection * dsSet );

#   endif	/*  DOC_DESCRIBE_SET_SELECTION_H  */
