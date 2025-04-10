/************************************************************************/
/*									*/
/*  Primitives for drawing metafiles (WMF,PICT) on X11.			*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	<drawDrawingSurface.h>
#   include	<geo2DInteger.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine points, arcs and segments to draw.			*/
/*  Semantcs are those of XDrawArc:					*/
/*	Coordinates are in pixels.					*/
/*	X coordinates go right.						*/
/*	Y coordinates go down.						*/
/*									*/
/*      / 2----------3 \						*/
/*     /  .          .  \						*/
/*  12=0..1          4..5						*/
/*     |                |						*/
/*     |                |						*/
/*    11..10         7..6						*/
/*     \  .          .  /						*/
/*      \ 9----------8 /						*/
/*									*/
/************************************************************************/

static const int DrawRoundedRectSideOffsets[4]= { 2, 5, 8, 11 };

static int drawPrepareRoundRect(	Arc2DI			arcs[4],
					Point2DI		points[13],
					const DocumentRectangle * dr,
					int			rx,
					int			ry )
    {
    DocumentRectangle	drHere;

    geoNormalizeRectangle( &drHere, dr );

    if  ( ry < 0 )
	{ ry= -ry;	}
    if  ( rx < 0 )
	{ rx= -rx;	}

    /* 0 */
    points->x= drHere.drX0;
    points->y= drHere.drY0+ ry/ 2;
    points++;

    /* 0 */
    arcs->a2diRect.drX0= drHere.drX0;
    arcs->a2diRect.drY0= drHere.drY0;
    arcs->a2diRect.drX1= arcs->a2diRect.drX0+ rx- 1;
    arcs->a2diRect.drY1= arcs->a2diRect.drY0+ ry- 1;
    arcs->a2diAngleFrom= 64*  180;
    arcs->a2diAngleStep= 64*  -90;
    arcs++;

    /* 1 */
    points->x= drHere.drX0+ rx/ 2;
    points->y= drHere.drY0+ ry/ 2;
    points++;
    /* 2 */
    points->x= drHere.drX0+ rx/ 2;
    points->y= drHere.drY0;
    points++;
    /* 3 */
    points->x= drHere.drX1- rx/ 2;
    points->y= drHere.drY0;
    points++;

    /* 1 */
    arcs->a2diRect.drX0= drHere.drX1- rx;
    arcs->a2diRect.drY0= drHere.drY0;
    arcs->a2diRect.drX1= arcs->a2diRect.drX0+ rx- 1;
    arcs->a2diRect.drY1= arcs->a2diRect.drY0+ ry- 1;
    arcs->a2diAngleFrom= 64*   90;
    arcs->a2diAngleStep= 64*  -90;
    arcs++;

    /* 4 */
    points->x= drHere.drX1- rx/ 2;
    points->y= drHere.drY0+ ry/ 2;
    points++;
    /* 5 */
    points->x= drHere.drX1;
    points->y= drHere.drY0+ ry/ 2;
    points++;
    /* 6 */
    points->x= drHere.drX1;
    points->y= drHere.drY1- ry/ 2;
    points++;

    /* 2 */
    arcs->a2diRect.drX0= drHere.drX1- rx;
    arcs->a2diRect.drY0= drHere.drY1- ry;
    arcs->a2diRect.drX1= arcs->a2diRect.drX0+ rx- 1;
    arcs->a2diRect.drY1= arcs->a2diRect.drY0+ ry- 1;
    arcs->a2diAngleFrom= 64*    0;
    arcs->a2diAngleStep= 64*  -90;
    arcs++;

    /* 7 */
    points->x= drHere.drX1- rx/ 2;
    points->y= drHere.drY1- ry/ 2;
    points++;
    /* 8 */
    points->x= drHere.drX1- rx/ 2;
    points->y= drHere.drY1;
    points++;
    /* 9 */
    points->x= drHere.drX0+ rx/ 2;
    points->y= drHere.drY1;
    points++;

    /* 3 */
    arcs->a2diRect.drX0= drHere.drX0;
    arcs->a2diRect.drY0= drHere.drY1- ry;
    arcs->a2diRect.drX1= arcs->a2diRect.drX0+ rx- 1;
    arcs->a2diRect.drY1= arcs->a2diRect.drY0+ ry- 1;
    arcs->a2diAngleFrom= 64*  -90;
    arcs->a2diAngleStep= 64*  -90;
    arcs++;

    /* 10 */
    points->x= drHere.drX0+ rx/ 2;
    points->y= drHere.drY1- ry/ 2;
    points++;
    /* 11 */
    points->x= drHere.drX0;
    points->y= drHere.drY1- ry/ 2;
    points++;

    /* 12 = 0 */
    points->x= drHere.drX0;
    points->y= drHere.drY0+ ry/ 2;

    return 0;
    }

int drawFillRoundedRect(	DrawingSurface			ds,
				const DocumentRectangle *	dr,
				int				rx,
				int				ry )
    {
    int		a;
    Arc2DI	arcs[4];
    Point2DI	points[13];

    drawPrepareRoundRect( arcs, points, dr, rx, ry );

    for ( a= 0; a < 4; a++ )
	{ drawFillArc( ds, &(arcs[a]) );	}

    drawFillPolygon( ds, points, 12 );

    return 0;
    }

int drawRoundedRect(		DrawingSurface			ds,
				const DocumentRectangle *	dr,
				int				rx,
				int				ry )
    {
    int		a;
    Arc2DI	arcs[4];
    Point2DI	points[13];

    drawPrepareRoundRect( arcs, points, dr, rx, ry );

    for ( a= 0; a < 4; a++ )
	{ drawArc( ds, &(arcs[a]) );	}

    for ( a= 0; a < 4; a++ )
	{
	int	o= DrawRoundedRectSideOffsets[a];

	drawLine( ds, points[o].x, points[o].y,
				points[o+1].x, points[o+1].y );
	}

    return 0;
    }

