/************************************************************************/
/*									*/
/*  Declarations for shapes.						*/
/*									*/
/*  Distances are in EMU's.						*/
/*  12700 EMU/Point.							*/
/*  635 EMU/Twip.							*/
/*  914400 EMU/Inch.							*/
/*  360000 EMU/cm.							*/
/*									*/
/*  With the exception of 'shplid', the ShapeProperties that come from	*/
/*  regular rtf only apply for the top level 'shp' or shpgrp'.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_H
#   define	DOC_SHAPE_H

struct DrawingSurface;
struct IndexSet;

#   include	<utilColor.h>
#   include	<geoRectangle.h>
#   include	<utilMemoryBuffer.h>
#   include	<geoAffineTransform.h>
#   include	<docShapeProperties.h>
#   include	<docPictureProperties.h>
#   include	"docDocumentTree.h"
#   include	<docShapeDrawing.h>
#   include	<bitmap.h>
#   include	<docSelectionScope.h>

typedef struct DrawingShape
    {
			/**
			 *  Properties as found in regular rtf tags.
			 *  Are only set for the top level shape in a hierarchy
			 */
    ShapeProperties	dsShapeProperties;

			/**
			 *  True if and only if this shape is the child 
			 *  of another shape. [I.E if it is not the top 
			 *  of the hierarchy.]
			 */
    unsigned char	dsIsChildShape;

			/**
			 *  The drawing of the shape.
			 */
    ShapeDrawing	dsDrawing;

			/**
			 *  The unique shape number in the document.
			 */
    int			dsShapeNumber;

			/**
			 *  The selection scope for the text inside 
			 *  the shape.
			 */
    SelectionScope	dsSelectionScope;

			/**
			 *  The text inside the shape. (If any)
			 */
    DocumentTree	dsDocumentTree;

			/**
			 *  The child shapes of this shape. In 
			 *  practice, a shape does not have both text 
			 *  content and children.
			 */
    struct DrawingShape **	dsChildren;
    int				dsChildCount;

    PictureProperties		dsPictureProperties;
    MemoryBuffer		dsPictureData;
    struct DrawingSurface *	dsDrawingSurface;
    RasterImage			dsRasterImage;
    } DrawingShape;

# define DSflipHORIZONTAL( ds ) \
    ( ( (ds)->dsIsChildShape && (ds)->dsDrawing.sd_fRelFlipH ) || (ds)->dsDrawing.sd_fFlipH )

# define DSflipVERTICAL( ds ) \
    ( ( (ds)->dsIsChildShape && (ds)->dsDrawing.sd_fRelFlipV ) || (ds)->dsDrawing.sd_fFlipV )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeAllocated(	DrawingShape *		ds );
extern void docInitDrawingShape(	DrawingShape *		ds );
extern void docCleanDrawingShape(	DrawingShape *		ds );

extern void docDeleteDrawingShape(	struct BufferDocument *	bd,
					DrawingShape *		ds );

extern void docShapeStartShapeTransform(
				AffineTransform2D *		at,
				const DrawingShape *		ds,
				const DocumentRectangle *	dr,
				int				xSize,
				int				ySize );

extern void docShapeInternalTransform(	AffineTransform2D *	at,
					const DrawingShape *	ds );

extern int docShapeGetFill(	int *			pFill,
				RGB8Color *		rgb8,
				const DrawingShape *	ds );

extern int docShapeGetLine(	int *			pFill,
				RGB8Color *		rgb8,
				const DrawingShape *	ds );

extern void docShapeGetRects(
			struct DocumentRectangle *		drHere,
			struct DocumentRectangle *		drNorm,
			struct AffineTransform2D *		atHere,
			const struct DocumentRectangle *	drOutside,
			const struct AffineTransform2D *	atOutside,
			const struct DrawingShape *		ds );

extern void docShapeGetChildRect(
			struct DocumentRectangle *		drChild,
			struct AffineTransform2D *		atChild,
			const struct DrawingShape *		dsChild,
			const struct DocumentRectangle *	drOutside,
			const struct AffineTransform2D *	atOutside,
			const struct DrawingShape *		ds );

extern int docGetAttributesUsedInShape( struct BufferDocument *	bd,
				struct DrawingShape *		ds,
				struct IndexSet *		isUsed );

#   endif	/*  DOC_SHAPE_H	*/
