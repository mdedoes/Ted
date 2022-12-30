#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"bmformats.h"
#   include	"bmintern.h"
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

#   if USE_LIBTIFF

#   include	<tiffio.h>

/************************************************************************/

typedef struct TiffReader
    {
    TIFF *		trTiff;
    unsigned short	trCompression;
    unsigned short	trBitsPerSample;
    unsigned short	trPlanarConfig;


    unsigned long	trStripSize;

    unsigned char	trTiled;
    unsigned int		trTileWide;
    unsigned int		trTileHigh;

    unsigned long	trTileSize;
    unsigned long	trTileRowSize;
    unsigned char *	trTileBuffer;

    unsigned short	trPhotometric;

    float *		trYcbcrCoefficients;
    float *		trReferenceBlackWhite;
    unsigned short	trYcbcrPositioning;
    unsigned short	trYcbcrSubSamplingX;
    unsigned short	trYcbcrSubSamplingY;

    /* libtiff wants these contigous and in this order */
    TIFFYCbCrToRGB	trYCbCrToRGB;
    TIFFRGBValue	tr_clamptab[4*256];
    int			tr_cr_r_tab[256];
    int			tr_cb_b_tab[256];
    int			tr_cr_g_tab[256];
    int			tr_cb_g_tab[256];
    int			tr_y_tab[256];
    /* end contiguous block */

    } TiffReader;

static void bmInitTiffReader(	TiffReader *	tr )
    {
    memset( tr, 0, sizeof(TiffReader) );
    }

static void bmCleanTiffReader(	TiffReader *	tr )
    {
    if  ( tr->trTiff )
	{ TIFFClose( tr->trTiff );	}
    if  ( tr->trTileBuffer )
	{ free( tr->trTileBuffer );	}
    }

static int bmTiffExtractYcbrcr(		TiffReader *		tr )
    {
    /* http://www.remotesensing.org/libtiff/TIFFTechNote2.html */

    TIFFGetFieldDefaulted( tr->trTiff, TIFFTAG_YCBCRCOEFFICIENTS, &tr->trYcbcrCoefficients );
    TIFFGetFieldDefaulted( tr->trTiff, TIFFTAG_REFERENCEBLACKWHITE, &tr->trReferenceBlackWhite );

    if  ( TIFFYCbCrToRGBInit( &(tr->trYCbCrToRGB), tr->trYcbcrCoefficients, tr->trReferenceBlackWhite ) < 0 )
	{ LDEB(1); return -1;	}

    TIFFGetFieldDefaulted( tr->trTiff, TIFFTAG_YCBCRPOSITIONING, &tr->trYcbcrPositioning );
    TIFFGetFieldDefaulted( tr->trTiff, TIFFTAG_YCBCRSUBSAMPLING, &tr->trYcbcrSubSamplingX, &tr->trYcbcrSubSamplingY );

    return 0;
    }

static int bmTiffExtractPalette(	TiffReader *		tr,
					BitmapDescription *	bd )
    {
    unsigned short *		redMap= (unsigned short *)0;
    unsigned short *		greenMap= (unsigned short *)0;
    unsigned short *		blueMap= (unsigned short *)0;

    int				color;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_COLORMAP,
				&redMap, &greenMap, &blueMap ) != 1 )
	{ LDEB(TIFFTAG_COLORMAP); return -1; }

    if  ( bd->bdHasAlpha )
	{
	if  ( bd->bdSamplesPerPixel != 2 )
	    { LDEB(bd->bdSamplesPerPixel); return -1; }
	}
    else{
	if  ( bd->bdSamplesPerPixel != 1 )
	    { LDEB(bd->bdSamplesPerPixel); return -1; }
	}

    if  ( utilPaletteSetCount( &(bd->bdPalette), 1 << tr->trBitsPerSample ) )
	{ LDEB(1 << tr->trBitsPerSample); return -1;	}

    for ( color= 0; color < bd->bdPalette.cpColorCount; color++ )
	{
	bd->bdPalette.cpColors[color].rgb8Red= redMap[color]/256;
	bd->bdPalette.cpColors[color].rgb8Green= greenMap[color]/256;
	bd->bdPalette.cpColors[color].rgb8Blue= blueMap[color]/256;
	}

    bd->bdBitsPerSample= 8;
    bd->bdColorEncoding= BMcoRGB8PALETTE;

    if  ( bd->bdHasAlpha )
	{
	bd->bdSamplesPerPixel= 4;
	bd->bdBitsPerPixel= 2* tr->trBitsPerSample;
	}
    else{
	bd->bdSamplesPerPixel= 3;
	bd->bdBitsPerPixel= 1* tr->trBitsPerSample;
	}

    return 0;
    }

static int bmTiffExtractDescription(	int *			pFileFormat,
					BitmapDescription *	bd,
					TiffReader *		tr )
    {
    int			fileFormat;

    unsigned short		u16;
    unsigned int		u32;
    unsigned short *		whatKind;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_COMPRESSION, &(tr->trCompression) ) != 1 )
	{ LDEB(TIFFTAG_COMPRESSION); return -1;	}
    fileFormat= tr->trCompression;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_IMAGEWIDTH, &u32 ) != 1 )
	{ LDEB(TIFFTAG_IMAGEWIDTH); return -1;	}
    bd->bdPixelsWide= u32;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_IMAGELENGTH, &u32 ) != 1 )
	{ LDEB(TIFFTAG_IMAGELENGTH); return -1;	}
    bd->bdPixelsHigh= u32;

    /* A tiled image */
    if  ( TIFFGetField( tr->trTiff, TIFFTAG_TILELENGTH, &tr->trTileHigh ) == 1 )
	{
	tr->trTiled= 1;

	if  ( TIFFGetField( tr->trTiff, TIFFTAG_TILEWIDTH, &tr->trTileWide ) != 1 )
	    { LDEB(TIFFTAG_TILEWIDTH); return -1;	}
	}

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_BITSPERSAMPLE, &tr->trBitsPerSample ) != 1 )
	{ LDEB(TIFFTAG_BITSPERSAMPLE); return -1; }
    if  ( tr->trBitsPerSample > 8 )
	{ LDEB(tr->trBitsPerSample); return -1;	}
    bd->bdBitsPerSample= tr->trBitsPerSample;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_SAMPLESPERPIXEL, &u16 ) != 1 )
	{ LDEB(TIFFTAG_SAMPLESPERPIXEL); return -1; }
    bd->bdSamplesPerPixel= u16;

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_EXTRASAMPLES, &u16,
							 &whatKind ) != 1 )
	{ bd->bdHasAlpha= 0; }
    else{
	switch( u16 )
	    {
	    case 0:
		bd->bdHasAlpha= 0;
		break;

	    case 1:
		if  ( whatKind[0] != EXTRASAMPLE_ASSOCALPHA	&&
		      whatKind[0] != EXTRASAMPLE_UNASSALPHA	)
		    { LLDEB(TIFFTAG_EXTRASAMPLES,whatKind[0]); return -1;}
		bd->bdHasAlpha= 1;
		break;

	    default:
		LLDEB(TIFFTAG_EXTRASAMPLES,u16);
		return -1;
	    }
	}

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_PHOTOMETRIC, &tr->trPhotometric ) != 1 )
	{ LDEB(TIFFTAG_PHOTOMETRIC); return -1;	}

    switch( tr->trPhotometric )
	{
	case PHOTOMETRIC_MINISWHITE:
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_MINISBLACK:
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_RGB:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_PALETTE:
	    if  ( bmTiffExtractPalette( tr, bd ) )
		{ LDEB(tr->trPhotometric); return -1; }
	    break;

	case PHOTOMETRIC_YCBCR:
	    if  ( tr->trCompression == COMPRESSION_JPEG )
		{
		TIFFSetField( tr->trTiff, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
		}
	    else{
		if  ( bmTiffExtractYcbrcr( tr ) )
		    { LDEB(tr->trPhotometric); return -1;	}
		LLDEB(tr->trCompression,COMPRESSION_JPEG); return -1;
		}

	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	default:
	    LDEB(tr->trPhotometric); return -1;
	}

    bd->bdBytesPerRow= ( bd->bdBitsPerPixel* bd->bdPixelsWide + 7 )/ 8;
    bd->bdBufferLength= bd->bdBytesPerRow* bd->bdPixelsHigh;

    if  ( bd->bdSamplesPerPixel != 1 )
	{
	if  ( TIFFGetField( tr->trTiff, TIFFTAG_PLANARCONFIG, &tr->trPlanarConfig ) != 1 )
	    { LDEB(TIFFTAG_PLANARCONFIG); return -1;	}
	if  ( tr->trPlanarConfig != PLANARCONFIG_CONTIG )
	    {
	    LLDEB(bd->bdSamplesPerPixel,tr->trPlanarConfig);
	    LLDEB(bd->bdBitsPerSample,bd->bdBitsPerPixel);
	    }

	/* TOP Raster GeoTiffs from pdok.nl */
	if  ( tr->trPhotometric == PHOTOMETRIC_PALETTE	&&
	      tr->trPlanarConfig != PLANARCONFIG_CONTIG )
	    {
	    LLDEB(tr->trPhotometric,tr->trPlanarConfig);
	    tr->trPlanarConfig= PLANARCONFIG_CONTIG;
	    }
	}
    else{
	tr->trPlanarConfig= PLANARCONFIG_CONTIG;
	}

    if  ( TIFFGetField( tr->trTiff, TIFFTAG_RESOLUTIONUNIT, &u16 ) != 1 )
	{
	/* SLDEB(filename,TIFFTAG_RESOLUTIONUNIT); */
	bd->bdUnit= BMunPIXEL;
	bd->bdXResolution= 1;
	bd->bdYResolution= 1;
	}
    else{
	float		resolution;

	switch( u16 )
	    {
	    case	RESUNIT_INCH:
		bd->bdUnit= BMunINCH;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); return -1; }
		bd->bdXResolution= (int)resolution;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); return -1; }
		bd->bdYResolution= (int)resolution;
		break;
	    case	RESUNIT_CENTIMETER:
		bd->bdUnit= BMunM;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); return -1; }
		bd->bdXResolution= (int)100* resolution;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); return -1; }
		bd->bdYResolution= 100* (int)resolution;
		break;
	    case	RESUNIT_NONE:
		bd->bdUnit= BMunPIXEL;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); resolution= 1;	}
		bd->bdXResolution= (int)resolution;
		if  ( TIFFGetField( tr->trTiff, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); resolution= 1;	}
		bd->bdYResolution= (int)resolution;
		break;
	    default:
		LDEB(u16); return -1;
	    }

	if  ( bd->bdXResolution == 0	||
	      bd->bdYResolution == 0	)
	    {
	    LLDEB(bd->bdXResolution,bd->bdYResolution);
	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;
	    }
	}

    *pFileFormat= fileFormat;

    return 0;
    }

# if 0
static int bmTiffYCbCrtoRGB(	TiffReader *		tr,
				unsigned char *		to,
				const unsigned char *	from )
    {
    unsigned int	Y, Cb, Cr;
    unsigned int	r, g, b;

    TIFFYCbCrtoRGB( &(tr->trYCbCrToRGB), Y, Cb, Cr, &r, &g, &b);

    *(to++)= (r&0xff);
    *(to++)= (g&0xff);
    *(to++)= (b&0xff);

    return 0;
    }
# endif

static int bmTiffReadStrips(	TiffReader *			tr,
				const BitmapDescription *	bd,
				unsigned char *			buffer )
    {
    int		offset= 0;
    int		strip= 0;

    tr->trStripSize= TIFFStripSize( tr->trTiff );

    while( offset < bd->bdBufferLength )
	{
	int		done;

	done= TIFFReadEncodedStrip( tr->trTiff, strip, buffer+ offset,
					    bd->bdBufferLength- offset );
	if  ( done < 1 )
	    { LDEB(done); return -1; }

	offset += done; strip++;
	}

    return 0;
    }

static int bmTiffReadTiles(	TiffReader *			tr,
				const BitmapDescription *	bd,
				unsigned char *			buffer )
    {
    int		imageRow;
    int		tileRowSizeSource;

    if  ( bd->bdBitsPerPixel % 8 )
	{ LDEB(bd->bdBitsPerPixel); return -1;	}

    tr->trTileSize= TIFFTileSize( tr->trTiff );
    /*tileSize= ( tileWide* tileHigh* bd->bdBitsPerPixel )/ 8;*/
    tr->trTileBuffer= malloc( tr->trTileSize+ 1 );
    if  ( ! tr->trTileBuffer )
	{ LXDEB(tr->trTileSize,tr->trTileBuffer); return -1;	}

    tr->trTileRowSize= TIFFTileRowSize( tr->trTiff );

    tileRowSizeSource= ( tr->trTileWide* bd->bdBitsPerPixel )/ 8;

    for ( imageRow= 0; imageRow < bd->bdPixelsHigh; imageRow += tr->trTileHigh )
	{
	int		imageCol;
	int		tileRow;

	for ( imageCol= 0; imageCol < bd->bdPixelsWide; imageCol += tr->trTileWide )
	    {
	    const int	z= 0; /* no support for image depth > 1 */
	    const int	sample= 0; /* no support for planar images */

	    int		colOffset= ( imageCol* bd->bdBitsPerPixel )/ 8;
	    int		row;

	    int		pixelsWide= bd->bdPixelsWide- imageCol;
	    int		tileRowSizeTarget;
	    int		done;

	    if  ( pixelsWide > tr->trTileWide )
		{ pixelsWide = tr->trTileWide;	}
	    tileRowSizeTarget= ( pixelsWide* bd->bdBitsPerPixel )/ 8;

	    done= TIFFReadTile( tr->trTiff, tr->trTileBuffer,
						    imageCol, imageRow, z, sample );
	    if  ( done < 0 )
		{ LLLDEB(imageRow,imageCol,done); return -1; }

	    for ( tileRow= 0, row= imageRow;
		  tileRow < tr->trTileHigh && row < bd->bdPixelsHigh;
		  tileRow++, row++ )
		{
		memcpy( buffer+ row* bd->bdBytesPerRow+ colOffset,
			tr->trTileBuffer+ tileRow* tileRowSizeSource,
			tileRowSizeTarget );
		}
	    }
	}

    return 0;
    }

static int bmTiffReadScanlines(	TiffReader *			tr,
				const BitmapDescription *	bd,
				unsigned char *			buffer )
    {
    int	row;

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	if  ( TIFFReadScanline( tr->trTiff, buffer+ row* bd->bdBytesPerRow,
							row, 0 ) < 0 )
	    { LDEB(row); return -1; }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a tiff file.							*/
/*									*/
/*  9)  Allocate an extra byte as the libtiff fax 3 code scribbles	*/
/*	beyond the end of the buffer.					*/
/*									*/
/************************************************************************/

int bmReadTiffFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    int				rval= 0;

    unsigned char *		buffer= (unsigned char *)0;
    int				fileFormat= -1;

    TiffReader			tr;

    bmInitTiffReader( &tr );

    tr.trTiff= TIFFOpen( utilMemoryBufferGetString( filename ), "r" );
    if  ( ! tr.trTiff )
	{ XDEB(tr.trTiff); rval= -1; goto ready;	}

    if  ( bmTiffExtractDescription( &fileFormat, bd, &tr ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  9  */
    buffer= (unsigned char *)malloc( bd->bdBufferLength+ 1 );
    if  ( ! buffer )
	{ XDEB(buffer); rval= -1; goto ready;	}

    if  ( bd->bdBitsPerPixel % 8 == 0 )
	{
	if  ( tr.trTiled )
	    {
	    if  ( bmTiffReadTiles( &tr, bd, buffer ) )
		{ LDEB(tr.trTiled); rval= -1; goto ready;	}
	    }
	else{
	    if  ( bmTiffReadStrips( &tr, bd, buffer ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}
    else{
	if  ( tr.trTiled )
	    {
	    if  ( bmTiffReadTiles( &tr, bd, buffer ) )
		{ LDEB(tr.trTiled); rval= -1; goto ready;	}
	    }
	else{
	    if  ( bmTiffReadScanlines( &tr, bd, buffer ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}

    if  ( tr.trPlanarConfig != PLANARCONFIG_CONTIG )
	{
	unsigned char *		scratch;

	scratch= (unsigned char *)malloc( bd->bdBufferLength+ 1 );
	if  ( ! scratch )
	    { XDEB(scratch); rval= -1; goto ready;	}

	if  ( bmPlanarToChunky( scratch, buffer, bd ) )
	    { LDEB(1); free( scratch ); rval= -1; goto ready;	}

	free( buffer ); buffer= scratch;
	}

    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
    *pPrivateFormat= fileFormat;

  ready:

    bmCleanTiffReader( &tr );

    return rval;
    }


int bmCanWriteTiffFile( const BitmapDescription *	bd,
			int				privateFormat )
    {
    switch( privateFormat )
	{
	case COMPRESSION_NONE:
	    break;
	case COMPRESSION_CCITTRLE:
	case COMPRESSION_CCITTFAX3:
	case COMPRESSION_CCITTFAX4:
	    if  ( bd->bdBitsPerPixel == 1 )
		{ break;	}
	    else{ return -1;	}
	case COMPRESSION_LZW:
	    return 0;
	case COMPRESSION_JPEG:
	    if  ( bd->bdBitsPerSample == 8 )
		{ break;	}
	    else{ return -1;	}
	case COMPRESSION_NEXT:
	    return -1;
	case COMPRESSION_CCITTRLEW:
	case COMPRESSION_PACKBITS:
	case COMPRESSION_THUNDERSCAN:
	    if  ( bd->bdBitsPerPixel == 1 )
		{ break;	}
	    else{ return -1;	}
	default:
	    LDEB(privateFormat); return -1;
	}

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    switch( bd->bdBitsPerSample )
		{
		case 4:
		case 8:
		    break;
		default:
		    /* LDEB(bd->bdBitsPerSample); */ return -1;
		}
	    break;
	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a bitmap to a TIFF file.					*/
/*									*/
/************************************************************************/

int bmWriteTiffFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    TIFF *			t= (TIFF *)0;
    int				colorSpace= PHOTOMETRIC_MINISWHITE; /*bah*/
    unsigned long		rowsPerStrip;
    int				unit, xResolution, yResolution;
    int				row;
    int				i;

    unsigned short *	redMap= (unsigned short *)0;
    unsigned short *	greenMap= (unsigned short *)0;
    unsigned short *	blueMap= (unsigned short *)0;

    t= TIFFOpen( utilMemoryBufferGetString( filename ), "w" );
    if  ( ! t )
	{ XDEB(t); rval= -1; goto ready;	}

    if  ( ! TIFFSetField( t, TIFFTAG_COMPRESSION, privateFormat ) )
	{ LLDEB(TIFFTAG_COMPRESSION,privateFormat); rval= -1; goto ready; }

    if  ( ! TIFFSetField( t, TIFFTAG_IMAGELENGTH, (long)bd->bdPixelsHigh ) )
	{ LLDEB(TIFFTAG_IMAGELENGTH,(long)bd->bdPixelsHigh); rval= -1; goto ready; }

    if  ( ! TIFFSetField( t, TIFFTAG_IMAGEWIDTH, (long)bd->bdPixelsWide ) )
	{ LLDEB(TIFFTAG_IMAGEWIDTH,(long)bd->bdPixelsWide); rval= -1; goto ready; }

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:	colorSpace= PHOTOMETRIC_MINISWHITE;
				break;
	case BMcoWHITEBLACK:	colorSpace= PHOTOMETRIC_MINISBLACK;
				break;
	case BMcoRGB:	colorSpace= PHOTOMETRIC_RGB;
				break;
	case BMcoRGB8PALETTE: colorSpace= PHOTOMETRIC_PALETTE;
				break;
	default:
		LDEB(bd->bdColorEncoding); rval= -1; goto ready;
	}

    if  ( ! TIFFSetField( t, TIFFTAG_PHOTOMETRIC, colorSpace ) )
	{ LLDEB(TIFFTAG_PHOTOMETRIC,colorSpace); rval= -1; goto ready; }

    if  ( colorSpace == PHOTOMETRIC_PALETTE )
	{
	int			colorCountOut;
	int			colorCountIn;

	switch( bd->bdBitsPerPixel )
	    {
	    case 1:
		colorCountIn= 2;
		colorCountOut= 16;
		break;

	    case 2:
		colorCountIn= 4;
		colorCountOut= 16;
		break;

	    case 4:
		colorCountIn= 16;
		colorCountOut= 16;
		break;

	    case 8:
		colorCountIn= 256;
		colorCountOut= 256;
		break;

	    case 16:
		if  ( bd->bdHasAlpha )
		    {
		    colorCountIn= 256;
		    colorCountOut= 256;
		    }
		else{
		    LDEB(bd->bdHasAlpha);
		    LLDEB(bd->bdBitsPerPixel,bd->bdBitsPerSample); rval= -1; goto ready;
		    }
		break;

	    default:
		LLDEB(bd->bdBitsPerPixel,bd->bdBitsPerSample); rval= -1; goto ready;
	    }

	if  ( bd->bdPalette.cpColorCount > colorCountIn )
	    { LLDEB(bd->bdPalette.cpColorCount,colorCountIn); rval= -1; goto ready; }

	/*
	if  ( colorCountIn != colorCountOut )
	    { LLDEB(colorCountIn,colorCountOut); LDEB(bd->bdBitsPerPixel); }
	*/

	redMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );
	greenMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );
	blueMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );

	if  ( ! redMap || ! greenMap || ! blueMap )
	    { LDEB(colorCountOut); rval= -1; goto ready; }

	for ( i= 0; i < bd->bdPalette.cpColorCount; i++ )
	    {
	    redMap[i]=	 257* bd->bdPalette.cpColors[i].rgb8Red;
	    greenMap[i]= 257* bd->bdPalette.cpColors[i].rgb8Green;
	    blueMap[i]=	 257* bd->bdPalette.cpColors[i].rgb8Blue;
	    }
	for ( ; i < colorCountOut; i++ )
	    {
	    redMap[i]=	 redMap[i-1];
	    greenMap[i]= greenMap[i-1];
	    blueMap[i]=	 blueMap[i-1];
	    }

	if  ( bd->bdHasAlpha )
	    {
	    if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE,
						    bd->bdBitsPerPixel/ 2 ) )
		{ LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); rval= -1; goto ready; }
	    if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL, 2 ) )
		{ LLDEB(TIFFTAG_SAMPLESPERPIXEL,1); rval= -1; goto ready; }
	    }
	else{
	    if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE,
						    bd->bdBitsPerPixel ) )
		{ LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); rval= -1; goto ready; }
	    if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL, 1 ) )
		{ LLDEB(TIFFTAG_SAMPLESPERPIXEL,1); rval= -1; goto ready; }
	    }

	if  ( TIFFSetField( t, TIFFTAG_COLORMAP,
				    redMap, greenMap, blueMap ) != 1 )
	    { LDEB(TIFFTAG_COLORMAP); rval= -1; goto ready;	}
	}
    else{
	if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE, bd->bdBitsPerSample ) )
	    { LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); rval= -1; goto ready; }

	if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL,
						bd->bdSamplesPerPixel ) )
	    { LLDEB(TIFFTAG_SAMPLESPERPIXEL,bd->bdSamplesPerPixel); rval= -1; goto ready; }
	}

    if  ( ! TIFFSetField( t, TIFFTAG_PLANARCONFIG, 1 ) )
	{ LLDEB(TIFFTAG_PLANARCONFIG,1); rval= -1; goto ready; }

					/********************************/
					/*  To make PC's happy, make	*/
					/*  strips that are a little	*/
					/*  smaller than 64K.		*/
					/*  JPEG compression works per	*/
					/*  strip. It requires that the	*/
					/*  height of a strip be a	*/
					/*  multiple of 8. An attempt	*/
					/*  is made to combine these	*/
					/*  restrictions.		*/
					/********************************/

    rowsPerStrip= 65000/ bd->bdBytesPerRow;
    if  ( rowsPerStrip % 8 )
	{ rowsPerStrip= 8* ( rowsPerStrip/ 8 );	}
    if  ( rowsPerStrip < 1 )
	{ rowsPerStrip= 1;	}

    if  ( ! TIFFSetField( t, TIFFTAG_ROWSPERSTRIP, rowsPerStrip ) )
	{ LLDEB(TIFFTAG_ROWSPERSTRIP,rowsPerStrip); rval= -1; goto ready; }

    if  ( bd->bdHasAlpha )
	{
	unsigned short	value= EXTRASAMPLE_ASSOCALPHA;

	if  ( ! TIFFSetField( t, TIFFTAG_EXTRASAMPLES, 1, &value ) )
	    {
	    LLDEB(colorSpace,bd->bdSamplesPerPixel);
	    LLDEB(TIFFTAG_EXTRASAMPLES,bd->bdHasAlpha); 
	    rval= -1; goto ready;
	    }
	}

    switch( bd->bdUnit )
	{
	case	BMunM:
	    unit= RESUNIT_CENTIMETER;
	    xResolution= ( bd->bdXResolution+ 50 )/ 100;
	    yResolution= ( bd->bdYResolution+ 50 )/ 100;
	    break;

	case	BMunINCH:
	    unit= RESUNIT_INCH;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;

	case	BMunPOINT:
	    unit= RESUNIT_INCH;
	    xResolution= bd->bdXResolution* 72;
	    yResolution= bd->bdYResolution* 72;
	    break;

	case BMunPIXEL:
	    unit= RESUNIT_NONE;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;

	default:
	    LDEB(bd->bdUnit);
	    unit= RESUNIT_NONE;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;
	}

    if  ( ! TIFFSetField( t, TIFFTAG_RESOLUTIONUNIT, unit ) )
	{ LLDEB(TIFFTAG_RESOLUTIONUNIT, unit); rval= -1; goto ready; }

    if  ( ! TIFFSetField( t, TIFFTAG_XRESOLUTION, (float)xResolution ) )
	{ LLDEB(TIFFTAG_XRESOLUTION, xResolution); rval= -1; goto ready; }

    if  ( ! TIFFSetField( t, TIFFTAG_YRESOLUTION, (float)yResolution ) )
	{ LLDEB(TIFFTAG_YRESOLUTION, yResolution); rval= -1; goto ready; }

    if  ( ! TIFFSetField( t, TIFFTAG_SOFTWARE, "appFrame:"
		    " Mark de Does,"
		    " mark@mdedoes.com" ) )
	{ LDEB(TIFFTAG_SOFTWARE); rval= -1; goto ready; }

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	if  ( TIFFWriteScanline( t, (char *) buffer+ row* bd->bdBytesPerRow,
								row, 0 ) < 0 )
	    { LDEB(row); rval= -1; goto ready; }
	}

  ready:

    if  ( t )
	{ TIFFClose( t ); 	}

    if  ( redMap )
	{ free( redMap );	}
    if  ( greenMap )
	{ free( greenMap );	}
    if  ( blueMap )
	{ free( blueMap );	}

    return rval;
    }

#   endif /* TIFF_FOUND */
