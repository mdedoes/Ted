/************************************************************************/
/*									*/
/*  Read/Write cell to/from RTF.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a cell property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberCellShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    if  ( docSetShadingProperty( &(rr->rrCellShading), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrCellPropertyMask), CLpropSHADING );
    PROPmaskADD( &(rr->rrStyle.dsCellMask), CLpropSHADING );

    return 0;
    }

void docRtfResetCellProperties(	RtfReader *	rr )
    {
    /*docCleanCellProperties( &(rr->rrCellProperties) );*/
    docInitCellProperties( &(rr->rrCellProperties) );
    docInitItemShading( &(rr->rrCellShading) );
    utilPropMaskClear( &(rr->rrCellPropertyMask) );

    docRowPropertiesSetCellDefaults( &(rr->rrCellProperties),
					&(rr->rrTreeStack->rtsRowProperties) );

    return;
    }

int docRtfGotCellX(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    const int		col= -1;

    if  ( arg < 0 )
	{
	DocumentGeometry * dg= &(rr->rrSectionProperties.spDocumentGeometry);

	LDEB(arg);

	arg= geoContentWide( dg );
	}


    rr->rrCellProperties.cpWide= arg- rr->rrTreeStack->rtsRowCellX;
    rr->rrStyle.dsCellProps.cpWide= arg- rr->rrTreeStack->rtsRowCellX;

    rr->rrCellProperties.cpShadingNumber= docItemShadingNumber(
				    rr->rrDocument, &(rr->rrCellShading) );
    rr->rrStyle.dsCellProps.cpShadingNumber=
				    rr->rrCellProperties.cpShadingNumber;

    if  ( rr->rrCellProperties.cpShadingNumber < 0 )
	{ LDEB(rr->rrCellProperties.cpShadingNumber);	}

    if  ( ! utilPropMaskIsEmpty( &(rr->rrCellPropertyMask) ) )
	{
	PROPmaskADD( &(rr->rrRowPropertyMask), RPprop_CELL_PROPS );
	}
    if  ( ! utilPropMaskIsEmpty( &(rr->rrStyle.dsCellMask) ) )
	{
	PROPmaskADD( &(rr->rrStyle.dsRowMask), RPprop_CELL_PROPS );
	}

    if  ( arg > 0 )
	{
	if  ( docInsertRowColumn( &(rr->rrTreeStack->rtsRowProperties), col,
		&(rr->rrCellProperties), (const DocumentAttributeMap *)0 ) )
	    { LDEB(rr->rrTreeStack->rtsRowProperties.rpCellCount); return -1; }

	rr->rrTreeStack->rtsRowCellX= arg;

	PROPmaskADD( &(rr->rrCellPropertyMask),  CLpropWIDTH );
	PROPmaskADD( &(rr->rrStyle.dsCellMask), CLpropWIDTH );

	PROPmaskADD( &(rr->rrRowPropertyMask),  RPprop_CELL_COUNT );
	PROPmaskADD( &(rr->rrStyle.dsRowMask), RPprop_CELL_COUNT );

	PROPmaskADD( &(rr->rrRowPropertyMask),  RPprop_CELL_PROPS );
	PROPmaskADD( &(rr->rrStyle.dsRowMask), RPprop_CELL_PROPS );
	}

    docRtfResetCellProperties( rr );

    return 0;
    }

int docRtfRememberCellProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case CLprop_NONE:
	    docRtfResetCellProperties( rr );
	    return 0;

	/**/
	case CLpropTOP_BORDER:
	case CLpropBOTTOM_BORDER:
	case CLpropLEFT_BORDER:
	case CLpropRIGHT_BORDER:
	case CLpropLEFT_RIGHT_DIAGONAL:
	case CLpropRIGHT_LEFT_DIAGONAL:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;

	case CLpropHOR_MERGE:
	case CLpropTEXT_FLOW:
	case CLpropVALIGN:
	case CLpropVERT_MERGE:
	    arg= rcw->rcwEnumValue;
	    break;

	case CLprop_NOT_SUPPORTED:
	    return 0;
	}

    PROPmaskADD( &(rr->rrCellPropertyMask), rcw->rcwID );
    if  ( docSetCellProperty( &(rr->rrCellProperties), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrStyle.dsCellMask), rcw->rcwID );
    if  ( docSetCellProperty( &(rr->rrStyle.dsCellProps),
						rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cell instance properties: How table styles are applied.		*/
/*									*/
/************************************************************************/

int docRtfRememberCellInstanceProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rr )
    {
    return 0;
    }
