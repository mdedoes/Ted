/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>
#   include	<string.h>
#   include	<bitmap.h>

#   include	<docObjectProperties.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<docObject.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfParaInsertType.h"
#   include	"docRtfTags.h"
#   include	<docBuf.h>
#   include	<docObjects.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

int docRtfObjectProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    InsertedObject *	io;

    io= rr->rrInsertedObject;
    if  ( ! io )
	{
	LLSXDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,rr->rrInsertedObject);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case IOpropOBJSCALE_X:
	    if  ( arg < 1 )
		{ LDEB(arg); arg= 1;	}
	    if  ( docSetObjectProperty( io, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}
	    io->ioScaleXUsed= io->ioScaleXSet;
	    break;

	case IOpropOBJSCALE_Y:
	    if  ( arg < 1 )
		{ LDEB(arg); arg= 1;	}
	    if  ( docSetObjectProperty( io, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}
	    io->ioScaleYUsed= io->ioScaleYSet;
	    break;

	case IOprop_UNSUPPORTED:
	    break;

	default:
	    if  ( docSetObjectProperty( io, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    break;
	}

    return 0;
    }

int docRtfPictureProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    PictureProperties *	pip= &(rr->rrPictureProperties);

    switch( rcw->rcwID )
	{
	case PIPpropTYPE:
	    /* Exception to cover \wmetafile8 */
	    if  ( docSetPictureProperty( pip, rcw->rcwID, rcw->rcwEnumValue ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    if  ( pip->pipType == DOCokPICTWMETAFILE )
		{ pip->pipMapMode= arg;	}
	    break;

	case PIPpropPICSCALE_X:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    pip->pipScaleXUsed= pip->pipScaleXSet;
	    break;
	case PIPpropPICSCALE_Y:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    pip->pipScaleYUsed= pip->pipScaleYSet;
	    break;

	default:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    break;
	}

    PROPmaskADD( &(rr->rrPicturePropMask), rcw->rcwID );

    return 0;
    }

static int docRtfObjectSetData(	RtfReader *		rr,
				int			prop,
				const char *		text,
				int			len )
    {
    InsertedObject *	io= rr->rrInsertedObject;

    if  ( ! io )
	{
	LLLXDEB(rr->rrCurrentLine,rr->rrBytesRead,prop,rr->rrInsertedObject);
	return -1;
	}

    if  ( docObjectSetData( io, prop, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveObjectData(	RtfReader *		rr,
					const char *		text,
					int			len )
    { return docRtfObjectSetData( rr, IOprop_OBJDATA, text, len );	}

/*
static int docRtfSaveResultData(	RtfReader *		rr,
					const char *		text,
					int			len )
    { return docRtfObjectSetData( rr, IOprop_RESULT, text, len );	}
*/

static int docRtfSaveObjectClass(	RtfReader *		rr,
					const char *		text,
					int			len )
    { return docRtfObjectSetData( rr, IOprop_OBJCLASS, text, len );	}

static int docRtfSaveObjectName(	RtfReader *		rr,
					const char *		text,
					int			len )
    { return docRtfObjectSetData( rr, IOprop_OBJNAME, text, len );	}

static void docRtfCheckPictMagicNumber(	RtfReader *		rr,
					const char *		text,
					int			len )
    {
    if  ( len >= 16 && ! memcmp( text, "89504e470d0a1a0a", 16 ) )
	{
	if  ( rr->rrPictureProperties.pipType != DOCokPICTPNGBLIP )
	    {
	    appDebug( "Image at line %d looks like a PNG image\n",
							rr->rrCurrentLine );
	    rr->rrPictureProperties.pipType= DOCokPICTPNGBLIP;
	    }
	}

    if  ( len >= 4				&&
	  ! memcmp( text, "ffd8", 4 )		&&
	  ! memcmp( text+ len- 4, "ffd9", 4 )	)
	{
	if  ( rr->rrPictureProperties.pipType != DOCokPICTJPEGBLIP )
	    {
	    appDebug( "Image at line %d looks like a JPEG image\n",
							rr->rrCurrentLine );
	    rr->rrPictureProperties.pipType= DOCokPICTJPEGBLIP;
	    }
	}
    }

static int docRtfSavePictResultData(	RtfReader *		rr,
					const char *		text,
					int			len )
    {
    docRtfCheckPictMagicNumber( rr, text, len );

    return docRtfObjectSetData( rr, IOprop_RESULT, text, len );
    }

static int docRtfSavePictObjectData(	RtfReader *		rr,
					const char *		text,
					int			len )
    {
    docRtfCheckPictMagicNumber( rr, text, len );

    return docRtfObjectSetData( rr, IOprop_OBJDATA, text, len );
    }

/************************************************************************/

int docRtfReadStartObject(	InsertedObject **	pIo,
				RtfReader *		rr )
    {
    int			objectNumber;
    InsertedObject *	io= docClaimObject( &objectNumber, rr->rrDocument );

    if  ( ! io )
	{ XDEB(io); return -1; }

    rr->rrInsertedObject= io;
    *pIo= io;

    return objectNumber;
    }

/************************************************************************/
/*									*/
/*  Consume picture property tags.					*/
/*									*/
/*  In particular:							*/
/*	shplid:	The identification of the shape of the picture.		*/
/*	sp:	Various shape properties for the picture.		*/
/*  We make a drawing shape to receive the properties.			*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfPicpropGroups[]=
    {
	RTF_DEST_XX( "sp", RTCscopePICT, 1,	docRtfShapeProperty ),

	{ (char *)0, 0, 0 }
    };

static int docRtfReadPicprop(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			rval;
    DrawingShape *	parent= rr->rrDrawingShape;
    DrawingShape *	ds;

    ds= docClaimDrawingShape( &(rr->rrDocument->bdShapeList) );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    ds->dsDrawing.sdShapeType= rcw->rcwID;

    rr->rrDrawingShape= ds;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
				rr, docRtfPicpropGroups, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    /* ? */
    docDeleteDrawingShape( rr->rrDocument, ds );

    rr->rrDrawingShape= parent;
    return rval;
    }

static RtfControlWord	docRtfPictGroups[]=
    {
	RTF_DEST_XX( "picprop",	RTCscopePICT, SHPtyPICPROP, docRtfReadPicprop ),

	{ (char *)0, 0, 0 }
    };

int docRtfReadPict(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    int				rval= 0;

    RtfReadingState *		externRrs= rr->rrState;
    RtfReadingState		internRrs;

    struct BufferItem *		paraNode;
    struct InsertedObject *	ioSave= rr->rrInsertedObject;
    RtfTreeStack *		rts= rr->rrTreeStack;

    InsertedObject *		io= (InsertedObject *)0;
    int				objectNumber= -1;

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ XDEB(paraNode); rval= -1; goto ready; }

    utilPropMaskClear( &(rr->rrPicturePropMask) );
    docInitPictureProperties( &(rr->rrPictureProperties) );

    if  ( externRrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, externRrs );	}

    objectNumber= docRtfReadStartObject( &io, rr );
    if  ( objectNumber < 0 )
	{ LDEB(objectNumber); rval= -1; goto ready; }

    docRtfPushReadingState( rr, &internRrs );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1, rr,
				docRtfPictGroups, docRtfSavePictObjectData );

    if  ( rval )
	{ LDEB(rval);	}
    if  ( rr->rrInsertedObject )
	{
	if  ( ! rr->rrAfterInlineShape		||
	      ! docRtfInsideShapeField( rr )	)
	    {
	    const PictureProperties *	pip= &(rr->rrPictureProperties);

	    io->ioKind= pip->pipType;
	    io->ioPictureProperties= *pip;

	    io->ioTwipsWide= pip->pipTwipsWide;
	    io->ioTwipsHigh= pip->pipTwipsHigh;
	    io->ioScaleXSet= pip->pipScaleXSet;
	    io->ioScaleYSet= pip->pipScaleYSet;
	    io->ioScaleXUsed= pip->pipScaleXUsed;
	    io->ioScaleYUsed= pip->pipScaleYUsed;

	    if  ( docParagraphBuilderAppendObject( rts->rtsParagraphBuilder,
			objectNumber, &(externRrs->rrsTextAttribute) ) < 0 )
		{ LDEB(objectNumber); rval= -1; goto ready;	}

	    objectNumber= -1;
	    }
	}

    docRtfPopReadingState( rr );

  ready:

    rr->rrInsertedObject= ioSave;

    if  ( objectNumber >= 0 )
	{ docDeleteObject( rr->rrDocument, objectNumber );	}

    return rval;
    }

int docRtfReadShpXPict(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			rval;
    int			ignore= 0;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    switch( rcw->rcwID )
	{
	case PARAinsertSHPPICT:
	    ignore= 0;
	    break;
	case PARAinsertNONSHPPICT:
	    ignore= 1;
	    break;
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    rr->rrInIgnoredGroup += ignore;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rr, (const RtfControlWord *)0, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rr->rrInIgnoredGroup -= ignore;

    return rval;
    }

static int docRtfReadResultPict(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    int				rval;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1, rr,
				docRtfPictGroups, docRtfSavePictResultData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    if  ( rr->rrInsertedObject )
	{
	InsertedObject *	io;
	PictureProperties *	pip;

	io= rr->rrInsertedObject;
	pip= &(rr->rrPictureProperties);

	io->ioResultKind= pip->pipType;
	io->ioPictureProperties= rr->rrPictureProperties;

	if  ( io->ioTwipsWide < 2 )
	    { io->ioTwipsWide= pip->pipTwipsWide;	}
	if  ( io->ioTwipsHigh < 2 )
	    { io->ioTwipsHigh= pip->pipTwipsHigh;	}
	if  ( io->ioScaleXSet == 100 )
	    {
	    io->ioScaleXSet= pip->pipScaleXSet;
	    io->ioScaleXUsed= pip->pipScaleXUsed;
	    }
	if  ( io->ioScaleYSet == 100 )
	    {
	    io->ioScaleYSet= pip->pipScaleYSet;
	    io->ioScaleYUsed= pip->pipScaleYUsed;
	    }
	}

    return rval;
    }

static RtfControlWord	docRtfReadResultXPpictGroups[]=
    {
	RTF_DEST_XX( RTFtag_pict, RTCscopePICT, PARAinsertPICT,	docRtfReadResultPict ),

	{ (char *)0, 0, 0 }
    };

static int docRtfReadResultXPpict(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    int				rval;
    int				ignore= 0;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    switch( rcw->rcwID )
	{
	case PARAinsertSHPPICT:
	    ignore= 0;
	    break;
	case PARAinsertNONSHPPICT:
	    ignore= 1;
	    break;
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    rr->rrInIgnoredGroup += ignore;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rr, docRtfReadResultXPpictGroups, docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rr->rrInIgnoredGroup -= ignore;

    return rval;
    }

static int docRtfObjectClass(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rr, (RtfControlWord *)0, docRtfSaveObjectClass );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectName(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rr )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rr, (RtfControlWord *)0, docRtfSaveObjectName );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectData(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rr, (RtfControlWord *)0, docRtfSaveObjectData );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfResultGroups[]=
{
    RTF_DEST_XX( RTFtag_pict,	RTCscopeOBJ, PARAinsertPICT,	docRtfReadResultPict ),
    RTF_DEST_XX( "shppict",	RTCscopeOBJ, PARAinsertSHPPICT, docRtfReadResultXPpict ),
    RTF_DEST_XX( "nonshppict",	RTCscopeOBJ, PARAinsertNONSHPPICT, docRtfReadResultXPpict ),

    { (char *)0, 0, 0 }
};

static int docRtfObjectResult(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rr )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
				rr, docRtfResultGroups, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfObjectGroups[]=
{
    RTF_DEST_XX( "objname",	RTCscopeOBJ, IOprop_OBJNAME,
				docRtfObjectName ),
    RTF_DEST_XX( "objclass",	RTCscopeOBJ, IOprop_OBJCLASS,
				docRtfObjectClass ),
    RTF_DEST_XX( "objdata",	RTCscopeOBJ, IOprop_OBJDATA,
				docRtfObjectData ),
    RTF_DEST_XX( RTFtag_result,
			    RTCscopeOBJ, IOprop_RESULT,
				docRtfObjectResult ),

    { (char *)0, 0, 0 }
};

int docRtfReadObject(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrState;
    RtfTreeStack *		rts= rr->rrTreeStack;
    int				rval= 0;

    InsertedObject *		ioSave= rr->rrInsertedObject;

    int				objectNumber= -1;
    InsertedObject *		io= (InsertedObject *)0;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready; }

    utilPropMaskClear( &(rr->rrPicturePropMask) );
    docInitPictureProperties( &(rr->rrPictureProperties) );

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    objectNumber= docRtfReadStartObject( &io, rr );
    if  ( objectNumber < 0 )
	{ SLDEB(rcw->rcwWord,objectNumber); rval= -1; goto ready;	}

    io->ioKind= DOCokOLEOBJECT;

    if  ( docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			    rr, docRtfObjectGroups, docRtfSaveObjectData ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    if  ( docParagraphBuilderAppendObject( rts->rtsParagraphBuilder,
				objectNumber, &(rrs->rrsTextAttribute) ) < 0 )
	{ LDEB(objectNumber); rval= -1; goto ready;	}

    objectNumber= -1;

  ready:

    if  ( objectNumber >= 0 )
	{ docDeleteObject( rr->rrDocument, objectNumber );	}

    rr->rrInsertedObject= ioSave;

    return rval;
    }
