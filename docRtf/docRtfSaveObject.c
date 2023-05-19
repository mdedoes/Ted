/************************************************************************/
/*									*/
/*  Save an inserted object to RTF					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<bitmap.h>

#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	"docRtfControlWord.h"
#   include	<docObjectProperties.h>
#   include	<docObject.h>
#   include	"docRtfWriterImpl.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docRtfWriteOleOobject(	RtfWriter *			rw,
					const InsertedObject *		io )
    {
    const int	scope= RTCscopeOBJ;

    docRtfWriteDestinationBegin( rw, "object" );

    if  ( docRtfWriteProperty( rw, scope,
				IOpropEMBED_KIND, io->ioRtfEmbedKind ) )
	{ LLLDEB(scope,IOpropEMBED_KIND,io->ioRtfEmbedKind); return -1;}

    if  ( io->ioRtfResultKind != RESULTkindUNKNOWN )
	{
	if  ( docRtfWriteProperty( rw, scope,
			    IOpropRESULT_KIND, io->ioRtfResultKind ) )
	    {
	    LLLDEB(scope,IOpropRESULT_KIND,io->ioRtfResultKind);
	    return -1;
	    }
	}

    if  ( ! utilMemoryBufferIsEmpty( &(io->ioObjectClass) ) )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedBufferDestination( rw, "*\\objclass",
				&(io->ioObjectClass), addSemicolon );
	}

    if  ( ! utilMemoryBufferIsEmpty( &(io->ioObjectName) ) )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedBufferDestination( rw, "*\\objname",
				&(io->ioObjectName), addSemicolon );
	}

    if  ( docRtfWriteProperty( rw, scope,
			    IOpropOBJTWIPS_WIDE, io->ioTwipsWide ) )
	{
	LLLDEB(scope,IOpropOBJTWIPS_WIDE,io->ioTwipsWide);
	return -1;
	}
    if  ( docRtfWriteProperty( rw, scope,
			    IOpropOBJTWIPS_HIGH, io->ioTwipsHigh ) )
	{
	LLLDEB(scope,IOpropOBJTWIPS_HIGH,io->ioTwipsHigh);
	return -1;
	}
    if  ( io->ioScaleXSet != 100 )
	{
	if  ( docRtfWriteProperty( rw, scope,
			    IOpropOBJSCALE_X, io->ioScaleXSet ) )
	    {
	    LLLDEB(scope,IOpropOBJSCALE_X,io->ioScaleXSet);
	    return -1;
	    }
	}
    if  ( io->ioScaleYSet != 100 )
	{
	if  ( docRtfWriteProperty( rw, scope,
			    IOpropOBJSCALE_Y, io->ioScaleYSet ) )
	    {
	    LLLDEB(scope,IOpropOBJSCALE_Y,io->ioScaleYSet);
	    return -1;
	    }
	}

    docRtfWriteDestinationBegin( rw, "*\\objdata" );
    docRtfWriteMemoryBuffer( rw, &(io->ioObjectData) );
    docRtfWriteNextLine( rw );
    docRtfWriteDestinationEnd( rw );

    switch( io->ioResultKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokPICTEMFBLIP:

	    docRtfWriteDestinationBegin( rw, RTFtag_result );

	    if  ( docRtfSavePicture( rw, &(io->ioPictureProperties),
						&(io->ioAltText),
						&(io->ioResultData) ) )
		{ SDEB(docObjectKindStr(io->ioResultKind)); return -1; }

	    docRtfWriteDestinationEnd( rw );
	    break;

	default:
	    break;
	}

    docRtfWriteDestinationEnd( rw );

    return 0;
    }

int docRtfSaveObject(		RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				const InsertedObject *		io )
    {
    docRtfWriteNextLine( rw );

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokPICTEMFBLIP:

	    if  ( docRtfSavePicture( rw, &(io->ioPictureProperties),
							&(io->ioAltText),
							&(io->ioObjectData) ) )
		{ SDEB(docObjectKindStr(io->ioKind)); return -1; }
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( docRtfWriteOleOobject( rw, io ) )
		{ SDEB(docObjectKindStr(io->ioKind)); return -1; }
	    return 0;

	case DOCokEPS_FILE:
	    return 0;

	case DOCokDRAWING_SHAPE:
	    if  ( docRtfSaveDrawingShape( rw, io->ioDrawingShape ) )
		{ SDEB(docObjectKindStr(io->ioKind)); return -1; }
	    return 0;

	default:
	    SDEB(docObjectKindStr(io->ioKind));
	    return -1;
	}

    /*  Not reached ..
    return 0;
    */
    }

