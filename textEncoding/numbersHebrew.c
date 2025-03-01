/************************************************************************/
/*									*/
/*  Hebrew numerals.							*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<string.h>

#   include	"numbersRoman.h"
#   include	"numbersAdditive.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a number to roman notation.					*/
/*									*/
/************************************************************************/

static const AdditivePiece	NUMBERS_HebrewPieces[]=
    {
	{	400,	"\xd7\xaa",	2	}, /* UTF-8 Tav		*/
	{	300,	"\xd7\xa9",	2	}, /* UTF-8 Shin	*/
	{	200,	"\xd7\xa8",	2	}, /* UTF-8 Resh	*/
	{	100,	"\xd7\xa7",	2	}, /* UTF-8 Kuf		*/

	{	 90,	"\xd7\xa6",	2	}, /* UTF-8 Tsadi	*/
	{	 80,	"\xd7\xa6",	2	}, /* UTF-8 Pei	*/
	{	 70,	"\xd7\xa4",	2	}, /* UTF-8 Ayin	*/
	{	 60,	"\xd7\xa1",	2	}, /* UTF-8 Samech	*/
	{	 50,	"\xd7\xa0",	2	}, /* UTF-8 Nun		*/
	{	 40,	"\xd7\x9e",	2	}, /* UTF-8 Mem		*/
	{	 30,	"\xd7\x9c",	2	}, /* UTF-8 Lamed	*/
	{	 20,	"\xd7\x9b",	2	}, /* UTF-8 Kuf		*/
	{	 10,	"\xd7\x99",	2	}, /* UTF-8 Yud		*/

	{	  9,	"\xd7\x98",	2	}, /* UTF-8 Tet		*/
	{	  8,	"\xd7\x97",	2	}, /* UTF-8 Het		*/
	{	  7,	"\xd7\x96",	2	}, /* UTF-8 Zayin	*/
	{	  6,	"\xd7\x95",	2	}, /* UTF-8 Vav		*/
	{	  5,	"\xd7\x94",	2	}, /* UTF-8 Hei		*/
	{	  4,	"\xd7\x93",	2	}, /* UTF-8 Dalet	*/
	{	  3,	"\xd7\x92",	2	}, /* UTF-8 Gimel	*/
	{	  2,	"\xd7\x91",	2	}, /* UTF-8 Bet		*/
	{	  1,	"\xd7\x90",	2	}, /* UTF-8 Aleph	*/
    };

int numbersHebrewString(	char *		to,
				int		maxlen,
				int		n )
    {
    int		res;
    int		l;

    if  ( n <= 0 || n > 999 )
	{ LDEB(n); return -1;	}

    if  ( n == 15 && maxlen >= 6 )
	{
	strcpy( to, "\xd7\x98\xd7\xb4\xd7\x95" );
	return 0;
	}

    if  ( n == 16 && maxlen >= 6 )
	{
	strcpy( to, "\xd7\x98\xd7\xb4\xd7\x96" );
	return 0;
	}

    res= numbersAdditiveString( to, maxlen- 1, n,
		    NUMBERS_HebrewPieces,
		    sizeof(NUMBERS_HebrewPieces)/sizeof(AdditivePiece) );

    if  ( res < 0 )
	{ LDEB(res); return res;	}

    l= strlen( to );
    if  ( l > 2 && maxlen >= l+ 2 )
	{
	to[l+ 2]= to[l]; /* null byte */

	to[l+ 1]= to[l-1];
	to[l+ 0]= to[l-2];

	memcpy( to+ l- 2, "\xd7\xb4", 2 );
	}

    return 0;
    }

