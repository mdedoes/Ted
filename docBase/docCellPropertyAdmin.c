/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docCellProperties.h"
#   include	"docCellPropertyAdmin.h"
#   include	"docDocumentAttributeMap.h"
#   include	<utilNumberedPropertiesAdmin.h>
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void docInitCellPropertyList(	NumberedPropertiesList *	cpl )
    {
    int			num;
    CellProperties	cp;

    utilInitNumberedPropertiesList( cpl );

    utilStartNumberedPropertyList( cpl,

		    CLprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetCellProperty,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)docCellPropertiesNumber,

		    sizeof(CellProperties),
		    (InitPagedListItem)docInitCellProperties,
		    (CleanPagedListItem)0 );

    docInitCellProperties( &cp );

    num= docCellPropertiesNumber( cpl, &cp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

const CellProperties * docGetCellPropertiesByNumber(
					const NumberedPropertiesList *	cpl,
					int				n )
    {
    void *	vcp= utilPagedListGetItemByNumber( &(cpl->nplPagedList), n );

    if  ( ! vcp )
	{
	static CellProperties	defCp;

	LXDEB(n,vcp);
	docInitCellProperties( &defCp );
	return &defCp;
	}

    return (CellProperties *)vcp;
    }

/************************************************************************/
/*									*/
/*  Call a function for all CellProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllCellProperties(	const NumberedPropertiesList *	cpl,
				CellPropertiesFunction		f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(cpl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vcp= utilPagedListGetItemByNumber( pl, n );

	(*f)( (CellProperties *)vcp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate cell properties to a number.				*/
/*									*/
/************************************************************************/

int docCellPropertiesNumber(	NumberedPropertiesList *	cpl,
				const CellProperties *		cp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( cpl, make, (void *)cp );
    }

static int docTranslateCellProperties(
				void *				vcpTo,
				const void *			vcpFrom,
				void *				vdam )
    {
    CellProperties *		cpTo= (CellProperties *)vcpTo;
    const CellProperties *	cpFrom= (const CellProperties *)vcpFrom;
    const DocumentAttributeMap * dam= (const DocumentAttributeMap *)vdam;

    PropertyMask		cpSetMask;

    utilPropMaskClear( &cpSetMask );
    utilPropMaskFill( &cpSetMask, CLprop_COUNT );

    if  ( docUpdCellProperties( (PropertyMask *)0, cpTo, &cpSetMask,
							    cpFrom, dam ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docMergeCellPropertiesLists(
				int **				pCellMap,
				const int *			borderMap,
				const int *			shadingMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom )
    {
    DocumentAttributeMap	dam;

    docInitDocumentAttributeMap( &dam );

    dam.damBorderMap= (int *)borderMap;
    dam.damShadingMap= (int *)shadingMap;

    return docMergeNumberedPropertiesLists( pCellMap, bplTo, bplFrom,
					docTranslateCellProperties, &dam );
    }

