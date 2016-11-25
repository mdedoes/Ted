/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible rulers for	*/
/*  a paragraph								*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docParaTabsAdmin.h"
#   include	"docTabStopList.h"
#   include	"docTabStop.h"
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docGetTabStopListProperty(	const TabStopList *	tsl,
					int			prop )
    {
    if  ( prop < 0 )
	{ LDEB(prop); return -1;	}

    if  ( prop >= 1+ tsl->tslTabStopCount* TABprop_COUNT )
	{
	LLLDEB(prop,tsl->tslTabStopCount,tsl->tslTabStopCount* TABprop_COUNT);
	return 0;
	}

    if  ( prop == 0 )
	{ return tsl->tslTabStopCount;	}

    prop--;

    return docTabStopGetProperty( tsl->tslTabStops+ (prop / TABprop_COUNT),
						     prop % TABprop_COUNT );
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void docInitTabStopListList(	NumberedPropertiesList *	tsll )
    {
    int			num;
    TabStopList		tsl;

    utilInitNumberedPropertiesList( tsll );

    utilStartNumberedPropertyList( tsll,

		    1, /* An arbitrary value */
		    (NumberedPropertiesGetProperty)docGetTabStopListProperty,
		    (NumberedPropertiesCopyProperties)docCopyTabStopList,
		    (NumberedPropertiesGetNumber)docTabStopListNumberImpl,

		    sizeof(TabStopList),
		    (InitPagedListItem)docInitTabStopList,
		    (CleanPagedListItem)docCleanTabStopList );

    docInitTabStopList( &tsl );

    num= docTabStopListNumberImpl( tsll, &tsl );
    if  ( num != 0 )
	{ LDEB(num);	}

    docCleanTabStopList( &tsl );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a ruler number to a value.				*/
/*									*/
/************************************************************************/

const TabStopList * docGetTabStopListByNumberImpl(
					const NumberedPropertiesList *	tsll,
					int				n )
    {
    void *	vtsl= utilPagedListGetItemByNumber( &(tsll->nplPagedList), n );

    if  ( ! vtsl )
	{
	static TabStopList	defTsl;

	LXDEB(n,vtsl);
	docInitTabStopList( &defTsl );
	return &defTsl;
	}

    return (TabStopList *)vtsl;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int docTabStopListNumberImpl(	NumberedPropertiesList *	npl,
				const TabStopList *		tsl )
    {
    int			rval;
    const int		make= 1;

    int			propCount= 1+ tsl->tslTabStopCount* TABprop_COUNT;

    if  ( propCount < 1 )
	{ LDEB(propCount); return -1;	}
    if  ( ! npl->nplGetProperty )
	{ XDEB(npl->nplGetProperty); return -1;	}

    rval= utilGetPropertyNumberX( npl, make, propCount, tsl );
    if  ( rval < 0 )
	{ LLLDEB(make,propCount,rval); return -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Merge ruler lists and create a mapping.				*/
/*									*/
/************************************************************************/

static int docTranslateTabStopList(
				void *				vtslTo,
				const void *			vtslFrom,
				void *				through )
    {
    TabStopList *		tslTo= (TabStopList *)vtslTo;
    const TabStopList *		tslFrom= (const TabStopList *)vtslFrom;

    if  ( docCopyTabStopList( tslTo, tslFrom ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docMergeTabstopListLists(	int **				pRulerMap,
				NumberedPropertiesList *	tsllTo,
				const NumberedPropertiesList *	tsllFrom )
    {
    return docMergeNumberedPropertiesLists( pRulerMap, tsllTo, tsllFrom,
				    docTranslateTabStopList, (void *)0 );
    }

