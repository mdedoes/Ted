/************************************************************************/
/*									*/
/*  Management of the io streams for reading the data of objects.	*/
/*									*/
/************************************************************************/

#   include	"bitmapConfig.h"

#   include	<stdlib.h>

#   include	"bmObjectReader.h"

#   include	"bitmap.h"
#   include	"bmio.h"
#   include	<sioGeneral.h>
#   include	<sioUtil.h>
#   include	<sioHexedMemory.h>

#   include	<appDebugon.h>

int bmReadRasterObject(	RasterImage *			ri,
			bmReadBitmap			readRaster,
			const char *			suggestedExtension,
			const struct MemoryBuffer *	mb )
    {
    int				rval= 0;

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    if  ( (*readRaster)( &(ri->riDescription), &(ri->riBytes), sisData ) )
	{
	if  ( getenv( "TED_SAVE_FAULTY_IMAGES" ) )
	    {
	    char	scratch[50];

	    static int	imageNumber= 0;

	    sioInClose( sisData );
	    sisData= (SimpleInputStream *)0;

	    sisData= sioInHexedMemoryOpen( mb );
	    if  ( ! sisData )
		{ XDEB(sisData); rval= -1; goto ready;	}

	    sprintf( scratch, "/tmp/failed_image_%d.%s",
					    imageNumber++, suggestedExtension );
	    sioCopyInputStreamToFile( scratch, sisData );
	    }

	LDEB(1); rval= -1; goto ready;
	}

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }
