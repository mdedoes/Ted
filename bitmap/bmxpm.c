#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"bmintern.h"

#   if USE_LIBXPM

#   include	<X11/xpm.h>

/************************************************************************/
/*									*/
/*  Characters approximately in gray scale order.			*/
/*									*/
/************************************************************************/

static char	BMXPMBytes[]=
    " "
    "."
    "-,`'"
    "_~:;"
    "*^"
    "!=+<>?1ijl|/"
    "(){}"
    "[]7"
    "35"
    "IJLT"
    "CF"
    "fcrst"
    "Eea2SZ$G"
    "69"
    "K4AP"
    "ovxyz"
    "nuw"
    "UYVXOkhQ"
    "Bmg8D"
    "bdpq"
    "%&0N"
    "@"
    "RWHM"
    "#"
    ;

/************************************************************************/
/*									*/
/*  Translate an XPM color to BMcoRGB8PALETTE format; update mapping.	*/
/*									*/
/*  1)  Translate transparent to white.					*/
/*									*/
/************************************************************************/

static int bmXpmPaletteColor(	const char *	c_color,
				int *		pTransparent,
				RGB8Color *	rgb8 )
    {
    char	ch;
    unsigned	r, g, b;

    FILE *	f;

    /*  1  */
    if  ( ! c_color				||
	  ! strcmp( c_color, "None" )		||
	  ! strcmp( c_color, "none" )		||
	  ! strcmp( c_color, "Transparent" )	||
	  ! strcmp( c_color, "transparent" )	)
	{
	*pTransparent= 1;
	rgb8->rgb8Red= 255;
	rgb8->rgb8Green= 255;
	rgb8->rgb8Blue= 255;
	rgb8->rgb8Alpha= 0;

	return 0;
	}

    if  ( sscanf( c_color, "#%4x%4x%4x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r/ 256;
	rgb8->rgb8Green= g/ 256;
	rgb8->rgb8Blue= b/ 256;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    if  ( sscanf( c_color, "#%2x%2x%2x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r;
	rgb8->rgb8Green= g;
	rgb8->rgb8Blue= b;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    if  ( sscanf( c_color, "#%1x%1x%1x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r* 16;
	rgb8->rgb8Green= g* 16;
	rgb8->rgb8Blue= b* 16;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    f= fopen( "/usr/lib/X11/rgb.txt", "r" );
    if  ( f )
	{
	char	line[100];
	char	color[100];

	while( fgets( line, 100, f ) )
	    {
	    if  ( sscanf( line, "%u %u %u %s", &r, &g, &b, color ) == 4 )
		{
		const char *	s1= color;
		const char *	s2= c_color;

		for (;;)
		    {
		    char	c1;
		    char	c2;

		    while( *s1 == ' ' )	{ s1++;	}
		    while( *s2 == ' ' )	{ s2++;	}

		    c1= *s1; c2= *s2;

		    if  ( ! c1 && ! c2 )
			{
			*pTransparent= 0;
			rgb8->rgb8Red= r;
			rgb8->rgb8Green= g;
			rgb8->rgb8Blue= b;
			rgb8->rgb8Alpha= 255;

			fclose( f ); return 0;
			}

		    if  ( isupper( c1 ) )	{ c1= tolower( c1 );	}
		    if  ( isupper( c2 ) )	{ c2= tolower( c2 );	}

		    if  ( c1 != c2 )
			{ break;	}

		    s1++; s2++;
		    }
		}
	    }

	fclose( f );
	}

    SDEB(c_color); return -1;
    }

/************************************************************************/
/*									*/
/*  Read an XPM file.							*/
/*									*/
/************************************************************************/

int bmReadXpmFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    int			rval= 0;

    int			ret;
    unsigned int	row;
    unsigned int	col;
    unsigned char *	buffer= (unsigned char *)0;
    
    XpmImage		image;

    int			bitsPerPixel= 8;

    const char *	fn= utilMemoryBufferGetString( filename );

    memset( &image, 0, sizeof(image) );

    ret= XpmReadFileToXpmImage( (char *)fn, &image, (XpmInfo *)0 );
    if  ( ret != XpmSuccess )
	{ SSDEB(fn,XpmGetErrorString(ret)); rval= -1; goto ready;	}

    if  ( image.ncolors > 256* 256 )
	{ LDEB(image.ncolors); rval= -1; goto ready;	}

    bd->bdPixelsWide= image.width;
    bd->bdPixelsHigh= image.height;

    if  ( image.ncolors > 256 && image.ncolors < 256* 256 )
	{ bitsPerPixel= 16;	}

    switch( bitsPerPixel )
	{
	case 8:
	case 16:
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdBitsPerPixel= bitsPerPixel;
	    bd->bdBytesPerRow= ( bd->bdBitsPerPixel+ 7 )/8* bd->bdPixelsWide;
	    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdHasAlpha= 0;
	    if  ( utilPaletteSetCount( &(bd->bdPalette), image.ncolors ) )
		{ LDEB(image.ncolors); rval= -1; goto ready; }

	    for ( col= 0; col < image.ncolors; col++ )
		{
		int		transparent= 0;

		if  ( bmXpmPaletteColor( image.colorTable[col].c_color,
				&transparent, bd->bdPalette.cpColors+ col ) )
		    {
		    SSDEB(image.colorTable[col].string,
					    image.colorTable[col].c_color);
		    rval= -1; goto ready;
		    }

		if  ( transparent && ! bd->bdHasAlpha )
		    {
		    bd->bdHasAlpha= 1;
		    bd->bdBitsPerPixel= 2* bitsPerPixel;
		    }
		}

	    if  ( bmCalculateSizes( bd ) )
		{
		LDEB(bd->bdPalette.cpColorCount);
		rval= -1; goto ready;
		}

	    buffer= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! buffer )
		{
		LXDEB(bd->bdBufferLength,buffer);
		rval= -1; goto ready;
		}
	    break;

	default:
	    LDEB(bitsPerPixel);
	    LLDEB(image.cpp,image.ncolors);
	    rval= -1; goto ready;
	}

    bd->bdXResolution= bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    switch( bd->bdBitsPerPixel )
	{
	case 8:
	    if  ( ! bd->bdHasAlpha )
		{
		unsigned int *	from= image.data;

		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    unsigned char *	to= buffer+ row* bd->bdBytesPerRow;

		    for ( col= 0; col < bd->bdPixelsWide; to++, from++, col++ )
			{ *to= *from; }
		    }
		}
	    else{
		LDEB(bd->bdHasAlpha);
		rval= -1; goto ready;
		}

	    break;

	case 16:
	    if  ( ! bd->bdHasAlpha )
		{
		const unsigned int *	from;
		BmUint16 *		to;

		from= image.data;
		to= (BmUint16 *)buffer;

		for ( col= 0; col < bd->bdBufferLength; col++ )
		    { *(to++)= *(from++);	}
		}
	    else{
		const unsigned int *	from;
		unsigned char *		to;

		from= image.data;
		to= buffer;

		for ( col= 0; col < bd->bdBufferLength; col += 2 )
		    {
		    int	c= *(from++);

		    *(to++)= c;
		    *(to++)= bd->bdPalette.cpColors[c].rgb8Alpha;
		    }
		}
	    break;

	case 32:
	    if  ( ! bd->bdHasAlpha )
		{ LDEB(bd->bdHasAlpha); rval= -1; goto ready; }
	    else{
		const unsigned int *	from;
		unsigned char *		to;
		ColorPalette * 		cp= &(bd->bdPalette);

		/* expand palette and alpha */
		from= image.data;
		to= buffer;

		for ( col= 0; col < bd->bdBufferLength; col += 4 )
		    {
		    int	c= *(from++);

		    *(to++)= cp->cpColors[c].rgb8Red;
		    *(to++)= cp->cpColors[c].rgb8Green;
		    *(to++)= cp->cpColors[c].rgb8Blue;
		    *(to++)= cp->cpColors[c].rgb8Alpha;
		    }

		utilPaletteSetCount( cp, 0 );

		bd->bdColorEncoding= BMcoRGB;
		bd->bdHasAlpha= 1;
		bd->bdBitsPerSample= 8;
		bd->bdSamplesPerPixel= 4;
		bd->bdBitsPerPixel= 32;
		}
	    break;

	default:
	    LLDEB(bd->bdBitsPerPixel,image.cpp); rval= -1; goto ready;
	}

    *pBuffer= buffer; buffer= (unsigned char *)0;
    *pPrivateFormat= 0;

  ready:

    XpmFreeXpmImage( &image );

    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write an XPM file.							*/
/*									*/
/************************************************************************/

static void bmXpmClean(		XpmImage *	xpmi )
    {
    unsigned		col;

    if  ( xpmi->data )
	{ free( xpmi->data );	}

    for ( col= 0; col < xpmi->ncolors; col++ )
	{
	if  ( xpmi->colorTable[col].string )
	    { free( xpmi->colorTable[col].string );	}
	if  ( xpmi->colorTable[col].symbolic )
	    { free( xpmi->colorTable[col].symbolic );	}
	if  ( xpmi->colorTable[col].m_color )
	    { free( xpmi->colorTable[col].m_color );	}
	if  ( xpmi->colorTable[col].g4_color )
	    { free( xpmi->colorTable[col].g4_color );	}
	if  ( xpmi->colorTable[col].g_color )
	    { free( xpmi->colorTable[col].g_color );	}
	if  ( xpmi->colorTable[col].c_color )
	    { free( xpmi->colorTable[col].c_color );	}
	}

    if  ( xpmi->colorTable )
	{ free( xpmi->colorTable );	}

    return;
    }

static int bmXpmAllocColors(	XpmImage *	xpmi,
				int		ncolors )
    {
    int		col;

    xpmi->colorTable= (XpmColor *)malloc( xpmi->ncolors* sizeof(XpmColor) );
    if  ( ! xpmi->colorTable )
	{ LXDEB(ncolors,xpmi->colorTable); return -1;	}

    for ( col= 0; col < ncolors; col++ )
	{
	xpmi->colorTable[col].string= (char *)0;
	xpmi->colorTable[col].symbolic= (char *)0;
	xpmi->colorTable[col].m_color= (char *)0;
	xpmi->colorTable[col].g4_color= (char *)0;
	xpmi->colorTable[col].g_color= (char *)0;
	xpmi->colorTable[col].c_color= (char *)0;
	}

    xpmi->ncolors= ncolors;

    return 0;
    }

static int bmXpmSetColor(	XpmColor *	xpmc,
				int		charactersPerPixel,
				char *		bytes,
				int		r,
				int		g,
				int		b )
    {
    xpmc->c_color= (char *)malloc( 14 );
    if  ( ! xpmc->c_color )
	{ XDEB(xpmc->c_color); return -1;	}

    xpmc->string= (char *)malloc( charactersPerPixel+ 1 );
    if  ( ! xpmc->string )
	{ XDEB(xpmc->string); return -1;	}

    sprintf( xpmc->c_color, "#%04x%04x%04x", 257* r, 257* g, 257* b );

    memcpy( xpmc->string, bytes, charactersPerPixel );
    xpmc->string[charactersPerPixel]= '\0';

    return 0;
    }

int bmWriteXpmFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				ret;
    unsigned int		col;
    int				row;
    XpmImage			image;

    int				charactersPerPixel= 1;

    unsigned int *		to;
    const unsigned char *	from;

    const char *		fn= utilMemoryBufferGetString( filename );

    const ColorPalette * 	cp= &(bd->bdPalette);

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE )
	{
	if  ( cp->cpColorCount >= sizeof(BMXPMBytes) )
	    { LLDEB(cp->cpColorCount,sizeof(BMXPMBytes)); return -1; }
	}
    else{
	if  ( bd->bdColorEncoding == BMcoBLACKWHITE	||
	      bd->bdColorEncoding == BMcoWHITEBLACK	)
	    {
	    if  ( bd->bdBitsPerPixel > 4 )
		{ LDEB(bd->bdBitsPerPixel); return -1; }
	    }
	else{ LDEB(bd->bdColorEncoding); return -1;	}
	}

    image.width= bd->bdPixelsWide;
    image.height= bd->bdPixelsHigh;
    image.cpp= 1;
    image.ncolors= 0;
    image.colorTable= (XpmColor *)0;
    image.data= (unsigned int *)0;

    switch( bd->bdColorEncoding )
	{
	unsigned int	col;

	case BMcoRGB8PALETTE:
	    image.cpp= charactersPerPixel;
	    image.ncolors= cp->cpColorCount;
	    ret= image.width* image.height* sizeof( unsigned int );
	    image.data= (unsigned int *)malloc( ret );
	    if  ( ! image.data )
		{ LXDEB(ret,image.data); return -1;	}

	    if  ( bmXpmAllocColors( &image, image.ncolors ) )
		{ LDEB(cp->cpColorCount); bmXpmClean( &image ); return -1; }

	    for ( col= 0; col < image.ncolors; col++ )
		{
		if  ( bmXpmSetColor( image.colorTable+ col, charactersPerPixel,
					BMXPMBytes+ col,
					cp->cpColors[col].rgb8Red,
					cp->cpColors[col].rgb8Green,
					cp->cpColors[col].rgb8Blue ) )
		    { XDEB(image.colorTable[col].c_color); return -1;	}
		}

	    break;
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    image.cpp= charactersPerPixel;
	    image.ncolors= 1 << bd->bdBitsPerPixel;
	    ret= image.width* image.height* sizeof( unsigned int );
	    image.data= (unsigned int *)
				    malloc( ret+ 7* sizeof( unsigned int ) );
	    if  ( ! image.data )
		{ LXDEB(ret,image.data); return -1;	}

	    if  ( bmXpmAllocColors( &image, image.ncolors ) )
		{ LDEB(image.ncolors); bmXpmClean( &image ); return -1; }

	    for ( col= 0; col < image.ncolors; col++ )
		{
		int	gray= ( 255* col )/ ( image.ncolors- 1 );

		if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
		    { gray= 255- gray;	}

		if  ( bmXpmSetColor( image.colorTable+ col, charactersPerPixel,
					BMXPMBytes+ col,
					gray, gray, gray ) )
		    { XDEB(image.colorTable[col].c_color); return -1;	}
		}

	    break;
	default:
	    LDEB(bd->bdColorEncoding); bmXpmClean( &image ); return -1;
	}

    switch( bd->bdBitsPerPixel )
	{
	case 1:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;
		to= image.data+ row* charactersPerPixel* image.width;

		for ( col= 0; col < bd->bdBytesPerRow; from++, col++ )
		    {
		    *(to++)= ( *from & 0x80 ) != 0;
		    *(to++)= ( *from & 0x40 ) != 0;
		    *(to++)= ( *from & 0x20 ) != 0;
		    *(to++)= ( *from & 0x10 ) != 0;
		    *(to++)= ( *from & 0x08 ) != 0;
		    *(to++)= ( *from & 0x04 ) != 0;
		    *(to++)= ( *from & 0x02 ) != 0;
		    *(to++)= ( *from & 0x01 ) != 0;
		    }
		}
	    break;
	case 2:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;
		to= image.data+ row* charactersPerPixel* image.width;

		for ( col= 0; col < bd->bdBytesPerRow; from++, col++ )
		    {
		    *(to++)= ( *from & 0xc0 ) >> 6;
		    *(to++)= ( *from & 0x30 ) >> 4;
		    *(to++)= ( *from & 0x0c ) >> 2;
		    *(to++)= ( *from & 0x03 );
		    }
		}
	    break;
	case 4:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;

		for ( col= 0; col < bd->bdPixelsWide- 1; from++, col += 2 )
		    {
		    *(to++)= ( *from & 0xf0 ) >> 4;
		    *(to++)= ( *from & 0x0f );
		    }

		if  ( col < bd->bdPixelsWide )
		    { *(to++)= ( *from & 0xf0 ) >> 4; }
		}
	    break;
	case 8:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;

		for ( col= 0; col < bd->bdPixelsWide; col++ )
		    { *(to++)= *(from++); }
		}
	    break;
	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}

    ret= XpmWriteFileFromXpmImage( (char *)fn, &image, (XpmInfo *)0 );
    if  ( ret != XpmSuccess )
	{ SSDEB(fn,XpmGetErrorString(ret)); ret= -1;	}
    else{ ret= 0;	}

    bmXpmClean( &image );

    return ret;
    }

int bmCanWriteXpmFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdColorEncoding == BMcoBLACKWHITE	||
	  bd->bdColorEncoding == BMcoWHITEBLACK	)
	{
	if  ( bd->bdBitsPerPixel <= 4 )
	    { return 0; }
	}

    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
	{ return -1;	}

    if  ( bd->bdPalette.cpColorCount >= sizeof(BMXPMBytes) )
	{ LLDEB(bd->bdPalette.cpColorCount,sizeof(BMXPMBytes)); return -1; }

    return 0;
    }

#   endif
