/************************************************************************/
/*									*/
/*  Ted: screen specific shape drawing.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<docScreenLayout.h>
#   include	"tedDraw.h"
#   include	<geoGrid.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw a Word 97+ drawing shape.					*/
/*									*/
/************************************************************************/

static int tedShapeDrawArrowHead(	const LayoutContext *	lc,
					int			xTo,
					int			yTo,
					int			xFrom,
					int			yFrom,
					int			widthPixels,
					const ShapeArrow *	sa )
    {
    DrawingSurface	ds= lc->lcDrawingSurface;
    double		xfac= lc->lcPixelsPerTwip;
    AffineTransform2D	at;
    AffineTransform2D	att;
    Point2DI		points[5];
    Arc2DI		arc;

    int			length;
    int			length2;
    int			width;

    docShapeArrowSizesTwips( &length, &length2, &width, sa );

    length= docLayoutXPixels( lc, length );
    length2= docLayoutXPixels( lc, length2 );
    width= COORDtoGRID( xfac, width );

    geoRotationAffineTransform2DAtan( &at, yTo- yFrom, xTo- xFrom );
    geoTranslationAffineTransform2D( &att, xTo, yTo );
    geoAffineTransform2DProduct( &at, &att, &at );

    points[0].x= xTo;
    points[0].y= yTo;

    switch( sa->saArrowHead )
	{
	case DSarrowNONE:
	    return 0;
	case DSarrowARROW:
	    points[1].x= AT2_X( -length,  width, &at );
	    points[1].y= AT2_Y( -length,  width, &at );
	    points[2].x= AT2_X( -length, -width, &at );
	    points[2].y= AT2_Y( -length, -width, &at );
	    points[3]= points[0];
	    drawFillPolygon( ds, points, 3 );
	    break;

	case DSarrowSTEALTH_ARROW:
	    points[1].x= AT2_X( -length2,  width, &at );
	    points[1].y= AT2_Y( -length2,  width, &at );
	    points[2].x= AT2_X( -length,  0, &at );
	    points[2].y= AT2_Y( -length,  0, &at );
	    points[3].x= AT2_X( -length2, -width, &at );
	    points[3].y= AT2_Y( -length2, -width, &at );
	    points[4]= points[0];
	    drawFillPolygon( ds, points, 4 );
	    break;

	case DSarrowDIAMOND:
	    points[1].x= AT2_X( -length/ 2,  width, &at );
	    points[1].y= AT2_Y( -length/ 2,  width, &at );
	    points[2].x= AT2_X( -length,  0, &at );
	    points[2].y= AT2_Y( -length,  0, &at );
	    points[3].x= AT2_X( -length/ 2, -width, &at );
	    points[3].y= AT2_Y( -length/ 2, -width, &at );
	    points[4]= points[0];
	    drawFillPolygon( ds, points, 4 );
	    break;

	case DSarrowOPEN_ARROW:
	    points[0].x= AT2_X( -length,  width, &at );
	    points[0].y= AT2_Y( -length,  width, &at );
	    points[1].x= AT2_X( 0, 0, &at );
	    points[1].y= AT2_Y( 0, 0, &at );
	    points[2].x= AT2_X( -length, -width, &at );
	    points[2].y= AT2_Y( -length, -width, &at );

	    drawSetLineAttributes( ds,
		    widthPixels, LineStyleSolid, LineCapButt, LineJoinMiter,
		    (unsigned char *)0, 0 );

	    drawLines( ds, points, 3, 0 );
	    break;

	case DSarrowOVAL:
	    arc.a2diRect.drX0= AT2_X( -length/ 2,  width, &at );
	    arc.a2diRect.drY0= AT2_Y( -length/ 2,  width, &at );
	    arc.a2diRect.drX1= AT2_X(  length/ 2, -width, &at );
	    arc.a2diRect.drY1= AT2_Y(  length/ 2, -width, &at );

	    geoNormalizeRectangle( &(arc.a2diRect), &(arc.a2diRect) );
	    arc.a2diAngleFrom= 64* 0;
	    arc.a2diAngleStep= 64* 360;

	    drawFillArc( ds, &arc );
	    break;

	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead); return 0;
	}

    return 0;
    }

static Point2DI * tedDrawGetVertices(
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
    const ShapeVertex *		sv;

    Point2DI *			xp;
    Point2DI *			xpret;

    xp= xpret= malloc( ( sd->sdVertexCount+ 1)* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(sd->sdVertexCount,xp); return (Point2DI *)0;	}

    sv= sd->sdVertices;
    for ( i= 0; i < sd->sdVertexCount; xp++, sv++, i++ )
	{
	xp->x= docLayoutXPixels( lc, x0Ref+ sv->svX )- lc->lcOx;
	xp->y= COORDtoGRID( xfac, y0Ref+ sv->svY )- lc->lcOy;
	}

    if  ( sd->sdVertexCount > 0 && xpret )
	{ xpret[sd->sdVertexCount]= xpret[0]; }

    return xpret;
    }

static void tedShapeGetLine(	int *				pLine,
				int *				pWidthPixels,
				const DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    RGB8Color			rgb8Stroke;

    docDrawShapeGetLine( pLine, &rgb8Stroke, ds, dc, sdd );

    if  ( *pLine )
	{
	const LayoutContext *	lc= &(dc->dcLayoutContext);
	int			widthTwips= EMUtoTWIPS( sd->sdLineWidthEmu );
	int			widthPixels= docLayoutXPixels( lc, widthTwips );

	static unsigned char	dash[]= { 3, 2 };
	static unsigned char	dot[]= { 1, 2 };
	static unsigned char	dashdot[]= { 2, 2, 1, 2 };
	static unsigned char	dashdotdot[]= { 2, 2, 1, 2, 1, 2 };

	const unsigned char *	dashList= (const unsigned char *)0;
	int			dashCount= 0;
	int			lineStyle= LineStyleSolid;
	int			capStyle= LineCapButt;
	int			joinStyle= LineJoinMiter;

	if  ( widthPixels < 1 )
	    { widthPixels=  1;	}

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
		    widthPixels, lineStyle, capStyle, joinStyle,
		    dashList, dashCount );

	*pWidthPixels= widthPixels;
	}
    }

static int tedShapeDrawPoints(	const DrawingShape *		ds,
				const Point2DI *		xp,
				int				np,
				int				closed,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    int				line= 0;
    int				widthPixels;

    if  ( closed )
	{
	int		fill= 0;
	RGB8Color	rgb8Fill;

	docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
	if  ( fill )
	    { drawFillPolygon( lc->lcDrawingSurface, xp, np ); }
	}

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
    if  ( line )
	{ drawLines( lc->lcDrawingSurface, xp, np, closed ); }

    if  ( line && ! closed && np >= 2 )
	{
	if  ( sd->sdLineStartArrow.saArrowHead != DSarrowNONE )
	    {
	    tedShapeDrawArrowHead( lc, xp[0].x, xp[0].y,
					xp[1].x, xp[1].y,
					widthPixels,
					&(sd->sdLineStartArrow) );
	    }
	if  ( sd->sdLineEndArrow.saArrowHead != DSarrowNONE )
	    {
	    tedShapeDrawArrowHead( lc, xp[np-1].x, xp[np-1].y,
					xp[np-2].x, xp[np-2].y,
					widthPixels,
					&(sd->sdLineEndArrow) );
	    }
	}

    return 0;
    }

static void tedDrawSetShapePath(Point2DI *			xp,
				const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    int				np= sp->spVertexCount;
    const ShapeVertex *		sv= sp->spVertices;
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
	    xp[i].x= ( ( xs- sv->svX )* x0+ sv->svX* x1 )/ xs;
	    xp[i].y= ( ( ys- sv->svY )* y0+ sv->svY* y1 )/ ys;
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

	    x= ( ( xs- sv->svX )* dr->drX0+ sv->svX* dr->drX1 )/ xs -xm;
	    y= ( ( ys- sv->svY )* dr->drY0+ sv->svY* dr->drY1 )/ ys- ym;

	    xp[i].x= AT2_X( x, y, &at )+ xm;
	    xp[i].y= AT2_Y( x, y, &at )+ ym;
	    }
	}

    xp[np]= xp[0];

    return;
    }

static int tedDrawShapePath(	const DrawingShape *		ds,
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

    tedDrawSetShapePath( xp, ds, sp, drPixels );

    if  ( tedShapeDrawPoints( ds, xp, np, sp->spClosed, dc, sdd ) )
	{ LDEB(np); rval= -1;	}

    free( xp );

    return rval;
    }

static int tedDrawPictureFrame( DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp;

    int				fill= 0;
    int				line= 0;
    int				widthPixels= 0;

    RGB8Color			rgb8Fill;
    DocumentRectangle		drSrc;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    tedDrawSetShapePath( xp, ds, sp, drPixels );

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{ drawFillPolygon( lc->lcDrawingSurface, xp, np ); }

    drSrc= *drPixels;
    geoShiftRectangle( &drSrc, -drPixels->drX0, -drPixels->drY0 );

    drawChildSurface( lc->lcDrawingSurface, ds->dsDrawingSurface,
					    drPixels->drX0, drPixels->drY0,
					    &drSrc );

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
    if  ( line )
	{ drawLines( lc->lcDrawingSurface, xp, np+ 1, 0 ); }

    free( xp );

    return 0;
    }

static int tedDrawOnlineStorage( DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp;
    Arc2DI			arc;

    int				fill= 0;
    int				line= 0;
    int				widthPixels= 0;

    int				wide;
    int				rx0;
    int				rx1;

    RGB8Color			rgb8Fill;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    tedDrawSetShapePath( xp, ds, sp, drPixels );

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

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
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

static int tedDrawMagneticDisk( DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *		sp= &SP_RECTANGLE;
    int				np= sp->spVertexCount;
    Point2DI *			xp;
    Arc2DI			arc;

    int				fill= 0;
    int				line= 0;
    int				widthPixels= 0;

    int				wide;
    int				ry0;
    int				ry1;

    RGB8Color			rgb8Fill;

    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    tedDrawSetShapePath( xp, ds, sp, drPixels );

    wide= drPixels->drX1- drPixels->drX0;
    /*
    high= drPixels->drY1- drPixels->drY0;
    */
    ry0= ( 5* drPixels->drY0+ 1* drPixels->drY1 )/ 6;
    ry1= ( 1* drPixels->drY0+ 5* drPixels->drY1 )/ 6;

    arc.a2diRect= *drPixels;
    arc.a2diRect.drX1= drPixels->drX0+ ( wide+ 2 )/ 3;
    arc.a2diAngleFrom= 64* 90;
    arc.a2diAngleStep= 64* 180;

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{
	DocumentRectangle	drFill= *drPixels;

	drawFillArc( drsf, &arc );

	drFill.drY0= ry0;
	drFill.drY1= ry1;
	drawFillRectangle( drsf, &drFill );

	drawFillArc( drsf, &arc );
	}

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
    if  ( line )
	{
	drawArc( drsf, &arc );

	drawLine( drsf, drPixels->drX0, ry0, drPixels->drX0, ry1 );
	drawLine( drsf, drPixels->drX1, ry0, drPixels->drX1, ry1 );

	drawArc( drsf, &arc );
	}

    free( xp );
    return 0;
    }


static int tedDrawCallout(	DrawingShape *			ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				const DocumentRectangle *	drPixels )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    DrawingSurface		drsf= lc->lcDrawingSurface;

    const ShapePath *	sp= &SP_RECTANGLE;
    int			np= sp->spVertexCount;
    Point2DI *		xp;

    int			fill= 0;
    int			line= 0;
    int			widthPixels= 0;

    RGB8Color		rgb8Fill;

    if  ( np != 4 )
	{ LDEB(np); return -1;	}

    xp= malloc( ( np+ 1+ 2+ 1 )* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    tedDrawSetShapePath( xp, ds, sp, drPixels );

    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
    if  ( fill )
	{ drawFillPolygon( drsf, xp, np ); }

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
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

int tedDrawDrawingShape(	const DocumentRectangle *	drTwips,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsdd )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    DrawingSurface		drsf= lc->lcDrawingSurface;
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;

    int				rval= 0;

    DocumentRectangle		drPixels;
    DocumentRectangle		drNorm;
    int				fill= 0;
    int				line= 0;
    int				widthPixels= 0;

    Point2DI *			xp= (Point2DI *)0;

    docGetPixelRect( &drPixels, lc, drTwips, page );
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
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_33 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_34 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    if  ( tedDrawPictureFrame( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCALLOUT_1:
	case SHPtyCALLOUT_90:
	    if  ( tedDrawCallout( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_RECTANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    {
	    RGB8Color		rgb8Fill;

	    const int	w= ( drPixels.drX1- drPixels.drX0 )/ 4;
	    const int	h= ( drPixels.drY1- drPixels.drY0 )/ 4;

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, sdd );
	    if  ( fill )
		{ drawFillRoundedRect( drsf, &drPixels, w, h );	}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{ drawRoundedRect( drsf, &drPixels, w, h );	}
	    }
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

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
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

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{ drawArc( drsf, &arc );	}
	    }
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_DIAMOND ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels,
						    &SP_ISOSCELES_TRIANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels,
						    &SP_FLOW_CHART_MERGE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels,
						    &SP_RIGHT_TRIANGLE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels,
						    &SP_PARALLELOGRAM ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_TRAPEZOID ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_HEXAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_OCTAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_PLUS_SIGN ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_NOTCHED_RIGHT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_HOME_PLATE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_CHEVRON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_RIGHT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_UP_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_DOWN_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_RIGHT_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_UP_DOWN_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_QUAD_ARROW_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_RIGHT_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_DOWN_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_UP_DOWN_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_QUAD_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_RIGHT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LEFT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_BENT_UP_ARROW ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_PENTAGON ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_STAR ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_SEAL4 ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_LINE ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_FLOW_CHART_MANUAL_INPUT ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_FLOW_CHART_PUNCHED_CARD ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( tedDrawShapePath( ds, dc, sdd, &drPixels, &SP_WEDGE_RECT_CALLOUT ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_ONLINE_STORAGE:
	    if  ( tedDrawOnlineStorage( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCAN:
	case SHPtyFLOW_CHART_MAGNETIC_DISK:
	    if  ( tedDrawMagneticDisk( ds, dc, sdd, &drPixels ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_BRACE:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    Point2DI *		xp;

	    int			wide;

	    xp= malloc( ( np+ 1 )* sizeof(Point2DI) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &drPixels );

	    wide= xp[2].x- xp[0].x;
	    /*
	    high= xp[2].y- xp[0].y;
	    */

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );

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

	    free( xp );
	    }
	    break;

	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( sd->sdVertexCount > 1 )
		{
		const int	closed= 1;

		xp= tedDrawGetVertices( drTwips, ds, lc, sdd );
		if  ( ! xp )
		    { XDEB(xp); rval= -1; goto ready;	}

		tedShapeDrawPoints( ds, xp, sd->sdVertexCount- 1,
							closed, dc, sdd );
		}
	    break;

	case SHPtyBENT_ARROW:
	default:
	    LSDEB(sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsDocumentTree.dtRoot )
	{}
    */

  ready:
    if  ( xp )
	{ free( xp ); }

    return rval;
    }

