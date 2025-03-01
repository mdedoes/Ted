/************************************************************************/
/*									*/
/*  A list of fields.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_DOCUMENT_FIELD_LIST_H
#   define DOC_DOCUMENT_FIELD_LIST_H

#   include	<utilPagedList.h>

typedef struct DocumentFieldList
    {
    PagedList		dflPagedList;
    } DocumentFieldList;

struct DocumentField;
struct SelectionScope;
struct EditPosition;
struct MemoryBuffer;

typedef int (*FieldListForAllFun)(	int			n,
					struct DocumentField *	df,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern struct DocumentField * docFieldListGetByNumber(
					const DocumentFieldList *	dfl,
					int				n );

extern void docDeleteFieldFromList(	DocumentFieldList *		dfl,
					struct DocumentField *		df );

extern int docGetFieldKindByNumber(	const DocumentFieldList *	dfl,
					int				n );

extern void docCleanFieldList(		DocumentFieldList *	dfl );
extern void docInitFieldList(		DocumentFieldList *	dfl );

extern int docSetPageOfField(		DocumentFieldList *	dfl,
					int			n,
					int			page );

extern struct DocumentField * docFieldListClaimField(	DocumentFieldList *	dfl );

extern struct DocumentField * docClaimFieldCopy(
				DocumentFieldList *		dfl,
				const struct DocumentField *	dfFrom,
				const struct SelectionScope *	ss,
				const struct EditPosition *	epStart );

extern int docFieldListFindBookmark( struct DocumentField **	pDf,
				const DocumentFieldList *	dfl,
				const struct MemoryBuffer *	markName );

extern int docMakeBookmarkList(	char ***			pBookmarks,
				int *				pBookmarkCount,
				int				includeTocMarks,
				const DocumentFieldList *	dfl );

extern int docFieldListForAll(	const DocumentFieldList *	pl,
				FieldListForAllFun		fun,
				void *				through );

#   endif /*  DOC_DOCUMENT_FIELD_LIST_H  */
