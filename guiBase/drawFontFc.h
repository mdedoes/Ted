#   ifndef	DRAW_FONT_FC_H		/*  {{	*/
#   define	DRAW_FONT_FC_H

#   include	"guiBase.h"

#   if	USE_FONTCONFIG		/*  {{	*/

#   include	<fontconfig/fontconfig.h>
#   include	<ft2build.h>
#   include	FT_FREETYPE_H

struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern FT_Face drawFcGetFace(	const struct AfmFontInfo *	afi );

#   endif	/*	USE_FONTCONFIG		}}	*/
#   endif	/*	DRAW_FONT_FC_H		}}	*/
