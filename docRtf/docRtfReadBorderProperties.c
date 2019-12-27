/************************************************************************/
/*									*/
/*  Read border properties from RTF.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	<docAttributes.h>

#   include	<appDebugon.h>

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

