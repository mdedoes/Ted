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

# if 1
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Set the current color (to draw)					*/
/*									*/
/************************************************************************/

static int appMacPictSetColorX11(	MacpictDevice *		md,
					AppColors *		ac,
					RGB8Color *		rgb8 )
    {
    if  ( bmRGB8ColorsDiffer( &(md->mdColorSet), rgb8 ) )
	{
	AppDrawingData *	add= &(md->mdDrawingData);
	APP_COLOR_RGB		xc;

	appColorRgb( &xc, ac, rgb8->rgb8Red, rgb8->rgb8Green, rgb8->rgb8Blue );

	appDrawSetForegroundColor( add, &xc );

	md->mdColorSet= *rgb8;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the background of the pixmap.					*/
/*									*/
/************************************************************************/

static void appMacPictFillBackgroundX11(	MacpictDevice *	md )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    appDrawSetForegroundWhite( add );
    appDrawFillRectangle( add, 0, 0, md->mdOutWide, md->mdOutHigh );

    return;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect						*/
/*									*/
/*  1)  Initialise bitmap.						*/
/*  2)  Get bytes per row and palette flag.				*/
/*  3)  Get image frame.						*/
/*  4)  Default is a black and white image.				*/
/*  5)  For color images, get additional information.			*/
/*  6)  Retrieve palette.						*/
/*  7)  Finish bitmap administration and allocate buffer.		*/
/*  8)  Read source and destination rectangle.				*/
/*  9)  Read raster operation. (Assume copy anyway)			*/
/*  10) Read bitmap data.						*/
/*  11) Later versions of mac pict have an even number of bytes per	*/
/*	operation.							*/
/*  12) Draw image.							*/
/*  13) Ready: clean image related data.				*/
/*									*/
/************************************************************************/

static int appMacPict_PackBitsRectX11(	MacpictDevice *		md,
					AppColors *		ac,
					SimpleInputStream *	sis )
    {
    int			rval= 0;

    unsigned int	bytesPerRow;
    int			hasPalette= 0;

    int			y0Frame;
    int			x0Frame;
    int			y1Frame;
    int			x1Frame;

    int			y0Source;
    int			x0Source;
    int			y1Source;
    int			x1Source;

    int			y0Dest;
    int			x0Dest;
    int			y1Dest;
    int			x1Dest;

    int			rop;

    AppBitmapImage	abi;

    int			bytesRead= 0;

    /*  1  */
    appInitBitmapImage( &abi );

    /*  2  */
    bytesPerRow= sioEndianGetBeUint16( sis ); bytesRead += 2;
    if  ( bytesPerRow & 0x8000 )
	{
	hasPalette= 1;
	bytesPerRow &= ~0x8000;
	}

    /*  3  */
    y0Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Frame= sioEndianGetBeInt16( sis ); bytesRead += 2;

    abi.abiBitmap.bdPixelsWide= x1Frame- x0Frame;
    abi.abiBitmap.bdPixelsHigh= y1Frame- y0Frame;

    abi.abiBitmap.bdBytesPerRow= bytesPerRow;

    /*  4  */
    abi.abiBitmap.bdBitsPerPixel= 1;
    abi.abiBitmap.bdSamplesPerPixel= 1;
    abi.abiBitmap.bdBitsPerSample= 1;
    abi.abiBitmap.bdColorEncoding= BMcoBLACKWHITE;

    /*  5  */
    if  ( hasPalette )
	{
	int		version;
	int		packType;
	long		packSize;
	int		xRes;
	int		yRes;
	int		pixelType;
	int		planeBytes;
	int		table;
	int		reserved;

	version= sioEndianGetBeInt16( sis ); bytesRead += 2;
	packType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	packSize= sioEndianGetBeInt32( sis ); bytesRead += 4;
	xRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	yRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	pixelType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	abi.abiBitmap.bdBitsPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	abi.abiBitmap.bdSamplesPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	abi.abiBitmap.bdBitsPerSample= sioEndianGetBeInt16( sis ); bytesRead += 2;
	planeBytes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	table= sioEndianGetBeInt32( sis ); bytesRead += 4;
	reserved= sioEndianGetBeInt32( sis ); bytesRead += 4;

	if  ( abi.abiBitmap.bdSamplesPerPixel == 4 )
	    { abi.abiBitmap.bdHasAlpha= 1;	}

	version= version;
	packType= packType;
	packSize= packSize;
	xRes= xRes;
	yRes= yRes;
	pixelType= pixelType;
	planeBytes= planeBytes;
	table= table;
	reserved= reserved;
	}

    /*  6  */
    if  ( hasPalette )
	{
	long		seed;
	int		flags;
	int		colorCount;

	int		done;

	seed= sioEndianGetBeInt32( sis ); bytesRead += 4;
	flags= sioEndianGetBeUint16( sis ); bytesRead += 2;
	colorCount= sioEndianGetBeInt16( sis )+ 1; bytesRead += 2;


	abi.abiBitmap.bdColorEncoding= BMcoRGB8PALETTE;

	if  ( appMacPictReadPaletteColors( &(abi.abiBitmap), &done, md,
						    colorCount, flags, sis ) )
	    { LDEB(hasPalette); rval= -1; goto ready;	}

	bytesRead += done;
	}

    /*  7  */
    bmCalculateSizes( &(abi.abiBitmap) );

    abi.abiBuffer= malloc( abi.abiBitmap.bdBufferLength );
    if  ( ! abi.abiBuffer )
	{
	LXDEB(abi.abiBitmap.bdBufferLength,abi.abiBuffer);
	rval= -1; goto ready;
	}

    /*  8  */
    y0Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Source= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Source= sioEndianGetBeInt16( sis ); bytesRead += 2;

    y0Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x0Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    y1Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;
    x1Dest= sioEndianGetBeInt16( sis ); bytesRead += 2;

    /*  9  */
    rop= sioEndianGetBeInt16( sis ); bytesRead += 2;
    if  ( rop != 0 )
	{ LDEB(rop);	}

    /*  10  */
    if  ( bytesPerRow < 8 )
	{
	int		row;

	for ( row= 0; row < abi.abiBitmap.bdPixelsHigh; row++ )
	    {
	    unsigned char *	to;
	    int			i;

	    to= abi.abiBuffer+ row* abi.abiBitmap.bdBytesPerRow;

	    for ( i= 0; i < bytesPerRow; i++ )
		{ *(to++)= sioInGetCharacter( sis ); bytesRead++; }
	    }
	}
    else{
	int	row;

	for ( row= 0; row < abi.abiBitmap.bdPixelsHigh; row++ )
	    {
	    int		done;

	    if  ( appMacPictReadPacBitsRow(
			abi.abiBuffer+ row* abi.abiBitmap.bdBytesPerRow,
			&done, bytesPerRow, md, &(abi.abiBitmap), sis ) )
		{ LDEB(row); rval= -1; goto ready; }

	    bytesRead += done;
	    }
	}

    /*  11  */
    if  ( md->mdVersion > 1 && bytesRead % 2 )
	{ sioInGetCharacter( sis );	}

    /*  12  */
    {
    int			pixelsWide= x1Dest- x0Dest;
    int			pixelsHigh= y1Dest- y0Dest;

    int			srcX= x0Source;
    int			srcY= y0Source;
    int			dstX= MD_X( x0Dest, md );
    int			dstY= MD_Y( y0Dest, md );

    AppDrawingData *	add= &(md->mdDrawingData);
    APP_IMAGE *		xim;

    if  ( appImgMakeImage( add, &xim, pixelsWide, pixelsHigh, ac, &abi ) )
	{ LDEB(1); rval= -1; goto ready; }

#   ifdef USE_MOTIF
    XPutImage( add->addDisplay, add->addDrawable, add->addGc,
					    xim, srcX, srcY, dstX, dstY,
					    pixelsWide, pixelsHigh );
#   ifdef USE_MECHECK
    if  ( xim->data )
	{ free( xim->data ); xim->data= (char *)0; }
#   endif

    XDestroyImage( xim );
#   endif

#   ifdef USE_GTK
    gdk_draw_image( add->addDrawable, add->addGc,
					    xim, srcX, srcY, dstX, dstY,
					    pixelsWide, pixelsHigh );
    gdk_image_destroy( xim );
#   endif
    }

  ready:

    /*  13  */
    appCleanBitmapImage( &abi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Play a macpict metafile.						*/
/*									*/
/************************************************************************/

int appMacPictPlayFileX11( SimpleInputStream *	sis,
			void **			pPrivate,
			AppColors *		ac,
			AppDrawingData *	parent_add,
			APP_WINDOW		pixmap,
			int			mapMode,
			int			xWinExt,
			int			yWinExt,
			int			pixelsWide,
			int			pixelsHigh,
			int			twipsWide,
			int			twipsHigh )
    {
    int			c;
    int			i;

    int			rval= 0;

    double		magnification;
    double		pixelsPerTwip;

    MacpictDevice	md;


    appMacPictInitDeviceHeader( &md, pixelsWide, pixelsHigh );

    if  ( appMacPictGetDeviceHeader( &md, sis ) )
	{ LDEB(1); return -1;	}

    pixelsPerTwip= sqrt( (double)( pixelsWide* pixelsHigh )/
						( twipsWide* twipsHigh ) );
    magnification= pixelsPerTwip/ parent_add->addMagnifiedPixelsPerTwip;

    appCloneDrawingEnvironment( &(md.mdDrawingData),
			parent_add, magnification, pixelsPerTwip, pixmap );

    appMacPictFillBackgroundX11( &md );

    md.mdForeColor.rgb8Red= 255;
    md.mdForeColor.rgb8Green= 255;
    md.mdForeColor.rgb8Blue= 255;

    md.mdColorSet.rgb8Red= 0;
    md.mdColorSet.rgb8Green= 0;
    md.mdColorSet.rgb8Blue= 0;
    appDrawSetForegroundBlack( &(md.mdDrawingData) );

    for (;;)
	{
	int	opcode;
	int	bytes;

	if  ( appMacPictGetOpcode( &opcode, &bytes, &md, sis ) )
	    { LDEB(1); rval= -1; goto ready;	}

	switch( opcode )
	    {
	    case MACPICT_DefHilite:
		continue;

	    case MACPICT_Clip:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		for ( i= 2; i < regionBytes; i += 2 )
		    {
		    int		regionCoord;

		    regionCoord= sioEndianGetBeInt16( sis );
		    }
		}
		continue;

	    case MACPICT_PnMode:
		md.mdPenMode= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnMode(%d)\n", md.mdPenMode ));

		continue;

	    case MACPICT_RGBFgCol:
		if  ( appMacPictGetColor( &(md.mdForeColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBFgCol(%d,%d,%d)\n",
					md.mdForeColor.rgb8Red,
					md.mdForeColor.rgb8Green,
					md.mdForeColor.rgb8Blue ));

		continue;

	    case MACPICT_RGBBkCol:
		if  ( appMacPictGetColor( &(md.mdBackColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBBkCol(%d,%d,%d)\n",
					md.mdBackColor.rgb8Red,
					md.mdBackColor.rgb8Green,
					md.mdBackColor.rgb8Blue ));
		continue;

	    case MACPICT_PnPat:
		for ( i= 0; i < 8; i++ )
		    {
		    c= sioInGetCharacter( sis );
		    }

		PICTDEB(appDebug( "PnPat( ..)\n" ));

		continue;

	    case MACPICT_FillPat:

		PICTDEB(appDebug( "FillPat( ..)\n" ));

		for ( i= 0; i < 8; i++ )
		    {
		    c= sioInGetCharacter( sis );
		    }
		continue;

	    case MACPICT_frameOval:

		PICTDEB(appDebug( "FrameOval( ..)\n" ));

		for ( i= 0; i < 8; i++ )
		    {
		    c= sioInGetCharacter( sis );
		    }
		continue;

	    case MACPICT_PaintPoly:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			x0;
		int			y0;
		int			x1;
		int			y1;
		int			count;

		PICTDEB(appDebug( "PaintPoly(,%d)\n", count ));

		if  ( appMacPictGetCountAndPoints(
				    &md, &x0, &y0, &x1, &y1, &count, sis ) )
		    { rval= -1; goto ready;	}

		if  ( appMacPictSetColorX11( &md, ac, &(md.mdForeColor) ) )
		    { LDEB(1); rval= -1; goto ready;	}

		appDrawFillPolygon( add, md.mdPoints, count );

		appDrawDrawLines( add, md.mdPoints, count );
		}
		continue;

	    case MACPICT_LongComment:
		{
		int	kind= sioEndianGetBeInt16( sis );
		int	size= sioEndianGetBeInt16( sis );

		kind= kind;

		for ( i= 0; i < size; i++ )
		    { c= sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_PackBitsRect:
		PICTDEB(appDebug( "PackBitsRect( .. )\n" ));

		if  ( appMacPict_PackBitsRectX11( &md, ac, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		continue;

	    default:
		if  ( bytes >= 0 )
		    {
		    XLDEB(opcode,bytes);

		    for ( i= 0; i < bytes; i++ )
			{ c= sioInGetCharacter( sis );	}

		    continue;
		    }
		XDEB(opcode); rval= -1; goto ready;

	    case MACPICT_OpEndPic:
		PICTDEB(appDebug( "OpEndPic()\n" ));
		break;
	    }

	break;
	}

  ready:

    appMacPictCleanDeviceHeader( &md );

    return rval;
    }
