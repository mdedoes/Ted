/************************************************************************/
/*									*/
/*  Management of the io streams for reading the data of objects.	*/
/*									*/
/************************************************************************/

#   include	"bitmapConfig.h"

#   include	<stdlib.h>

#   include	"bmObjectReader.h"

#   include	<sioHex.h>
#   include	<sioMemory.h>
#   include	"bitmap.h"
#   include	"bmio.h"
#   include	<sioGeneral.h>
#   include	<sioUtil.h>

#   include	<appDebugon.h>

void bmInitObjectReader(	ObjectReader *		or )
    {
    or->orSisMem= (SimpleInputStream *)0;
    or->orSisHex= (SimpleInputStream *)0;

    return;
    }

void bmCleanObjectReader(	ObjectReader *		or )
    {
    if  ( or->orSisHex )
	{ sioInClose( or->orSisHex );	}

    if  ( or->orSisMem )
	{ sioInClose( or->orSisMem );	}
    }

int bmOpenObjectReader(	ObjectReader *			or,
			const struct MemoryBuffer *	mb )
    {
    or->orSisMem= sioInMemoryOpen( mb );
    if  ( ! or->orSisMem )
	{ XDEB(or->orSisMem); return -1;	}

    or->orSisHex= sioInHexOpen( or->orSisMem );
    if  ( ! or->orSisHex )
	{ XDEB(or->orSisHex); return -1;	}

    return 0;
    }

int bmReadRasterObject(	RasterImage *			ri,
			bmReadBitmap			readRaster,
			const char *			suggestedExtension,
			const struct MemoryBuffer *	mb )
    {
    int			rval= 0;

    ObjectReader	or;

    bmInitObjectReader( &or );

    if  ( bmOpenObjectReader( &or, mb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( (*readRaster)( &(ri->riDescription), &(ri->riBytes), or.orSisHex ) )
	{
	if  ( getenv( "TED_SAVE_FAULTY_IMAGES" ) )
	    {
	    char	scratch[50];

	    static int	imageNumber= 0;

	    bmCleanObjectReader( &or );
	    bmInitObjectReader( &or );

	    if  ( bmOpenObjectReader( &or, mb ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    sprintf( scratch, "/tmp/failed_image_%d.%s",
					    imageNumber++, suggestedExtension );
	    sioCopyInputStreamToFile( scratch, or.orSisHex );
	    }

	LDEB(1); rval= -1; goto ready;
	}

  ready:

    bmCleanObjectReader( &or );

    return rval;
    }
