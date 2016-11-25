/************************************************************************/
/*									*/
/*  Definitions for finding patterns in the document and spelling	*/
/*  checking.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FIND_H
#   define	DOC_FIND_H

#   include	"docBuf.h"


typedef int (*PARA_FIND_STRING)(
			    DocumentSelection *		ds,
			    struct BufferItem *		paraBi,
			    const BufferDocument *	bd,
			    const DocumentPosition *	dpFrom,
			    void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFindParaFindNext(	DocumentSelection *		ds,
				struct BufferItem *		paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

extern int docFindParaFindPrev(	DocumentSelection *		ds,
				struct BufferItem *		paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

extern int docFindFindNextInDocument(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through );

extern int docFindFindPrevInDocument(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through );

extern int docFindFindNextInCurrentTree(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through );

#   endif
