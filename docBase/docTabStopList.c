/************************************************************************/
/*									*/
/*  Manage tab stops.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	"docTabStop.h"
#   include	"docTabStopList.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a tab stop List.						*/
/*									*/
/************************************************************************/

void docInitTabStopList(	TabStopList *	tsl )
    {
    tsl->tslTabStopCount= 0;
    tsl->tslTabStops= (TabStop *)0;

    return;
    }

void docCleanTabStopList(	TabStopList *	tsl )
    {
    if  ( tsl->tslTabStops )
	{ free( tsl->tslTabStops );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a tab stop into the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

int docAddTabToList(	TabStopList *		tsl,
			const TabStop *		tsNew )
    {

    TabStop *		ts;
    int			i;

    {
    TabStop *		fresh;

    fresh= (TabStop *)realloc( tsl->tslTabStops,
			    ( tsl->tslTabStopCount+ 1 )* sizeof(TabStop) );
    if  ( ! fresh )
	{ LXDEB(tsl->tslTabStopCount,fresh); return -1; }
    tsl->tslTabStops= fresh;
    }

    i= tsl->tslTabStopCount;
    ts= tsl->tslTabStops+ tsl->tslTabStopCount;
    while( i > 0 && ts[-1].tsOffset > tsNew->tsOffset )
	{ ts[0]= ts[-1]; i--; ts--;	}

    *ts= *tsNew;
    tsl->tslTabStopCount++;

    return i;
    }

/************************************************************************/
/*									*/
/*  Remove a tab stop from the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

void docDeleteTabFromList(	TabStopList *		tsl,
				int			n )
    {
    TabStop *		ts;

    if  ( n < 0 || n >= tsl->tslTabStopCount )
	{ LLDEB(n,tsl->tslTabStopCount); return;	}

    tsl->tslTabStopCount--;

    ts= tsl->tslTabStops+ n;
    while( n < tsl->tslTabStopCount )
	{ ts[0]= ts[1]; ts++; n++;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Copy the list of tabs of one paragraph to another.			*/
/*									*/
/************************************************************************/

int docCopyTabStopList(		TabStopList *		to,
				const TabStopList *	from )
    {
    TabStop *	fresh= (TabStop *)0;

    if  ( from->tslTabStopCount > 0 )
	{
	int	i;

	fresh= (TabStop *)malloc( from->tslTabStopCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(from->tslTabStopCount,fresh); return -1;	}

	for ( i= 0; i < from->tslTabStopCount; i++ )
	    { fresh[i]= from->tslTabStops[i];	}
	}

    if  ( to->tslTabStops )
	{ free( to->tslTabStops );	}
    to->tslTabStops= fresh;
    to->tslTabStopCount= from->tslTabStopCount;

    return 0;
    }

int docEqualTabStopList(	const TabStopList *	ts1,
				const TabStopList *	ts2 )
    {
    int		i;

    if  ( ts1->tslTabStopCount != ts2->tslTabStopCount )
	{ return 0;	}

    for ( i= 0; i < ts1->tslTabStopCount; i++ )
	{
	if  ( ! docEqualTabStop( &(ts1->tslTabStops[i]),
						&(ts2->tslTabStops[i]) ) )
	    { return 0;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Find the position of the next tab.					*/
/*									*/
/************************************************************************/

# define DEFTAB 720

static int docNextCalculatedTabStop(
			TabStop *			pTs,
			int *				pX,
			int *				pTab,
			int				tabInterval,
			int				xPosition )
    {
    if  ( tabInterval == 0 )
	{ LDEB(tabInterval); tabInterval= DEFTAB;	}

    docCalculatedTabStop( pTs,
	    tabInterval* ( ( xPosition+ 1 )/ tabInterval )+ tabInterval );
    *pX= pTs->tsOffset;
    *pTab= -1;

    return 0;
    }

int docNextTabStop(	TabStop *			pTs,
			int *				pX,
			int *				pTab,
			const TabStopList *		tsl,
			int				tabInterval,
			int				xPosition )
    {
    const TabStop *		ts= tsl->tslTabStops;


    if  ( tsl->tslTabStopCount == 0 )
	{
	return docNextCalculatedTabStop( pTs, pX, pTab,
						tabInterval, xPosition );
	}
    else{
	int		tab= 0;

	for ( tab= 0; tab < tsl->tslTabStopCount; ts++, tab++ )
	    {
	    if  ( ts->tsOffset > xPosition )
		{ break;	}
	    }

	if  ( tab >= tsl->tslTabStopCount )
	    {
	    return docNextCalculatedTabStop( pTs, pX, pTab,
						tabInterval, xPosition );
	    }

	*pTs= *ts;
	*pTab= tab;
	*pX= ts->tsOffset;

	return 0;
	}
    }

static int docCompareTabStopOffsets(	const void *	voidts1,
					const void *	voidts2	)
    {
    const TabStop *	ts1= (const TabStop *)voidts1;
    const TabStop *	ts2= (const TabStop *)voidts2;

    return ts1->tsOffset- ts2->tsOffset;
    }

void docTabStopListSortByOffset(	TabStopList *	ts )
    {
    qsort( ts->tslTabStops, ts->tslTabStopCount,
				sizeof(TabStop), docCompareTabStopOffsets );
    }
