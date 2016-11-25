#   include "ucdIntern.h"

int ucdInClass(		const int					sym,
			const unsigned char * const * const * const	ppp )
    {
    int					s= sym;
    const unsigned char * const *	pp;
    const unsigned char * 		p;

    if  ( sym < 0 || sym >= 256* 256 )
	{ return 0;	}

    pp= ppp[s/( 16* 256 )];
    if  ( ! pp )
	{ return 0;	}
    s= s % ( 16* 256 );

    p= pp[s/ 256];
    if  ( ! p )
	{ return 0;	}
    s= s% 256;

    return ( p[s/8] & ( 1 << ( s % 8 ) ) ) != 0;
    }
