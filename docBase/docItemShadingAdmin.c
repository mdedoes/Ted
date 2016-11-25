/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docItemShadingAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void docInitItemShadingList(	NumberedPropertiesList *	isl )
    {
    int			num;
    ItemShading		is;

    utilInitNumberedPropertiesList( isl );

    utilStartNumberedPropertyList( isl,
    
			ISprop_COUNT,
			(NumberedPropertiesGetProperty)docGetShadingProperty,

			sizeof(ItemShading),
			(InitPagedListItem)docInitItemShading,
			(CleanPagedListItem)0 );

    docInitItemShading( &is );

    num= docItemShadingNumber( isl, &is );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute number to a struct value.		*/
/*									*/
/************************************************************************/

void docGetItemShadingByNumber(	ItemShading *			is,
				const NumberedPropertiesList *	isl,
				int				n )
    {
    void *	vis= utilPagedListGetItemByNumber( &(isl->nplPagedList), n );

    if  ( ! vis )
	{ LXDEB(n,vis); docInitItemShading( is ); return; }

    *is= *((ItemShading *)vis);
    return;
    }

/************************************************************************/
/*									*/
/*  Is this number really a shading?					*/
/*									*/
/************************************************************************/

int docShadingNumberIsShading(		const NumberedPropertiesList *	isl,
					int				n )
    {
    ItemShading	is;

    if  ( n < 0 )
	{ return 0;	}

    docGetItemShadingByNumber( &is, isl, n );

    return DOCisSHADING( &is );
    }

/************************************************************************/
/*									*/
/*  Call a function for all ItemShading in the list.		*/
/*									*/
/************************************************************************/

void docForAllItemShadings(	const NumberedPropertiesList *		isl,
				ItemShadingFunction		f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < isl->nplPagedList.plItemCount; n++ )
	{
	void *      vis= utilPagedListGetItemByNumber(
						&(isl->nplPagedList), n );

	(*f)( (ItemShading *)vis, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int docItemShadingNumber(	NumberedPropertiesList *	isl,
				const ItemShading *		is )
    {
    const int	make= 1;

    return utilGetPropertyNumber( isl, make, (void *)is );
    }

/************************************************************************/
/*									*/
/*  Merge shading lists and create a mapping.				*/
/*									*/
/************************************************************************/

int docMergeItemShadingLists(
				int **				pShadingMap,
				const int *			cmap,
				NumberedPropertiesList *	islTo,
				const NumberedPropertiesList *	islFrom )
    {
    int		fromCount= islFrom->nplPagedList.plItemCount;

    if  ( fromCount > 0 )
	{
	int		n;
	int *		smap= (int *)malloc( fromCount* sizeof(int) );

	if  ( ! smap )
	    { LXDEB(fromCount,smap); return -1; }

	for ( n= 0; n < fromCount; n++ )
	    { smap[n]= -1;	}

	for ( n= 0; n < fromCount; n++ )
	    {
	    int				to;
	    void *			vis;
	    ItemShading			is;

	    vis= utilPagedListGetItemByNumber( &(islFrom->nplPagedList), n );
	    if  ( ! vis )
		{ continue;	}

	    is= *((ItemShading *)vis);
	    if  ( is.isBackColor > 0 && cmap )
		{ is.isBackColor= cmap[is.isBackColor];	}
	    if  ( is.isForeColor > 0 && cmap )
		{ is.isForeColor= cmap[is.isForeColor];	}

	    to= docItemShadingNumber( islTo, &is );
	    if  ( to < 0 )
		{ LDEB(to); free( smap ); return -1;	}
	    smap[n]= to;
	    }

	*pShadingMap= smap;
	}

    return 0;
    }

