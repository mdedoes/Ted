/************************************************************************/
/*									*/
/*  Translation between unicode values and utf-8.			*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>
#   include	<string.h>

#   include	"uniUtf8.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  UTF-8 Calculations.							*/
/*									*/
/************************************************************************/

# define UTF8_1(a)		(a)

# define UTF8_2(a,b)		(       (64)*((a)&0x1f)+ \
				             ((b)&0x3f) )

# define UTF8_3(a,b,c)		(    (64*64)*((a)&0x0f)+ \
				        (64)*((b)&0x3f)+ \
				             ((c)&0x3f) )

# define UTF8_4(a,b,c,d)	( (64*64*64)*((a)&0x07)+ \
				     (64*64)*((b)&0x3f)+ \
				        (64)*((c)&0x3f)+ \
				             ((d)&0x3f) )

/************************************************************************/
/*									*/
/*  Retrieve one character from a string.				*/
/*									*/
/************************************************************************/

int uni_GetUtf8(	unsigned short *	pSymbol,
			const unsigned char *	buf )
    {
    if  ( ! buf[0] )
	{ return 0;	}

    if  ( buf[0] < 128 )
	{ *pSymbol= UTF8_1(buf[0]); return 1; }

    if  ( ( buf[1] & 0xc0 ) != 0x80 )
	{ XXDEB(buf[0],buf[1]); return 0;	}
    if  ( buf[0] < 224 )
	{ *pSymbol= UTF8_2(buf[0],buf[1]); return 2; }

    if  ( ( buf[2] & 0xc0 ) != 0x80 )
	{ XXDEB(buf[0],buf[2]); return 0;	}
    if  ( buf[0] < 240 )
	{ *pSymbol= UTF8_3(buf[0],buf[1],buf[2]); return 3; }

    if  ( ( buf[3] & 0xc0 ) != 0x80 )
	{ XXDEB(buf[0],buf[3]); return 0;	}
    /* Does not fit in a 16 bit integer!
    *pSymbol= UTF8_4(buf[0],buf[1],buf[2],buf[3]); return 4;
    */
    LLDEB(buf[3],UTF8_4(buf[0],buf[1],buf[2],buf[3]));
    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit one character to a string.					*/
/*									*/
/************************************************************************/

int uni_PutUtf8(	unsigned char *		buf,
			int			symbol )
    {
    if  ( symbol < 0 )
	{ LDEB(symbol); return 0;	}
    if  ( symbol < 1 )
	{ return 0;	}

    if  ( symbol < 128 )
	{
	*(buf++)= symbol;
	return 1;
	}

    if  ( symbol < 2048 )
	{
	*(buf++)= 0xc0 | ( ( symbol >>  6 )        );
	*(buf++)= 0x80 | ( ( symbol       ) & 0x3f );
	return 2;
	}

    if  ( symbol < 65536 )
	{
	*(buf++)= 0xe0 | ( ( symbol >> 12 )        );
	*(buf++)= 0x80 | ( ( symbol >>  6 ) & 0x3f );
	*(buf++)= 0x80 | ( ( symbol       ) & 0x3f );
	return 3;
	}

    if  ( symbol < 2097152 )
	{
	*(buf++)= 0xf0 | ( ( symbol >> 18 ) & 0x07 );
	*(buf++)= 0x80 | ( ( symbol >> 12 ) & 0x3f );
	*(buf++)= 0x80 | ( ( symbol >>  6 ) & 0x3f );
	*(buf++)= 0x80 | ( ( symbol       ) & 0x3f );
	return 4;
	}

    LDEB(symbol); return 0;
    }

/************************************************************************/
/*									*/
/*  Translate an utf8 string to 16 bit unicode values.			*/
/*									*/
/************************************************************************/

unsigned short * uniUtf8ToUnicodes(	int *			pUlen,
					const unsigned char *	word )
    {
    int			l= strlen( (char *)word );
    unsigned short *	ucods= (unsigned short *)malloc( ( l+ 1 )* sizeof(unsigned short) );
    int			fr;
    int			ulen;

    if  ( ! ucods )
	{ LXDEB(l,ucods); return (unsigned short *)0;	}

    fr= 0; ulen= 0;
    while( fr < l )
	{
	int	step= uniGetUtf8( ucods+ ulen, word+ fr );

	if  ( step < 1 )
	    { break;	}

	ulen++; fr += step;
	}

    *pUlen= ulen;
    return ucods;
    }

/************************************************************************/
/*									*/
/*  Translate string of 16 bit unicode values to UTF-8.			*/
/*									*/
/************************************************************************/

unsigned char * uniUnicodesToUtf8(	int *			pMBlen,
					const unsigned short *	codes )
    {
    int			ulen;
    unsigned char *	utf8;
    int			fr;
    int			mblen;

    ulen= 0;
    while( codes[ulen] )
	{ ulen++;	}

    utf8= (unsigned char *)malloc( 4* ulen+ 1 );
    if  ( ! utf8 )
	{ LXDEB(ulen,utf8); return (unsigned char *)0;	}

    fr= 0; mblen= 0;
    while( fr < ulen )
	{
	int	step= uniPutUtf8( utf8+ mblen, codes[fr] );

	if  ( step < 1 )
	    { break;	}

	fr++; mblen += step;
	}

    *pMBlen= mblen;
    return utf8;
    }

