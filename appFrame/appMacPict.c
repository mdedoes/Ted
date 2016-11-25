#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMacPict.h>
#   include	<utilPs.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Initialize/Clean a mac pict device.					*/
/*									*/
/************************************************************************/

void appMacPictInitDeviceHeader(	MacpictDevice *		md,
					int			outWide,
					int			outHigh )
    {
    md->mdVersion= 0;

    md->mdInX0= 0;
    md->mdInY0= 0;
    md->mdInX1= 0;
    md->mdInY1= 0;

    md->mdOutWide= outWide;
    md->mdOutHigh= outHigh;

    md->mdPenMode= PENMODE_patOr;

    bmInitRGB8Color( &(md->mdForeColor) );
    bmInitRGB8Color( &(md->mdBackColor) );
    bmInitRGB8Color( &(md->mdColorSet) );

    md->mdPoints= (APP_POINT *)0;

    appInitDrawingData( &(md->mdDrawingData) );

    return;
    }

void appMacPictCleanDeviceHeader(	MacpictDevice *		md )
    {
    if  ( md->mdPoints )
	{ free( md->mdPoints ); }

    appCleanDrawingData( &(md->mdDrawingData) );

    return;
    }

/************************************************************************/
/*									*/
/*  Read the header of a macpict metafile.				*/
/*									*/
/*  2)  Read picture size.						*/
/*  3)  Read picture frame.						*/
/*  4)  Read version.							*/
/*									*/
/************************************************************************/

int appMacPictGetDeviceHeader(	MacpictDevice *		md,
				SimpleInputStream *	sis )
    {
    int			pictureSize;
    int			version;

    int			x0;
    int			y0;
    int			x1;
    int			y1;

    int			c;
    int			i;
    long		l;

    /*  2  */
    pictureSize= sioEndianGetBeInt16( sis );

    /*  3  */
    y0= sioEndianGetBeInt16( sis );
    x0= sioEndianGetBeInt16( sis );
    y1= sioEndianGetBeInt16( sis );
    x1= sioEndianGetBeInt16( sis );

    /*  4  */
    c= sioInGetCharacter( sis );
    switch( c )
	{
	case 0:
	    c= sioInGetCharacter( sis );
	    if  ( c != 0x11 )
		{ XDEB(c); return -1;	}

	    version= sioInGetCharacter( sis );
	    if  ( version != 2 )
		{ LDEB(version); return -1;	}

	    c= sioInGetCharacter( sis );
	    if  ( c != 0xff )
		{ XDEB(c); return -1;	}

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0xc00 )
		{ XDEB(c); return -1;	}

	    l= sioEndianGetBeInt32( sis );
	    /*
	    if  ( l != -1 )
		{ LDEB(l); return -1;	}
	    */

	    for ( i= 0; i < 8; i++ )
		{ c= sioEndianGetBeInt16( sis ); }

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); return -1;	}
	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); return -1;	}
	    break;

	case 0x11:
	    version= sioInGetCharacter( sis );
	    if  ( version != 1 )
		{ LDEB(version); return -1;	}
	    break;

	default:
	    XDEB(c); return -1;
	}


    md->mdVersion= version;

    md->mdInX0= x0;
    md->mdInY0= y0;
    md->mdInX1= x1;
    md->mdInY1= y1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the next opcode from a macpict metafile.			*/
/*									*/
/************************************************************************/

int appMacPictGetOpcode(	int *			pOpcode,
				int *			pBytes,
				const MacpictDevice *	md,
				SimpleInputStream *	sis )
    {
    int		bytes= -1;
    int		opcode;

    switch( md->mdVersion )
	{
	case 1:
	    opcode= sioInGetCharacter( sis );
	    break;
	case 2:
	    opcode= sioEndianGetBeInt16( sis );
	    break;
	default:
	    LDEB(md->mdVersion); return -1;
	}

    if  ( opcode == EOF )
	{ LDEB(opcode); return -1;	}

    bytes= -1;
    if  ( opcode >= 0x100 && opcode <= 0x7fff )
	{
	bytes= 2* ( ( opcode >> 8 ) & 0xff );
	opcode= opcode & 0xff;
	}


    *pBytes= bytes;
    *pOpcode= opcode;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a color.							*/
/*									*/
/************************************************************************/

int appMacPictGetColor(	RGB8Color *		rgb8,
			const MacpictDevice *	md,
			SimpleInputStream *	sis )
    {
    int		r= sioEndianGetBeInt16( sis );
    int		g= sioEndianGetBeInt16( sis );
    int		b= sioEndianGetBeInt16( sis );

    rgb8->rgb8Red= r/ 256;
    rgb8->rgb8Green= g/ 256;
    rgb8->rgb8Blue= b/ 256;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the points of a polyline/polygon.				*/
/*									*/
/************************************************************************/

int appMacPictGetCountAndPoints(	MacpictDevice *		md,
					int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					int *			pCount,
					SimpleInputStream *	sis )
    {
    int			bytes;
    int			count;

    APP_POINT *		xp;
    int			done;

    bytes= sioEndianGetBeInt16( sis );
    bytes -= 2;
    bytes -= 8;

    if  ( bytes % 4 || bytes <= 4 )
	{ LDEB(bytes); return -1;	}

    count= bytes/ 4;

    xp= (APP_POINT *)realloc( md->mdPoints, (count+ 1)* sizeof(APP_POINT) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}
    md->mdPoints= xp;

    *pY0= sioEndianGetBeInt16( sis );
    *pX0= sioEndianGetBeInt16( sis );
    *pY1= sioEndianGetBeInt16( sis );
    *pX1= sioEndianGetBeInt16( sis );

    for ( done= 0; done < count; xp++, done++ )
	{
	int	y= sioEndianGetBeInt16( sis );
	int	x= sioEndianGetBeInt16( sis );

	xp->x= MD_X( x, md );
	xp->y= MD_Y( y, md );
	}

    *xp= md->mdPoints[0];

    *pCount= count; return 0;
    }

/************************************************************************/
/*									*/
/*  Read the colors in a mac pict palette.				*/
/*									*/
/************************************************************************/

int appMacPictReadPaletteColors(	BitmapDescription *	bd,
					int *			pBytesRead,
					const MacpictDevice *	md,
					int			colorCount,
					int			flags,
					SimpleInputStream *	sis )
    {
    int		col;

    RGB8Color *	rgb8;

    int		bytesRead= 0;

    rgb8= realloc( bd->bdRGB8Palette, colorCount* sizeof( RGB8Color ) );
    if  ( ! rgb8 )
	{ LXDEB(colorCount,bd->bdRGB8Palette); return -1;	}
    bd->bdRGB8Palette= rgb8;

    for ( col= 0; col < colorCount; rgb8++, col++ )
	{
	int	idx;

	idx= sioEndianGetBeInt16( sis ); bytesRead += 2;
	if  ( idx < 0 )
	    { LDEB(idx); idx= 0;			}
	if  ( idx >= colorCount )
	    { LLDEB(idx,colorCount); idx= colorCount- 1; }

	if  ( flags & 0x8000 )
	    { idx= col;	}

	rgb8->rgb8Red= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Green= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Blue= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	}

    *pBytesRead= bytesRead;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read one row of PackBits data.					*/
/*									*/
/************************************************************************/

int appMacPictReadPacBitsRow(	unsigned char *			to,
				int *				pBytesRead,
				int				bytesPerRow,
				const MacpictDevice *		md,
				const BitmapDescription *	bd,
				SimpleInputStream *		sis )
    {
    int			byteCount;
    int			done= 0;
    int			bytesRead= 0;

    if  ( bytesPerRow > 250 )
	{ byteCount= sioEndianGetBeInt16( sis ); bytesRead += 2;	}
    else{ byteCount= sioInGetCharacter( sis ); bytesRead++;		}

    if  ( byteCount > bd->bdBytesPerRow )
	{ LLDEB(byteCount,bd->bdBytesPerRow); return -1;	}

    bytesRead += byteCount;

    while( byteCount > 0 )
	{
	int	c0= sioInGetCharacter( sis );
	int	l;

	if  ( c0 == EOF )
	    { LDEB(c0); return -1;	}

	if  ( c0 == 0x80 )
	    { XDEB(c0); byteCount--; continue;	}

	if  ( c0 & 0x80 )
	    {
	    int		c1= sioInGetCharacter( sis );

	    l= 257- c0;

	    if  ( done+ l > bd->bdBytesPerRow )
		{ LLLDEB(done,l,bd->bdBytesPerRow); return -1; }

	    byteCount -= 2; done += l;

	    while( l > 0 )
		{ *(to++)= c1; l--; }
	    }
	else{
	    l= c0+ 1;

	    if  ( done+ l > bd->bdBytesPerRow )
		{ LLLDEB(done,l,bd->bdBytesPerRow); return -1; }

	    byteCount -= 1+ l; done += l;

	    while( l > 0 )
		{ *(to++)= sioInGetCharacter( sis ); l--; }
	    }
	}

    *pBytesRead= bytesRead;
    return 0;
    }
