#   ifndef	DRAW_FONT_X11_H		/*  {{	*/
#   define	DRAW_FONT_X11_H

#   include	"guiBase.h"

#   if		FONTS_X11		/*  {{	*/

#   include	"drawFontImpl.h"
#   include	<utilIndexMapping.h>

struct DrawScreenFont;
struct PostScriptFontList;

/************************************************************************/
/*									*/
/*  For the administration of the relation between PostScript fonts and	*/
/*  X11 fonts.								*/
/*									*/
/************************************************************************/

# define X_FONTNAME_MAX	100

typedef struct EncodedScreenFont
    {
    APP_FONT *			esfFontStruct;
    char			esfFontName[X_FONTNAME_MAX+ 1];
    unsigned char		esfIsTwoByte;
    unsigned char		esfFontBorrowed;
    const IndexMapping *	esfSymbolToByteMapping;
    } EncodedScreenFont;

typedef struct EncodedScreenFontList
    {
    EncodedScreenFont *	esfFonts;
    int			esfFontCount;
    IndexMapping	esflSymbolToFont;
    } EncodedScreenFontList;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int appFindX11Fonts(		APP_WIDGET			topw,
					struct PostScriptFontList *	psfl );

extern void drawCleanEncodedScreenFont(	struct DrawScreenFont *	dsf,
					    EncodedScreenFont *	esf );

extern void drawInitEncodedScreenFont(	EncodedScreenFont *	esf );

extern void drawCleanEncodedScreenFontList(	struct DrawScreenFont *	dsf,
						EncodedScreenFontList * esfl );

extern void drawInitEncodedScreenFontList(	EncodedScreenFontList * esfl );

extern void drawX11GetFontProperties(	EncodedScreenFont *	esf,
					struct DrawScreenFont *	dsf );

#   endif	/*	FONTS_X11		}}	*/
#   endif	/*	DRAW_FONT_X11_H		}}	*/
