/************************************************************************/
/*									*/
/*  Open the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	<appImage.h>
#   include	<appWinMeta.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

static int docGetBitmap(	bmReadBitmap		readBitmap,
				InsertedObject *	io,
				const MemoryBuffer *	mb )
    {
    int			res;
    AppBitmapImage *	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );

    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisBitmap;

    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisMem); return -1;	}

    res= (*readBitmap)( &(abi->abiBitmap), &(abi->abiBuffer), sisBitmap );

    sioInClose( sisBitmap );
    sioInClose( sisMem );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0;		}

    io->ioPrivate= (void *)abi;

    return 0;
    }

int docGetBitmapForObject(	InsertedObject *	io )
    {
    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( appMetaPlayWmfImg,
						io, &io->ioObjectData )	)
		{ LDEB(1); return 0;	}
	    break;

	case DOCokPICTPNGBLIP:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( bmPngReadPng,
					    io, &io->ioObjectData )	)
		{ LDEB(1); return 0;	}
	    break;

	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( bmJpegReadJfif,
					    io, &io->ioObjectData )	)
		{ LDEB(1); return 0;	}

	    break;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( ! io->ioPrivate					&&
		      docGetBitmap( appMetaPlayWmfImg,
					    io, &io->ioResultData )	)
		    { LDEB(1); return 0;	}

		return 0;
		}
	    return 0;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

