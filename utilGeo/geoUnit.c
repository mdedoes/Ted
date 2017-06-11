/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   include	"utilGeoConfig.h"

#   include	<string.h>

#   include	"geoUnit.h"
#   include	<appDebugon.h>

int geoUnitTypeInt(	const char *	unitTypeString )
    {
    if  ( ! strcmp( unitTypeString, "cm" ) )
	{ return UNITtyCM;	}

    if  ( ! strcmp( unitTypeString, "inch" )	||
	  ! strcmp( unitTypeString, "\"" )	)
	{ return UNITtyINCH;	}

    if  ( ! strcmp( unitTypeString, "points" )	||
	  ! strcmp( unitTypeString, "pt" )	)
	{ return UNITtyPOINTS;	}

    if  ( ! strcmp( unitTypeString, "picas" )	||
	  ! strcmp( unitTypeString, "pi" )	)
	{ return UNITtyPICAS;	}

    if  ( ! strcmp( unitTypeString, "mm" ) )
	{ return UNITtyMM;	}

    return -1;
    }

const char * geoUnitTypeString(	int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return "cm";
	case UNITtyINCH:	return "\"";
	case UNITtyPOINTS:	return "pt";
	case UNITtyPICAS:	return "pi";
	case UNITtyMM:		return "mm";
	default:
	    LDEB(unitTypeInt); return "?";
	}
    }

double geoUnitFromTwips(	int	twips,
				int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return ( 2.54* twips )/ ( 20.0* 72 );
	case UNITtyINCH:	return twips/ ( 20.0* 72 );
	case UNITtyPOINTS:	return twips/ 20.0;
	case UNITtyPICAS:	return twips/ ( 20.0* 12 );
	case UNITtyMM:		return ( 25.4* twips )/ ( 20.0* 72 );
	default:
	    LDEB(unitTypeInt); return -1;
	}
    }

double geoUnitToTwips(	double	units,
			int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return ( 20.0* 72* units )/ 2.54;
	case UNITtyINCH:	return 20.0* 72* units;
	case UNITtyPOINTS:	return 20.0* units;
	case UNITtyPICAS:	return 20.0* 12* units;
	case UNITtyMM:		return ( 20.0* 72* units )/ 25.4;
	default:
	    LDEB(unitTypeInt); return -1;
	}
    }
