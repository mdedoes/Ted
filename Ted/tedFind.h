/************************************************************************/
/*									*/
/*  Find utilities.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_FIND_H
#   define	TED_FIND_H

struct DocumentSelection;
struct BufferItem;
struct BufferDocument;
struct DocumentPosition;
struct DocumentTree;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSpellParaFindNext(
				struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const struct DocumentPosition *	dpFrom,
				void *				through );

#   endif	/*  TED_FIND_H */
