/************************************************************************/
/*									*/
/*  A tool to manage the style of notes in the document.		*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<limits.h>
#   include	<stdlib.h>

#   include	"tedNotesTool.h"
#   include	<tedAppFront.h>
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docDocumentProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docBuf.h>
#   include	<utilPropMask.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<docSectProperties.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A notes tool, i.e. the 'Notes' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct NotesPageResources
    {
    const char *	nprFirstNumberText;
    const char *	nprJustifyText;
    const char *	nprPositionText;
    const char *	nprRestartText;
    const char *	nprStyleText;

    const char *	nprJustifyMenuTexts[FTNjustify_COUNT];
    const char *	nprPlacementMenuTexts[FTNplace_COUNT];
    const char *	nprRestartMenuTexts[FTN_RST__COUNT];
    const char *	nprNumberStyleMenuTexts[FTNstyle_COUNT];

    const char *	nprRevertSelectionText;
    const char *	nprChangeSelectionText;
    } NotesPageResources;

typedef struct NotesTool
    {
    struct EditApplication *	ntApplication;
    struct AppInspector *	ntInspector;
    const NotesPageResources *	ntPageResources;

    unsigned int		ntNoteTreeType;

    unsigned char		ntCanChangeSelection;
    unsigned char		ntCanChangeDocument;
    int				ntInProgrammaticChange;

    FootEndNotesProperties	ntNotesPropertiesSetDocument;
    FootEndNotesProperties	ntNotesPropertiesSetSelection;
    FootEndNotesProperties	ntNotesPropertiesChosen;

    APP_WIDGET			ntRevertSelectionWidget;
    APP_WIDGET			ntChangeSelectionWidget;


    APP_WIDGET			ntJustifyRow;
    APP_WIDGET			ntJustifyLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntJustifyOptionmenu;

    APP_WIDGET			ntPlacementRow;
    APP_WIDGET			ntPlacementLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntPlacementOptionmenu;

    APP_WIDGET			ntRestartRow;
    APP_WIDGET			ntRestartLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntRestartOptionmenu;

    APP_WIDGET			ntStyleRow;
    APP_WIDGET			ntStyleLabel;
    AppOptionmenu		ntStyleOptionmenu;

    APP_WIDGET			ntStartNumberRow;
    APP_WIDGET			ntStartNumberLabel;
    APP_WIDGET			ntStartNumberText;
    } NotesTool;

static NotesPageResources		TED_NotesPageResources;
static InspectorSubjectResources	TED_FootNotesSubjectResources;

/************************************************************************/
/*									*/
/*  Clean the notes tool.						*/
/*									*/
/************************************************************************/

static void tedInitNotesTool(	NotesTool *		nt )
    {
    nt->ntNoteTreeType= DOCinBODY;

    nt->ntApplication= (struct EditApplication *)0;
    nt->ntInspector= (struct AppInspector *)0;
    nt->ntPageResources= (NotesPageResources *)0;

    nt->ntInProgrammaticChange= 0;

    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetDocument) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetSelection) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesChosen) );

    nt->ntRevertSelectionWidget= (APP_WIDGET)0;
    nt->ntChangeSelectionWidget= (APP_WIDGET)0;

    nt->ntJustifyRow= (APP_WIDGET)0;
    guiInitOptionmenu( &(nt->ntJustifyOptionmenu) );

    nt->ntPlacementRow= (APP_WIDGET)0;
    guiInitOptionmenu( &(nt->ntPlacementOptionmenu) );

    nt->ntRestartRow= (APP_WIDGET)0;
    guiInitOptionmenu( &(nt->ntRestartOptionmenu) );

    nt->ntStyleRow= (APP_WIDGET)0;
    guiInitOptionmenu( &(nt->ntStyleOptionmenu) );

    nt->ntStartNumberText= (APP_WIDGET)0;

    return;
    }

static void tedFormatFreeNotesTool(	void *		vnt )
    {
    free( vnt );
    }

/************************************************************************/
/*									*/
/*  What property values are relevant for footnotes/endnotes?		*/
/*									*/
/************************************************************************/

static const int TED_FootnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    FTN_RST_PER_PAGE,
    };

static const int TED_FootnoteRestartCount= sizeof(TED_FootnoteRestarts)/sizeof(int);

static const int TED_EndnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    };

static const int TED_EndnoteRestartCount= sizeof(TED_EndnoteRestarts)/sizeof(int);

static const int TED_EndnotePlacements[]=
    {
    /*
    FTNplacePAGE_END,
    */
    FTNplaceSECT_END,
    FTNplaceDOC_END,
    };

static const int TED_EndnotePlacementCount= sizeof(TED_EndnotePlacements)/sizeof(int);

/************************************************************************/
/*									*/
/*  Refresh the 'Notes' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedSetNotesMenu(	AppOptionmenu *		aom,
				int			val,
				const int *		values,
				int			valueCount )
    {
    int		i;

    for ( i= 0; i < valueCount; i++ )
	{
	if  ( values[i] == val )
	    { guiSetOptionmenu( aom, i ); return;	}
	}

    LLDEB(val,valueCount);
    guiSetOptionmenu( aom, -1 );
    return;
    }

static void tedNotesRefreshNotesStartNumber(	NotesTool *		nt,
						const NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	guiIntegerToText( nt->ntStartNumberText, np->npStartNumber );
	guiEnableText( nt->ntStartNumberText, 1 );
	}
    else{
	guiStringToText( nt->ntStartNumberText, "" );
	guiEnableText( nt->ntStartNumberText, 0 );
	}

    return;
    }

static void tedNotesRefreshNotePropertiesTool(
					NotesTool *		nt,
					const NotesProperties *	np,
					const int *		restarts,
					int			restartCount,
					const int *		placements,
					int			placementCount )
    {
    tedNotesRefreshNotesStartNumber( nt, np );

    if  ( placements )
	{
	guiEnableWidget( nt->ntPlacementRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	guiEnableOptionmenu( &(nt->ntPlacementOptionmenu),
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	tedSetNotesMenu( &(nt->ntPlacementOptionmenu),
					    np->npPlacement,
					    placements, placementCount );
	}
    else{
	if  ( nt->ntPlacementOptionmenu.aomInplace )
	    {
	    guiEnableWidget( nt->ntPlacementRow, 0 );
	    guiEnableOptionmenu( &(nt->ntPlacementOptionmenu), 0 );
	    guiSetOptionmenu( &(nt->ntPlacementOptionmenu), -1 );
	    }
	}

    tedSetNotesMenu( &(nt->ntRestartOptionmenu),
					    np->npRestart,
					    restarts, restartCount );

    guiSetOptionmenu( &(nt->ntJustifyOptionmenu),
					    np->npJustification );
    guiSetOptionmenu( &(nt->ntStyleOptionmenu),
					    np->npNumberStyle );

    return;
    }

static void tedNotesToolRefreshNotesWidgets(	NotesTool *	nt )
    {
    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesRefreshNotePropertiesTool( nt,
			&(nt->ntNotesPropertiesChosen.fepFootnotesProps),
			TED_FootnoteRestarts,
			TED_FootnoteRestartCount,
			(int *)0, 0 );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesRefreshNotePropertiesTool( nt,
			&(nt->ntNotesPropertiesChosen.fepEndnotesProps),
			TED_EndnoteRestarts,
			TED_EndnoteRestartCount,
			TED_EndnotePlacements,
			TED_EndnotePlacementCount );
	}

    return;
    }

static void tedRefreshNotesTool( void *				vnt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    NotesTool *			nt= (NotesTool *)vnt;

    const DocumentProperties *	dp= bd->bdProperties;

    const struct BufferItem *		bodySectNode= (struct BufferItem *)0;

    if  ( ds )
	{
	bodySectNode= docGetBodySectNode( ds->dsHead.dpNode, bd );
	if  ( ! bodySectNode )
	    { XDEB(bodySectNode);	}
	}

    nt->ntNotesPropertiesSetDocument= dp->dpNotesProps;
    nt->ntNotesPropertiesSetSelection= dp->dpNotesProps;

    if  ( bodySectNode )
	{
	nt->ntNotesPropertiesSetSelection=
			    bodySectNode->biSectNotesProperties;
	}

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetSelection;

    nt->ntCanChangeSelection= cmdEnabled[EDITcmdUPD_SECT_PROPS];
    nt->ntCanChangeDocument= cmdEnabled[EDITcmdUPD_SECTDOC_PROPS];

    tedNotesToolRefreshNotesWidgets( nt );

    guiEnableWidget( nt->ntJustifyRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    if  ( nt->ntPlacementRow )
	{
	guiEnableWidget( nt->ntPlacementRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	}

    guiEnableWidget( nt->ntRestartRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntStyleRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntStartNumberRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableText( nt->ntStartNumberText,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    guiEnableWidget( nt->ntRevertSelectionWidget,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntChangeSelectionWidget,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    guiEnableWidget( is->isRevertButton,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( is->isApplyButton,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the document properties have been pushed.	*/
/*									*/
/************************************************************************/

static int tedNotesToolGetStartNumber(	NotesTool *		nt,
					NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	if  ( guiIntegerFromText( nt->ntStartNumberText,
				    &(np->npStartNumber), 1, 0, INT_MAX, 0 ) )
	    { return -1;	}
	}

    return 0;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesFootRestartChanged, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( nt, np )		)
	{ return;	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesEndRestartChanged, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    NotesProperties *		np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( nt, np )		)
	{ return;	}

    return;
    }

static int tedNotesToolSetProperties(	NotesTool *	nt,
					int		wholeDocument )
    {
    int				rval= 0;

    PropertyMask		dpSetMask;
    PropertyMask		spSetMask;

    DocumentProperties		dpNew;
    SectionProperties		spNew;

    docInitSectionProperties( &spNew );
    docInitDocumentProperties( &dpNew );

    utilPropMaskClear( &dpSetMask );
    utilPropMaskClear( &spSetMask );

    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesToolGetStartNumber( nt,
		    &(nt->ntNotesPropertiesChosen.fepFootnotesProps) );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesToolGetStartNumber( nt,
		    &(nt->ntNotesPropertiesChosen.fepEndnotesProps) );
	}

    /**/
    dpNew.dpNotesProps= nt->ntNotesPropertiesChosen;
    spNew.spNotesProperties= nt->ntNotesPropertiesChosen;

    {
    PropertyMask	cmpMask;

    utilPropMaskClear( &cmpMask );
    docFillDocNotesMask( &cmpMask );

    docFootEndNotesPropertyDifference( &dpSetMask,
				&(dpNew.dpNotesProps), &cmpMask,
				&(nt->ntNotesPropertiesSetDocument),
				DOCdocNOTE_PROP_MAP );

    }

    {
    PropertyMask	cmpMask;

    utilPropMaskClear( &cmpMask );
    docFillSectNotesMask( &cmpMask );

    docFootEndNotesPropertyDifference( &spSetMask,
				&(spNew.spNotesProperties), &cmpMask,
				&(nt->ntNotesPropertiesSetSelection),
				DOCsectNOTE_PROP_MAP );
    }

    if  ( wholeDocument )
	{
	tedAppChangeAllSectionProperties( nt->ntApplication,
							&spSetMask, &spNew,
							&dpSetMask, &dpNew );
	}
    else{
	tedAppChangeSectionProperties( nt->ntApplication, &spSetMask, &spNew );
	}

    docCleanSectionProperties( &spNew );
    docCleanDocumentProperties( &dpNew );

    return rval;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolChangeDocumentPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    const int			wholeDocument= 1;

    tedNotesToolSetProperties( nt, wholeDocument );
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolChangeSelectionPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    const int			wholeDocument= 0;

    tedNotesToolSetProperties( nt, wholeDocument );
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolRevertDocumentPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetDocument;
    tedNotesToolRefreshNotesWidgets( nt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolRevertSelectionPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetSelection;
    tedNotesToolRefreshNotesWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Menu callbacks.							*/
/*									*/
/************************************************************************/

static void tedFootnoteStyleChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static void tedEndnoteStyleChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static void tedFootnoteRestartChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= TED_FootnoteRestartCount )
	{ LLDEB(n,TED_FootnoteRestartCount); return;	}

    np->npRestart= TED_FootnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( nt, np );

    return;
    }

static void tedEndnoteRestartChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= TED_EndnoteRestartCount )
	{ LLDEB(n,TED_EndnoteRestartCount); return;	}

    np->npRestart= TED_EndnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( nt, np );

    return;
    }

static void tedEndnotePlacementChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= TED_EndnotePlacementCount )
	{ LLDEB(n,TED_EndnotePlacementCount); return;	}

    if  ( TED_EndnotePlacements[n] < 0			||
	  TED_EndnotePlacements[n] >= FTNplace_COUNT	)
	{ LDEB(TED_EndnotePlacements[n]); return;	}

    np->npPlacement= TED_EndnotePlacements[n];

    return;
    }

static void tedFootnoteJustifyChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= FTNjustify_COUNT )
	{ LDEB(n); return;	}

    np->npJustification= n;

    return;
    }

static void tedEndnoteJustifyChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= FTNjustify_COUNT )
	{ LDEB(n); return;	}

    np->npJustification= n;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Notes' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedNotesMakeNotePropertiesTool(
				APP_WIDGET			pageWidget,
				NotesTool *			nt,
				const NotesPageResources *	npr,
				APP_TXACTIVATE_CALLBACK_T	startCB,
				OptionmenuCallback		justifyCB,
				OptionmenuCallback		placementCB,
				OptionmenuCallback		restartCB,
				OptionmenuCallback		styleCB )
    {
    const int	textColumns= 6;

    guiToolMakeLabelAndMenuRow( &(nt->ntJustifyRow), &(nt->ntJustifyOptionmenu),
			    &(nt->ntJustifyLabel),
			    pageWidget, npr->nprJustifyText,
			    justifyCB, (void *)nt );

    if  ( placementCB )
	{
	guiToolMakeLabelAndMenuRow( &(nt->ntPlacementRow),
			    &(nt->ntPlacementOptionmenu),
			    &(nt->ntPlacementLabel),
			    pageWidget, npr->nprPositionText,
			    placementCB, (void *)nt );
	}

    guiToolMakeLabelAndMenuRow( &(nt->ntRestartRow), &(nt->ntRestartOptionmenu),
			    &(nt->ntRestartLabel),
			    pageWidget, npr->nprRestartText,
			    restartCB, (void *)nt );

    guiToolMakeLabelAndMenuRow( &(nt->ntStyleRow), &(nt->ntStyleOptionmenu),
			    &(nt->ntStyleLabel), pageWidget, npr->nprStyleText,
			    styleCB, (void *)nt );

    guiToolMakeLabelAndTextRow( &(nt->ntStartNumberRow),
			&(nt->ntStartNumberLabel), &(nt->ntStartNumberText),
			pageWidget, npr->nprFirstNumberText,
			textColumns, 1 );

    if  ( startCB )
	{
	guiSetGotValueCallbackForText( nt->ntStartNumberText,
						    startCB, (void *)nt );
	}

    return;
    }

static void * tedFormatBuildNotesPage(
				const NotesPageResources *	npr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr,
				int				noteTreeType )
    {
    APP_WIDGET			row= (APP_WIDGET )0;

    NotesTool *		nt= malloc( sizeof(NotesTool) );
    if  ( ! nt )
	{ XDEB(nt); return nt;	}

    tedInitNotesTool( nt );

    /**/
    nt->ntPageResources= npr;
    nt->ntInspector= ai;
    nt->ntApplication= ea;
    nt->ntInProgrammaticChange= 0;
    nt->ntNoteTreeType= noteTreeType;

    nt->ntRevertSelectionWidget= (APP_WIDGET)0;
    nt->ntChangeSelectionWidget= (APP_WIDGET)0;


    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetDocument) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetSelection) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesChosen) );

    /**************/

    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesMakeNotePropertiesTool( pageWidget, nt,
					npr, tedNotesFootRestartChanged,
					tedFootnoteJustifyChosen,
					(OptionmenuCallback)0,
					tedFootnoteRestartChosen,
					tedFootnoteStyleChosen );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesMakeNotePropertiesTool( pageWidget, nt,
					npr, tedNotesEndRestartChanged,
					tedEndnoteJustifyChosen,
					tedEndnotePlacementChosen,
					tedEndnoteRestartChosen,
					tedEndnoteStyleChosen );
	}

    /**************/

    guiToolMake2BottonRow( &row, pageWidget,
				&(nt->ntRevertSelectionWidget),
				&(nt->ntChangeSelectionWidget),
				npr->nprRevertSelectionText,
				npr->nprChangeSelectionText,
				tedNotesToolRevertSelectionPushed,
				tedNotesToolChangeSelectionPushed,
				(void *)nt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
				&(is->isRevertButton),
				&(is->isApplyButton),
				isr->isrRevert,
				isr->isrApplyToSubject,
				tedNotesToolRevertDocumentPushed,
				tedNotesToolChangeDocumentPushed,
				(void *)nt );

    return (void *)nt;
    }

static void * tedFormatBuildFootNotesPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const NotesPageResources *	npr= &TED_NotesPageResources;

    return tedFormatBuildNotesPage( npr, subjectPage,
				ai, ea, is, pageWidget, isr, DOCinFOOTNOTE );
    }

static void * tedFormatBuildEndNotesPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const NotesPageResources *	npr= &TED_NotesPageResources;

    return tedFormatBuildNotesPage( npr, subjectPage,
				ai, ea, is, pageWidget, isr, DOCinENDNOTE );
    }

static void tedNotePropertiesToolFillStyleChooser(
				NotesTool *			nt,
				const NotesPageResources *	npr )
    {
    appFillInspectorMenu( FTNjustify_COUNT, FTNjustifyBELOW_TEXT,
		    npr->nprJustifyMenuTexts, &(nt->ntJustifyOptionmenu) );

    appFillInspectorMenu( FTNstyle_COUNT, FTNstyleNAR,
		    npr->nprNumberStyleMenuTexts, &(nt->ntStyleOptionmenu) );

    return;
    }

static void tedNotePropertiesToolFillChoosers(
			NotesTool *			nt,
			const NotesPageResources *	npr,
			const int *			restarts,
			int				restartCount,
			const int *			placements,
			int				placementCount )
    {
    int		i;

    for ( i= 0; i < restartCount; i++ )
	{
	guiAddItemToOptionmenu( &(nt->ntRestartOptionmenu),
			npr->nprRestartMenuTexts[restarts[i]] );
	}

    for ( i= 0; i < placementCount; i++ )
	{
	guiAddItemToOptionmenu( &(nt->ntPlacementOptionmenu),
			npr->nprPlacementMenuTexts[placements[i]] );
	}

    return;
    }

static void tedFormatFillFootNotesChoosers( void *		vnt )
    {
    NotesTool *			nt= (NotesTool *)vnt;
    const NotesPageResources *	npr= &TED_NotesPageResources;

    tedNotePropertiesToolFillStyleChooser( nt, npr );

    tedNotePropertiesToolFillChoosers( nt, npr,
				TED_FootnoteRestarts,
				TED_FootnoteRestartCount,
				(int *)0, 0 );
    }

static void tedFormatFillEndNotesChoosers( void *		vnt )
    {
    NotesTool *			nt= (NotesTool *)vnt;
    const NotesPageResources *	npr= &TED_NotesPageResources;

    tedNotePropertiesToolFillStyleChooser( nt, npr );

    tedNotePropertiesToolFillChoosers( nt, npr,
				TED_EndnoteRestarts,
				TED_EndnoteRestartCount,
				TED_EndnotePlacements,
				TED_EndnotePlacementCount );
    }

static void tedFormatFinishNotesPage(	void *		vnt )
    {
    NotesTool *	nt= (NotesTool *)vnt;

    if  ( nt->ntJustifyOptionmenu.aomInplace )
	{ guiOptionmenuRefreshWidth( &(nt->ntJustifyOptionmenu) );	}

    if  ( nt->ntPlacementOptionmenu.aomInplace )
	{ guiOptionmenuRefreshWidth( &(nt->ntPlacementOptionmenu) );	}

    if  ( nt->ntRestartOptionmenu.aomInplace )
	{ guiOptionmenuRefreshWidth( &(nt->ntRestartOptionmenu) );	}

    if  ( nt->ntStyleOptionmenu.aomInplace )
	{ guiOptionmenuRefreshWidth( &(nt->ntStyleOptionmenu) );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get notes tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedFootnotesSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolFootnotes",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Footnotes" ),
    APP_RESOURCE( "formatToolChangeNotes",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevertDocumentNotesProperties",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert to Document" ),
    };

static AppConfigurableResource TED_TedEndnotesSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolEndnotes",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Endnotes" ),
    APP_RESOURCE( "formatToolChangeNotes",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevertDocumentNotesProperties",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert to Document" ),
    };

static AppConfigurableResource TED_TedNotesToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolRevertSection",
		offsetof(NotesPageResources,nprRevertSelectionText),
		"Revert to Selection" ),
    APP_RESOURCE( "tableToolChangeSelection",
		offsetof(NotesPageResources,nprChangeSelectionText),
		"Apply to Selection" ),

    /**/
    APP_RESOURCE( "formatToolNotesFirstNumber",
		offsetof(NotesPageResources,nprFirstNumberText),
		"First Number" ),
    APP_RESOURCE( "formatToolNotesJustify",
		offsetof(NotesPageResources,nprJustifyText),
		"Justify" ),
    APP_RESOURCE( "formatToolNotesPosition",
		offsetof(NotesPageResources,nprPositionText),
		"Position" ),
    APP_RESOURCE( "formatToolNotesRestart",
		offsetof(NotesPageResources,nprRestartText),
		"Numbering" ),
    APP_RESOURCE( "formatToolNotesNumberStyle",
		offsetof(NotesPageResources,nprStyleText),
		"Number Format" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberStyleNar",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAR]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNalc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNALC]),
	    "a, b, c" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNauc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAUC]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNrlc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRLC]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNruc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRUC]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNchi",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNCHI]),
	    "*, +, #, $" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberingContinuous",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_CONTINUOUS]),
	"Continuous" ),
    APP_RESOURCE( "formatToolNoteNumberingPerSection",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_SECTION]),
	"Per Section" ),
    APP_RESOURCE( "formatToolNoteNumberingPerPage",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_PAGE]),
	"Per Page" ),
    /**/
    APP_RESOURCE( "formatToolNotePlacementSectEnd",
	offsetof(NotesPageResources,nprPlacementMenuTexts[FTNplaceSECT_END]),
	"End of Section" ),
    APP_RESOURCE( "formatToolNotePlacementDocEnd",
	offsetof(NotesPageResources,nprPlacementMenuTexts[FTNplaceDOC_END]),
	"End of Document" ),

    APP_RESOURCE( "formatToolNoteJustifyBelowText",
	offsetof(NotesPageResources,nprJustifyMenuTexts[FTNjustifyBELOW_TEXT]),
	"Below Text" ),
    APP_RESOURCE( "formatToolNoteJustifyPageBottom",
	offsetof(NotesPageResources,nprJustifyMenuTexts[FTNjustifyPAGE_BOTTOM]),
	"Page Bottom" ),
    };

static void tedFormatToolGetNotesResourceTable(
			struct EditApplication *	ea,
			NotesPageResources *		npr )
    {
    static int	gotToolResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)npr,
				TED_TedNotesToolResourceTable,
				sizeof(TED_TedNotesToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedFootnotesToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    static int	gotSubjectResources= 0;

    tedFormatToolGetNotesResourceTable( ea, &TED_NotesPageResources );

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea,
				&TED_FootNotesSubjectResources,
				TED_TedFootnotesSubjectResourceTable,
				sizeof(TED_TedFootnotesSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    ist->istResources= &TED_FootNotesSubjectResources;

    ist->istBuildTool= tedFormatBuildFootNotesPage;
    ist->istFillChoosers= tedFormatFillFootNotesChoosers;
    ist->istFinishPage= tedFormatFinishNotesPage;
    ist->istFreeTool= tedFormatFreeNotesTool;

    ftst->ftstRefreshPage= tedRefreshNotesTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }

static InspectorSubjectResources	TED_EndNotesSubjectResources;

int tedEndnotesToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    static int	gotSubjectResources= 0;

    tedFormatToolGetNotesResourceTable( ea, &TED_NotesPageResources );

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea,
				&TED_EndNotesSubjectResources,
				TED_TedEndnotesSubjectResourceTable,
				sizeof(TED_TedEndnotesSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    ist->istResources= &TED_EndNotesSubjectResources;

    ist->istBuildTool= tedFormatBuildEndNotesPage;
    ist->istFillChoosers= tedFormatFillEndNotesChoosers;
    ist->istFinishPage= tedFormatFinishNotesPage;
    ftst->ftstRefreshPage= tedRefreshNotesTool;
    ist->istFreeTool= tedFormatFreeNotesTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }
