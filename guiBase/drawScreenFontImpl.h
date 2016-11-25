#   ifndef	DRAW_SCREEN_FONT_IMPL_H
#   define	DRAW_SCREEN_FONT_IMPL_H

#   include	"guiBase.h"
#   include	"drawImpl.h"
#   include	"drawFontImpl.h"
#   include	<drawDrawingSurface.h>
#   include	<utilMemoryBuffer.h>

#   if FONTS_X11
#   include		"drawFontX11.h"
#   endif

struct IndexSet;
struct AfmFontInfo;

#   define	DSFkeyFACE_NUMBER	0
#   define	DSFkeySIZE_PIXELS	1
#   define	DSFkey_COUNT		2

typedef struct DrawScreenFont
    {
				/*  KEY  */
    short int			apfPsFaceNumber;
    short int			dsfSizePixels;

				/*  DEP  */
    const struct AfmFontInfo *	dsfPsFontInfo;
    int				dsfUnderlinePositionPixels;
    int				dsfUnderlineThicknessPixels;
    int				dsfStrikethroughPositionPixels;
    int				dsfStrikethroughThicknessPixels;
    int				apfSuperBaseline;
    int				apfSubBaseline;
    int				apfXHeightPixels;

#   if FONTS_X11
    MemoryBuffer		dsfFontNameX11;
    EncodedScreenFontList	dsfEncodedFonts;
#   endif

    const struct IndexSet *	dsfUnicodesUsed;

#   if DRAW_X11 || FONTS_XFT
    Display *			dsfDisplay;
#   endif

#   if DRAW_X11 || DRAW_GDK
    APP_DRAWABLE		dsfDrawable;
    APP_GC			dsfGc;
#   endif

#   if DRAW_XFT
    XftDraw *			dsfXftDrawable;
#   endif

#   if FONTS_XFT
    XftFont *			dsfXftFont;
#   endif

#   if DRAW_CAIRO
    FT_Face			dsfFT_Face;

    /* NO! Resources are allocated for every drawing action
    cairo_font_face_t *		dsfCairoFace;
    cairo_scaled_font_t *	dsfCairoScaledFont;
    */
#   endif

    } DrawScreenFont;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void drawCleanScreenFont(	DrawScreenFont * 	dsf );
extern void drawInitScreenFont(		DrawScreenFont *	dsf );

extern int drawGetScreenFontKey(	const DrawScreenFont *	dsf,
					int			prop );

extern int drawFontOpenScreenFont(	DrawScreenFont *	dsf,
					int			avoidFc );
#   if ! USE_HEADLESS

extern int guiSystemFont(		DrawingSurface		ds,
					APP_FONT *		xfs );

#   endif

#   if FONTS_XFT

    extern XftFont * drawOpenXftFont(	DrawScreenFont *	dsf );

#   endif

extern int drawTextExtentsXft(		struct DocumentRectangle * drText,
					int			x,
					int			y,
					const DrawScreenFont *	hDsf,
					const DrawScreenFont *	vDsf,
					const char *		s,
					int			len );

#   if FONTS_X11

    extern int appFontOpenX11Fonts(		DrawScreenFont *	dsf );

#   endif

#   if DRAW_CAIRO

    cairo_scaled_font_t * drawOpenScreenFontCairo(
						const DrawingSurface	ds,
						DrawScreenFont *	dsf );

#   endif

#   endif
