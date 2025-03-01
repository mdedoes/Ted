#   ifndef		DRAW_IMPL_XFT_H
#   define		DRAW_IMPL_XFT_H

#   include	"drawImpl.h"

struct DrawScreenFont;
struct AppXftColorList;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

#   if DRAW_XFT

extern int drawStringXft(		const struct DrawScreenFont *	dsf,
					struct AppXftColorList *	axcl,
					int			x,
					int			y,
					const char *		s,
					int			len );

extern int drawFillRectangleXft( XftDraw *			draw,
				struct AppXftColorList *	axcl,
				const struct DocumentRectangle *	dr );

# if USE_MOTIF
    extern XftDraw * appMotifXftDrawCreate(
					Display *		display,
					int			screen,
					Drawable		drawable,
					struct AppXftColorList *	xftColorList );
# endif

#   endif

#   endif	/*	DRAW_IMPL_XFT_H	*/
