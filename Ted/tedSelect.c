/************************************************************************/
/*									*/
/*  Ted: Management of the selection and the current position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<limits.h>

#   include	"docLayout.h"
#   include	"tedApp.h"
#   include	"tedLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Is the selection a position between two chars?			*/
/*									*/
/************************************************************************/

int tedHasIBarSelection(	const TedDocument *   td )
    {
    return docIsIBarSelection( &(td->tdDocumentSelection) );

    return 0;
    }

int tedHasSelection(	const TedDocument *   td )
    {
    const DocumentSelection *	ds= &(td->tdDocumentSelection);

    if  ( ds->dsBegin.dpBi )
	{ return 1;	}

    return 0;
    }

int tedGetSelection(	DocumentSelection *	ds,
			SelectionGeometry *	sg,
			TedDocument *		td )
    {
    const DocumentSelection *	dss= &(td->tdDocumentSelection);

    if  ( ! dss->dsBegin.dpBi )
	{ return 1;	}

    *ds= td->tdDocumentSelection;
    *sg= td->tdSelectionGeometry;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the derived properties such as line number and		*/
/*  coordinates of the current selection.				*/
/*									*/
/************************************************************************/

void tedDelimitCurrentSelection(	TedDocument *		td,
					AppDrawingData *	add )
    {
    DocumentSelection *		ds= &(td->tdDocumentSelection);
    BufferDocument *		bd= td->tdDocument;

    docSetDocumentPosition( &(ds->dsBegin),
				ds->dsBegin.dpBi, ds->dsBegin.dpStroff, 1 );
    docSetDocumentPosition( &(ds->dsEnd),
				ds->dsEnd.dpBi, ds->dsEnd.dpStroff, 0 );

    tedSelectionGeometry( &(td->tdSelectionGeometry),
					&(td->tdDocumentSelection), bd, add );

    return;
    }

/************************************************************************/
/*									*/
/*  Is the selection exactly an object.					*/
/*									*/
/************************************************************************/

int tedGetObjectSelection(	TedDocument *   	td,
				DocumentPosition *	dpObject,
				InsertedObject **	pIo )
    {
    return docGetObjectSelection( &(td->tdDocumentSelection), dpObject, pIo );
    }


static void tedScrollToPosition(EditDocument *			ed,
				const PositionGeometry *	pg,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    appScrollToRectangle( ed,
			pg->pgXPixels, PG_TOP_PIXELS( add, pg ),
			pg->pgXPixels, pg->pgY1Pixels,
			pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Extend a selection that begins outside a table cell and ends inside	*/
/*  that cell.								*/
/*									*/
/*  The speccial cases that occur when the selection begins in a table	*/
/*  are covered by tedExtendSelectionFromTable(), that has priority	*/
/*  over tedExtendSelectionIntoTable().					*/
/*									*/
/************************************************************************/

static int tedExtendSelectionIntoTable(	EditDocument *		ed,
					DocumentPosition *	dpEnd )
    {
    BufferItem *	cellBi= dpEnd->dpBi->biParent;
    BufferItem *	rowBi=	cellBi->biParent;

    int			col1= rowBi->biChildCount -1;

    docLastPosition( dpEnd, rowBi->biChildren[col1] );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extend a selection that begins inside a table cell and ends outside	*/
/*  that cell.								*/
/*									*/
/*  1)  Not in same section.						*/
/*  2)  Look for selection end in same table.				*/
/*  3)  Found?								*/
/*									*/
/************************************************************************/

static int tedExtendSelectionFromTable(	EditDocument *		ed,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd,
					int *			pCol0,
					int *			pCol1 )
    {
    BufferItem *	cellBi= dpBegin->dpBi->biParent;
    BufferItem *	rowBi=	cellBi->biParent;
    BufferItem *	sectBi= rowBi->biParent;

    BufferItem *	endRowBi= dpEnd->dpBi->biParent->biParent;

    int			row1= rowBi->biNumberInParent;

    int			col0= -1;
    int			col1= -1;

    /*  1  */
    if  ( dpEnd->dpBi->biParent->biParent->biParent != sectBi )
	{
	docFirstPosition( dpBegin, rowBi->biChildren[0] );

	if  ( dpEnd->dpBi->biParaInTable )
	    { tedExtendSelectionIntoTable( ed, dpEnd ); }

	return 0;
	}

    /*  2  */
    while( row1 < rowBi->biRowTablePast- 1 )
	{
	if  ( endRowBi == sectBi->biChildren[row1] )
	    { break;	}

	row1++;
	}

    if  ( endRowBi == sectBi->biChildren[row1] )
	{
	if  ( dpEnd->dpBi->biParent->biNumberInParent <
					    cellBi->biNumberInParent )
	    {
	    col0= dpEnd->dpBi->biParent->biNumberInParent;
	    col1= dpBegin->dpBi->biParent->biNumberInParent;

	    docFirstPosition( dpBegin, rowBi->biChildren[col0] );
	    rowBi= dpEnd->dpBi->biParent->biParent;
	    docLastPosition( dpEnd, rowBi->biChildren[col1] );
	    }
	else{
	    docFirstPosition( dpBegin, dpBegin->dpBi->biParent );
	    docLastPosition( dpEnd, dpEnd->dpBi->biParent );
	    }

	col0= dpBegin->dpBi->biParent->biNumberInParent;
	col1= dpEnd->dpBi->biParent->biNumberInParent;
	}
    else{
	docFirstPosition( dpBegin, rowBi->biChildren[0] );

	if  ( dpEnd->dpBi->biParaInTable )
	    { tedExtendSelectionIntoTable( ed, dpEnd ); }

	return 0;
	}

    *pCol0= col0; *pCol1= col1; return 0;
    }

/************************************************************************/
/*									*/
/*  Extend the selection upon subsequent MotionNotify events.		*/
/*									*/
/************************************************************************/

static void tedSetExtendedSelection(
				EditDocument *			ed,
				int				exposeSelection,
				int				col0,
				int				col1,
				int				direction,
				const DocumentPosition *	dpExposeBegin,
				const DocumentPosition *	dpExposeEnd,
				const DocumentPosition *	dpTo,
				const DocumentPosition *	dpBegin,
				const DocumentPosition *	dpEnd )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);

    DocumentSelection		dsExpose;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;

    PositionGeometry		pgTo;

    int				scrolledX= 0;
    int				scrolledY= 0;

    docSetRangeSelection( &dsExpose, dpExposeBegin, dpExposeEnd, 1, -1, -1 );

    tedPositionGeometry( &pgTo, dpTo, bd, add );

    tedScrollToPosition( ed, &pgTo, &scrolledX, &scrolledY );

    if  ( exposeSelection )
	{
	if  ( tedGetSelection( &dsDoc, &sgDoc, td ) )
	    { LDEB(1); return;	}

	appDocExposeRectangle( ed, &(sgDoc.sgRectangle), scrolledX, scrolledY );
	}

    tedExposeSelection( ed, &dsExpose, scrolledX, scrolledY );

    docSetRangeSelection( &(td->tdDocumentSelection), dpBegin, dpEnd,
						    direction, col0, col1 );
    tedSelectionGeometry( &(td->tdSelectionGeometry),
				    &(td->tdDocumentSelection), bd, add );

    if  ( exposeSelection )
	{
	if  ( tedGetSelection( &dsDoc, &sgDoc, td ) )
	    { LDEB(1); return;	}

	appDocExposeRectangle( ed, &(sgDoc.sgRectangle), scrolledX, scrolledY );
	}

    return;
    }

static void tedBalanceFieldSelection(	EditDocument *		ed,
					int *			pBalanced,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd )
    {
    int				beginMoved= 0;
    int				endMoved= 0;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    docBalanceFieldSelection( &beginMoved, &endMoved, td->tdDocument,
							    dpBegin, dpEnd );

    if  ( beginMoved )
	{ *pBalanced= 1;	}

    if  ( endMoved )
	{ *pBalanced= 1;	}

    return;
    }

int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				col0= -1;
    int				col1= -1;

    DocumentPosition		dpFrom;
    DocumentPosition		dpTo;

    int				directionToAnchor;
    int				directionBeginAnchor;
    int				directionEndAnchor;
    int				dir;

    int				scrolledX= 0;
    int				scrolledY= 0;

    const int			mindLine= 1;

    int				cellChanged= 0;
    int				balanced= 0;
    int				exposeSelection;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;

    dpFrom= *dpAnchor;
    dpTo= *dpFound;

    if  ( tedGetSelection( &dsDoc, &sgDoc, td ) )
	{ LDEB(1); return -1;	}

    directionToAnchor= docComparePositions( &dpTo, dpAnchor, mindLine );
    directionBeginAnchor= docComparePositions(
			    &(dsDoc.dsBegin), dpAnchor, mindLine );
    directionEndAnchor= docComparePositions(
			    &(dsDoc.dsEnd), dpAnchor, mindLine );

    /********************/
    /*  Before anchor.	*/
    /********************/
    if  ( directionToAnchor < 0 )
	{
	int		directionToBegin;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpBi->biParaInTable )
		{ col0= col1= dpFrom.dpBi->biParent->biNumberInParent;	}
	    }
	else{
	    if  ( dpTo.dpBi->biParaInTable )
		{
		if  ( tedExtendSelectionFromTable( ed, &dpTo, &dpFrom,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpFrom.dpBi->biParaInTable )
		    {
		    if  ( tedExtendSelectionIntoTable( ed, &dpFrom ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( ed, &balanced, &dpTo, &dpFrom );

	directionToBegin= docComparePositions( &dpTo,
				    &(dsDoc.dsBegin), mindLine );
	cellChanged= ! docPositionsInsideCell(
					&dpTo, &(dsDoc.dsBegin) );

	/****************************************/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionEndAnchor > 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Left.	*/
	/************************/
	if  ( directionToBegin < 0 )
	    {
	    dir= -1;
	    exposeSelection=	directionEndAnchor > 0	||
				balanced		||
				cellChanged		;

	    tedSetExtendedSelection( ed, exposeSelection, col0, col1, dir,
				&dpTo, &(dsDoc.dsBegin),
				&dpTo, &dpTo, &dpFrom );

	    return 0;
	    }

	/************************/
	/*  Shrunk Left.	*/
	/************************/
	if  ( directionToBegin > 0 )
	    {
	    dir= -1;
	    exposeSelection=	directionEndAnchor > 0	||
				balanced		||
				cellChanged		;

	    tedSetExtendedSelection( ed, exposeSelection, col0, col1, dir,
				    &(dsDoc.dsBegin), &dpTo,
				    &dpTo, &dpTo, &dpFrom );

	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  After anchor.	*/
    /********************/
    if  ( directionToAnchor > 0 )
	{
	int		directionToEnd;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpBi->biParaInTable )
		{ col0= col1= dpFrom.dpBi->biParent->biNumberInParent;	}
	    }
	else{
	    if  ( dpFrom.dpBi->biParaInTable )
		{
		if  ( tedExtendSelectionFromTable( ed, &dpFrom, &dpTo,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpTo.dpBi->biParaInTable )
		    {
		    if  ( tedExtendSelectionIntoTable( ed, &dpTo ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( ed, &balanced, &dpFrom, &dpTo );

	directionToEnd= docComparePositions( &dpTo,
					&(dsDoc.dsEnd), mindLine );
	cellChanged= ! docPositionsInsideCell(
					&dpTo, &(dsDoc.dsEnd) );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Right.	*/
	/************************/
	if  ( directionToEnd > 0 )
	    {
	    dir= 1;
	    exposeSelection=	directionBeginAnchor < 0	||
				balanced			||
				cellChanged			;

	    tedSetExtendedSelection( ed, exposeSelection, col0, col1, dir,
				    &(dsDoc.dsEnd), &dpTo,
				    &dpTo, &dpFrom, &dpTo );

	    return 0;
	    }

	/************************/
	/*  Shrunk Right.	*/
	/************************/
	if  ( directionToEnd < 0 )
	    {
	    dir= 1;
	    exposeSelection=	directionBeginAnchor < 0	||
				balanced			||
				cellChanged			;

	    tedSetExtendedSelection( ed, exposeSelection, col0, col1, dir,
				    &dpTo, &(dsDoc.dsEnd),
				    &dpTo, &dpFrom, &dpTo );

	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  At anchor.	*/
    /********************/
	{
	dpFrom= *dpAnchor;
	dpTo= *dpAnchor;

	tedBalanceFieldSelection( ed, &balanced, &dpFrom, &dpTo );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      directionEndAnchor > 0	)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	dir= 0;
	exposeSelection=	directionBeginAnchor < 0	||
				directionEndAnchor > 0		||
				balanced			;

	tedSetExtendedSelection( ed, exposeSelection, col0, col1, dir,
				&dpFrom, &dpTo,
				&dpFrom, &dpFrom, &dpTo );
	}

    return 0;
    }

int tedExtendSelectionToXY(	EditDocument *			ed,
				BufferItem *			rootBi,
				const DocumentPosition *	dpAnchor,
				int				mouseX,
				int				mouseY )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const AppDrawingData *	add= &(ed->edDrawingData);

    DocumentPosition		dpTo;
    PositionGeometry		pgTo;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    dpTo= *dpAnchor;

    if  ( tedFindPosition( &dpTo, &pgTo, bd, rootBi,
					    add, mouseX+ ox, mouseY+ oy ) )
	{ /*LLDEB(mouseX,mouseY);*/ return 0; }

    return tedExtendSelectionToPosition( ed, dpAnchor, &dpTo );
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

int tedLineUp(		DocumentPosition *	dp,
			const BufferDocument *	bd,
			const AppDrawingData *	add )
    {
    TextLine *		tl;
    TextParticule *	tp;
    int			part;
    int			off;

    int			x;
    int			baseline;

    PositionGeometry	pg;

    tedPositionGeometry( &pg, dp, bd, add );

    if  ( docLineUp( &tp, &tl, dp ) )
	{ return -1;	}

    baseline= TL_BASE_PIXELS( add, tl );

    part= tedFindParticule( tl, dp->dpBi->biParaParticules,
						    pg.pgXPixels, baseline );
    if  ( part < 0 )
	{ LDEB(part); return -1;	}

    off= tedFindStringOffset( bd, dp->dpBi, part, add, &x, pg.pgXPixels, baseline );
    if  ( off < 0 )
	{ LDEB(off); return -1;	}

    dp->dpStroff= off;
    dp->dpParticule= part;

    return 0;
    }

int tedArrowDown(	DocumentPosition *	dp,
			const BufferDocument *	bd,
			const AppDrawingData *	add )
    {
    TextLine *		tl;
    TextParticule *	tp;
    int			part;
    int			off;

    int			x;
    int			baseline;

    PositionGeometry	pg;

    tedPositionGeometry( &pg, dp, bd, add );

    if  ( docLineDown( &tp, &tl, dp ) )
	{ return -1;	}

    baseline= TL_BASE_PIXELS( add, tl );

    part= tedFindParticule( tl, dp->dpBi->biParaParticules,
						    pg.pgXPixels, baseline );
    if  ( part < 0 )
	{ LDEB(part); return -1;	}

    off= tedFindStringOffset( bd, dp->dpBi, part, add, &x, pg.pgXPixels, baseline );
    if  ( off < 0 )
	{ LDEB(off); return -1;	}

    dp->dpStroff= off;
    dp->dpParticule= part;

    return 0;
    }

int tedPageUp(	DocumentPosition *	dp,
		BufferDocument *	bd,
		const AppDrawingData *	add,
		int			pageHeight )
    {
    int			y;

    PositionGeometry	pg;

    tedPositionGeometry( &pg, dp, bd, add );

    y= pg.pgBaselinePixels- pageHeight;
    if  ( y < 0 )
	{ y= 0;	}

    if  ( tedFindPosition( dp, &pg, bd, &(bd->bdItem), add, pg.pgXPixels, y ) )
	{ return -1;	}

    return 0;
    }

int tedPageDown(	DocumentPosition *	dp,
			BufferDocument *	bd,
			const AppDrawingData *	add,
			int			docHeight,
			int			pageHeight )
    {
    int			y;

    PositionGeometry	pg;

    tedPositionGeometry( &pg, dp, bd, add );

    y= pg.pgBaselinePixels+ pageHeight;

    if  ( y >= docHeight )
	{ y= docHeight- 1;	}

    if  ( tedFindPosition( dp, &pg, bd, &(bd->bdItem), add, pg.pgXPixels, y ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scroll a selection into view.					*/
/*									*/
/************************************************************************/

void tedScrollToSelection(	EditDocument *		ed,
				int *			pScrolledX,
				int *			pScrolledY )
    {
    const AppDrawingData *	add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;

    if  ( tedGetSelection( &dsDoc, &sgDoc, td ) )
	{ LDEB(1); return;	}

    appScrollToRectangle( ed,
		    sgDoc.sgBegin.pgXPixels,
		    PG_TOP_PIXELS( add, &(sgDoc.sgBegin) ),
		    sgDoc.sgEnd.pgXPixels, sgDoc.sgEnd.pgY1Pixels,
		    pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Remember the text attribute of the beginning of the selection.	*/
/*									*/
/*  NOTE that for I-Bar selections, in case of ambiguity, there is a	*/
/*	preference for the attribute of the particule before the	*/
/*	current position.						*/
/*									*/
/************************************************************************/

static void tedSetCurrentTextAttribute(	TedDocument *			td,
					int				IBar,
					const DocumentPosition *	dp )
    {
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		bi= dp->dpBi;
    TextParticule *		tp= bi->biParaParticules+ dp->dpParticule;

    if  ( IBar				&&
	  dp->dpParticule > 0		&&
	  dp->dpStroff == tp->tpStroff	&&
	  tp[-1].tpKind == DOCkindTEXT	)
	{ tp--; }

    utilGetTextAttributeByNumber( &(td->tdCurrentTextAttribute),
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
    td->tdCurrentPhysicalFont= tp->tpPhysicalFont;
    td->tdCurrentTextAttributeNumber= tp->tpTextAttributeNumber;

    return;
    }

/************************************************************************/
/*									*/
/*  A selection was made, do bookkeeping and provoke drawing.		*/
/*									*/
/*  0)  If an object (Picture) was selected, remove the special window.	*/
/*  1)  Provoke the old selection to be redrawn. (Before scrolling)	*/
/*  2)  Stop Cursor blinking.						*/
/*  3)  Scroll to the new selection.					*/
/*  4)  Provoke the old selection to be redrawn. (It might have moved)	*/
/*  5)  Bookkeeping.							*/
/*  6)  Provoke the new selection to be redrawn.			*/
/*									*/
/*  NOTE:	As scrolling to the new selection can provoke a redraw	*/
/*		the selection needs to be set before the scrollbar is	*/
/*		adapted. Otherwise the selection administration can	*/
/*		still refer to deleted text.				*/
/*									*/
/*  A)  If the selection is partially inside and partially outside a	*/
/*	text level field, force it to contain the whole field. This is	*/
/*	to enforce proper text field nesting.				*/
/*									*/
/************************************************************************/

void tedSetSelection(	EditDocument *			ed,
			const DocumentSelection *	dsSet,
			int *				pScrolledX,
			int *				pScrolledY )
    {
    EditApplication *		ea= ed->edApplication;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);
    int				hadSelection;

    DocumentSelection		dsOld;
    SelectionGeometry		sgOld;

    DocumentSelection		dsNew;
    SelectionGeometry		sgNew;

    int				beginMoved= 0;
    int				endMoved= 0;

    DocumentRectangle		drExpose;

    int				redrawExternalItemOld= 0;

    ExternalItem *		eiSet= (ExternalItem *)0;
    BufferItem *		bodySectBiSet= (BufferItem *)0;

    BufferItem *		rootBiSet;
    DocumentRectangle		drExternalSet;
    int				redrawExternalItemSet= 0;

    int				otherRoot;

    const int			justUsed= 0;

    /*  0  */
    if  ( td->tdObjectSelected )
	{ tedHideObjectWindows( ed );	}

    if  ( tedGetSelection( &dsOld, &sgOld, td ) )
	{ LDEB(1); return;	}

    rootBiSet= docGetSelectionRoot( &eiSet, &bodySectBiSet, bd, dsSet );
    if  ( ! rootBiSet )
	{ XDEB(rootBiSet); return;	}

    otherRoot= docSelectionDifferentRoot( &dsOld, rootBiSet );

    /******/
    switch( dsOld.dsSelectionScope.ssInExternalItem )
	{
	case DOCinBODY:
	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    if  ( otherRoot )
		{ redrawExternalItemOld= 1;	}
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( otherRoot )
		{ redrawExternalItemOld= 1;	}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    if  ( otherRoot )
		{ redrawExternalItemOld= 1;	}
	    break;

	default:
	    LDEB(dsOld.dsSelectionScope.ssInExternalItem);
	    break;
	}

    /******/
    switch( rootBiSet->biInExternalItem )
	{
	case DOCinBODY:
	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    {
	    const int		page= eiSet->eiPageSelectedUpon;
	    int			changed= 0;
	    ExternalItem *	ei= (ExternalItem *)0;
	    BufferItem *	bodySectBiSet;

	    if  ( otherRoot )
		{ redrawExternalItemSet= 1;	}

	    if  ( docGetHeaderFooter( &ei, &bodySectBiSet, dsSet, bd,
					    rootBiSet->biInExternalItem ) )
		{ LDEB(rootBiSet->biInExternalItem);	}

	    /*
	    page= docSectionHeaderFooterFirstPage( &exists, bodySectBiSet,
					rootBiSet->biInExternalItem, dp );

	    if  ( page < 0 || ! exists )
		{ LLDEB(page,exists); return;	}
	    */

	    if  ( tedCheckPageOfSelectedExtItem( &changed, &drExternalSet,
							    bd, eiSet, add ) )
		{ LDEB(1);	}

	    if  ( changed )
		{ redrawExternalItemSet= 1;	}

	    if  ( redrawExternalItemSet )
		{
		if  ( docGetExternalItemBox( &drExternalSet, bodySectBiSet,
					    eiSet, justUsed, page, bd, add ) )
		    {
		    LDEB(rootBiSet->biInExternalItem);
		    redrawExternalItemSet= 0;
		    }
		}
	    break;
	    }

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( otherRoot )
		{
		BufferItem *		bodySectBiSet= (BufferItem *)0;
		ExternalItem *		ei= (ExternalItem *)0;
		const int		page= eiSet->eiPageFormattedFor;

		redrawExternalItemSet= 1;

		if  ( docGetExternalItem( &ei, &bodySectBiSet, bd, rootBiSet ) )
		    { LDEB(1); return;	}

		if  ( docGetExternalItemBox( &drExternalSet, bodySectBiSet,
					    eiSet, justUsed, page, bd, add ) )
		    {
		    LDEB(rootBiSet->biInExternalItem);
		    redrawExternalItemSet= 0;
		    }
		}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    {
	    const int		page= eiSet->eiPageSelectedUpon;
	    int			changed= 0;
	    BufferItem * const	bodySectBiSet= (BufferItem *)0;

	    if  ( otherRoot )
		{ redrawExternalItemSet= 1;	}

	    if  ( tedCheckPageOfSelectedExtItem( &changed, &drExternalSet,
							bd, eiSet, add ) )
		{ LDEB(1);	}

	    if  ( changed )
		{ redrawExternalItemSet= 1;	}

	    if  ( docGetExternalItemBox( &drExternalSet, bodySectBiSet,
					eiSet, justUsed, page, bd, add ) )
		{
		LDEB(rootBiSet->biInExternalItem);
		redrawExternalItemSet= 0;
		}
	    }
	    break;

	default:
	    LDEB(rootBiSet->biInExternalItem);
	    break;
	}

    /*  A  */
    dsNew= *dsSet;
    docBalanceFieldSelection( &beginMoved, &endMoved, td->tdDocument,
					    &(dsNew.dsBegin), &(dsNew.dsEnd) );

    hadSelection= tedHasSelection( td );
    if  ( hadSelection )
	{
	/*  1  */
	int	hadIBarSelection= tedHasIBarSelection( td );

	drExpose= sgOld.sgRectangle;

	if  ( redrawExternalItemOld )
	    { docUnionRectangle( &drExpose, &drExpose, &(add->addBackRect) ); }

	/*  2  */
	if  ( hadIBarSelection )
	    { tedStopCursorBlink( ed ); }

	appDocExposeRectangle( ed, &drExpose, *pScrolledX, *pScrolledY );
	}

    /*  5  */
    if  ( dsNew.dsBegin.dpBi->biParaInTable			&&
	  dsNew.dsEnd.dpBi->biParaInTable			&&
	  dsNew.dsBegin.dpBi->biParent->biParent ==
			dsNew.dsEnd.dpBi->biParent->biParent	)
	{
	dsNew.dsCol0= dsNew.dsBegin.dpBi->biParent->biNumberInParent;
	dsNew.dsCol1= dsNew.dsEnd.dpBi->biParent->biNumberInParent;
	}

    /*  5  */
    tedSelectionGeometry( &sgNew, &dsNew, bd, add );
    td->tdVisibleSelectionCopied= 0;
    td->tdDocumentSelection= dsNew;
    td->tdSelectionGeometry= sgNew;

    /*  3  */
    tedScrollToSelection( ed, pScrolledX, pScrolledY );

    if  ( hadSelection )
	{
	/*  4  */
	drExpose= sgOld.sgRectangle;

	if  ( redrawExternalItemOld )
	    { docUnionRectangle( &drExpose, &drExpose, &(add->addBackRect) ); }

	appDocExposeRectangle( ed, &drExpose, *pScrolledX, *pScrolledY );
	}

    /*  6  */
    drExpose= sgNew.sgRectangle;

    if  ( redrawExternalItemSet )
	{ docUnionRectangle( &drExpose, &drExpose, &drExternalSet ); }

    appDocExposeRectangle( ed, &drExpose, *pScrolledX, *pScrolledY );

    if  ( tedHasIBarSelection( td ) )
	{ tedSetCurrentTextAttribute( td, 1, &(dsNew.dsBegin) ); }
    else{ tedSetCurrentTextAttribute( td, 0, &(dsNew.dsBegin) ); }

    if  ( ea->eaFindTool )
	{ appFindToolDisableReplace( ea->eaFindTool );	}

    return;
    }

void tedSetSelectedPosition(	EditDocument *			ed,
				const DocumentPosition *	dpSet,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    DocumentSelection		dsNew;

    docInitDocumentSelection( &dsNew );

    docSetIBarSelection( &dsNew, dpSet );

    tedSetSelection( ed, &dsNew, pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Set an I Bar selection and cause the I Bar to be drawn.		*/
/*									*/
/************************************************************************/

int tedSetIBarSelection(		EditDocument *		ed,
					BufferItem *		bi,
					int			stroff,
					int *			pScrolledX,
					int *			pScrolledY )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    DocumentPosition	dp;

    const int		lastOne= 1;

    td->tdVisibleSelectionCopied= 0;

    if  ( td->tdObjectSelected )
	{ tedHideObjectWindows( ed );	}

    if  ( docSetDocumentPosition( &dp, bi, stroff, lastOne ) )
	{ LDEB(stroff); return -1;	}

    tedSetSelectedPosition( ed, &dp, pScrolledX, pScrolledY );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  'Select All' from the 'Edit' menu.					*/
/*									*/
/************************************************************************/

void tedDocSelAll(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( tedGetSelection( &dsNew, &sg, td ) )
	{ docInitDocumentSelection( &dsNew );	}

    if  ( docSelectAll( &dsNew, bd ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }


/************************************************************************/
/*									*/
/*  Change the selection to cover whole paragraphs.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedSelectWholeParagraph(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferItem *		bi;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( tedGetSelection( &dsNew, &sg, td ) )
	{ LDEB(1); return -1;	}

    bi= dsNew.dsBegin.dpBi;
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    if  ( direction > 0 )
	{ bi= docNextParagraph( bi );	}
    if  ( direction < 0 )
	{ bi= docPrevParagraph( bi );	}

    if  ( ! bi )
	{ return -1;	}

    if  ( direction == 0 )
	{ direction= 1;	}

    docInitDocumentSelection( &dsNew );

    docSetParaSelection( &dsNew, bi, direction, 0, bi->biParaStrlen );

    tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedSelectWholeSection(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;

    int				ret;

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( tedGetSelection( &dsNew, &sg, td ) )
	{ LDEB(1); return -1;	}

    ret= docSelectWholeSection( &dsNew, bd, direction );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return -1;	}

    tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

void tedDocTableSelectTableRectangle(	EditDocument *		ed,
					const TableRectangle *	tr )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    BufferDocument *	bd= td->tdDocument;

    DocumentSelection	dsNew;
    SelectionGeometry	sg;

    BufferItem *	selSectBi;

    int			scrolledX= 0;
    int			scrolledY= 0;

    if  ( tedGetSelection( &dsNew, &sg, td  ) )
	{ LDEB(1); return;	}

    if  ( docTableRectangleSelection( &dsNew, &selSectBi, bd, tr ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

void tedDocTableSelectTable(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    DocumentSelection	ds;
    SelectionGeometry	sg;

    TableRectangle	tr;

    if  ( tedGetSelection( &ds, &sg, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeTable( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

void tedDocTableSelectRow(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    DocumentSelection	ds;
    SelectionGeometry	sg;

    TableRectangle	tr;

    if  ( tedGetSelection( &ds, &sg, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeRows( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

void tedDocTableSelectColumn(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    DocumentSelection	ds;
    SelectionGeometry	sg;

    TableRectangle	tr;

    if  ( tedGetSelection( &ds, &sg, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeColumns( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

/************************************************************************/
/*									*/
/*  Is a posintion in a hyperlink,bookmark,field?			*/
/*									*/
/************************************************************************/

static void tedPositionEnvironment(
				const BufferDocument *		bd,
				const DocumentPosition *	dp,
				int *				pInExtIt,
				int *				pInBody,
				int *				pInHeadFoot,
				int *				pInTable,
				int *				pInTableHeader,
				int *				pInHyperlink,
				int *				pInField,
				int *				pInBookmark )
    {
    int				i;

    int				startPart;
    int				endPart;
    const char *		markName;
    int				markSize;

    const char *		fileName;
    int				fileSize;

    const BufferItem *		rowBi= dp->dpBi;

    while( rowBi && rowBi->biLevel != DOClevROW )
	{ rowBi= rowBi->biParent;	}

    *pInTable= dp->dpBi->biParaInTable != 0;
    *pInExtIt= dp->dpBi->biInExternalItem;
    *pInBody= *pInExtIt == DOCinBODY;
    *pInHeadFoot= 0;

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	if  ( *pInExtIt == DOC_HeaderScopes[i] )
	    { *pInHeadFoot= 1; break;	}
	if  ( *pInExtIt == DOC_FooterScopes[i] )
	    { *pInHeadFoot= 1; break;	}
	}

    *pInTableHeader= 0;
    *pInHyperlink= 0;
    *pInField= 0;
    *pInBookmark= 0;

    if  ( rowBi					&&
	  rowBi->biRowHasTableParagraphs	&&
	  rowBi->biRowIsTableHeader		)
	{ *pInTableHeader= 1;	}

    if  ( ! docGetHyperlinkForPosition( bd, dp, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{ *pInField= *pInHyperlink= 1;	}
    else{
	*pInField= docParticuleInField( dp->dpBi, dp->dpParticule );
	}


    if  ( ! docGetBookmarkForPosition( bd, dp, &startPart, &endPart,
						    &markName, &markSize ) )
	{ *pInBookmark= 1;	}

    return;
    }


/************************************************************************/
/*									*/
/*  Adapt tools and rulers to the current position.			*/
/*									*/
/************************************************************************/

static void tedAdaptToolsToPosition(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentPosition *	dp= &(td->tdDocumentSelection.dsBegin);

    int				inHyperlink;
    int				inField;
    int				inTable;
    int				inBookmark;
    int				canReplace;
    int				inExtIt;
    int				inBody;
    int				inHeadFoot;
    int				inTableHeader;

    if  ( ! ed->edIsReadonly )
	{ td->tdCanReplaceSelection= 1;	}

    canReplace= td->tdCanReplaceSelection;

    tedPositionEnvironment( bd, dp,
		&inExtIt, &inBody, &inHeadFoot, &inTable,
		&inTableHeader, &inHyperlink, &inField, &inBookmark );

    tedDocAdaptHorizontalRuler( ed, dp->dpBi );

    tedAdaptFontIndicatorsToSelection( ea, ed );

    appGuiEnableWidget( td->tdCopyWidget, 0 );
    appGuiEnableWidget( td->tdCutWidget, 0 );
    appGuiEnableWidget( td->tdPasteWidget, canReplace );
    appGuiEnableWidget( td->tdInsPictOption, canReplace );
    appGuiEnableWidget( td->tdInsFileOption, canReplace );

    appGuiEnableWidget( td->tdInsSymbolOption, canReplace );
    appGuiEnableWidget( td->tdFormatOneParaOption, 0 );
    appGuiEnableWidget( td->tdInsHyperlinkOption, inHyperlink );
    appGuiEnableWidget( td->tdInsBookmarkOption, inBookmark || ! inField );

    appGuiEnableWidget( td->tdInsInsertFootnoteOption,
				    canReplace			&&
				    inBody			&&
				    ! inTableHeader		&&
				    ! inField			);

    appGuiEnableWidget( td->tdInsInsertEndnoteOption,
				    canReplace			&&
				    inBody			&&
				    ! inField			);

    appGuiEnableWidget( td->tdInsInsertChftnsepOption,
				    canReplace			&&
				    inExtIt == DOCinFTNSEP	);

    appGuiEnableWidget( td->tdInsInsertTableOption, canReplace && ! inTable );

    appGuiEnableWidget( td->tdInsInsertPageNumberOption, 
					    canReplace && inHeadFoot );

    appGuiEnableWidget( td->tdInsInsertLineBreakOption, canReplace );
    appGuiEnableWidget( td->tdInsInsertPageBreakOption,
					canReplace && inBody && ! inTable );
    appGuiEnableWidget( td->tdInsInsertSectBreakOption,
					canReplace && inBody && ! inTable );

    appGuiEnableWidget( td->tdTabInsertTableOption, ! inTable );
    appGuiEnableWidget( td->tdTabAddRowOption, inTable );
    appGuiEnableWidget( td->tdTabAddColumnOption, inTable );

    appGuiEnableWidget( td->tdSelectTableWidget, inTable );
    appGuiEnableWidget( td->tdSelectRowWidget, inTable );
    appGuiEnableWidget( td->tdSelectColumnOption, inTable );

    if  ( tar->tarInspector )
	{
	tedFormatToolAdaptToSelection( tar->tarInspector, bd,
					    &(td->tdDocumentSelection),
					    &(td->tdSelectionGeometry),
					    ed->edFileReadOnly );
	}

    return;
    }

void tedAdaptToolsToSelection(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				inHyperlink;
    int				inField;
    int				inTable;
    int				inBookmark;

    const DocumentPosition *	dpBegin= &(td->tdDocumentSelection.dsBegin);
    const DocumentPosition *	dpEnd= &(td->tdDocumentSelection.dsEnd);

    TableRectangle		tr;

    int				inExtIt;
    int				inBody;
    int				inHeadFoot;
    int				inTableHeader;
    int				canReplace;
    int				tableRectangle= 0;
    int				oneParagraph;
    int				tableSlice= 0;

    if  ( tedHasIBarSelection( td ) )
	{ tedAdaptToolsToPosition( ed ); return; }

    oneParagraph= dpBegin->dpBi == dpEnd->dpBi;

    tedPositionEnvironment( bd, dpBegin,
			&inExtIt, &inBody, &inHeadFoot, &inTable,
			&inTableHeader, &inHyperlink, &inField, &inBookmark );

    if  ( ! docGetTableRectangle( &tr, &(td->tdDocumentSelection) ) )
	{
	int	isRowSlice= 0;
	int	isColSlice= 0;

	if  ( ! docGetTableSliceSelection( &isRowSlice, &isColSlice,
					    &tr, &(td->tdDocumentSelection) ) )
	    { tableSlice= 1;	}

	tableRectangle= 1;
	}

    tedDocAdaptHorizontalRuler( ed, dpBegin->dpBi );

    tedAdaptFontIndicatorsToSelection( ea, ed );

    appGuiEnableWidget( td->tdFormatOneParaOption,
			    ! oneParagraph &&
			docSelectionInsideCell( &(td->tdDocumentSelection) ) );

    appGuiEnableWidget( td->tdCopyWidget, 1 );

    if  ( ed->edIsReadonly )
	{ td->tdCanReplaceSelection= 0;	}
    else{
	td->tdCanReplaceSelection=
		! tableRectangle ||
		( tr.trCol0 == tr.trCol1 && tr.trRow0 == tr.trRow1 );
	}

    canReplace= td->tdCanReplaceSelection;

    appGuiEnableWidget( td->tdCutWidget, canReplace || tableSlice );
    appGuiEnableWidget( td->tdPasteWidget, canReplace );

    appGuiEnableWidget( td->tdInsPictOption, canReplace );
    appGuiEnableWidget( td->tdInsFileOption, canReplace );
    appGuiEnableWidget( td->tdInsSymbolOption, canReplace );
    appGuiEnableWidget( td->tdInsHyperlinkOption, oneParagraph );
    appGuiEnableWidget( td->tdInsBookmarkOption,
				    oneParagraph	&&
				    ( inBookmark || ! inField ) );

    appGuiEnableWidget( td->tdInsInsertFootnoteOption,
				    canReplace		&&
				    oneParagraph	&&
				    inBody		&&
				    ! inTableHeader	&&
				    ! inField		);

    appGuiEnableWidget( td->tdInsInsertEndnoteOption,
				    canReplace		&&
				    oneParagraph	&&
				    inBody		&&
				    ! inField		);

    appGuiEnableWidget( td->tdInsInsertChftnsepOption,
				    canReplace			&&
				    inExtIt == DOCinFTNSEP	);

    appGuiEnableWidget( td->tdInsInsertTableOption, 0 );

    appGuiEnableWidget( td->tdInsInsertPageNumberOption, 
				    canReplace && oneParagraph && inHeadFoot );

    appGuiEnableWidget( td->tdTabInsertTableOption, 0 );

    appGuiEnableWidget( td->tdInsInsertLineBreakOption, 0 );
    appGuiEnableWidget( td->tdInsInsertPageBreakOption, 0 );
    appGuiEnableWidget( td->tdInsInsertSectBreakOption, 0 );

    appGuiEnableWidget( td->tdTabAddColumnOption, tableRectangle );
    appGuiEnableWidget( td->tdTabAddRowOption, tableRectangle );

    appGuiEnableWidget( td->tdSelectTableWidget, tableRectangle );
    appGuiEnableWidget( td->tdSelectRowWidget, tableRectangle );
    appGuiEnableWidget( td->tdSelectColumnOption, tableRectangle );

    if  ( tar->tarInspector )
	{
	tedFormatToolAdaptToSelection( tar->tarInspector, bd,
					    &(td->tdDocumentSelection),
					    &(td->tdSelectionGeometry),
					    ed->edFileReadOnly );
	}
    }
