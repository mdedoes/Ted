/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	<docFrameProperties.h>
#   include	<docCellProperties.h>
#   include	<utilPropMask.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save row properties to RTF.						*/
/*									*/
/************************************************************************/

void docRtfSaveRowProperties(		RtfWriter *		rw,
					const PropertyMask *	rpSetMask,
					const RowProperties *	rpSet,
					int			col0,
					int			col1 )
    {
    const int			anyway= 0;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( PROPmaskISSET( rpSetMask, RPpropROW_NUMBER ) )
	{ docRtfWriteArgTag( rw, "irow", rpSet->rpRowNumber ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropROW_BAND_NUMBER ) )
	{ docRtfWriteArgTag( rw, "irowband", rpSet->rpRowBandNumber ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropROW_STYLE ) )
	{ docRtfWriteArgTag( rw, "ts", rpSet->rpRowStyle ); }

    if  ( PROPmaskISSET( rpSetMask, RPpropRTOL ) )
	{
	docRtfWriteAltTag( rw, "rtlrow", "ltrrow", rpSet->rpRToL );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropGAP_WIDTH ) )
	{ docRtfWriteArgTag( rw, "trgaph", rpSet->rpHalfGapWidthTwips ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropLEFT_INDENT ) )
	{ docRtfWriteArgTag( rw, "trleft", rpSet->rpLeftIndentTwips ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropHEIGHT ) )
	{ docRtfWriteArgTag( rw, "trrh", rpSet->rpHeightTwips ); }

    if  ( PROPmaskISSET( rpSetMask, RPpropIS_TABLE_HEADER ) )
	{ docRtfWriteFlagTag( rw, "trhdr", rpSet->rpIsTableHeader ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropKEEP_ON_ONE_PAGE ) )
	{ docRtfWriteFlagTag( rw, "trkeep", rpSet->rpKeepOnOnePage ); }
    if  ( PROPmaskISSET( rpSetMask, RPprop_KEEPFOLLOW ) )
	{ docRtfWriteFlagTag( rw, "trkeepfollow", rpSet->rp_Keepfollow ); }
    if  ( PROPmaskISSET( rpSetMask, RPpropAUTOFIT ) )
	{ docRtfWriteArgTag( rw, "trautofit", rpSet->rpAutofit ); }

    docRtfWriteNextLine( rw );

    if  ( PROPmaskISSET( rpSetMask, RPpropTOP_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrt",
				    rpSet->rpTopBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( rpSetMask, RPpropLEFT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrl",
				    rpSet->rpLeftBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( rpSetMask, RPpropRIGHT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrr",
				    rpSet->rpRightBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( rpSetMask, RPpropBOTTOM_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrb",
				    rpSet->rpBottomBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( rpSetMask, RPpropVERT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrv",
				    rpSet->rpVerticalBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( rpSetMask, RPpropHORIZ_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "trbrdrh",
				    rpSet->rpHorizontalBorderNumber, anyway );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, rpSet->rpShadingNumber,
		DOCrtf_RowShadingTags, DOCrtf_RowShadingTagCount,
		"trcfpat", "trcbpat", "trshdng" );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropFRAME ) )
	{
	const FrameProperties *	fp;

	fp= docGetFramePropertiesByNumber(
					rw->rwDocument, rpSet->rpFrameNumber );
	if  ( DOCisFRAME( fp ) )
	    { docRtfSaveRowFrameProperties( rw, fp );	}
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropTRFTS_WIDTH )	||
	  PROPmaskISSET( rpSetMask, RPpropTRW_WIDTH )	)
	{
	docRtfSaveAutoSpace( rw,
			"trftsWidth", rpSet->rpPreferredWidthUnit,
			"trwWidth", rpSet->rpPreferredWidth );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropTRSPDFL )	||
	  PROPmaskISSET( rpSetMask, RPpropTRSPDL )	)
	{
	docRtfSaveAutoSpace( rw,
			"trspdfl", rpSet->rpLeftCellSpacingUnit,
			"trspdl", rpSet->rpLeftCellSpacing );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRSPDFR )	||
	  PROPmaskISSET( rpSetMask, RPpropTRSPDR )	)
	{
	docRtfSaveAutoSpace( rw,
			"trspdfr", rpSet->rpRightCellSpacingUnit,
			"trspdr", rpSet->rpRightCellSpacing );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRSPDFT )	||
	  PROPmaskISSET( rpSetMask, RPpropTRSPDT )	)
	{
	docRtfSaveAutoSpace( rw,
			"trspdft", rpSet->rpTopCellSpacingUnit,
			"trspdt", rpSet->rpTopCellSpacing );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRSPDFB )	||
	  PROPmaskISSET( rpSetMask, RPpropTRSPDB )	)
	{
	docRtfSaveAutoSpace( rw,
			"trspdfb", rpSet->rpBottomCellSpacingUnit,
			"trspdb", rpSet->rpBottomCellSpacing );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropTRPADDFL )	||
	  PROPmaskISSET( rpSetMask, RPpropTRPADDL )	)
	{
	docRtfSaveAutoSpace( rw,
			"trpaddfl", rpSet->rpLeftCellPaddingUnit,
			"trpaddl", rpSet->rpLeftCellPadding );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRPADDFR )	||
	  PROPmaskISSET( rpSetMask, RPpropTRPADDR )	)
	{
	docRtfSaveAutoSpace( rw,
			"trpaddfr", rpSet->rpRightCellPaddingUnit,
			"trpaddr", rpSet->rpRightCellPadding );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRPADDFT )	||
	  PROPmaskISSET( rpSetMask, RPpropTRPADDT )	)
	{
	docRtfSaveAutoSpace( rw,
			"trpaddft", rpSet->rpTopCellPaddingUnit,
			"trpaddt", rpSet->rpTopCellPadding );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRPADDFB )	||
	  PROPmaskISSET( rpSetMask, RPpropTRPADDB )	)
	{
	docRtfSaveAutoSpace( rw,
			"trpaddfb", rpSet->rpBottomCellPaddingUnit,
			"trpaddb", rpSet->rpBottomCellPadding );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropTRFTS_WIDTHB )	||
	  PROPmaskISSET( rpSetMask, RPpropTRW_WIDTHB )		)
	{
	docRtfSaveAutoSpace( rw,
			"trftsWidthB", rpSet->rpCellWidthBeforeUnit,
			"trwWidthB", rpSet->rpCellWidthBefore );
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropTRFTS_WIDTHA )	||
	  PROPmaskISSET( rpSetMask, RPpropTRW_WIDTHA )		)
	{
	docRtfSaveAutoSpace( rw,
			"trftsWidthA", rpSet->rpCellWidthAfterUnit,
			"trwWidthA", rpSet->rpCellWidthAfter );
	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPprop_CELL_COUNT )	||
	  PROPmaskISSET( rpSetMask, RPprop_CELL_PROPS )	)
	{
	int			leftOffset= rpSet->rpLeftIndentTwips;
	PropertyMask		cpAllMask;
	const CellProperties *	cp= rpSet->rpCells;
	int			col;
	CellProperties		cpRef;

	docInitCellProperties( &cpRef );
	docRowPropertiesSetCellDefaults( &cpRef, rpSet );

	utilPropMaskFill( &cpAllMask, CLprop_COUNT );

	for ( col= 0; col < rpSet->rpCellCount; cp++, col++ )
	    {
	    if  ( ( col0 < 0 || col >= col0 )	&&
		  ( col1 < 0 || col <= col1 )	)
		{
		PropertyMask		cpSetMask;

		utilPropMaskClear( &cpSetMask );

		docCellPropertyDifference( &cpSetMask, &cpRef, &cpAllMask,
								    cp, dam0 );

		docRtfSaveCellProperties( rw, &cpSetMask, cp, leftOffset );
		leftOffset += cp->cpWide;
		}
	    }

	/*docCleanCellProperties( &cpRef );*/
	}

    return;
    }

