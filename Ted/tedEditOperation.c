/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docLayout.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

#   define	VALIDATE_TREE		0

/************************************************************************/
/*									*/
/*  Do the last steps that set an I-Bar selection after an editing	*/
/*  action.								*/
/*									*/
/*  1)  The order in which the size of the window is adapted and the	*/
/*	window is scrolled to the selection depends on the direction	*/
/*	of the change, as the scrollbars complain when the corrent	*/
/*	position is not in range.					*/
/*									*/
/************************************************************************/

int tedEditFinishIBarSelection(	EditDocument *			ed,
				EditOperation *			eo,
				BufferItem *			bi,
				int				stroff )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); /* return -1; */	}

    /*  1  */
    if  ( add->addBackRect.drY1 > eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    if  ( bi )
	{ tedSetIBarSelection( ed, bi, stroff, &scrolledX, &scrolledY ); }
    else{ XDEB(bi);	}


    /*  1  */
    if  ( add->addBackRect.drY1 < eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    appDocExposeRectangle( ed, &(eo->eoChangedRectangle),
						    scrolledX, scrolledY );

    tedStartCursorBlink( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt to a formatting change.					*/
/*									*/
/************************************************************************/

int tedEditFinishSelection(		EditDocument *			ed,
					EditOperation *			eo,
					const DocumentSelection *	dsNew )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( docIsIBarSelection( dsNew ) )
	{
	return tedEditFinishIBarSelection( ed, eo,
				dsNew->dsBegin.dpBi, dsNew->dsBegin.dpStroff );
	}

    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); return -1;	}

    if  ( add->addBackRect.drY1 > eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    tedSetSelection( ed, dsNew, &scrolledX, &scrolledY );

    if  ( add->addBackRect.drY1 < eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    appDocExposeRectangle( ed, &(eo->eoChangedRectangle),
						    scrolledX, scrolledY );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the place from where redrawing after editing is to begin.	*/
/*									*/
/*  E.G. because when the first particule of the line was split, also	*/
/*	reformat the previous line.					*/
/*									*/
/************************************************************************/

/*  B  */
void tedAdjustRedrawBegin(	EditDocument *			ed,
				EditOperation *			eo,
				int *				pLine,
				int				part )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    const DocumentRectangle *	drBack= &(add->addBackRect);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    BufferItem *		paraBi;
    TextLine *			tl;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return;	}
    paraBi= ds.dsBegin.dpBi;

    tl= paraBi->biParaLines+ *pLine;

    if  ( *pLine == 0 )
	{
	if  ( ds.dsBegin.dpBi != ds.dsEnd.dpBi )
	    {
	    eo->eoChangedRectangle.drX0= drBack->drX0;
	    eo->eoChangedRectangle.drX1= drBack->drX1;

	    if  ( eo->eoChangedRectangle.drY0 > TL_TOP_PIXELS( add, tl ) )
		{ eo->eoChangedRectangle.drY0=  TL_TOP_PIXELS( add, tl ); }
	    }

	docIncludePositionInReformat( eo, paraBi, tl->tlStroff );

	return;
	}

    if  ( part == tl->tlFirstParticule		||
	  ds.dsBegin.dpBi != ds.dsEnd.dpBi	)
	{
	(*pLine)--; tl--;

	eo->eoChangedRectangle.drX0= drBack->drX0;
	eo->eoChangedRectangle.drX1= drBack->drX1;

	if  ( eo->eoChangedRectangle.drY0 > TL_TOP_PIXELS( add, tl ) )
	    { eo->eoChangedRectangle.drY0=  TL_TOP_PIXELS( add, tl ); }

	docIncludePositionInReformat( eo, paraBi, tl->tlStroff );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Start an Edit Operation.					*/
/*  2)  Finish an Edit Operation.					*/
/*									*/
/*  The reference positions in the Start and Finish calls should be	*/
/*  the same position in the document from a logical point of view.	*/
/*									*/
/************************************************************************/

/*  1  */
int tedStartEditOperation(	EditOperation *		eo,
				DocumentSelection *	ds,
				SelectionGeometry *	sg,
				EditDocument *		ed,
				int			fullWidth )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);
    EditRange *			er= &(eo->eoReformatRange);

    ExternalItem *		ei;
    BufferItem *		selRootBi;
    BufferItem *		bodySectBi;
    DocumentPosition		dp;

#   if VALIDATE_TREE
    {
    const BufferDocument *	bd= td->tdDocument;
    LDEB(1);
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
    }
#   endif

    docInitEditOperation( eo );

    if  ( tedGetSelection( ds, sg, td ) )
	{ LDEB(1); return -1;	}

    eo->eoSelectionScope= ds->dsSelectionScope;

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi,
				&bodySectBi, bd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &dp, selRootBi ) )
	{ LDEB(1); return -1;	}
    er->erParagraphCount= docNumberOfParagraph( dp.dpBi );

    docIncludePositionInReformat( eo, ds->dsBegin.dpBi, ds->dsBegin.dpStroff );
    docIncludePositionInReformat( eo, ds->dsEnd.dpBi, ds->dsEnd.dpStroff );

    eo->eoIBarSelectionOld= tedHasIBarSelection( td );
    eo->eoMultiParagraphSelectionOld=
				ds->dsEnd.dpBi != ds->dsBegin.dpBi;

    eo->eoChangedRectangle= sg->sgRectangle;
    eo->eoChangedRectangle.drX1= add->addBackRect.drX1;

    if  ( fullWidth )
	{ eo->eoChangedRectangle.drX0= add->addBackRect.drX0; }

    eo->eoOldBackY1= add->addBackRect.drY1;

    /**/
    eo->eoBd= bd;
    eo->eoVoidadd= (void *)&(ed->edDrawingData);
    eo->eoCloseObject= tedCloseObject;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Widen redraw rectangle to whole BufferItems.			*/
/*									*/
/************************************************************************/

int tedEditIncludeItemInRedraw(	EditOperation *		eo,
				EditDocument *		ed,
				const BufferItem *	bi )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    eo->eoChangedRectangle.drX0= add->addBackRect.drX0;
    eo->eoChangedRectangle.drX1= add->addBackRect.drX1;
    eo->eoChangedRectangle.drY0= BI_TOP_PIXELS( add, bi )- 1;
    eo->eoChangedRectangle.drY1= BI_BELOW_PIXELS( add, bi )- 1;

    if  ( bi->biLevel == DOClevROW	&&
	  bi->biRowHasTableParagraphs	)
	{
	eo->eoChangedRectangle.drY1=
		    LP_YPIXELS( add, &(bi->biRowBelowAllPosition) )- 1;
	}

    return 0;
    }

int tedEditIncludeRowsInRedraw(	EditOperation *		eo,
				EditDocument *		ed,
				const BufferItem *	sectBi,
				int			row0,
				int			row1 )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    const BufferItem *		rowBi;

    eo->eoChangedRectangle.drX0= add->addBackRect.drX0;
    eo->eoChangedRectangle.drX1= add->addBackRect.drX1;

    rowBi= sectBi->biChildren[row0];
    eo->eoChangedRectangle.drY0= BI_TOP_PIXELS( add, rowBi )- 1;

    rowBi= sectBi->biChildren[row1];
    eo->eoChangedRectangle.drY1=
		    LP_YPIXELS( add, &(rowBi->biRowBelowAllPosition) )- 1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo layout of the relevant part of the document after editing.	*/
/*									*/
/************************************************************************/

int tedEditRefreshLayout(	EditOperation *		eo,
				EditDocument *		ed )
    {
    EditRange *			er= &(eo->eoReformatRange);

    AppDrawingData *		add= &(ed->edDrawingData);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    RecalculateFields		rf;

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= eo->eoFieldUpdate;
    rf.rfFieldsUpdated= 0;

    if  ( eo->eoNotesDeleted > 0 || eo->eoNotesAdded > 0 )
	{
	int			changed= 0;

	docRenumberNotes( &changed, bd );

	if  ( changed )
	    { rf.rfUpdateFlags |= FIELDdoCHFTN;	}
	}

    if  ( rf.rfUpdateFlags )
	{
	if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	    { LDEB(1);	}
	}

    if  ( rf.rfFieldsUpdated > 0 )
	{
	if  ( tedLayoutDocumentTree( td, add ) )
	    { LDEB(1); return -1;	}

	docUnionRectangle( &(eo->eoChangedRectangle),
			    &(eo->eoChangedRectangle), &(add->addBackRect) );
	}
    else{
	ExternalItem *		ei;
	BufferItem *		selRootBi;
	BufferItem *		bodySectBi;

	const SelectionScope *	ss;

	ss= &(eo->eoSelectionScope);

	if  ( docGetRootOfSelectionScope( &ei, &selRootBi,
						    &bodySectBi, bd, ss ) )
	    { LDEB(1); return -1;	}

	if  ( eo->eoNotesDeleted > 0 || eo->eoNotesAdded > 0 )
	    {
	    if  ( tedLayoutItem( &(bd->bdItem), bd, add,
						&(eo->eoChangedRectangle) ) )
		{ LDEB(eo->eoNotesDeleted); return -1;	}

	    if  ( tedOpenItemObjects( &(bd->bdItem), &(ed->edColors), add ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( eo->eoParaAdjustParagraphNumber >= 0 )
		{
		int		paraNr= eo->eoParaAdjustParagraphNumber;
		BufferItem *	paraBi;

		if  ( er->erStartPosition.epParagraphNumber != paraNr )
		    { LLDEB(er->erStartPosition.epParagraphNumber,paraNr); }

		if  ( er->erEndPosition.epParagraphNumber != paraNr )
		    { LLDEB(er->erEndPosition.epParagraphNumber,paraNr); }

		paraBi= docGetParagraphByNumber( selRootBi, paraNr );

		if  ( tedAdjustParagraphLayout( eo, add, paraBi,
					    eo->eoParaAdjustFromLine,
					    eo->eoParaAdjustStroffShift,
					    eo->eoParaAdjustStroffUpto ) )
		    { LDEB(eo->eoParaAdjustFromLine); }

		if  ( tedOpenItemObjects( paraBi, &(ed->edColors), add ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		DocumentSelection	dsLayout;
		BufferItem *		startBi;
		BufferItem *		endBi;

		if  ( er->erStartPosition.epParagraphNumber == 0 )
		    {
		    LDEB(er->erStartPosition.epParagraphNumber);
		    er->erStartPosition.epParagraphNumber= 1;
		    }

		startBi= docGetParagraphByNumber( selRootBi,
				er->erStartPosition.epParagraphNumber );
		endBi= docGetParagraphByNumber( selRootBi,
				er->erEndPosition.epParagraphNumber );

		if  ( ! startBi || ! endBi )
		    {
		    LXDEB(er->erStartPosition.epParagraphNumber,startBi);
		    LXDEB(er->erEndPosition.epParagraphNumber,endBi);
		    return -1;
		    }

		docInitDocumentSelection( &dsLayout );
		dsLayout.dsBegin.dpBi= startBi;
		dsLayout.dsBegin.dpParticule= 0;
		dsLayout.dsBegin.dpStroff= 0;

		dsLayout.dsEnd.dpBi= endBi;
		dsLayout.dsEnd.dpParticule= endBi->biParaParticuleCount- 1;
		dsLayout.dsEnd.dpStroff= endBi->biParaStrlen;

		selRootBi= docGetSelectionRoot( &ei, &bodySectBi,
							    bd, &dsLayout );
		if  ( ! selRootBi )
		    { XDEB(selRootBi); return -1;	}

		while( selRootBi && selRootBi->biLevel > er->erBottomLevel )
		    { selRootBi= selRootBi->biParent;	}
		if  ( ! selRootBi )
		    { XDEB(selRootBi); return -1;	}

		if  ( selRootBi->biInExternalItem != DOCinBODY	&&
		      ! selRootBi->biParent			)
		    {
		    int				page;
		    ExternalItem *		ei= (ExternalItem *)0;
		    int				y1Twips;

		    if  ( selRootBi->biLevel != DOClevSECT )
			{
			SDEB(docLevelStr(selRootBi->biLevel));
			return -1;
			}

		    page= selRootBi->biSectHeaderFooterUseForPage;

		    {
		    BufferItem *	bodySectBi= (BufferItem *)0;

		    if  ( docGetExternalItem( &ei, &bodySectBi, bd,
								selRootBi ) )
			{ LDEB(selRootBi->biInExternalItem); return -1;	}
		    }

		    tedLayoutExternalItem( &y1Twips, ei,
					    page, ei->eiY0UsedTwips, bd, add,
					    &(eo->eoChangedRectangle) );

		    if  ( tedOpenItemObjects( ei->eiItem,
						    &(ed->edColors), add ) )
			{ LDEB(1); return -1;	}
		    }
		else{
		    if  ( tedLayoutItem( selRootBi, bd,
					    add, &(eo->eoChangedRectangle) ) )
			{ LDEB(1);	}

		    if  ( tedOpenItemObjects( selRootBi,
						    &(ed->edColors), add ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }
	}

    return 0;
    }

