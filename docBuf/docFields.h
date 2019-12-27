/************************************************************************/
/*									*/
/*  Manage fields in a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FIELDS_H
#   define	DOC_FIELDS_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct DocumentTree;
struct DocumentSelection;
struct BufferItem;
struct DocumentPosition;
struct ChildFields;
struct EditRange;
struct DocumentField;
struct MemoryBuffer;

typedef int (*DocFieldListForAllFun)(	int			n,
					struct DocumentField *	df,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docCountParticulesInFieldFwd( const struct BufferItem * paraNode,
					int *			pClosed,
					int			part,
					int			partUpto );

extern int docCountParticulesInFieldBwd( const struct BufferItem * paraNode,
					int *			pClosed,
					int			part,
					int			partFrom );

extern int docBalanceFieldSelection(	struct DocumentField **	pDfHead,
					struct DocumentField **	pDftail,
					int *			pBeginMoved,
					int *			pEndMoved,
					int *			pHeadPart,
					int *			pTailPart,
					struct DocumentPosition * dpHead,
					struct DocumentPosition * dpTail,
					struct DocumentTree *		dt,
					struct BufferDocument *	bd );

extern int docParaHeadFieldKind( const struct BufferItem *	paraNode );

extern int docDelimitParaHeadField(
				struct DocumentField **	pField,
				struct DocumentSelection *	dsInsideField,
				struct DocumentSelection *	dsAroundField,
				int *			pPartBegin,
				int *			pPartEnd,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern int docDelimitFieldInDoc(struct DocumentSelection *	dsInside,
				struct DocumentSelection *	dsAround,
				int *				pPart0,
				int *				pPart1,
				const struct BufferDocument *	bd,
				const struct DocumentField *	df );

extern int docDelimitFieldInTree(
				struct DocumentSelection *	dsInside,
				struct DocumentSelection *	dsAround,
				int *				pPart0,
				int *				pPart1,
				const struct DocumentTree *	dt,
				const struct DocumentField *	df );

extern int docDeleteFieldFromParent(
				struct DocumentTree *		dt,
				struct DocumentField *		df );

extern int docShiftFieldReferences(	struct DocumentTree *	dt,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift );

extern void docDeleteFieldFromDocument(	struct BufferDocument *	bd,
					struct DocumentField *	df );

extern int docDeleteFieldRange(	int *				pUpdateFlags,
				struct BufferDocument *		bd,
				const struct EditRange *	er,
				struct ChildFields *		rootFields );

extern void docDeleteChildFields(	int *			pUpdateFlags,
					struct BufferDocument *	bd,
					struct ChildFields *	cf );

extern struct DocumentField * docGetFieldByNumber(
					const struct BufferDocument *	bd,
					int				n );

extern int docFindBookmarkField( struct DocumentField **	pDf,
				const struct BufferDocument *	bd,
				const struct MemoryBuffer *	markName );

extern struct DocumentField * docClaimField(
					struct BufferDocument *	bd );

extern int docForAllFields(	const struct BufferDocument *	bd,
				DocFieldListForAllFun		fun,
				void *				through );

#   endif
