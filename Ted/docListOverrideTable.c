/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docListOverrideTable.h"

void docInitListOverrideTable(		ListOverrideTable *		lot )
    {
    lot->lotOverrides= (ListOverride *)0;
    lot->lotOverrideCount= 0;

    return;
    }

void docCleanListOverrideTable(		ListOverrideTable *		lot )
    {
    int		i;

    for ( i= 0; i < lot->lotOverrideCount; i++ )
	{ docCleanListOverride( &(lot->lotOverrides[i]) );	}

    if  ( lot->lotOverrides )
	{ free( (void *)lot->lotOverrides );	}

    return;
    }

int docCopyListOverrideTable(		ListOverrideTable *		to,
				const ListOverrideTable *	from )
    {
    int		i;

    if  ( to->lotOverrideCount > from->lotOverrideCount )
	{
	for ( i= from->lotOverrideCount; i < to->lotOverrideCount; i++ )
	    { docCleanListOverride( &(to->lotOverrides[i]) );	}

	to->lotOverrideCount= from->lotOverrideCount;
	}

    if  ( to->lotOverrideCount < from->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= realloc( to->lotOverrides,
				from->lotOverrideCount* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(from->lotOverrideCount,fresh); return -1;	}

	to->lotOverrides= fresh;

	while( to->lotOverrideCount < from->lotOverrideCount )
	    {
	    docInitListOverride( &(to->lotOverrides[to->lotOverrideCount++]) );
	    }
	}

    for ( i= 0; i < from->lotOverrideCount; i++ )
	{
	if  ( docCopyListOverride(
			&(to->lotOverrides[i]), &(from->lotOverrides[i]) ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a list to the listtable.					*/
/*									*/
/************************************************************************/

int docListOverrideTableAddOverride(	ListOverrideTable *		lot,
					const ListOverride *		lo )
    {
    if  ( lo->loIndex >= lot->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= (ListOverride *)realloc( lot->lotOverrides,
				    (lo->loIndex+ 1)* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(lo->loIndex,fresh); return -1;	}

	lot->lotOverrides= fresh;

	fresh += lot->lotOverrideCount;
	while( lot->lotOverrideCount <= lo->loIndex )
	    {
	    docInitListOverride( fresh );
	    lot->lotOverrideCount++; fresh++;
	    }
	}

    if  ( docCopyListOverride( lot->lotOverrides+ lo->loIndex, lo ) )
	{ LDEB(lot->lotOverrideCount); return -1;	}

    return 0;
    }
