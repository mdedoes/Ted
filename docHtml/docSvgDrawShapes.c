/************************************************************************/
/*									*/
/*  Infrastructure for printing 'shapes'.				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	<sioGeneral.h>
#   include	<docObjectProperties.h>
#   include	<docDraw.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<docShapePath.h>
#   include	<svgWriter.h>
#   include	"docSvgDrawImpl.h"
#   include	<geo2DInteger.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Include procedures to draw the paths that make up the simple shapes	*/
/*  in the SVG printout.						*/
/*									*/
/************************************************************************/

static void docSvgDrawShapePathLow(	SvgWriter *			sw,
					const AffineTransform2D *	at,
					const ShapePath *		sp )
    {
    XmlWriter *		xw= &(sw->swXmlWriter);
    const char *	command= "M";
    const char *	sep= "";

    int			i;

    double		x;
    double		y;

    double		x0= 0;
    double		y0= 0;

    xmlPutString( " d=\"", xw );

    for ( i= 0; i < sp->spVertexCount; i++ )
	{
	double		x1;
	double		y1;

	x= sp->spVertices[i].x- 0.5* sp->spXSize;
	y= sp->spVertices[i].y- 0.5* sp->spYSize;
	x1= AT2_X( x, y, at );
	y1= AT2_Y( x, y, at );

	sioOutPrintf( xw->xwSos, "%s%s %g,%g", sep, command, x1- x0, y1- y0 );

	x0= x1; y0= y1;
	sep="\n\t";
	command= "l";
	}

    if  ( sp->spClosed )
	{ xmlPutString( "Z", xw );	}

    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the form given by the path for an individual shape.		*/
/*									*/
/************************************************************************/

static int docSvgDrawShapePath(	const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr,
				DrawingContext *		dc,
				SvgWriter *			sw )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				rval= 0;
    AffineTransform2D		at;

    docShapeStartShapeTransform( &at, ds, dr, sp->spXSize, sp->spYSize );

    sioOutPutString( "<path", xw->xwSos );

    docSvgDrawShapePathLow( sw, &at, sp );

    docSvgEmitFill( sw, ds );
    docSvgEmitStroke( sw, ds );

    sioOutPutString( "/>", xw->xwSos );
    xmlNewLine( xw );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw an individual shape.						*/
/*									*/
/************************************************************************/

int docSvgDrawDrawDrawingShape(	const DocumentRectangle *	drOutside,
				const AffineTransform2D *	atOutside,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsw )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    SvgWriter *			sw= (SvgWriter *)vsw;
    XmlWriter *			xw= &(sw->swXmlWriter);

    int				rval= 0;

    AffineTransform2D		atHere;
    DocumentRectangle		drHere;
    DocumentRectangle		drNorm;

    docShapeGetRects( &drHere, &drNorm, &atHere, drOutside, atOutside, ds );

    switch( sd->sdShapeType )
	{
	case SHPtyGROUP:
	    /* Done by docDrawDrawingShape(). */
	    LSDEB(sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;

	case 33:
	    if  ( docSvgDrawShapePath( ds, &SP_33, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( docSvgDrawShapePath( ds, &SP_34, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;

	    if  ( docSvgDrawShapePath( ds, sp, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}

	    docShapeStartShapeTransform( &atHere, ds, drOutside,
						    sp->spXSize, sp->spYSize );

	    if  ( ds->dsPictureProperties.pipType != DOCokUNKNOWN )
		{ docSvgDrawShapeImage( sw, dc, ds, &drNorm, &atHere ); }
	    }
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( docSvgDrawShapePath( ds, &SP_RECTANGLE, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( docSvgDrawShapePath( ds, &SP_DIAMOND, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( docSvgDrawShapePath( ds, &SP_ISOSCELES_TRIANGLE,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( docSvgDrawShapePath( ds, &SP_FLOW_CHART_MERGE,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( docSvgDrawShapePath( ds, &SP_RIGHT_TRIANGLE,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( docSvgDrawShapePath( ds, &SP_PARALLELOGRAM,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( docSvgDrawShapePath( ds, &SP_TRAPEZOID, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( docSvgDrawShapePath( ds, &SP_HEXAGON, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( docSvgDrawShapePath( ds, &SP_OCTAGON, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( docSvgDrawShapePath( ds, &SP_PLUS_SIGN, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_ARROW, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_NOTCHED_RIGHT_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( docSvgDrawShapePath( ds, &SP_HOME_PLATE, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( docSvgDrawShapePath( ds, &SP_CHEVRON, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_ARROW, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_RIGHT_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_UP_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_DOWN_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_RIGHT_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_UP_DOWN_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_QUAD_ARROW_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_RIGHT_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_UP_ARROW, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_DOWN_ARROW, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_UP_DOWN_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_QUAD_ARROW, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_RIGHT_UP_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_LEFT_UP_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( docSvgDrawShapePath( ds, &SP_BENT_UP_ARROW,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( docSvgDrawShapePath( ds, &SP_PENTAGON, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( docSvgDrawShapePath( ds, &SP_STAR, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( docSvgDrawShapePath( ds, &SP_SEAL4, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( docSvgDrawShapePath( ds, &SP_LINE, drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( docSvgDrawShapePath( ds, &SP_FLOW_CHART_MANUAL_INPUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( docSvgDrawShapePath( ds, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( docSvgDrawShapePath( ds, &SP_FLOW_CHART_PUNCHED_CARD,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( docSvgDrawShapePath( ds, &SP_WEDGE_RECT_CALLOUT,
							drOutside, dc, sw ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    sioOutPutString( "<rect", xw->xwSos );

		{
		int		w;
		int		h;
		int		r;

		w= drNorm.drX1- drNorm.drX0+ 1;
		h= drNorm.drY1- drNorm.drY0+ 1;
		r= sqrt( w*w+ h*h )/ 8;

		svgWriteRectangleAttributes( sw, &drNorm );
		xmlWriteIntAttribute( xw, "rx", r );
		xmlWriteIntAttribute( xw, "ry", r );

		docSvgEmitFill( sw, ds );
		docSvgEmitStroke( sw, ds );
		}

	    sioOutPutString( "/>", xw->xwSos );
	    xmlNewLine( xw );
	    break;

	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( sd->sdVertexCount > 1 )
		{
		int		i;
		const char *	command= "M";
		const char *	sep= "";
		int		x0= ( drOutside->drX1- drOutside->drX0 )/ 2;
		int		y0= ( drOutside->drY1- drOutside->drY0 )/ 2;

		docShapeStartShapeTransform( &atHere, ds, drOutside,
					drOutside->drX1- drOutside->drX0,
					drOutside->drY1- drOutside->drY0 );

		for ( i= 0; i < sd->sdVertexCount; i++ )
		    {
		    sioOutPrintf( xw->xwSos, "%s%s %d,%d", sep, command,
						sd->sdVertices[i].x- x0,
						sd->sdVertices[i].y- y0 );

		    x0= sd->sdVertices[i].x;
		    y0= sd->sdVertices[i].y;
		    sep="\n\t";
		    command= "l";
		    }

		docSvgEmitFill( sw, ds );
		docSvgEmitStroke( sw, ds );
		}
	    break;

	default:
	    LLSDEB(ds->dsShapeNumber,sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsDocumentTree.dtRoot )
	{}
    */

    return rval;
    }

