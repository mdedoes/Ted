/************************************************************************/
/*									*/
/*  Tabs in the ruler of a paragraph.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TAB_STOP_LIST_H
#   define	DOC_TAB_STOP_LIST_H

struct TabStop;

typedef struct TabStopList
    {
			/**
			 *  The tab stops in the list. They are sorted 
			 *  by offset.
			 */
    struct TabStop *	tslTabStops;

			/**
			 *  The number of tab stops in the list.
			 */
    short int		tslTabStopCount;
    } TabStopList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void docInitTabStopList(	TabStopList *			tsl );
void docCleanTabStopList(	TabStopList *			tsl );

extern int docAddTabToList(	TabStopList *			tsl,
				const struct TabStop *		tsNew );

extern void docDeleteTabFromList( TabStopList *			tsl,
				int				n );

extern int docCopyTabStopList(	TabStopList *			to,
				const TabStopList *		from );

extern int docEqualTabStopList(	const TabStopList *		ts1,
				const TabStopList *		ts2 );

extern void docTabStopListSortByOffset(
				TabStopList *			ts );

extern int docNextTabStop(	struct TabStop *		ts,
				int *				pX,
				int *				pTab,
				const TabStopList *		tsl,
				int				tabInterval,
				int				xPosition );

#   endif	/*  DOC_TAB_STOP_LIST_H	*/
