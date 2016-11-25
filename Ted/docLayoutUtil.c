/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<charnames.h>

#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Default leading in terms of the size of the dominant font in a	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

#   define LINEDISTFAC	6

/************************************************************************/
/*									*/
/*  Initialize a layout job.						*/
/*									*/
/************************************************************************/

void docPsInitLayoutJob(	LayoutJob *	lj )
    {
    lj->ljChangedRectanglePixels= (DocumentRectangle *)0;
    lj->ljAdd= (AppDrawingData *)0;
    lj->ljBd= (BufferDocument *)0;
    lj->ljChangedItem= (BufferItem *)0;

    docInitLayoutPosition( &(lj->ljPosition) );

    lj->ljLayoutScreen.slStartRow= (START_ROW)0;
    lj->ljLayoutScreen.slStartParagraph= (START_SCREEN_PARAGRAPH)0;
    lj->ljLayoutScreen.slScreenFrame= (SCREEN_FRAME)0;
    lj->ljLayoutScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    lj->ljPlaceScreen.slStartRow= (START_ROW)0;
    lj->ljPlaceScreen.slStartParagraph= (START_SCREEN_PARAGRAPH)0;
    lj->ljPlaceScreen.slScreenFrame= (SCREEN_FRAME)0;
    lj->ljPlaceScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    return;
    }

void docPsCleanLayoutJob(	LayoutJob *	lj )
    {
    return;
    }

void docLayoutInitBlockFrame(	BlockFrame *	bf )
    {
    bf->bfX0Twips= 0;
    bf->bfX1Twips= 0;
    bf->bfY0Twips= 0;
    bf->bfY1Twips= 0;

    bf->bfColumnWidthTwips= 0;

    docInitNotesReservation( &(bf->bfNotesReservation) );
    }

void docInitNotesReservation(	NotesReservation *	nr )
    {
    nr->nrFirstFootnote= -1;
    nr->nrFootnoteCount= 0;

    nr->nrFtnsepHeight= 0;
    nr->nrFootnoteHeight= 0;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text on a subsequent page.			*/
/*									*/
/************************************************************************/

void docLayoutSectColumnTop(	BufferItem *		bodySectBi,
				BufferDocument *	bd,
				LayoutPosition *	lpTop,
				BlockFrame *		bf )
    {
    docBlockFrameTwips( bf, bodySectBi, bd, lpTop->lpPage, lpTop->lpColumn );

    lpTop->lpPageYTwips= bf->bfY0Twips;
    lpTop->lpAtTopOfColumn= 1;

    return;
    }

void docLayoutToNextColumn(	BufferItem *		bi,
				BufferDocument *	bd,
				LayoutPosition *	lpTop,
				BlockFrame *		bf )
    {
    while ( bi				&&
	    bi->biLevel != DOClevSECT	)
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return;	}

    lpTop->lpPage++;

    docLayoutSectColumnTop( bi, bd, lpTop, bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Add the space occupied by a border to an inset.			*/
/*									*/
/************************************************************************/

void docLayoutAddBorderToInset(	int *				pInset,
				const BorderProperties *	bp )
    {
    if  ( bp->bpStyle == DOCbsNONE )
	{ return;	}

    (*pInset) += bp->bpSpacingTwips;
    (*pInset) += bp->bpPenWideTwips;

    return;
    }

void docLayoutStretchInsetForBorder(
				int *				pInset,
				const BorderProperties *	bp )
    {
    int		inset= 0;

    inset += bp->bpSpacingTwips;
    inset += bp->bpPenWideTwips;

    if  ( (*pInset) < inset )
	{ (*pInset)=  inset;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the height of a series of lines in a paragraph.		*/
/*									*/
/************************************************************************/

static void docLayoutCalculateParaTopInset(	BufferItem *	paraBi )
    {
    int				topInset= 0;
    const BorderProperties *	bp= (const BorderProperties *)0;
    const BufferItem *		prevBi= (const BufferItem *)0;

    if  ( paraBi->biNumberInParent > 0 )
	{ prevBi= paraBi->biParent->biChildren[paraBi->biNumberInParent- 1]; }

    topInset= paraBi->biParaSpaceBeforeTwips;

    if  ( paraBi->biParaTopBorder.bpStyle != DOCbsNONE )
	{
	if  ( ! prevBi						||
	      prevBi->biParaTopBorder.bpStyle == DOCbsNONE	)
	    {
	    docLayoutAddBorderToInset( &topInset, &(paraBi->biParaTopBorder) );
	    bp= &(paraBi->biParaTopBorder);
	    }
	}
    else{
	if  ( paraBi->biParaBoxBorder.bpStyle != DOCbsNONE )
	    {
	    if  ( ! prevBi					||
		  prevBi->biParaBoxBorder.bpStyle == DOCbsNONE	)
		{
		docLayoutAddBorderToInset( &topInset,
						&(paraBi->biParaBoxBorder) );
		bp= &(paraBi->biParaBoxBorder);
		}
	    }
	}

    paraBi->biTopInsetTwips= topInset;
    paraBi->biParaBorderAboveParagraph= bp;

    return;
    }

static void docParaSpaceForBottomBorder(int *				pBelow,
					const BorderProperties *	bp,
					const BufferItem *		paraBi )
    {
    int		below= *pBelow;

    below += paraBi->biParaLeadingTwips;

    docLayoutAddBorderToInset( &below, bp );

    *pBelow= below;
    }

static void docLayoutCalculateParaBottomInset(	BufferItem *	paraBi )
    {
    int				bottomInset= 0;
    const BorderProperties *	bp= (const BorderProperties *)0;
    const BufferItem *		nextBi= (const BufferItem *)0;

    if  ( paraBi->biNumberInParent < paraBi->biParent->biChildCount- 1 )
	{ nextBi= paraBi->biParent->biChildren[paraBi->biNumberInParent+ 1]; }

    if  ( paraBi->biParaBottomBorder.bpStyle != DOCbsNONE )
	{
	if  ( ! nextBi						||
	      nextBi->biParaBottomBorder.bpStyle == DOCbsNONE	)
	    {
	    docParaSpaceForBottomBorder( &bottomInset,
				    &(paraBi->biParaBottomBorder), paraBi );
	    bp= &(paraBi->biParaBottomBorder);
	    }
	}
    else{
	if  ( paraBi->biParaBoxBorder.bpStyle != DOCbsNONE )
	    {
	    if  ( ! nextBi					||
		  nextBi->biParaBoxBorder.bpStyle == DOCbsNONE	)
		{
		docParaSpaceForBottomBorder( &bottomInset,
					&(paraBi->biParaBoxBorder), paraBi );
		bp= &(paraBi->biParaBoxBorder);
		}
	    }
	}

    bottomInset += paraBi->biParaSpaceAfterTwips;

    paraBi->biBottomInsetTwips= bottomInset;
    paraBi->biParaBorderBelowParagraph= bp;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the 'majority' font of a paragraph, and get the font	*/
/*  extents for that font.						*/
/*									*/
/*  1)  Note that subscript/superscript is NOT taken into account.	*/
/*									*/
/************************************************************************/

int docPsParagraphLineExtents(	const AppPhysicalFontList *	apfl,
				BufferItem *			bi )
    {
    const TextParticule *	tp= bi->biParaParticules;

    int				size;
    int				paraAscent= 0;
    int				paraDescent= 0;
    int				i;

    static int *		counts;
    int *			fresh;

    int				found;
    int				foundCount;

    fresh= (int *)realloc( counts, apfl->apflCount* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(apfl->apflCount,fresh); return -1;	}
    counts= fresh;

    for ( i= 0; i < apfl->apflCount; i++ )
	{ counts[i]= 0;	}

    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
	{
	if  ( tp->tpKind != DOCkindTEXT		&&
	      tp->tpKind != DOCkindTAB		&&
	      tp->tpKind != DOCkindOBJECT	)
	    { continue;	}

	if  ( tp->tpPhysicalFont < 0			||
	      tp->tpPhysicalFont >= apfl->apflCount	)
	    { LLDEB(tp->tpPhysicalFont,apfl->apflCount); continue;	}

	counts[tp->tpPhysicalFont] += tp->tpStrlen+ 1;
	}

    found= -1;
    foundCount= 0;
    for ( i= 0; i < apfl->apflCount; i++ )
	{
	if  ( counts[i] > foundCount )
	    { found= i; foundCount= counts[i];	}
	}

    if  ( found >= 0 )
	{
	AfmFontInfo *	afi;
	int		encoding;

	afi= docPsPrintGetAfi( &encoding, apfl, found );

	size= 10* apfl->apflFonts[found].apfAttribute.taFontSizeHalfPoints;

	paraAscent= ( size* afi->afiFontBBox.abbTop+ 500 ) / 1000;
	paraDescent= paraAscent- size;
	}
    else{ /* LDEB(found); */ size= 200;	}

    bi->biParaAscentTwips= paraAscent;
    bi->biParaDescentTwips= paraDescent;

    bi->biParaLeadingTwips= size/ LINEDISTFAC;

    docLayoutCalculateParaTopInset( bi );
    docLayoutCalculateParaBottomInset( bi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the correct page number in a page header/footer.		*/
/*									*/
/************************************************************************/

int docLayoutExternalItem(	ExternalItem *			ei,
				DocumentRectangle *		drChanged,
				int				page,
				int				y0Twips,
				BufferDocument *		bd,
				const BufferItem *		sectBi,
				AppDrawingData *		add,
				LAYOUT_EXTERNAL			layoutExternal,
				DOC_CLOSE_OBJECT		closeObject )
    {
    if  ( page != ei->eiPageFormattedFor )
	{
	RecalculateFields		rf;
	int				y1Twips;

	docInitRecalculateFields( &rf );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= closeObject;
	rf.rfUpdateFlags=
		FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoPAGE_NUMBER;
	rf.rfFieldsUpdated= 0;


	if  ( docRecalculateTextLevelFieldsInExternalItem( &rf, ei,
							    sectBi, page ) )
	    { LDEB(page); return -1;	}

	y1Twips= y0Twips;

	if  ( layoutExternal 					&&
	      (*layoutExternal)( &y1Twips, ei, page, y0Twips,
					bd, add, drChanged )	)
	    { LDEB(1); return -1;	}

	ei->eiPageFormattedFor= page;
	ei->eiY0UsedTwips= y0Twips;
	ei->eiY1UsedTwips= y1Twips;
	}

    return 0;
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

void docBlockFrameTwips(	BlockFrame *		bf,
				BufferItem *		bi,
				const BufferDocument *	bd,
				int			page,
				int			column )
    {
    ExternalItem *		ei;
    int				inHeaderFooter;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    BufferItem *		sectBi;

    while( bi				&&
	   bi->biLevel != DOClevSECT	)
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return;	}

    sp= &(bi->biSectProperties);
    dg= &(sp->spDocumentGeometry);

    bf->bfX0Twips= dg->dgLeftMarginTwips;
    bf->bfX1Twips= dg->dgPageWideTwips- dg->dgRightMarginTwips;
    bf->bfColumnWidthTwips= bf->bfX1Twips- bf->bfX0Twips;

    docInitNotesReservation( &(bf->bfNotesReservation) );

    switch( bi->biInExternalItem )
	{
	case DOCinBODY:

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    inHeaderFooter= docWhatPageHeader( &ei, bi, page, dp );

	    /*  1 Reserved!!  */
	    if  ( ei && ei->eiItem )
		{ bf->bfY0Twips= ei->eiY1ReservedTwips;		}
	    else{ bf->bfY0Twips= dg->dgTopMarginTwips;		}

	    inHeaderFooter= docWhatPageFooter( &ei, bi, page, dp );

	    /*  2 Reserved!!  */
	    if  ( ei && ei->eiItem )
		{ bf->bfY1Twips= ei->eiY0ReservedTwips; }
	    else{ bf->bfY1Twips= dg->dgPageHighTwips- dg->dgBottomMarginTwips; }

	    /*
	    appDebug( "PAGE %3d BLOCK Y: %5d..%5d %s\n",
			    page, bf->bfY0Twips, bf->bfY1Twips,
			    docExternalKindStr( bi->biInExternalItem ) );
	    */

	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( column != 0 )
		{ LDEB(column);	}

	    sectBi= bd->bdItem.biChildren[bi->biNumberInParent];
	    ei= docSectionHeaderFooter( sectBi, bi->biInExternalItem );
	    bf->bfY0Twips= ei->eiY0ReservedTwips;
	    bf->bfY1Twips= ei->eiY1ReservedTwips;
	    break;

	default:
	    LDEB(bi->biInExternalItem);
	}

    return;
    }

void docParagraphFrameTwips(	ParagraphFrame *	pf,
				const BlockFrame *	bf,
				int			bottom,
				int			stripHigh,
				BufferItem *		paraBi )
    {
    const NotesReservation *	nrBf= &(bf->bfNotesReservation);
    int				footnoteHeight;

    footnoteHeight= nrBf->nrFtnsepHeight+ nrBf->nrFootnoteHeight;

    pf->pfX0GeometryTwips= bf->bfX0Twips;
    pf->pfX1GeometryTwips= bf->bfX1Twips;

    pf->pfPageHigh= bf->bfY1Twips- bf->bfY0Twips;

    pf->pfStripHigh= stripHigh;
    pf->pfPageY1Twips= bf->bfY1Twips- footnoteHeight;

    if  ( bottom > 0			&&
	  bottom <= pf->pfPageY1Twips	)
	{ pf->pfPageY1Twips= bottom;	}

    if  ( paraBi->biParaInTable )
	{
	int			col0;
	int			col1;
	const BufferItem *	cellBi= paraBi->biParent;
	const BufferItem *	rowBi= cellBi->biParent;
	const RowProperties *	rp= &(rowBi->biRowProperties);

	col0= col1= cellBi->biNumberInParent;

	if  ( col1 >= rowBi->biRowCellCount )
	    { LLDEB(col1,rowBi->biRowCellCount);	}
	else{
	    const CellProperties *	cp1= rp->rpCells+ col1;

	    if  ( cp1->cpLeftInMergedRow )
		{
		while( col1 < rowBi->biRowCellCount- 1	&&
		       cp1[1].cpMergedWithLeft		)
		    { col1++; cp1++;	}
		}

	    pf->pfX1GeometryTwips= bf->bfX0Twips+ cp1->cpRightBoundaryTwips;

	    if  ( col0 == 0 )
		{ pf->pfX0GeometryTwips += rowBi->biRowLeftIndentTwips;	}
	    else{
		const CellProperties *	cpp;

		cpp= rp->rpCells+ col0- 1;
		pf->pfX0GeometryTwips += cpp->cpRightBoundaryTwips;
		}
	    }

	pf->pfX0GeometryTwips += rowBi->biRowHalfGapWidthTwips;
	pf->pfX1GeometryTwips -= rowBi->biRowHalfGapWidthTwips;
	}

    pf->pfX0TextLinesTwips= pf->pfX0GeometryTwips+
					paraBi->biParaLeftIndentTwips;
    pf->pfX1TextLinesTwips= pf->pfX1GeometryTwips-
					paraBi->biParaRightIndentTwips;

    pf->pfX0FirstLineTwips= pf->pfX0TextLinesTwips+
					paraBi->biParaFirstIndentTwips;

    if  ( paraBi->biInExternalItem == DOCinBODY		||
	  paraBi->biInExternalItem == DOCinENDNOTE	||
	  paraBi->biInExternalItem == DOCinAFTNSEP	)
	{ pf->pfHasBottom= 1;	}
    else{ pf->pfHasBottom= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Scale an inserted object to fit where we want to place it.		*/
/*									*/
/*  1)  If the object fits on the page at its current scale, nothing	*/
/*	needs to be done.						*/
/*  2)  Otherwise scale the object to fit on the page. X and Y scale	*/
/*	are the same, irrespective of the original scaling.		*/
/*									*/
/************************************************************************/

void docLayoutScaleObjectToFitParagraphFrame(	InsertedObject *	io,
						const ParagraphFrame *	pf )
    {
    int		textWideTwips= pf->pfX1TextLinesTwips- pf->pfX0TextLinesTwips;

    int		objectWideTwips= ( io->ioScaleX* io->ioTwipsWide )/ 100.0;
    int		objectHighTwips= ( io->ioScaleY* io->ioTwipsHigh )/ 100.0;

    double	scaleX;
    double	scaleY;

    /*  1  */
    if  ( objectWideTwips <= textWideTwips	&&
	  objectHighTwips <= pf->pfPageHigh	)
	{ return;	}

    /*  2  */
    scaleX= (double)textWideTwips/ (double)io->ioTwipsWide;
    scaleY= (double)pf->pfPageHigh/ (double)io->ioTwipsHigh;

    if  ( scaleY > scaleX )
	{ scaleY=  scaleX;	}

    if  ( scaleY < 0.99 )
	{
	io->ioScaleX= (int)( 99* scaleY );
	io->ioScaleY= (int)( 99* scaleY );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Push a position to one at least below another one. An example of	*/
/*  its use is pushing down the bottom of a row to the bottom of its	*/
/*  tallest cell.							*/
/*									*/
/************************************************************************/

void docLayoutPushBottomDown(		LayoutPosition *	lpParentBottom,
					const LayoutPosition *	lpChildBottom )
    {
    if  ( lpParentBottom->lpPage < lpChildBottom->lpPage )
	{
	lpParentBottom->lpPage= lpChildBottom->lpPage;
	lpParentBottom->lpPageYTwips= lpChildBottom->lpPageYTwips;
	lpParentBottom->lpAtTopOfColumn= lpChildBottom->lpAtTopOfColumn;
	}
    else{
	if  ( lpParentBottom->lpPage == lpChildBottom->lpPage )
	    {
	    if  ( lpParentBottom->lpPageYTwips < lpChildBottom->lpPageYTwips )
		{
		lpParentBottom->lpPageYTwips= lpChildBottom->lpPageYTwips;
		lpParentBottom->lpAtTopOfColumn= lpChildBottom->lpAtTopOfColumn;
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a stretch of text to upper case for capitals or smallcaps	*/
/*  drawing.								*/
/*									*/
/*  3)  Worst case alternating, starting with lower case. 1+ len	*/
/*	segments. But when it is just one lower case letter we need 3!	*/
/*									*/
/************************************************************************/

int docMakeCapsString(		unsigned char **	pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const BufferDocument *	bd,
				const TextAttribute *	ta,
				const unsigned char *	printString,
				int			len )
    {
    int				rval= 0;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);
    const DocumentFont *	df= dfl->dflFonts+ ta->taFontNumber;

    int				segmentCount= 0;
    unsigned char *		upperString= (unsigned char *)0;
    int *			segments= (int *)0;

    if  ( df->dfEncodingUsed >= 0		&&
	  df->dfEncodingUsed < ENCODINGps_COUNT	)
	{
	const unsigned char *	charKinds;
	const unsigned char *	charShifts;
	int			n;
	const unsigned char *	from;
	unsigned char *		to;

	charKinds= PS_Encodings[df->dfEncodingUsed].fcCharKinds;
	charShifts= PS_Encodings[df->dfEncodingUsed].fcCharShifts;

	upperString= malloc( len+ 1 );
	if  ( ! upperString )
	    { XDEB(upperString); rval= -1; goto ready;	}

	if  ( ta->taSmallCaps && ! ta->taCapitals )
	    {
	    /*  3  */
	    segments= malloc( (len+ 2)* sizeof(int) );
	    if  ( ! segments )
		{ XDEB(segments); rval= -1; goto ready;	}
	    }

	to= upperString;
	from= printString;
	n= 0;
	while( n < len )
	    {
	    int		n0= 0;
	    int		n1= 0;

	    while( n < len && ! ( charKinds[*from] & CHARisLOWER ) )
		{
		*to= *from;
		n0++; from++; to++; n++;
		}

	    while( n < len &&   ( charKinds[*from] & CHARisLOWER ) )
		{
		*to= charShifts[*from];
		n1++; from++; to++; n++;
		}

	    if  ( segments )
		{
		segments[2*segmentCount+ 0]= n0;
		segments[2*segmentCount+ 1]= n1;
		segmentCount++;
		}
	    }

	*to= '\0';
	}

    if  ( segmentCount > len+ 2 )
	{ LLDEB(len,segmentCount);	}

    *pUpperString= upperString; upperString= (unsigned char *)0;
    *pSegments= segments; segments= (int *)0;
    *pSegmentCount= segmentCount;

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }
