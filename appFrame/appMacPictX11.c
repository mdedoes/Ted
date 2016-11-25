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

# if 0
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
/*  Set a (bitmap) pattern to draw with.				*/
/*									*/
/*  1)  Nothing to be done, but not for solid.				*/
/*  2)  Discard previous pattern.					*/
/*  3)  Solid pattern.							*/
/*  4)  1=BLACK= (foreground), 0=WHITE= (background)			*/
/*  5)  Make pixmap.							*/
/*									*/
/************************************************************************/

static int appMacPictSetStipple(	MacpictDevice *		md,
					AppColors *		ac,
					unsigned char		pattern[8],
					int			which )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    /*  1  */
    if  ( which != STIPPLE_SOLID && md->mdStippleSet == which )
	{ return 0;	}

    /*  2  */
    if  ( md->mdTilePixmap )
	{
	appDrawFreePixmap( add, md->mdTilePixmap );
	md->mdTilePixmap= (APP_BITMAP_IMAGE)0;
	}

    /*  3  */
    if  ( which == STIPPLE_SOLID )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( add->addGc, GDK_SOLID );
#	endif

	/*  4  */
	if  ( pattern[0] )
	    {
	    if  ( appMacPictSetColorX11( md, ac, &(md->mdForeColor) ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( appMacPictSetColorX11( md, ac, &(md->mdBackColor) ) )
		{ LDEB(1); return -1;	}
	    }

	md->mdStippleSet= which;
	return 0;
	}

    /*  1  */
    if  ( md->mdStippleSet == which )
	{ return 0;	}

    /*  5  */
    {
    AppBitmapImage	abi;

    const int		dstXExt= 8;
    const int		dstYExt= 8;

    appInitBitmapImage( &abi );
    abi.abiBuffer= pattern;

    abi.abiBitmap.bdPixelsWide= 8;
    abi.abiBitmap.bdPixelsHigh= 8;
    abi.abiBitmap.bdColorEncoding= BMcoBLACKWHITE;
    abi.abiBitmap.bdBitsPerSample= 1;
    abi.abiBitmap.bdSamplesPerPixel= 1;
    abi.abiBitmap.bdBitsPerPixel= 1;

    abi.abiBitmap.bdXResolution= 1;
    abi.abiBitmap.bdYResolution= 1;
    abi.abiBitmap.bdUnit= BMunPIXEL;

    bmCalculateSizes( &(abi.abiBitmap) );

    if  ( appImgMakePixmap( add, &(md->mdTilePixmap),
					dstXExt, dstYExt, ac, &abi ) )
	{ LDEB(1); return -1;	}

    abi.abiBuffer= (unsigned char *)0;
    appCleanBitmapImage( &abi );
    }

#   ifdef USE_MOTIF
    XSetTile( add->addDisplay, add->addGc, md->mdTilePixmap );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_tile( add->addGc, md->mdTilePixmap );
#   endif

#   ifdef USE_MOTIF
    XSetFillStyle( add->addDisplay, add->addGc, FillTiled );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( add->addGc, GDK_TILED );
#   endif

    md->mdStippleSet= which;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Use the pen/fill pattern for drawing.				*/
/*									*/
/************************************************************************/

static int appMacPictSetPen(		MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdPenIsSolid )
	{
	if  ( appMacPictSetStipple( md, ac, md->mdPenPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipple( md, ac, md->mdPenPattern, STIPPLE_PEN ) )
	    { LDEB(STIPPLE_PEN); return -1;	}
	}

    return 0;
    }

static int appMacPictSetFill(		MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdFillIsSolid )
	{
	if  ( appMacPictSetStipple( md, ac, md->mdFillPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipple( md, ac, md->mdFillPattern, STIPPLE_FILL ) )
	    { LDEB(STIPPLE_FILL); return -1;	}
	}

    return 0;
    }

static int appMacPictSetBack(		MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdBackIsSolid )
	{
	if  ( appMacPictSetStipple( md, ac, md->mdBackPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipple( md, ac, md->mdBackPattern, STIPPLE_BACK ) )
	    { LDEB(STIPPLE_BACK); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

static int appMacPictDrawStringX11(	MacpictDevice *		md,
					AppColors *		ac,
					int			x,
					int			y,
					int			count )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    while( count > 0 && isspace( md->mdTextString[count-1] ) )
	{ count--;	}

    if  ( count == 0 )
	{ return 0;	}

    x= MD_X( x, md );
    y= MD_Y( y, md );

    appDrawDrawString( add, x, y, (char *)md->mdTextString, count );

    return 0;
    }

/************************************************************************/
/*									*/
/*  FrameRect/PaintRect variants.					*/
/*									*/
/************************************************************************/

static int appMacPictFrameRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictPaintRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictFillRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetFill( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictEraseRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetBack( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintPoly/FramePoly variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintPoly(		MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillPolygon( add, md->mdPolyPoints, md->mdPolyPointCount );

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawLines( add, md->mdPolyPoints, md->mdPolyPointCount );

    return 0;
    }

static int appMacPictFramePoly(		MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawLines( add, md->mdPolyPoints, md->mdPolyPointCount );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintOval/FrameOval variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintOval(		MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int x0= MD_X( md->mdOvalX0, md );
    int y0= MD_Y( md->mdOvalY0, md );
    int x1= MD_X( md->mdOvalX1, md );
    int y1= MD_Y( md->mdOvalY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    return 0;
    }

static int appMacPictFrameOval(		MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int x0= MD_X( md->mdOvalX0, md );
    int y0= MD_Y( md->mdOvalY0, md );
    int x1= MD_X( md->mdOvalX1, md );
    int y1= MD_Y( md->mdOvalY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPen( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    return 0;
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

static int appMacPict_BitsRectX11(	MacpictDevice *		md,
					AppColors *		ac,
					SimpleInputStream *	sis,
					int			packed )
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
    if  ( ! packed || bytesPerRow < 8 )
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
	{ (void)sioInGetCharacter( sis );	}

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

    pixelsWide= MD_W( pixelsWide, md );
    pixelsHigh= MD_H( pixelsHigh, md );

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

    PICTDEB(appDebug( "MACPICT( %d .. %d x %d .. %d -> %d x %d )\n",
		    md.mdInX0, md.mdInX1, md.mdInY0, md.mdInY1,
		    md.mdOutWide, md.mdOutHigh ));

    pixelsPerTwip= sqrt( (double)( pixelsWide* pixelsHigh )/
						( twipsWide* twipsHigh ) );
    magnification= pixelsPerTwip/ parent_add->addMagnifiedPixelsPerTwip;

    appCloneDrawingEnvironment( &(md.mdDrawingData),
			parent_add, magnification, pixelsPerTwip, pixmap );

    appMacPictFillBackgroundX11( &md );

    md.mdBackColor.rgb8Red= 255;
    md.mdBackColor.rgb8Green= 255;
    md.mdBackColor.rgb8Blue= 255;

    md.mdForeColor.rgb8Red= 0;
    md.mdForeColor.rgb8Green= 0;
    md.mdForeColor.rgb8Blue= 0;

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
		PICTDEB(appDebug( "DefHilite()\n" ));
		continue;

	    case MACPICT_Clip:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "Clip( %d )\n", regionBytes/ 2 ));

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

	    case MACPICT_PnLocHFrac:
		{
		int	val;

		val= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnLocHFrac(%d)\n", val ));

		}
		continue;

	    case MACPICT_RGBFgCol:
		if  ( appMacPictGetColor( &(md.mdForeColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBFgCol(%d,%d,%d)\n",
					md.mdForeColor.rgb8Red,
					md.mdForeColor.rgb8Green,
					md.mdForeColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_RGBBkCol:
		if  ( appMacPictGetColor( &(md.mdBackColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBBkCol(%d,%d,%d)\n",
					md.mdBackColor.rgb8Red,
					md.mdBackColor.rgb8Green,
					md.mdBackColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_PnPat:
		md.mdPenPattern[0]= sioInGetCharacter( sis );
		md.mdPenIsSolid=
			( md.mdPenPattern[0] == 0x00 ||
			  md.mdPenPattern[0] == 0xff );
		for ( i= 1; i < 8; i++ )
		    {
		    md.mdPenPattern[i]= sioInGetCharacter( sis );

		    if  ( md.mdPenPattern[i] != md.mdPenPattern[0] )
			{ md.mdPenIsSolid= 0;	}
		    }

		if  ( md.mdStippleSet == STIPPLE_PEN )
		    { md.mdStippleSet=   STIPPLE_UNDEF;	}

		PICTDEB(appDebug(
			"PnPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdPenPattern[0],
					    md.mdPenPattern[1],
					    md.mdPenPattern[2],
					    md.mdPenPattern[3],
					    md.mdPenPattern[4],
					    md.mdPenPattern[4],
					    md.mdPenPattern[6],
					    md.mdPenPattern[7] ));

		continue;

	    case MACPICT_FillPat:

		md.mdFillPattern[0]= sioInGetCharacter( sis );
		md.mdFillIsSolid=
			( md.mdFillPattern[0] == 0x00 ||
			  md.mdFillPattern[0] == 0xff );
		for ( i= 1; i < 8; i++ )
		    {
		    md.mdFillPattern[i]= sioInGetCharacter( sis );

		    if  ( md.mdFillPattern[i] != md.mdFillPattern[0] )
			{ md.mdFillIsSolid= 0;	}
		    }

		if  ( md.mdStippleSet == STIPPLE_FILL )
		    { md.mdStippleSet=   STIPPLE_UNDEF;	}

		PICTDEB(appDebug(
			"FillPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdFillPattern[0],
					    md.mdFillPattern[1],
					    md.mdFillPattern[2],
					    md.mdFillPattern[3],
					    md.mdFillPattern[4],
					    md.mdFillPattern[4],
					    md.mdFillPattern[6],
					    md.mdFillPattern[7] ));

		continue;

	    case MACPICT_BkPat:

		md.mdBackPattern[0]= sioInGetCharacter( sis );
		md.mdBackIsSolid=
			( md.mdBackPattern[0] == 0x00 ||
			  md.mdBackPattern[0] == 0xff );
		for ( i= 1; i < 8; i++ )
		    {
		    md.mdBackPattern[i]= sioInGetCharacter( sis );

		    if  ( md.mdBackPattern[i] != md.mdBackPattern[0] )
			{ md.mdBackIsSolid= 0;	}
		    }

		if  ( md.mdStippleSet == STIPPLE_BACK )
		    { md.mdStippleSet=   STIPPLE_UNDEF;	}

		PICTDEB(appDebug(
			"BkPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdBackPattern[0],
					    md.mdBackPattern[1],
					    md.mdBackPattern[2],
					    md.mdBackPattern[3],
					    md.mdBackPattern[4],
					    md.mdBackPattern[4],
					    md.mdBackPattern[6],
					    md.mdBackPattern[7] ));

		continue;

	    case MACPICT_PnSize:
		{
		int			h;
		int			w;

		h= sioEndianGetBeInt16( sis );
		w= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnSize( %d, %d )\n", w, h ));
		}

		continue;

	    case MACPICT_TxRatio:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		y0= sioEndianGetBeInt16( sis );
		x0= sioEndianGetBeInt16( sis );
		y1= sioEndianGetBeInt16( sis );
		x1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxRatio( %d, %d, %d, %d )\n",
							x0, y0, x1, y1 ));
		}
		continue;

	    case MACPICT_FrameOval:
		{
		md.mdOvalY0= sioEndianGetBeInt16( sis );
		md.mdOvalX0= sioEndianGetBeInt16( sis );
		md.mdOvalY1= sioEndianGetBeInt16( sis );
		md.mdOvalX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FrameOval( %d, %d, %d, %d )\n",
						md.mdOvalX0, md.mdOvalY0,
						md.mdOvalX1, md.mdOvalY1 ));

		if  ( appMacPictFrameOval( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameOval:
		{
		PICTDEB(appDebug( "FrameSameOval()\n" ));

		if  ( appMacPictFrameOval( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintOval:
		{
		md.mdOvalY0= sioEndianGetBeInt16( sis );
		md.mdOvalX0= sioEndianGetBeInt16( sis );
		md.mdOvalY1= sioEndianGetBeInt16( sis );
		md.mdOvalX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PaintOval( %d, %d, %d, %d )\n",
						md.mdOvalX0, md.mdOvalY0,
						md.mdOvalX1, md.mdOvalY1 ));

		if  ( appMacPictPaintOval( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameOval:
		{
		PICTDEB(appDebug( "PaintSameOval()\n" ));

		if  ( appMacPictPaintOval( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintPoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintPoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictPaintPoly( &md, ac ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_FramePoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "FramePoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictFramePoly( &md, ac ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_ShortLine:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			y0= sioEndianGetBeInt16( sis );
		int			x0= sioEndianGetBeInt16( sis );
		int			y1;
		int			x1;
		int			c;

		c= sioInGetCharacter( sis );
		if  ( c & 0x80 )
		    { x1= x0+ c- 256;	}
		else{ x1= x0+ c;	}

		c= sioInGetCharacter( sis );
		if  ( c & 0x80 )
		    { y1= y0+ c- 256;	}
		else{ y1= y0+ c;	}


		PICTDEB(appDebug( "ShortLine( %d+ %d, %d+ %d )\n",
						x0, x1- x0, y0, y1- y0 ));

		if  ( x1 != x0 || y1 != y0 )
		    {
		    /*  !  */
		    md.mdPenX= x1;
		    md.mdPenY= y1;

		    if  ( appMacPictSetPen( &md, ac ) )
			{ LDEB(1); return -1;	}

		    x0= MD_X( x0, &md );
		    y0= MD_Y( y0, &md );
		    x1= MD_X( x1, &md );
		    y1= MD_Y( y1, &md );
		    appDrawDrawLine( add, x0, y0, x1, y1 );
		    }
		}
		continue;

	    case MACPICT_ShortLineFrom:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			y0= md.mdPenY;
		int			x0= md.mdPenX;
		int			y1;
		int			x1;
		int			c;

		c= sioInGetCharacter( sis );
		if  ( c & 0x80 )
		    { x1= x0+ c- 256;	}
		else{ x1= x0+ c;	}

		c= sioInGetCharacter( sis );
		if  ( c & 0x80 )
		    { y1= y0+ c- 256;	}
		else{ y1= y0+ c;	}

		PICTDEB(appDebug( "ShortLineFrom( %d+ %d, %d+ %d )\n",
						x0, x1- x0, y0, y1- y0 ));

		if  ( appMacPictSetPen( &md, ac ) )
		    { LDEB(1); return -1;	}

		x0= MD_X( x0, &md );
		y0= MD_Y( y0, &md );
		x1= MD_X( x1, &md );
		y1= MD_Y( y1, &md );
		appDrawDrawLine( add, x0, y0, x1, y1 );
		}
		continue;

	    case MACPICT_LineFrom:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			y0= sioEndianGetBeInt16( sis );
		int			x0= sioEndianGetBeInt16( sis );
		int			y1= md.mdPenY;
		int			x1= md.mdPenX;

		PICTDEB(appDebug( "LineFrom( %d, %d )\n", x0, y0 ));

		if  ( appMacPictSetPen( &md, ac ) )
		    { LDEB(1); return -1;	}

		x0= MD_X( x0, &md );
		y0= MD_Y( y0, &md );
		x1= MD_X( x1, &md );
		y1= MD_Y( y1, &md );
		appDrawDrawLine( add, x0, y0, md.mdPenX, md.mdPenY );
		}
		continue;

	    case MACPICT_FrameRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FrameRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictFrameRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameRect:
		{
		PICTDEB(appDebug( "FrameSameRect()\n" ));

		if  ( appMacPictFrameRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PaintRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictPaintRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameRect:
		{
		PICTDEB(appDebug( "PaintSameRect()\n" ));

		if  ( appMacPictPaintRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FillRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictFillRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillSameRect:
		{
		PICTDEB(appDebug( "FillSameRect()\n" ));

		if  ( appMacPictFillRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "EraseRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictEraseRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseSameRect:
		{
		PICTDEB(appDebug( "EraseSameRect()\n" ));

		if  ( appMacPictEraseRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameArc:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			y0= sioEndianGetBeInt16( sis );
		int			x0= sioEndianGetBeInt16( sis );
		int			y1= sioEndianGetBeInt16( sis );
		int			x1= sioEndianGetBeInt16( sis );

		int			a0= sioEndianGetBeInt16( sis );
		int			a1= sioEndianGetBeInt16( sis );

		int			swap;

		PICTDEB(appDebug( "FrameArc( %d, %d, %d, %d, %d, %d )\n",
						x0, y0, x1, y1, a0, a1 ));

		if  ( appMacPictSetPen( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}

		x0= MD_X( x0, &md );
		y0= MD_Y( y0, &md );
		x1= MD_X( x1, &md );
		y1= MD_Y( y1, &md );

		if  ( x1 < x0 )
		    { swap= x0; x0= x1; x1= swap; }
		if  ( y1 < y0 )
		    { swap= y0; y0= y1; y1= swap; }

		appDrawDrawArc( add, x0, y0, x1- x0, y1- y0,
						    64* a0, 64* ( a1- a0 ) );
		}
		continue;

	    case MACPICT_Line:
		{
		AppDrawingData *	add= &(md.mdDrawingData);
		int			y0= sioEndianGetBeInt16( sis );
		int			x0= sioEndianGetBeInt16( sis );
		int			y1= sioEndianGetBeInt16( sis );
		int			x1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "Line( %d, %d, %d, %d )\n",
						x0, y0, x1, y1 ));

		if  ( appMacPictSetPen( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}

		md.mdPenX= x1;
		md.mdPenY= y1;

		x0= MD_X( x0, &md );
		y0= MD_Y( y0, &md );
		x1= MD_X( x1, &md );
		y1= MD_Y( y1, &md );
		appDrawDrawLine( add, x0, y0, x1, y1 );
		}
		continue;

	    case MACPICT_FontName:
		{
		int	dataLength= sioEndianGetBeInt16( sis );
		int	oldFontId= sioEndianGetBeInt16( sis );
		int	nameLength= sioInGetCharacter( sis );

		int	i;
		char *	fresh;

		oldFontId= oldFontId;

		fresh= realloc( md.mdFontName, nameLength+ 1 );
		if  ( ! fresh )
		    { LXDEB(nameLength,fresh); rval= -1; goto ready;	}
		md.mdFontName= fresh;

		for ( i= 0; i < nameLength; fresh++, i++ )
		    { *fresh= sioInGetCharacter( sis ); }
		*fresh= '\0';

		PICTDEB(appDebug( "FontName( %d, %d, %d, \"%s\" )\n",
			dataLength, oldFontId, nameLength, md.mdFontName ));

		for ( i= 3+ nameLength; i < dataLength; i++ )
		    { (void) sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_TxFont:
		{
		int	val= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxFont( %d )\n", val ));

		val= val;
		}
		continue;

	    case MACPICT_TxFace:
		{
		int	val= sioInGetCharacter( sis );

		PICTDEB(appDebug( "TxFace( %d )\n", val ));

		val= val;

		if  ( md.mdVersion > 1 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_TxSize:
		{
		md.mdFontSizePoints= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxSize( %d )\n", md.mdFontSizePoints ));
		}
		continue;

	    case MACPICT_GlyphState:
		{
		int	dataLength= sioEndianGetBeInt16( sis );
		int	i;
		int	c;

		int	outlinePreferred= 0;
		int	preserveGlyph= 0;
		int	fractionalWidth= 0;
		int	scalingDisabled= 0;

		for ( i= 0; i < dataLength; i++ )
		    {
		    c= sioInGetCharacter( sis );

		    if  ( i == 0 )
			{ outlinePreferred= c;	}
		    if  ( i == 1 )
			{ preserveGlyph= c;	}
		    if  ( i == 2 )
			{ fractionalWidth= c;	}
		    if  ( i == 3 )
			{ scalingDisabled= c;	}
		    }

		PICTDEB(appDebug( "GlyphState( %d:%x,%x,%x,%x )\n",
					    dataLength,
					    outlinePreferred,
					    preserveGlyph,
					    fractionalWidth,
					    scalingDisabled ));

		md.mdPenX= 0;
		md.mdPenY= 0;
		}
		continue;

	    case MACPICT_DHText:
		{
		int	dh= sioInGetCharacter( sis );
		int	count;

		if  ( appMacPictGetCountAndString( &md, &count, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "DHText( %d, %d: \"%s\" )\n",
					    dh, count, md.mdTextString ));

		md.mdPenX += dh;

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}

		if  ( md.mdVersion > 1 && count % 2 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_DVText:
		{
		int			dv= sioInGetCharacter( sis );
		int			count;

		if  ( appMacPictGetCountAndString( &md, &count, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "DVText( %d, %d: \"%s\" )\n",
					    dv, count, md.mdTextString ));

		md.mdPenY += dv;

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}

		if  ( md.mdVersion > 1 && count % 2 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_DHDVText:
		{
		int	dh= sioInGetCharacter( sis );
		int	dv= sioInGetCharacter( sis );
		int	count;

		if  ( appMacPictGetCountAndString( &md, &count, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "DHDVText( %d, %d, %d: \"%s\" )\n",
					    dh, dv, count, md.mdTextString ));

		md.mdPenX += dh;
		md.mdPenY += dv;

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}

		if  ( md.mdVersion > 1 && count % 2 == 0 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_LongText:
		{
		int	y= sioEndianGetBeInt16( sis );
		int	x= sioEndianGetBeInt16( sis );
		int	count;

		if  ( appMacPictGetCountAndString( &md, &count, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "LongText( %d, %d, %d: \"%s\" )\n",
					    x, y, count, md.mdTextString ));

		md.mdPenX= x;
		md.mdPenY= y;

		if  ( appMacPictDrawStringX11( &md, ac, x, y, count ) )
		    { LDEB(count); rval= -1; goto ready;	}

		if  ( md.mdVersion > 1 && count % 2 == 0 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_LongComment:
		{
		int	kind= sioEndianGetBeInt16( sis );
		int	size= sioEndianGetBeInt16( sis );

		kind= kind;

		PICTDEB(appDebug( "LongComment( %d, %d, .. )\n", kind, size ));

		for ( i= 0; i < size; i++ )
		    { c= sioInGetCharacter( sis );	}

		if  ( md.mdVersion > 1 && size % 2 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_ShortComment:
		{
		int	val= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "ShortComment( %d )\n", val ));

		val= val;
		}
		continue;

	    case MACPICT_BitsRect:
		{
		const int	packed= 0;

		PICTDEB(appDebug( "BitsRect( .. )\n" ));

		if  ( appMacPict_BitsRectX11( &md, ac, sis, packed ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRect:
		{
		const int	packed= 1;

		PICTDEB(appDebug( "PackBitsRect( .. )\n" ));

		if  ( appMacPict_BitsRectX11( &md, ac, sis, packed ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
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
