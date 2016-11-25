/************************************************************************/
/*									*/
/*  Read/Write border properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docBorderPropertyAdmin.h>
#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Save border definition.						*/
/*									*/
/************************************************************************/

static void docRtfSaveBorder(		RtfWriter *			rw,
					const char *			tag,
					const BorderProperties *	bp )
    {
    docRtfWriteTag( rw, tag );

    docRtfWriteEnumTag( rw, DOCrtf_BorderStyleTags, bp->bpStyle,
				    DOCrtf_BorderStyleTagCount, DOCbs_COUNT );

    if  ( bp->bpArt != 0 )
	{ docRtfWriteArgTag( rw, "brdrart", bp->bpColor );	}

    if  ( bp->bpColor != 0 )
	{ docRtfWriteArgTag( rw, "brdrcf", bp->bpColor );	}

    if  ( bp->bpPenWideTwips != 0 )
	{ docRtfWriteArgTag( rw, "brdrw", bp->bpPenWideTwips ); }

    if  ( bp->bpSpacingTwips != 0 )
	{ docRtfWriteArgTag( rw, "brsp", bp->bpSpacingTwips );	}

    docRtfWriteNextLine( rw );

    return;
    }

void docRtfSaveBorderByNumber(		RtfWriter *			rwc,
					const char *			tag,
					int				num,
					int				anyway )
    {
    const BufferDocument *		bd= rwc->rwDocument;
    const NumberedPropertiesList *	bpl= &(bd->bdBorderPropertyList);

    BorderProperties			bp;

    docGetBorderPropertiesByNumber( &bp, bpl, num );

    if  ( ! anyway && bp.bpStyle == DOCbsNONE )
	{ return;	}

    docRtfSaveBorder( rwc, tag, &bp );
    }

int docRtfBeginBorder(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    docInitBorderProperties( &(rrc->rrcBorderProperties) );
    rrc->rrcBorderProperties.bpStyle= DOCbsS;
    return 0;
    }

int docRtfReadGetBorderNumber(	RtfReader *	rrc )
    {
    int rval= docBorderPropertiesNumber(
				    &(rrc->rrcDocument->bdBorderPropertyList),
				    &(rrc->rrcBorderProperties) );

    docInitBorderProperties( &(rrc->rrcBorderProperties) );

    return rval;
    }

int docRtfBrdrProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    if  ( docSetBorderProperty( &(rrc->rrcBorderProperties),
						    rcw->rcwID, arg ) < 0 )
	{ SLLDEB(rcw->rcwWord,rcw->rcwID,arg); return -1; }

    return 0;
    }

