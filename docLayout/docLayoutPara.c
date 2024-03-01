/************************************************************************/
/*									*/
/*  Paragraph Layout related administration.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<docPageGrid.h>
#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	"docLayoutObject.h"
#   include	"docParagraphLayout.h"
#   include	"docStripLayoutJob.h"
#   include	"docRowLayout.h"
#   include	"layoutContext.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docPropertiesAdmin.h>
#   include	<textAttribute.h>
#   include	<docBorderProperties.h>
#   include	<docRowProperties.h>
#   include	<docParaProperties.h>
#   include	<docPropVal.h>
#   include	<docBreakKind.h>
#   include	<docCellProperties.h>
#   include	<docBuf.h>
#   include	<docStripFrame.h>
#   include	<docBlockFrame.h>
#   include	<docObjects.h>
#   include	<docAttributes.h>
#   include	<psTextExtents.h>
#   include	<docObject.h>
#   include	<docObjectProperties.h>
#   include	"docLayoutDocumentTree.h"
#   include	<docBlockOrnaments.h>
#   include	<docNodeTree.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static int docGetParaTopBorder(	const BorderProperties **	pBpTop,
				int *				pNrAbove,
				int *				pFillBefore,
				const struct BufferDocument *	bd,
				const struct BufferItem * const	paraNode,
				const struct BufferItem *	cellNode )
    {
    int				rval= 0;
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	prevPp= (const ParagraphProperties *)0;
    const BorderProperties *	bpTop= (const BorderProperties *)0;

    if  ( paraNode->biNumberInParent > 0 )
	{
	const struct BufferItem *	prevNode;

	prevNode= cellNode->biChildren[paraNode->biNumberInParent- 1];
	if  ( prevNode->biLevel == DOClevPARA )
	    { prevPp= prevNode->biParaProperties;	}
	}

    /*  5  */
    if  ( docBorderNumberIsBorder( bd, pp->ppTopBorderNumber ) )
	{
	if  ( ! prevPp							||
	      prevPp->ppFrameNumber != pp->ppFrameNumber 		||
	      prevPp->ppTopBorderNumber != pp->ppTopBorderNumber	)
	    {
	    bpTop= docGetBorderPropertiesByNumber( bd,
					    pp->ppTopBorderNumber );

	    *pNrAbove= pp->ppTopBorderNumber;
	    rval= 1;
	    }

	if  ( prevPp							&&
	      prevPp->ppFrameNumber == pp->ppFrameNumber		&&
	      prevPp->ppBottomBorderNumber == pp->ppBottomBorderNumber	)
	    { *pFillBefore= 1;	}
	}

    if  ( prevPp							&&
	  docShadingNumberIsShading( bd, pp->ppShadingNumber )		&&
	  docShadingNumberIsShading( bd, prevPp->ppShadingNumber )	)
	{ *pFillBefore= 1;	}

    *pBpTop= bpTop;
    return rval;
    }

static int docGetParaBottomBorder( const BorderProperties **	pBpBottom,
				int *				pNrBelow,
				int *				pFillAfter,
				const struct BufferDocument *	bd,
				const struct BufferItem * const	paraNode,
				const struct BufferItem *	cellNode )
    {
    int				rval= 0;
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	nextPp= (const ParagraphProperties *)0;
    const BorderProperties *	bpBottom= (const BorderProperties *)0;

    if  ( paraNode->biNumberInParent < cellNode->biChildCount- 1 )
	{
	const struct BufferItem *	nextNode;

	nextNode= cellNode->biChildren[paraNode->biNumberInParent+ 1];
	if  ( nextNode->biLevel == DOClevPARA )
	    { nextPp= nextNode->biParaProperties;	}
	}

    if  ( docBorderNumberIsBorder( bd, pp->ppBottomBorderNumber ) )
	{
	if  ( ! nextPp							||
	      nextPp->ppFrameNumber != pp->ppFrameNumber 		||
	      nextPp->ppBottomBorderNumber != pp->ppBottomBorderNumber	)
	    {
	    bpBottom= docGetBorderPropertiesByNumber( bd,
					pp->ppBottomBorderNumber );

	    *pNrBelow= pp->ppBottomBorderNumber;
	    rval= 1;
	    }

	if  ( nextPp							&&
	      nextPp->ppFrameNumber == pp->ppFrameNumber		&&
	      nextPp->ppBottomBorderNumber == pp->ppBottomBorderNumber	)
	    { *pFillAfter= 1;	}
	}

    if  ( nextPp							&&
	  docShadingNumberIsShading( bd, pp->ppShadingNumber )		&&
	  docShadingNumberIsShading( bd, nextPp->ppShadingNumber )	)
	{ *pFillAfter= 1;	}

    *pBpBottom= bpBottom;
    return rval;
    }

/************************************************************************/
/*									*/
/*  Calculate the height of a series of lines in a paragraph.		*/
/*									*/
/*  1)  The first paragraph in a cell must accomodate space for the	*/
/*	top border of the cells of the row and the cell padding.	*/
/*  2)  Allocate space for the fattest top border in the row.		*/
/*  3)  Allocate space for the row padding.				*/
/*  4)  Allocate space for the cell padding.				*/
/*  5)  Allocate space for the paragraph top border.			*/
/*									*/
/************************************************************************/

void docLayoutCalculateParaTopInset(
				const struct BufferDocument *	bd,
				struct BufferItem *		paraNode )
    {
    int				topInset= 0;
    int				nrAbove= -1;
    const BorderProperties *	bpTop= (const BorderProperties *)0;
    const struct BufferItem * const	cellNode= paraNode->biParent;
    const struct BufferItem * const	rowNode= cellNode->biParent;
    const RowProperties *	rp= rowNode->biRowProperties;

    int				cellMargin= 0;
    int				fillBefore= 0;

    topInset= paraNode->biParaProperties->ppSpaceBeforeTwips;

    /*  1  */
    if  ( paraNode->biParaProperties->ppTableNesting > 0	&&
	  paraNode->biNumberInParent == 0			)
	{
	const CellProperties *	cp= rp->rpCells+ cellNode->biNumberInParent;

	if  ( ! docIsRowNode( rowNode ) )
	    { LDEB(1); cp= (const CellProperties *)0;	}

	/*  2  */
	cellMargin += rowNode->biRowTopInset;

	/*  3  */
	switch( rp->rpTopCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpTopCellPadding;
		break;

	    default:
		LDEB(rp->rpTopCellPaddingUnit);
		break;
	    }

	/*  4  */
	if  ( cp ) switch( cp->cpTopPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpTopPadding;
		break;

	    default:
		LDEB(cp->cpTopPaddingUnit);
		break;
	    }
	}

    if  ( paraNode->biParaProperties->ppTableNesting == 0	&&
	  paraNode->biNumberInParent == 0			&&
	  cellNode->biNumberInParent == 0			)
	{ topInset += rowNode->biRowTopInset;	}

    /*  5  */
    if  ( docGetParaTopBorder( &bpTop, &nrAbove, &fillBefore,
						    bd, paraNode, cellNode ) )
	{ docAddBorderToInset( &topInset, bpTop ); 		}

    paraNode->biParaTopInset= cellMargin+ topInset;

    return;
    }

void docLayoutCalculateParaBottomInset(
				const struct BufferDocument *	bd,
				struct BufferItem *		paraNode )
    {
    int				bottomInset= 0;
    int				nrBelow= -1;
    const BorderProperties *	bpBottom= (const BorderProperties *)0;
    const struct BufferItem *	cellNode= paraNode->biParent;
    const struct BufferItem *	rowNode= cellNode->biParent;
    const RowProperties *	rp= rowNode->biRowProperties;

    int				cellMargin= 0;
    int				fillAfter= 0;

    if  ( paraNode->biParaProperties->ppTableNesting > 0		&&
	  cellNode->biNumberInParent < rp->rpCellCount			&&
	  paraNode->biNumberInParent == cellNode->biChildCount- 1	)
	{
	const CellProperties *	cp= rp->rpCells+ cellNode->biNumberInParent;

	switch( rp->rpBottomCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpBottomCellPadding;
		break;

	    default:
		LDEB(rp->rpBottomCellPaddingUnit);
		break;
	    }

	switch( cp->cpBottomPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpBottomPadding;
		break;

	    default:
		LDEB(cp->cpBottomPaddingUnit);
		break;
	    }
	}

    if  ( docGetParaBottomBorder( &bpBottom, &nrBelow, &fillAfter,
						    bd, paraNode, cellNode ) )
	{ docAddBorderToInset( &bottomInset, bpBottom ); 		}

    bottomInset += paraNode->biParaProperties->ppSpaceAfterTwips;

    paraNode->biParaBottomInset= bottomInset+ cellMargin;

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

int docLayoutParagraphLineExtents(
				int *				pFontSizeTwips,
				const LayoutContext *		lc,
				struct BufferItem *		paraNode )
    {
    struct BufferDocument *		bd= lc->lcDocument;
    const TextParticule *	tp= paraNode->biParaParticules;

    int				sizeHalfPoints;
    int				y0= 0;
    int				y1= 0;

    int				box;
    int				part;

    static int *		counts;
    int *			fresh;

    int				found;
    int				foundCount;

    DocumentPropertyLists *	dpl= bd->bdPropertyLists;
    const NumberedPropertiesList * tal= &(dpl->dplTextAttributeList);
    int				count= tal->nplPagedList.plItemCount;

    fresh= (int *)realloc( counts, count* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}
    counts= fresh;

    for ( box= 0; box < count; box++ )
	{ counts[box]= 0;	}

    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != TPkindSPAN		&&
	      tp->tpKind != TPkindTAB		&&
	      tp->tpKind != TPkindOBJECT	)
	    { continue;	}

	if  ( tp->tpTextAttrNr < 0			||
	      tp->tpTextAttrNr >= count	)
	    {
	    LLLDEB(part,tp->tpTextAttrNr,count);
	    docListNode(bd,0,paraNode,0);
	    continue;
	    }

	counts[tp->tpTextAttrNr] += tp->tpStrlen+ 1;
	}

    found= -1;
    foundCount= 0;
    for ( box= 0; box < count; box++ )
	{
	if  ( counts[box] > foundCount )
	    { found= box; foundCount= counts[box];	}
	}

    if  ( found >= 0 )
	{
	const TextAttribute *		ta;
	const struct AfmFontInfo *	afi;

	const int			vswap= 1;
	DocumentRectangle		drFontBBox;
	DocumentRectangle		drFontAscDesc;
	int				fontHigh;
	int				fontSizeTwips;

	ta= docGetTextAttributeByNumber( bd, found );

	afi= docDocLayoutGetFontInfo( lc, ta );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	sizeHalfPoints= ta->taFontSizeHalfPoints;
	fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

	psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwips, vswap, afi );

	y0= drFontAscDesc.drY0;
	y1= drFontAscDesc.drY1;

	/*LINEDISTANCE: scale the position of the baseline based on the bbox */
	fontHigh= drFontBBox.drY1- drFontBBox.drY0;
	if  ( fontHigh < 2 )
	    { LLDEB(ta->taFontSizeHalfPoints,fontHigh); fontHigh= 2;	}
	y0= ( drFontBBox.drY0* fontSizeTwips )/ fontHigh;
	y1= ( drFontBBox.drY1* fontSizeTwips )/ fontHigh;
	}
    else{
	/* LDEB(found); */
	sizeHalfPoints= 24;
	y0= -190;
	y1=   50;
	}

    paraNode->biParaMajorityFontSize= sizeHalfPoints;
    paraNode->biParaMajorityFontAscY0= y0;
    paraNode->biParaMajorityFontDescY1= y1;

    *pFontSizeTwips= 10* sizeHalfPoints;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Invalidate the layout of all paragraphs in the modified range.	*/
/*									*/
/************************************************************************/

void docInvalidateParagraphLayout(	struct BufferItem *	paraNode )
    {
    paraNode->biParaLineCount= 0;

    paraNode->biParaMajorityFontAscY0= 0;
    paraNode->biParaMajorityFontDescY1= 0;
    paraNode->biParaMajorityFontSize= 0;
    }

void docInvalidateNodeLayout(		struct BufferItem *	node )
    {
    if  ( node->biLevel == DOClevPARA )
	{ docInvalidateParagraphLayout( node );	}
    else{
	int c;

	for ( c= 0; c < node->biChildCount; c++ )
	    { docInvalidateNodeLayout( node->biChildren[c] );	}
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the top inset after a table.				*/
/*									*/
/*  This routine is also used to calculate the inset that is used to	*/
/*  accomodate space for the bottom border of the previous row. This	*/
/*  inset is not used in real rows, but only in the immediate successor	*/
/*  of a real row.							*/
/*									*/
/*  1)  Reserve space for the bottom border of all cells.		*/
/*									*/
/************************************************************************/

void docLayoutCalculateAfterRowTopInset(
				    struct BufferItem *		belowNode,
				    const struct BufferDocument *	bd )
    {
    int				col;
    const struct BufferItem *		rowNode;
    const RowProperties *	rp;
    const CellProperties *	cp;

    if  ( belowNode->biNumberInParent == 0 )
	{ return;	}

    rowNode= belowNode->biParent->biChildren[belowNode->biNumberInParent- 1];
    if  ( ! docIsRowNode( rowNode ) )
	{ return;	}
    rp= rowNode->biRowProperties;

    belowNode->biRowTopInset= 0;

    /*  1  */
    cp= rp->rpCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	const int			atRowBottom= 1;
	int				useBelow= 0;
	const BorderProperties *	bpBottom;
	int				bottomNr;

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	bpBottom= docGetCellBottomBorder( &bottomNr, &useBelow, bd,
					    rowNode, col, atRowBottom );

	{
	int rti= belowNode->biRowTopInset;
	docStretchInsetForBorder( &rti, bpBottom );
	belowNode->biRowTopInset= rti;
	}

	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine paragraph border and shading.				*/
/*									*/
/*  Experimentation with MS-Word revealed that MS-Word includes the	*/
/*  space before/after in the borders and shading between two		*/
/*  paragraphs if either:						*/
/*  a)  Both have the same border.					*/
/*  b)  Both have a shading.						*/
/*									*/
/*  1)	To avoid a trellis effect, we must fill the space above the,	*/
/*	paragraph if the paragraph is part of a series of contiguous	*/
/*	shaded paragraphs without a border between them.		*/
/*  2)	To avoid a trellis effect, we must fill the space below the,	*/
/*	paragraph if the paragraph is part of a series of contiguous	*/
/*	shaded paragraphs without a border between them.		*/
/*									*/
/************************************************************************/

void docGetParaOrnaments(
			BlockOrnaments *		ornaments,
			DocumentRectangle *		drOutside,
			DocumentRectangle *		drInside,
			const DocumentRectangle *	drParaIn,
			const struct BufferDocument *	bd,
			const struct BufferItem * const	paraNode,
			int				atParaTop,
			int				atParaBottom )
    {
    int				thick;
    int				space;

    const ParagraphProperties *	pp= paraNode->biParaProperties;
    int				nrAbove= -1;
    const BorderProperties *	bpTop= (const BorderProperties *)0;
    int				nrBelow= -1;
    const BorderProperties *	bpBottom= (const BorderProperties *)0;

    DocumentRectangle		drPara= *drParaIn;

    if  ( atParaTop )
	{
	int		fillBefore= 0;

	docGetParaTopBorder( &bpTop, &nrAbove, &fillBefore,
					    bd, paraNode, paraNode->biParent );

	/*  1  */
	if  ( fillBefore )
	    { drPara.drY0 -= pp->ppSpaceBeforeTwips;	}
	}

    if  ( atParaBottom )
	{
	int		fillAfter= 0;

	docGetParaBottomBorder( &bpBottom, &nrBelow, &fillAfter,
					    bd, paraNode, paraNode->biParent );

	/*  2  */
	if  ( fillAfter )
	    { drPara.drY1 += pp->ppSpaceAfterTwips;	}
	}

    *drOutside= drPara;
    *drInside= drPara;

    if  ( pp->ppShadingNumber != 0 )
	{
	ornaments->boShading= docGetItemShadingByNumber( bd,
						pp->ppShadingNumber );

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawSHADE );
	}

    ornaments->boLeftBorder= docGetBorderPropertiesByNumber( bd,
						    pp->ppLeftBorderNumber );
    ornaments->boRightBorder= docGetBorderPropertiesByNumber( bd,
						    pp->ppRightBorderNumber );

    if  ( atParaTop && nrAbove >= 0 )
	{
	ornaments->boTopBorder= bpTop;
	ornaments->boTopBorderNumber= nrAbove;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawTOP_BORDER );

	thick= docBorderThick( &space, ornaments->boTopBorder );
	/* No! The paragraph above covers the lower one like the 
	 * tiles on a roof.
	drOutside->drY0= drPara->drY0- space- thick;
	drInside->drY0= drPara->drY0- space;
	*/
	drOutside->drY0= drPara.drY0+ space;
	drInside->drY0= drPara.drY0+ space+ thick;
	}

    if  ( atParaBottom && nrBelow >= 0 )
	{
	ornaments->boBottomBorder= bpBottom;
	ornaments->boBottomBorderNumber= nrBelow;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawBOTTOM_BORDER );

	thick= docBorderThick( &space, ornaments->boBottomBorder );
	drInside->drY1= drPara.drY1+ space;
	drOutside->drY1= drPara.drY1+ space+ thick;
	}

    if  ( pp->ppLeftBorderNumber != 0 )
	{
	ornaments->boLeftBorder= docGetBorderPropertiesByNumber( bd,
					pp->ppLeftBorderNumber );
	ornaments->boLeftBorderNumber= pp->ppLeftBorderNumber;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawLEFT_BORDER );

	thick= docBorderThick( &space, ornaments->boLeftBorder );
	drInside->drX0= drPara.drX0- space;
	drOutside->drX0= drPara.drX0- space- thick;
	}

    if  ( pp->ppRightBorderNumber != 0 )
	{
	ornaments->boRightBorder= docGetBorderPropertiesByNumber( bd,
					pp->ppRightBorderNumber );
	ornaments->boRightBorderNumber= pp->ppRightBorderNumber;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawRIGHT_BORDER );

	thick= docBorderThick( &space, ornaments->boRightBorder );
	drInside->drX1= drPara.drX1+ space;
	drOutside->drX1= drPara.drX1+ space+ thick;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialisation for paragraph layout.				*/
/*									*/
/*  1)  Scale objects if necessary. If a scale was chenged, cause the	*/
/*	lines in the paragraph to be reformatted. There is no need to	*/
/*	completely invalidate the layout: Font related information is	*/
/*	unaffected.							*/
/*  2)  Calculate global line extent properties if necessary.		*/
/*									*/
/************************************************************************/

static int docStartParagraphLayout(
				const ParagraphFrame *		pf,
				const BlockFrame *		bf,
				struct BufferItem *		paraNode,
				const struct BufferItem *	bodySectNode,
				const LayoutContext *		lc )
    {
    int				part;
    const TextParticule *	tp;
    int				fontSize= 0;
    int				pageHigh;

    pageHigh= geoContentHigh( &(bf->bfPageGeometry) );

    /*  1  */
    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	struct InsertedObject *	io;
	int			fixed= 0;
	int			changed= 0;

	if  ( tp->tpKind != TPkindOBJECT )
	    { continue;	}

	io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
	if  ( ! io )
	    { LPDEB(tp->tpObjectNumber,io); continue;	}

	if  ( io->ioKind == DOCokDRAWING_SHAPE )
	    {
	    if  ( docShapePrelayout( io, bodySectNode, lc ) )
		{ LDEB(io->ioKind);	}
	    }
	else{
	    if  ( docCheckObjectLayout( &fixed, io ) )
		{ LDEB(part); continue;	}

	    docLayoutScaleObjectToFitParagraphFrame( &changed,
				    io, pageHigh, &(pf->pfParaContentRect) );
	    }

	if  ( fixed || changed )
	    {
	    docCloseParticuleObject( lc->lcDocument, tp );
	    docInvalidateParagraphLayout( paraNode );
	    }
	}

    /*  2  */
    if  ( paraNode->biParaMajorityFontSize == 0				&&
	  docLayoutParagraphLineExtents( &fontSize, lc, paraNode )	)
	{ LDEB(1); return -1;	}

    docLayoutCalculateParaTopInset( lc->lcDocument, paraNode );
    docLayoutCalculateParaBottomInset( lc->lcDocument, paraNode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize the formatting of a paragraph by determining its frame	*/
/*									*/
/************************************************************************/

int docLayoutStartParagraph(	const LayoutJob *		lj,
				int *				pStopCode,
				struct BufferItem *		paraNode,
				const BlockFrame *		bf,
				ParagraphLayoutPosition *	plp )
    {
    const LayoutContext *	lc= lj->ljContext;
    int				paraBreakKind;

#   if 0
     /*
      * MS-Word does not do this
      */
    if  ( paraNode->biParaListOverride > 0				&&
	  docAdaptParagraphToListLevel( &indentChanged, paraNode, bd )	)
	{ LDEB(1);		}
#   endif

    /*  1  */
    paraBreakKind= paraNode->biParaProperties->ppBreakKind;

    /*AVOID_PAGE_BREAKS*/
    if  ( paraNode->biTreeType == DOCinBODY			&&
	  paraNode->biParaProperties->ppTableNesting == 0	&&
	  paraBreakKind != DOCibkNONE				&&
	  ! plp->plpPos.lpAtTopOfColumn				)
	{
	switch( paraBreakKind )
	    {
	    case DOCibkCOL:
		*pStopCode= FORMATstopCOLUMN_BREAK;
		break;
	    case DOCibkPAGE:
		*pStopCode= FORMATstopPAGE_BREAK;
		break;

	    default:
		LDEB(paraBreakKind); return -1;
	    }

	return 0;
	}

    docParagraphFrameTwips( &(plp->plpParagraphFrame), bf, paraNode );

    if  ( docStartParagraphLayout( &(plp->plpParagraphFrame),
				    bf, paraNode, lj->ljBodySectNode, lc ) )
	{ LDEB(1); return -1;	}

    if  ( lj->ljStartScreenParagraph					&&
	  (*lj->ljStartScreenParagraph)( paraNode,
				    &(plp->plpParagraphFrame), lc )	)
	{ LDEB(1); return -1;	}

    *pStopCode= FORMATstopREADY;
    return 0;
    }

