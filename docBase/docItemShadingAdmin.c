/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docItemShading.h"
#   include	"docItemShadingAdmin.h"
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

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
			(NumberedPropertiesCopyProperties)0,
			(NumberedPropertiesGetNumber)docItemShadingNumberImpl,

			sizeof(ItemShading),
			(InitPagedListItem)docInitItemShading,
			(CleanPagedListItem)0 );

    docInitItemShading( &is );

    num= docItemShadingNumberImpl( isl, &is );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute number to a struct value.		*/
/*									*/
/************************************************************************/

const ItemShading * docGetItemShadingByNumberImpl(
				const NumberedPropertiesList *	isl,
				int				n )
    {
    void *	vis= utilPagedListGetItemByNumber( &(isl->nplPagedList), n );

    if  ( ! vis )
	{
	static ItemShading	defIs;

	LXDEB(n,vis);
	docInitItemShading( &defIs );
	return &defIs;
	}

    return (ItemShading *)vis;
    }

/************************************************************************/
/*									*/
/*  Is this number really a shading?					*/
/*									*/
/************************************************************************/

int docShadingNumberIsShadingImpl(	const NumberedPropertiesList *	isl,
					int				n )
    {
    const ItemShading *	is;

    if  ( n < 0 )
	{ return 0;	}

    is= docGetItemShadingByNumberImpl( isl, n );

    return DOCisSHADING( is );
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

int docItemShadingNumberImpl(	NumberedPropertiesList *	isl,
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

static int docTranslateItemShading(
				void *				visTo,
				const void *			visFrom,
				void *				vcolorMap )
    {
    ItemShading *		isTo= (ItemShading *)visTo;
    const ItemShading *		isFrom= (const ItemShading *)visFrom;
    const int *			colorMap= (const int *)vcolorMap;

    *isTo= *isFrom;
    if  ( isFrom->isBackColor > 0 && colorMap )
	{ isTo->isBackColor= colorMap[isFrom->isBackColor];	}
    if  ( isFrom->isForeColor > 0 && colorMap )
	{ isTo->isForeColor= colorMap[isFrom->isForeColor];	}

    return 0;
    }

int docMergeItemShadingLists(	int **				pShadingMap,
				const int *			colorMap,
				NumberedPropertiesList *	islTo,
				const NumberedPropertiesList *	islFrom )
    {
    return docMergeNumberedPropertiesLists( pShadingMap, islTo, islFrom,
				docTranslateItemShading, (void *)colorMap );
    }

