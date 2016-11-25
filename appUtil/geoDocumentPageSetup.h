/************************************************************************/
/*									*/
/*  Various definitions relating to the layout of a page of the		*/
/*  document.								*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_DOCUMENT_GEOMETRY_H
#   define	UTIL_DOCUMENT_GEOMETRY_H

#   include	"geoRectangleOffsets.h"

struct PropertyMask;
struct DocumentRectangle;

/**
 *  The template for the layout of a page in a document.
 */
typedef struct DocumentGeometry
    {
			/**
			 *  The width of the page. Margins are subtracted 
			 *  from the width.
			 */
    int			dgPageWideTwips;

			/**
			 *  The height of the page. Margins are subtracted 
			 *  from the width.
			 */
    int			dgPageHighTwips;

    RectangleOffsets	dgMargins;

    int			dgHeaderPositionTwips;
    int			dgFooterPositionTwips;

    int			dgGutterTwips;
    unsigned char	dgMirrorMargins;
    } DocumentGeometry;

# define geoContentWide( dg ) \
    ( (dg)->dgPageWideTwips- \
		(dg)->dgMargins.roLeftOffset- (dg)->dgMargins.roRightOffset )

# define geoContentHigh( dg ) \
    ( (dg)->dgPageHighTwips- \
		(dg)->dgMargins.roTopOffset- (dg)->dgMargins.roBottomOffset )

/************************************************************************/
/*									*/
/*  Update masks for document geometry.					*/
/*									*/
/*  NOTE: This range is continued in Ted/docBuf.h for the section and	*/
/*	document properties. Adding a bit here may make it necessary to	*/
/*	shift the masks there as well.					*/
/*									*/
/************************************************************************/

typedef enum GeometryProperty
    {
    DGprop_NONE= -1,

    DGpropPAGE_WIDTH= 0,
    DGpropPAGE_HEIGHT,
    DGpropLEFT_MARGIN,
    DGpropRIGHT_MARGIN,
    DGpropTOP_MARGIN,
    DGpropBOTTOM_MARGIN,
    DGpropHEADER_POSITION,
    DGpropFOOTER_POSITION,
    DGpropGUTTER,
    DGpropMARGMIR,

    DGprop_COUNT
    } GeometryProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitDocumentGeometry(	DocumentGeometry *	dg );

extern void utilUpdDocumentGeometry(
				struct PropertyMask *		dpDoneMask,
				DocumentGeometry *		dgTo,
				const struct PropertyMask *	dgSetMask,
				const DocumentGeometry *	dgSet );

extern void utilDocumentGeometryGetBodyRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void utilDocumentGeometryGetPageRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void utilDocumentGeometryGetHeaderRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void utilDocumentGeometryGetFooterRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void utilDocumentGeometryGetPageBoundingBox(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg,
				int				hasHeader,
				int				hasFooter );

extern void utilOverridePaperSize(
				DocumentGeometry *		dgTo,
				const DocumentGeometry *	dgFrom );

#   endif
