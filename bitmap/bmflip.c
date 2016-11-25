#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static unsigned char	bmFlipLeft[256];
static unsigned char	bmFlipRight[256];

static void bmFlip1Init( int n )
    {
    int		i;
    int		rshift= n % 8;
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x01 ) << 7	|
				( i & 0x02 ) << 5	|
				( i & 0x04 ) << 3	|
				( i & 0x08 ) << 1	|
				( i & 0x10 ) >> 1	|
				( i & 0x20 ) >> 3	|
				( i & 0x40 ) >> 5	|
				( i & 0x80 ) >> 7	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip2Init( int n )
    {
    int		i;
    int		rshift= 2* ( n % 4 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x03 ) << 6	|
				( i & 0x0c ) << 2	|
				( i & 0x30 ) >> 2	|
				( i & 0xc0 ) >> 6	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip4Init( int n )
    {
    int		i;
    int		rshift= 4* ( n % 2 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x0f ) << 4	|
				( i & 0xf0 ) >> 4	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

/************************************************************************/
/*									*/
/*  Flip the bytes in a scan line.					*/
/*									*/
/************************************************************************/

int bmFlipBytes(		unsigned char *	buffer,
				int		byteCount,
				int		bitsPerPixel )
    {
    switch( bitsPerPixel )
	{
	case  1:
	case  2:
	case  4:
	    bmFlip1Init( 8/bitsPerPixel );
	    break;

	case  8:
	    return 0;

	case 24:
	default:
	    LDEB(bitsPerPixel); return -1;
	}

    while( byteCount-- > 0 )
	{ *buffer= bmFlipRight[*buffer]; buffer++;	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlipBits(	unsigned char *		to,
			const unsigned char *	from,
			int			pixelsWide,
			int			pixelsPerByte )
    {
    int			i;
    int			count;

    count= pixelsWide/ pixelsPerByte;
    to += count;

    if  ( pixelsWide % pixelsPerByte )
	{
	*(to)= bmFlipLeft[*from];
	to--;

	for ( i= count; i; i-- )
	    {
	    *to=       bmFlipRight[*(from++)];
	    *(to--) |= bmFlipLeft [* from   ];
	    }
	}
    else{
	for ( i= count; i; i-- )
	    { *(--to)= bmFlipRight[*(from++)];	}
	}
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 8 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlip8Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += pixelsWide- 1;

    for ( i= pixelsWide; i; i-- )
	{ *(to--)= *(from++); }
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 24 bit pixels			*/
/*									*/
/************************************************************************/

static void bmFlip24Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += 3*pixelsWide- 3;

    for ( i= pixelsWide; i; i-- )
	{
	to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
	to -= 3; from += 3;
	}
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over horizontal axis.			*/
/*									*/
/************************************************************************/

int bmVerticalFlip(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    unsigned char * 		bufOut;

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	memcpy( to, from, bdIn->bdBytesPerRow );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Rotate 180 degrees.				*/
/*									*/
/************************************************************************/

int bmUpsideDown(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    unsigned char * 		bufOut;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over vertical axis.			*/
/*									*/
/************************************************************************/

int bmHorizontalFlip(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    unsigned char * 		bufOut;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ row* bdIn->bdBytesPerRow;
	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Rotate a bitmap by 0,90,180,270 degrees.				*/
/*									*/
/************************************************************************/

int bmRotate90(	BitmapDescription *		bdOut,
		const BitmapDescription *	bdIn,
		unsigned char **		pBufOut,
		const unsigned char *		bufIn,
		int				angle )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row, col;
    unsigned char * 		bufOut;
    BitmapDescription		bd;

    int				s;
    int				shift;
    unsigned char		m= 0;
    unsigned char		mr;
    int				step;

    switch( angle )
	{
	case 0:
	    if  ( bmCopyDescription( &bd, bdIn ) )
		{ LDEB(1); return -1;	}

	    bufOut= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! bufOut )
		{ LLDEB(bd.bdBufferLength,bufOut); return -1;	}

	    memcpy( bufOut, bufIn, bd.bdBufferLength );

	    *bdOut= bd; *pBufOut= bufOut;
	    return 0;
	case 180:
	    return bmUpsideDown( bdOut, bdIn, pBufOut, bufIn, angle );
	case 270:
	case 90:
	    break;
	default:
	    LDEB(angle); return -1;
	}


    bmCopyDescription( &bd, bdIn );
    bd.bdPixelsWide= bdIn->bdPixelsHigh;
    bd.bdPixelsHigh= bdIn->bdPixelsWide;
    bd.bdBytesPerRow= ( bd.bdPixelsWide* bdIn->bdBitsPerPixel+ 7 )/ 8;
    bd.bdBufferLength= bd.bdPixelsHigh* bd.bdBytesPerRow;

    bufOut= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bd.bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  4:
	    m |= 0x30;
	    /*FALLTHROUGH*/
	case  2:
	    m |= 0x40;
	    /*FALLTHROUGH*/
	case  1:
	    m |= 0x80;

	    memset( bufOut, 0, bd.bdBufferLength );
	    step= 8/ bdIn->bdBitsPerPixel;

	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    int		toRow= bd.bdPixelsHigh- row- 1;

		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			to= bufOut+ toRow* bd.bdBytesPerRow;
			from= bufIn+ row/step+ s* bdIn->bdBytesPerRow;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < bd.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) << shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			to= bufOut+ toRow* bd.bdBytesPerRow;
			from= bufIn+ row/step+ s* bdIn->bdBytesPerRow;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < bd.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) >> shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			int ri= bdIn->bdPixelsHigh- 1;

			to= bufOut+ row* bd.bdBytesPerRow;
			from= bufIn+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			ri -= s;
			from += row/ step;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < bd.bdPixelsWide; col += step )
			    {
			    if  ( ri < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) << shift;
			    from -= step* bdIn->bdBytesPerRow;
			    ri -= step;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			int ri= bdIn->bdPixelsHigh- 1;

			to= bufOut+ row* bd.bdBytesPerRow;
			from= bufIn+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			ri -= s;
			from += row/ step;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < bd.bdPixelsWide; col += step )
			    {
			    if  ( ri < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) >> shift;
			    from -= step* bdIn->bdBytesPerRow;
			    ri -= step;
			    }
			}
		    }
		}

	    break;
	case  8:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ row;
		    to= bufOut+ ( bd.bdPixelsHigh- row- 1 )* bd.bdBytesPerRow;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{ *(to++)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ row;
		    to= bufOut+ ( row+ 1 )* bd.bdBytesPerRow- 1;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{ *(to--)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    break;
	case 24:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ 3* row;
		    to= bufOut+ ( bd.bdPixelsHigh- row- 1 )* bd.bdBytesPerRow;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{
			*(to++)= from[0]; *(to++)= from[1]; *(to++)= from[2];
			from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    else{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ 3* row;
		    to= bufOut+ ( row+ 1 )* bd.bdBytesPerRow- 3;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{
			to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
			to -= 3; from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    *bdOut= bd; *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Rotate a 1, 2, 4 bit scan line over a sharp angle.			*/
/*									*/
/************************************************************************/

static void bmRotateScanline124( const unsigned char *	from,
				int			pixelsWide,
				unsigned char *		bufOut,
				int			bytesPerRow,
				int			trow,
				int			tcol,
				int			eRow,
				int			eCol,
				int			xRow,
				int			xCol,
				int			sRow,
				int			d,
				int			bitsPerPixel )
    {
    int			fcol;
    int			step= 8/ bitsPerPixel;
    unsigned char	tmask;
    int			rshift= ( tcol % step ) * bitsPerPixel;

    unsigned char *	to= bufOut+ trow* bytesPerRow+ tcol/step;

    tmask= 0xff << ( 8- bitsPerPixel );
    tmask= tmask >> ( ( tcol % step )* bitsPerPixel );

    for ( fcol= 0; fcol < pixelsWide; fcol += step )
	{
	int		s;
	unsigned char	byteval= *(from++);

	for ( s= 0; s < step; s++ )
	    {
	    while( eCol >= 0 )
		{
		*to &= ~tmask; *to |=  tmask & ( byteval >> rshift );

		tcol++; eCol -= d;
		tmask= ( 257 * tmask ) >> bitsPerPixel;
		rshift= ( rshift+ bitsPerPixel )% 8;
		if  ( tcol % step == 0 )
		    { to++;	}
		}
	    eCol += xCol;
	    while( eRow >= 0 )
		{
		*to &= ~tmask; *to |=  tmask & ( byteval >> rshift );

		trow += sRow; eRow -= d;
		to += sRow* bytesPerRow;
		}
	    eRow += xRow;

	    byteval= byteval << bitsPerPixel;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Rotate an 8 bit scan line over a sharp angle.			*/
/*									*/
/************************************************************************/

static void bmRotateScanline8(	const unsigned char *	from,
				int			pixelsWide,
				unsigned char *		bufOut,
				int			bytesPerRow,
				int			trow,
				int			tcol,
				int			eRow,
				int			eCol,
				int			xRow,
				int			xCol,
				int			sRow,
				int			d		)
    {
    int		fcol;

    for ( fcol= 0; fcol < pixelsWide; fcol++ )
	{
	int		pixval= *(from++);

	while( eCol >= 0 )
	    {
	    bufOut[ trow* bytesPerRow+ tcol ]= pixval;
	    tcol++; eCol -= d;
	    }
	eCol += xCol;
	while( eRow >= 0 )
	    {
	    bufOut[ trow* bytesPerRow+ tcol ]= pixval;
	    trow += sRow; eRow -= d;
	    }
	eRow += xRow;
	}
    }

/************************************************************************/
/*									*/
/*  Rotate a 24 bit scan line over a sharp angle.			*/
/*									*/
/************************************************************************/

static void bmRotateScanline24(	const unsigned char *	from,
				int			pixelsWide,
				unsigned char *		bufOut,
				int			bytesPerRow,
				int			trow,
				int			tcol,
				int			eRow,
				int			eCol,
				int			xRow,
				int			xCol,
				int			sRow,
				int			d )
    {
    int		fcol;
    int		tbyte= tcol* 3;

    for ( fcol= 0; fcol < pixelsWide; fcol++ )
	{
	int		r= *(from++);
	int		g= *(from++);
	int		b= *(from++);

	while( eCol >= 0 )
	    {
	    bufOut[ trow* bytesPerRow+ tbyte    ]= r;
	    bufOut[ trow* bytesPerRow+ tbyte+ 1 ]= g;
	    bufOut[ trow* bytesPerRow+ tbyte+ 2 ]= b;
	    tbyte += 3; eCol -= d;
	    }
	eCol += xCol;
	while( eRow >= 0 )
	    {
	    bufOut[ trow* bytesPerRow+ tbyte    ]= r;
	    bufOut[ trow* bytesPerRow+ tbyte+ 1 ]= g;
	    bufOut[ trow* bytesPerRow+ tbyte+ 2 ]= b;
	    trow += sRow; eRow -= d;
	    }
	eRow += xRow;
	}
    }

/************************************************************************/
/*									*/
/*  Rotate a bitmap over an arbitrary angle.				*/
/*									*/
/*  1)  Sanity of the input.						*/
/*									*/
/************************************************************************/

int bmRotate(	BitmapDescription *		bdOut,
		const BitmapDescription *	bdIn,
		unsigned char **		pBufOut,
		const unsigned char *		bufIn,
		double				angle )
    {
    int				rotations;

    double			sinAngle;
    double			cosAngle;

    int				frow, trow0;
    int				tcol0;

    int				h1;
    int				h2;
    int				w1;
    int				w2;

    /****************************/
    /*  Within the rows.	*/
    /****************************/
    int				d;
    int				xRow, xCol;
    int				sRow;

    /****************************/
    /*  Beginning of rows.	*/
    /****************************/
    int				eRow0, eCol0;
    int				d0;
    int				xRow0, xCol0;

    BitmapDescription		bd;
    unsigned char *		bufOut;

    /*  1  */
    while( angle < -M_PI/4 )
	{ angle += 2* M_PI;	}
    for ( rotations= 0; angle > M_PI/4; rotations++ )
	{ angle -= M_PI/2;	}
    rotations= rotations % 4;

    sinAngle= sin( angle );
    cosAngle= cos( angle );

    h1= sinAngle* bdIn->bdPixelsWide;	/* neg for neg angle.	*/
    h2= cosAngle* bdIn->bdPixelsHigh;
    w1= sinAngle* bdIn->bdPixelsHigh;	/* neg for neg angle.	*/
    w2= cosAngle* bdIn->bdPixelsWide;

    bmCopyDescription( &bd, bdIn );

    if  ( angle > 0 )
	{ bd.bdPixelsWide=  w1+ w2; bd.bdPixelsHigh=  h1+ h2; }
    else{ bd.bdPixelsWide= -w1+ w2; bd.bdPixelsHigh= -h1+ h2; }

    bd.bdBytesPerRow= ( bd.bdPixelsWide* bdIn->bdBitsPerPixel+ 7 )/ 8;
    bd.bdBufferLength= bd.bdPixelsHigh* bd.bdBytesPerRow;
    bufOut= malloc( bd.bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bd.bdBufferLength,bufOut);	}
    memset( bufOut, 0, bd.bdBufferLength );

    if  ( angle > 0 )
	{ sRow= -1; xRow=  2* h1; trow0= h1; tcol0=   0; xCol0=  2* w1; }
    else{ sRow=  1; xRow= -2* h1; trow0=  0; tcol0= -w1; xCol0= -2* w1; }

    xRow0= 2* h2;
    eRow0= xRow0- bdIn->bdPixelsHigh;
    eCol0= xCol0- bdIn->bdPixelsHigh;
    d0= 2* bdIn->bdPixelsHigh;

    d= 2* bdIn->bdPixelsWide;
    xCol= 2* w2;

    for ( frow= 0; frow < bdIn->bdPixelsHigh; frow++ )
	{
	while( eRow0 >= 0 )
	    {
	    switch( bdIn->bdBitsPerPixel )
		{
		case 1:
		case 2:
		case 4:
		    bmRotateScanline124( bufIn+ frow* bdIn->bdBytesPerRow,
				bdIn->bdPixelsWide, bufOut,
				bd.bdBytesPerRow, trow0, tcol0,
				xRow- bdIn->bdPixelsWide,
				xCol- bdIn->bdPixelsWide,
				xRow, xCol, sRow, d, bdIn->bdBitsPerPixel );
		    break;
		case 8:
		    bmRotateScanline8( bufIn+ frow* bdIn->bdBytesPerRow,
				bdIn->bdPixelsWide, bufOut,
				bd.bdBytesPerRow, trow0, tcol0,
				xRow- bdIn->bdPixelsWide,
				xCol- bdIn->bdPixelsWide,
				xRow, xCol, sRow, d );
		    break;
		case 24:
		    bmRotateScanline24( bufIn+ frow* bdIn->bdBytesPerRow,
				bdIn->bdPixelsWide, bufOut,
				bd.bdBytesPerRow, trow0, tcol0,
				xRow- bdIn->bdPixelsWide,
				xCol- bdIn->bdPixelsWide,
				xRow, xCol, sRow, d );
		    break;
		default:
		    LDEB(bdIn->bdBitsPerPixel); return -1;
		}

	    trow0++; eRow0 -= d0;
	    }
	eRow0 += xRow0;
	while( eCol0 >= 0 )
	    { tcol0 -= sRow; eCol0 -= d0; }
	eCol0 += xCol0;
	}

    *bdOut= bd; *pBufOut= bufOut;
    return 0;
    }
