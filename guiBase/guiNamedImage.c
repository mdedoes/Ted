/************************************************************************/
/*									*/
/*  Retrieve images.							*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"guiBase.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	<bitmap.h>
#   include	<bmio.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	"guiNamedImages.h"
#   include	"guiNamedImage.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get a pixmap.							*/
/*									*/
/************************************************************************/

int guiMakeImageRasterImage(	NamedImage *		ni )
    {
    int			rval= 0;
    SimpleInputStream *	sis= (SimpleInputStream *)0;

    RasterImage *	ri= (RasterImage *)0;

    ri= (RasterImage *)malloc( sizeof(RasterImage) );
    if  ( ! ri )
	{ XDEB(ri); rval= -1; goto ready;	}

    bmInitRasterImage( ri );

    sis= sioInMemoryArrayOpen( ni->niBuffers, ni->niBufferCount );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    if  ( bmPngReadPng( &(ri->riDescription), &(ri->riBytes), sis ) )
	{ SDEB(ni->niPictureName); rval= -1; goto ready;	}

    ni->niRasterImage= ri; ri= (RasterImage *)0; /* steal */

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    if  ( ri )
	{
	bmCleanRasterImage( ri );
	free( ri );
	}

    return rval;
    }

int guiGetNamedImage(	struct NamedImage *		namedImages,
			int				namedImageCount,
			const char *			name,
			struct NamedImage **		pImage )
    {
    int			i;
    NamedImage *	ni;

    ni= namedImages;
    for ( i= 0; i < namedImageCount; ni++, i++ )
	{
	if  ( ! strcmp( name, ni->niPictureName ) )
	    { break;	}
	}

    if  ( i >= namedImageCount )
	{ SLDEB(name,namedImageCount); return -1;	}

    *pImage= ni;
    return 0;
    }

void guiCleanNamedImages(
			struct NamedImage *		namedImages,
			int				namedImageCount )
    {
    int			i;
    NamedImage *	ni;

    ni= namedImages;
    for ( i= 0; i < namedImageCount; ni++, i++ )
	{
	if  ( ni->niRasterImage )
	    {
	    bmCleanRasterImage( ni->niRasterImage );
	    free( ni->niRasterImage );

	    ni->niRasterImage= (RasterImage *)0;
	    }

	guiCleanNamedImageImpl( ni );
	}

    return;
    }
