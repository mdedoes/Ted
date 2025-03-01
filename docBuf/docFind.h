/************************************************************************/
/*									*/
/*  Definitions for finding patterns in the document and spelling	*/
/*  checking.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FIND_H
#   define	DOC_FIND_H

struct DocumentSelection;
struct BufferItem;
struct BufferDocument;
struct DocumentPosition;
struct DocumentTree;

typedef int (*PARA_FIND_STRING)(
			    struct DocumentSelection *		ds,
			    struct BufferItem *			paraNode,
			    struct BufferDocument *		bd,
			    struct DocumentTree *		tree,
			    const struct DocumentPosition *	dpFrom,
			    void *				through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFindParaFindNext(	struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const struct DocumentPosition *	dpFrom,
				void *				through );

extern int docFindParaFindPrev(	struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const struct DocumentPosition *	dpFrom,
				void *				through );

extern int docFindFindNextInDocument(
				struct DocumentSelection *	ds,
				const struct DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through );

extern int docFindFindPrevInDocument(
				struct DocumentSelection *	ds,
				const struct DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through );

extern int docFindFindNextInCurrentTree(
				struct DocumentSelection *	ds,
				const struct DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				PARA_FIND_STRING		findNext,
				void *				through );

extern int docFindSetPattern(	void **				pProg,
				const char *			pattern,
				int				useRegex,
				int				asWord,
				int				caseSensitive );

#   endif
