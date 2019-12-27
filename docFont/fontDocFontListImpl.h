/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_LIST_IMPL_H
#   define	DOC_FONT_LIST_IMPL_H

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

#   endif	/*  DOC_FONT_LIST_IMPL_H	*/
