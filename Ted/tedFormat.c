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
/*  1)  Adjust the start line for recalculating the layout of the first	*/
/*	paragraph in the selection.					*/
/*  2)  If necessary, split the first particule to change the text	*/
/*	attributes of the second half.					*/
/*  3)  For all paragraphs in the selection upto the last one..		*/
/*  4)  Change the text attributes.					*/
/*									*/
/************************************************************************/

static int tedChangeParaProperties( PropertyMask *		pPpChgMask,
				    PropertyMask *		pTaChgMask,
				    BufferItem *		paraBi,
				    int				partFrom,
				    int				partUpto,
				    AppDrawingData *		add,
				    BufferDocument *		bd,
				    DocumentFontList *		dfl,
				    const PropertyMask *	taSetMask,
				    const TextAttribute *	taSet,
				    const PropertyMask *	ppUpdMask,
				    const ParagraphProperties *	ppNew )
    {
    PropertyMask	ppChgMask;
    PropertyMask	taDoneMask;

    PROPmaskCLEAR( &ppChgMask );
    PROPmaskCLEAR( &taDoneMask );

    if  ( ! PROPmaskISEMPTY( taSetMask ) )
	{
	if  ( tedChangeParticuleAttributes( &taDoneMask, add, bd,
						paraBi, partFrom, partUpto,
						taSet, taSetMask ) )
	    { LLDEB(partFrom,partUpto); return -1;	}
	}

    if  ( ! PROPmaskISEMPTY( ppUpdMask ) )
	{
	if  ( docUpdParaProperties( &ppChgMask, &(paraBi->biParaProperties),
					ppUpdMask, ppNew, (const int *)0 ) )
	    { XDEB(ppUpdMask); return -1;	}
	}

    *pTaChgMask= taDoneMask;
    *pPpChgMask= ppChgMask;
    return 0;
    }

static int tedChangeSectionProperties(	PropertyMask *		pSpChgMask,
					BufferItem *		sectBi,
					const PropertyMask *	spUpdMask,
					const SectionProperties * spNew,
					DocumentRectangle *	drChanged,
					const DocumentRectangle * drBack )
    {
    PropertyMask	spChgMask;

    PROPmaskCLEAR( &spChgMask );

    if  ( docUpdSectProperties( &spChgMask,
			    &(sectBi->biSectProperties), spUpdMask, spNew ) )
	{ XDEB(spUpdMask); return -1;	}

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
	{ docUnionRectangle( drChanged, drChanged, drBack );	}

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
    DocumentRectangle * drChanged= &(eo->eoChangedRectangle);
    DocumentRectangle	drLocal;

    drLocal.drX0= add->addBackRect.drX0;
    drLocal.drX1= add->addBackRect.drX1;
    drLocal.drY0= BI_TOP_PIXELS( add, bi );
    drLocal.drY1= BI_BELOW_PIXELS( add, bi );

    if  ( bi->biLevel == DOClevROW	&&
	  bi->biRowHasTableParagraphs	)
	{ drLocal.drY1= LP_YPIXELS( add, &(bi->biRowBelowAllPosition) ); }

    docUnionRectangle( drChanged, drChanged, &drLocal );

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
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);

    int				part;

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

    EditOperation		eo;

    tedStartEditOperation( &eo, &ds, &sg, ed, 1 );

    taSetMask= *taSetMaskCall;

    PROPmaskCLEAR( &selTaChgMask );
    PROPmaskCLEAR( &selPpChgMask );
    PROPmaskCLEAR( &selSpChgMask );

    if  ( docIsIBarSelection( &ds ) )
	{ PROPmaskCLEAR( &taSetMask );	}

    paraBi= ds.dsBegin.dpBi;
    part= ds.dsBegin.dpParticule;

    if  ( ! paraBi )
	{ XDEB(paraBi); return -1; }

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
    if  ( ! PROPmaskISEMPTY( spUpdMask ) )
	{
	if  ( tedChangeSectionProperties( &sectSpChgMask, sectBi,
						    spUpdMask, spNew,
						    &(eo.eoChangedRectangle),
						    &(add->addBackRect) ) )
	    { XDEB(spUpdMask); return -1;	}

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
	    if  ( tedChangeParaProperties( &paraPpChgMask, &paraTaChgMask,
				    paraBi, part, partUpto, add, bd, dfl,
				    &taSetMask, taSet, ppUpdMask, ppNew ) )
		{ LDEB(1);	}

	    if  ( sectBi != paraBi->biParent->biParent->biParent )
		{
		if  ( ! PROPmaskISEMPTY( &sectSpChgMask ) )
		    { tedApplyItemFormat( &eo, sectBi, add );	}

		sectBi= paraBi->biParent->biParent->biParent;
		if  ( ! PROPmaskISEMPTY( spUpdMask ) )
		    {
		    if  ( tedChangeSectionProperties( &sectSpChgMask, sectBi,
						    spUpdMask, spNew,
						    &(eo.eoChangedRectangle),
						    &(add->addBackRect) ) )
			{ XDEB(spUpdMask); return -1;	}

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
    if  ( tedChangeParaProperties( &paraPpChgMask, &paraTaChgMask, paraBi,
		    part,
		    ds.dsEnd.dpParticule+ firstParticuleSplit, add, bd, dfl,
		    &taSetMask, taSet, ppUpdMask, ppNew ) )
	{ LDEB(1);	}

    part= ds.dsEnd.dpParticule+ firstParticuleSplit;
    tp= paraBi->biParaParticules+ part;

    if  ( sectBi != paraBi->biParent->biParent->biParent )
	{
	if  ( ! PROPmaskISEMPTY( &sectSpChgMask ) )
	    { tedApplyItemFormat( &eo, sectBi, add ); }

	sectBi= paraBi->biParent->biParent->biParent;
	if  ( ! PROPmaskISEMPTY( spUpdMask ) )
	    {
	    if  ( tedChangeSectionProperties( &sectSpChgMask, sectBi,
						    spUpdMask, spNew,
						    &(eo.eoChangedRectangle),
						    &(add->addBackRect) ) )
		{ XDEB(spUpdMask); return -1;	}

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
	if  ( tedChangeParticuleAttributes( &pmSplit, add, bd,
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
	{
	DocumentSelection	dsNew;

	docSetDocumentPosition( &(ds.dsBegin),
				ds.dsBegin.dpBi, ds.dsBegin.dpStroff, 1 );
	docSetDocumentPosition( &(ds.dsEnd),
				ds.dsEnd.dpBi, ds.dsEnd.dpStroff, 0 );

	docInitDocumentSelection( &dsNew );

	docSetRangeSelection( &dsNew, &(ds.dsBegin), &(ds.dsEnd),
					ds.dsDirection, ds.dsCol0, ds.dsCol1 );

	if  ( tedEditFinishSelection( ed, &eo, &dsNew ) )
	    { LDEB(1);	}

	tedAdaptToolsToSelection( ed );

	appDocumentChanged( ed, 1 );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Utility routines around tedChangeSelectionProperties():		*/
/*									*/
/************************************************************************/

int tedAppChangeSectionProperties( EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    TextAttribute		taSet;

    PropertyMask		taUpdMask;
    PropertyMask		ppUpdMask;

    PROPmaskCLEAR( &taUpdMask );
    PROPmaskCLEAR( &ppUpdMask );

    utilInitTextAttribute ( &taSet );

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedChangeSelectionProperties( ed,
				    &taUpdMask, &taSet,
				    &ppUpdMask, (ParagraphProperties *)0,
				    spUpdMask, spNew ) )
	{ XDEB(spUpdMask); return -1;	}

    return 0;
    }

int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew )
    {
    TextAttribute		taSet;

    PropertyMask		taUpdMask;
    PropertyMask		spUpdMask;

    PROPmaskCLEAR( &taUpdMask );
    PROPmaskCLEAR( &spUpdMask );

    utilInitTextAttribute ( &taSet );

    if  ( tedChangeSelectionProperties( ed,
				    &taUpdMask, &taSet,
				    ppUpdMask, ppNew,
				    &spUpdMask, (SectionProperties *)0 ) )
	{ XDEB(ppUpdMask); return -1;	}

    return 0;
    }

int tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeParagraphProperties( ed, ppUpdMask, ppNew );
    }

/************************************************************************/
/*									*/
/*  Adapt format tool to the circumstances.				*/
/*									*/
/************************************************************************/

void tedAdaptFormatToolToDocument(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( ! tar->tarInspector )
	{ return;	}

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return;	}

    tedFormatToolAdaptToSelection( tar->tarInspector, bd,
					    &ds, &sg, ed->edFileReadOnly );

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
    TedDocument *		td;
    AppDrawingData *		add;

    BufferItem *		bi;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    add= &(ed->edDrawingData);
    td= (TedDocument *)ed->edPrivateData;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return -1;	}

    bi= ds.dsBegin.dpBi;

    if  ( docPsParagraphLineExtents( &(add->addPhysicalFontList), bi ) )
	{ LDEB(1); return -1;	}

    *pLineHeight= bi->biParaAscentTwips- bi->biParaDescentTwips; return 0;
    }

