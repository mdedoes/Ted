/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Save row properties to RTF.						*/
/*									*/
/************************************************************************/

void docRtfSaveRowProperties(		const RowProperties *	rp,
					int *			pCol,
					int			col0,
					int			col1,
					SimpleOutputStream *	sos )
    {
    const CellProperties *	cp= rp->rpCells;

    int				col;
    int				shiftLeft= 0;

    docRtfWriteTag( "\\trowd", pCol, sos );

    if  ( rp->rpHalfGapWidthTwips != 0 )
	{ docRtfWriteArgTag( "\\trgaph", pCol, rp->rpHalfGapWidthTwips, sos ); }

    if  ( rp->rpLeftIndentTwips != 0 )
	{ docRtfWriteArgTag( "\\trleft", pCol, rp->rpLeftIndentTwips, sos ); }

    if  ( rp->rpHeightTwips != 0 )
	{ docRtfWriteArgTag( "\\trrh", pCol, rp->rpHeightTwips, sos ); }

    if  ( rp->rpIsTableHeader )
	{ docRtfWriteTag( "\\trhdr", pCol, sos ); }
    if  ( rp->rpKeepOnOnePage )
	{ docRtfWriteTag( "\\trkeep", pCol, sos ); }
    if  ( rp->rpAutofit )
	{ docRtfWriteArgTag( "\\trautofit", pCol, rp->rpAutofit, sos ); }

    docRtfWriteNextLine( pCol, sos );

    if  ( rp->rpTopBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrt", pCol, &(rp->rpTopBorder), sos ); }
    if  ( rp->rpLeftBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrl", pCol, &(rp->rpLeftBorder), sos ); }
    if  ( rp->rpRightBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrr", pCol, &(rp->rpRightBorder), sos ); }
    if  ( rp->rpBottomBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrb", pCol, &(rp->rpBottomBorder), sos ); }

    if  ( rp->rpVerticalBorder.bpStyle != DOCbsNONE )
	{
	docRtfSaveBorder( "\\trbrdrv", pCol, &(rp->rpVerticalBorder), sos );
	}
    if  ( rp->rpHorizontalBorder.bpStyle != DOCbsNONE )
	{
	docRtfSaveBorder( "\\trbrdrh", pCol, &(rp->rpHorizontalBorder), sos );
	}

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidth", rp->rpPreferredWidthUnit,
			"\\trwWidth", rp->rpPreferredWidth );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfl", rp->rpLeftDefaultCellSpacingUnit,
			"\\trspdl", rp->rpLeftDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfr", rp->rpRightDefaultCellSpacingUnit,
			"\\trspdr", rp->rpRightDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdft", rp->rpTopDefaultCellSpacingUnit,
			"\\trspdt", rp->rpTopDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfb", rp->rpBottomDefaultCellSpacingUnit,
			"\\trspdb", rp->rpBottomDefaultCellSpacing );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfl", rp->rpLeftDefaultCellMarginUnit,
			"\\trpaddl", rp->rpLeftDefaultCellMargin );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfr", rp->rpRightDefaultCellMarginUnit,
			"\\trpaddr", rp->rpRightDefaultCellMargin );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddft", rp->rpTopDefaultCellMarginUnit,
			"\\trpaddt", rp->rpTopDefaultCellMargin );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfb", rp->rpBottomDefaultCellMarginUnit,
			"\\trpaddb", rp->rpBottomDefaultCellMargin );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidthB", rp->rpCellWidthBeforeUnit,
			"\\trwWidthB", rp->rpCellWidthBefore );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidthA", rp->rpCellWidthAfterUnit,
			"\\trwWidthA", rp->rpCellWidthAfter );

    /**/
    if  ( col0 > 0 )
	{ shiftLeft= cp[col0-1].cpRightBoundaryTwips; }

    for ( col= 0; col < rp->rpCellCount; cp++, col++ )
	{
	if  ( ( col0 < 0 || col >= col0 )	&&
	      ( col1 < 0 || col <= col1 )	)
	    { docRtfSaveCellProperties( cp, pCol, shiftLeft, sos );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a row property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberRowProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case RPprop_NONE:
	    docCleanRowProperties( &(rrc->rrcRowProperties) );
	    docInitRowProperties(  &(rrc->rrcRowProperties) );

	    docCleanCellProperties( &(rrc->rrcCellProperties) );
	    docInitCellProperties(  &(rrc->rrcCellProperties) );
	    return 0;

	case RPpropGAP_WIDTH:
	    rrc->rrcRowProperties.rpHalfGapWidthTwips= arg;
	    break;

	case RPpropLEFT_INDENT:
	    rrc->rrcRowProperties.rpLeftIndentTwips= arg;
	    break;

	case RPpropHEIGHT:
	    rrc->rrcRowProperties.rpHeightTwips= arg;
	    break;

	case RPpropTOP_BORDER:
	    rrc->rrcRowProperties.rpTopBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropBOTTOM_BORDER:
	    rrc->rrcRowProperties.rpBottomBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropLEFT_BORDER:
	    rrc->rrcRowProperties.rpLeftBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropRIGHT_BORDER:
	    rrc->rrcRowProperties.rpRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropHORIZ_BORDER:
	    rrc->rrcRowProperties.rpHorizontalBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropVERT_BORDER:
	    rrc->rrcRowProperties.rpVerticalBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case RPpropALIGNMENT:
	    rrc->rrcRowProperties.rpAlignment= rcw->rcwEnumValue;
	    break;

	case RPpropIS_TABLE_HEADER:
	    rrc->rrcRowProperties.rpIsTableHeader= ( arg != 0 );
	    break;
	case RPpropKEEP_ON_ONE_PAGE:
	    rrc->rrcRowProperties.rpKeepOnOnePage= ( arg != 0 );
	    break;
	case RPpropAUTOFIT:
	    rrc->rrcRowProperties.rpAutofit= ( arg != 0 );
	    break;

	/**/
	case RPpropTRW_WIDTH:
	    rrc->rrcRowProperties.rpPreferredWidth= arg;
	    break;
	case RPpropTRFTS_WIDTH:
	    rrc->rrcRowProperties.rpPreferredWidthUnit= arg;
	    break;

	/**/
	case RPpropTRSPDL:
	    rrc->rrcRowProperties.rpLeftDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDR:
	    rrc->rrcRowProperties.rpRightDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDT:
	    rrc->rrcRowProperties.rpTopDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDB:
	    rrc->rrcRowProperties.rpBottomDefaultCellSpacing= arg;
	    break;

	case RPpropTRSPDFL:
	    rrc->rrcRowProperties.rpLeftDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFR:
	    rrc->rrcRowProperties.rpRightDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFT:
	    rrc->rrcRowProperties.rpTopDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFB:
	    rrc->rrcRowProperties.rpBottomDefaultCellSpacingUnit= arg;
	    break;

	/**/
	case RPpropTRPADDL:
	    rrc->rrcRowProperties.rpLeftDefaultCellMargin= arg;
	    break;
	case RPpropTRPADDR:
	    rrc->rrcRowProperties.rpRightDefaultCellMargin= arg;
	    break;
	case RPpropTRPADDT:
	    rrc->rrcRowProperties.rpTopDefaultCellMargin= arg;
	    break;
	case RPpropTRPADDB:
	    rrc->rrcRowProperties.rpBottomDefaultCellMargin= arg;
	    break;

	case RPpropTRPADDFL:
	    rrc->rrcRowProperties.rpLeftDefaultCellMarginUnit= arg;
	    break;
	case RPpropTRPADDFR:
	    rrc->rrcRowProperties.rpRightDefaultCellMarginUnit= arg;
	    break;
	case RPpropTRPADDFT:
	    rrc->rrcRowProperties.rpTopDefaultCellMarginUnit= arg;
	    break;
	case RPpropTRPADDFB:
	    rrc->rrcRowProperties.rpBottomDefaultCellMarginUnit= arg;
	    break;

	/**/
	case RPpropTRW_WIDTHB:
	    rrc->rrcRowProperties.rpCellWidthBefore= arg;
	    break;
	case RPpropTRW_WIDTHA:
	    rrc->rrcRowProperties.rpCellWidthAfter= arg;
	    break;

	case RPpropTRFTS_WIDTHB:
	    rrc->rrcRowProperties.rpCellWidthBeforeUnit= arg;
	    break;
	case RPpropTRFTS_WIDTHA:
	    rrc->rrcRowProperties.rpCellWidthAfterUnit= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return -1;
	}

    return 0;
    }

