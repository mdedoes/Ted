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
#   include	<docObjectProperties.h>
#   include	<docObject.h>
#   include	"docRtfWriterImpl.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

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
							&(io->ioObjectData) ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case DOCokOLEOBJECT:
	    docRtfWriteDestinationBegin( rw, "object" );

	    switch( io->ioRtfEmbedKind )
		{
		case EMBEDkindOBJEMB:
		    docRtfWriteTag( rw, "objemb" );
		    break;
		case EMBEDkindOBJLINK:
		    docRtfWriteTag( rw, "objlink" );
		    break;
		case EMBEDkindOBJAUTLINK:
		    docRtfWriteTag( rw, "objautlink" );
		    break;
		case EMBEDkindOBJSUB:
		    docRtfWriteTag( rw, "objsub" );
		    break;
		case EMBEDkindOBJPUB:
		    docRtfWriteTag( rw, "objpub" );
		    break;
		case EMBEDkindOBJICEMB:
		    docRtfWriteTag( rw, "objicemb" );
		    break;
		case EMBEDkindOBJHTML:
		    docRtfWriteTag( rw, "objhtml" );
		    break;
		case EMBEDkindOBJOCX:
		    docRtfWriteTag( rw, "objocx" );
		    break;
		default:
		    LDEB(io->ioRtfEmbedKind);
		    break;
		}

	    switch( io->ioRtfResultKind )
		{
		case RESULTkindUNKNOWN:
		    break;
		case RESULTkindRTF:
		    docRtfWriteTag( rw, "rsltrtf" );
		    break;
		case RESULTkindTXT:
		    docRtfWriteTag( rw, "rslttxt" );
		    break;
		case RESULTkindPICT:
		    docRtfWriteTag( rw, "rsltpict" );
		    break;
		case RESULTkindBMP:
		    docRtfWriteTag( rw, "rsltbmp" );
		    break;
		case RESULTkindHTML:
		    docRtfWriteTag( rw, "rslthtml" );
		    break;
		default:
		    LDEB(io->ioRtfResultKind);
		    break;
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

	    docRtfWriteArgTag( rw, "objw", io->ioTwipsWide );
	    docRtfWriteArgTag( rw, "objh", io->ioTwipsHigh );
	    if  ( io->ioScaleXSet != 100 )
		{ docRtfWriteArgTag( rw, "objscalex", io->ioScaleXSet ); }
	    if  ( io->ioScaleYSet != 100 )
		{ docRtfWriteArgTag( rw, "objscaley", io->ioScaleYSet ); }

	    docRtfWriteDestinationBegin( rw, "*\\objdata" );
	    docRtfWriteMemoryBuffer( rw, &io->ioObjectData );
	    docRtfWriteNextLine( rw );
	    docRtfWriteDestinationEnd( rw );

	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		docRtfWriteDestinationBegin( rw, RTFtag_result );

		if  ( docRtfSavePicture( rw, &(io->ioPictureProperties),
						    &(io->ioResultData) ) )
		    { LDEB(1); return -1;	}

		docRtfWriteDestinationEnd( rw );
		}

	    docRtfWriteDestinationEnd( rw );
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

