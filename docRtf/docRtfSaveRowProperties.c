/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
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

static const int RowRegularProperties[]=
{
    RPpropROW_NUMBER,
    RPpropROW_BAND_NUMBER,
    RPpropROW_STYLE,
    RPpropRTOL,
    RPpropGAP_WIDTH,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPprop_KEEPFOLLOW,
    RPpropAUTOFIT,
    RPpropTOP_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropVERT_BORDER,
    RPpropHORIZ_BORDER,
    RPpropTRFLAT,
};

static const int RowRegularPropertyCount=
			sizeof(RowRegularProperties)/sizeof(int);

int docRtfSaveRowProperties(		RtfWriter *		rw,
					const PropertyMask *	rpSetMask,
					const RowProperties *	rpSet,
					int			col0,
					int			col1 )
    {
    const int			scope= RTCscopeROW;

    if  ( docRtfWriteItemProperties( rw, scope, rpSet,
			(RtfGetIntProperty)docGetRowProperty, rpSetMask,
			RowRegularProperties, RowRegularPropertyCount ) )
	{ LLDEB(scope,RowRegularPropertyCount); return -1;	}

    /**/
    if  ( PROPmaskISSET( rpSetMask, RPpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, RTCscopeROW_SHADING,
						rpSet->rpShadingNumber );
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
	const struct DocumentAttributeMap * const dam0=
				    (const struct DocumentAttributeMap *)0;

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

		if  ( docRtfSaveCellProperties( rw, &cpSetMask,
							cp, leftOffset ) )
		    { LDEB(col); return -1;	}

		leftOffset += cp->cpWide;
		}
	    }

	/*docCleanCellProperties( &cpRef );*/
	}

    return 0;
    }

