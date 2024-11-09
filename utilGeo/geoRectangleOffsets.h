/************************************************************************/
/*									*/
/*  Offsets inside or around a rectangle. (Padding, Margins)		*/
/*									*/
/************************************************************************/

#   ifndef	GEO_RECTANGLE_OFFSETS_H
#   define	GEO_RECTANGLE_OFFSETS_H

/**
 * The margins inside a rectangle. Comparable to _padding_ in html/css.
 */
typedef struct RectangleOffsets
    {
    int		roLeftOffset;
    int		roTopOffset;
    int		roRightOffset;
    int		roBottomOffset;
    } RectangleOffsets;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void geoInitRectangleOffsets(	RectangleOffsets *	ro );

#   endif
