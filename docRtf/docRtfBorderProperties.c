/************************************************************************/
/*									*/
/*  Read/Write border properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"
#   include	<docAttributes.h>

#   include	<appDebugon.h>

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
    const struct BufferDocument *	bd= rwc->rwDocument;

    const BorderProperties *		bp;

    bp= docGetBorderPropertiesByNumber( bd, num );

    if  ( ! anyway && bp->bpStyle == DOCbsNONE )
	{ return;	}

    docRtfSaveBorder( rwc, tag, bp );
    }

int docRtfBeginBorder(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    docInitBorderProperties( &(rr->rrBorderProperties) );
    rr->rrBorderProperties.bpStyle= DOCbsS;
    return 0;
    }

int docRtfReadGetBorderNumber(	RtfReader *	rr )
    {
    int rval= docBorderPropertiesNumber( rr->rrDocument,
						&(rr->rrBorderProperties) );

    docInitBorderProperties( &(rr->rrBorderProperties) );

    return rval;
    }

int docRtfBrdrProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    if  ( docSetBorderProperty( &(rr->rrBorderProperties),
						    rcw->rcwID, arg ) < 0 )
	{ SLLDEB(rcw->rcwWord,rcw->rcwID,arg); return -1; }

    return 0;
    }

