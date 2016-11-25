/************************************************************************/
/*									*/
/*  The paths do draw some kinds of shapes.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_PATH_H
#   define	DOC_SHAPE_PATH_H

struct Point2DI;

typedef struct ShapePath
    {
    const char *	spName;
    short int		spXSize;
    short int		spYSize;
    short int		spClosed;
    short int		spVertexCount;
    struct Point2DI *	spVertices;
    } ShapePath;

/************************************************************************/
/*									*/
/*  Descriptions of shapes.						*/
/*									*/
/************************************************************************/

extern const ShapePath	SP_33;
extern const ShapePath	SP_34;
extern const ShapePath	SP_LINE;
extern const ShapePath	SP_RECTANGLE;
extern const ShapePath	SP_DIAMOND;
extern const ShapePath	SP_ISOSCELES_TRIANGLE;
extern const ShapePath	SP_RIGHT_TRIANGLE;
extern const ShapePath	SP_PARALLELOGRAM;
extern const ShapePath	SP_TRAPEZOID;
extern const ShapePath	SP_HEXAGON;
extern const ShapePath	SP_OCTAGON;
extern const ShapePath	SP_PLUS_SIGN;
extern const ShapePath	SP_ARROW;
extern const ShapePath	SP_NOTCHED_RIGHT_ARROW;
extern const ShapePath	SP_HOME_PLATE;
extern const ShapePath	SP_CHEVRON;
extern const ShapePath	SP_LEFT_ARROW;
extern const ShapePath	SP_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_QUAD_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW;
extern const ShapePath	SP_UP_ARROW;
extern const ShapePath	SP_DOWN_ARROW;
extern const ShapePath	SP_UP_DOWN_ARROW;
extern const ShapePath	SP_QUAD_ARROW;
extern const ShapePath	SP_LEFT_RIGHT_UP_ARROW;
extern const ShapePath	SP_LEFT_UP_ARROW;
extern const ShapePath	SP_BENT_UP_ARROW;
extern const ShapePath	SP_PENTAGON;
extern const ShapePath	SP_FLOW_CHART_PUNCHED_CARD;
extern const ShapePath	SP_FLOW_CHART_MANUAL_INPUT;
extern const ShapePath	SP_FLOW_CHART_OFF_PAGE_CONNECTOR;
extern const ShapePath	SP_WEDGE_RECT_CALLOUT;
extern const ShapePath	SP_STAR;
extern const ShapePath	SP_SEAL4;
extern const ShapePath	SP_FLOW_CHART_MERGE;

#   endif	/*  DOC_SHAPE_PATH_H	*/
