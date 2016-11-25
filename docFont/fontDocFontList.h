/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_LIST_H
#   define	DOC_FONT_LIST_H

struct DocumentFont;

typedef struct DocFontSort
    {
    int		arrayIndex;
    int		sortIndex;
    } DocFontSort;

typedef struct DocumentFontList
    {
    int				dflFontCount;
    struct DocumentFont **	dflFontPages;
    int				dflFontPageCount;
    DocFontSort *		dflFontSortIndex;
    } DocumentFontList;

# define DFL_PGSZ	256

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct DocumentFont * fontFontListGetFontByNumber(
					const DocumentFontList *	dfl,
					int				n );

extern void fontInitDocFontList(	DocumentFontList *		dfl );

extern void fontCleanDocFontList(	DocumentFontList *		dfl );

extern int fontCopyDocFontList(		DocumentFontList *		to,
					const DocumentFontList *	from );

extern int fontListGetFontByName(	DocumentFontList *	dfl,
					const char *		fontName );

extern int fontMergeFontIntoList(	DocumentFontList *	dflTo,
					const struct DocumentFont *	dfFrom );

extern const struct DocumentFont * fontListGetFontBySortIndex(
						DocumentFontList *	dfl,
						int			idx );

extern int fontListGetSortIndex(		DocumentFontList *	dfl,
						int			aidx );

extern int fontListGetArrayIndex(		DocumentFontList *	dfl,
						int			sidx );

extern struct DocumentFont * fontAddDocFontToList( DocumentFontList *	dfl,
					const char *		name,
					int			styleInt,
					int			pitch );

extern void fontListClearCharsUsed(	DocumentFontList *	dfl );

#   endif	/*  DOC_FONT_LIST_H	*/
