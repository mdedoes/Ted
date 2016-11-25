/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docParaProperties.h"
#   include	"docParaPropertyAdmin.h"
#   include	"docDocumentAttributeMap.h"
#   include	<utilNumberedPropertiesAdmin.h>
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void docInitParagraphPropertyList(	NumberedPropertiesList *	ppl )
    {
    int			num;
    ParagraphProperties	pp;

    utilInitNumberedPropertiesList( ppl );

    utilStartNumberedPropertyList( ppl,

		    PPprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetParaProperty,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)docParagraphPropertiesNumber,

		    sizeof(ParagraphProperties),
		    (InitPagedListItem)docInitParagraphProperties,
		    (CleanPagedListItem)0 );

    docInitParagraphProperties( &pp );

    num= docParagraphPropertiesNumber( ppl, &pp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

const ParagraphProperties * docGetParagraphPropertiesByNumber(
					const NumberedPropertiesList *	ppl,
					int				n )
    {
    void *	vpp= utilPagedListGetItemByNumber( &(ppl->nplPagedList), n );

    if  ( ! vpp )
	{
	static ParagraphProperties	defCp;

	LXDEB(n,vpp);
	docInitParagraphProperties( &defCp );
	return &defCp;
	}

    return (ParagraphProperties *)vpp;
    }

/************************************************************************/
/*									*/
/*  Call a function for all ParagraphProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllParagraphProperties(	const NumberedPropertiesList *	ppl,
				ParagraphPropertiesFunction	f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(ppl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vpp= utilPagedListGetItemByNumber( pl, n );

	(*f)( (ParagraphProperties *)vpp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate border properties to a number.				*/
/*									*/
/************************************************************************/

int docParagraphPropertiesNumber(	NumberedPropertiesList *		ppl,
				const ParagraphProperties *		pp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( ppl, make, (void *)pp );
    }

static int docTranslateParagraphProperties(
				void *				vppTo,
				const void *			vppFrom,
				void *				vdam )
    {
    ParagraphProperties *	ppTo= (ParagraphProperties *)vppTo;
    const ParagraphProperties *	ppFrom= (const ParagraphProperties *)vppFrom;
    const DocumentAttributeMap * dam= (const DocumentAttributeMap *)vdam;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0, ppTo, &ppSetMask,
							    ppFrom, dam ) )
	{ LDEB(1); return -1;	}


    return 0;
    }

int docMergeParagraphPropertiesLists(
				int **				pParagraphMap,
				const int *			listStyleMap,
				const int *			rulerMap,
				const int *			borderMap,
				const int *			shadingMap,
				const int *			frameMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom )
    {
    DocumentAttributeMap	dam;

    docInitDocumentAttributeMap( &dam );

    dam.damListStyleMap= (int *)listStyleMap;
    dam.damRulerMap= (int *)rulerMap;
    dam.damBorderMap= (int *)borderMap;
    dam.damShadingMap= (int *)shadingMap;
    dam.damFrameMap= (int *)frameMap;

    return docMergeNumberedPropertiesLists( pParagraphMap, bplTo, bplFrom,
				    docTranslateParagraphProperties, &dam );
    }

