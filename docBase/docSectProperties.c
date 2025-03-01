/************************************************************************/
/*									*/
/*  Manipulate iten properties in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<utilPropMask.h>

#   include	"docBreakKind.h"
#   include	"docSectProperties.h"
#   include	"docDocumentProperties.h"

#   define	MIN_COL_WIDE	( 20* 36 )
#   define	MIN_GAP_WIDE	( 20* 12 )

#   define	DEF_GAP_WIDE	( 20* 36 )

/************************************************************************/
/*									*/
/*   Translate section property numbers to notes property numbers.	*/
/*									*/
/************************************************************************/

const int DOCsectNOTE_PROP_MAP[FEPprop_COUNT]=
{
    SPpropFOOTNOTE_STARTNR,
    SPpropFOOTNOTE_JUSTIFICATION,
    -1, /* No SPpropFOOTNOTE_PLACEMENT, */
    SPpropFOOTNOTE_RESTART,
    SPpropFOOTNOTE_STYLE,

    SPpropENDNOTE_STARTNR,
    -1, /* No SPpropENDNOTE_JUSTIFICATION, */
    -1, /* No SPpropENDNOTE_PLACEMENT, */
    SPpropENDNOTE_RESTART,
    SPpropENDNOTE_STYLE,
};

const int * const DOCsectFOOTNOTE_PROP_MAP= DOCsectNOTE_PROP_MAP;
const int * const DOCsectENDNOTE_PROP_MAP= DOCsectNOTE_PROP_MAP+ NOTESprop_COUNT;

static const int DocSectIntProps[]=
    {
    DGpropPAGE_WIDTH,
    DGpropPAGE_HEIGHT,
    DGpropLEFT_MARGIN,
    DGpropRIGHT_MARGIN,
    DGpropTOP_MARGIN,
    DGpropBOTTOM_MARGIN,
    DGpropHEADER_POSITION,
    DGpropFOOTER_POSITION,
    DGpropGUTTER,
    DGpropMARGMIR,
    SPpropSTYLE,
    SPpropTITLEPG,
    SPpropENDPG,
    SPpropBREAK_KIND,
    SPpropNUMBER_STYLE,
    SPpropNUMBER_HYPHEN,
    SPpropPAGE_RESTART,
    SPpropRTOL,
    SPpropFST_HEADER_NO_PDF_ARTIFACT,
    SPpropFST_FOOTER_NO_PDF_ARTIFACT,
    SPpropLST_HEADER_NO_PDF_ARTIFACT,
    SPpropLST_FOOTER_NO_PDF_ARTIFACT,
    SPpropSTART_PAGE,
    SPpropCOLUMN_COUNT,
    SPpropCOLUMN_SPACING,
    SPpropLINEBETCOL,
    SPpropFOOTNOTE_STARTNR,
    SPpropFOOTNOTE_JUSTIFICATION,
    /* No SPpropFOOTNOTE_PLACEMENT, */
    SPpropFOOTNOTE_RESTART,
    SPpropFOOTNOTE_STYLE,
    SPpropENDNOTE_STARTNR,
    /* No SPpropENDNOTE_JUSTIFICATION, */
    /* No SPpropENDNOTE_PLACEMENT, */
    SPpropENDNOTE_RESTART,
    SPpropENDNOTE_STYLE,
    };

static const int DocSectIntPropCount= sizeof(DocSectIntProps)/sizeof(int);

/************************************************************************/
/*									*/
/*  Fill a mask with notes properties that are relevant for a section.	*/
/*  I.E: Set those section properties that are a note property.		*/
/*									*/
/************************************************************************/

void docFillSectNotesMask(	PropertyMask *	spMask )
    {
    int		i;

    for ( i= 0; i < FEPprop_COUNT; i++ )
	{
	if  ( DOCsectNOTE_PROP_MAP[i] >= 0 )
	    { PROPmaskADD( spMask, DOCsectNOTE_PROP_MAP[i] );	}
	}
    }

/************************************************************************/
/*									*/
/*  Make sure all column width are zero, or that none of them is.	*/
/*									*/
/************************************************************************/

static void docSectCheckFixedColumnWidth( SectionProperties *	sp )
    {
    if  ( sp->spColumnCount > 1 )
	{
	int			haveZero= 0;
	int			haveWidth= 0;
	int			col;
	SectionColumn *	sc;

	sc= sp->spColumns;
	for ( col= 0; col < sp->spColumnCount; sc++, col++ )
	    {
	    if  ( sc->scColumnWidthTwips == 0 )
		{ haveZero++;	}
	    else{ haveWidth++;	}
	    }

	if  ( haveZero && haveWidth )
	    {
	    sc= sp->spColumns;
	    for ( col= 0; col < sp->spColumnCount; sc++, col++ )
		{ sc->scColumnWidthTwips= 0; }
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change section properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdSectProperties(	PropertyMask *			pSpDoneMask,
				SectionProperties *		spTo,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet )
    {
    PropertyMask		doneMask;
    int				p;

    utilPropMaskClear( &doneMask );

    for ( p= 0; p < DocSectIntPropCount; p++ )
	{
	int	prop= DocSectIntProps[p];
	int	oval;
	int	nval;

	if  ( ! PROPmaskISSET( spSetMask, prop ) )
	    { continue;	}

	oval= docGetSectionProperty( spTo, prop );
	nval= docGetSectionProperty( spSet, prop );

	if  ( oval == nval )
	    { continue;	}

	if  ( docSetSectionProperty( spTo, prop, nval ) )
	    { LLDEB(prop,nval); return -1;	}

	PROPmaskADD( &doneMask, prop );
	}

    if  ( PROPmaskISSET( spSetMask, SPpropCOLUMN_COUNT ) )
	{
	if  ( spTo->spColumnCount != spSet->spColumnCount )
	    {
	    if  ( docSectionPropertiesSetColumnCount( spTo,
						    spSet->spColumnCount ) )
		{ LDEB(spSet->spColumnCount); return -1;	}

	    PROPmaskADD( &doneMask, SPpropCOLUMN_COUNT );
	    }
	}

    if  ( PROPmaskISSET( spSetMask, SPpropCOLUMNS ) )
	{
	int		count;

	if  ( spTo->spColumnCount < spSet->spColumnCount )
	    { count= spTo->spColumnCount;		}
	else{ count= spSet->spColumnCount;		}

	if  ( count > 1 )
	    {
	    const SectionColumn *	scFrom= spSet->spColumns;
	    SectionColumn *		scTo= spTo->spColumns;
	    int				col;

	    for ( col= 0; col < count- 1; scTo++, scFrom++, col++ )
		{
		if  ( scTo->scColumnWidthTwips != scFrom->scColumnWidthTwips )
		    {
		    scTo->scColumnWidthTwips= scFrom->scColumnWidthTwips;
		    PROPmaskADD( &doneMask, SPpropCOLUMNS );
		    }
		if  ( scTo->scSpaceAfterTwips != scFrom->scSpaceAfterTwips )
		    {
		    scTo->scSpaceAfterTwips= scFrom->scSpaceAfterTwips;
		    PROPmaskADD( &doneMask, SPpropCOLUMNS );
		    }
		}

	    if  ( scTo->scColumnWidthTwips != scFrom->scColumnWidthTwips )
		{
		scTo->scColumnWidthTwips= scFrom->scColumnWidthTwips;
		PROPmaskADD( &doneMask, SPpropCOLUMNS );
		}
	    }
	}

    docSectCheckFixedColumnWidth( spTo );

    if  ( pSpDoneMask )
	{ *pSpDoneMask= doneMask;	}

    return 0;
    }

void docSectPropertyDifference( PropertyMask *			pDiffMask,
				const SectionProperties *	sp1,
				const PropertyMask *		cmpMask,
				const SectionProperties *	sp2 )
    {
    PropertyMask		diffMask;
    int				p;

    utilPropMaskClear( &diffMask );

    for ( p= 0; p < DocSectIntPropCount; p++ )
	{
	int	prop= DocSectIntProps[p];
	int	oval;
	int	nval;

	if  ( ! PROPmaskISSET( cmpMask, prop ) )
	    { continue;	}

	oval= docGetSectionProperty( sp1, prop );
	nval= docGetSectionProperty( sp2, prop );

	if  ( oval == nval )
	    { continue;	}

	PROPmaskADD( &diffMask, prop );
	}

    if  ( PROPmaskISSET( cmpMask, SPpropCOLUMNS ) )
	{
	int		count;

	if  ( sp1->spColumnCount < sp2->spColumnCount )
	    { count= sp1->spColumnCount;		}
	else{ count= sp2->spColumnCount;		}

	if  ( count > 1 )
	    {
	    const SectionColumn *	sc1= sp1->spColumns;
	    const SectionColumn *	sc2= sp2->spColumns;
	    int				col;

	    for ( col= 0; col < count- 1; sc1++, sc2++, col++ )
		{
		if  ( sc1->scColumnWidthTwips != sc2->scColumnWidthTwips )
		    { PROPmaskADD( &diffMask, SPpropCOLUMNS );	}
		if  ( sc1->scSpaceAfterTwips != sc2->scSpaceAfterTwips )
		    { PROPmaskADD( &diffMask, SPpropCOLUMNS ); }
		}
	    if  ( sc1->scColumnWidthTwips != sc2->scColumnWidthTwips )
		{ PROPmaskADD( &diffMask, SPpropCOLUMNS ); }
	    }
	}

    *pDiffMask= diffMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Clean, Initialize section properties.				*/
/*									*/
/************************************************************************/

void docCleanSectionProperties(	SectionProperties *	sp )
    {
    if  ( sp->spColumns )
	{ free( sp->spColumns );	}

    return;
    }

void docInitSectionProperties(	SectionProperties *	sp )
    {
    geoInitDocumentGeometry( &(sp->spDocumentGeometry) );

    sp->spStyle= 0;

    sp->spColumnSpacingTwips= DEF_GAP_WIDE;
    sp->spLineBetweenColumns= 0;

    sp->spHasTitlePage= 0;
    sp->spHasEndPage= 0;
    sp->spBreakKind= DOCibkPAGE;
    sp->spPageNumberStyle= DOCpgnDEC;
    sp->spPageNumberHyphen= DOCpgnhPGNHNSH;
    sp->spRestartPageNumbers= 0;

    sp->spFirstPageHeaderNoPdfArtifact= 0;
    sp->spFirstPageFooterNoPdfArtifact= 0;
    sp->spLastPageHeaderNoPdfArtifact= 0;
    sp->spLastPageFooterNoPdfArtifact= 0;

    sp->spColumnCount= 1;
    sp->spColumns= (SectionColumn *)0;

    sp->spRToL= 0;
    sp->spStartPageNumber= 0;

    docInitFootEndNotesProperties( &(sp->spNotesProperties) );

    return;
    }

int docSectionPropertiesSetColumnCount(	SectionProperties *	sp,
					int			n )
    {
    if  ( n > 1 && sp->spColumnCount < n )
	{
	SectionColumn *	sc= (SectionColumn *)realloc( sp->spColumns,
						    n* sizeof(SectionColumn) );
	if  ( ! sc )
	    { LXDEB(n,sc); return -1;	}

	sp->spColumns= sc;

	if  ( sp->spColumnCount == 1 )
	    {
	    sc->scSpaceAfterTwips= 0;
	    sc->scColumnWidthTwips= 0;
	    }

	sc= sp->spColumns+ sp->spColumnCount;
	while( sp->spColumnCount < n )
	    {
	    sc->scSpaceAfterTwips= 0;
	    sc->scColumnWidthTwips= 0;

	    sc++; sp->spColumnCount++;
	    }
	}

    sp->spColumnCount= n;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy SectionProperties.						*/
/*									*/
/*  NOTE that the headers and footers are not copied.			*/
/*									*/
/************************************************************************/

int docCopySectionProperties(	SectionProperties *		to,
				const SectionProperties *	from )
    {
    if  ( docSectionPropertiesSetColumnCount( to, from->spColumnCount ) )
	{ LDEB(from->spColumnCount); return -1;	}

    to->spDocumentGeometry= from->spDocumentGeometry;

    to->spStyle= from->spStyle;

    to->spColumnSpacingTwips= from->spColumnSpacingTwips;
    to->spLineBetweenColumns= from->spLineBetweenColumns;

    to->spHasTitlePage= from->spHasTitlePage;
    to->spHasEndPage= from->spHasEndPage;
    to->spBreakKind= from->spBreakKind;
    to->spPageNumberStyle= from->spPageNumberStyle;
    to->spPageNumberHyphen= from->spPageNumberHyphen;
    to->spRestartPageNumbers= from->spRestartPageNumbers;
    to->spRToL= from->spRToL;

    to->spFirstPageHeaderNoPdfArtifact= from->spFirstPageHeaderNoPdfArtifact;
    to->spFirstPageFooterNoPdfArtifact= from->spFirstPageFooterNoPdfArtifact;
    to->spLastPageHeaderNoPdfArtifact= from->spLastPageHeaderNoPdfArtifact;
    to->spLastPageFooterNoPdfArtifact= from->spLastPageFooterNoPdfArtifact;

    to->spStartPageNumber= from->spStartPageNumber;

    /* docSectionPropertiesSetColumnCount() has allocated the memory */
    if  ( from->spColumnCount > 1 )
	{
	int		col;

	for ( col= 0; col < from->spColumnCount; col++ )
	    { to->spColumns[col]= from->spColumns[col];	}
	}

    to->spNotesProperties= from->spNotesProperties;

    docSectCheckFixedColumnWidth( to );

    return 0;
    }

int docSectSetEqualColumnWidth(	SectionProperties *	sp )
    {
    int				col;
    SectionColumn *		sc;
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
    int				pageWide;
    int				colWide;

    pageWide= geoContentWide( dg );

    if  ( sp->spColumnCount < 2 )
	{ return pageWide;	}

    colWide= ( pageWide- ( sp->spColumnCount- 1 )*
			    sp->spColumnSpacingTwips )/ sp->spColumnCount;

    sc=  sp->spColumns;
    for ( col= 0; col < sp->spColumnCount; sc++, col++ )
	{
	sc->scColumnWidthTwips= 0;
	sc->scSpaceAfterTwips= 0;
	}

    return colWide;
    }

int docSectSetExplicitColumnWidth(	SectionProperties *	sp )
    {
    int				col;
    SectionColumn *		sc;
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
    int				pageWide;
    int				colWide;

    pageWide= geoContentWide( dg );

    if  ( sp->spColumnCount < 2 )
	{ LDEB(sp->spColumnCount); return -1;	}

    colWide= ( pageWide- ( sp->spColumnCount- 1 )*
			    sp->spColumnSpacingTwips )/ sp->spColumnCount;

    if  ( colWide < MIN_COL_WIDE )
	{ LLDEB(colWide,MIN_COL_WIDE); return -1;	}

    sc=  sp->spColumns;
    for ( col= 0; col < sp->spColumnCount; sc++, col++ )
	{
	sc->scColumnWidthTwips= colWide;
	sc->scSpaceAfterTwips= sp->spColumnSpacingTwips;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the column margins.					*/
/*									*/
/*  Because of gutters and/or mirrored margins, the page geometry is	*/
/*  not necessarily identical to that in the section properties.	*/
/*									*/
/************************************************************************/

void docSectGetColumnX(		int *				pXLineBefore,
				int *				pX0,
				int *				pX1,
				const SectionProperties *	sp,
				const DocumentGeometry *	dgPage,
				int				column )
    {
    int		used= 0;
    int		col;

    int		colWide;
    int		halfGap;

    if  ( sp->spColumnCount < 2 )
	{
	*pX0= dgPage->dgMargins.roLeftOffset;
	*pX1= dgPage->dgPageWideTwips- dgPage->dgMargins.roRightOffset;
	return;
	}

    for ( col= 0; col < column; col++ )
	{
	if  ( sp->spColumns[col].scColumnWidthTwips == 0 )
	    { break;	}

	used += sp->spColumns[col].scColumnWidthTwips;
	used += sp->spColumns[col].scSpaceAfterTwips;
	}

    if  ( col < column || sp->spColumns[column].scColumnWidthTwips == 0 )
	{
	int		pageWide;

	pageWide= geoContentWide( dgPage );

	colWide= ( pageWide- ( sp->spColumnCount- 1 )*
			sp->spColumnSpacingTwips )/ sp->spColumnCount;
	halfGap= sp->spColumnSpacingTwips/ 2;

	used= column* ( colWide+ sp->spColumnSpacingTwips );
	}
    else{
	colWide= sp->spColumns[column].scColumnWidthTwips;

	if  ( column == 0 )
	    { halfGap= sp->spColumnSpacingTwips/ 2;			}
	else{ halfGap= sp->spColumns[column-1].scSpaceAfterTwips/ 2;	}
	}

    if  ( sp->spRToL )
	{
	int		x1;

	x1= dgPage->dgPageWideTwips- dgPage->dgMargins.roRightOffset- used;

	*pX0= x1- colWide;
	*pX1= x1;
	*pXLineBefore= x1+ halfGap;
	return;
	}
    else{
	int		x0= dgPage->dgMargins.roLeftOffset+ used;

	*pX0= x0;
	*pX1= x0+ colWide;
	*pXLineBefore= x0- halfGap;
	return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set a section property.						*/
/*									*/
/************************************************************************/

int docSetSectionProperty(	SectionProperties *	sp,
				int			prop,
				int			arg )
    {
    DocumentGeometry *		dg= &(sp->spDocumentGeometry);

    switch( prop )
	{
	case DGpropPAGE_WIDTH:
	    dg->dgPageWideTwips= arg;
	    break;
	case DGpropPAGE_HEIGHT:
	    dg->dgPageHighTwips= arg;
	    break;

	case DGpropLEFT_MARGIN:
	    dg->dgMargins.roLeftOffset= arg;
	    break;
	case DGpropRIGHT_MARGIN:
	    dg->dgMargins.roRightOffset= arg;
	    break;
	case DGpropTOP_MARGIN:
	    dg->dgMargins.roTopOffset= arg;
	    break;
	case DGpropBOTTOM_MARGIN:
	    dg->dgMargins.roBottomOffset= arg;
	    break;

	case DGpropHEADER_POSITION:
	    if  ( arg != 0 )
		{ dg->dgHeaderPositionTwips= arg; }
	    break;
	case DGpropFOOTER_POSITION:
	    if  ( arg != 0 )
		{ dg->dgFooterPositionTwips= arg; }
	    break;

	case DGpropGUTTER:
	    dg->dgGutterTwips= arg;
	    break;
	case DGpropMARGMIR:
	    dg->dgMirrorMargins= arg != 0;
	    break;

	case SPpropSTYLE:
	    sp->spStyle= arg;
	    break;

	case SPpropTITLEPG:
	    sp->spHasTitlePage= ( arg != 0 );
	    break;
	case SPpropENDPG:
	    sp->spHasEndPage= ( arg != 0 );
	    break;

	case SPpropBREAK_KIND:
	    sp->spBreakKind= arg;
	    break;

	case SPpropNUMBER_STYLE:
	    sp->spPageNumberStyle= arg;
	    break;

	case SPpropNUMBER_HYPHEN:
	    sp->spPageNumberHyphen= arg;
	    break;

	case SPpropPAGE_RESTART:
	    sp->spRestartPageNumbers= arg;
	    break;

	case SPpropRTOL:
	    sp->spRToL= arg != 0;
	    break;

	case SPpropFST_HEADER_NO_PDF_ARTIFACT:
	    sp->spFirstPageHeaderNoPdfArtifact= arg != 0;
	    break;
	case SPpropFST_FOOTER_NO_PDF_ARTIFACT:
	    sp->spFirstPageFooterNoPdfArtifact= arg != 0;
	    break;
	case SPpropLST_HEADER_NO_PDF_ARTIFACT:
	    sp->spLastPageHeaderNoPdfArtifact= arg != 0;
	    break;
	case SPpropLST_FOOTER_NO_PDF_ARTIFACT:
	    sp->spLastPageFooterNoPdfArtifact= arg != 0;
	    break;

	case SPpropSTART_PAGE:
	    /*  Count from 1! */
	    if  ( arg > 0 )
		{ sp->spStartPageNumber= arg -1;	}
	    else{ sp->spStartPageNumber= 0;		}
	    break;

	case SPpropCOLUMN_COUNT:
	    if  ( docSectionPropertiesSetColumnCount( sp, arg ) )
		{ LDEB(arg); return -1;	}
	    break;

	case SPpropCOLUMN_SPACING:
	    sp->spColumnSpacingTwips= arg;
	    break;
	case SPpropLINEBETCOL:
	    sp->spLineBetweenColumns= ( arg != 0 );
	    break;

	/* FOOTNOTE */
	case SPpropFOOTNOTE_STARTNR:
	    docSetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropSTARTNR, arg );
	    return 0;
	case SPpropFOOTNOTE_JUSTIFICATION:
	    docSetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropJUSTIFICATION, arg );
	    return 0;
	/* No
	case SPpropFOOTNOTE_PLACEMENT:
	    docSetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropPLACEMENT, arg );
	    return 0;
	*/
	case SPpropFOOTNOTE_RESTART:
	    docSetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropRESTART, arg );
	    return 0;
	case SPpropFOOTNOTE_STYLE:
	    docSetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropSTYLE, arg );
	    return 0;

	/* ENDNOTE */
	case SPpropENDNOTE_STARTNR:
	    docSetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropSTARTNR, arg );
	    return 0;
	/* No
	case SPpropENDNOTE_JUSTIFICATION:
	    docSetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropJUSTIFICATION, arg );
	    return 0;
	*/
	/* No
	case SPpropENDNOTE_PLACEMENT:
	    docSetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropPLACEMENT, arg );
	    return 0;
	*/
	case SPpropENDNOTE_RESTART:
	    docSetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropRESTART, arg );
	    return 0;
	case SPpropENDNOTE_STYLE:
	    docSetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropSTYLE, arg );
	    return 0;


	default:
	    LLDEB(prop,arg); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get a section property.						*/
/*									*/
/************************************************************************/

int docGetSectionProperty(	const SectionProperties *	sp,
				int				prop )
    {
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);

    switch( prop )
	{
	case DGpropPAGE_WIDTH:
	    return dg->dgPageWideTwips;
	case DGpropPAGE_HEIGHT:
	    return dg->dgPageHighTwips;

	case DGpropLEFT_MARGIN:
	    return dg->dgMargins.roLeftOffset;
	case DGpropRIGHT_MARGIN:
	    return dg->dgMargins.roRightOffset;
	case DGpropTOP_MARGIN:
	    return dg->dgMargins.roTopOffset;
	case DGpropBOTTOM_MARGIN:
	    return dg->dgMargins.roBottomOffset;

	case DGpropHEADER_POSITION:
	    return dg->dgHeaderPositionTwips;
	case DGpropFOOTER_POSITION:
	    return dg->dgFooterPositionTwips;

	case DGpropGUTTER:
	    return dg->dgGutterTwips;
	case DGpropMARGMIR:
	    return dg->dgMirrorMargins;

	case SPpropSTYLE:
	    return sp->spStyle;

	case SPpropTITLEPG:
	    return sp->spHasTitlePage;
	case SPpropENDPG:
	    return sp->spHasEndPage;

	case SPpropBREAK_KIND:
	    return sp->spBreakKind;

	case SPpropNUMBER_STYLE:
	    return sp->spPageNumberStyle;

	case SPpropNUMBER_HYPHEN:
	    return sp->spPageNumberHyphen;

	case SPpropPAGE_RESTART:
	    return sp->spRestartPageNumbers;

	case SPpropRTOL:
	    return sp->spRToL;

	case SPpropFST_HEADER_NO_PDF_ARTIFACT:
	    return sp->spFirstPageHeaderNoPdfArtifact;
	case SPpropFST_FOOTER_NO_PDF_ARTIFACT:
	    return sp->spFirstPageFooterNoPdfArtifact;
	case SPpropLST_HEADER_NO_PDF_ARTIFACT:
	    return sp->spLastPageHeaderNoPdfArtifact;
	case SPpropLST_FOOTER_NO_PDF_ARTIFACT:
	    return sp->spLastPageFooterNoPdfArtifact;

	case SPpropSTART_PAGE:
	    /* Count from 1 */
	    return sp->spStartPageNumber+ 1;

	case SPpropCOLUMN_COUNT:
	    return sp->spColumnCount;

	case SPpropCOLUMN_SPACING:
	    return sp->spColumnSpacingTwips;
	    break;
	case SPpropLINEBETCOL:
	    return sp->spLineBetweenColumns;
	    break;

	/* FOOTNOTE */
	case SPpropFOOTNOTE_STARTNR:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropSTARTNR );
	case SPpropFOOTNOTE_JUSTIFICATION:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropJUSTIFICATION );
	/* No
	case SPpropFOOTNOTE_PLACEMENT:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropPLACEMENT );
	*/
	case SPpropFOOTNOTE_RESTART:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropRESTART );
	case SPpropFOOTNOTE_STYLE:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepFootnotesProps),
						NOTESpropSTYLE );

	/* ENDNOTE */
	case SPpropENDNOTE_STARTNR:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropSTARTNR );
	/* No
	case SPpropENDNOTE_JUSTIFICATION:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropJUSTIFICATION );
	*/
	/* No
	case SPpropENDNOTE_PLACEMENT:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropPLACEMENT );
	*/
	case SPpropENDNOTE_RESTART:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropRESTART );
	case SPpropENDNOTE_STYLE:
	    return docGetNotesProperty( &(sp->spNotesProperties.fepEndnotesProps),
						NOTESpropSTYLE );


	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }
/************************************************************************/
/*									*/
/*  Return the sum of the other widths in a section with explicit	*/
/*  column layout. Additionally, assign a victim whose column width can	*/
/*  be narrowed to allocate extra space.				*/
/*									*/
/*  The maximum value is the value that leaves MIN_COL_WIDE as the	*/
/*  column width of the victim.						*/
/*									*/
/************************************************************************/

static int docSectColsSumOthers( int *				pVictim,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col,
				int				colOther )
    {
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
    int				pageWide;

    int				sumOthers= 0;
    int				i;

    int				victim;
    int				left;

    pageWide= geoContentWide( dg );

    for ( i= 0; i < sp->spColumnCount- 1; i++ )
	{
	if  ( i == col )
	    { continue;	}

	sumOthers += sp->spColumns[i].scColumnWidthTwips;
	sumOthers += sp->spColumns[i].scSpaceAfterTwips;
	}

    if  ( i != col )
	{ sumOthers += sp->spColumns[i].scColumnWidthTwips;	}

    sumOthers += colOther;

    if  ( col == sp->spColumnCount- 1 )
	{ victim= sp->spColumnCount-2;	}
    else{ victim= sp->spColumnCount-1;	}

    left= sp->spColumns[victim].scColumnWidthTwips- MIN_COL_WIDE;

    if  ( pVictim )
	{ *pVictim= victim;	}
    if  ( pMaxValue )
	{ *pMaxValue= pageWide- sumOthers+ left;	}

    return sumOthers;
    }

/************************************************************************/
/*									*/
/*  Claim extra space by subtracting it from the column width of the	*/
/*  victim. Applies for explicit column layout only.			*/
/*									*/
/************************************************************************/

static int docSectReserveWidth(	SectionProperties *	sp,
				int			victim,
				int			sumValues )
    {
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
    int				pageWide;

    pageWide= geoContentWide( dg );

    if  ( sumValues > pageWide )
	{
	int	narrowed;

	narrowed= pageWide- sumValues+ sp->spColumns[victim].scColumnWidthTwips;
	if  ( narrowed < MIN_COL_WIDE )
	    { LDEB(narrowed); return -1;	}

	sp->spColumns[victim].scColumnWidthTwips= narrowed;
	}

    return 0;
    }

/************************************************************************/

static void docSectGetEqualWidths(
				int *				pColWide,
				int *				pGapWide,
				int *				pMaxColWide,
				int *				pMaxGapWide,
				const SectionProperties *	sp )
    {
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
    int				pageWide;

    int				colWide;
    int				maxColWide;
    int				gapWide;
    int				maxGapWide;

    int				ncol= sp->spColumnCount;
    int				ngap= sp->spColumnCount- 1;

    pageWide= geoContentWide( dg );

    colWide=    ( pageWide- ngap* sp->spColumnSpacingTwips )/ ncol;
    maxColWide= ( pageWide- ngap* MIN_GAP_WIDE )/ ncol;

    gapWide= sp->spColumnSpacingTwips;
    if  ( ngap == 0 )
	{ maxGapWide= 0;					}
    else{ maxGapWide= ( pageWide- ncol* MIN_COL_WIDE )/ ngap;	}

    if  ( pColWide )
	{ *pColWide= colWide;	}
    if  ( pGapWide )
	{ *pGapWide= gapWide;	}
    if  ( pMaxColWide )
	{ *pMaxColWide= maxColWide;	}
    if  ( pMaxGapWide )
	{ *pMaxGapWide= maxGapWide;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the spacing right of a column.					*/
/*									*/
/************************************************************************/

int docSectGetColumnSpacing(	int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col )
    {
    int				value;
    int				maxValue;

    int				haveFixedWidth;

    haveFixedWidth= docSectPropsFixedWidthColumns( sp );

    if  ( haveFixedWidth )
	{ docSectGetEqualWidths( (int *)0, &value, (int *)0, &maxValue, sp ); }
    else{
	docSectColsSumOthers( (int *)0, &maxValue,
			sp, col, sp->spColumns[col].scColumnWidthTwips );

	value= sp->spColumns[col].scSpaceAfterTwips;
	}

    if  ( pMaxValue )
	{ *pMaxValue= maxValue;	}
    if  ( pMinValue )
	{ *pMinValue= MIN_GAP_WIDE;	}

    return value;
    }

/************************************************************************/
/*									*/
/*  Set the spacing right of a column.					*/
/*									*/
/************************************************************************/

int docSectSetColumnSpacing(	SectionProperties *	sp,
				int			col,
				int			newValue )
    {
    int		maxValue;
    int		haveFixedWidth;

    haveFixedWidth= docSectPropsFixedWidthColumns( sp );

    if  ( haveFixedWidth )
	{
	if  ( col > 0 )
	    { LLLDEB(haveFixedWidth,col,newValue); return -1;	}

	docSectGetEqualWidths( (int *)0, (int *)0, (int *)0, &maxValue, sp );
	if  ( newValue < MIN_GAP_WIDE || newValue > maxValue )
	    { LLDEB(newValue,maxValue); return -1;	}

	sp->spColumnSpacingTwips= newValue;
	}
    else{
	int		victim;
	int		sumOthers;

	sumOthers= docSectColsSumOthers( &victim, &maxValue,
			    sp, col, sp->spColumns[col].scColumnWidthTwips );

	if  ( newValue < MIN_GAP_WIDE || newValue > maxValue )
	    { LLDEB(newValue,maxValue); return -1;	}
	if  ( victim == col )
	    { LLDEB(victim,col); return -1;	}

	if  ( docSectReserveWidth( sp, victim, sumOthers+ newValue ) )
	    { LLDEB(sumOthers,newValue); return -1;	}

	sp->spColumns[col].scSpaceAfterTwips= newValue;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the spacing right of a column.					*/
/*									*/
/************************************************************************/

int docSectGetColumnWidth(	int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col )
    {
    int				value;
    int				maxValue;

    int				haveFixedWidth;

    haveFixedWidth= docSectPropsFixedWidthColumns( sp );

    if  ( haveFixedWidth )
	{
	docSectGetEqualWidths( &value, (int *)0, &maxValue, (int *)0, sp );
	}
    else{
	docSectColsSumOthers( (int *)0, &maxValue,
			sp, col, sp->spColumns[col].scSpaceAfterTwips );

	value= sp->spColumns[col].scColumnWidthTwips;
	}

    if  ( pMinValue )
	{ *pMinValue= MIN_COL_WIDE;	}
    if  ( pMaxValue )
	{ *pMaxValue= maxValue;	}

    return value;
    }

/************************************************************************/
/*									*/
/*  Set the spacing right of a column.					*/
/*									*/
/************************************************************************/

int docSectSetColumnWidth(	SectionProperties *	sp,
				int			col,
				int			newValue )
    {
    int				maxValue;
    int				haveFixedWidth;

    haveFixedWidth= docSectPropsFixedWidthColumns( sp );

    if  ( haveFixedWidth )
	{
	const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
	int				pageWide;
	int				gapWide= MIN_GAP_WIDE;

	docSectGetEqualWidths( (int *)0, &gapWide, &maxValue, (int *)0, sp );

	if  ( newValue < MIN_COL_WIDE || newValue > maxValue )
	    { LLDEB(newValue,maxValue); return -1;	}

	pageWide= geoContentWide( dg );

	/* Always true, but static checkers cannot know */
	if  ( sp->spColumnCount > 1 )
	    {
	    gapWide= ( pageWide- ( sp->spColumnCount* newValue ) )/ 
						    ( sp->spColumnCount- 1 );
	    }
	if  ( gapWide < MIN_GAP_WIDE )
	    { LLDEB(gapWide,MIN_GAP_WIDE); gapWide= MIN_GAP_WIDE;	}

	sp->spColumnSpacingTwips= gapWide;
	}
    else{
	int		victim;
	int		sumOthers;

	sumOthers= docSectColsSumOthers( &victim, &maxValue,
			    sp, col, sp->spColumns[col].scSpaceAfterTwips );

	if  ( newValue < MIN_COL_WIDE || newValue > maxValue )
	    { LLDEB(newValue,maxValue); return -1;	}
	if  ( victim == col )
	    { LLDEB(victim,col); return -1;	}

	if  ( docSectReserveWidth( sp, victim, sumOthers+ newValue ) )
	    { LLDEB(sumOthers,newValue); return -1;	}

	sp->spColumns[col].scColumnWidthTwips= newValue;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the kind of page where this section starts.			*/
/*  MS-Word only implements odd/even if the document has different odd	*/
/*  and even pages.							*/
/*									*/
/************************************************************************/

int docSectGetBreakKind( const SectionProperties *	sp,
			const DocumentProperties *	dp,
			int				honourSpecialBreaks )
    {
    if  ( ( sp->spBreakKind == DOCibkEVEN	||
	    sp->spBreakKind == DOCibkODD	)	&&
	    ! honourSpecialBreaks			&&
	    ! dp->dpHasFacingPages			)
	{ return DOCibkPAGE;	}

    return sp->spBreakKind;
    }
