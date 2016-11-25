#   ifndef		DRAW_IMPL_H
#   define		DRAW_IMPL_H

#   if USE_FONTCONFIG
#	include	<ft2build.h>
#	include	FT_FREETYPE_H
#   endif

#   if DRAW_XFT || FONTS_XFT
#	include	<X11/Xft/Xft.h>
#	include	<X11/extensions/Xrender.h>
#   endif

#   endif	/*	DRAW_IMPL_H	*/
