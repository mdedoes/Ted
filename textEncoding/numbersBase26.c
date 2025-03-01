/************************************************************************/
/*									*/
/*  Base 26 numbers. Like in Excel column headers.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	"numbersBase26.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a number to base 26. Like in Excel column headers.		*/
/*  1->A; 26->Z; 27->AA; 28->AB &c.					*/
/*									*/
/*  1)  Produce letters in the opposite order.				*/
/*  2)  Terminate.							*/
/*  3)  Swap.								*/
/*									*/
/************************************************************************/

static const char UTIL_Base26LowerDigits[]= "abcdefghijklmnopqrstuvwxyz";
static const char UTIL_Base26UpperDigits[]= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int numbersBase26String(	char *		target,
				int		maxlen,
				int		n,
				int		upper )
    {
    const char *	d;
    int			done= 0;
    int			i;

    if  ( n <= 0 )
	{ LDEB(n); return -1;	}

    if  ( upper )
	{ d= UTIL_Base26UpperDigits;	}
    else{ d= UTIL_Base26LowerDigits;	}

    n--;

    /*  1  */
    while( n >= 0 )
	{
	if  ( maxlen == 0 )
	    { LLDEB(n,maxlen); return -1;	}

	target[done++]= d[n % 26];
	maxlen--;

	n /= 26; n--;
	}

    /*  2  */
    target[done]= '\0';

    /*  3  */
    for ( i= 0; i < done/ 2; i++ )
	{
	char	s;

	s= target[i]; target[i]= target[done-i-1]; target[done-i-1]= s;
	}

    return 0;
    }

int numbersBase26Int(	const char *	from,
			int		size )
    {
    int			rval= 0;
    const char *	dig;

    while( isspace( *from ) && size > 0 )
	{ from++; size--;	}

    if  ( size == 0 )
	{ SDEB(from); return -1;	}

    while( size > 0 )
	{
	if  ( isupper( *from ) )
	    {
	    dig= strchr( UTIL_Base26UpperDigits, *from );
	    if  ( ! dig )
		{ SDEB(from); return -1;	}

	    rval= 26* rval+ ( dig- UTIL_Base26UpperDigits );
	    }
	else{
	    dig= strchr( UTIL_Base26LowerDigits, *from );
	    if  ( ! dig )
		{ SDEB(from); return -1;	}

	    rval= 26* rval+ ( dig- UTIL_Base26LowerDigits );
	    }

	rval++;
	from++; size--;
	}

    return rval;
    }

int numbersBase26Cell(	int *		pX,
			int *		pY,
			const char *	from,
			int		size )
    {
    int		pos= 0;
    int		x;
    int		y;

    char *	e;

    while( pos < size && isalpha( from[pos] ) )
	{ pos++;	}
    x= numbersBase26Int( from, pos );
    if  ( x < 1 )
	{ SLDEB(from,x); return -1;	}

    y= strtol( from+ pos, &e, 10 );
    if  ( e == from+ pos )
	{ SSDEB(from,from+pos); return -1;	}
    if  ( y < 1 )
	{ SLDEB(from,y); return -1;	}
    pos= e- from;
    if  ( pos != size )
	{ LLDEB(pos,size); return -1;	}

    *pX= x- 1; *pY= y- 1; return 0;
    }


int numbersBase26Rectangle(	int *		pX0,
				int *		pX1,
				int *		pY0,
				int *		pY1,
				const char *	from,
				int		size )
    {
    int		pos= 0;
    int		x0= 0;
    int		x1= 0;
    int		y0= 0;
    int		y1= 0;

    char *	e;

    if  ( isalpha( from[pos] ) )
	{
	int	len= 0;

	while( pos+ len < size && isalpha( from[pos+ len] ) )
	    { len++;	}
	x0= numbersBase26Int( from, len );
	if  ( x0 < 1 )
	    { SLDEB(from,x0); return -1;	}
	pos += len;

	x1= x0;
	}

    if  ( pos < size && isdigit( from[pos] ) )
	{
	y0= strtol( from+ pos, &e, 10 );
	if  ( e == from+ pos )
	    { SSDEB(from,from+pos); return -1;	}
	if  ( y0 < 1 )
	    { SLDEB(from,y0); return -1;	}
	pos= e- from;

	y1= y0;
	}

    if  ( pos == 0 )
	{ LLDEB(pos,size); return -1;	}

    if  ( pos < size && from[pos] == ':' )
	{
	pos++;

	if  ( isalpha( from[pos] ) )
	    {
	    int	len= 0;

	    while( pos+ len < size && isalpha( from[pos+ len] ) )
		{ len++; }
	    x1= numbersBase26Int( from+ pos, len );
	    if  ( x1 < 1 )
		{ SLDEB(from,x1); return -1;	}
	    pos += len;
	    }

	if  ( pos < size && isdigit( from[pos] ) )
	    {
	    y1= strtol( from+ pos, &e, 10 );
	    if  ( e == from+ pos )
		{ SSDEB(from,from+pos); return -1;	}
	    if  ( y1 < 1 )
		{ SLDEB(from,y1); return -1;	}
	    pos= e- from;
	    }
	}

    if  ( pos != size )
	{ LLDEB(pos,size); return -1;	}

    *pX0= x0- 1; *pX1= x1- 1;
    *pY0= y0- 1; *pY1= y1- 1;

    return 0;
    }

