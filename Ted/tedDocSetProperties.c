/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docEditCommand.h>
#   include	<docParaProperties.h>
#   include	<docSectProperties.h>
#   include	<docDocumentProperties.h>
#   include	<appEditDocument.h>
#   include	<docTreeNode.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int tedEditChangeSelectionProperties(
			TedEditOperation *			teo,
			const DocumentSelection *		ds,
			int					level,
			int					command,

			const struct PropertyMask *		taSetMask,
			const struct TextAttribute *		taSet,

			const struct PropertyMask *		ppSetMask,
			const struct ParagraphProperties *	ppSet,

			const struct PropertyMask *		cpSetMask,
			const struct CellProperties *		cpSet,

			const struct PropertyMask *		rpSetMask,
			const struct RowProperties *		rpSet,

			const struct PropertyMask *		spSetMask,
			const struct SectionProperties *	spSet,

			const struct PropertyMask *		dpSetMask,
			const struct DocumentProperties *	dpSet )
    {
    int				rval = 0;
    EditOperation *		eo= &(teo->teoEo);

    DocumentSelection		dsTraced= *ds;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    if  ( tedEditStartStep( teo, command ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, level,
					taSetMask, ppSetMask,
					cpSetMask, rpSetMask,
					spSetMask, dpSetMask ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docRtfTraceNewProperties( eo,
					taSetMask, taSet,
					ppSetMask, ppSet,
					cpSetMask, cpSet,
					rpSetMask, rpSet,
					spSetMask, spSet,
					dpSetMask, dpSet ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditChangeSelectionPropertiesImpl(
					&(teo->teoRefreshScreenRectangle),
					eo, &dsTraced,
					taSetMask, taSet,
					ppSetMask, ppSet,
					cpSetMask, cpSet,
					rpSetMask, rpSet,
					spSetMask, spSet,
					dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( cpSetMask )
	{
	EditRange *		er= &(eo->eoSelectedRange);
	DocumentSelection	dsNew;

	docInitDocumentSelection( &dsNew );

	if  ( docSelectionForEditPositionsInTree( &dsNew, eo->eoTree,
					    &(er->erHead), &(er->erTail) ) )
	    { LDEB(1); return -1;	}

	if  ( dsNew.dsCol0 >= 0 && dsNew.dsCol1 >= 0 )
	    { docAvoidMergedCellTail( &dsNew, er );	}
	}

    docSetEditRange( &(eo->eoAffectedRange), &dsTraced );

    if  ( tedEditFinishOldSelection( teo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    /*  Activate text attribute setting for future typing */
    if  ( docIsIBarSelection( ds ) && taSetMask )
	{
	EditDocument *		ed= teo->teoEditDocument;
	TedDocument *		td= (TedDocument *)ed->edPrivateData;
	struct BufferDocument *	bd= td->tdDocument;
	SelectionDescription *	sd= &(td->tdSelectionDescription);
	TextAttribute *		ta= &(sd->sdTextAttribute);

	PropertyMask		taDoneMask;

	utilPropMaskClear( &taDoneMask );

	textUpdateTextAttribute( &taDoneMask, ta, taSetMask, taSet );

	if  ( ! utilPropMaskIsEmpty( &taDoneMask ) )
	    {
	    const IndexMapping *	a2s= &(td->tdAttributeToScreenFont);

	    sd->sdTextAttributeNumber= docTextAttributeNumber( bd, ta );

	    td->tdCurrentScreenFont= utilIndexMappingGet( a2s,
			    td->tdSelectionDescription.sdTextAttributeNumber );
	    }
	}

    tedFinishEditOperation( teo );

  ready:

    return rval;
    }

static int tedDocChangeSelectionProperties(
			EditDocument *				ed,
			int					command,

			const PropertyMask *			taSetMask,
			const struct TextAttribute *		taSet,

			const PropertyMask *			ppSetMask,
			const struct ParagraphProperties *	ppSet,

			const PropertyMask *			cpSetMask,
			const struct CellProperties *		cpSet,

			const PropertyMask *			rpSetMask,
			const struct RowProperties *		rpSet,

			const PropertyMask *			spSetMask,
			const struct SectionProperties *	spSet,

			const PropertyMask *			dpSetMask,
			const struct DocumentProperties *	dpSet,
			
			int				traced )
    {
    int				rval= 0;
    const int			level= DOClevSPAN;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedEditChangeSelectionProperties( &teo, &ds, level, command,
						taSetMask, taSet,
						ppSetMask, ppSet,
						cpSetMask, cpSet,
						rpSetMask, rpSet,
						spSetMask, spSet,
						dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Utility routines around tedDocChangeSelectionProperties():		*/
/*									*/
/************************************************************************/

int tedDocChangeSectionProperties(
			EditDocument *				ed,
			const PropertyMask *			spSetMask,
			const struct SectionProperties *	spSet,
			int					traced )
    {
    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_SECT_PROPS,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		(const PropertyMask *)0, (const struct ParagraphProperties *)0,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		spSetMask, spSet,
		(const PropertyMask *)0, (const struct DocumentProperties *)0,
		traced ) )
	{ XDEB(spSetMask); return -1;	}

    return 0;
    }

int tedDocChangeParagraphProperties(
			EditDocument *				ed,
			const PropertyMask *			ppSetMask,
			const struct ParagraphProperties *	ppSet,
			int					traced )
    {
    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_PARA_PROPS,
		    (const PropertyMask *)0, (const struct TextAttribute *)0,
		    ppSetMask, ppSet,
		    (const PropertyMask *)0, (const struct CellProperties *)0,
		    (const PropertyMask *)0, (const struct RowProperties *)0,
		    (const PropertyMask *)0, (const struct SectionProperties *)0,
		    (const PropertyMask *)0, (const struct DocumentProperties *)0,
		    traced ) )
	{ XDEB(ppSetMask); return -1;	}

    return 0;
    }

void tedDocChangeTextAttribute(	EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const struct TextAttribute *	taSet,
				int				traced )
    {
    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_SPAN_PROPS,
		taSetMask, taSet,
		(const PropertyMask *)0, (const struct ParagraphProperties *)0,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0,
		traced ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change table properties.						*/
/*									*/
/************************************************************************/

int tedDocSetTableProperties(	EditDocument *			ed,
				int				wholeRow,
				int				wholeColumn,
				const PropertyMask *		cpSetMask,
				const struct CellProperties *	cpSet,
				const PropertyMask *		rpSetMask,
				const struct RowProperties *	rpSet )
    {
    int				rval= 0;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    int				level= DOClevCELL;
    int				command= EDITcmdUPD_TABLE_PROPS;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, td->tdTraced );

    docInitDocumentSelection( &ds );

    docEditOperationGetSelection( &ds, eo );

    if  ( wholeRow )
	{
	if  ( wholeColumn )
	    { command= EDITcmdUPD_TABLE_PROPS; level= DOClevTABLE;	}
	else{ command= EDITcmdUPD_ROW_PROPS; level= DOClevROW;		}
	}
    else{
	if  ( wholeColumn )
	    { command= EDITcmdUPD_COLUMN_PROPS; level= DOClevCOLUMN;	}
	else{ command= EDITcmdUPD_CELL_PROPS; level= DOClevCELL;	}
	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		level, command,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		(const PropertyMask *)0, (const struct ParagraphProperties *)0,
		cpSetMask, cpSet, rpSetMask, rpSet,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0 ) )
	{ XDEB(cpSetMask); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the tabs of a paragraph.					*/
/*									*/
/************************************************************************/

int tedDocSetParagraphTabs(	EditDocument *			ed,
				const struct TabStopList *	tsl,
				int				traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    docInitParagraphProperties( &ppSet );
    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropTAB_STOPS );

    ppSet.ppTabStopListNumber= docTabStopListNumber( bd, tsl );

    return tedDocChangeParagraphProperties( ed, &ppSetMask, &ppSet, traced );
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

int tedDocChangeAllSectionProperties(
				EditDocument *			ed,
				const PropertyMask *		spSetMask,
				const struct SectionProperties *	spSet,
				const PropertyMask *		dpSetMask,
				const struct DocumentProperties *	dpSet,
				int				traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docInitDocumentSelection( &ds );

    if  ( docSelectWholeBody( &ds, eo->eoDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		DOClevBODY, EDITcmdUPD_SECTDOC_PROPS,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		(const PropertyMask *)0, (const struct ParagraphProperties *)0,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		spSetMask, spSet, dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change document properties.						*/
/*									*/
/************************************************************************/

int tedDocSetDocumentProperties( EditDocument *			ed,
				const PropertyMask *		dpSetMask,
				const struct DocumentProperties *	dpSet )
    {
    int		traced= ((TedDocument *)ed->edPrivateData)->tdTraced;

    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_DOC_PROPS,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		(const PropertyMask *)0, (const struct ParagraphProperties *)0,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		dpSetMask, dpSet,
		traced ) )
	{ XDEB(dpSetMask); return -1;	}

    return 0;
    }

/************************************************************************/

int tedDocChangeParagraphPropertyValue(
				EditDocument *			ed,
				int				prop,
				int				value )
    {
    int		traced= ((TedDocument *)ed->edPrivateData)->tdTraced;

    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, prop );

    docInitParagraphProperties( &ppSet );
    if  ( docSetParaProperty( &ppSet, prop, value ) )
	{ LLDEB(prop,value); return -1;	}

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppSet, traced ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &ppSet );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the document geometry: Callback from the page layout tool	*/
/*  etc.								*/
/*									*/
/************************************************************************/

void tedDocSetPageLayout(	EditDocument *			ed,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet,
				int				wholeDocument,
				int				traced )
    {
    SectionProperties		spSet;
    DocumentProperties		dpSet;

    docInitSectionProperties( &spSet );
    docInitDocumentProperties( &dpSet );

    geoUpdDocumentGeometry( (PropertyMask *)0, &(spSet.spDocumentGeometry),
							    dgSetMask, dgSet );

    if  ( wholeDocument )
	{
	geoUpdDocumentGeometry( (PropertyMask *)0, &(dpSet.dpGeometry),
							    dgSetMask, dgSet );

	if  ( tedDocChangeAllSectionProperties( ed,
						dgSetMask, &spSet,
						dgSetMask, &dpSet,
						traced ) )
	    { LDEB(wholeDocument); goto ready;	}
	}
    else{
	if  ( tedDocChangeSectionProperties( ed, dgSetMask, &spSet, traced ) )
	    { XDEB(dgSetMask); goto ready;	}
	}

  ready:

    docCleanSectionProperties( &spSet );
    docCleanDocumentProperties( &dpSet );

    return;
    }

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

int tedFormatShiftIndent(	EditDocument *	ed,
				int		step,
				int		traced )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    PropertyMask		ppSetMask;

    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );
    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( step >= 0 )
	{
	if  ( tedEditStartStep( &teo, EDITcmdSHIFT_RIGHT ) )
	    { LDEB(step); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdSHIFT_LEFT ) )
	    { LDEB(step); rval= -1; goto ready;	}
	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, DOClevPARA,
				    (PropertyMask *)0, &ppSetMask,
				    (PropertyMask *)0, (PropertyMask *)0,
				    (PropertyMask *)0, (PropertyMask *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditShiftIndent( eo, step ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

