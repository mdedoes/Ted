/************************************************************************/
/*									*/
/*  Shape Geometry.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_GEOMETRY_H
#   define	DOC_SHAPE_GEOMETRY_H

struct DocumentRectangle;
struct RectangleOffsets;
struct DocumentGeometry;

struct ShapeProperties;
struct ShapeDrawing;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docPlaceRootShapeRect(
				struct DocumentRectangle *	drOutside,
				const struct ShapeProperties *	sp,
				int				x0,
				int				y0 );

extern int docShapeGetPadding(
				struct RectangleOffsets *	padding,
				const struct DocumentRectangle * drOutside,
				const struct ShapeDrawing *	sd );

extern int docShapeGetRoundedRectRadius(
				const struct DocumentRectangle * drOutside,
				const struct ShapeDrawing *	sd );

extern void docShapeRootGeometry(
			struct DocumentGeometry *		dgBox,
			const struct ShapeDrawing *		sd,
			const struct DocumentRectangle *	drOutside );

#   endif	/*  DOC_SHAPE_GEOMETRY_H	*/
