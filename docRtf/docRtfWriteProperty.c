/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	"docRtfFindProperty.h"
#   include	<utilPropMask.h>
#   include	<docAttributes.h>
#   include	<docBorderProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save border definition.						*/
/*									*/
/************************************************************************/

static int docRtfSaveBorder(		RtfWriter *			rw,
					const char *			tag,
					const BorderProperties *	bp )
    {
    const int	scope= RTCscopeBORDER;

    docRtfWriteTag( rw, tag );

    if  ( docRtfWriteProperty( rw, scope,
				BRDRpropSTYLE, bp->bpStyle ) )
	{ LLLDEB(scope,BRDRpropSTYLE,bp->bpStyle); return -1;	}

    if  ( bp->bpArt != 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
				BRDRpropART, bp->bpArt ) )
	    { LLLDEB(scope,BRDRpropART,bp->bpArt); return -1;	}
	}

    if  ( bp->bpColor != 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
				BRDRpropCOLOR, bp->bpColor ) )
	    { LLLDEB(scope,BRDRpropCOLOR,bp->bpColor); return -1;	}
	}

    if  ( bp->bpPenWideTwips != 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
				BRDRpropPEN_WIDE, bp->bpPenWideTwips ) )
	    { LLLDEB(scope,BRDRpropPEN_WIDE,bp->bpPenWideTwips); return -1; }
	}

    if  ( bp->bpSpacingTwips != 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
				BRDRpropSPACING, bp->bpSpacingTwips ) )
	    { LLLDEB(scope,BRDRpropSPACING,bp->bpSpacingTwips); return -1; }
	}

    docRtfWriteNextLine( rw );

    return 0;
    }

static int docRtfSaveBorderByNumber(	RtfWriter *			rwc,
					const char *			tag,
					int				num,
					int				anyway )
    {
    const struct BufferDocument *	bd= rwc->rwDocument;

    const BorderProperties *		bp;

    bp= docGetBorderPropertiesByNumber( bd, num );

    if  ( ! anyway && bp->bpStyle == DOCbsNONE )
	{ return 0;	}

    if  ( docRtfSaveBorder( rwc, tag, bp ) )
	{ LDEB(num); return -1;	}

    return 0;
    }

int docRtfWriteAllItemProperties( RtfWriter *			rw,
				int				scope,
				const void *			item,
				RtfGetIntProperty		getIntProp,
				const PropertyMask *		setMask,
				int				count )
    {
    int		prop;

    for ( prop= 0; prop < count; prop++ )
	{
	if  ( PROPmaskISSET( setMask, prop ) )
	    {
	    const int	value= (*getIntProp)( item, prop );

	    if  ( docRtfWriteProperty( rw, scope, prop, value ) )
		{ LLLDEB(scope,prop,value); return -1;	}
	    }
	}

    return 0;
    }

int docRtfWriteItemProperties(	RtfWriter *			rw,
				int				scope,
				const void *			item,
				RtfGetIntProperty		getIntProp,
				const PropertyMask *		setMask,
				const int *			properties,
				int				propertyCount )
    {
    int				propidx;

    for ( propidx= 0; propidx < propertyCount; propidx++ )
	{
	const int	prop= properties[propidx];

	if  ( PROPmaskISSET( setMask, prop ) )
	    {
	    const int	value= (*getIntProp)( item, prop );

	    if  ( docRtfWriteProperty( rw, scope, prop, value ) )
		{ LLDEB(prop,value); return -1;	}
	    }
	}

    return 0;
    }

int docRtfWriteProperty(		RtfWriter *		rw,
					int			scope,
					int			prop,
					int			value )
    {
    const RtfControlWord *	rcw;

    rcw= docRtfFindProperty( scope, prop, value );
    if  ( ! rcw )
	{ LLLXDEB(scope,prop,value,rcw); return -1;	}

    switch( rcw->rcwType )
	{
	case RTCtypeFLAG:
	    if  ( docRtfWriteFlagTag( rw, rcw->rcwWord, value ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTCtypeNUMBER:
	    if  ( docRtfWriteArgTag( rw, rcw->rcwWord, value ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTCtypeBORDER_POSITIVE:
	    {
	    const int	anyway= 0;

	    return docRtfSaveBorderByNumber( rw, rcw->rcwWord, value, anyway );
	    }

	case RTCtypeBORDER_ANYWAY:
	    {
	    const int	anyway= 1;

	    return docRtfSaveBorderByNumber( rw, rcw->rcwWord, value, anyway );
	    }

	case RTCtypeENUM:
	    if  ( docRtfWriteTag( rw, rcw->rcwWord ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTCtypeSYMBOL:
	case RTCtypeDEST:
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwType);
	    return -1;
	}

    return 0;
    }

int docRtfWriteFlagTag(		RtfWriter *		rw,
				const char *		tag,
				int			arg )
    {
    if  ( arg )
	{ return docRtfWriteTag( rw, tag );		}
    else{ return docRtfWriteArgTag( rw, tag, arg );	}
    }

int docRtfWriteAltTag(		RtfWriter *		rw,
				const char *		yesTag,
				const char *		noTag,
				int			arg )
    {
    if  ( arg )
	{ return docRtfWriteTag( rw, yesTag );	}
    else{ return docRtfWriteTag( rw, noTag  );	}
    }

int docRtfWriteEnumTag(		RtfWriter *		rw,
				const char * const *	tags,
				int			arg,
				int			tagCount,
				int			enumCount )
    {
    if  ( tagCount != enumCount )
	{ LLSDEB(tagCount,enumCount,tags[0]); return -1;	}

    if  ( arg < 0 || arg >= tagCount )
	{ LLSDEB(tagCount,arg,tags[0]); return -1;	}

    if  ( tags[arg] )
	{ return docRtfWriteTag( rw, tags[arg] );	}
    else{ return 0;					}
    }

