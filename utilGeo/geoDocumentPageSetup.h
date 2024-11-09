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

			/**
			 * The (vertical) position of the (top of the)
			 * page header on the page.
			 */
    int			dgHeaderPositionTwips;

			/**
			 * The (vertical) position of the (bottom of the)
			 * page footer on the page.
			 */
    int			dgFooterPositionTwips;

			/**
			 * With multiple columns of text per page, this is 
			 * the distance between the columns. (With a single
			 * column, dgGutterTwips is irrelevant)
			 */
    int			dgGutterTwips;

			/**
			 * If true, swap=mirror left and right margins on
			 * pages with an odd index. (Normally, an odd index
			 * results in an even page number)
			 */
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

extern void geoInitDocumentGeometry(	DocumentGeometry *	dg );

extern void geoUpdDocumentGeometry(
				struct PropertyMask *		dpDoneMask,
				DocumentGeometry *		dgTo,
				const struct PropertyMask *	dgSetMask,
				const DocumentGeometry *	dgSet );

extern void geoDocumentGeometryGetBodyRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void geoDocumentGeometryGetPageRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void geoDocumentGeometryGetHeaderRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void geoDocumentGeometryGetFooterRect(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg );

extern void geoDocumentGeometryGetPageBoundingBox(
				struct DocumentRectangle *	dr,
				const DocumentGeometry *	dg,
				int				hasHeader,
				int				hasFooter );

extern void geoOverridePaperSize(
				DocumentGeometry *		dgTo,
				const DocumentGeometry *	dgFrom );

#   endif
