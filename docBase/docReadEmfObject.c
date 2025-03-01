/************************************************************************/
/*									*/
/*  Obtain the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	<sioFileio.h>
#   include	<sioHexedMemory.h>
#   include	<bitmap.h>
#   include	<bmEmfIo.h>
#   include	<geoUnits.h>

#   include	"docObject.h"
#   include	"docObjectIo.h"
#   include	"docObjectProperties.h"
#   include	<sioHexedMemory.h>
#   include	<sioGeneral.h>
#   include	<sioUtil.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docSizeFromEmfObject(	InsertedObject *	io,
					const EmfHeader *	eh )
    {
    PictureProperties *		pip= &(io->ioPictureProperties);
    const DocumentRectangle *	dr;

    dr= &(eh->ehFrame);

    if  ( dr->drX1 > dr->drX0 )
	{
	pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX1- dr->drX0 ) )/ 100000;
	}
    else{
	pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX0- dr->drX1 ) )/ 100000;
	}

    if  ( dr->drY1 > dr->drY0 )
	{
	pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY1- dr->drY0 ) )/ 100000;
	}
    else{
	pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY0- dr->drY1 ) )/ 100000;
	}

    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    dr= &(eh->ehBounds);
    if  ( dr->drX1 > dr->drX0 )
	{ pip->pip_xWinExt= dr->drX1- dr->drX0;	}
    else{ pip->pip_xWinExt= dr->drX0- dr->drX1;	}

    if  ( dr->drY1 > dr->drY0 )
	{ pip->pip_yWinExt= dr->drY1- dr->drY0;	}
    else{ pip->pip_yWinExt= dr->drY0- dr->drY1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a metafile object from file.					*/
/*									*/
/************************************************************************/

int docReadEmfObject(		InsertedObject *	io,
				const MemoryBuffer *	filename )
    {
    EmfHeader			eh;
    int				rval= 0;

    MemoryBuffer		mb;

    SimpleInputStream *		sisIn= (SimpleInputStream *)0;
    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosMeta= (SimpleOutputStream *)0;

    PictureProperties *		pip= &(io->ioPictureProperties);
    const DocumentRectangle *	dr;

    utilInitMemoryBuffer( &mb );

    sisIn= sioInFileioOpen( filename );
    if  ( ! sisIn )
	{ XDEB(sisIn); rval= -1; goto ready;	}

    sosData= sioOutHexedMemoryOpen( &mb );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    if  ( bmMetaReadEmfHeader( &eh, sisIn ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    dr= &(eh.ehFrame);

    if  ( dr->drX1 > dr->drX0 )
	{ pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX1- dr->drX0 ) )/ 100000;	}
    else{ pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX0- dr->drX1 ) )/ 100000;	}

    if  ( dr->drY1 > dr->drY0 )
	{ pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY1- dr->drY0 ) )/ 100000;	}
    else{ pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY0- dr->drY1 ) )/ 100000;	}

    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    dr= &(eh.ehBounds);
    if  ( dr->drX1 > dr->drX0 )
	{ pip->pip_xWinExt= dr->drX1- dr->drX0;	}
    else{ pip->pip_xWinExt= dr->drX0- dr->drX1;	}

    if  ( dr->drY1 > dr->drY0 )
	{ pip->pip_yWinExt= dr->drY1- dr->drY0;	}
    else{ pip->pip_yWinExt= dr->drY0- dr->drY1;	}

    if  ( bmMetaWriteEmfHeader( &eh, sosMeta ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( sioCopyStream( sosMeta, sisIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutClose( sosData ); sosData= (SimpleOutputStream *)0; /*flush,steal*/

    io->ioObjectData= mb; utilInitMemoryBuffer( &mb ); /* steal */
    io->ioKind= DOCokPICTEMFBLIP;
    pip->pipType= DOCokPICTEMFBLIP;
    pip->pipMapMode= 8; /*MM_ANISOTROPIC*/

  ready:

    utilCleanMemoryBuffer( &mb );

    if  ( sosData )
	{ sioOutClose( sosData );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

int docReadEmfSize(		InsertedObject *	io,
				const MemoryBuffer *	mb )
    {
    int			rval= 0;

    EmfHeader		eh;

    SimpleInputStream *	sisData= (SimpleInputStream *)0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    if  ( bmMetaReadEmfHeader( &eh, sisData ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSizeFromEmfObject( io, &eh ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

