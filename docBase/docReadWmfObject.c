/************************************************************************/
/*									*/
/*  Obtain the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	<sioFileio.h>
#   include	<sioHexedMemory.h>
#   include	<sioEndian.h>
#   include	<bmWmfIo.h>
#   include	<sioHexedMemory.h>

#   include	"docObject.h"
#   include	"docObjectIo.h"
#   include	"docObjectProperties.h"
#   include	<sioGeneral.h>
#   include	<sioUtil.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docSizeFromWmfObject(	InsertedObject *	io,
					const WmfFileHeader *	wfh,
					long			key,
					int			ret )
    {
    PictureProperties *		pip= &(io->ioPictureProperties);

    if  ( ret > 0 )
	{
	if  ( key == 0x90001 )
	    {
	    pip->pipTwipsWide= 20* 72* 4;		/*  10 cm	*/
	    pip->pipTwipsHigh= 20* 72* 4;		/*  10 cm	*/
	    pip->pip_xWinExt= 72* 4;
	    pip->pip_yWinExt= 72* 4;

	    io->ioTwipsWide= pip->pipTwipsWide;
	    io->ioTwipsHigh= pip->pipTwipsHigh;
	    }
	else{ XDEB(key); return -1;	}
	}

    if  ( ret == 0 )
	{
	const DocumentRectangle *	dr= &(wfh->wfhBounds);

	if  ( dr->drX1 > dr->drX0 )
	    {
	    pip->pipTwipsWide= ( 20* 72* ( dr->drX1- dr->drX0 ) )/
						    wfh->wfhUnitsPerInch;
	    }
	else{
	    pip->pipTwipsWide= ( 20* 72* ( dr->drX0- dr->drX1 ) )/
						    wfh->wfhUnitsPerInch;
	    }

	if  ( dr->drY1 > dr->drY0 )
	    {
	    pip->pipTwipsHigh= ( 20* 72* ( dr->drY1- dr->drY0 ) )/
						    wfh->wfhUnitsPerInch;
	    }
	else{
	    pip->pipTwipsHigh= ( 20* 72* ( dr->drY0- dr->drY1 ) )/
						    wfh->wfhUnitsPerInch;
	    }

	io->ioTwipsWide= pip->pipTwipsWide;
	io->ioTwipsHigh= pip->pipTwipsHigh;

	pip->pip_xWinExt= dr->drX1- dr->drX0;
	pip->pip_yWinExt= dr->drY0- dr->drY1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a metafile object from file.					*/
/*									*/
/************************************************************************/

int docReadWmfObject(		InsertedObject *	io,
				const MemoryBuffer *	filename )
    {
    WmfFileHeader		wfh;
    unsigned long		key;
    int				ret;
    int				rval= 0;

    MemoryBuffer		mb;

    SimpleInputStream *		sisIn= (SimpleInputStream *)0;
    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;

    PictureProperties *		pip= &(io->ioPictureProperties);

    utilInitMemoryBuffer( &mb );

    sisIn= sioInFileioOpen( filename );
    if  ( ! sisIn )
	{ XDEB(sisIn); rval= -1; goto ready;	}

    sosData= sioOutHexedMemoryOpen( &mb );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    ret= bmMetaReadWmfFileHeader( &key, &wfh, sisIn );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}

    if  ( docSizeFromWmfObject( io, &wfh, key, ret ) )
	{ XDEB(key); rval= -1; goto ready;	}

    if  ( ret > 0 &&  key == 0x90001 )
	{ sioEndianPutLeUint32( key, sosData );	}

    if  ( sioCopyStream( sosData, sisIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutClose( sosData ); sosData= (SimpleOutputStream *)0; /*flush,steal*/

    io->ioObjectData= mb; utilInitMemoryBuffer( &mb ); /* steal */
    io->ioKind= DOCokPICTWMETAFILE;
    pip->pipType= DOCokPICTWMETAFILE;
    pip->pipMapMode= MM_ANISOTROPIC;

  ready:

    utilCleanMemoryBuffer( &mb );

    if  ( sosData )
	{ sioOutClose( sosData );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

int docReadWmfSize(		InsertedObject *	io,
				const MemoryBuffer *	mb )
    {
    int			rval= 0;
    int			ret;

    WmfFileHeader	wfh;
    unsigned long	key;

    SimpleInputStream *	sisData= (SimpleInputStream *)0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    ret= bmMetaReadWmfFileHeader( &key, &wfh, sisData );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}

    if  ( docSizeFromWmfObject( io, &wfh, key, ret ) )
	{ XDEB(key); rval= -1; goto ready;	}

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

