#   ifndef	DRAW_FONT_IMPL_H
#   define	DRAW_FONT_IMPL_H

#   include	"guiBase.h"

#   if DRAW_GDK
    typedef		GdkFont			(APP_FONT);
#   endif

#   if DRAW_CAIRO
    /*  We do not use an allocated object with cairo.  */
    typedef cairo_scaled_font_t			(APP_FONT);
#   endif

#   if DRAW_X11
    typedef		XFontStruct		(APP_FONT);
#   endif

#   endif
