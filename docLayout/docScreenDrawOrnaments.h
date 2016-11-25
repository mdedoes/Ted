/************************************************************************/
/*									*/
/*  Ted, Drawing utility methods shared between tool and document.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_DRAW_ORNAMENTS_H
#   define	DOC_SCREEN_DRAW_ORNAMENTS_H

#   include	<drawDrawingSurface.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedDrawHorizontalBorderLine(DrawingSurface		ds,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y );

#   endif	/*  DOC_SCREEN_DRAW_ORNAMENTS_H	*/
