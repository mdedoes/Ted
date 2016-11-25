/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docParaProperties.h"
#   include	"docPropVal.h"

/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

void docCleanParagraphProperties(	ParagraphProperties *	pp )
    {
    }

void docInitParagraphProperties(	ParagraphProperties *	pp )
    {
    pp->ppTabStopListNumber= 0;

    pp->ppFirstIndentTwips= 0;
    pp->ppLeftIndentTwips= 0;
    pp->ppRightIndentTwips= 0;

    pp->ppSpaceBeforeTwips= 0;
    pp->ppSpaceAfterTwips= 0;
    pp->ppLineSpacingTwips= 0;

    pp->ppStyle= 0;
    pp->ppListOverride= 0;

    pp->ppTopBorderNumber= 0;
    pp->ppLeftBorderNumber= 0;
    pp->ppRightBorderNumber= 0;
    pp->ppBottomBorderNumber= 0;
    pp->ppBetweenBorderNumber= 0;
    pp->ppBarNumber= 0;

    pp->ppShadingNumber= 0;
    pp->ppFrameNumber= 0;

    pp->ppOutlineLevel= PPoutlineBODYTEXT;
    pp->ppListLevel= 0;

    pp->ppAlignment= DOCthaLEFT;

    pp->ppBreakKind= DOCibkNONE;
    pp->ppTableNesting= 0;
    pp->ppLineSpacingIsMultiple= 0;

    pp->ppKeepOnPage= 0;
    pp->ppKeepWithNext= 0;
    pp->ppWidowControl= 0;
    pp->ppHyphenateParagraph= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Change paragraph properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docCopyParagraphProperties(	ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom )
    {
    PropertyMask	ppSetMask;

    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    return docUpdParaProperties( (PropertyMask *)0, ppTo, &ppSetMask, ppFrom,
					    (const DocumentAttributeMap *)0 );
    }

int docUpdParaProperties(	PropertyMask *			pDoneMask,
				ParagraphProperties *		ppTo,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		ppDoneMask;

    utilPropMaskClear( &ppDoneMask );

    if  ( PROPmaskISSET( ppSetMask, PPpropSTYLE ) )
	{
	if  ( ppTo->ppStyle != ppFrom->ppStyle )
	    {
	    ppTo->ppStyle= ppFrom->ppStyle;
	    PROPmaskADD( &ppDoneMask, PPpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLISTOVERRIDE ) )
	{
	int		listStyle= ppFrom->ppListOverride;

	if  ( dam && dam->damListStyleMap )
	    { listStyle= dam->damListStyleMap[listStyle];	}

	if  ( ppTo->ppListOverride != listStyle )
	    {
	    ppTo->ppListOverride= listStyle;
	    PROPmaskADD( &ppDoneMask, PPpropLISTOVERRIDE );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropTABLE_NESTING ) )
	{
	if  ( ppTo->ppTableNesting != ppFrom->ppTableNesting )
	    {
	    ppTo->ppTableNesting= ppFrom->ppTableNesting;
	    PROPmaskADD( &ppDoneMask, PPpropTABLE_NESTING );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropFIRST_INDENT ) )
	{
	if  ( ppTo->ppFirstIndentTwips != ppFrom->ppFirstIndentTwips )
	    {
	    ppTo->ppFirstIndentTwips= ppFrom->ppFirstIndentTwips;
	    PROPmaskADD( &ppDoneMask, PPpropFIRST_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLEFT_INDENT ) )
	{
	if  ( ppTo->ppLeftIndentTwips != ppFrom->ppLeftIndentTwips )
	    {
	    ppTo->ppLeftIndentTwips= ppFrom->ppLeftIndentTwips;
	    PROPmaskADD( &ppDoneMask, PPpropLEFT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropRIGHT_INDENT ) )
	{
	if  ( ppTo->ppRightIndentTwips != ppFrom->ppRightIndentTwips )
	    {
	    ppTo->ppRightIndentTwips= ppFrom->ppRightIndentTwips;
	    PROPmaskADD( &ppDoneMask, PPpropRIGHT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropALIGNMENT ) )
	{
	if  ( ppTo->ppAlignment != ppFrom->ppAlignment )
	    {
	    ppTo->ppAlignment= ppFrom->ppAlignment;
	    PROPmaskADD( &ppDoneMask, PPpropALIGNMENT );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropTAB_STOPS ) )
	{
	int	fromNumber= ppFrom->ppTabStopListNumber;

	if  ( fromNumber >= 0 && dam && dam->damRulerMap )
	    { fromNumber= dam->damRulerMap[fromNumber];	}

	if  ( ppTo->ppTabStopListNumber != fromNumber )
	    {
	    ppTo->ppTabStopListNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropTAB_STOPS );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropBREAK_KIND ) )
	{
	if  ( ppTo->ppBreakKind != ppFrom->ppBreakKind )
	    {
	    ppTo->ppBreakKind= ppFrom->ppBreakKind;
	    PROPmaskADD( &ppDoneMask, PPpropBREAK_KIND );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropWIDCTLPAR ) )
	{
	if  ( ppTo->ppWidowControl != ppFrom->ppWidowControl )
	    {
	    ppTo->ppWidowControl= ppFrom->ppWidowControl;
	    PROPmaskADD( &ppDoneMask, PPpropWIDCTLPAR );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropKEEPN ) )
	{
	if  ( ppTo->ppKeepWithNext != ppFrom->ppKeepWithNext )
	    {
	    ppTo->ppKeepWithNext= ppFrom->ppKeepWithNext;
	    PROPmaskADD( &ppDoneMask, PPpropKEEPN );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropKEEP ) )
	{
	if  ( ppTo->ppKeepOnPage != ppFrom->ppKeepOnPage )
	    {
	    ppTo->ppKeepOnPage= ppFrom->ppKeepOnPage;
	    PROPmaskADD( &ppDoneMask, PPpropKEEP );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropSPACE_BEFORE ) )
	{
	if  ( ppTo->ppSpaceBeforeTwips != ppFrom->ppSpaceBeforeTwips )
	    {
	    ppTo->ppSpaceBeforeTwips= ppFrom->ppSpaceBeforeTwips;
	    PROPmaskADD( &ppDoneMask, PPpropSPACE_BEFORE );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropSPACE_AFTER ) )
	{
	if  ( ppTo->ppSpaceAfterTwips != ppFrom->ppSpaceAfterTwips )
	    {
	    ppTo->ppSpaceAfterTwips= ppFrom->ppSpaceAfterTwips;
	    PROPmaskADD( &ppDoneMask, PPpropSPACE_AFTER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLINE_SPACING_DIST ) )
	{
	if  ( ppTo->ppLineSpacingTwips != ppFrom->ppLineSpacingTwips )
	    {
	    ppTo->ppLineSpacingTwips= ppFrom->ppLineSpacingTwips;
	    PROPmaskADD( &ppDoneMask, PPpropLINE_SPACING_DIST );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLINE_SPACING_MULT ) )
	{
	if  ( ppTo->ppLineSpacingIsMultiple != ppFrom->ppLineSpacingIsMultiple )
	    {
	    ppTo->ppLineSpacingIsMultiple= ppFrom->ppLineSpacingIsMultiple;
	    PROPmaskADD( &ppDoneMask, PPpropLINE_SPACING_MULT );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropTOP_BORDER ) )
	{
	int	fromNumber= ppFrom->ppTopBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppTopBorderNumber != fromNumber )
	    {
	    ppTo->ppTopBorderNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropBOTTOM_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBottomBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBottomBorderNumber != fromNumber )
	    {
	    ppTo->ppBottomBorderNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLEFT_BORDER ) )
	{
	int	fromNumber= ppFrom->ppLeftBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppLeftBorderNumber != fromNumber )
	    {
	    ppTo->ppLeftBorderNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropRIGHT_BORDER ) )
	{
	int	fromNumber= ppFrom->ppRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppRightBorderNumber != fromNumber )
	    {
	    ppTo->ppRightBorderNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropBETWEEN_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBetweenBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBetweenBorderNumber != fromNumber )
	    {
	    ppTo->ppBetweenBorderNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropBETWEEN_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropBAR_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBarNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBarNumber != fromNumber )
	    {
	    ppTo->ppBarNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropBAR_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropOUTLINELEVEL ) )
	{
	if  ( ppTo->ppOutlineLevel != ppFrom->ppOutlineLevel )
	    {
	    ppTo->ppOutlineLevel= ppFrom->ppOutlineLevel;
	    PROPmaskADD( &ppDoneMask, PPpropOUTLINELEVEL );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLISTLEVEL ) )
	{
	if  ( ppTo->ppListLevel != ppFrom->ppListLevel )
	    {
	    ppTo->ppListLevel= ppFrom->ppListLevel;
	    PROPmaskADD( &ppDoneMask, PPpropLISTLEVEL );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropSHADING ) )
	{
	int	fromNumber= ppFrom->ppShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( ppTo->ppShadingNumber != fromNumber )
	    {
	    ppTo->ppShadingNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropSHADING );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropFRAME ) )
	{
	int	fromNumber= ppFrom->ppFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( ppTo->ppFrameNumber != fromNumber )
	    {
	    ppTo->ppFrameNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropFRAME );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropHYPHPAR ) )
	{
	if  ( ppTo->ppHyphenateParagraph != ppFrom->ppHyphenateParagraph )
	    {
	    ppTo->ppHyphenateParagraph= ppFrom->ppHyphenateParagraph;
	    PROPmaskADD( &ppDoneMask, PPpropHYPHPAR );
	    }
	}

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &ppDoneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare paragraph properties.					*/
/*									*/
/************************************************************************/

void docParaPropertyDifference(	PropertyMask *			pDifMask,
				const ParagraphProperties *	ppTo,
				const PropertyMask *		ppCmpMask,
				const ParagraphProperties *	ppFrom )
    {
    PropertyMask		ppDifMask;

    const DocumentAttributeMap *	dam= (const DocumentAttributeMap *)0;

    utilPropMaskClear( &ppDifMask );

    if  ( PROPmaskISSET( ppCmpMask, PPpropSTYLE ) )
	{
	if  ( ppTo->ppStyle != ppFrom->ppStyle )
	    { PROPmaskADD( &ppDifMask, PPpropSTYLE ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLISTOVERRIDE ) )
	{
	if  ( ppTo->ppListOverride != ppFrom->ppListOverride )
	    { PROPmaskADD( &ppDifMask, PPpropLISTOVERRIDE ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropTABLE_NESTING ) )
	{
	if  ( ppTo->ppTableNesting != ppFrom->ppTableNesting )
	    { PROPmaskADD( &ppDifMask, PPpropTABLE_NESTING );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropFIRST_INDENT ) )
	{
	if  ( ppTo->ppFirstIndentTwips != ppFrom->ppFirstIndentTwips )
	    { PROPmaskADD( &ppDifMask, PPpropFIRST_INDENT ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLEFT_INDENT ) )
	{
	if  ( ppTo->ppLeftIndentTwips != ppFrom->ppLeftIndentTwips )
	    { PROPmaskADD( &ppDifMask, PPpropLEFT_INDENT ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropRIGHT_INDENT ) )
	{
	if  ( ppTo->ppRightIndentTwips != ppFrom->ppRightIndentTwips )
	    { PROPmaskADD( &ppDifMask, PPpropRIGHT_INDENT ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropALIGNMENT ) )
	{
	if  ( ppTo->ppAlignment != ppFrom->ppAlignment )
	    { PROPmaskADD( &ppDifMask, PPpropALIGNMENT ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropTAB_STOPS ) )
	{
	if  ( ppTo->ppTabStopListNumber != ppFrom->ppTabStopListNumber )
	    { PROPmaskADD( &ppDifMask, PPpropTAB_STOPS );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropBREAK_KIND ) )
	{
	if  ( ppTo->ppBreakKind != ppFrom->ppBreakKind )
	    { PROPmaskADD( &ppDifMask, PPpropBREAK_KIND ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropWIDCTLPAR ) )
	{
	if  ( ppTo->ppWidowControl != ppFrom->ppWidowControl )
	    { PROPmaskADD( &ppDifMask, PPpropWIDCTLPAR ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropKEEPN ) )
	{
	if  ( ppTo->ppKeepWithNext != ppFrom->ppKeepWithNext )
	    { PROPmaskADD( &ppDifMask, PPpropKEEPN ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropKEEP ) )
	{
	if  ( ppTo->ppKeepOnPage != ppFrom->ppKeepOnPage )
	    { PROPmaskADD( &ppDifMask, PPpropKEEP ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropSPACE_BEFORE ) )
	{
	if  ( ppTo->ppSpaceBeforeTwips != ppFrom->ppSpaceBeforeTwips )
	    { PROPmaskADD( &ppDifMask, PPpropSPACE_BEFORE ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropSPACE_AFTER ) )
	{
	if  ( ppTo->ppSpaceAfterTwips != ppFrom->ppSpaceAfterTwips )
	    { PROPmaskADD( &ppDifMask, PPpropSPACE_AFTER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLINE_SPACING_DIST ) )
	{
	if  ( ppTo->ppLineSpacingTwips != ppFrom->ppLineSpacingTwips )
	    { PROPmaskADD( &ppDifMask, PPpropLINE_SPACING_DIST ); }
	}
    if  ( PROPmaskISSET( ppCmpMask, PPpropLINE_SPACING_MULT ) )
	{
	if  ( ppTo->ppLineSpacingIsMultiple != ppFrom->ppLineSpacingIsMultiple )
	    { PROPmaskADD( &ppDifMask, PPpropLINE_SPACING_MULT ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropTOP_BORDER ) )
	{
	int	fromNumber= ppFrom->ppTopBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppTopBorderNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropTOP_BORDER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropBOTTOM_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBottomBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBottomBorderNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropBOTTOM_BORDER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLEFT_BORDER ) )
	{
	int	fromNumber= ppFrom->ppLeftBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppLeftBorderNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropLEFT_BORDER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropRIGHT_BORDER ) )
	{
	int	fromNumber= ppFrom->ppRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppRightBorderNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropRIGHT_BORDER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropBETWEEN_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBetweenBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBetweenBorderNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropBETWEEN_BORDER ); }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropBAR_BORDER ) )
	{
	int	fromNumber= ppFrom->ppBarNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( ppTo->ppBarNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropBAR_BORDER ); }
	}


    if  ( PROPmaskISSET( ppCmpMask, PPpropOUTLINELEVEL ) )
	{
	if  ( ppTo->ppOutlineLevel != ppFrom->ppOutlineLevel )
	    { PROPmaskADD( &ppDifMask, PPpropOUTLINELEVEL );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLISTLEVEL ) )
	{
	if  ( ppTo->ppListLevel != ppFrom->ppListLevel )
	    { PROPmaskADD( &ppDifMask, PPpropLISTLEVEL );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropSHADING ) )
	{
	int	fromNumber= ppFrom->ppShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( ppTo->ppShadingNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropSHADING );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropFRAME ) )
	{
	int	fromNumber= ppFrom->ppFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( ppTo->ppFrameNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropFRAME );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropHYPHPAR ) )
	{
	if  ( ppTo->ppHyphenateParagraph != ppFrom->ppHyphenateParagraph )
	    { PROPmaskADD( &ppDifMask, PPpropHYPHPAR ); }
	}

    *pDifMask= ppDifMask;
    return;
    }

int docSetParaProperty(		ParagraphProperties *	pp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case PPpropSTYLE:
	    pp->ppStyle= arg;
	    break;

	case PPpropLISTOVERRIDE:
	    pp->ppListOverride= arg;
	    break;

	case PPprop_IN_TABLE:  /* intbl */
	    if  ( arg == 0 )
		{ pp->ppTableNesting= 0;	}
	    else{
		if  ( pp->ppTableNesting == 0 )
		    { pp->ppTableNesting= 1;	}
		}
	    break;

	case PPpropTABLE_NESTING:  /* itap */
	    pp->ppTableNesting= arg;
	    break;

	case PPpropLEFT_INDENT:
	    pp->ppLeftIndentTwips= arg;
	    break;

	case PPpropFIRST_INDENT:
	    pp->ppFirstIndentTwips= arg;
	    break;

	case PPpropRIGHT_INDENT:
	    pp->ppRightIndentTwips= arg;
	    break;

	case PPpropALIGNMENT:
	    pp->ppAlignment= arg;
	    break;

	case PPpropTAB_STOPS:
	    LDEB(prop);
	    return 0;

	case PPpropBREAK_KIND:
	    pp->ppBreakKind= arg;
	    break;

	case PPpropWIDCTLPAR:
	    pp->ppWidowControl= arg;
	    break;

	case PPpropKEEP:
	    pp->ppKeepOnPage= arg != 0;
	    break;

	case PPpropKEEPN:
	    pp->ppKeepWithNext= arg != 0;
	    break;

	case PPpropSPACE_BEFORE:
	    pp->ppSpaceBeforeTwips= arg;
	    break;

	case PPpropSPACE_AFTER:
	    pp->ppSpaceAfterTwips= arg;
	    break;

	case PPpropLINE_SPACING_DIST:
	    pp->ppLineSpacingTwips= arg;
	    break;

	case PPpropLINE_SPACING_MULT:
	    pp->ppLineSpacingIsMultiple= ( arg != 0 );
	    break;

	case PPpropTOP_BORDER:
	    pp->ppTopBorderNumber= arg;
	    break;

	case PPpropBOTTOM_BORDER:
	    pp->ppBottomBorderNumber= arg;
	    break;

	case PPpropLEFT_BORDER:
	    pp->ppLeftBorderNumber= arg;
	    break;

	case PPpropRIGHT_BORDER:
	    pp->ppRightBorderNumber= arg;
	    break;

	case PPprop_BOX_BORDER:
	    pp->ppTopBorderNumber= arg;
	    pp->ppBottomBorderNumber= arg;
	    pp->ppLeftBorderNumber= arg;
	    pp->ppRightBorderNumber= arg;
	    break;

	case PPpropBETWEEN_BORDER:
	    pp->ppBetweenBorderNumber= arg;
	    break;

	case PPpropBAR_BORDER:
	    pp->ppBarNumber= arg;
	    break;

	case PPpropOUTLINELEVEL:
	    if  ( arg >= 0 && arg <= PPoutlineBODYTEXT )
		{ pp->ppOutlineLevel= arg;			}
	    else{ pp->ppOutlineLevel= PPoutlineBODYTEXT;	}
	    break;

	case PPpropLISTLEVEL:
	    if  ( arg > 0 && arg < 10 )
		{ pp->ppListLevel= arg;	}
	    else{ pp->ppListLevel= 0;	}
	    break;

	case PPpropHYPHPAR:
	    pp->ppHyphenateParagraph= arg != 0;
	    break;

	case PPprop_LISB:
	case PPprop_LISA:
	    break;

	case PPprop_IGNORED:
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetParaProperty(		const ParagraphProperties *	pp,
				int				prop )
    {
    switch( prop )
	{
	case PPpropSTYLE:
	    return pp->ppStyle;

	case PPpropLISTOVERRIDE:
	    return pp->ppListOverride;

	case PPprop_IN_TABLE:  /* intbl */
	    return ( pp->ppTableNesting > 0 );

	case PPpropTABLE_NESTING:  /* itap */
	    return pp->ppTableNesting;

	case PPpropLEFT_INDENT:
	    return pp->ppLeftIndentTwips;

	case PPpropFIRST_INDENT:
	    return pp->ppFirstIndentTwips;

	case PPpropRIGHT_INDENT:
	    return pp->ppRightIndentTwips;

	case PPpropALIGNMENT:
	    return pp->ppAlignment;

	case PPpropTAB_STOPS:
	    LDEB(prop); return -1;

	case PPpropBREAK_KIND:
	    return pp->ppBreakKind;

	case PPpropWIDCTLPAR:
	    return pp->ppWidowControl;

	case PPpropKEEP:
	    return pp->ppKeepOnPage;

	case PPpropKEEPN:
	    return pp->ppKeepWithNext;

	case PPpropSPACE_BEFORE:
	    return pp->ppSpaceBeforeTwips;

	case PPpropSPACE_AFTER:
	    return pp->ppSpaceAfterTwips;

	case PPpropLINE_SPACING_DIST:
	    return pp->ppLineSpacingTwips;

	case PPpropLINE_SPACING_MULT:
	    return pp->ppLineSpacingIsMultiple;

	case PPpropTOP_BORDER:
	    return pp->ppTopBorderNumber;

	case PPpropBOTTOM_BORDER:
	    return pp->ppBottomBorderNumber;

	case PPpropLEFT_BORDER:
	    return pp->ppLeftBorderNumber;

	case PPpropRIGHT_BORDER:
	    return pp->ppRightBorderNumber;

	case PPprop_BOX_BORDER:
	    LDEB(prop); return -1;

	case PPpropBETWEEN_BORDER:
	    return pp->ppBetweenBorderNumber;

	case PPpropBAR_BORDER:
	    return pp->ppBarNumber;

	case PPpropOUTLINELEVEL:
	    return pp->ppOutlineLevel;

	case PPpropLISTLEVEL:
	    return pp->ppListLevel;

	case PPpropHYPHPAR:
	    return pp->ppHyphenateParagraph;

	case PPprop_LISB:
	case PPprop_LISA:
	case PPprop_IGNORED:
	    LDEB(prop); return -1;

	default:
	    LDEB(prop); return -1;
	}
    }

