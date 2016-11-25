/************************************************************************/
/*									*/
/*  Manipulate iten properties in a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<utilPropMask.h>

#   include	"docSectProperties.h"

/************************************************************************/
/*									*/
/*  Change section properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdSectProperties(	PropertyMask *			pSpChgMask,
				SectionProperties *		to,
				const PropertyMask *		updMask,
				const SectionProperties *	from )
    {
    PropertyMask		chgMask;
    PropertyMask		dgMask;

    PROPmaskCLEAR( &chgMask );

    PROPmaskCLEAR( &dgMask );
    PROPmaskFILL( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, updMask );

    appSetDocumentGeometry( &(to->spDocumentGeometry),
					&(from->spDocumentGeometry),
					&chgMask, &dgMask );

    if  ( PROPmaskISSET( updMask, SPpropSTYLE ) )
	{
	if  ( to->spStyle != from->spStyle )
	    {
	    to->spStyle= from->spStyle;
	    PROPmaskADD( &chgMask, SPpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropTITLEPG ) )
	{
	if  ( to->spHasTitlePage != from->spHasTitlePage )
	    {
	    to->spHasTitlePage= from->spHasTitlePage;
	    PROPmaskADD( &chgMask, SPpropTITLEPG );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropBREAK_KIND ) )
	{
	if  ( to->spBreakKind != from->spBreakKind )
	    {
	    to->spBreakKind= from->spBreakKind;
	    PROPmaskADD( &chgMask, SPpropBREAK_KIND );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropNUMBER_STYLE ) )
	{
	if  ( to->spPageNumberStyle != from->spPageNumberStyle )
	    {
	    to->spPageNumberStyle= from->spPageNumberStyle;
	    PROPmaskADD( &chgMask, SPpropNUMBER_STYLE );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropPAGE_RESTART ) )
	{
	if  ( to->spRestartPageNumbers != from->spRestartPageNumbers )
	    {
	    to->spRestartPageNumbers= from->spRestartPageNumbers;
	    PROPmaskADD( &chgMask, SPpropPAGE_RESTART );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropSTART_PAGE ) )
	{
	if  ( to->spStartPageNumber != from->spStartPageNumber )
	    {
	    to->spStartPageNumber= from->spStartPageNumber;
	    PROPmaskADD( &chgMask, SPpropSTART_PAGE );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_COUNT ) )
	{
	if  ( to->spColumnCount != from->spColumnCount )
	    {
	    to->spColumnCount= from->spColumnCount;
	    PROPmaskADD( &chgMask, SPpropCOLUMN_COUNT );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_SPACING ) )
	{
	if  ( to->spColumnSpacingTwips != from->spColumnSpacingTwips )
	    {
	    to->spColumnSpacingTwips= from->spColumnSpacingTwips;
	    PROPmaskADD( &chgMask, SPpropCOLUMN_SPACING );
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_WIDTH ) )
	{
	if  ( to->spColumnWidthTwips != from->spColumnWidthTwips )
	    {
	    to->spColumnWidthTwips= from->spColumnWidthTwips;
	    PROPmaskADD( &chgMask, SPpropCOLUMN_WIDTH );
	    }
	}

    *pSpChgMask= chgMask; return 0;
    }

void docSectPropertyDifference( PropertyMask *			pChgMask,
				const SectionProperties *	sp1,
				const SectionProperties *	sp2,
				const PropertyMask *		updMask )
    {
    PropertyMask		chgMask;
    PropertyMask		dgMask;

    DocumentGeometry		dg;

    PROPmaskCLEAR( &chgMask );

    PROPmaskCLEAR( &dgMask );
    PROPmaskFILL( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, updMask );

    dg= sp1->spDocumentGeometry;
    appSetDocumentGeometry( &dg, &(sp2->spDocumentGeometry),
							&chgMask, &dgMask );

    if  ( PROPmaskISSET( updMask, SPpropSTYLE ) )
	{
	if  ( sp1->spStyle != sp2->spStyle )
	    { PROPmaskADD( &chgMask, SPpropSTYLE ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropTITLEPG ) )
	{
	if  ( sp1->spHasTitlePage != sp2->spHasTitlePage )
	    { PROPmaskADD( &chgMask, SPpropTITLEPG ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropBREAK_KIND ) )
	{
	if  ( sp1->spBreakKind != sp2->spBreakKind )
	    { PROPmaskADD( &chgMask, SPpropBREAK_KIND ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropNUMBER_STYLE ) )
	{
	if  ( sp1->spPageNumberStyle != sp2->spPageNumberStyle )
	    { PROPmaskADD( &chgMask, SPpropNUMBER_STYLE ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropPAGE_RESTART ) )
	{
	if  ( sp1->spRestartPageNumbers != sp2->spRestartPageNumbers )
	    { PROPmaskADD( &chgMask, SPpropPAGE_RESTART ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropSTART_PAGE ) )
	{
	if  ( sp1->spStartPageNumber != sp2->spStartPageNumber )
	    { PROPmaskADD( &chgMask, SPpropSTART_PAGE ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_COUNT ) )
	{
	if  ( sp1->spColumnCount != sp2->spColumnCount )
	    { PROPmaskADD( &chgMask, SPpropCOLUMN_COUNT ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_SPACING ) )
	{
	if  ( sp1->spColumnSpacingTwips != sp2->spColumnSpacingTwips )
	    { PROPmaskADD( &chgMask, SPpropCOLUMN_SPACING ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_WIDTH ) )
	{
	if  ( sp1->spColumnWidthTwips != sp2->spColumnWidthTwips )
	    { PROPmaskADD( &chgMask, SPpropCOLUMN_WIDTH ); }
	}

    *pChgMask= chgMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Clean, Initialize section properties.				*/
/*									*/
/************************************************************************/

void docCleanSectionProperties(	SectionProperties *	sp )
    {
    if  ( sp->spParagraphNumbers )
	{ free( sp->spParagraphNumbers );	}

    return;
    }

void docInitSectionProperties(	SectionProperties *	sp )
    {
    appInitDocumentGeometry( &(sp->spDocumentGeometry) );

    sp->spStyle= 0;

    sp->spColumnCount= 1;
    sp->spColumnSpacingTwips= 720;
    sp->spColumnWidthTwips= 0;

    sp->spHasTitlePage= 0;
    sp->spBreakKind= DOCsbkPAGE;
    sp->spPageNumberStyle= DOCpgnDEC;
    sp->spRestartPageNumbers= 0;

    sp->spStartPageNumber= 0;

    sp->spParagraphNumbers= (ParagraphNumber *)0;
    sp->spParagraphNumberCount= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Copy SectionProperties.						*/
/*									*/
/*  NOTE that the headers and footers or the paragraph numbers for	*/
/*  bulleted lists etc are not copied.					*/
/*									*/
/************************************************************************/

int docCopySectionProperties(	SectionProperties *		to,
				const SectionProperties *	from	)
    {
    to->spDocumentGeometry= from->spDocumentGeometry;

    to->spStyle= from->spStyle;

    to->spColumnCount= from->spColumnCount;
    to->spColumnSpacingTwips= from->spColumnSpacingTwips;
    to->spColumnWidthTwips= from->spColumnWidthTwips;

    to->spHasTitlePage= from->spHasTitlePage;
    to->spBreakKind= from->spBreakKind;
    to->spPageNumberStyle= from->spPageNumberStyle;
    to->spRestartPageNumbers= from->spRestartPageNumbers;

    to->spStartPageNumber= from->spStartPageNumber;

    return 0;
    }

