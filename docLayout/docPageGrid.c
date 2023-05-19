/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docParaNodeProperties.h>
#   include	<docRowProperties.h>
#   include	<docPropVal.h>
#   include	<docFrameProperties.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	<docParaProperties.h>
#   include	<docCellProperties.h>
#   include	<docHeaderFooterScopes.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Administrative routines: manage a block frame.			*/
/*									*/
/************************************************************************/

void docLayoutInitBlockFrame(	BlockFrame *	bf )
    {
    bf->bfPage= -1;
    bf->bfColumn= -1;
    geoInitRectangle( &(bf->bfFlowRect) );
    geoInitRectangle( &(bf->bfContentRect) );

    geoInitDocumentGeometry( &(bf->bfPageGeometry) );

    bf->bfFootnotesPlaced= 0;
    docInitNotesReservation( &(bf->bfNotesReservation) );
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

void docLayoutCleanBlockFrame(	BlockFrame *	bf )
    {
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void docInitNotesReservation(	NotesReservation *	nr )
    {
    nr->nrDfFirstFootnote= (struct DocumentField *)0;
    nr->nrFootnoteCount= 0;
    nr->nrFootnoteSectNode= (const struct BufferItem *)0;

    nr->nrFtnsepHeight= 0;
    nr->nrFootnoteHeight= 0;
    }

/************************************************************************/

static void docBlockFramePageGeometry(	DocumentGeometry *		dg,
					const DocumentProperties *	dp,
					const SectionProperties *	sp,
					int				page )
    {
    if  ( sp->spDocumentGeometry.dgMirrorMargins )
	{
	if  ( dp->dpHasFacingPages && page % 2 )
	    {
	    dg->dgMargins.roLeftOffset= sp->spDocumentGeometry.dgMargins.roRightOffset;
	    dg->dgMargins.roRightOffset= sp->spDocumentGeometry.dgMargins.roLeftOffset;
	    }
	}
    else{
	if  ( dp->dpHasFacingPages && page % 2 )
	    {
	    dg->dgMargins.roRightOffset += sp->spDocumentGeometry.dgGutterTwips;
	    }
	else{
	    dg->dgMargins.roLeftOffset += sp->spDocumentGeometry.dgGutterTwips;
	    }
	}

    /*  Should be irrelevant: */
    dg->dgMirrorMargins= 0;
    dg->dgGutterTwips= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/*  1)  The block starts below what has been reserved for the page	*/
/*	header.								*/
/*  2)  The block starts above what has been reserved for the page	*/
/*	footer.								*/
/*									*/
/************************************************************************/

void docSectionBlockFrameTwips(	BlockFrame *			bf,
				const struct BufferItem *	node,
				const struct BufferItem *	bodySectNode,
				struct BufferDocument *		bd,
				int				page,
				int				column )
    {
    int				useColumns= 0;

    int				newFrame;
    int				prevY1;

    const struct BufferItem *	sectNode;

    const SectionProperties *	sp;

    sectNode= docGetSectNode( (struct BufferItem *)node );
    if  ( ! sectNode )
	{ XDEB(sectNode); return;	}

    sp= sectNode->biSectProperties;

    newFrame= page != bf->bfPage ||
	( bf->bfNotesReservation.nrFootnoteSectNode == bodySectNode &&
						    column != bf->bfColumn );

    if  ( sectNode->biTreeType != DOCinBODY && ! bodySectNode )
	{ SXDEB(docTreeTypeStr(sectNode->biTreeType),bodySectNode);	}

    if  ( sectNode->biTreeType != DOCinBODY	&&
	  sectNode->biTreeType != DOCinSHPTXT	&&
	  bodySectNode				)
	{ sp= bodySectNode->biSectProperties;	}

    bf->bfPageGeometry= sp->spDocumentGeometry;

    bf->bfPage= page;
    bf->bfColumn= column;

    bf->bfContentRect.drX0= bf->bfPageGeometry.dgMargins.roLeftOffset;
    bf->bfContentRect.drX1= bf->bfPageGeometry.dgPageWideTwips-
					bf->bfPageGeometry.dgMargins.roRightOffset;

    if  ( sectNode->biTreeType != DOCinSHPTXT )
	{
	docBlockFramePageGeometry( &(bf->bfPageGeometry),
						bd->bdProperties, sp, page );
	}

    useColumns= docTreeInColumnType( sectNode->biTreeType );

    if  ( useColumns && sp->spColumnCount > 1 )
	{
	int	xLine;

	docSectGetColumnX( &xLine, &(bf->bfContentRect.drX0),
				    &(bf->bfContentRect.drX1),
				    sp, &(bf->bfPageGeometry), column );
	}

    if  ( sectNode->biTreeType != DOCinBODY || newFrame )
	{ docInitNotesReservation( &(bf->bfNotesReservation) );	}

    switch( sectNode->biTreeType )
	{
	int		isEmpty;

	case DOCinBODY:

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    {
	    DocumentTree *	headerTree= (DocumentTree *)0;

	    docLayoutWhatPageHeader( &headerTree, &isEmpty,
							sectNode, page, bd );

	    /*  1 Reserved!!  */
	    if  ( headerTree && headerTree->dtRoot )
		{
		bf->bfContentRect.drY0= headerTree->dtY1ReservedTwips;
		}
	    else{
		bf->bfContentRect.drY0=
				bf->bfPageGeometry.dgMargins.roTopOffset;	
		}
	    }

	    {
	    DocumentTree *	footerTree= (DocumentTree *)0;

	    docLayoutWhatPageFooter( &footerTree, &isEmpty,
							sectNode, page, bd );

	    /*  2 Reserved!!  */
	    if  ( footerTree && footerTree->dtRoot )
		{
		bf->bfContentRect.drY1= footerTree->dtY0ReservedTwips;
		}
	    else{
		bf->bfContentRect.drY1=
				bf->bfPageGeometry.dgPageHighTwips-
				bf->bfPageGeometry.dgMargins.roBottomOffset;
		}
	    }

	    /*
	    appDebug( "PAGE %3d BLOCK Y: %5d..%5d %s\n",
			page, bf->bfContentRect.drY0, bf->bfContentRect.drY1,
			docTreeTypeStr( sectNode->biTreeType ) );
	    */

	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    {
	    DocumentTree *	tree;

	    if  ( column != 0 )
		{
		SLLDEB(docTreeTypeStr(sectNode->biTreeType),page,column);
		/* Unused
		column= 0;
		*/
		}

	    tree= docSectionHeaderFooter( bodySectNode, (unsigned char *)0,
				bd->bdProperties, sectNode->biTreeType );
	    if  ( ! tree )
		{ XDEB(tree); return;	}

	    bf->bfContentRect.drY0= tree->dtY0ReservedTwips;
	    bf->bfContentRect.drY1= tree->dtY1ReservedTwips;
	    }
	    break;

	case DOCinSHPTXT:

	    bf->bfContentRect.drY0= bf->bfPageGeometry.dgMargins.roTopOffset;
	    bf->bfContentRect.drY1=
			    bf->bfPageGeometry.dgPageHighTwips-
			    bf->bfPageGeometry.dgMargins.roBottomOffset;
	    break;

	default:
	    LDEB(sectNode->biTreeType);
	}

    prevY1= bf->bfContentRect.drY1;

    bf->bfFlowRect= bf->bfContentRect;

    if  ( sectNode->biTreeType == DOCinBODY && ! newFrame )
	{ bf->bfFlowRect.drY1= prevY1;	}

    return;
    }

void docBlockFrameTwips(	BlockFrame *			bf,
				const struct BufferItem *	node,
				struct BufferDocument *		bd,
				int				page,
				int				column )
    {
    struct BufferItem *		bodySectNode;
    struct BufferItem *		sectNode;

    const NotesProperties *	npEndnotes= &(bd->bdProperties->dpNotesProps.fepEndnotesProps);

    sectNode= docGetSectNode( (struct BufferItem *)node );
    if  ( ! sectNode )
	{ XDEB(sectNode); return;	}

    if  ( sectNode->biTreeType == DOCinENDNOTE		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	const struct BufferItem *	rootNode= bd->bdBody.dtRoot;

	bodySectNode= rootNode->biChildren[rootNode->biChildCount- 1];
	}

    if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0,
					&bodySectNode, bd,
					&(sectNode->biSectSelectionScope) ) )
	{ LDEB(1); return;	}

    docSectionBlockFrameTwips( bf, node, bodySectNode, bd, page, column );
    }

void docParaBlockFrameTwips(	BlockFrame *			bf,
				struct BufferItem *		paraNode,
				const struct BufferItem *	bodySectNode,
				struct BufferDocument *		bd,
				const LayoutPosition *		lpHere )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ LDEB(paraNode->biLevel);	}
    else{
	const FrameProperties *	fp;

	fp= docParaNodeGetFrameProperties( (int *)0, paraNode, bd );
	if  ( DOCisFRAME( fp ) )
	    {
	    BlockFrame		bfPage;
	    LayoutPosition	lpScratch;

	    int			frameHigh= fp->fpHighTwips;

	    if  ( frameHigh < 0 )
		{ frameHigh= -frameHigh;	}

	    docInitLayoutPosition( &lpScratch );

	    docLayoutInitBlockFrame( bf );
	    docLayoutInitBlockFrame( &bfPage );

	    docBlockFrameTwips( &bfPage, paraNode, bd,
					    lpHere->lpPage, lpHere->lpColumn );
	    docLayoutSetTextFrame( bf, &lpScratch, &bfPage, fp, frameHigh );

	    docLayoutCleanBlockFrame( &bfPage );

	    return;
	    }
	}

    docSectionBlockFrameTwips( bf, paraNode, bodySectNode, bd,
					    lpHere->lpPage, lpHere->lpColumn );
    }

/************************************************************************/

static int docGetCellXRange(	DocumentRectangle *		drCellFrame,
				DocumentRectangle *		drCellContent,
				const struct BufferItem *	cellNode,
				const RowProperties *		rp,
				const DocumentRectangle *	drRowParent )
    {
    int				col= cellNode->biNumberInParent;
    const CellProperties *	cp= rp->rpCells+ col;

    int				headGap= rp->rpHalfGapWidthTwips;
    int				tailGap= rp->rpHalfGapWidthTwips;

    switch( rp->rpLeftCellPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    headGap= rp->rpLeftCellPadding;
	    break;
	default:
	    LDEB(rp->rpLeftCellPaddingUnit);
	    return -1;
	}

    switch( cp->cpLeftPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    headGap= cp->cpLeftPadding;
	    break;
	default:
	    LDEB(cp->cpLeftPaddingUnit);
	    return -1;
	}

    switch( rp->rpRightCellPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    tailGap= rp->rpRightCellPadding;
	    break;
	default:
	    LDEB(rp->rpRightCellPaddingUnit);
	    return -1;
	}

    switch( cp->cpRightPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    tailGap= cp->cpRightPadding;
	    break;
	default:
	    LDEB(cp->cpRightPaddingUnit);
	    return -1;
	}

    if  ( rp->rpRToL )
	{
	if  ( drCellFrame )
	    {
	    drCellFrame->drX0= drRowParent->drX1- cellNode->biCellTailX;
	    drCellFrame->drX1= drRowParent->drX1- cellNode->biCellHeadX;
	    }
	if  ( drCellContent )
	    {
	    drCellContent->drX0= drRowParent->drX1- cellNode->biCellTailX+
									tailGap;
	    drCellContent->drX1= drRowParent->drX1- cellNode->biCellHeadX-
									headGap;
	    }
	}
    else{
	if  ( drCellFrame )
	    {
	    drCellFrame->drX0= drRowParent->drX0+ cellNode->biCellHeadX;
	    drCellFrame->drX1= drRowParent->drX0+ cellNode->biCellTailX;
	    }
	if  ( drCellContent )
	    {
	    drCellContent->drX0= drRowParent->drX0+ cellNode->biCellHeadX+
									headGap;
	    drCellContent->drX1= drRowParent->drX0+ cellNode->biCellTailX-
									tailGap;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the frame in which a table row is to be formatted.	*/
/*									*/
/*  For nested tables, we go into recursion to do the same for the	*/
/*  cell that holds the row etc. We expect table nesting to be shallow,	*/
/*  so this is not a performance problem.				*/
/*									*/
/************************************************************************/

static int docRowParentFrame(	DocumentRectangle *		drRowParent,
				const struct BufferItem *	rowNode,
				const BlockFrame *		bf )
    {
    const struct BufferItem *	parentNode= rowNode->biParent;
    const struct BufferItem *	parentRow;

    DocumentRectangle		drParentContent;

    if  ( ! parentNode )
	{ XDEB(parentNode); return -1;	}

    switch( parentNode->biLevel )
	{
	case DOClevCELL:
	    break;

	case DOClevSECT:
	    *drRowParent= bf->bfContentRect;
	    return 0;

	default:
	    SDEB(docLevelStr(parentNode->biLevel));
	    return -1;
	}

    parentRow= parentNode->biParent;
    if  ( ! parentRow || ! docIsRowNode( parentRow ) )
	{
	XDEB(parentRow);
	/* Cope with strange documents: Skipped a row nesting level? */
	*drRowParent= bf->bfContentRect;
	return 0;
	}

    if  ( docRowParentFrame( &drParentContent, parentRow, bf ) )
	{ LDEB(1); return -1;	}

    if  ( docGetCellXRange( (DocumentRectangle *)0, drRowParent,
				    parentNode, parentRow->biRowProperties,
				    &drParentContent ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the frame for a table cell.				*/
/*									*/
/*  This is used to calculate the layout.				*/
/*									*/
/************************************************************************/

void docCellFrameTwips(		ParagraphFrame *		pf,
				const BlockFrame *		bf,
				const struct BufferItem *	cellNode )
    {
    const struct BufferItem *	rowNode= cellNode->biParent;
    const RowProperties *	rp= rowNode->biRowProperties;

    DocumentRectangle		drRowParent;

    pf->pfCellRect.drY0= INT_MIN/ 3; /* /3 for predictable arithmetic */
    pf->pfCellRect.drY1= INT_MAX/ 3; /* /3 for predictable arithmetic */
    pf->pfCellRect.drX0= bf->bfContentRect.drX0;
    pf->pfCellRect.drX1= bf->bfContentRect.drX1;

    if  ( rp->rpHeightTwips < 0 )
	{
	pf->pfCellRect.drY0= rowNode->biTopPosition.lpPageYTwips;
	pf->pfCellRect.drY1= pf->pfCellRect.drY0- rp->rpHeightTwips;
	}

    pf->pfCellContentRect= pf->pfCellRect;
    pf->pfParaContentRect= pf->pfCellContentRect;

    if  ( rp->rpCellCount < 1 )
	{ LDEB(rp->rpCellCount); return;	}

    if  ( docRowParentFrame( &drRowParent, rowNode, bf ) )
	{ LDEB(1); return;	}

    if  ( docGetCellXRange( &(pf->pfCellRect), &(pf->pfCellContentRect),
						cellNode, rp, &drRowParent ) )
	{ LDEB(1);	}

    pf->pfParentContentRect= drRowParent;
    pf->pfParaContentRect= pf->pfCellContentRect;

    return;
    }

void docParagraphFrameTwips(	ParagraphFrame *		pf,
				const BlockFrame *		bf,
				const struct BufferItem *	paraNode )
    {
    const	ParagraphProperties *	pp= paraNode->biParaProperties;

    if  ( paraNode->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(paraNode->biLevel));	}

    if  ( paraNode->biParaProperties->ppTableNesting > 0 )
	{ docCellFrameTwips( pf, bf, paraNode->biParent );	}
    else{
	pf->pfCellRect.drY0= INT_MIN/ 3; /* /3 for predictable arithmetic */
	pf->pfCellRect.drY1= INT_MAX/ 3; /* /3 for predictable arithmetic */
	pf->pfCellRect.drX0= bf->bfContentRect.drX0;
	pf->pfCellRect.drX1= bf->bfContentRect.drX1;

	pf->pfCellContentRect= pf->pfCellRect;
	pf->pfParentContentRect= bf->bfContentRect;
	}

    pf->pfParaContentRect= pf->pfCellContentRect;

    /*  Indpendent of pp->ppRToL: */
    pf->pfParaContentRect.drX0 += pp->ppLeftIndentTwips;
    pf->pfParaContentRect.drX1 -= pp->ppRightIndentTwips;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle occupied by a table cell.			*/
/*									*/
/************************************************************************/

void docCellRectangleTwips(	DocumentRectangle *		drCell,
				const BlockFrame *		bf,
				const struct BufferItem *	cellNode )
    {
    ParagraphFrame		pf;
    const struct BufferItem *	rowNode= cellNode->biParent;
    const struct BufferItem *	sectNode= docGetSectNode( (struct BufferItem *)rowNode );
    const LayoutPosition *	lpTop;
    LayoutPosition		lpBelow;

    docCellFrameTwips( &pf, bf, cellNode );

    lpTop= &(rowNode->biTopPosition);
    docGetCellBottom( &lpBelow, cellNode );

    if  ( docCompareLayoutPositionToFrame( lpTop,
					bf->bfPage, bf->bfColumn ) < 0 )
	{
	if  ( sectNode->biTopPosition.lpPage == bf->bfPage )
	    {
	    pf.pfCellRect.drY0= sectNode->biTopPosition.lpPageYTwips;
	    }
	else{
	    pf.pfCellRect.drY0= bf->bfContentRect.drY0;
	    }
	}
    else{
	pf.pfCellRect.drY0= lpTop->lpPageYTwips;
	}

    if  ( docCompareLayoutPositionToFrame( &lpBelow,
					bf->bfPage, bf->bfColumn ) > 0 )
	{
	if  ( sectNode->biBelowPosition.lpPage == bf->bfPage )
	    {
	    pf.pfCellRect.drY1= sectNode->biBelowPosition.lpPageYTwips;
	    }
	else{
	    pf.pfCellRect.drY1= bf->bfContentRect.drY1;
	    }
	}
    else{
	pf.pfCellRect.drY1= lpBelow.lpPageYTwips;
	}

    *drCell= pf.pfCellRect;
    return;
    }

