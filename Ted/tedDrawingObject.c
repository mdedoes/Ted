/************************************************************************/
/*									*/
/*  Ted, Draw a Word 95 style drawing object.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	"tedApp.h"
#   include	"docDraw.h"

#   include	<appWinMeta.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw a Word 95 style rectangle drawing object on a pixmap.		*/
/*									*/
/************************************************************************/

static void tedDrawRectSetBg(	RGB8Color *		bg,
				int			p,
				const RGB8Color *	fore,
				const RGB8Color *	back )
    {
    bg->rgb8Red= ( p* fore->rgb8Red+ (100- p)* back->rgb8Red )/ 100;
    bg->rgb8Green= ( p* fore->rgb8Green+ (100- p)* back->rgb8Green )/ 100;
    bg->rgb8Blue= ( p* fore->rgb8Blue+ (100- p)* back->rgb8Blue )/ 100;

    return;
    }

static int tedDrawGetFill(	RGB8Color *		bg,
				int *			pFill,
				int *			pHatch,
				const DrawingObject *	wdo )
    {
    int		fill= 1;
    int		hatch= 0;

    switch( wdo->doFillPattern )
	{
	case DOfillCLEAR:
	    bg->rgb8Red= 255;
	    bg->rgb8Green= 255;
	    bg->rgb8Blue= 255;
	    fill= 0;
	    break;

	case DOfillSOLID:
	    *bg= wdo->doFillBackColor;
	    break;

	case DOfillSHADE_5P:
	    tedDrawRectSetBg( bg, 5, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_10P:
	    tedDrawRectSetBg( bg, 10, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_20P:
	    tedDrawRectSetBg( bg, 20, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_25P:
	    tedDrawRectSetBg( bg, 25, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_30P:
	    tedDrawRectSetBg( bg, 30, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_40P:
	    tedDrawRectSetBg( bg, 40, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_50P:
	    tedDrawRectSetBg( bg, 50, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_60P:
	    tedDrawRectSetBg( bg, 60, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_70P:
	    tedDrawRectSetBg( bg, 70, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_75P:
	    tedDrawRectSetBg( bg, 75, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_80P:
	    tedDrawRectSetBg( bg, 80, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;
	case DOfillSHADE_90P:
	    tedDrawRectSetBg( bg, 90, &(wdo->doFillForeColor),
						&(wdo->doFillBackColor) );
	    break;

	case DOfillDKHORIZ:
	case DOfillDKVERT:
	case DOfillDKFDIAG:
	case DOfillDKBDIAG:
	case DOfillDKCROSS:
	case DOfillDKDCROSS:
	case DOfillHORIZ:
	case DOfillVERT:
	case DOfillFDIAG:
	case DOfillBDIAG:
	case DOfillCROSS:
	case DOfillDCROSS:
	    hatch= 1;
	    *bg= wdo->doFillBackColor;
	    break;

	default:
	    LDEB(wdo->doFillPattern);
	    return -1;
	}

    *pFill= fill;
    *pHatch= hatch;
    return 0;
    }

static int tedDrawSetLineStyle(	AppDrawingData *	add,
				int *			pBorder,
				int			solidStyle,
				int			pixelsWide,
				const DrawingObject *	wdo )
    {
    int		border= 0;

    switch( wdo->doLineStyle )
	{
	case DOlineSOLID:
	    appMetaX11SetWindowsLineStyle( add, &border,
			    solidStyle, wdo->doLineWidth, pixelsWide );
	    break;

	case DOlineHOLLOW:
	    break;

	case DOlineDASH:
	    appMetaX11SetWindowsLineStyle( add, &border,
			    PS_DASH, wdo->doLineWidth, pixelsWide );
	    break;

	case DOlineDOT:
	    appMetaX11SetWindowsLineStyle( add, &border,
			    PS_DOT, wdo->doLineWidth, pixelsWide );
	    break;

	case DOlineDADO:
	    appMetaX11SetWindowsLineStyle( add, &border,
			    PS_DASHDOT, wdo->doLineWidth, pixelsWide );
	    break;

	case DOlineDADODO:
	    appMetaX11SetWindowsLineStyle( add, &border,
			    PS_DASHDOTDOT, wdo->doLineWidth, pixelsWide );
	    break;

	default:
	    LDEB(wdo->doLineStyle); return -1;
	}

    *pBorder= border;
    return 0;
    }

static int tedDrawArcPixmap(
				APP_BITMAP_IMAGE		pixmap,
				const InsertedObject *		io,
				const DrawingObject *		wdo,
				double				pixelsPerTwip,
				AppColors *			ac,
				AppDrawingData *		add,
				int				x,
				int				y,
				int				wide,
				int				high,
				int				a0,
				int				aS )
    {
    RGB8Color		co;
    APP_COLOR_RGB	xc;

    int			fill= 1;
    int			hatch= 0;
    int			border= 0;
    int			pixelsWide;

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, 0, 0, io->ioPixelsWide, io->ioPixelsHigh );

    if  ( tedDrawGetFill( &co, &fill, &hatch, wdo ) )
	{ LDEB(1); return -1;	}

    if  ( fill )
	{
	if  ( appColorRgb( &xc, ac, co.rgb8Red, co.rgb8Green, co.rgb8Blue ) )
	    { LDEB(1); return -1;	}

	appDrawSetForegroundColor( add, &xc );

	appDrawFillArc( add, x, y, wide, high, 64* a0, 64* aS );
	}

    pixelsWide= pixelsPerTwip* wdo->doLineWidth;
    if  ( pixelsWide == 0 )
	{ pixelsWide= 1;	}

    if  ( tedDrawSetLineStyle( add, &border, PS_INSIDEFRAME, pixelsWide, wdo ) )
	{ LDEB(1); return -1;	}

    if  ( border )
	{
	if  ( appColorRgb( &xc, ac,
			    wdo->doLineColor.rgb8Red,
			    wdo->doLineColor.rgb8Green,
			    wdo->doLineColor.rgb8Blue ) )
	    { LDEB(1); return -1;	}

	appDrawSetForegroundColor( add, &xc );

	appDrawDrawArc( add, x, y, wide, high, 64* a0, 64* aS );
	}

    return 0;
    }

static int tedDrawRectanglePixmap(
				APP_BITMAP_IMAGE		pixmap,
				const InsertedObject *		io,
				const DrawingObject *		wdo,
				double				pixelsPerTwip,
				AppColors *			ac,
				AppDrawingData *		add )
    {
    RGB8Color		co;
    APP_COLOR_RGB	xc;

    int			fill= 0;
    int			hatch= 0;
    int			border= 0;
    int			pixelsWide;

    if  ( tedDrawGetFill( &co, &fill, &hatch, wdo ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &xc, ac, co.rgb8Red, co.rgb8Green, co.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    appDrawSetForegroundColor( add, &xc );

    appDrawFillRectangle( add, 0, 0, io->ioPixelsWide, io->ioPixelsHigh );

    pixelsWide= pixelsPerTwip* wdo->doLineWidth;
    if  ( pixelsWide == 0 )
	{ pixelsWide= 1;	}

    if  ( tedDrawSetLineStyle( add, &border, PS_INSIDEFRAME, pixelsWide, wdo ) )
	{ LDEB(1); return -1;	}

    if  ( border )
	{
	int		x0= pixelsWide/ 2;
	int		y0= pixelsWide/ 2;
	int		x1= io->ioPixelsWide- 1- pixelsWide/ 2;
	int		y1= io->ioPixelsHigh- 1- pixelsWide/ 2;

	if  ( appColorRgb( &xc, ac,
			    wdo->doLineColor.rgb8Red,
			    wdo->doLineColor.rgb8Green,
			    wdo->doLineColor.rgb8Blue ) )
	    { LDEB(1); return -1;	}

	appDrawSetForegroundColor( add, &xc );

	appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );
	}

    return 0;
    }

static int tedDrawPolygonPixmap(
				APP_BITMAP_IMAGE		pixmap,
				const InsertedObject *		io,
				const DrawingObject *		wdo,
				int				close,
				double				pixelsPerTwip,
				AppColors *			ac,
				AppDrawingData *		add )
    {
    int			rval= 0;

    RGB8Color		co;
    APP_COLOR_RGB	xc;

    int			fill= 0;
    int			hatch= 0;
    int			border= 0;
    int			pixelsWide;
    int			i;

    APP_POINT *		points= (APP_POINT *)0;

    points= malloc( ( wdo->doPointCount+ 1)* sizeof( APP_POINT ) );
    if  ( ! points )
	{ LXDEB(wdo->doPointCount,points); rval= -1; goto ready; }

    for ( i= 0; i < wdo->doPointCount; i++ )
	{
	points[i].x= pixelsPerTwip* wdo->doPoints[i].x;
	points[i].y= pixelsPerTwip* wdo->doPoints[i].y;
	}
    points[wdo->doPointCount]= points[0];

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, 0, 0, io->ioPixelsWide, io->ioPixelsHigh );

    if  ( close )
	{
	if  ( tedDrawGetFill( &co, &fill, &hatch, wdo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( fill )
	    {
	    if  ( appColorRgb( &xc, ac,
				co.rgb8Red, co.rgb8Green, co.rgb8Blue ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    appDrawSetForegroundColor( add, &xc );

	    appDrawFillPolygon( add, points, wdo->doPointCount+ 1 );
	    }
	}

    pixelsWide= pixelsPerTwip* wdo->doLineWidth;
    if  ( pixelsWide == 0 )
	{ pixelsWide= 1;	}

    if  ( tedDrawSetLineStyle( add, &border, PS_INSIDEFRAME, pixelsWide, wdo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( border )
	{
	if  ( appColorRgb( &xc, ac,
			    wdo->doLineColor.rgb8Red,
			    wdo->doLineColor.rgb8Green,
			    wdo->doLineColor.rgb8Blue ) )
	    { LDEB(1); rval= -1; goto ready;	}

	appDrawSetForegroundColor( add, &xc );

	appDrawDrawLines( add, points, wdo->doPointCount+ ( close != 0 ) );
	}

  ready:

    if  ( points )
	{ free( points );	}

    return rval;
    }


/************************************************************************/
/*									*/
/*  Draw a Word 95 style drawing object on a pixmap.			*/
/*									*/
/************************************************************************/

int tedDrawDrawingPixmap(	APP_BITMAP_IMAGE		pixmap,
				const InsertedObject *		io,
				AppColors *			ac,
				AppDrawingData *		parentAdd )
    {
    const DrawingObject *	wdo= &(io->ioDrawingObject);

    int				rval= 0;
    AppDrawingData		add;

    double			pixelsPerTwip;
    double			magnification;

    pixelsPerTwip= sqrt( (double)( io->ioPixelsWide* io->ioPixelsHigh )/
					( io->ioTwipsWide* io->ioTwipsHigh ) );
    magnification= pixelsPerTwip/ parentAdd->addMagnifiedPixelsPerTwip;

    appInitDrawingData( &add );

    appCloneDrawingEnvironment( &add, parentAdd,
				    magnification, pixelsPerTwip, pixmap );

    switch( wdo->doKind )
	{
	case DOkindRECT:
	    if  ( tedDrawRectanglePixmap( pixmap, io, wdo,
						pixelsPerTwip, ac, &add ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    break;

	case DOkindPOLYGON:
	    if  ( tedDrawPolygonPixmap( pixmap, io, wdo, 1,
						pixelsPerTwip, ac, &add ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    break;

	case DOkindPOLYLINE:
	    if  ( tedDrawPolygonPixmap( pixmap, io, wdo, 0,
						pixelsPerTwip, ac, &add ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    break;

	case DOkindLINE:
	    if  ( tedDrawPolygonPixmap( pixmap, io, wdo, 0,
						pixelsPerTwip, ac, &add ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    break;

	case DOkindARC:
	    {
	    int		a0= 0;
	    int		aS= 90;
	    int		x0= 0;
	    int		y0= 0;

	    if  ( wdo->doArcFlipX )
		{
		if  ( wdo->doArcFlipY )
		    {
		    x0= -io->ioPixelsWide;
		    a0= 0;
		    }
		else{
		    x0= -io->ioPixelsWide;
		    y0= -io->ioPixelsHigh;
		    a0= 270;
		    }
		}
	    else{
		if  ( wdo->doArcFlipY )
		    {
		    /* ! */
		    a0= 90;
		    }
		else{
		    y0= -io->ioPixelsHigh;
		    a0= 180;
		    }
		}

	    if  ( tedDrawArcPixmap( pixmap, io, wdo,
			    pixelsPerTwip, ac, &add,
			    x0, y0, 2* io->ioPixelsWide, 2* io->ioPixelsHigh,
			    a0, aS ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    }
	    break;

	case DOkindELLIPSE:
	    if  ( tedDrawArcPixmap( pixmap, io, wdo,
			    pixelsPerTwip, ac, &add,
			    0, 0, io->ioPixelsWide, io->ioPixelsHigh,
			    0, 360 ) )
		{ LDEB(wdo->doKind); rval= -1; goto ready;	}
	    break;

	default:
	    LDEB(wdo->doKind); rval= -1; goto ready;
	}

  ready:

    appCleanDrawingData( &add );

    return rval;
    }

