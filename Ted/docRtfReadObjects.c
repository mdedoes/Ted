/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"


int docRtfObjectProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    InsertedObject *	io= rrc->rrcInsertedObject;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! io )
	{
	LSXDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInsertedObject);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case IOpropKIND:
	    io->ioKind= rcw->rcwEnumValue;

	    if  ( io->ioKind == DOCokPICTWMETAFILE )
		{ io->ioMapMode= arg;	}

	    break;

	case IOpropRESULT_KIND:
	    io->ioRtfResultKind= rcw->rcwEnumValue;
	    break;

	case IOpropEMBED_KIND:
	    io->ioRtfEmbedKind= rcw->rcwEnumValue;
	    break;

	case IOpropPICT_RESULT_KIND:
	    io->ioResultKind= rcw->rcwEnumValue;

	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{ io->ioResultMapMode= arg;	}

	    break;

	case IOpropPICTWIPS_WIDE:
	    if  ( io->ioTwipsWide == 0 )
		{ io->ioTwipsWide= arg; }
	    else{
		if  ( io->ioTwipsWide- arg >  1	||
		      io->ioTwipsWide- arg < -1	)
		    { LLDEB(io->ioTwipsWide,arg);	}

		io->ioTwipsWide= arg;
		}
	    break;

	case IOpropPICTWIPS_HIGH:
	    if  ( io->ioTwipsHigh == 0 )
		{ io->ioTwipsHigh= arg; }
	    else{
		if  ( io->ioTwipsHigh- arg >  1	||
		      io->ioTwipsHigh- arg < -1	)
		    { LLDEB(io->ioTwipsHigh,arg);	}

		io->ioTwipsHigh= arg;
		}
	    break;

	case IOpropOBJSCALE_X:
	case IOpropPICSCALE_X:
	    if  ( io->ioScaleX == 100 )
		{ io->ioScaleX= arg; }
	    else{
		if  ( io->ioScaleX != arg )
		    { LLDEB(io->ioScaleX,arg);	}

		io->ioScaleX= arg;
		}
	    break;

	case IOpropOBJSCALE_Y:
	case IOpropPICSCALE_Y:
	    if  ( io->ioScaleY == 100 )
		{ io->ioScaleY= arg; }
	    else{
		if  ( io->ioScaleY != arg )
		    { LLDEB(io->ioScaleY,arg);	}

		io->ioScaleY= arg;
		}
	    break;

	case IOpropPICX_WIN_EXT:
	    io->io_xWinExt= arg;
	    break;
	case IOpropPICY_WIN_EXT:
	    io->io_yWinExt= arg;
	    break;

	case IOpropOBJCROP_TOP:
	    io->ioTopCropTwips= arg;
	    break;
	case IOpropOBJCROP_BOTTOM:
	    io->ioBottomCropTwips= arg;
	    break;
	case IOpropOBJCROP_LEFT:
	    io->ioLeftCropTwips= arg;
	    break;
	case IOpropOBJCROP_RIGHT:
	    io->ioRightCropTwips= arg;
	    break;

	case IOpropPICCROP_TOP:
	    io->ioTopCropTwips= arg;
	    break;
	case IOpropPICCROP_BOTTOM:
	    io->ioBottomCropTwips= arg;
	    break;
	case IOpropPICCROP_LEFT:
	    io->ioLeftCropTwips= arg;
	    break;
	case IOpropPICCROP_RIGHT:
	    io->ioRightCropTwips= arg;
	    break;

	case IOpropBLIPTAG:
	    io->ioBliptag= arg;
	    break;

	case IOpropWBMBITSPIXEL:
	    io->ioBmBitsPerPixel= arg;
	    break;
	case IOpropWBMPLANES:
	    io->ioBmPlanes= arg;
	    break;
	case IOpropWBMWIDTHBYTES:
	    io->ioBmBytessPerRow= arg;
	    break;

	case IOpropPICBMP:
	    io->ioMetafileIsBitmap= arg != 0;
	    break;
	case IOpropPICBPP:
	    io->ioMetafileBitmapBpp= arg;
	    break;

	case IOprop_UNSUPPORTED:
	    break;

	default:
	    /* SLDEB(rcw->rcwWord,rcw->rcwID); */
	    break;
	}

    return 0;
    }

static RtfControlWord	docRtfResultPictWords[]=
    {
	{ "wmetafile",	IOpropPICT_RESULT_KIND,	DOClevANY, docRtfObjectProperty,
							DOCokPICTWMETAFILE },

	{ 0, 0, 0 }
    };

static int docRtfSaveObjectData(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len )
    {
    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( docObjectSetData( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveResultData(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetResultData( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

int docRtfReadPict(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    RtfReadingState *		externRrs= rrc->rrcState;
    RtfReadingState		internRrs;

    int				rval;
    TextParticule *		tp;

    BufferItem *		bi= rrc->rrcBi;
    int				off= bi->biParaStrlen;
    InsertedObject *		ioSave= rrc->rrcInsertedObject;

    /*
    LSLDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInIgnoredGroup);
    */

    if  ( rrc->rrcInIgnoredGroup == 0 )
	{
	if  ( docInflateTextString( bi, 1 ) )
	    { LLDEB(bi->biParaStrlen,1); return -1; }

	bi->biParaString[off]= ' ';
	bi->biParaStrlen++;
	bi->biParaString[bi->biParaStrlen]= '\0';

	tp= docInsertObject( rrc->rrcBd, bi, &(rrc->rrcInsertedObject),
				    -1, off, &(externRrs->rrsTextAttribute) );
	if  ( ! tp )
	    { LDEB(bi->biParaParticuleCount); return -1;	}
	}

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfEmptyTable,
				docRtfSaveObjectData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInsertedObject= ioSave;
    docRtfPopReadingState( rrc );

    return rval;
    }

static RtfControlWord	docRtfXshppictGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadPict, },
    };

int docRtfReadNonshppict(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				rval;

    rrc->rrcInIgnoredGroup++;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			    (const RtfControlWord *)0, docRtfXshppictGroups,
			    docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInIgnoredGroup--;

    return rval;
    }

int docRtfReadShppict(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			    (const RtfControlWord *)0, docRtfXshppictGroups,
			    docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfReadResultPict(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfResultPictWords, docRtfEmptyTable,
				docRtfSaveResultData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfNonshpResultpictGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadResultPict, },
    };

static int docRtfReadNonshResultpict(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfNonshpResultpictGroups,
				docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfSaveObjectClass(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetObjectClass( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveObjectName(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetObjectName( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfObjectClass(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectClass );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectName(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectName );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectData(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectData );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfResultGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadResultPict, },
	{ "nonshppict",	RTFidNONSHPPICT,DOClevPARA, docRtfReadNonshResultpict, },
	{ 0, 0, 0 }
    };

static int docRtfObjectResult(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfResultGroups,
				docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfObjectGroups[]=
    {
	{ "objname",	RTFidOBJNAME,	DOClevPARA, docRtfObjectName, },
	{ "objclass",	RTFidOBJCLASS,	DOClevPARA, docRtfObjectClass, },
	{ "objdata",	RTFidOBJDATA,	DOClevPARA, docRtfObjectData, },
	{ "result",	RTFidOBJDATA,	DOClevPARA, docRtfObjectResult, },
	{ 0, 0, 0 }
    };

int docRtfReadObject(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    int				rval;

    BufferItem *		bi= rrc->rrcBi;
    int				off= bi->biParaStrlen;
    InsertedObject *		ioSave= rrc->rrcInsertedObject;

    if  ( docInflateTextString( bi, 1 ) )
	{ LLDEB(bi->biParaStrlen,1); return -1; }

    bi->biParaString[off]= ' ';
    bi->biParaStrlen++;
    bi->biParaString[bi->biParaStrlen]= '\0';

    if  ( ! docInsertObject( rrc->rrcBd, bi, &(rrc->rrcInsertedObject), -1,
					    off, &(rrs->rrsTextAttribute) ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    rrc->rrcInsertedObject->ioKind= DOCokOLEOBJECT;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfObjectGroups,
				docRtfSaveObjectData );

    rrc->rrcInsertedObject= ioSave;

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }
