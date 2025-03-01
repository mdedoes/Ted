/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_LIST_H
#   define	DOC_FONT_LIST_H

struct DocumentFont;
struct DocumentFontList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct DocumentFont * fontFontListGetFontByNumber(
					const struct DocumentFontList *	dfl,
					int				n );

extern void fontInitDocFontList(	struct DocumentFontList *	dfl );

extern void fontCleanDocFontList(	struct DocumentFontList *	dfl );

extern int fontCopyDocFontList(		struct DocumentFontList *	to,
					const struct DocumentFontList *	from );

extern int fontListGetFontByName(	struct DocumentFontList *	dfl,
					const char *		fontName );

extern int fontMergeFontIntoList(	struct DocumentFontList *	dflTo,
					const struct DocumentFont *	dfFrom );

extern const struct DocumentFont * fontListGetFontBySortIndex(
					struct DocumentFontList *	dfl,
					int			idx );

extern int fontListGetSortIndex(	struct DocumentFontList *	dfl,
					int			aidx );

extern int fontListGetArrayIndex(	struct DocumentFontList *	dfl,
					int			sidx );

extern struct DocumentFont * fontAddDocFontToList(
					struct DocumentFontList * dfl,
					const char *		name,
					int			styleInt,
					int			pitch );

extern void fontListClearCharsUsed(	struct DocumentFontList *	dfl );

extern int fontListMergeLists(	struct DocumentFontList *	dflTo,
				const struct DocumentFontList *	dflFrom,
				int *				fontMap,
				const unsigned char *		fontUsed );

extern int fontListAllocateMergeAdmin(
				int **				pFontMap,
				unsigned char **		pFontUsed,
				const struct DocumentFontList *	dflFrom );

extern void fontListFontList(	const struct DocumentFontList *	dfl );

#   endif	/*  DOC_FONT_LIST_H	*/
