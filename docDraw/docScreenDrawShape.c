/************************************************************************/
/*									*/
/*  Screen specific shape drawing.					*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"docScreenDraw.h"
#   include	<layoutContext.h>
#   include	<geoGrid.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<docDrawShapeArrow.h>
#   include	<docShapePath.h>
#   include	<docLayout.h>
#   include	"docDraw.h"
#   include	<geo2DInteger.h>
#   include	<docShapeGeometry.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw a Word 97+ drawing shape.					*/
/*									*/
/************************************************************************/

static int docScreenDrawShapeArrowHead(	const LayoutContext *	lc,
					int			pixelsWide,
					const DrawShapeArrow *	dsa )
    {
    const ShapeArrow *	sa= &(dsa->dsaArrow);
    DrawingSurface	ds= lc->lcDrawingSurface;
    Point2DI		points[7];
    Arc2DI		arc;

    int			i;

    switch( sa->saArrowHead )
	{
	case DSarrowNONE:
	    break;

	case DSarrowARROW:
	case DSarrowSTEALTH_ARROW:
	case DSarrowDIAMOND:
	    for ( i= 0; i <= dsa->dsaPathLength; i++ )
		{
		points[i].x= dsa->dsaPath[i].x;
		points[i].y= dsa->dsaPath[i].y;
		}
	    drawFillPolygon( ds, points, dsa->dsaPathLength );
	    break;

	case DSarrowOPEN_ARROW:
	    for ( i= 0; i <= dsa->dsaPathLength; i++ )
		{
		points[i].x= dsa->dsaPath[i].x;
		points[i].y= dsa->dsaPath[i].y;
		}

	    drawSetLineAttributes( ds,
		    pixelsWide, LineStyleSolid, LineCapButt, LineJoinMiter,
		    (unsigned char *)0, 0 );

	    drawLines( ds, points, dsa->dsaPathLength, 0 );
	    break;

	case DSarrowOVAL:
	    arc.a2diRect.drX0= dsa->dsaArrowRectangle.x0;
	    arc.a2diRect.drY0= dsa->dsaArrowRectangle.y0;
	    arc.a2diRect.drX1= dsa->dsaArrowRectangle.x1;
	    arc.a2diRect.drY1= dsa->dsaArrowRectangle.y1;

	    geoNormalizeRectangle( &(arc.a2diRect), &(arc.a2diRect) );
	    arc.a2diAngleFrom= 64* 0;
	    arc.a2diAngleStep= 64* 360;

	    drawFillArc( ds, &arc );
	    break;

	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead); break;
	}

    return 0;
    }

static Point2DI * docScreenDrawGetVertices(
				const DocumentRectangle *	drTwips,
				const DrawingShape *		ds,
				const LayoutContext *		lc,
				ScreenDrawingData *		sdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    double			xfac= lc->lcPixelsPerTwip;
    int				x0Ref= drTwips->drX0;
    int				y0Ref= drTwips->drY0;

    int				i;
    const Point2DI *		sv;

    Point2DI *			xp;
    Point2DI *			xpret;

    xp= xpret= malloc( ( sd->sdVertexCount+ 1)* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(sd->sdVertexCount,xp); return (Point2DI *)0;	}

    sv= sd->sdVertices;
    for ( i= 0; i < sd->sdVertexCount; xp++, sv++, i++ )
	{
	xp->x= docLayoutXPixels( lc, x0Ref+ sv->x )- lc->lcOx;
	xp->y= COORDtoGRID( xfac, y0Ref+ sv->y )- lc->lcOy;
	}

    if  ( sd->sdVertexCount > 0 && xpret )
	{ xpret[sd->sdVertexCount]= xpret[0]; }

    return xpret;
    }

static void docScreenDrawShapeGetLine(
				int *				pLine,
				int *				pPixelsWide,
				int *				pTwipsWide,
				const DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    RGB8Color			rgb8Stroke;

    docDrawShapeGetLine( pLine, &rgb8Stroke, ds, dc, sdd );

    if  ( *pLine )
	{
	const LayoutContext *	lc= dc->dcLayoutContext;
	int			twipsWide= EMUtoTWIPS( sd->sdLineWidthEmu );
	int			pixelsWide= docLayoutXPixels( lc, twipsWide );

	static unsigned char	dash[]= { 3, 2 };
	static unsigned char	dot[]= { 1, 2 };
	static unsigned char	dashdot[]= { 2, 2, 1, 2 };
	static unsigned char	dashdotdot[]= { 2, 2, 1, 2, 1, 2 };

	const unsigned char *	dashList= (const unsigned char *)0;
	int			dashCount= 0;
	int			lineStyle= LineStyleSolid;
	int			capStyle= LineCapButt;
	int			joinStyle= LineJoinMiter;

	if  ( pixelsWide < 1 )
	    { pixelsWide=  1;	}

	switch( sd->sdLineDashing )
	    {
	    case DSdashSOLID:
		dashList= (const unsigned char *)0;
		dashCount= 0;
		lineStyle= LineStyleSolid;
		capStyle= LineCapButt;
		joinStyle= LineJoinMiter;
		break;

	    case DSdashDASHED:
	    case DSdashDASHED_X:
	    case DSdashDASHED_L:
		dashList= dash;
		dashCount= sizeof( dash );
		lineStyle= LineStyleSingleDash;
		capStyle= LineCapButt;
		joinStyle= LineJoinMiter;
		break;

	    case DSdashDOT:
	    case DSdashDOT_X:
		dashList= dot;
		dashCount= sizeof( dot );
		lineStyle= LineStyleSingleDash;
		capStyle= LineCapButt;
		joinStyle= LineJoinMiter;
		break;

	    case DSdashDASHDOT:
	    case DSdashDASHDOT_X:
	    case DSdashDASHDOT_L:
		dashList= dashdot;
		dashCount= sizeof( dashdot );
		lineStyle= LineStyleSingleDash;
		capStyle= LineCapButt;
		joinStyle= LineJoinMiter;
		break;

	    case DSdashDASHDOTDOT:
	    case DSdashDASHDOTDOT_L:
		dashList= dashdotdot;
		dashCount= sizeof( dashdotdot );
		lineStyle= LineStyleSingleDash;
		capStyle= LineCapButt;
		joinStyle= LineJoinMiter;
		break;

	    default:
		LDEB(sd->sdLineDashing);
		dashList= (const unsigned char *)0;
		dashCount= 0;
	    }

	drawSetLineAttributes( lc->lcDrawingSurface,
		    pixelsWide, lineStyle, capStyle, joinStyle,
		    dashList, dashCount );

	if  ( pPixelsWide )
	    { *pPixelsWide= pixelsWide;	}
	if  ( pTwipsWide )
	    { *pTwipsWide= twipsWide;		}
	}
    }

static int docScreenDrawShapePoints(	const DrawingShape *		ds,
				Point2DI *			xp,
				int				np,
				int				closed,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= dc->dcLayoutContext;
    double			xfac= lc->lcPixelsPerTwip;

    int				line= 0;
    int				pixelsWide;
    int				twipsWide;

    int				drawStartArrow= 0;
    DrawShapeArrow		dsaStart;
    Point2DD			shaftStart[2];
    int				drawEndArrow= 0;
    DrawShapeArrow		dsaEnd;
    Point2DD			shaftEnd[2];

    docScreenDrawShapeGetLine( &line, &pixelsWide, &twipsWide, ds, dc, sdd );

    if  ( line && ! closed && np >= 2 )
	{
	if  ( sd->sdLineHeadArrow.saArrowHead != DSarrowNONE )
	    {
	    drawStartArrow= 1;

	    shaftStart[0].x= xp[1].x;
	    shaftStart[0].y= xp[1].y;
	    shaftStart[1].x= xp[0].x;
	    shaftStart[1].y= xp[0].y;

	    docShapeArrowSizesTwips( &dsaStart, twipsWide, xfac,
					shaftStart, &(sd->sdLineHeadArrow) );
	    }

	if  ( sd->sdLineTailArrow.saArrowHead != DSarrowNONE )
	    {
	    drawEndArrow= 1;

	    shaftEnd[0].x= xp[np-2].x;
	    shaftEnd[0].y= xp[np-2].y;
	    shaftEnd[1].x= xp[np-1].x;
	    shaftEnd[1].y= xp[np-1].y;

	    docShapeArrowSizesTwips( &dsaEnd, twipsWide, xfac,
					shaftEnd, &(sd->sdLineTailArrow) );
	    }
	}

    if  ( closed )
	{
	int		fill= 0;
	RGB8Color	rgb8Fill;

	docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
	if  ( fill )
	    { drawFillPolygon( lc->lcDrawingSurface, xp, np ); }
	}

    if  ( line )
	{
	if  ( drawStartArrow )
	    {
	    xp[0].x= dsaStart.dsaShaft[1].x;
	    xp[0].y= dsaStart.dsaShaft[1].y;
	    }

	if  ( drawEndArrow )
	    {
	    xp[np-1].x= dsaEnd.dsaShaft[1].x;
	    xp[np-1].y= dsaEnd.dsaShaft[1].y;
	    }

	drawLines( lc->lcDrawingSurface, xp, np, closed );
	}

    if  ( drawStartArrow )
	{ docScreenDrawShapeArrowHead( lc, pixelsWide, &dsaStart );	}
    if  ( drawEndArrow )
	{ docScreenDrawShapeArrowHead( lc, pixelsWide, &dsaEnd );	}

    return 0;
    }

static void docScreenDrawSetShapePath(
				Point2DI *			xp,
				const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    int				np= sp->spVertexCount;
    const Point2DI *		sv= sp->spVertices;
    int				i;

    if  ( sd->sdRotation == 0 )
	{
	int			xs= sp->spXSize;
	int			ys= sp->spYSize;

	int			x0= dr->drX0;
	int			x1= dr->drX1;
	int			y0= dr->drY0;
	int			y1= dr->drY1;

	if  ( DSflipHORIZONTAL( ds ) )
	    { int swap= x0; x0= x1; x1= swap;	}
	if  ( DSflipVERTICAL( ds ) )
	    { int swap= y0; y0= y1; y1= swap;	}

	for ( i= 0; i < np; sv++, i++ )
	    {
	    xp[i].x= ( ( xs- sv->x )* x0+ sv->x* x1 )/ xs;
	    xp[i].y= ( ( ys- sv->y )* y0+ sv->y* y1 )/ ys;
	    }
	}
    else{
	AffineTransform2D	at;
	double			xs= sp->spXSize;
	double			ys= sp->spYSize;
	double			xm= ( dr->drX1+ dr->drX0 )/ 2.0;
	double			ym= ( dr->drY1+ dr->drY0 )/ 2.0;

	docShapeInternalTransform( &at, ds );

	for ( i= 0; i < np; sv++, i++ )
	    {
	    double	x;
	    double	y;

	    x= ( ( xs- sv->x )* dr->drX0+ sv->x* dr->drX1 )/ xs -xm;
	    y= ( ( ys- sv->y )* dr->drY0+ sv->y* dr->drY1 )/ ys- ym;

	    xp[i].x= AT2_X( x, y, &at )+ xm;
	    xp[i].y= AT2_Y( x, y, &at )+ ym;
	    }
	}

    xp[np]= xp[0];

    return;
    }

static int docScreenDrawShapePath(	const DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels,
				const ShapePath *		sp )
    {
    int			rval= 0;
    int			np= sp->spVertexCount;
    Point2DI *		xp;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    docScreenDrawSetShapePath( xp, ds, sp, drPixels );

    if  ( docScreenDrawShapePoints( ds, xp, np, sp->spClosed, dc, sdd ) )
	{ LDEB(np); rval= -1;	}

    free( xp );

    return rval;
    }

static int docScreenDrawPictureFrame( DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp;

    int				fill= 0;
    int				line= 0;
    int				pixelsWide= 0;

    RGB8Color			rgb8Fill;
    DocumentRectangle		drSrc;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    docScreenDrawSetShapePath( xp, ds, sp, drPixels );

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{ drawFillPolygon( lc->lcDrawingSurface, xp, np ); }

    drSrc= *drPixels;
    geoShiftRectangle( &drSrc, -drPixels->drX0, -drPixels->drY0 );

    drawChildSurface( lc->lcDrawingSurface, ds->dsDrawingSurface,
					    drPixels->drX0, drPixels->drY0,
					    &drSrc );

    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
    if  ( line )
	{ drawLines( lc->lcDrawingSurface, xp, np+ 1, 0 ); }

    free( xp );

    return 0;
    }

static int docScreenDrawOnlineStorage( DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp;
    Arc2DI			arc;

    int				fill= 0;
    int				line= 0;
    int				pixelsWide= 0;

    int				wide;
    int				rx0;
    int				rx1;

    RGB8Color			rgb8Fill;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    docScreenDrawSetShapePath( xp, ds, sp, drPixels );

    wide= drPixels->drX1- drPixels->drX0;
    /*
    high= drPixels->drY1- drPixels->drY0;
    */
    rx0= ( 5* drPixels->drX0+ 1* drPixels->drX1 )/ 6;
    rx1= ( 1* drPixels->drX0+ 5* drPixels->drX1 )/ 6;

    arc.a2diRect= *drPixels;
    arc.a2diRect.drX1= drPixels->drX0+ ( wide+ 2 )/ 3;
    arc.a2diAngleFrom= 64* 90;
    arc.a2diAngleStep= 64* 180;

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{
	DocumentRectangle	drFill= *drPixels;

	drawFillArc( drsf, &arc );

	drFill.drX0= rx0;
	drFill.drX1= rx1;
	drawFillRectangle( drsf, &drFill );
	}

    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
    if  ( line )
	{
	drawArc( drsf, &arc );

	drawLine( drsf, rx0, drPixels->drY0, rx1, drPixels->drY0 );
	drawLine( drsf, rx0, drPixels->drY1, rx1, drPixels->drY1 );

	drawArc( drsf, &arc );
	}

    free( xp );

    return 0;
    }

static int docScreenDrawCan( 	DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;

    Arc2DI			topArc;
    Arc2DI			botArc;

    int				fill= 0;
    int				line= 0;
    int				pixelsWide= 0;

    int				ry0;
    int				ry1;

    RGB8Color			rgb8Fill;

    ry0= ( 5* drPixels->drY0+ 1* drPixels->drY1 )/ 6;
    ry1= ( 1* drPixels->drY0+ 5* drPixels->drY1 )/ 6;

    topArc.a2diRect= *drPixels;
    topArc.a2diRect.drY0= drPixels->drY0;
    topArc.a2diRect.drY1= ( 2* drPixels->drY0+ 1* drPixels->drY1 )/ 3;
    topArc.a2diAngleFrom= 64* 0;
    topArc.a2diAngleStep= 64* 360;

    botArc.a2diRect= *drPixels;
    botArc.a2diRect.drY0= ( 1* drPixels->drY0+ 2* drPixels->drY1 )/ 3;
    botArc.a2diRect.drY1= drPixels->drY1;
    botArc.a2diAngleFrom= 64* 180;
    botArc.a2diAngleStep= 64* 180;

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{
	DocumentRectangle	drFill= *drPixels;

	drawFillArc( drsf, &topArc );

	drFill.drY0= ry0;
	drFill.drY1= ry1;
	drawFillRectangle( drsf, &drFill );

	drawFillArc( drsf, &botArc );
	}

    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
    if  ( line )
	{
	drawArc( drsf, &topArc );

	drawLine( drsf, drPixels->drX0, ry0, drPixels->drX0, ry1 );
	drawLine( drsf, drPixels->drX1, ry0, drPixels->drX1, ry1 );

	drawArc( drsf, &botArc );
	}

    return 0;
    }


static int docScreenDrawCallout( DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *	sp= &SP_RECTANGLE;
    int			np= sp->spVertexCount;
    Point2DI *		xp;

    int			fill= 0;
    int			line= 0;
    int			pixelsWide= 0;

    RGB8Color		rgb8Fill;

    if  ( np != 4 )
	{ LDEB(np); return -1;	}

    xp= malloc( ( np+ 1+ 2+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    docScreenDrawSetShapePath( xp, ds, sp, drPixels );

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{ drawFillPolygon( drsf, xp, np ); }

    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
    if  ( line )
	{
	int		pp0= 5;
	int		pp1= pp0;
	long		a0, a2;

	const int	SC= 21600; /* adjust to geoLeft etc? */

	a2= sd->sdAdjustValue; a0= SC- a2;
	xp[pp1].x= ( a0* xp[0].x+ a2* xp[2].x )/ SC;
	a2= sd->sdAdjust2Value; a0= SC- a2;
	xp[pp1].y= ( a0* xp[0].y+ a2* xp[2].y )/ SC;
	pp1++;

	a2= sd->sdAdjust3Value; a0= SC- a2;
	xp[pp1].x= ( a0* xp[0].x+ a2* xp[2].x )/ SC;
	a2= sd->sdAdjust4Value; a0= SC- a2;
	xp[pp1].y= ( a0* xp[0].y+ a2* xp[2].y )/ SC;
	pp1++;

	drawLines( drsf, xp+ pp0, pp1- pp0, 0 );
	}

    free( xp );

    return 0;
    }

static int docScreenDrawRoundedRectangleShape(
				const DocumentRectangle *	drPixels,
				DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;

    int			fill= 0;
    int			line= 0;
    int			pixelsWide;
    RGB8Color		rgb8Fill;

    int			r;

    r= docShapeGetRoundedRectRadius( drPixels, &(ds->dsDrawing) );
    if  ( r < 0 )
	{ LDEB(r); return -1;	}

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{ drawFillRoundedRect( drsf, drPixels, r, r );	}
    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
    if  ( line )
	{ drawRoundedRect( drsf, drPixels, r, r );	}

    return 0;
    }

static int docScreenDrawRightBrace( DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    int				rval= 0;

    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp= (Point2DI *)0;

    int				line= 0;

    int				pixelsWide;
    int				wide;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); rval= -1; goto ready;	}

    docScreenDrawSetShapePath( xp, ds, sp, drPixels );

    wide= xp[2].x- xp[0].x;
    /*
    high= xp[2].y- xp[0].y;
    */

    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );

# if 0
    appDrawDrawArc( add, xp[0].x- wide/ 2,
			    ( 8* xp[0].y+ 0* xp[2].y )/ 8,
			    wide+ 1, ( high+ 3 )/ 4,
			    64* 0, 64* 90 );
# endif

    drawLine( drsf,
	    xp[0].x+ wide/ 2, ( 7* xp[0].y+ 1* xp[2].y )/ 8,
	    xp[0].x+ wide/ 2, ( 5* xp[0].y+ 3* xp[2].y )/ 8 );

# if 0
    appDrawDrawArc( add, xp[2].x- wide/ 2,
			    ( 6* xp[0].y+ 2* xp[2].y )/ 8,
			    wide+ 1, ( high+ 3 )/ 4,
			    64* 180, 64* 90 );

    appDrawDrawArc( add, xp[2].x- wide/ 2,
			    ( 4* xp[0].y+ 4* xp[2].y )/ 8,
			    wide+ 1, ( high+ 3 )/ 4,
			    64* 90, 64* 90 );
# endif

    drawLine( drsf,
	    xp[0].x+ wide/ 2, ( 3* xp[0].y+ 5* xp[2].y )/ 8,
	    xp[0].x+ wide/ 2, ( 1* xp[0].y+ 7* xp[2].y )/ 8 );

# if 0
    appDrawDrawArc( add, xp[0].x- wide/ 2,
			    ( 2* xp[0].y+ 6* xp[2].y )/ 8,
			    wide+ 1, ( high+ 3 )/ 4,
			    64* 270, 64* 90 );
# endif

  ready:

    if  ( xp )
	{ free( xp );	}

    return rval;
    }

static int docScreenDrawFreeForm(
				const DocumentRectangle *	drOutside,
				DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				rval= 0;
    Point2DI *			xp= (Point2DI *)0;

    const int			closed= 1;

    xp= docScreenDrawGetVertices( drOutside, ds, lc, sdd );
    if  ( ! xp )
	{ XDEB(xp); rval= -1; goto ready;	}

    docScreenDrawShapePoints( ds, xp, sd->sdVertexCount- 1,
					closed, dc, sdd );
  ready:
    if  ( xp )
	{ free( xp );	}

    return rval;
    }

int docScreenDrawDrawingShape(	const DocumentRectangle *	drOutside,
				const AffineTransform2D *	atOutside,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= dc->dcLayoutContext;
    DrawingSurface		drsf= lc->lcDrawingSurface;
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;

    int				rval= 0;

    DocumentRectangle		drPixels;
    DocumentRectangle		drNorm;
    int				fill= 0;
    int				line= 0;
    int				pixelsWide= 0;

    docGetPixelRect( &drPixels, lc, drOutside, page );
    geoNormalizeRectangle( &drNorm, &drPixels );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    &drNorm, dc->dcClipRect )	)
	{ return 0;	}

    geoShiftRectangle( &drPixels, -lc->lcOx, -lc->lcOy );

    switch( sd->sdShapeType )
	{
	case SHPtyGROUP:
	    /* Done by docDrawDrawingShape(). */
	    LSDEB(sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;

	case 33:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_33 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_34 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    if  ( docScreenDrawPictureFrame( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCALLOUT_1:
	case SHPtyCALLOUT_90:
	    if  ( docScreenDrawCallout( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
							    &SP_RECTANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyRIGHT_BRACKET:
	case SHPtyLEFT_BRACKET:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    docScreenDrawRoundedRectangleShape( &drPixels, ds, dc, sdd );
	    break;

	case SHPtyELLIPSE:
	case SHPtyFLOW_CHART_CONNECTOR:
	    {
	    RGB8Color		rgb8Fill;
	    Arc2DI		arc;

	    arc.a2diRect= drPixels;
	    arc.a2diAngleFrom= 64* 0;
	    arc.a2diAngleStep= 64* 360;

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
	    if  ( fill )
		{ drawFillArc( drsf, &arc );	}

	    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
	    if  ( line )
		{ drawArc( drsf, &arc );	}
	    }
	    break;

	case SHPtyARC:
	    {
	    RGB8Color		rgb8Fill;
	    Arc2DI		arc;

	    arc.a2diRect= drPixels;
	    arc.a2diAngleFrom= 64* 0;
	    arc.a2diAngleStep= 64* 90;

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
	    if  ( fill )
		{ drawFillArc( drsf, &arc );	}

	    docScreenDrawShapeGetLine( &line, &pixelsWide, (int *)0, ds, dc, sdd );
	    if  ( line )
		{ drawArc( drsf, &arc );	}
	    }
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_DIAMOND ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
						    &SP_ISOSCELES_TRIANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_FLOW_CHART_MERGE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_RIGHT_TRIANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_PARALLELOGRAM ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_TRAPEZOID ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_HEXAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_OCTAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_PLUS_SIGN ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_NOTCHED_RIGHT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_HOME_PLATE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_CHEVRON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_RIGHT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_UP_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_DOWN_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_RIGHT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_UP_DOWN_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_QUAD_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_RIGHT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_DOWN_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_UP_DOWN_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_QUAD_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_RIGHT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_LEFT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_BENT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_PENTAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_STAR ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_SEAL4 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels, &SP_LINE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_FLOW_CHART_MANUAL_INPUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_FLOW_CHART_OFF_PAGE_CONNECTOR ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_FLOW_CHART_PUNCHED_CARD ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( docScreenDrawShapePath( ds, dc, sdd, &drPixels,
					&SP_WEDGE_RECT_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_ONLINE_STORAGE:
	    if  ( docScreenDrawOnlineStorage( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCAN:
	case SHPtyFLOW_CHART_MAGNETIC_DISK:
	    if  ( docScreenDrawCan( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_BRACE:
	    if  ( docScreenDrawRightBrace( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( sd->sdVertexCount > 1 )
		{
		if  ( docScreenDrawFreeForm( drOutside, ds, dc, sdd, &drPixels ) )
		    { LDEB(1); rval= -1;	}
		}
	    break;

	case SHPtyBENT_ARROW:
	default:
	    LSDEB(sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsstruct DocumentTree.dtRoot )
	{}
    */

    return rval;
    }

