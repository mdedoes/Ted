/************************************************************************/
/*									*/
/*  Various definitions relating to document geometry.			*/
/*									*/
/************************************************************************/

#   ifndef		GEO_RECTANGLE_H
#   define		GEO_RECTANGLE_H

struct RectangleOffsets;
struct AffineTransform2D;

typedef struct DocumentRectangle
    {
    int		drX0;
    int		drY0;
    int		drX1;
    int		drY1;
    } DocumentRectangle;

#   define RECTDEB(dr) appDebug(					\
			    "%s(%3d) %s= [%d..%d x %d..%d]\n",		\
			    __FILE__, __LINE__, #dr,			\
			    (dr)->drX0, (dr)->drX1,			\
			    (dr)->drY0, (dr)->drY1 )

#   define	geoIncludeRectangleY( dr1, dr2 ) \
		{ \
		if  ( (dr1)->drY0 > (dr2)->drY0 )  \
		    { (dr1)->drY0=  (dr2)->drY0; } \
		if  ( (dr1)->drY1 < (dr2)->drY1 )  \
		    { (dr1)->drY1=  (dr2)->drY1; } \
		}

# define geo2DIXYInBox( x, y, b ) \
		    ( (x) >= (b)->drX0 && \
		      (x) <= (b)->drX1 && \
		      (y) >= (b)->drY0 && \
		      (y) <= (b)->drY1 )

# define geo2DIPointInBox( p, b ) geo2DIXYInBox( (p)->x, (p)->y, (b) )

# define geo2DIBoxAroundXY( x, y, b ) \
    { \
    if  ( (b)->drX0 > (x) ) { (b)->drX0=  (x);	} \
    if  ( (b)->drY0 > (y) ) { (b)->drY0=  (y);	} \
    if  ( (b)->drX1 < (x) ) { (b)->drX1=  (x);	} \
    if  ( (b)->drY1 < (y) ) { (b)->drY1=  (y);	} \
    }

# define geoShiftRectangle( dr, ox, oy ) \
    { \
    (dr)->drX0 += (ox); \
    (dr)->drX1 += (ox); \
    (dr)->drY0 += (oy); \
    (dr)->drY1 += (oy); \
    }

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void geoInitRectangle(		DocumentRectangle *		dr );
extern void geoInvalidateRectangle(	DocumentRectangle *		dr );

extern void geoUnionRectangle(	DocumentRectangle *		dr,
				const DocumentRectangle *	dr1,
				const DocumentRectangle *	dr2	);

extern void geoNormalizeRectangle(
				DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom );

extern int geoIntersectRectangle(	DocumentRectangle *		dr,
					const DocumentRectangle *	dr1,
					const DocumentRectangle *	dr2 );

extern int geoRectangleSubtractPadding(
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drOutside,
				const struct RectangleOffsets *	padding );

extern int geoRectangleAddMargins(
				DocumentRectangle *		drOutside,
				const DocumentRectangle *	drInside,
				const struct RectangleOffsets *	margins );

extern void geoLogRectangle(	const char *			label,
				const DocumentRectangle *	dr );

extern void geoLogRectangles(	const char *			label1,
				const DocumentRectangle *	dr1,
				const char *			label2,
				const DocumentRectangle *	dr2 );

extern void geoTransformRectangle(
			    DocumentRectangle *			drTo,
			    const DocumentRectangle *		drFrom,
			    const struct AffineTransform2D *	at );

#   endif	/*	GEO_RECTANGLE_H */
