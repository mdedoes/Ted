#   include	"bitmapConfig.h"

#   include	"bmFlipBits.h"

#   include	<appDebugon.h>

unsigned char BM_FLIPPED_BITS[256];

void bmInitFlipBits( void )
    {
    if  ( ! BM_FLIPPED_BITS[1] )
	{
	int	i;

	for ( i= 0; i < 256; i++ )
	    {
	    BM_FLIPPED_BITS[i]=	( ( i & 0x80 ) >> 7 )   |
				( ( i & 0x40 ) >> 5 )   |
				( ( i & 0x20 ) >> 3 )   |
				( ( i & 0x10 ) >> 1 )   |
				( ( i & 0x08 ) << 1 )   |
				( ( i & 0x04 ) << 3 )   |
				( ( i & 0x02 ) << 5 )   |
				( ( i & 0x01 ) << 7 )   ;
	    }
	}
    }

void bmFlipBits(	unsigned char *		bytes,
			int			n )
    {
    bmInitFlipBits();

    while( n > 0 )
	{
	*bytes= BM_FLIPPED_BITS[*bytes];
	bytes++; n--;
	}
    }

