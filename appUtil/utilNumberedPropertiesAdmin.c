/************************************************************************/
/*									*/
/*  Keep a collection of objects with integer properties by number.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"utilNumberedPropertiesAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean property administration.				*/
/*									*/
/************************************************************************/

void utilInitNumberedPropertiesList(	NumberedPropertiesList *	npl )
    {
    utilInitPagedList( &(npl->nplPagedList) );
    utilInitIntegerValueNode( &(npl->nplValueNodes) );

    npl->nplPropCount= 0;
    npl->nplGetProperty= (NumberedPropertiesGetProperty)0;
    }

void utilCleanNumberedPropertiesList(	NumberedPropertiesList *	npl )
    {
    utilCleanPagedList( &(npl->nplPagedList) );
    utilCleanIntegerValueNode( &(npl->nplValueNodes) );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialise an administration.					*/
/*									*/
/************************************************************************/

void utilStartNumberedPropertyList(
				NumberedPropertiesList * 	npl,
				int				propCount,
				NumberedPropertiesGetProperty	getProp,
				NumberedPropertiesCopyProperties copyProps,
				NumberedPropertiesGetNumber	getNumber,
				int				sizeofItem,
				InitPagedListItem		initItem,
				CleanPagedListItem	 	cleanItem )
    {
    npl->nplPropCount= propCount;
    npl->nplGetProperty= getProp;
    npl->nplCopyProperties= copyProps;
    npl->nplGetNumber= getNumber;

    utilStartPagedList( &(npl->nplPagedList), sizeofItem, initItem, cleanItem );
    }

/************************************************************************/
/*									*/
/*  Call a function for all properties in the list.			*/
/*									*/
/************************************************************************/

void utilForAllNumberedProperties(
				const NumberedPropertiesList *	npl,
				NumberedPropertiesFunction	f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < npl->nplPagedList.plItemCount; n++ )
	{
	void *      vta= utilPagedListGetItemByNumber(
						&(npl->nplPagedList), n );

	if  ( ! vta )
	    { continue;	}

	(*f)( vta, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an instance to its number.				*/
/*									*/
/************************************************************************/

int utilGetPropertyNumberX(	NumberedPropertiesList *	npl,
				int				make,
				int				propCount,
				const void *			vob )
    {
    int			prop;
    IntegerValueNode *	ivn= &(npl->nplValueNodes);
    void *		vta;

    if  ( propCount < 1 )
	{ LDEB(propCount); return -1;	}
    if  ( ! npl->nplGetProperty )
	{ XDEB(npl->nplGetProperty); return -1;	}

    for ( prop= 0; prop < propCount; prop++ )
	{
	int	propval= (*npl->nplGetProperty)( vob, prop );

	if  ( ivn->ivnIsLeaf )
	    { LLLDEB(prop,propCount,ivn->ivnIsLeaf); return -1;	}

	ivn= utilChildIntegerValueNode( ivn, make, propval );
	if  ( ! ivn )
	    {
	    if  ( make )
		{ LLXDEB(propval,make,ivn);	}
	    return -1;
	    }
	}

    if  ( ivn->ivnIsLeaf )
	{ return ivn->ivnReference;	}

    vta= utilPagedListClaimItemAtEnd( &(ivn->ivnReference),
						    &(npl->nplPagedList) );
    if  ( ! vta )
	{ XDEB(vta); return -1;	}

    if  ( npl->nplCopyProperties )
	{
	if  ( (*npl->nplCopyProperties)( vta, vob ) )
	    { LDEB(ivn->ivnReference); return -1;	}
	}
    else{
	memcpy( vta, vob, npl->nplPagedList.plSizeofItem );
	}

    ivn->ivnIsLeaf= 1;

    return ivn->ivnReference;
    }

int utilGetPropertyNumber(	NumberedPropertiesList *	npl,
				int				make,
				const void *			vob )
    {
    if  ( npl->nplPropCount < 1 )
	{ LDEB(npl->nplPropCount); return -1;	}

    return utilGetPropertyNumberX( npl, make, npl->nplPropCount, vob );
    }

int docMergeNumberedPropertiesLists(
				int **				pPropMap,
				NumberedPropertiesList *	nplTo,
				const NumberedPropertiesList *	nplFrom,
				NumberedPropertiesMergeProperties merge,
				void *				through )
    {
    int		rval= 0;
    int		fromCount= nplFrom->nplPagedList.plItemCount;

    int *	propMap= (int *)0;
    void *	scratch= (void *)0;

    if  ( fromCount > 0 )
	{
	int		n;

	propMap= (int *)malloc( fromCount* sizeof(int) );
	if  ( ! propMap )
	    { LXDEB(fromCount,propMap); rval= -1; goto ready; }

	for ( n= 0; n < fromCount; n++ )
	    { propMap[n]= -1;	}

	scratch= malloc( nplTo->nplPagedList.plSizeofItem );
	if  ( ! scratch )
	    {
	    LXDEB(nplTo->nplPagedList.plSizeofItem,scratch);
	    rval= -1; goto ready;
	    }

	if  ( nplTo->nplPagedList.plInitItem )
	    { (*nplTo->nplPagedList.plInitItem)( scratch );	}

	for ( n= 0; n < fromCount; n++ )
	    {
	    int			to;
	    void *      	vfrom;

	    vfrom= utilPagedListGetItemByNumber( &(nplFrom->nplPagedList), n );
	    if  ( ! vfrom )
		{ continue;	}

	    if  ( (*merge)( scratch, vfrom, through ) )
		{ LDEB(n); rval= -1; goto ready;	}

	    if  ( nplTo->nplGetNumber )
		{
		to= (*nplTo->nplGetNumber)( nplTo, scratch );
		if  ( to < 0 )
		    { LDEB(to); rval= -1; goto ready;	}
		}
	    else{
		const int	make= 1;

		to= utilGetPropertyNumber( nplTo, make, scratch );
		if  ( to < 0 )
		    { LDEB(to); rval= -1; goto ready;	}
		}

	    propMap[n]= to;

	    if  ( nplTo->nplPagedList.plCleanItem )
		{ (*nplTo->nplPagedList.plCleanItem)( scratch );	}
	    if  ( nplTo->nplPagedList.plInitItem )
		{ (*nplTo->nplPagedList.plInitItem)( scratch );	}
	    }
	}

    *pPropMap= propMap; propMap= (int *)0; /* steal */

  ready:

    if  ( propMap )
	{ free( propMap );	}

    if  ( scratch )
	{
	if  ( nplTo->nplPagedList.plCleanItem )
	    { (*nplTo->nplPagedList.plCleanItem)( scratch );	}

	free( scratch );
	}

    return rval;
    }

