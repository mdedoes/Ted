#   include	"bitmapConfig.h"

#   include	<string.h>

#   include	"bmintern.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a buffer in chunky format to native integers.		*/
/*									*/
/************************************************************************/

int bmToNativeARGB32(	unsigned char *			target,
			unsigned char *			source,
			const BitmapDescription *	bd,
			int				stride )
    {
    int		row;
    BmUint32	native;

    if  ( bd->bdColorEncoding != BMcoRGB )
	{ LDEB(bd->bdColorEncoding); return -1;	}
    if  ( bd->bdBitsPerSample != 8 )
	{ LDEB(bd->bdBitsPerSample); return -1;	}
    if  ( sizeof(native) != 4 )
	{ LDEB(sizeof(native)); return -1;	}

    if  ( bd->bdHasAlpha )
	{
	if  ( bd->bdSamplesPerPixel != 4 )
	    { LDEB(bd->bdSamplesPerPixel); return -1;	}

	for ( row= bd->bdPixelsHigh-1; row >= 0; row-- )
	    {
	    const unsigned char *	fr;
	    unsigned char *		to;
	    int				col= bd->bdPixelsWide- 1;

	    to= target+ row* stride+ 4* col;
	    fr= source+ row* bd->bdBytesPerRow+ 4* col;

	    for ( col= bd->bdPixelsWide- 1; col >= 0; fr -= 4, to -= 4, col-- )
		{
		native=   fr[3] << 24;
		native += fr[0] << 16;
		native += fr[1] <<  8;
		native += fr[2] <<  0;

		memcpy( to, &native, 4 );
		}
	    }
	}
    else{
	if  ( bd->bdSamplesPerPixel != 3 )
	    { LDEB(bd->bdSamplesPerPixel); return -1;	}

	for ( row= bd->bdPixelsHigh-1; row >= 0; row-- )
	    {
	    const unsigned char *	fr;
	    unsigned char *		to;
	    int				col= bd->bdPixelsWide- 1;

	    to= target+ row* stride+ 4* col;
	    fr= source+ row* bd->bdBytesPerRow+ 3* col;

	    for ( col= bd->bdPixelsWide- 1; col >= 0; fr -= 3, to -= 4, col-- )
		{
		native=   255   << 24;
		native += fr[0] << 16;
		native += fr[1] <<  8;
		native += fr[2] <<  0;

		memcpy( to, &native, 4 );
		}
	    }
	}

    return 0;
    }

int bmToNativeA1(	unsigned char *			target,
			unsigned char *			source,
			const BitmapDescription *	bd,
			int				stride )
    {
    int		row;
    BmUint32	native;

    int		past;
    int		count;
    int		extra= 0;

    if  ( bd->bdColorEncoding != BMcoBLACKWHITE )
	{ LDEB(bd->bdColorEncoding); return -1;	}
    if  ( bd->bdBitsPerSample != 1 )
	{ LDEB(bd->bdBitsPerSample); return -1;	}
    if  ( bd->bdBitsPerPixel != 1 )
	{ LDEB(bd->bdBitsPerPixel); return -1;	}
    if  ( sizeof(native) != 4 )
	{ LDEB(sizeof(native)); return -1;	}

    past= count= ( ( bd->bdPixelsWide+ 31 )/ 32 );
    extra= bd->bdBytesPerRow % 4;
    if  ( extra > 0 )
	{ count--;	}

    for ( row= bd->bdPixelsHigh-1; row >= 0; row-- )
	{
	const unsigned char *	fr;
	unsigned char *		to;
	int			l;

	fr= source+ row* bd->bdBytesPerRow+ 4* ( past- 1 );
	to= target+ row* stride+ 4* ( past- 1 );

	if  ( extra > 0 )
	    {
	    native= 0;

	    if  ( extra > 0 )
		{ native += fr[0] << 24;	}
	    if  ( extra > 1 )
		{ native += fr[1] << 16;	}
	    if  ( extra > 2 )
		{ native += fr[2] <<  8;	}

	    memcpy( to, &native, 4 );

	    fr -= 4; to -= 4;
	    }

	for ( l= count- 1; l >= 0; fr -= 4, to -= 4, l-- )
	    {
	    native=   fr[0] << 24;
	    native += fr[1] << 16;
	    native += fr[2] <<  8;
	    native += fr[3] <<  0;

	    memcpy( to, &native, 4 );
	    }
	}

    return 0;
    }
