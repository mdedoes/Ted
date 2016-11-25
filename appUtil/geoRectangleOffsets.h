/************************************************************************/
/*									*/
/*  Offsets inside or around a rectangle. (Padding, Margins)		*/
/*									*/
/************************************************************************/

#   ifndef	GEO_RECTANGLE_OFFSETS_H
#   define	GEO_RECTANGLE_OFFSETS_H

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
