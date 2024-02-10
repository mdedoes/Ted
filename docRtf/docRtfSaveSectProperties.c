/************************************************************************/
/*									*/
/*  Exchange of section properties with RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	"docRtfTags.h"
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	<docBuf.h>
#   include	<utilPropMask.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write Section Properties. For a node, or for the stylesheet.	*/
/*									*/
/************************************************************************/

static const int SectRegularProperties[]=
{
    DGpropPAGE_WIDTH,
    DGpropPAGE_HEIGHT,
    DGpropLEFT_MARGIN,
    DGpropTOP_MARGIN,
    DGpropRIGHT_MARGIN,
    DGpropBOTTOM_MARGIN,
    DGpropGUTTER,
    DGpropMARGMIR,
    DGpropHEADER_POSITION,
    DGpropFOOTER_POSITION,
    SPpropRTOL,
    SPpropSTYLE,
    SPpropTITLEPG,
    SPpropENDPG,
    SPpropBREAK_KIND,
    SPpropNUMBER_STYLE,
    SPpropNUMBER_HYPHEN,
    SPpropPAGE_RESTART,
    SPpropSTART_PAGE, /*  Count from 1! */
    SPpropFST_HEADER_NO_PDF_ARTIFACT,
    SPpropFST_FOOTER_NO_PDF_ARTIFACT,
    SPpropLST_HEADER_NO_PDF_ARTIFACT,
    SPpropLST_FOOTER_NO_PDF_ARTIFACT,
    SPpropCOLUMN_COUNT,
    SPpropCOLUMN_SPACING,
};

static const int SectRegularPropertyCount=
			sizeof(SectRegularProperties)/sizeof(int);

int docRtfSaveSectionProperties( RtfWriter *			rw,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet )
    {
    const int			scope= RTCscopeSECT;

    if  ( docRtfWriteItemProperties( rw, scope, spSet,
			(RtfGetIntProperty)docGetSectionProperty, spSetMask,
			SectRegularProperties, SectRegularPropertyCount ) )
	{ LLDEB(scope,SectRegularPropertyCount); return -1;	}

    if  ( PROPmaskISSET( spSetMask, SPpropCOLUMNS )	||
	  spSet->spColumnCount > 1			)
	{
	int			i;
	const SectionColumn *	sc= spSet->spColumns;


	for ( i= 0; i < spSet->spColumnCount; sc++, i++ )
	    {
	    if  ( sc->scColumnWidthTwips == 0	&&
		  sc->scSpaceAfterTwips == 0	)
		{ continue;	}

	    docRtfWriteArgTag( rw, "colno", i+ 1 );

	    if  ( sc->scColumnWidthTwips != 0 )
		{
		docRtfWriteArgTag( rw, "colw", sc->scColumnWidthTwips );
		}
	    if  ( sc->scSpaceAfterTwips != 0 )
		{
		docRtfWriteArgTag( rw, "colsr", sc->scSpaceAfterTwips );
		}
	    }
	}

    if  ( PROPmaskISSET( spSetMask, SPpropLINEBETCOL ) )
	{ docRtfWriteFlagTag( rw, "linebetcol", spSet->spLineBetweenColumns ); }

    docRtfWriteNextLine( rw );

    /**************/

    docRtfSaveNotesProperties( rw, spSetMask,
			    &(spSet->spNotesProperties.fepFootnotesProps),
			    DOCsectFOOTNOTE_PROP_MAP, "sftnstart",
			    DOCrtf_SectFootNotesJustificationTags,
			    DOCrtf_SectFootNotesJustificationTagCount,
			    (const char **)0, 0,
			    DOCrtf_SectFootNotesRestartTags,
			    DOCrtf_SectFootNotesRestartTagCount,
			    DOCrtf_SectFootNotesNumberStyleTags,
			    DOCrtf_SectFootNotesNumberStyleTagCount );

    docRtfSaveNotesProperties( rw, spSetMask,
			    &(spSet->spNotesProperties.fepEndnotesProps),
			    DOCsectENDNOTE_PROP_MAP, "saftnstart",
			    DOCrtf_SectEndNotesJustificationTags,
			    DOCrtf_SectEndNotesJustificationTagCount,
			    (const char **)0, 0,
			    DOCrtf_SectEndNotesRestartTags,
			    DOCrtf_SectEndNotesRestartTagCount,
			    DOCrtf_SectEndNotesNumberStyleTags,
			    DOCrtf_SectEndNotesNumberStyleTagCount );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write Section Properties for a node					*/
/*									*/
/*  1)  Only save geometry if it differs from that of the document.	*/
/*  2)  Then always start from the default for this document.		*/
/*									*/
/************************************************************************/

int docRtfSaveSectionPropertiesOfNode(
			    RtfWriter *				rw,
			    const struct BufferItem *		sectNode )
    {
    const SectionProperties *	spSet= sectNode->biSectProperties;
    const DocumentProperties *	dp= rw->rwDocument->bdProperties;

    SectionProperties		spDef;
    PropertyMask		dgSetMask;
    PropertyMask		spCmpMask;
    PropertyMask		spDifMask;

    /*  1  */
    docInitSectionProperties( &spDef );

    utilPropMaskClear( &dgSetMask );
    utilPropMaskFill( &dgSetMask, DGprop_COUNT );
    PROPmaskUNSET( &dgSetMask, DGpropHEADER_POSITION );
    PROPmaskUNSET( &dgSetMask, DGpropFOOTER_POSITION );

    geoUpdDocumentGeometry( (PropertyMask *)0, &(spDef.spDocumentGeometry),
						&dgSetMask, &(dp->dpGeometry) );

    spDef.spNotesProperties= dp->dpNotesProps;

    utilPropMaskClear( &spCmpMask );
    utilPropMaskFill( &spCmpMask, SPprop_COUNT );

    utilPropMaskClear( &spDifMask );
    docSectPropertyDifference( &spDifMask, &spDef, &spCmpMask, spSet );

    if  ( rw->rwCurrentTree->ptSelection )
	{
	PROPmaskUNSET( &spDifMask, SPpropTITLEPG );
	PROPmaskUNSET( &spDifMask, SPpropENDPG );
	}

    if  ( rw->rwSpExtraMask )
	{ utilPropMaskOr( &spDifMask, &spDifMask, rw->rwSpExtraMask );	}

    docRtfWriteNextLine( rw );
    docRtfWriteTag( rw, "sectd" );

    /* Make consecutive sections independent */
    docRtfWriteTag( rw, RTFtag_pard );
    docCleanParagraphProperties( &(rw->rwCurrentTree->ptParagraphProperties) );
    docInitParagraphProperties( &(rw->rwCurrentTree->ptParagraphProperties) );

    docRtfSaveSectionProperties( rw, &spDifMask, spSet );

    rw->rwSectionPropertiesSaved= 1;

  /*ready:*/

    docCleanSectionProperties( &spDef );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a header or a footer.. If it exists.				*/
/*									*/
/************************************************************************/

static int docRtfSaveSectHeaderFooter(
				RtfWriter *			rw,
				const struct BufferItem *	sectNode,
				const struct BufferItem *	prevNode,
				const char *			tag,
				int				treeType )
    {
    const int			forcePar= 1;
    struct DocumentTree *	tree;
    int				evenIfAbsent= 0;
    unsigned char		applies= 1;

    tree= docSectionHeaderFooter( sectNode, &applies,
				rw->rwDocument->bdProperties, treeType );

    if  ( prevNode )
	{
	const struct DocumentTree *	prevDt;

	prevDt= docSectionHeaderFooter( prevNode, (unsigned char *)0,
				rw->rwDocument->bdProperties, treeType );
	if  ( prevDt && prevDt->dtRoot )
	    { evenIfAbsent= 1;	}
	}

    if  ( tree )
	{
	if  ( docRtfSaveDocumentTree( rw, tag, tree, evenIfAbsent, forcePar ) )
	    { LSDEB(treeType,tag); return -1;	}
	}

    return 0;
    }

/************************************************************************/

typedef struct HeaderFooterType
    {
    const char *	hftTag;
    int			htfTreeType;
    } HeaderFooterType;

static const HeaderFooterType	HeaderFooterTypes[]=
{
    { "headerf",	DOCinFIRST_HEADER,	},
    { "headerl",	DOCinLEFT_HEADER,	},
    { "headerr",	DOCinRIGHT_HEADER,	},
    { "*\\headere",	DOCinLAST_HEADER,	},

    { "footerf",	DOCinFIRST_FOOTER,	},
    { "footerl",	DOCinLEFT_FOOTER,	},
    { "footerr",	DOCinRIGHT_FOOTER,	},
    { "*\\footere",	DOCinLAST_FOOTER,	},
};

static const int HeaderFooterTypeCount=
			    sizeof(HeaderFooterTypes)/sizeof(HeaderFooterType);

/************************************************************************/
/*									*/
/*  Save the headers and footers of a Section.				*/
/*									*/
/************************************************************************/

int docRtfSaveSectHeadersFooters(
				RtfWriter *			rw,
				const struct BufferItem *	sectNode )
    {
    const struct BufferItem *	prevNode= (const struct BufferItem *)0;

    int				hdft;

    if  ( sectNode->biParent				&&
	  sectNode->biNumberInParent > 0		)
	{
	prevNode= sectNode->biParent->biChildren[
					    sectNode->biNumberInParent- 1];
	}

#   if 0
    {
    const DocumentProperties *	dp= rw->rwDocument->bdProperties;

    /*  Word 11+ uses right header/footer anyway. For compatibility: */
    /*  Now that Open/Libre Office behaves like MS-Word No longer do this. */
    if  ( ! dp->dpHasFacingPages )
	{
	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevNode,
					    "header", DOCinRIGHT_HEADER ) )
	    { LDEB(1); return -1;	}

	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevNode,
					    "footer", DOCinRIGHT_FOOTER ) )
	    { LDEB(1); return -1;	}
	}
    }
#   endif

    for ( hdft= 0; hdft < HeaderFooterTypeCount; hdft++ )
	{
	const HeaderFooterType* hft= HeaderFooterTypes+ hdft;

	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevNode,
					    hft->hftTag, hft->htfTreeType ) )
	    { SDEB(hft->hftTag); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save an individial header or footer.				*/
/*  Used to trace just this header or footer.				*/
/*									*/
/************************************************************************/

int docRtfSaveHeaderFooter(	RtfWriter *		rw,
				struct DocumentTree *	tree )
    {
    const int			forcePar= 1;
    const int			evenIfAbsent= 0;

    int				hdft;

    if  ( ! tree->dtRoot )
	{ XDEB(tree->dtRoot); return -1;	}

    for ( hdft= 0; hdft < HeaderFooterTypeCount; hdft++ )
	{
	const HeaderFooterType* hft= HeaderFooterTypes+ hdft;

	if  ( hft->htfTreeType != tree->dtRoot->biTreeType )
	    { continue;	}

	if  ( docRtfSaveDocumentTree( rw, hft->hftTag, tree,
						    evenIfAbsent, forcePar ) )
	    { SDEB(hft->hftTag); return -1;	}

	return 0;
	}

    SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
    return -1;
    }
