/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docBorderPropertyAdmin.h"
#   include	"docBorderProperties.h"
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void docInitBorderPropertyList(	NumberedPropertiesList *	bpl )
    {
    int			num;
    BorderProperties	bp;

    utilInitNumberedPropertiesList( bpl );

    utilStartNumberedPropertyList( bpl,

		    BRDRprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetBorderProperty,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)docBorderPropertiesNumberImpl,

		    sizeof(BorderProperties),
		    (InitPagedListItem)docInitBorderProperties,
		    (CleanPagedListItem)0 );

    docInitBorderProperties( &bp );

    num= docBorderPropertiesNumberImpl( bpl, &bp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

const BorderProperties * docGetBorderPropertiesByNumberImpl(
					const NumberedPropertiesList *	bpl,
					int				n )
    {
    void *	vbp= utilPagedListGetItemByNumber( &(bpl->nplPagedList), n );

    if  ( ! vbp )
	{
	static BorderProperties	defBp;

	LXDEB(n,vbp);
	docInitBorderProperties( &defBp );
	return &defBp;
	}

    return (BorderProperties *)vbp;
    }

/************************************************************************/
/*									*/
/*  Is this number really a border?					*/
/*									*/
/************************************************************************/

int docBorderNumberIsBorderImpl(	const NumberedPropertiesList *	bpl,
					int				n )
    {
    const BorderProperties *	bp;

    if  ( n < 0 )
	{ return 0;	}

    bp= docGetBorderPropertiesByNumberImpl( bpl, n );

    return DOCisBORDER( bp );
    }

/************************************************************************/
/*									*/
/*  Call a function for all BorderProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllBorderProperties(	const NumberedPropertiesList *	bpl,
				BorderPropertiesFunction	f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(bpl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vbp= utilPagedListGetItemByNumber( pl, n );

	(*f)( (BorderProperties *)vbp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate border properties to a number.				*/
/*									*/
/************************************************************************/

int docBorderPropertiesNumberImpl( NumberedPropertiesList *		bpl,
				const BorderProperties *		bp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( bpl, make, (void *)bp );
    }

static int docTranslateBorderProperties(
				void *				vbpTo,
				const void *			vbpFrom,
				void *				vcolorMap )
    {
    BorderProperties *		bpTo= (BorderProperties *)vbpTo;
    const BorderProperties *	bpFrom= (const BorderProperties *)vbpFrom;
    const int *			colorMap= (const int *)vcolorMap;

    *bpTo= *bpFrom;
    if  ( bpFrom->bpColor > 0 && colorMap )
	{ bpTo->bpColor= colorMap[bpFrom->bpColor];	}

    return 0;
    }

int docMergeBorderPropertiesLists(
				int **				pBorderMap,
				const int *			colorMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom )
    {
    return docMergeNumberedPropertiesLists( pBorderMap, bplTo, bplFrom,
				    docTranslateBorderProperties,
				    (void *)colorMap );
    }

