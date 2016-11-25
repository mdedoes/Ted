#   include	"ucdIntern.h"

int ucdToCase(		const int						sym,
			const unsigned short * const * const * const * const	pppp )
    {
    int						s= sym;
    const unsigned short * const * const *	ppp;
    const unsigned short * const *		pp;
    const unsigned short * 			p;

    if  ( sym < 0 || sym >= 256* 256 )
	{ return sym;	}

    ppp= pppp[s/( 16* 16* 16 )];
    if  ( ! ppp )
	{ return sym;	}
    s= s % ( 16* 16 * 16 );

    pp= ppp[s/( 16* 16 )];
    if  ( ! pp )
	{ return sym;	}
    s= s % ( 16* 16 );

    p= pp[s/( 16 )];
    if  ( ! p )
	{ return sym;	}
    s= s % ( 16 );

    return p[s];
    }
