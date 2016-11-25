/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible rulers for	*/
/*  a paragraph								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_RULER_ADMIN_H
#   define	DOC_PARA_RULER_ADMIN_H

struct TabStopList;
struct NumberedPropertiesList;

typedef void (*TabStopListFunction)(
			const struct TabStopList *	 	tsl,
			int					n,
			void *					through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTabStopListList(
			struct NumberedPropertiesList *		tsll );

extern int docTabStopListNumberImpl(
			struct NumberedPropertiesList *		tsll,
			const struct TabStopList *		tsl );

extern const struct TabStopList * docGetTabStopListByNumberImpl(
			const struct NumberedPropertiesList *	tsll,
			int					n );

extern int docMergeTabstopListLists(
			int **					pRulerMap,
			struct NumberedPropertiesList *		tsllTo,
			const struct NumberedPropertiesList *	tsllFrom );

#   endif	/*  DOC_PARA_RULER_ADMIN_H	*/
