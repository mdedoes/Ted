/************************************************************************/
/*									*/
/*  Shapes.. Geometry.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<limits.h>
#   include	<math.h> /* for M_SQRT1_2 */

#   include	"docShapeProperties.h"
#   include	"docShapeDrawing.h"
#   include	"docShapeGeometry.h"
#   include	"docShapeType.h"
#   include	<geoRectangleOffsets.h>
#   include	<geoDocumentPageSetup.h>

#   include	<appDebugon.h>

#   ifndef	M_SQRT1_2
#   define	M_SQRT1_2	0.70710678118654752440
#   endif

/************************************************************************/
/*									*/
/*  Given the origin.. Get the rectangle for a shape.			*/
/*									*/
/************************************************************************/

void docPlaceRootShapeRect(		DocumentRectangle *	drOutside,
					const ShapeProperties *	sp,
					int			x0,
					int			y0 )
    {
    drOutside->drX0= x0;
    drOutside->drX1= drOutside->drX0+ sp->spRect.drX1- sp->spRect.drX0;

    drOutside->drY0= y0;
    drOutside->drY1= drOutside->drY0+ sp->spRect.drY1- sp->spRect.drY0;
    }

/************************************************************************/
/*									*/
/*  Given the outside.. Get the inside padding for a shape.		*/
/*									*/
/************************************************************************/

int docShapeGetPadding(	RectangleOffsets *		padding,
			const DocumentRectangle *	drOutside,
			const ShapeDrawing *		sd )
    {
    static double	ARC_PAD= (1.0- M_SQRT1_2);

    switch( sd->sdShapeType )
	{
	case SHPtyRECTANGLE:
	    geoInitRectangleOffsets( padding );
	    return 0;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    {
	    int		r;

	    r= docShapeGetRoundedRectRadius( drOutside, sd );
	    if  ( r < 0 )
		{ LLDEB(sd->sdShapeType,r); return -1;	}
	    padding->roTopOffset= ARC_PAD* r;
	    padding->roLeftOffset= ARC_PAD* r;
	    padding->roRightOffset= ARC_PAD* r;
	    padding->roBottomOffset= ARC_PAD* r;
	    return 0;
	    }

	case SHPtyDIAMOND:
	    {
	    int		w= drOutside->drX1- drOutside->drX0;
	    int		h= drOutside->drY1- drOutside->drY0;

	    padding->roTopOffset= h/ 4;
	    padding->roLeftOffset= w/ 4;
	    padding->roRightOffset= w/ 4;
	    padding->roBottomOffset= h/ 4;
	    return 0;
	    }

	case SHPtyELLIPSE:
	    {
	    int		w= drOutside->drX1- drOutside->drX0;
	    int		h= drOutside->drY1- drOutside->drY0;

	    padding->roTopOffset= ( ARC_PAD* h )/ 2;
	    padding->roLeftOffset= ( ARC_PAD* w )/ 2;
	    padding->roRightOffset= ( ARC_PAD* w )/ 2;
	    padding->roBottomOffset= ( ARC_PAD* h )/ 2;
	    return 0;
	    }

	default:
	    LDEB(sd->sdShapeType);
	    geoInitRectangleOffsets( padding );
	    return 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Given the rectangle and the adjusr value.. Get the radius of the	*/
/*  circle for the corners of a rounded rectangle.			*/
/*									*/
/************************************************************************/

int docShapeGetRoundedRectRadius(
				const DocumentRectangle *	drOutside,
				const ShapeDrawing *		sd )
    {
    int			w;
    int			h;
    int			r;

    int			adjustValue;

    const int		fac= 32767;

    switch( sd->sdShapeType )
	{
	case SHPtyROUND_RECTANGLE:
	    adjustValue= sd->sdAdjustValue;
	    break;

	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    adjustValue= fac/ 10;
	    break;

	default:
	    LDEB(sd->sdShapeType);
	    return -1;
	}

    w= ( drOutside->drX1- drOutside->drX0 );
    h= ( drOutside->drY1- drOutside->drY0 );
    if  ( w < h )
	{ r= ( (long)adjustValue* w+ fac/ 2 )/ fac;	}
    else{ r= ( (long)adjustValue* h+ fac/ 2 )/ fac;	}

    return r;
    }

/************************************************************************/
/*									*/
/*  Build a fake page layout to format the contents of a shape.		*/
/*									*/
/*  As the contents of the shape are drawn with an origin shift, only	*/
/*  use the width of the rectangle.					*/
/*									*/
/************************************************************************/

void docShapeRootGeometry(
			DocumentGeometry *		dgBox,
			const ShapeDrawing *		sd,
			const DocumentRectangle *	drOutside )
    {
    RectangleOffsets	padding;

    geoInitRectangleOffsets( &padding );

    docShapeGetPadding( &padding, drOutside, sd );

    if  ( drOutside->drX0 <= drOutside->drX1 )
	{
	dgBox->dgPageWideTwips= drOutside->drX1- drOutside->drX0+ 1;
	dgBox->dgMargins.roLeftOffset= padding.roLeftOffset;
	dgBox->dgMargins.roRightOffset= padding.roRightOffset;
	}
    else{
	dgBox->dgPageWideTwips= drOutside->drX0- drOutside->drX1+ 1;
	dgBox->dgMargins.roLeftOffset= padding.roLeftOffset;
	dgBox->dgMargins.roRightOffset= padding.roRightOffset;
	}

    dgBox->dgPageHighTwips= INT_MAX;
    dgBox->dgMargins.roTopOffset= padding.roTopOffset;

    dgBox->dgMargins.roLeftOffset += EMUtoTWIPS( sd->sd_dxTextLeft );
    dgBox->dgMargins.roTopOffset += EMUtoTWIPS( sd->sd_dyTextTop );
    dgBox->dgMargins.roRightOffset += EMUtoTWIPS( sd->sd_dxTextRight );

    /* Irrelevant: */
    dgBox->dgMargins.roBottomOffset= 0;

    return;
    }
