#   include	"utilGeoConfig.h"

#   include	<limits.h>

#   include	"geoRectangle.h"
#   include	"geoRectangleOffsets.h"
#   include	"geoAffineTransform.h"

#   include	<appDebugon.h>

void geoInitRectangle(	DocumentRectangle *		dr )
    { dr->drX0= dr->drY0= dr->drX1= dr->drY1= 0; return; }

void geoInvalidateRectangle(	DocumentRectangle *		dr )
    {
    dr->drX0= dr->drY0= INT_MAX;
    dr->drX1= dr->drY1= INT_MIN;

    return;
    }

/************************************************************************/
/*									*/
/*  Remove the possible orientation of a rectangle: Make sure x0 <= x1	*/
/*  and y0 <= y1.							*/
/*									*/
/************************************************************************/

void geoNormalizeRectangle(	DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom )
    {
    DocumentRectangle	dr;
    int			swap;

    dr= *drFrom;

    if  ( dr.drX1 < dr.drX0 )
	{ swap= dr.drX1; dr.drX1= dr.drX0; dr.drX0= swap; }

    if  ( dr.drY1 < dr.drY0 )
	{ swap= dr.drY1; dr.drY1= dr.drY0; dr.drY0= swap; }

    *drTo= dr;

    return;
    }

void geoUnionRectangle(	DocumentRectangle *		dr,
			const DocumentRectangle *	dr1,
			const DocumentRectangle *	dr2 )
    {
    if  ( dr1->drX0 < dr2->drX0 )
	{ dr->drX0= dr1->drX0;	}
    else{ dr->drX0= dr2->drX0;	}

    if  ( dr1->drY0 < dr2->drY0 )
	{ dr->drY0= dr1->drY0;	}
    else{ dr->drY0= dr2->drY0;	}

    if  ( dr1->drX1 < dr2->drX1 )
	{ dr->drX1= dr2->drX1;	}
    else{ dr->drX1= dr1->drX1;	}

    if  ( dr1->drY1 < dr2->drY1 )
	{ dr->drY1= dr2->drY1;	}
    else{ dr->drY1= dr1->drY1;	}
    }

int geoIntersectRectangle(	DocumentRectangle *		dr,
				const DocumentRectangle *	dr1,
				const DocumentRectangle *	dr2 )
    {
    DocumentRectangle	res= *dr1;

    if  ( dr1->drX1 < dr2->drX0 )
	{ return 0;	}

    if  ( dr2->drX1 < dr1->drX0 )
	{ return 0;	}

    if  ( dr1->drY1 < dr2->drY0 )
	{ return 0;	}

    if  ( dr2->drY1 < dr1->drY0 )
	{ return 0;	}

    if  ( ! dr )
	{ return 1;	}

    if  ( dr1->drX0 < dr2->drX0 )
	{ res.drX0= dr2->drX0;	}
    else{ res.drX0= dr1->drX0;	}

    if  ( dr1->drY0 < dr2->drY0 )
	{ res.drY0= dr2->drY0;	}
    else{ res.drY0= dr1->drY0;	}

    if  ( dr1->drX1 < dr2->drX1 )
	{ res.drX1= dr1->drX1;	}
    else{ res.drX1= dr2->drX1;	}

    if  ( dr1->drY1 < dr2->drY1 )
	{ res.drY1= dr1->drY1;	}
    else{ res.drY1= dr2->drY1;	}

    *dr= res; return 1;
    }

int geoRectangleSubtractPadding( DocumentRectangle *		drInside,
				const DocumentRectangle *	drOutside,
				const RectangleOffsets *	padding )
    {
    DocumentRectangle	dr;

    dr= *drOutside;
    if  ( dr.drX0 != INT_MIN )
	{ dr.drX0 += padding->roLeftOffset;	}
    if  ( dr.drX1 != INT_MAX )
	{ dr.drX1 -= padding->roRightOffset;	}
    if  ( dr.drY0 != INT_MIN )
	{ dr.drY0 += padding->roTopOffset;	}
    if  ( dr.drY1 != INT_MAX )
	{ dr.drY1 -= padding->roBottomOffset;	}

    if  ( dr.drX0 > dr.drX1 || dr.drY0 > dr.drY1 )
	{ return -1;	}

    *drInside= dr;
    return 0;
    }

int geoRectangleAddMargins( 	DocumentRectangle *		drOutside,
				const DocumentRectangle *	drInside,
				const RectangleOffsets *	margins )
    {
    DocumentRectangle	dr;

    dr= *drInside;
    if  ( dr.drX0 != INT_MIN )
	{ dr.drX0 -= margins->roLeftOffset;	}
    if  ( dr.drX1 != INT_MAX )
	{ dr.drX1 += margins->roRightOffset;	}
    if  ( dr.drY0 != INT_MIN )
	{ dr.drY0 -= margins->roTopOffset;	}
    if  ( dr.drY1 != INT_MAX )
	{ dr.drY1 += margins->roBottomOffset;	}

    if  ( dr.drX0 > dr.drX1 || dr.drY0 > dr.drY1 )
	{ return -1;	}

    *drOutside= dr;
    return 0;
    }

void geoLogRectangle(	const char *			label,
			const DocumentRectangle *	dr )
    {
    appDebug( "%s: [%4d+%4d=%4d]x[%4d+%4d=%4d]\n", label,
		dr->drX0, dr->drX1- dr->drX0+ 1, dr->drX1,
		dr->drY0, dr->drY1- dr->drY0+ 1, dr->drY1 );

    return;
    }

void geoLogRectangles(	const char *			label1,
			const DocumentRectangle *	dr1,
			const char *			label2,
			const DocumentRectangle *	dr2 )
    {
    appDebug( "%s: [%4d+%4d]x[%4d+%4d] %s [%4d+%4d]x[%4d+%4d]\n",
		label1,
		dr1->drX0, dr1->drX1- dr1->drX0+ 1,
		dr1->drY0, dr1->drY1- dr1->drY0+ 1,
		label2,
		dr2->drX0, dr2->drX1- dr2->drX0+ 1,
		dr2->drY0, dr2->drY1- dr2->drY0+ 1 );

    return;
    }

/**
 * Determine the rectangle around the transform of the source rectangle.
 * Depending on the transorm, that might be the transform of the source 
 * rectangle.
 */
void geoTransformRectangle(	DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom,
				const AffineTransform2D *	at )
    {
    DocumentRectangle	dr;

    double		x;
    double		y;

    x= AT2_X( drFrom->drX0, drFrom->drY0, at );
    y= AT2_Y( drFrom->drX0, drFrom->drY0, at );

    dr.drX0= dr.drX1= x;
    dr.drY0= dr.drY1= y;

    x= AT2_X( drFrom->drX1, drFrom->drY0, at );
    y= AT2_Y( drFrom->drX1, drFrom->drY0, at );

    if  ( dr.drX0 > x )
	{ dr.drX0= x;	}
    if  ( dr.drY0 > y )
	{ dr.drY0= y;	}
    if  ( dr.drX1 < x )
	{ dr.drX1= x;	}
    if  ( dr.drY1 < y )
	{ dr.drY1= y;	}

    x= AT2_X( drFrom->drX0, drFrom->drY1, at );
    y= AT2_Y( drFrom->drX0, drFrom->drY1, at );

    if  ( dr.drX0 > x )
	{ dr.drX0= x;	}
    if  ( dr.drY0 > y )
	{ dr.drY0= y;	}
    if  ( dr.drX1 < x )
	{ dr.drX1= x;	}
    if  ( dr.drY1 < y )
	{ dr.drY1= y;	}

    x= AT2_X( drFrom->drX1, drFrom->drY1, at );
    y= AT2_Y( drFrom->drX1, drFrom->drY1, at );

    if  ( dr.drX0 > x )
	{ dr.drX0= x;	}
    if  ( dr.drY0 > y )
	{ dr.drY0= y;	}
    if  ( dr.drX1 < x )
	{ dr.drX1= x;	}
    if  ( dr.drY1 < y )
	{ dr.drY1= y;	}

    *drTo= dr;

    return;
    }

