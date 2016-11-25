/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
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

/************************************************************************/
/*									*/
/*  Change the attributes of the selection.				*/
/*									*/
/*  A)  Exclude properties that are derived from a list from the task.	*/
/*  1)  Adjust the start line for recalculating the layout of the first	*/
/*	paragraph in the selection.					*/
/*  2)  If necessary, split the first particule to change the text	*/
/*	attributes of the second half.					*/
/*  3)  For all paragraphs in the selection upto the last one..		*/
/*  4)  Change the text attributes.					*/
/*									*/
/************************************************************************/

static int tedChangeParaProperties( EditOperation *		eo,
				    PropertyMask *		pPpChgMask,
				    PropertyMask *		pTaChgMask,
				    BufferItem *		paraBi,
				    int				partFrom,
				    int				partUpto,
				    AppDrawingData *		add,
				    const PostScriptFontList *	psfl,
				    BufferDocument *		bd,
				    DocumentFontList *		dfl,
				    const PropertyMask *	taSetMask,
				    const TextAttribute *	taSet,
				    const PropertyMask *	ppSetMask,
				    const ParagraphProperties *	ppNew )
    {
    PropertyMask		ppChgMask;
    PropertyMask		ppSetMaskThisPara;
    PropertyMask		taDoneMask;

    ScreenFontList *		sfl= eo->eoScreenFontList;

    ppSetMaskThisPara= *ppSetMask;

    PROPmaskCLEAR( &ppChgMask );
    PROPmaskCLEAR( &taDoneMask );

    /*  A  */
    if  ( paraBi->biParaListOverride > 0 )
	{
	int		override= -1;

	int		bulletFieldNr= -1;
	int		bulletPartBegin= -1;
	int		bulletPartEnd= -1;
	int		bulletStroffBegin= -1;
	int		bulletStroffEnd= -1;

	override= paraBi->biParaProperties.ppListOverride;

	if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd, paraBi, bd ) )
	    { LDEB(1);	}

	if  ( partFrom <= bulletPartEnd )
	    { partFrom= bulletPartEnd+ 1;	}

	PROPmaskUNSET( &ppSetMaskThisPara, PPpropLEFT_INDENT );
	PROPmaskUNSET( &ppSetMaskThisPara, PPpropFIRST_INDENT );
	}

    if  ( ! PROPmaskISEMPTY( taSetMask ) )
	{
	if  ( tedChangeParticuleAttributes( &taDoneMask, add, sfl, bd,
						paraBi, partFrom, partUpto,
						taSet, taSetMask ) )
	    { LLDEB(partFrom,partUpto); return -1;	}
	}

    if  ( ! PROPmaskISEMPTY( &ppSetMaskThisPara ) )
	{
	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	if  ( docEditUpdParaProperties( eo, &ppChgMask, paraBi,
						&ppSetMaskThisPara, ppNew,
						colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}
	}

    *pTaChgMask= taDoneMask;
    *pPpChgMask= ppChgMask;
    return 0;
    }

static int tedChangeSectionProperties(	PropertyMask *		pSpChgMask,
					int *			pRedraw,
					BufferItem *		sectBi,
					const PropertyMask *	spSetMask,
					const SectionProperties * spNew )
    {
    PropertyMask	spChgMask;

    PROPmaskCLEAR( &spChgMask );

    if  ( docUpdSectProperties( &spChgMask,
			    &(sectBi->biSectProperties), spSetMask, spNew ) )
	{ XDEB(spSetMask); return -1;	}

    if  ( sectBi->biInExternalItem == DOCinBODY )
	{
	if  ( PROPmaskISSET( &spChgMask, DGpropPAGE_WIDTH )	||
	      PROPmaskISSET( &spChgMask, DGpropLEFT_MARGIN )	||
	      PROPmaskISSET( &spChgMask, DGpropRIGHT_MARGIN )	)
	    {
	    PropertyMask	spWideMask;

	    PROPmaskCLEAR( &spWideMask );

	    PROPmaskADD( &spWideMask, DGpropPAGE_WIDTH );
	    PROPmaskADD( &spWideMask, DGpropLEFT_MARGIN );
	    PROPmaskADD( &spWideMask, DGpropRIGHT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( &spChgMask, SPpropTITLEPG )		||
	  PROPmaskISSET( &spChgMask, SPpropNUMBER_STYLE )	||
	  PROPmaskISSET( &spChgMask, SPpropPAGE_RESTART )	||
	  PROPmaskISSET( &spChgMask, SPpropSTART_PAGE )		)
	{ *pRedraw= 1;	}

    if  ( PROPmaskISSET( &spChgMask, SPpropNUMBER_STYLE )	||
	  PROPmaskISSET( &spChgMask, SPpropPAGE_RESTART )	||
	  PROPmaskISSET( &spChgMask, SPpropSTART_PAGE )		||
	  PROPmaskISSET( &spChgMask, DGpropPAGE_WIDTH )		||
	  PROPmaskISSET( &spChgMask, DGpropLEFT_MARGIN )	||
	  PROPmaskISSET( &spChgMask, DGpropRIGHT_MARGIN )	)
	{
	sectBi->biSectHeader.eiPageFormattedFor= -1;
	sectBi->biSectFirstPageHeader.eiPageFormattedFor= -1;
	sectBi->biSectLeftPageHeader.eiPageFormattedFor= -1;
	sectBi->biSectRightPageHeader.eiPageFormattedFor= -1;

	sectBi->biSectFooter.eiPageFormattedFor= -1;
	sectBi->biSectFirstPageFooter.eiPageFormattedFor= -1;
	sectBi->biSectLeftPageFooter.eiPageFormattedFor= -1;
	sectBi->biSectRightPageFooter.eiPageFormattedFor= -1;
	}

    utilPropMaskOr( pSpChgMask, pSpChgMask, &spChgMask );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Use the new ruler settings, or other geometry properties of a	*/
/*  BufferItem.								*/
/*									*/
/************************************************************************/

static void tedApplyItemFormat(	EditOperation *			eo,
				BufferItem *			bi,
				const AppDrawingData *		add )
    {
    DocumentRectangle	drLocal;

    drLocal.drX0= add->addBackRect.drX0;
    drLocal.drX1= add->addBackRect.drX1;
    drLocal.drY0= BI_TOP_PIXELS( add, bi );
    drLocal.drY1= BI_BELOW_PIXELS( add, bi );

    if  ( bi->biLevel == DOClevROW	&&
	  bi->biRowHasTableParagraphs	)
	{ drLocal.drY1= LP_YPIXELS( add, &(bi->biRowBelowAllPosition) ); }

    docIncludeRectangleInChange( eo, &drLocal );

    docEditIncludeItemInReformatRange( eo, bi );

    return;
    }

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int tedChangeSelectionProperties( EditDocument *		ed,
				const PropertyMask *		taSetMaskCall,
				const TextAttribute *		taSet,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    ScreenFontList *		sfl= &(td->tdScreenFontList);
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);

    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    int				part;
    int				part1;

    TextParticule *		tp;

    BufferItem *		paraBi;
    BufferItem *		sectBi= (BufferItem *)0;

    int				firstParticuleSplit= 0;

    PropertyMask		taSetMask;

    PropertyMask		selTaChgMask;
    PropertyMask		selPpChgMask;
    PropertyMask		selSpChgMask;

    PropertyMask		paraTaChgMask;
    PropertyMask		paraPpChgMask;
    PropertyMask		sectSpChgMask;

    PropertyMask		pmSplit;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    EditOperation		eo;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    taSetMask= *taSetMaskCall;

    PROPmaskCLEAR( &selTaChgMask );
    PROPmaskCLEAR( &selPpChgMask );
    PROPmaskCLEAR( &selSpChgMask );

    if  ( ! ds.dsBegin.dpBi || ! ds.dsEnd.dpBi )
	{ XXDEB(ds.dsBegin.dpBi,ds.dsEnd.dpBi); return -1; }

    if  ( docIsIBarSelection( &ds ) )
	{ PROPmaskCLEAR( &taSetMask );	}

    paraBi= ds.dsBegin.dpBi;
    if  ( docFindParticuleOfPosition( &part, &(ds.dsBegin), 1 ) )
	{ LDEB(ds.dsBegin.dpStroff); return -1;	}
    if  ( docFindParticuleOfPosition( &part1, &(ds.dsEnd), 0 ) )
	{ LDEB(ds.dsEnd.dpStroff); return -1;	}

    /*  0  */
    if  ( PROPmaskISSET( &taSetMask, TApropDOC_FONT_NUMBER ) )
	{
	DocumentFontList *	dfl= &(dp->dpFontList);
	DocumentFont *		df;

	if  ( taSet->taFontNumber < 0			||
	      taSet->taFontNumber >= dfl->dflFontCount	)
	    { LLDEB(taSet->taFontNumber,dfl->dflFontCount); return -1; }

	df= dfl->dflFonts+ taSet->taFontNumber;
	df->dfUsed= 1;
	}

    /*  1  */
    tp= paraBi->biParaParticules+ part;

    /*  2  */
    PROPmaskCLEAR( &pmSplit );
    if  ( (unsigned)ds.dsBegin.dpStroff > tp->tpStroff )
	{
	TextAttribute	ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	utilAttributeDifference( &pmSplit, &ta, taSet, &taSetMask );
	}

    if  ( ! PROPmaskISEMPTY( &pmSplit ) )
	{
	int		stroff= ds.dsBegin.dpStroff;

	if  ( (unsigned)stroff < tp->tpStroff+ tp->tpStrlen )
	    {
	    TextParticule *	oldTp;

	    if  ( docSplitTextParticule( &oldTp, &tp, paraBi, part, stroff ) )
		{ LDEB(part); return -1;	}

	    part++;

	    if  ( ds.dsEnd.dpBi == paraBi )
		{ firstParticuleSplit= 1;	}
	    }
	else{ tp++; part++; }
	}

    PROPmaskCLEAR( &sectSpChgMask );

    sectBi= paraBi->biParent->biParent->biParent;
    if  ( ! PROPmaskISEMPTY( spSetMask ) )
	{
	int	redraw= 0;

	if  ( tedChangeSectionProperties( &sectSpChgMask, &redraw, sectBi,
							spSetMask, spNew ) )
	    { XDEB(spSetMask); return -1;	}

	if  ( redraw )
	    { docIncludeRectangleInChange( &eo, &(add->addBackRect) ); }

	utilPropMaskOr( &selSpChgMask, &selSpChgMask, &sectSpChgMask );
	}

    /*  3  */
    while( paraBi != ds.dsEnd.dpBi )
	{
	int	col;
	int	partUpto= paraBi->biParaParticuleCount;

	col= paraBi->biParent->biNumberInParent;

	if  ( ds.dsCol0 < 0					||
	      ds.dsCol1 < 0					||
	      ( col >= ds.dsCol0 && col <= ds.dsCol1 )	)
	    {
	    /*  4  */
	    if  ( tedChangeParaProperties( &eo, &paraPpChgMask, &paraTaChgMask,
				    paraBi, part, partUpto, add, psfl, bd, dfl,
				    &taSetMask, taSet, ppSetMask, ppNew ) )
		{ LDEB(1);	}

	    if  ( sectBi != paraBi->biParent->biParent->biParent )
		{
		if  ( ! PROPmaskISEMPTY( &sectSpChgMask ) )
		    { tedApplyItemFormat( &eo, sectBi, add );	}

		sectBi= paraBi->biParent->biParent->biParent;
		if  ( ! PROPmaskISEMPTY( spSetMask ) )
		    {
		    int		redraw= 0;

		    if  ( tedChangeSectionProperties( &sectSpChgMask, &redraw,
						sectBi, spSetMask, spNew ) )
			{ XDEB(spSetMask); return -1;	}

		    if  ( redraw )
			{
			docIncludeRectangleInChange( &eo,
						    &(add->addBackRect) );
			}

		    utilPropMaskOr( &selSpChgMask,
					    &selSpChgMask, &sectSpChgMask );
		    }
		}

	    if  ( ! PROPmaskISEMPTY( &paraPpChgMask )	||
		  ! PROPmaskISEMPTY( &paraTaChgMask )	)
		{
		tedApplyItemFormat( &eo, paraBi, add );

		if  ( ! PROPmaskISEMPTY( &paraPpChgMask )	&&
		      paraBi == ds.dsBegin.dpBi		)
		    { tedDocAdaptHorizontalRuler( ed, paraBi );	}

		utilPropMaskOr( &selTaChgMask, &selTaChgMask, &paraTaChgMask );
		utilPropMaskOr( &selPpChgMask, &selPpChgMask, &paraPpChgMask );
		}
	    }

	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { XDEB(paraBi); return -1;	}

	part= 0; firstParticuleSplit= 0;
	}

    /*  4  */
    if  ( tedChangeParaProperties( &eo, &paraPpChgMask, &paraTaChgMask, paraBi,
			part, part1+ firstParticuleSplit, add, psfl, bd, dfl,
			&taSetMask, taSet, ppSetMask, ppNew ) )
	{ LDEB(1);	}

    part= part1+ firstParticuleSplit;
    tp= paraBi->biParaParticules+ part;

    if  ( sectBi != paraBi->biParent->biParent->biParent )
	{
	if  ( ! PROPmaskISEMPTY( &sectSpChgMask ) )
	    { tedApplyItemFormat( &eo, sectBi, add ); }

	sectBi= paraBi->biParent->biParent->biParent;
	if  ( ! PROPmaskISEMPTY( spSetMask ) )
	    {
	    int		redraw= 0;

	    if  ( tedChangeSectionProperties( &sectSpChgMask, &redraw,
					    sectBi, spSetMask, spNew ) )
		{ XDEB(spSetMask); return -1;	}

	    if  ( redraw )
		{ docIncludeRectangleInChange( &eo, &(add->addBackRect) ); }

	    utilPropMaskOr( &selSpChgMask, &selSpChgMask, &sectSpChgMask );
	    }
	}

    if  ( ! PROPmaskISEMPTY( &sectSpChgMask ) )
	{ tedApplyItemFormat( &eo, sectBi, add ); }

    PROPmaskCLEAR( &pmSplit );
    if  ( part < paraBi->biParaParticuleCount )
	{
	TextAttribute	ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	utilAttributeDifference( &pmSplit, &ta, taSet, &taSetMask );
	}

    if  ( ! PROPmaskISEMPTY( &pmSplit ) )
	{
	int	strend= tp->tpStroff+ tp->tpStrlen;

	if  ( ds.dsEnd.dpStroff < strend )
	    {
	    TextParticule *	tpNext;

	    if  ( docSplitTextParticule( &tp, &tpNext,
					paraBi, part, ds.dsEnd.dpStroff ) )
		{ LDEB(1); return -1;	}
	    }

	PROPmaskCLEAR( &pmSplit );

	/*  2  */
	if  ( tedChangeParticuleAttributes( &pmSplit, add, sfl, bd,
						    paraBi, part, part+ 1,
						    taSet, &taSetMask ) )
	    { LDEB(1);	}

	utilPropMaskOr( &paraTaChgMask, &paraTaChgMask, &pmSplit );
	}

    if  ( ! PROPmaskISEMPTY( &paraPpChgMask )	||
	  ! PROPmaskISEMPTY( &paraTaChgMask )	)
	{
	tedApplyItemFormat( &eo, paraBi, add );

	if  ( ! PROPmaskISEMPTY( &paraPpChgMask )	&&
	      paraBi == ds.dsBegin.dpBi		)
	    { tedDocAdaptHorizontalRuler( ed, paraBi );	}

	utilPropMaskOr( &selPpChgMask, &selPpChgMask, &paraPpChgMask );
	utilPropMaskOr( &selTaChgMask, &selTaChgMask, &paraTaChgMask );
	}

    if  ( ! PROPmaskISEMPTY( &selPpChgMask )	||
	  ! PROPmaskISEMPTY( &selTaChgMask )	||
	  ! PROPmaskISEMPTY( &selSpChgMask )	)

    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Utility routines around tedChangeSelectionProperties():		*/
/*									*/
/************************************************************************/

int tedAppChangeSectionProperties( EditApplication *		ea,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    TextAttribute		taSet;

    PropertyMask		taUpdMask;
    PropertyMask		ppSetMask;

    PROPmaskCLEAR( &taUpdMask );
    PROPmaskCLEAR( &ppSetMask );

    utilInitTextAttribute ( &taSet );

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedChangeSelectionProperties( ed,
				    &taUpdMask, &taSet,
				    &ppSetMask, (ParagraphProperties *)0,
				    spSetMask, spNew ) )
	{ XDEB(spSetMask); return -1;	}

    return 0;
    }

int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew )
    {
    TextAttribute		taSet;

    PropertyMask		taUpdMask;
    PropertyMask		spSetMask;

    PROPmaskCLEAR( &taUpdMask );
    PROPmaskCLEAR( &spSetMask );

    utilInitTextAttribute ( &taSet );

    if  ( tedChangeSelectionProperties( ed,
				    &taUpdMask, &taSet,
				    ppSetMask, ppNew,
				    &spSetMask, (SectionProperties *)0 ) )
	{ XDEB(ppSetMask); return -1;	}

    return 0;
    }

int tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeParagraphProperties( ed, ppSetMask, ppNew );
    }

/************************************************************************/
/*									*/
/*  Adapt format tool to the circumstances.				*/
/*									*/
/************************************************************************/

void tedAdaptFormatToolToDocument(	EditDocument *	ed,
					int		choosePage )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! tar->tarInspector )
	{ return;	}

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    tedFormatToolAdaptToSelection( tar->tarInspector, ed, choosePage,
							    &ds, &sg, &sd );

    return;
    }

/************************************************************************/
/*									*/
/*  Get a suggestion about the line height: Used for the initial value	*/
/*  for space before/after in the format tool.				*/
/*									*/
/************************************************************************/

int tedGetParaLineHeight(	int *			pLineHeight,
				EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    TedDocument *		td;
    BufferDocument *		bd;
    AppDrawingData *		add;

    BufferItem *		bi;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    add= &(ed->edDrawingData);
    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    bi= ds.dsBegin.dpBi;

    if  ( docPsParagraphLineExtents( bd, psfl, bi ) )
	{ LDEB(1); return -1;	}

    *pLineHeight= bi->biParaAscentTwips- bi->biParaDescentTwips; return 0;
    }

/************************************************************************/
/*									*/
/*  Change properties of all sections.					*/
/*									*/
/*  9)  Eventually this routine must be combined with the set document	*/
/*	properties routine. Now it is called in conjunction with its	*/
/*	colleague that takes care of reformats and redraws.		*/
/*									*/
/************************************************************************/

int tedAppChangeAllSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew )
    {
    int				i;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    TextAttribute		taSet;

    PropertyMask		taUpdMask;
    PropertyMask		ppSetMask;

    PropertyMask		changed;

    PROPmaskCLEAR( &taUpdMask );
    PROPmaskCLEAR( &ppSetMask );
    PROPmaskCLEAR( &changed );

    utilInitTextAttribute ( &taSet );

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    for ( i= 0; i < bd->bdItem.biChildCount; i++ )
	{
	PropertyMask		sectChanged;
	BufferItem *		sectBi= bd->bdItem.biChildren[i];

	PROPmaskCLEAR( &sectChanged );

	if  ( docUpdSectProperties( &sectChanged,
			    &(sectBi->biSectProperties), spSetMask, spNew ) )
	    { LDEB(1);	}

	utilPropMaskOr( &changed, &changed, &sectChanged );
	}

    /*  9  */

    return 0;
    }

