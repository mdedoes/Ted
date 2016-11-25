/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialisation for paragraph layout.				*/
/*									*/
/*  1)  Optimalisation: If the previous particule has the same text	*/
/*	attributes, it MUST have the same physical font.		*/
/*  2)  If no physical font has been determined for this particule,	*/
/*	open its physical font.						*/
/*  2a) Translate attribute number to the actual attribute values.	*/
/*  2b) Get the physical font for those attributes.			*/
/*									*/
/************************************************************************/

static int docLayoutOpenParaFonts(	const BufferDocument *		bd,
					BufferItem *			paraBi,
					AppDrawingData *		add )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    int				part;
    TextParticule *		tp;

				/*  1  */
    int				curTextAttributeNumber= -1;
    int				curPhysicalFont= -1;

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	/*  1  */
	if  ( tp->tpPhysicalFont < 0					&&
	      curTextAttributeNumber >= 0				&&
	      tp->tpTextAttributeNumber == curTextAttributeNumber	)
	    { tp->tpPhysicalFont= curPhysicalFont;	}

	/*  2  */
	if  ( tp->tpPhysicalFont < 0 )
	    {
	    TextAttribute	ta;

	    /*  2a  */
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	    /*  2b  */
	    tp->tpPhysicalFont= appOpenDocumentFont( add, dfl, ta );

	    if  ( tp->tpPhysicalFont < 0 )
		{
		APP_DEB(appDebug( "\"%.*s\"\n",
		    (int)tp->tpStrlen, paraBi->biParaString+ tp->tpStroff ));
		LLDEB(part,tp->tpPhysicalFont);
		return -1;
		}
	    }

	/*  1  */
	if  ( tp->tpPhysicalFont >= 0 )
	    {
	    curTextAttributeNumber= tp->tpTextAttributeNumber;
	    curPhysicalFont= tp->tpPhysicalFont;
	    }
	}

    return 0;
    }

static int docLayoutParaInit(	const BufferDocument *		bd,
				BufferItem *			paraBi,
				AppDrawingData *		add )
    {
    if  ( docLayoutOpenParaFonts( bd, paraBi, add ) )
	{ LDEB(1); return -1;	}

    if  ( docPsParagraphLineExtents( &(add->addPhysicalFontList), paraBi ) )
	{ LDEB(1); return -1;	}

    paraBi->biParaLineCount= 0;

    return 0;
    }

static int docPlaceParaInit(	const BufferDocument *		bd,
				BufferItem *			paraBi,
				AppDrawingData *		add )
    {
    return 0;
    }

static int docLayoutParaItem(	BufferItem *			paraBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc )
    {
    ParagraphLayoutJob		plj;

    docPsBeginParagraphLayoutProgress( &plj,
			paraBi->biNumberInParent, 0, 0,
			paraBi->biNumberInParent+ 1,
			&(lj->ljPosition) );

    if  ( docLayoutParagraphs( plc, paraBi->biParent, bf, lj, &plj ) )
	{ LDEB(1); return -1;	}

    lj->ljPosition= plj.pljPos.plpPos;

    if  ( paraBi->biParaLineCount < 1 )
	{ LDEB(paraBi->biParaLineCount); docListItem(0,paraBi); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the bottom of an item to changes inside.			*/
/*									*/
/************************************************************************/

static void docPsPlaceAdjustBottom(	int *			pChanged,
					BufferItem *		bi,
					const LayoutPosition *	lp,
					AppDrawingData *	add,
					DocumentRectangle *	drChanged )
    {
    int			changed= 0;

    int			oldY1Pixels;
    int			newY1Pixels;

    oldY1Pixels= BI_BELOW_PIXELS( add, bi );

    if  ( bi->biBelowPosition.lpPage != lp->lpPage		||
	  bi->biBelowPosition.lpPageYTwips != lp->lpPageYTwips	)
	{ bi->biBelowPosition= *lp; changed= 1; }

    newY1Pixels= LP_YPIXELS( add, lp );

    if  ( oldY1Pixels < newY1Pixels )
	{
	if  ( drChanged					&&
	      drChanged->drY1 < newY1Pixels -1	)
	    { drChanged->drY1=  newY1Pixels -1;	}
	}

    if  ( oldY1Pixels > newY1Pixels )
	{
	if  ( drChanged							&&
	      drChanged->drY1 < oldY1Pixels- 1	)
	    { drChanged->drY1=  oldY1Pixels- 1;	}
	}

    if  ( changed )
	{ *pChanged= changed;	}

    return;
    }

static void docPsLayoutAdjustBottom(	int *			pChanged,
					BufferItem *		bi,
					const LayoutPosition *	lp,
					AppDrawingData *	add,
					DocumentRectangle *	drChanged )
    {
    docPsPlaceAdjustBottom( pChanged, bi, lp, add, drChanged );

    *pChanged= 1;
    }

/************************************************************************/
/*									*/
/*  Place successive items, after the predecessor.			*/
/*									*/
/************************************************************************/

static int docLayoutPlaceChildren(
				BufferItem *			parentBi,
				int				from,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc )
    {
    int				i;
    int				changed;
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;

    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);

    for ( i= from; i < parentBi->biChildCount; i++ )
	{
	BufferItem *	child= parentBi->biChildren[i];

	child->biTopPosition= lj->ljPosition;

	switch( child->biLevel )
	    {
	    case DOClevSECT:
		docLayoutPlaceSectTop( child, bf, lj );

		if  ( docLayoutPlaceChildren( child, 0, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		if  ( child->biInExternalItem == DOCinBODY		&&
		      npEndnotes->npPosition == DPftnPOS_SECT_END	)
		    {
		    if  ( docLayoutEndnotesForSection(
					    child->biNumberInParent, bf, lj ) )
			{ LDEB(child->biNumberInParent); return -1;	}
		    }

		break;

	    case DOClevDOC:
	    rowAsGroup:
		if  ( docLayoutPlaceChildren( child, 0, bf, lj, plc ) )
		    { LDEB(1); return -1;	}
		break;

	    case DOClevCELL:
		{
		ParagraphLayoutJob	plj;

		docInitParagraphLayoutJob( &plj );

		docPsBeginParagraphLayoutProgress( &plj,
						0, 0, 0, child->biChildCount,
						&(lj->ljPosition) );

		if  ( docLayoutParagraphs( plc, child, bf, lj, &plj ) )
		    { LDEB(1); return -1;	}

		lj->ljPosition= plj.pljPos.plpPos;
		}
		break;

	    case DOClevROW:
		if  ( ! child->biRowHasTableParagraphs )
		    { goto rowAsGroup;	}

		if  ( docLayoutRowItem( child, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOClevPARA:
		if  ( child->biParaLineCount == 0 )
		    { break;	}

		if  ( docLayoutParaItem( child, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		break;

	    default:
		LDEB(parentBi->biLevel); return -1;
	    }

	docPsPlaceAdjustBottom( &changed, child,
				    &(lj->ljPosition), lj->ljAdd, drChanged );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a section because one of its external items	*/
/*  changed size.							*/
/*									*/
/************************************************************************/

static int docRedoBodyLayout(		BufferItem *		bodyBi,
					const LayoutJob *	ljRef )
    {
    LayoutJob			bodyLj;

    if  ( bodyBi->biInExternalItem != DOCinBODY )
	{ LDEB(bodyBi->biInExternalItem);	}

    bodyLj= *ljRef;
    bodyLj.ljAdd= ljRef->ljAdd;
    bodyLj.ljBd= ljRef->ljBd;
    bodyLj.ljChangedItem= bodyBi;
    bodyLj.ljPosition= bodyBi->biTopPosition;

    if  ( docLayoutItemAndParents( bodyBi, &bodyLj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRedoBodyItemLayout(	BufferItem *		bi,
					LayoutJob *		lj )

    {
    BufferDocument *		bd= lj->ljBd;

    ExternalItem *		ei;
    BufferItem *		bodyBi;

    if  ( lj->ljChangedItem->biInExternalItem != bi->biInExternalItem )
	{
	LDEB(lj->ljChangedItem->biInExternalItem);
	LDEB(bi->biInExternalItem);
	return 0;
	}

    if  ( docGetExternalItem( &ei, &bodyBi, bd, bi ) )
	{ LDEB(bi->biInExternalItem); return -1; }
    if  ( ! bodyBi )
	{ bodyBi= &(bd->bdItem);	}

    if  ( lj->ljPosition.lpPageYTwips == ei->eiY1UsedTwips )
	{ return 0;	}

    if  ( docRedoBodyLayout( bodyBi, lj ) )
	{ LDEB(bi->biInExternalItem); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the document rectangle to the changed layout of the document	*/
/*									*/
/************************************************************************/

static void docLayoutAdjustDocumentRectangle( LayoutJob *		ljRef )
    {
    AppDrawingData *		add= ljRef->ljAdd;
    DocumentRectangle *		drChanged= ljRef->ljChangedRectanglePixels;

    int				extendRectangle;
    int				y1;

    y1= ljRef->ljPosition.lpPage* add->addPageStepPixels+
			add->addPaperRect.drY1- add->addPaperRect.drY0;

    extendRectangle= add->addBackRect.drY1 != y1;

    if  ( drChanged					&&
	  extendRectangle				&&
	  drChanged->drY1 < add->addBackRect.drY1 )
	{ drChanged->drY1=  add->addBackRect.drY1; }

    add->addBackRect.drY1= y1;
    add->addDocRect.drY1= y1- add->addBottomMarginPixels;

    if  ( drChanged					&&
	  extendRectangle				&&
	  drChanged->drY1 < add->addBackRect.drY1 )
	{ drChanged->drY1=  add->addBackRect.drY1; }

    return;
    }

/************************************************************************/
/*									*/
/*  Fixup routine that is called when the first child item does not	*/
/*  have the same top position as its parent.				*/
/*									*/
/*  This can happen when during a reformat of part of the document	*/
/*  the child moves to a different page. E.G: When a property change	*/
/*  of a paragraph makes it higher or lower.				*/
/*									*/
/************************************************************************/

static int docPsFixupParentGeometry(	BufferItem *		bi,
					BufferItem *		biParent )
    {
    LayoutPosition	lpBi= bi->biTopPosition;
    LayoutPosition	lpPa= biParent->biTopPosition;

    lpBi.lpPageYTwips -= biParent->biTopInsetTwips;

    if  ( lpPa.lpPage != lpBi.lpPage			||
	  lpPa.lpPageYTwips != lpBi.lpPageYTwips	)
	{
#	if 0
	if  ( bi->biLevel ==DOClevPARA)
	    { SDEB((char *)bi->biParaString);	}

	SSDEB(docLevelStr(biParent->biLevel),docLevelStr(bi->biLevel));
	LLDEB(lpPa.lpPage,lpBi.lpPage);
	LLDEB(lpPa.lpPageYTwips,lpBi.lpPageYTwips);
#	endif

	biParent->biTopPosition= lpBi;

	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the geometry of a parent item to changes in a child.		*/
/*									*/
/*  This actually is a full layout action of everything below the	*/
/*  actually reformatted part. There are two differences:		*/
/*  a)  Reformatting stops when a buffer item lands in the same		*/
/*	position where it was before the change that forces us to redo	*/
/*	part of the layout.						*/
/*  b)  We try not to recalculate the layout of the text inside the	*/
/*	individual text lines.						*/
/*									*/
/************************************************************************/

static int docAdjustParentGeometry(	BufferItem *		bi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);

    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    BufferItem *		parentBi;
    int				from;

    ParagraphLayoutContext	plc;

    plc.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plc.plcAdd= lj->ljAdd;
    plc.plcBd= lj->ljBd;

    plc.plcScreenLayout= lj->ljPlaceScreen;
    plc.plcStartParagraph= docPlaceParaInit;
    plc.plcAdjustBottom= docPsPlaceAdjustBottom;

    parentBi= bi->biParent;

    if  ( bi->biLevel == DOClevPARA )
	{
	ParagraphLayoutJob	plj;
	int			advanced= 0;

	docInitParagraphLayoutJob( &plj );

	docPsBeginParagraphLayoutProgress( &plj,
				    bi->biNumberInParent+ 1, 0, 0,
				    parentBi->biChildCount,
				    &(lj->ljPosition) );

	docPsAdvanceParagraphLayout( &advanced,
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos.plpProgress),
				    lj->ljPosition.lpPage, parentBi );

	if  ( bi->biParaStartsOnNewPage && ! bi->biParaInTable )
	    {
	    BufferItem *	bbi= bi;

	    while( bbi->biNumberInParent == 0 )
		{
		if  ( ! bbi->biParent )
		    { break;	}

		bbi->biParent->biTopPosition= bi->biTopPosition;
		bbi= bbi->biParent;
		}
	    }

	if  ( plj.pljPos0.plpProgress.pspPara >= bi->biNumberInParent+ 1 )
	    {
	    if  ( plj.pljPos0.plpProgress.pspPara > bi->biNumberInParent+ 1 )
		{ LLDEB(plj.pljPos0.plpProgress.pspPara,bi->biNumberInParent); }

	    from= bi->biNumberInParent+ 1;
	    }
	else{
	    from= plj.pljPos0.plpProgress.pspPara;

	    if  ( from == 0 )
		{
		lj->ljPosition= parentBi->biChildren[from]->biTopPosition;
		}
	    else{
		lj->ljPosition=
			    parentBi->biChildren[from- 1]->biBelowPosition;
		}
	    }
	}
    else{ from= bi->biNumberInParent+ 1;	}

    while( parentBi )
	{
	int		changed= 0;

	switch( parentBi->biLevel )
	    {
	    case DOClevDOC:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		if  ( parentBi->biInExternalItem == DOCinBODY		&&
		      npEndnotes->npPosition == DPftnPOS_DOC_END	)
		    {
		    if  ( docLayoutEndnotesForDocument( bf, lj ) )
			{ LDEB(1); return -1;	}
		    }

		break;

	    case DOClevSECT:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		if  ( parentBi->biInExternalItem == DOCinBODY		&&
		      npEndnotes->npPosition == DPftnPOS_SECT_END	)
		    {
		    if  ( docLayoutEndnotesForSection(
				    parentBi->biNumberInParent, bf, lj ) )
			{ LDEB(1); return -1;	}
		    }

		break;


	    parentRowAsGroup:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		break;

	    case DOClevCELL:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    ParagraphLayoutJob	plj;

		    docInitParagraphLayoutJob( &plj );

		    docPsBeginParagraphLayoutProgress( &plj,
					    from, 0, 0, parentBi->biChildCount,
					    &(lj->ljPosition) );

		    if  ( docLayoutParagraphs( &plc, parentBi, bf, lj, &plj ) )
			{ LDEB(1); return -1;	}

		    lj->ljPosition= plj.pljPos.plpPos;
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		break;

	    case DOClevROW:
		if  ( ! parentBi->biRowHasTableParagraphs )
		    { goto parentRowAsGroup;	}

		if  ( parentBi->biNumberInParent > 0	&&
		      parentBi->biParent		&&
		      parentBi->biRowKeepOnOnePage	)
		    {
		    BufferItem *	pp= parentBi->biParent;
		    BufferItem *	ch;

		    ch= pp->biChildren[parentBi->biNumberInParent- 1];

		    parentBi->biTopPosition= ch->biBelowPosition;
		    lj->ljPosition= ch->biBelowPosition;
		    }

		lj->ljPosition= parentBi->biTopPosition;

		if  ( docLayoutRowItem( parentBi, bf, lj, &plc ) )
		    { LDEB(1); return -1;	}

		break;

	    default:
		LDEB(parentBi->biLevel); return -1;
	    }

	docPsPlaceAdjustBottom( &changed, parentBi,
				    &(lj->ljPosition), lj->ljAdd, drChanged );

	if  ( ! changed )
	    { break;	}

	bi= parentBi; parentBi= bi->biParent;
	from= bi->biNumberInParent+ 1;
	}

    if  ( bi->biInExternalItem == DOCinBODY	&&
	  BF_HAS_FOOTNOTES( bf )		)
	{
	if  ( docLayoutFootnotesForColumn( bf, &(lj->ljPosition), lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( ! parentBi )
	{
	switch( bi->biInExternalItem )
	    {
	    case DOCinBODY:
		docLayoutAdjustDocumentRectangle( lj );
		break;

	    case DOCinSECT_HEADER:
	    case DOCinFIRST_HEADER:
	    case DOCinLEFT_HEADER:
	    case DOCinRIGHT_HEADER:
	    case DOCinSECT_FOOTER:
	    case DOCinFIRST_FOOTER:
	    case DOCinLEFT_FOOTER:
	    case DOCinRIGHT_FOOTER:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOCinFOOTNOTE:
	    case DOCinENDNOTE:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOCinFTNSEP:
	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
	    case DOCinAFTNSEP:
	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    default:
		LDEB(bi->biInExternalItem);
		break;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of the document root item.				*/
/*									*/
/************************************************************************/

static int docLayoutDocItem(		BufferItem *		docBi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);
    const DocumentGeometry *	dgDoc= &(dp->dpGeometry);

    int				i;

    if  ( docExternalItemPrelayout( &(bd->bdEiFtnsep), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiFtnsepc), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiFtncn), dgDoc, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(bd->bdEiAftnsep), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiAftnsepc), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiAftncn), dgDoc, lj ) )
	{ LDEB(1); return -1;	}

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	if  ( docLayoutItemImplementation( docBi->biChildren[i], bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docBi->biInExternalItem == DOCinBODY		&&
	  npEndnotes->npPosition == DPftnPOS_DOC_END	)
	{
	if  ( docLayoutEndnotesForDocument( bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a document item.					*/
/*									*/
/*  This is the main entry poin of the formatter.			*/
/*									*/
/************************************************************************/

int docLayoutItemImplementation(	BufferItem *		bi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    int				i;
    int				y1;

    ParagraphLayoutContext	plc;

    plc.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plc.plcAdd= lj->ljAdd;
    plc.plcBd= lj->ljBd;

    plc.plcScreenLayout= lj->ljLayoutScreen;
    plc.plcStartParagraph= docLayoutParaInit;
    plc.plcAdjustBottom= docPsLayoutAdjustBottom;

    y1= BI_BELOW_PIXELS( lj->ljAdd, bi )- 1;

    i= BI_TOP_PIXELS( lj->ljAdd, bi );

    if  ( drChanged				&&
	  drChanged->drY0 > i	)
	{ drChanged->drY0=  i;	}

    bi->biTopPosition= lj->ljPosition;

    if  ( drChanged )
	{
	i= BI_TOP_PIXELS( lj->ljAdd, bi );

	if  ( drChanged->drY0 > i )
	    { drChanged->drY0=  i;			}
	if  ( drChanged->drY1 < y1 )
	    { drChanged->drY1=  y1;	}
	}

    switch( bi->biLevel )
	{
	case DOClevDOC:

	    if  ( docLayoutDocItem( bi, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	rowAsGroup:
	sectAsGroup:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docLayoutItemImplementation( bi->biChildren[i], bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevCELL:
	    {
	    ParagraphLayoutJob	plj;

	    docInitParagraphLayoutJob( &plj );

	    docPsBeginParagraphLayoutProgress( &plj,
					    0, 0, 0, bi->biChildCount,
					    &(lj->ljPosition) );

	    if  ( docLayoutParagraphs( &plc, bi, bf, lj, &plj ) )
		{ LDEB(1); return -1;	}

	    lj->ljPosition= plj.pljPos.plpPos;
	    }
	    break;

	case DOClevSECT:
	    if  ( ! bi->biParent )
		{ goto sectAsGroup;	}

	    if  ( docLayoutSectItem( bi, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{ goto rowAsGroup;	}

	    if  ( docLayoutRowItem( bi, bf, lj, &plc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docLayoutParaItem( bi, bf, lj, &plc ) )
		{ LDEB(1); return -1;	}

	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    bi->biBelowPosition= lj->ljPosition;

    y1= BI_BELOW_PIXELS( lj->ljAdd, bi )- 1;

    if  ( drChanged		&&
	  drChanged->drY1 < y1	)
	{ drChanged->drY1=  y1;	}

    return 0;
    }

int docLayoutItemAndParents(	BufferItem *		bi,
				LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    BlockFrame			bf;

    docLayoutInitBlockFrame( &bf );

    if  ( bi->biLevel != DOClevDOC )
	{
	docBlockFrameTwips( &bf, bi, bd,
			    lj->ljPosition.lpPage, lj->ljPosition.lpColumn );

	if  ( bi->biInExternalItem == DOCinBODY		&&
	      docPrevParagraph( bi )			)
	    {
	    DocumentPosition		dpHere;

	    if  ( docFirstPosition( &dpHere, bi ) )
		{ LDEB(1); return -1;	}

	    if  ( docCollectFootnotesForColumn( &bf, &dpHere, lj ) )
		{ LDEB(lj->ljPosition.lpPage); return -1;	}
	    }
	}

    if  ( docLayoutItemImplementation( bi, &bf, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docAdjustParentGeometry( bi, &bf, lj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish formatting a paragraph, either because additional lines are	*/
/*  to be added to the paragraph, or because its tail is to be shifted	*/
/*  (up or) down after some lines have been reformatted.		*/
/*									*/
/************************************************************************/

static int docLayoutFinishParaAdjust(
				ParagraphLayoutPosition *	plp,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				const LayoutJob *		lj,
				BufferItem *			paraBi )
    {
    int				done= 0;
    ParagraphStripPosition *	psp= &(plp->plpProgress);

    while( psp->pspPart < paraBi->biParaParticuleCount )
	{
	int	accepted;

	accepted= docLayoutLines( plp, bf, plc, paraBi );

	if  ( accepted < 0 )
	    { LDEB(accepted); return -1;	}

	plp->plpProgress.pspPart += accepted;
	done += accepted;

	/*  1  */
	if  ( psp->pspPart < paraBi->biParaParticuleCount		&&
	      ( accepted > 0 || ! plp->plpPos.lpAtTopOfColumn )		)
	    {
	    if  ( BF_HAS_FOOTNOTES( bf )				&&
		  docLayoutFootnotesForColumn( bf, &(plp->plpPos), lj )	)
		{ LDEB(1); return -1;	}

	    docLayoutToNextColumn( paraBi, plc->plcBd, &(plp->plpPos), bf );

	    if  ( psp->pspPart == 0 )
		{ paraBi->biTopPosition= plp->plpPos;	}
	    }
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Adjust the layout of a paragraph after a modification.		*/
/*									*/
/*  0)  Do not start with the first line of the page.			*/
/*  1)  Initialisation for paragraph formatting.			*/
/*  2)  Recalculate the layout of the first line that was changed.	*/
/*  3)  Did not fit on this page.. Try the next one.			*/
/*  4)  Layout subsequent lines of the paragraph, until the layout	*/
/*	process reaches the point outside the modified region where it	*/
/*	starts to cut lines at the same point where they were		*/
/*	originally cut.							*/
/*  5)  More lines than the paragraph originally contained are needed.	*/
/*	Just layout the rest.						*/
/*  6)  We are beyond the end of the modified region, and the new line	*/
/*	is to start where the old line starts. Just place the		*/
/*	subsequent lines from here.					*/
/*  7)  Recalculate the layout of a subsequent line that was changed.	*/
/*  8)  Remove superfluous lines from the administration.		*/
/*  9)  If the paragraph has changed height.. Adjust the redraw region.	*/
/*  10) Adjust the geometry of the rest of the document to the changes.	*/
/*									*/
/************************************************************************/

/*  3  */
static int docPsLayoutOneLine(	TextLine *			tl,
				BufferItem *			paraBi,
				ParagraphLayoutPosition *	plp,
				ParticuleData *			pd,
				const ParagraphLayoutContext *	plc,
				LayoutJob *			lj,
				DocumentRectangle *		drChanged,
				BlockFrame *			bf )
    {
    AppDrawingData *		add= plc->plcAdd;
    BufferDocument *		bd= plc->plcBd;

    ParagraphStripPosition *	psp= &(plp->plpProgress);

    const int			fromLinePos= 1;
    int				accepted;

    NotesReservation		nrLine;

    TextLine			boxLine;

    if  ( drChanged )
	{
	int	x0Pixels= TL_TOP_PIXELS( add, tl );
	int	x1Pixels= TL_BELOW_PIXELS( add, tl );

	if  ( drChanged->drY0 > x0Pixels	)
	    { drChanged->drY0=  x0Pixels;	}

	if  ( drChanged->drY1 < x1Pixels	)
	    { drChanged->drY1=  x1Pixels;	}
	}

    docInitNotesReservation( &nrLine );

    /*  2,7  */
    accepted= docLayout_Line( &boxLine, &nrLine, bf, fromLinePos,
				paraBi, psp->pspPart, pd, plc,
				&(plp->plpFormattingFrame),
				&(plp->plpPos), &(plp->plpPos),
				&(lj->ljLayoutScreen) );

    if  ( accepted < 0 )
	{ LDEB(accepted); return -1;	}

    /*  3  */
    if  ( accepted == 0 )
	{
	if  ( paraBi->biInExternalItem != DOCinBODY	&&
	      paraBi->biInExternalItem != DOCinENDNOTE	&&
	      paraBi->biInExternalItem != DOCinAFTNSEP	)
	    { LDEB(paraBi->biInExternalItem);	}

	if  ( BF_HAS_FOOTNOTES( bf )					&&
	      docLayoutFootnotesForColumn( bf, &(plp->plpPos), lj )	)
	    { LDEB(1); return -1;	}

	docLayoutToNextColumn( paraBi, bd, &(plp->plpPos), bf );

	docInitNotesReservation( &nrLine );

	accepted= docLayout_Line(
			    &boxLine, &nrLine, bf, fromLinePos,
			    paraBi, psp->pspPart, pd, plc,
			    &(plp->plpFormattingFrame),
			    &(plp->plpPos), &(plp->plpPos),
			    &(lj->ljLayoutScreen) );

	if  ( accepted < 1 )
	    { LDEB(accepted); return -1;	}
	}

    docLayoutReserveNoteHeight( &(plp->plpFormattingFrame), bf, &nrLine );

    if  ( drChanged )
	{
	int	x1= LP_YPIXELS( add, &(plp->plpPos) );

	if  ( drChanged->drY1 < x1	)
	    { drChanged->drY1=  x1;	}
	}

    *tl= boxLine;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph after an editing action.		*/
/*									*/
/*  To avoid screen flashing, a minimalistic approach is used: As	*/
/*  little of the layout is redone, and an administration is kept of	*/
/*  what has been changed, in order to redraw a minimal screen		*/
/*  rectangle.								*/
/*									*/
/************************************************************************/

int docAdjustParaLayout(	BufferItem *		paraBi,
				int			fromLine,
				int			stroffShift,
				int			stroffUpto,
				LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    AppDrawingData *		add= lj->ljAdd;

    TextLine			boxLine;
    TextLine *			tl;
    int				oneMore;

    TextParticule *		tp;

    ParticuleData *		pd;

    int				accepted;
    int				off;

    const int			bottomTwips= -1;
    const int			stripHigh= -1;

    int				paragraphHeightChanged;
    int				firstLineHeightChanged;

    ParagraphLayoutContext	plcL;
    ParagraphLayoutContext	plcP;

    ParagraphLayoutPosition	plp;

    BlockFrame			bf;

#   if 0
    LDEB(444);
    lj->ljPosition= paraBi->biTopPosition;
    if  ( docLayoutItemAndParents( paraBi, lj ) )
	{ LDEB(1); return -1;	}
    return 0;
#   endif

    plcL.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plcL.plcAdd= lj->ljAdd;
    plcL.plcBd= lj->ljBd;

    plcP.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plcP.plcAdd= lj->ljAdd;
    plcP.plcBd= lj->ljBd;

    plcL.plcScreenLayout= lj->ljLayoutScreen;
    plcL.plcStartParagraph= docLayoutParaInit;
    plcL.plcAdjustBottom= docPsLayoutAdjustBottom;

    plcP.plcScreenLayout= lj->ljPlaceScreen;
    plcP.plcStartParagraph= docPlaceParaInit;
    plcP.plcAdjustBottom= docPsPlaceAdjustBottom;

    if  ( docPsClaimParticuleData( paraBi, &pd ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}

    /*  1  */
    if  ( docLayoutOpenParaFonts( bd, paraBi, add ) )
	{ LDEB(1); return -1;	}

    /*  0  */
    tl= paraBi->biParaLines+ fromLine;
    oneMore= 0;
    if  ( fromLine > 0							&&
	  tl[-1].tlTopPosition.lpPage != tl[ 0].tlTopPosition.lpPage	)
	{ fromLine--; tl--; oneMore= 1;	}
    tp= paraBi->biParaParticules+ tl->tlFirstParticule;

    plp.plpProgress.pspPara= paraBi->biNumberInParent;
    plp.plpProgress.pspLine= fromLine;
    plp.plpProgress.pspPart= tl->tlFirstParticule;
    plp.plpPos= tl->tlTopPosition;

    lj->ljPosition= tl->tlTopPosition;

    docBlockFrameTwips( &bf, paraBi, bd,
					tl[ 0].tlTopPosition.lpPage,
					tl[ 0].tlTopPosition.lpColumn );

    if  ( paraBi->biInExternalItem == DOCinBODY	)
	{
	DocumentSelection	dsLine;

	docLineSelection( &dsLine, paraBi, fromLine );

	if  ( docCollectFootnotesForColumn( &bf, &(dsLine.dsBegin), lj ) )
	    { LDEB(lj->ljPosition.lpPage); return -1;	}
	}

    docParagraphFrameTwips( &(plp.plpFormattingFrame), &bf,
					bottomTwips, stripHigh, paraBi );

    if  ( docPsParagraphLineExtents( &(add->addPhysicalFontList), paraBi ) )
	{ LDEB(1); return -1;	}

    if  ( lj->ljLayoutScreen.slScreenFrame )
	{
	(*lj->ljLayoutScreen.slScreenFrame)( &(plp.plpFormattingFrame),
								add, paraBi );
	}

    if  ( lj->ljLayoutScreen.slStartParagraph				&&
	  (*lj->ljLayoutScreen.slStartParagraph)( paraBi, add, bd )	)
	{ LDEB(1); return -1;	}

    if  ( drChanged )
	{
	int	y0= TL_TOP_PIXELS( add, tl );

	if  ( drChanged->drX0 > add->addBackRect.drX0 )
	    { drChanged->drX0=  add->addBackRect.drX0;	}
	if  ( drChanged->drX1 < add->addBackRect.drX1 )
	    { drChanged->drX1=  add->addBackRect.drX1;	}

	if  ( drChanged->drY0 > y0 )
	    { drChanged->drY0=  y0;	}
	}

    /*  2,3  */
    boxLine= *tl;
    accepted= docPsLayoutOneLine( &boxLine, paraBi, &plp, pd, &plcL,
							lj, drChanged, &bf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    firstLineHeightChanged= 0;
    if  ( boxLine.tlTopPosition.lpPage != tl->tlTopPosition.lpPage	||
	  boxLine.tlTopPosition.lpPageYTwips !=
				    tl->tlTopPosition.lpPageYTwips	||
	  boxLine.tlLineHeightTwips != tl->tlLineHeightTwips		)
	{ firstLineHeightChanged= 1;	}

    *tl= boxLine;

    if  ( tl->tlHasPageBreak )
	{
	if  ( paraBi->biInExternalItem != DOCinBODY )
	    { LDEB(paraBi->biInExternalItem);			}
	else{
	    if  ( BF_HAS_FOOTNOTES( &bf )				&&
		  docLayoutFootnotesForColumn( &bf, &(plp.plpPos), lj )	)
		{ LDEB(1); return -1;	}

	    docLayoutToNextColumn( paraBi, bd, &(plp.plpPos), &bf );
	    }
	}

    off= tl->tlStroff+ tl->tlStrlen;
    plp.plpProgress.pspLine++; tl++;
    plp.plpProgress.pspPart += accepted; tp += accepted; pd += accepted;

    /*  4  */
    while( plp.plpProgress.pspPart < paraBi->biParaParticuleCount )
	{
	/*  5  */
	if  ( plp.plpProgress.pspLine >= paraBi->biParaLineCount )
	    {
	    accepted= docLayoutFinishParaAdjust( &plp, &bf, &plcL, lj, paraBi );

	    if  ( accepted < 1 )
		{ LDEB(accepted); return -1;	}

	    break;
	    }

	/*  6  */
	if  ( ! oneMore							&&
	      tl->tlStroff + stroffShift == off && off >= stroffUpto	)
	    {
	    int		ll;
	    int		partShift;

	    partShift= plp.plpProgress.pspPart- tl->tlFirstParticule;

	    for ( ll= plp.plpProgress.pspLine;
					ll < paraBi->biParaLineCount; ll++ )
		{
		paraBi->biParaLines[ll].tlFirstParticule += partShift;
		paraBi->biParaLines[ll].tlStroff += stroffShift;
		}

	    accepted= docLayoutFinishParaAdjust( &plp, &bf, &plcP, lj, paraBi );
	    if  ( accepted < 1 )
		{ LDEB(accepted); return -1;	}

	    break;
	    }

	/*  7,3  */
	boxLine= *tl;
	accepted= docPsLayoutOneLine( &boxLine, paraBi, &plp, pd, 
						&plcL, lj, drChanged, &bf );

	if  ( accepted < 1 )
	    { LDEB(accepted); return -1;	}

	*tl= boxLine;

	if  ( tl->tlHasPageBreak )
	    {
	    if  ( paraBi->biInExternalItem != DOCinBODY )
		{ LDEB(paraBi->biInExternalItem);		}
	    else{
		if  ( BF_HAS_FOOTNOTES( &bf )				&&
		      docLayoutFootnotesForColumn( &bf, &(plp.plpPos), lj ) )
		    { LDEB(1); return -1;	}

		docLayoutToNextColumn( paraBi, lj->ljBd, &(plp.plpPos), &bf );
		}
	    }

	off= tl->tlStroff+ tl->tlStrlen;
	plp.plpProgress.pspLine++; tl++; oneMore= 0;
	plp.plpProgress.pspPart += accepted; tp += accepted; pd += accepted;
	}

    /*  8  */
    if  ( plp.plpProgress.pspLine < paraBi->biParaLineCount )
	{
	docDeleteLines( paraBi, plp.plpProgress.pspLine,
			    paraBi->biParaLineCount- plp.plpProgress.pspLine );
	}

    if  ( plp.plpProgress.pspPart != paraBi->biParaParticuleCount )
	{
	LLDEB(plp.plpProgress.pspPart,paraBi->biParaParticuleCount);
	return -1;
	}
    if  ( plp.plpProgress.pspLine !=  paraBi->biParaLineCount )
	{
	LLDEB(plp.plpProgress.pspLine,paraBi->biParaLineCount);
	return -1;
	}

    /*  9  */
    paragraphHeightChanged= 0;

    docPsPlaceAdjustBottom( &paragraphHeightChanged, paraBi,
					&(plp.plpPos), lj->ljAdd, drChanged );

    lj->ljPosition= plp.plpPos;

    if  ( firstLineHeightChanged || paragraphHeightChanged )
	{
	if  ( firstLineHeightChanged		&&
	      paraBi->biNumberInParent > 0	)
	    {
	    paraBi= paraBi->biParent->biChildren[paraBi->biNumberInParent- 1];
	    lj->ljPosition= paraBi->biBelowPosition;
	    }

	/*  10  */
	if  ( docAdjustParentGeometry( paraBi, &bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

