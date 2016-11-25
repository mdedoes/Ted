/************************************************************************/
/*									*/
/*  Lists: Diagnostic routines.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_LIST_H
#   define	DOC_DEBUG_LIST_H

struct DocumentList;
struct ListOverrideLevel;
struct ListOverrideTable;
struct ListOverride;
struct DocumentListTable;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docListDocumentList(	int				n,
					const struct DocumentList *		dl );

extern void docListListOverrideLevel(	int				n,
					const struct ListOverrideLevel *	lol );

extern void docListListOverrideTable(	int				n,
					const struct ListOverrideTable *	lot );

extern void docListListOverride(	int				n,
					const struct ListOverride *		lo );

extern void docListDocumentListTable( const struct DocumentListTable *		dlt );

#   endif	/*	DOC_DEBUG_LIST_H	*/
