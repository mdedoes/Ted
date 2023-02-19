/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	"textAttributeAdmin.h"
#   include	"textAttribute.h"
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void textInitTextAttributeList(	NumberedPropertiesList *	tal )
    {
    int			num;
    TextAttribute	ta;

    utilInitNumberedPropertiesList( tal );

    utilStartNumberedPropertyList( tal,

		    TAprop_COUNT,
		    (NumberedPropertiesGetProperty)textGetTextProperty,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)textTextAttributeNumber,

		    sizeof(TextAttribute),
		    (InitPagedListItem)textInitTextAttribute,
		    (CleanPagedListItem)0 );

    textInitTextAttribute( &ta );
    ta.taFontNumber= 0;

    num= textTextAttributeNumber( tal, &ta );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute number to a struct value.		*/
/*									*/
/************************************************************************/

const TextAttribute * textGetTextAttributeByNumber(
					const NumberedPropertiesList *	tal,
					int				n )
    {
    void *	vta= utilPagedListGetItemByNumber( &(tal->nplPagedList), n );

    if  ( ! vta )
	{
	static TextAttribute	defTa;

	LXDEB(n,vta);
	textInitTextAttribute( &defTa );
	return &defTa;
	}

    return (TextAttribute *)vta;
    }

/************************************************************************/
/*									*/
/*  Call a function for all TextAttributes in the list.			*/
/*									*/
/************************************************************************/

int textForAllAttributesInList(
				const NumberedPropertiesList *	tal,
				const IndexSet *		filter,
				TextAttributeFunction		f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(tal->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vta;

	if  ( ! utilIndexSetContains( &(pl->plItemUsed), n ) )
	    { continue;	}
	if  ( filter && ! utilIndexSetContains( filter, n ) )
	    { continue;	}

	vta= utilPagedListGetItemByNumber( &(tal->nplPagedList), n );

	if  ( (*f)( n, (TextAttribute *)vta, through ) )
	    { LDEB(n); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int textTextAttributeNumber(	NumberedPropertiesList *	tal,
				const TextAttribute *		ta )
    {
    const int	make= 1;

    return utilGetPropertyNumber( tal, make, (void *)ta );
    }
