/************************************************************************/
/*									*/
/*  A simple mechanism to find XImages and Pixmaps by name.		*/
/*									*/
/************************************************************************/

#   ifndef	GUI_NAMED_IMAGE_H
#   define	GUI_NAMED_IMAGE_H

struct MemoryBuffer;
struct RasterImage;

#   include	"guiBase.h"
#   include	"guiImageImpl.h"

typedef struct NamedImage
    {
    const char * const			niPictureName;
    const struct MemoryBuffer * const	niBuffers;
    int					niBufferCount;

    struct RasterImage *		niRasterImage;

#   if USE_MOTIF

    APP_BITMAP_IMAGE	niPixmap;
    APP_BITMAP_MASK	niMask;
    Display *		niDisplay;

#   endif

#   if USE_GTK

#   if GTK_MAJOR_VERSION < 3

    APP_BITMAP_IMAGE	niPixmap;
    APP_BITMAP_MASK	niMask;

#   else

    APP_IMAGE *		niImage;

#   endif
#   endif

    } NamedImage;

extern int guiMakeImageRasterImage(	NamedImage *	ni );

extern void guiCleanNamedImageImpl(	NamedImage *	ni );

#   endif
