/************************************************************************/
/*									*/
/*  Property mask operations.						*/
/*  Most is done by macros in the header file.				*/
/*									*/
/************************************************************************/

#   include	<string.h>

#   include	"utilPropMask.h"
#   include	<appDebugon.h>

void utilDebugPropMask(	const char *		file,
			int			line,
			const char *		name,
			const PropertyMask *	pm )
    {
    if  ( pm )
	{
	int		p;
	const char *	s= "";

	appDebug( "%s(%d) %s={", file, line, name );

	for ( p= 0; p < PROPmaskMAXPROPS; p++ )
	    {
	    if  ( PROPmaskISSET(pm,p) )
		{
		appDebug( "%s%d", s, p );
		s= ",";
		}
	    }

	appDebug( "}\n" );
	}
    else{
	appDebug( "%s(%d) %s=0x0\n", file, line, name );
	}
    }

void utilPropMaskClear(	PropertyMask *	pm )
    {
    memset( pm->pmBits, 0, PROPmaskSIZE );
    return;
    }

void utilPropMaskFill(		PropertyMask *	pm,
				int		c )
    {
    int			prop;

    for ( prop= 0; prop < c; prop++ )
	{ PROPmaskADD( pm, prop ); }

    return;
    }

int utilPropMaskIsEmpty(	const PropertyMask *	pm )
    {
    int		byte;

    if  ( ! pm )
	{ XDEB(pm); return 1;	}

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{
	if  ( pm->pmBits[byte] )
	    { return 0;	}
	}

    return 1;
    }

void utilPropMaskOr(		PropertyMask *		pm0,
				const PropertyMask *	pm1,
				const PropertyMask *	pm2 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= pm1->pmBits[byte] | pm2->pmBits[byte]; }

    return;
    }

void utilPropMaskAnd(		PropertyMask *		pm0,
				const PropertyMask *	pm1,
				const PropertyMask *	pm2 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= pm1->pmBits[byte] & pm2->pmBits[byte]; }

    return;
    }

void utilPropMaskNot(		PropertyMask *		pm0,
				const PropertyMask *	pm1 )
    {
    int		byte;

    for ( byte= 0; byte < PROPmaskSIZE; byte++ )
	{ pm0->pmBits[byte]= ~pm1->pmBits[byte];	}

    return;
    }

