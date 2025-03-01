/************************************************************************/
/*									*/
/*  Include a file from the file system in an object.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	<sioHexedMemory.h>

#   include	<psPrint.h>
#   include	<geoRectangle.h>
#   include	"docObject.h"
#   include	"docObjectIo.h"
#   include	"docObjectProperties.h"
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save an included Eps file to a memory buffer and determine its	*/
/*  bounding box on the fly.						*/
/*									*/
/************************************************************************/

int docReadEpsObject(	const MemoryBuffer *	fullName,
			InsertedObject *	io )
    {
    int				rval= 0;
    int				res= 0;

    DocumentRectangle		drBBox;

    SimpleOutputStream *	sosData= (SimpleOutputStream *)0;

    sosData= sioOutHexedMemoryOpen( &(io->ioResultData) );
    if  ( ! sosData )
	{ XDEB(sosData); rval= -1; goto ready;	}

    res= psSaveEpsFile( sosData, &drBBox, fullName );
    if  ( res )
	{ LDEB(res); rval= -1; goto ready;	}

    io->ioResultKind= DOCokEPS_FILE;

    if  ( drBBox.drX0 < 0 )
	{ io->ioTwipsWide= 20* ( drBBox.drX1- drBBox.drX0 );	}
    else{ io->ioTwipsWide= 20* ( drBBox.drX1 );			}

    if  ( drBBox.drY0 < 0 )
	{ io->ioTwipsHigh= 20* ( drBBox.drY1- drBBox.drY0 );	}
    else{ io->ioTwipsHigh= 20* ( drBBox.drY1 );			}

    if  ( utilCopyMemoryBuffer( &(io->ioObjectData), fullName ) )
	{ return -1;	}

    io->ioKind= DOCokEPS_FILE;

 ready:

    if  ( sosData )
	{ sioOutClose( sosData );	}

    return rval;
    }
