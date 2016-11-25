/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible combinations*/
/*  of rows in the document.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docRowPropertyAdmin.h"
#   include	"docRowProperties.h"
#   include	"docCellProperties.h"
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

# define RP_PROP_COUNT( rp )\
		( RPprop_ADMIN_COUNT+ 1+ (rp)->rpCellCount* CLprop_COUNT )

/************************************************************************/

static int docGetRowPropertiesProperty(	const RowProperties *	rp,
					int			prop )
    {
    int		column;
    int		cellpr;

    if  ( prop < 0 )
	{ LDEB(prop); return -1;	}
    if  ( prop >= RP_PROP_COUNT( rp ) )
	{
	LLLDEB(prop,rp->rpCellCount,RP_PROP_COUNT(rp));
	return 0;
	}

    if  ( prop <= RPprop_ADMIN_COUNT )
	{ return docGetRowProperty( rp, prop );	}

    column= ( prop- RPprop_ADMIN_COUNT- 1 )/ CLprop_COUNT;
    cellpr= ( prop- RPprop_ADMIN_COUNT- 1 )% CLprop_COUNT;

    return docGetCellProperty( rp->rpCells+ column, cellpr );
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean row properties administration.			*/
/*									*/
/************************************************************************/

static int docCopyVRowPropertiesDam0(
				void *			vrpTo,
				const void *		vrpFrom )
    {
    RowProperties *		rpTo= (RowProperties *)vrpTo;
    const RowProperties *	rpFrom= (RowProperties *)vrpFrom;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( docCopyRowProperties( rpTo, rpFrom, dam0 ) )
	{ LDEB(rpFrom->rpCellCount); return -1;	}

    return 0;
    }

void docInitRowPropertiesList(	NumberedPropertiesList *	rpl )
    {
    int			num;
    RowProperties		rp;

    docInitRowProperties( &rp );
    utilInitNumberedPropertiesList( rpl );

    utilStartNumberedPropertyList( rpl,

		    1, /* An arbitrary value */
		    (NumberedPropertiesGetProperty)docGetRowPropertiesProperty,
		    docCopyVRowPropertiesDam0,
		    (NumberedPropertiesGetNumber)docRowPropertiesNumberImpl,

		    sizeof(RowProperties),
		    (InitPagedListItem)docInitRowProperties,
		    (CleanPagedListItem)docCleanRowProperties );

    num= docRowPropertiesNumberImpl( rpl, &rp );
    if  ( num != 0 )
	{ LDEB(num);	}

    docCleanRowProperties( &rp );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a ruler number to a value.				*/
/*									*/
/************************************************************************/

const RowProperties * docGetRowPropertiesByNumberImpl(
					const NumberedPropertiesList *	rpl,
					int				n )
    {
    void *	vrp= utilPagedListGetItemByNumber( &(rpl->nplPagedList), n );

    if  ( ! vrp )
	{
	static RowProperties	defRp;

	LXDEB(n,vrp);
	docInitRowProperties( &defRp );
	return &defRp;
	}

    return (RowProperties *)vrp;
    }

/************************************************************************/
/*									*/
/*  Translate row properties to their number.				*/
/*									*/
/************************************************************************/

int docRowPropertiesNumberImpl(	NumberedPropertiesList *	npl,
				const RowProperties *		rp )
    {
    const int		make= 1;

    int			propCount= RP_PROP_COUNT( rp );
    int			rval;

    if  ( propCount < 1 )
	{ LDEB(propCount); return -1;	}
    if  ( ! npl->nplGetProperty )
	{ XDEB(npl->nplGetProperty); return -1;	}

    rval= utilGetPropertyNumberX( npl, make, propCount, rp );
    if  ( rval < 0 )
	{ LLLDEB(make,propCount,rval); return -1;	}

    return rval;
    }

