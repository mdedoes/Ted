/************************************************************************/
/*									*/
/*  Read the the list override table of a document.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	<docListAdmin.h>
#   include	<docListOverride.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write a list override table to rtf.					*/
/*									*/
/************************************************************************/

static void docRtfWriteListOverrideLevels(
				RtfWriter *			rwc,
				const ListOverride *		lo )
    {
    const ListOverrideLevel *	lol;
    int				lev;

    docRtfWriteNextLine( rwc );

    lol= lo->loLevels;
    for ( lev= 0; lev < lo->loLevelCount; lol++, lev++ )
	{
	docRtfWriteDestinationBegin( rwc, "lfolevel" );

	if  ( lol->lolOverrideStartAt && ! lol->lolOverrideFormat )
	    {
	    docRtfWriteArgTag( rwc, "listoverridestartat\\levelstartat",
						lol->lolListLevel.llStartAt );
	    }

	if  ( lol->lolOverrideFormat )
	    {
	    docRtfWriteTag( rwc, "listoverrideformat" );

	    docRtfWriteListLevel( rwc, &(lol->lolListLevel) );
	    }

	docRtfWriteDestinationEnd( rwc );
	docRtfWriteNextLine( rwc );
	}
    return;
    }

/************************************************************************/
/*									*/
/*  Write the list override table for a document.			*/
/*									*/
/************************************************************************/

void docRtfWriteListOverrideTable(	RtfWriter *			rwc,
					const ListOverrideTable *	lot )
    {
    int				ov;
    const ListOverride *	lo;
    int				overrideCount;

    docRtfWriteDestinationBegin( rwc, "*\\listoverridetable" );
    docRtfWriteNextLine( rwc );

    overrideCount= lot->lotOverrideCount;
    while( lot->lotOverrideCount > 0				&&
	   lot->lotOverrides[overrideCount- 1].loIndex < 0	)
	{ overrideCount--;	}

    lo= lot->lotOverrides;
    for ( ov= 0; ov < overrideCount; lo++, ov++ )
	{
	if  ( lo->loIndex < 1 )
	    { continue;	}

	docRtfWriteDestinationBegin( rwc, "listoverride" );

	if  ( lo->loListID != -1 )
	    { docRtfWriteArgTag( rwc, "listid", lo->loListID );	}

	docRtfWriteArgTag( rwc, "listoverridecount", lo->loOverrideCount );

	if  ( lo->loLevelCount > 0 )
	    { docRtfWriteListOverrideLevels( rwc, lo ); }

	docRtfWriteArgTag( rwc, "ls", lo->loIndex );

	docRtfWriteDestinationEnd( rwc );
	if  ( ov+ 1 < overrideCount )
	    { docRtfWriteNextLine( rwc );	}
	}

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteNextLine( rwc );

    return;
    }

