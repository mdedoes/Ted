/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfFindProperty.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a row property when reading RTF.				*/
/*									*/
/************************************************************************/

static void docRtfResetRowProperties(	RtfReader *	rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    docCleanRowProperties( &(rts->rtsRowProperties) );
    docInitRowProperties(  &(rts->rtsRowProperties) );
    rts->rtsRowCellX= rts->rtsRowProperties.rpLeftIndentTwips;
    utilEmptyMemoryBuffer( &(rts->rtsRowSummary) );

    utilPropMaskClear( &(rr->rrRowPropertyMask) );
    docInitItemShading(  &(rr->rrRowShading) );

    docRtfResetCellProperties( rr );
    }

int docRtfRememberRowShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    if  ( docSetShadingProperty( &(rr->rrRowShading), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrRowPropertyMask), RPpropSHADING );
    PROPmaskADD( &(rr->rrStyle.dsRowMask), RPpropSHADING );

    return 0;
    }

int docRtfRememberRowProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RowProperties *	rp= &(rr->rrTreeStack->rtsRowProperties);

    switch( rcw->rcwID )
	{
	case RPprop_NONE:
	    docRtfResetRowProperties( rr );
	    return 0;

	case RPpropTOP_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropBOTTOM_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropLEFT_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropRIGHT_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropHORIZ_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropVERT_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;

	case RPpropLEFT_INDENT:
	    rr->rrTreeStack->rtsRowCellX= arg;
	    break;

	case RPprop_IGNORED:
	    return 0;
	}

    if  ( docSetRowProperty( rp, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrRowPropertyMask), rcw->rcwID );
    PROPmaskADD( &(rr->rrStyle.dsRowMask), rcw->rcwID );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cope with Word10 {\trowd ...... } peculiarity.			*/
/*									*/
/************************************************************************/

int docRtfReadRowProperties(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			rval= 0;

    RowProperties	savedRp;
    MemoryBuffer	savedSummary;
    int			savedCx;
    CellProperties	savedCp;
    ItemShading		savedRowShading;
    ItemShading		savedCellShading;

    savedRp= rr->rrTreeStack->rtsRowProperties;
    savedSummary= rr->rrTreeStack->rtsRowSummary;
    savedCx= rr->rrTreeStack->rtsRowCellX;
    savedCp= rr->rrCellProperties;
    savedRowShading= rr->rrRowShading;
    savedCellShading= rr->rrCellShading;

    docInitRowProperties( &(rr->rrTreeStack->rtsRowProperties) );
    utilInitMemoryBuffer( &(rr->rrTreeStack->rtsRowSummary) );
    docInitCellProperties( &(rr->rrCellProperties) );
    rr->rrTreeStack->rtsRowCellX=
			rr->rrTreeStack->rtsRowProperties.rpLeftIndentTwips;

    docInitItemShading(  &(rr->rrRowShading) );
    docInitItemShading(  &(rr->rrCellShading) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
	    docRtfDocumentGroups, docRtfGotText, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &(rr->rrTreeStack->rtsRowProperties) );
    utilCleanMemoryBuffer( &(rr->rrTreeStack->rtsRowSummary) );
    /*docCleanCellProperties( &(rr->rrCellProperties) );*/

    rr->rrTreeStack->rtsRowProperties= savedRp;
    rr->rrTreeStack->rtsRowSummary= savedSummary;
    rr->rrTreeStack->rtsRowCellX= savedCx;
    rr->rrCellProperties= savedCp;
    rr->rrRowShading= savedRowShading;
    rr->rrCellShading= savedCellShading;

    return rval;
    }

int docRtfCommitRowText(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;
    const int		removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rts->rtsRowSummary),
						    rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
