/************************************************************************/
/*									*/
/*  Roman numerals.							*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"numbersRoman.h"
#   include	"numbersAdditive.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a number to roman notation.					*/
/*									*/
/************************************************************************/

static const AdditivePiece	NUMBERS_RomanPieces[]=
    {
	{	1000,	"m",	1	},
	{	900,	"cm",	2	},
	{	500,	"d",	1	},
	{	400,	"cd",	2	},
	{	100,	"c",	1	},
	{	 90,	"xc",	2	},
	{	 50,	"l",	1	},
	{	 40,	"xl",	2	},
	{	 10,	"x",	1	},
	{	 9,	"ix",	2	},
	{	 5,	"v",	1	},
	{	 4,	"iv",	2	},
	{	 1,	"i",	1	}
    };

int numbersRomanString(	char *		to,
			int		maxlen,
			int		n,
			int		upper )
    {
    int		res;

    if  ( n <= 0 || n > 3500 )
	{ LDEB(n); return -1;	}

    res= numbersAdditiveString( to, maxlen, n,
			NUMBERS_RomanPieces,
			sizeof(NUMBERS_RomanPieces)/sizeof(AdditivePiece) );

    if  ( res < 0 )
	{ LDEB(res); return res;	}

    if  ( upper )
	{
	while( *to )
	    { *to= toupper( *to ); to++;	}
	}

    return 0;
    }

