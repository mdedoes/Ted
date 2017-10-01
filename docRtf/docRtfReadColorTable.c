/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	<docBuf.h>
#   include	<utilPalette.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a color table.							*/
/*  Note that like the font table, the color table is flat. It does	*/
/*  not have groups for the colors.					*/
/*									*/
/************************************************************************/

static int docRtfSaveColor(	RtfReader *		rrc,
				const char *		text,
				int			len )
    {
    DocumentProperties *	dp= rrc->rrDocument->bdProperties;
    int				n= dp->dpColorPalette->cpColorCount;

    if  ( utilPaletteSetCount( dp->dpColorPalette, n+ 1 ) )
	{ LDEB(n); return -1;	}

    dp->dpColorPalette->cpColors[n]= rrc->rrColor;

    if  ( ! rrc->rrGotColorComponent )
	{
	if  ( dp->dpDefaultColor < 0 )
	    { dp->dpDefaultColor= n;	}
	}

    rrc->rrGotColorComponent= 0;
    return 0;
    }

int docRtfColorIgnored(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    { return 0;	}

int docRtfColorComp(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case RGBAcompRED:
	    rrc->rrGotColorComponent= 1;
	    rrc->rrColor.rgb8Red= arg;
	    break;
	case RGBAcompGREEN:
	    rrc->rrGotColorComponent= 1;
	    rrc->rrColor.rgb8Green= arg;
	    break;
	case RGBAcompBLUE:
	    rrc->rrGotColorComponent= 1;
	    rrc->rrColor.rgb8Blue= arg;
	    break;
	default:
	    /* SLDEB(rcw->rcwWord,arg); */
	    break;
	}

    return 0;
    }

int docRtfColorTable(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    rrc->rrGotColorComponent= 0;

    rrc->rrColor.rgb8Red= 255;
    rrc->rrColor.rgb8Green= 255;
    rrc->rrColor.rgb8Blue= 255;
    rrc->rrColor.rgb8Alpha= 255;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfSaveColor, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

