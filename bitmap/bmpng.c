#   include	"bitmapConfig.h"

#   if		USE_LIBPNG	/*	{{	*/

#   include	<stdlib.h>

#   include	"bmformats.h"
#   include	"bmintern.h"
#   include	"bmio.h"
#   include	<png.h>
#   include	<sioFileio.h>
#   include	<utilEndian.h>
#   include	<geoUnits.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

# if 1
#   ifndef png_jmpbuf
    /*  Works with older libs as well */
    # define png_jmpbuf( pngp ) pngp->jmpbuf
#   endif
# endif

/************************************************************************/
/*									*/
/*  Read a bitmap from a PNG file.					*/
/*									*/
/************************************************************************/

static int bpPngiToBitmap(	const png_structp		pngp,
				png_info *			pngi,
				BitmapDescription *		bd )
    {
    unsigned int	col;
    png_uint_32		res_x, res_y;
    int			unit_type= 0;

    bd->bdPixelsWide= png_get_image_width( pngp, pngi );
    bd->bdPixelsHigh= png_get_image_height( pngp, pngi );
    bd->bdHasAlpha= 0;

    switch( png_get_color_type( pngp, pngi ) )
	{
	case PNG_COLOR_TYPE_PALETTE:
	    {
	    int		num_palette;
	    png_colorp	palette;

	    png_get_PLTE( pngp, pngi, &palette, &num_palette );

	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    if  ( utilPaletteSetCount( &(bd->bdPalette), num_palette ) )
		{ LDEB(num_palette); return -1;	}
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdBitsPerPixel= png_get_bit_depth( pngp, pngi );

	    for ( col= 0; col < bd->bdPalette.cpColorCount; col++ )
		{
		bd->bdPalette.cpColors[col].rgb8Red= palette[col].red;
		bd->bdPalette.cpColors[col].rgb8Green= palette[col].green;
		bd->bdPalette.cpColors[col].rgb8Blue= palette[col].blue;
		bd->bdPalette.cpColors[col].rgb8Alpha= 255;
		}
	    }
	    break;

	case PNG_COLOR_TYPE_RGB:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_GRAY:
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	    bd->bdHasAlpha= 1;
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
	    bd->bdHasAlpha= 1;
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	default:
	    LDEB(png_get_color_type( pngp, pngi )); return -1;
	}

    bd->bdBytesPerRow= png_get_rowbytes( pngp, pngi );
    bd->bdBufferLength= bd->bdBytesPerRow* bd->bdPixelsHigh;

    if  ( !  png_get_pHYs( pngp, pngi, &res_x, &res_y, &unit_type ) )
	{ unit_type= PNG_RESOLUTION_UNKNOWN;	}

    switch( unit_type )
	{
	case PNG_RESOLUTION_UNKNOWN:
	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;
	    break;

	case PNG_RESOLUTION_METER:
	    bd->bdUnit= BMunM;
	    bd->bdXResolution= res_x;
	    bd->bdYResolution= res_y;
	    break;

	default:
	    LDEB(unit_type);
	    return -1;
	}

    return 0;
    }

static int bmPngReadContents(	png_info *		pngi,
				png_struct *		png,
				BitmapDescription *	bd,
				unsigned char **	pBuffer )
    {
    int				numberOfPasses;
    int				i;

    unsigned char *		buffer;

    numberOfPasses= 1;
    if  ( png_get_interlace_type( png, pngi ) )
	{ numberOfPasses= png_set_interlace_handling( png ); }

    if  ( png_get_color_type( png, pngi ) == PNG_COLOR_TYPE_RGB	&&
	  png_get_bit_depth( png, pngi ) == 16			)
	{
	const unsigned short	one= 1;
	const unsigned char *	testEndian= (const unsigned char *)&one;

	if  ( testEndian[0] )
	    { png_set_swap( png );	}
	}

    png_start_read_image( png );

    buffer= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); return -1;	}

    for ( i= 0; i < numberOfPasses; i++ )
	{
	unsigned int		row;

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    unsigned char *	to= buffer+ row* bd->bdBytesPerRow;

	    png_read_rows( png, &to, NULL, 1 );
	    }
	}

    png_read_end( png, pngi );

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! bd->bdHasAlpha				)
	{ bmMakeMonochrome( bd, buffer );	}

    *pBuffer= buffer; return 0;
    }

int bmReadPngFile(	const struct MemoryBuffer *	filename,
			unsigned char **		pBuffer,
			BitmapDescription *		bd,
			int *				pPrivateFormat )
    {
    SimpleInputStream *	sis;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    if  ( bmPngReadPng( bd, pBuffer, sis ) )
	{ LDEB(1); sioInClose( sis ); return -1; }

    sioInClose( sis );

    *pPrivateFormat= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an image from a PNG input stream.				*/
/*									*/
/************************************************************************/

static void bmReadPngBytes(	png_struct *	png,
				png_byte *	buffer,
				png_size_t	count )
    {
    SimpleInputStream *	sis;

    sis= (SimpleInputStream *)png_get_io_ptr( png );

    sioInReadBytes( sis, (unsigned char *)buffer, count );

    return;
    }

int bmPngReadPng(	BitmapDescription *	bd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    png_structp		pngp= (png_structp)0;
    png_infop		pngip= (png_infop)0;

    unsigned char *	buffer;

    pngp = png_create_read_struct( PNG_LIBPNG_VER_STRING, (void *)0,
				   (png_error_ptr)0, (png_error_ptr)0 );
    if  ( ! pngp )
	{ LDEB(1); return -1;	}

    pngip = png_create_info_struct( pngp );
    if  ( ! pngip )
	{
	LDEB(1);
	png_destroy_read_struct( &pngp, (png_infop *)0, (png_infop *)0 );
	return -1;
	}

    if  ( setjmp( png_jmpbuf( pngp ) ) )
	{ 
	LDEB(1);
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1; 
	}

    png_init_io( pngp, (FILE *)0 );
    png_set_read_fn( pngp, (void *)sis, bmReadPngBytes );

    png_read_info( pngp, pngip );

    if  ( bpPngiToBitmap( pngp, pngip, bd ) )
	{
	LDEB(bd->bdPalette.cpColorCount);
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1;
	}

    if  ( bmPngReadContents( pngip, pngp, bd, &buffer ) )
	{
	LDEB(bd->bdBufferLength);
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1;
	}

    *pBuffer= buffer;

    png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Can write a bitmap to a PNG file?					*/
/*  All images can be written to png.					*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int bmCanWritePngFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! bd->bdHasAlpha				&&
	  bd->bdBitsPerPixel > 8			)
	{ return -1;	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdHasAlpha				&&
	  bd->bdBitsPerPixel > 16			)
	{ return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Write a bitmap to a PNG file.					*/
/*									*/
/************************************************************************/

static int bpPngiFromBitmap(	png_structp			png,
				png_info *			pngi,
				png_colorp *			pPalette,
				const BitmapDescription *       bd )
    {
    int			bit_depth;
    int			color_type;
    png_color_8		sig_bit;

    sig_bit.red= sig_bit.green= sig_bit.blue= sig_bit.gray= sig_bit.alpha= 0;

    switch( bd->bdUnit )
	{
	case BMunM:
	    png_set_pHYs( png, pngi,
			    bd->bdXResolution,
			    bd->bdYResolution,
			    PNG_RESOLUTION_METER);
	    break;

	case BMunINCH:
	    png_set_pHYs( png, pngi,
			    (int)( 39.37* bd->bdXResolution ),
			    (int)( 39.37* bd->bdYResolution ),
			    PNG_RESOLUTION_METER);
	    break;

	case BMunPOINT:
	    png_set_pHYs( png, pngi,
			    POINTS_PER_M* bd->bdXResolution,
			    POINTS_PER_M* bd->bdYResolution,
			    PNG_RESOLUTION_METER);
	    break;

	case BMunPIXEL:
	    png_set_pHYs(png, pngi, 1, 1, PNG_RESOLUTION_UNKNOWN);
	    break;

	default:
	    LDEB(bd->bdUnit);
	    png_set_pHYs(png, pngi, 1, 1, PNG_RESOLUTION_UNKNOWN);
	    break;
	}

    switch( bd->bdColorEncoding )
	{
	int	i;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    bit_depth= bd->bdBitsPerSample;
	    if  ( bd->bdHasAlpha )
		{
		color_type= PNG_COLOR_TYPE_GRAY_ALPHA;
		sig_bit.alpha= bd->bdBitsPerSample;
		}
	    else{ color_type= PNG_COLOR_TYPE_GRAY;		}
	    sig_bit.gray= bd->bdBitsPerSample;
	    break;

	case BMcoRGB:
	    bit_depth= bd->bdBitsPerSample;
	    if  ( bd->bdHasAlpha )
		{
		color_type= PNG_COLOR_TYPE_RGB_ALPHA;
		sig_bit.alpha= bd->bdBitsPerSample;
		}
	    else{ color_type= PNG_COLOR_TYPE_RGB;	}
	    sig_bit.red= bd->bdBitsPerSample;
	    sig_bit.green= bd->bdBitsPerSample;
	    sig_bit.blue= bd->bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bd->bdHasAlpha )
		{ bit_depth= bd->bdBitsPerPixel/ 2;	}
	    else{ bit_depth= bd->bdBitsPerPixel;	}
	    color_type= PNG_COLOR_TYPE_PALETTE;

	    if  ( bd->bdPalette.cpColorCount + bd->bdHasAlpha >
						    PNG_MAX_PALETTE_LENGTH )
		{
		LLDEB(bd->bdPalette.cpColorCount,PNG_MAX_PALETTE_LENGTH);
		return -1;
		}

	    *pPalette= (png_color *)malloc( PNG_MAX_PALETTE_LENGTH*
						    sizeof( png_color ) );
	    if  ( ! *pPalette )
		{ XDEB(*pPalette); return -1;	}
	    sig_bit.red= bd->bdBitsPerSample;
	    sig_bit.green= bd->bdBitsPerSample;
	    sig_bit.blue= bd->bdBitsPerSample;
	    sig_bit.alpha= 0;

	    /* Give palette predictable contents */
	    for ( i= 0; i < PNG_MAX_PALETTE_LENGTH; i++ )
		{
		(*pPalette)[i].red= 0;
		(*pPalette)[i].green= 0;
		(*pPalette)[i].blue= 0;
		}

	    for ( i= 0; i < bd->bdPalette.cpColorCount; i++ )
		{
		(*pPalette)[i].red= bd->bdPalette.cpColors[i].rgb8Red;
		(*pPalette)[i].green= bd->bdPalette.cpColors[i].rgb8Green;
		(*pPalette)[i].blue= bd->bdPalette.cpColors[i].rgb8Blue;
		}
	    if  ( bd->bdHasAlpha )
		{
		if  ( bd->bdHasAlpha )
		    { sig_bit.alpha= bd->bdBitsPerSample;	}

		(*pPalette)[i].red= 255;
		(*pPalette)[i].green= 255;
		(*pPalette)[i].blue= 255;
		/*
		(*pPalette)[i].alpha= 0;
		*/
		}

	    png_set_PLTE( png, pngi, (*pPalette),
				bd->bdPalette.cpColorCount+ bd->bdHasAlpha );
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    png_set_sBIT( png, pngi, &sig_bit );
    png_set_IHDR( png, pngi,
			bd->bdPixelsWide, bd->bdPixelsHigh,
			bit_depth, color_type,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE,
			PNG_FILTER_TYPE_BASE );

    return 0;
    }

static void bmPngWriteContents(	unsigned char **		pScratch,
				png_structp			png,
				png_infop			pngi,
				const unsigned char *		buffer,
				const BitmapDescription *	bd )
    {
    int			row;
    int			color_type= png_get_color_type( png, pngi );

    if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
	{
	if  ( bd->bdBitsPerPixel == 1 )
	    { png_set_invert_mono( png );	}
	else{
	    unsigned char *	scratch= (unsigned char *)realloc( *pScratch, bd->bdBytesPerRow );
	    if  ( ! scratch )
		{ LXDEB(bd->bdBytesPerRow,scratch); return;	}
	    *pScratch= scratch;
	    }
	}

    if  ( color_type == PNG_COLOR_TYPE_RGB	&&
	  bd->bdBitsPerSample == 16		)
	{
	const unsigned short	one= 1;
	const unsigned char *	testEndian= (const unsigned char *)&one;

	/* Does not work!
	if  ( testEndian[0] )
	    { png_set_swap( png );	}
	*/
	if  ( testEndian[0] )
	    {
	    unsigned char *	scratch= (unsigned char *)realloc( *pScratch, bd->bdBytesPerRow );
	    if  ( ! scratch )
		{ LXDEB(bd->bdBytesPerRow,scratch); return;	}
	    *pScratch= scratch;
	    }
	}

    png_write_info( png, pngi );

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= buffer+ row* bd->bdBytesPerRow;

	if  ( bd->bdColorEncoding == BMcoBLACKWHITE	&&
	      *pScratch					)
	    {
	    int			col;
	    unsigned char *	to= *pScratch;

	    for ( col= 0; col < bd->bdBytesPerRow; col++ )
		{ *(to++)= ~*(from++); }

	    from= *pScratch;
	    }

	if  ( color_type == PNG_COLOR_TYPE_RGB	&&
	      bd->bdBitsPerSample == 16					&&
	      *pScratch							)
	    {
	    int			col;
	    const BmUint16 *	fr= (const BmUint16 *)from;
	    unsigned char *	to= *pScratch;

	    for ( col= 0; col < bd->bdBytesPerRow; col += sizeof(BmUint16) )
		{
		utilEndianStoreBeInt16( *(fr++), to );
		to += sizeof(BmUint16);
		}

	    from= *pScratch;
	    }

	png_write_rows( png, (unsigned char **)&from, 1 );
	}

    png_write_end( png, pngi );

    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int bmWritePngFile(	const struct MemoryBuffer *	filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    SimpleOutputStream *	sos;

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    if  ( bmPngWritePng( bd, buffer, sos ) )
	{ LDEB(1); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Write a bitmap to a png stream.					*/
/*									*/
/************************************************************************/

static void bmPngWriteBytes(	png_struct *	png,
				png_byte *	buffer,
				png_size_t	count )
    {
    SimpleOutputStream *	sos;

    sos= (SimpleOutputStream *)png_get_io_ptr( png );

    if  ( sioOutWriteBytes( sos, (const unsigned char *)buffer, count ) != count )
	{ LDEB(count);	}

    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static void bmPngFlushBytes(    png_struct *    png )
    { return;	}

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Write a png. Intermediate function to cope with setjmp/longjmp	*/
/*  semantics.								*/
/*									*/
/************************************************************************/

static int bmPngWritePng_x(	png_colorp *			pPalette,
				unsigned char **		pScratch,
				const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int			rval= 0;
    png_structp		pngp= (png_structp)0;
    png_infop		pngip= (png_infop)0;

    pngp = png_create_write_struct( PNG_LIBPNG_VER_STRING, (void *)0,
				    (png_error_ptr)0, (png_error_ptr)0 );
    if  ( ! pngp )
	{ XDEB(pngp); rval= -1; goto ready;	}

    pngip = png_create_info_struct( pngp );
    if  ( ! pngip )
	{ XDEB(pngip); rval= -1; goto ready;	}

    /*
    As the info struct is built by libpng this is not needed:
    (The call will disappear from libpng in version 1.4)
    png_info_init( pngi ); 
    */

    if  ( setjmp( png_jmpbuf( pngp ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rval= 0; /* to make gcc happy */

    png_init_io( pngp, (FILE *)0 );
    png_set_write_fn( pngp, (void *)sos, bmPngWriteBytes, bmPngFlushBytes );

    if  ( bpPngiFromBitmap( pngp, pngip, pPalette, bd ) )
	{ LDEB(bd->bdColorEncoding); rval= -1; goto ready; }

    bmPngWriteContents( pScratch, pngp, pngip, buffer, bd );

  ready:

    png_destroy_write_struct( &pngp, &pngip );

    return rval;
    }

int bmPngWritePng(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int			rval= 0;

    png_colorp		palette= (png_colorp)0;
    unsigned char *	scratch= (unsigned char *)0;

    rval= bmPngWritePng_x( &palette, &scratch, bd, buffer, sos );

    if  ( palette )
	{ free( palette );	}
    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

#   endif		/*	USE_LIBPNG	}}	*/
