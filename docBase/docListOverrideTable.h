/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_OVERRIDE_TABLE_H
#   define	DOC_LIST_OVERRIDE_TABLE_H

struct ListOverride;

/**
 * The list overrides in the document
 * List overrides serve two purposes:
 * - Provide an indirect reference from the paragraph to the list that it belongs to
 * - Make it possible to override some of the formatting for particular (groups of) paragraphs in the list.
 */
typedef struct ListOverrideTable
    {
    struct ListOverride *	lotOverrides;
    int				lotOverrideCount;
    } ListOverrideTable;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListOverrideTable(	ListOverrideTable *		lot );
extern void docCleanListOverrideTable(	ListOverrideTable *		lot );

extern int docCopyListOverrideTable(	ListOverrideTable *		to,
					const ListOverrideTable *	from );

extern int docListOverrideTableAddOverride(
				struct ListOverride **		pLo,
				ListOverrideTable *		lot,
				int				ls,
				int				listId,
				int				listIndex );

extern int docListOverrideTableSetOverride(
				ListOverrideTable *		lot,
				const struct ListOverride *	lo,
				const int *			listIndexMap,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docMergeListOverrideIntoTable(
				ListOverrideTable *		lot,
				const struct ListOverride *	lo,
				const int *			listIndexMap,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docListOverrideTableDeleteOverride(
				ListOverrideTable *		lot,
				int				ls );

#   endif
