/************************************************************************/
/*									*/
/*  The user properties in a document as they are used to provide data	*/
/*  for DOCPROPERTY fields.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docUserProperties.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise/Clean User Properties.					*/
/*									*/
/************************************************************************/

void docInitUserProperties(	UserProperties *	up )
    {
    utilInitMemoryBuffer( &(up->upName) );
    utilInitMemoryBuffer( &(up->upStaticValue) );
    utilInitMemoryBuffer( &(up->upLinkValue) );
    up->upType= 0; /* an illegal value */
    }

void docCleanUserProperties(	UserProperties *	up )
    {
    utilCleanMemoryBuffer( &(up->upName) );
    utilCleanMemoryBuffer( &(up->upStaticValue) );
    utilCleanMemoryBuffer( &(up->upLinkValue) );
    }

int docUpdateUserProperties(	PropertyMask *		pUpDoneMask,
				UserProperties *	upTo,
				const PropertyMask *	upSetMask,
				const UserProperties *	upSet )
    {
    PropertyMask		upDoneMask;

    utilPropMaskClear( &upDoneMask );

    if  ( PROPmaskISSET( upSetMask, USERPpropNAME ) )
	{
	if  ( ! utilEqualMemoryBuffer( &(upTo->upName), &(upSet->upName) ) )
	    {
	    if  ( utilCopyMemoryBuffer( &(upTo->upName), &(upSet->upName) ) )
		{ LDEB(1); return -1;	}
	    PROPmaskADD( &upDoneMask, USERPpropNAME );
	    }
	}

    if  ( PROPmaskISSET( upSetMask, USERPpropSTATICVAL ) )
	{
	if  ( ! utilEqualMemoryBuffer( &(upTo->upStaticValue),
						&(upSet->upStaticValue) ) )
	    {
	    if  ( utilCopyMemoryBuffer( &(upTo->upStaticValue),
						&(upSet->upStaticValue) ) )
		{ LDEB(1); return -1;	}
	    PROPmaskADD( &upDoneMask, USERPpropSTATICVAL );
	    }
	}

    if  ( PROPmaskISSET( upSetMask, USERPpropLINKVAL ) )
	{
	if  ( ! utilEqualMemoryBuffer( &(upTo->upLinkValue),
						&(upSet->upLinkValue) ) )
	    {
	    if  ( utilCopyMemoryBuffer( &(upTo->upLinkValue),
						&(upSet->upLinkValue) ) )
		{ LDEB(1); return -1;	}
	    PROPmaskADD( &upDoneMask, USERPpropLINKVAL );
	    }
	}

    if  ( PROPmaskISSET( upSetMask, USERPpropTYPE ) )
	{
	if  ( upTo->upType != upSet->upType )
	    {
	    upTo->upType= upSet->upType;
	    PROPmaskADD( &upDoneMask, USERPpropTYPE );
	    }
	}

    *pUpDoneMask= upDoneMask;
    return 0;
    }

int docCopyUserProperties(	UserProperties *		upTo,
				const UserProperties *		upFrom )
    {
    if  ( utilCopyMemoryBuffer( &(upTo->upName), &(upFrom->upName) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(upTo->upStaticValue),
						&(upFrom->upStaticValue) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(upTo->upLinkValue), &(upFrom->upLinkValue) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set a user property.						*/
/*									*/
/************************************************************************/

int docSetUserProperty(		UserProperties *	up,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case USERPpropTYPE:
	    up->upType= arg;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Get a user property.						*/
/*									*/
/************************************************************************/

int docGetUserProperty(		const UserProperties *	up,
				int			prop )
    {
    switch( prop )
	{
	case USERPpropTYPE:
	    return up->upType;

	default:
	    LDEB(prop); return -1;
	}
    }

