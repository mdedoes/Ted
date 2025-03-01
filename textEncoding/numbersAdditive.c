/************************************************************************/
/*									*/
/*  Additive numerals.							*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<string.h>

#   include	"numbersAdditive.h"
#   include	<appDebugon.h>

int numbersAdditiveString(	char *			to,
				int			maxlen,
				int			n,
				const AdditivePiece *	ap,
				int			apCount )
    {
    int			i;

    for ( i= 0; i < apCount; ap++, i++ )
	{
	while( n >= ap->apValue )
	    {
	    if  ( ap->apStrlen > maxlen )
		{ LLDEB(ap->apStrlen,maxlen); return -1;	}

	    strcpy( to, ap->apString );
	    to += ap->apStrlen;

	    maxlen -= ap->apStrlen; n -= ap->apValue;
	    }
	}

    return 0;
    }

