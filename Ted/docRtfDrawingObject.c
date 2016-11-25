/************************************************************************/
/*									*/
/*  Read/Write MS-Word 95 style drawing objects from/to rtf.		*/
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

static int docRtfDrawingObjectAllocatePoints(	DrawingObject *	wdo,
						int		n )
    {
    APP_POINT *	fresh;

    fresh= (APP_POINT *)realloc( wdo->doPoints, n* sizeof(APP_POINT) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}

    wdo->doPoints= fresh;
    fresh += wdo->doPointCount;

    while( wdo->doPointCount < n )
	{
	fresh->x= fresh->y= 0;
	fresh++; wdo->doPointCount++;
	}

    return 0;
    }

int docRtfDrawingObjectProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    InsertedObject *	io= rrc->rrcInsertedObject;
    DrawingObject *	wdo;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! io )
	{
	LSXDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInsertedObject);
	return 0;
	}

    wdo= &(io->ioDrawingObject);

    switch( rcw->rcwID )
	{
	case DOpropANCHOR_LOCKED:
	    wdo->doAnchorLocked= arg != 0;
	    break;

	case DOpropX_ATTACH:
	    wdo->doXAttach= rcw->rcwEnumValue;
	    break;

	case DOpropY_ATTACH:
	    wdo->doYAttach= rcw->rcwEnumValue;
	    break;

	case DOpropKIND:
	    wdo->doKind= rcw->rcwEnumValue;
	    if  ( wdo->doKind == DOkindLINE	&&
		  wdo->doPointCount < 2		)
		{
		if  ( docRtfDrawingObjectAllocatePoints( wdo, 2 ) )
		    { LDEB(arg); return -1;	}
		}
	    break;

	case DOpropLINE_STYLE:
	    wdo->doLineStyle= rcw->rcwEnumValue;
	    break;

	case DOpropFILL_PATTERN:
	    wdo->doFillPattern= arg;
	    break;

	case DOpropARC_FLIP_X:
	    wdo->doArcFlipX= arg != 0;
	    break;
	case DOpropARC_FLIP_Y:
	    wdo->doArcFlipY= arg != 0;
	    break;

	case DOpropX:
	    wdo->doX= arg;
	    break;
	case DOpropY:
	    wdo->doY= arg;
	    break;
	case DOpropZ:
	    wdo->doZ= arg;
	    break;
	case DOpropWIDE:
	    wdo->doWide= arg;
	    io->ioTwipsWide= arg;
	    break;
	case DOpropHIGH:
	    wdo->doHigh= arg;
	    io->ioTwipsHigh= arg;
	    break;

	case DOpropLINE_WIDTH:
	    wdo->doLineWidth= arg;
	    break;

	case DOpropPOINT_COUNT:
	    if  ( docRtfDrawingObjectAllocatePoints( wdo, arg ) )
		{ LDEB(arg); return -1;	}
	    break;

	case DOpropLINE_RED:
	    wdo->doLineColor.rgb8Red= arg;
	    wdo->doLineColorGray= 0;
	    break;
	case DOpropLINE_GREEN:
	    wdo->doLineColor.rgb8Green= arg;
	    wdo->doLineColorGray= 0;
	    break;
	case DOpropLINE_BLUE:
	    wdo->doLineColor.rgb8Blue= arg;
	    wdo->doLineColorGray= 0;
	    break;
	case DOpropLINE_GRAY:
	    wdo->doLineColor.rgb8Red= arg;
	    wdo->doLineColor.rgb8Green= arg;
	    wdo->doLineColor.rgb8Blue= arg;
	    wdo->doLineColorGray= 1;
	    break;

	case DOpropFILL_FORE_RED:
	    wdo->doFillForeColor.rgb8Red= arg;
	    wdo->doFillForeColorGray= 0;
	    break;
	case DOpropFILL_FORE_GREEN:
	    wdo->doFillForeColor.rgb8Green= arg;
	    wdo->doFillForeColorGray= 0;
	    break;
	case DOpropFILL_FORE_BLUE:
	    wdo->doFillForeColor.rgb8Blue= arg;
	    wdo->doFillForeColorGray= 0;
	    break;
	case DOpropFILL_FORE_GRAY:
	    wdo->doFillForeColor.rgb8Red= arg;
	    wdo->doFillForeColor.rgb8Green= arg;
	    wdo->doFillForeColor.rgb8Blue= arg;
	    wdo->doFillForeColorGray= 1;
	    break;

	case DOpropFILL_BACK_RED:
	    wdo->doFillBackColor.rgb8Red= arg;
	    wdo->doFillBackColorGray= 0;
	    break;
	case DOpropFILL_BACK_GREEN:
	    wdo->doFillBackColor.rgb8Green= arg;
	    wdo->doFillBackColorGray= 0;
	    break;
	case DOpropFILL_BACK_BLUE:
	    wdo->doFillBackColor.rgb8Blue= arg;
	    wdo->doFillBackColorGray= 0;
	    break;
	case DOpropFILL_BACK_GRAY:
	    wdo->doFillBackColor.rgb8Red= arg;
	    wdo->doFillBackColor.rgb8Green= arg;
	    wdo->doFillBackColor.rgb8Blue= arg;
	    wdo->doFillBackColorGray= 1;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    /* SLDEB(rcw->rcwWord,rcw->rcwID); */
	    break;
	}

    return 0;
    }

int docRtfDrawingObjectCoordinate(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    InsertedObject *	io= rrc->rrcInsertedObject;
    DrawingObject *	wdo;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! io )
	{
	LSXDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInsertedObject);
	return 0;
	}

    wdo= &(io->ioDrawingObject);

    if  ( wdo->doNextPoint >= wdo->doPointCount )
	{
	LLLDEB(wdo->doKind,wdo->doNextPoint,wdo->doPointCount);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case DOpropX:
	    wdo->doPoints[wdo->doNextPoint  ].x= arg;
	    break;
	case DOpropY:
	    wdo->doPoints[wdo->doNextPoint++].y= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfReadDrawingObject(
			SimpleInputStream *	sis,
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
				(RtfAddTextParticule)0 );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInsertedObject->ioKind= DOCokDRAWING_OBJECT;

    rrc->rrcInsertedObject= ioSave;
    docRtfPopReadingState( rrc );

    return rval;
    }

