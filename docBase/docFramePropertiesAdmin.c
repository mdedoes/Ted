/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docFramePropertiesAdmin.h"
#   include	"docFrameProperties.h"
#   include	<utilNumberedPropertiesAdmin.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void docInitFramePropertyList(	NumberedPropertiesList *	fpl )
    {
    int			num;
    FrameProperties	fp;

    utilInitNumberedPropertiesList( fpl );

    utilStartNumberedPropertyList( fpl,

		    TFPprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetFrameProperty,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)docFramePropertiesNumberImpl,

		    sizeof(FrameProperties),
		    (InitPagedListItem)docInitFrameProperties,
		    (CleanPagedListItem)0 );

    docInitFrameProperties( &fp );

    num= docFramePropertiesNumberImpl( fpl, &fp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a frame properties number to a struct value.		*/
/*									*/
/************************************************************************/

const FrameProperties * docGetFramePropertiesByNumberImpl(
					const NumberedPropertiesList *	fpl,
					int				n )
    {
    void *	vfp= utilPagedListGetItemByNumber( &(fpl->nplPagedList), n );

    if  ( ! vfp )
	{
	static FrameProperties	fpDef;

	LXDEB(n,vfp);
	docInitFrameProperties( &fpDef );
	return &fpDef;
	}

    return (FrameProperties *)vfp;
    }

/************************************************************************/
/*									*/
/*  Is this number really a border?					*/
/*									*/
/************************************************************************/

int docFrameNumberIsFrameImpl(		const NumberedPropertiesList *	fpl,
					int				n )
    {
    const FrameProperties *	fp;

    if  ( n < 0 )
	{ return 0;	}

    fp= docGetFramePropertiesByNumberImpl( fpl, n );

    return DOCisFRAME( fp );
    }

/************************************************************************/
/*									*/
/*  Call a function for all FrameProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllFrameProperties(	const NumberedPropertiesList *	fpl,
				FramePropertiesFunction		f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < fpl->nplPagedList.plItemCount; n++ )
	{
	void *      vbp= utilPagedListGetItemByNumber(
						&(fpl->nplPagedList), n );

	(*f)( (FrameProperties *)vbp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int docFramePropertiesNumberImpl(	NumberedPropertiesList *	fpl,
					const FrameProperties *		fp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( fpl, make, (void *)fp );
    }

static int docTranslateFrameProperties(
				void *				vfpTo,
				const void *			vfpFrom,
				void *				through )
    {
    FrameProperties *		fpTo= (FrameProperties *)vfpTo;
    const FrameProperties *	fpFrom= (const FrameProperties *)vfpFrom;

    *fpTo= *fpFrom;

    return 0;
    }

int docMergeFramePropertyLists(	int **				pFrameMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom )
    {
    return docMergeNumberedPropertiesLists( pFrameMap, bplTo, bplFrom,
				    docTranslateFrameProperties, (void *)0 );
    }

