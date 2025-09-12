/************************************************************************/
/*									*/
/*  List related functionality for the document as a whole		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_UTIL_H
#   define	DOC_LIST_UTIL_H

struct DocumentTree;
struct BufferDocument;
struct ListOverride;
struct DocumentList;
struct ListLevel;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docGetListOfParagraph(	struct ListOverride **		pLo,
					struct DocumentList **		pDl,
					int				ls,
					const struct BufferDocument *	bd );

extern int docGetListLevelOfParagraph(
				int *				startPath,
				int *				formatPath,
				struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				const struct ListLevel **	pLl,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern int docGetListLevel(	int *				startPath,
				int *				formatPath,
				struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				const struct ListLevel **	pLl,
				int				listOverride,
				int				listLevel,
				const struct BufferDocument *	bd );

extern int docRemoveParagraphFromList(
				struct BufferItem *		paraNode,
				struct DocumentTree *		dt,
				const struct BufferDocument *	bd );

extern int docAdaptParagraphToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const struct BufferDocument *	bd );

extern int docApplyListRuler(	const struct DocumentList *	dl,
				const struct ListOverride *	lo,
				struct BufferDocument *		bd );

extern int docListNumbersInsertParagraph(
			    struct DocumentTree *	tree,
			    struct BufferDocument *	bd,
			    int				ls,
			    int				ilvl,
			    int				paraNr );

extern int docListNumbersGetNumberPath(
				int *			numberPath,
				struct DocumentTree *	tree,
				struct BufferDocument *	bd,
				int			ls,
				int			ilvl,
				int			paraNr );

extern int docListNumbersGetRootOffset( int *		pRootOffset,
				struct DocumentTree *	tree,
				struct BufferDocument *	bd,
				int			ls,
				int			ilvl,
				int			paraNr );

extern int docListNumbersDeleteParagraph(
				struct DocumentTree *	tree,
				struct BufferDocument *	bd,
				int			ls,
				int			paraNr );

extern int docListNumbersPrevNumberPath( int *		numberPath,
				int *			pLevel,
				struct DocumentTree *	tree,
				struct BufferDocument *	bd,
				int			ls,
				int			paraNr );

#   endif	/*  DOC_LIST_UTIL_H  */
