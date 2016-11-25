/************************************************************************/
/*									*/
/*  GTK definitions for guiImageImpl.h. NEVER INCLUDE DIRECTLY, BUT	*/
/*  VIA guiImageImpl.h							*/
/*									*/
/************************************************************************/

# if USE_GTK	/*  { */

#   include	<gdk/gdk.h>

#   if GTK_MAJOR_VERSION < 3

    typedef		GdkPixmap		(*APP_BITMAP_IMAGE);
    typedef		GdkBitmap		(*APP_BITMAP_MASK);
    typedef		GdkImage		(APP_IMAGE);

#   else

    typedef		cairo_t			(*APP_BITMAP_IMAGE);
    typedef		cairo_t			(*APP_BITMAP_MASK);
    typedef		GdkPixbuf		(APP_IMAGE);

#   endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

# endif		/*  } USE_GTK */

