/************************************************************************/
/*									*/
/*  The 'Language' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	"tedLanguageTool.h"
#   include	<tedAppFront.h>
#   include	<guiToolUtil.h>
#   include	<docEditCommand.h>
#   include	<textMsLocale.h>
#   include	<textMsLcid.h>
#   include	<docExpandedTextAttribute.h>
#   include	<spellLocaleName.h>
#   include	<docDocumentProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<appEditApplication.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Language' page.					*/
/*									*/
/************************************************************************/

typedef enum ShowWhat
    {
    ShowLocale= 0,
    ShowDocument,
    ShowSelection,

    Show_COUNT
    } ShowWhat;

typedef struct LanguagePageResources
    {
    const char *	lprLanguage;
    const char *	lprDoNotCheck;

    const char *	lprUnknown;

    const char *	lprChangeSelectionText;

    const char *	lprShowText;
    const char *	lprShowWhat[Show_COUNT];

    const MsLcid *	lprLocales;
    int			lprLocaleCount;
    } LanguagePageResources;

/************************************************************************/
/*									*/
/*  A language tool, i.e. the 'Language' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct LanguageTool
    {
    struct EditApplication *		ltApplication;
    struct AppInspector *		ltInspector;
    const LanguagePageResources *	ltPageResources;

    int					ltInProgrammaticChange;

    unsigned char			ltCanChangeSelection;
    unsigned char			ltCanChangeDocument;

    ExpandedTextAttribute		ltTextAttributeSet;
    ExpandedTextAttribute		ltTextAttributeChosen;
    int					ltLocaleLanguageSet;
    int					ltDocumentLanguageSet;

    int					ltUnknownIdx;
    int					ltShowWhat;

    APP_WIDGET				ltShowFrame;
    APP_WIDGET				ltShowPaned;
    APP_WIDGET				ltShowLocaleToggle;
    APP_WIDGET				ltShowDocumentToggle;
    APP_WIDGET				ltShowSelectionToggle;

    /**/
    APP_WIDGET				ltLocaleList;
    APP_WIDGET				ltDoNotCheckToggle;

    APP_WIDGET				ltRevertButton;
    APP_WIDGET				ltChangeLanguageButton;

    APP_WIDGET				ltRevertButton2;
    APP_WIDGET				ltChangeDeflangButton;
    } LanguageTool;

static LanguagePageResources		TED_LanguagePageResources;
static InspectorSubjectResources	TED_LanguageSubjectResources;

static void tedInitLanguageTool(	LanguageTool *		lt )
    {
    memset( lt, 0, sizeof(LanguageTool) );
    }

/************************************************************************/

static void tedRefreshLanguageControls(	LanguageTool *	lt )
    {
    TextAttribute *	taChosen= &(lt->ltTextAttributeChosen.etaTextAttribute);
    int			listIdx;

    const LanguagePageResources *	lpr= lt->ltPageResources;

    if  ( taChosen->taLocaleId <= 0 )
	{ listIdx= lt->ltUnknownIdx;	}
    else{
	for ( listIdx= 0; listIdx < lpr->lprLocaleCount; listIdx++ )
	    {
	    if  ( lpr->lprLocales[listIdx].mlId == taChosen->taLocaleId )
		{ break;	}
	    }

	if  ( listIdx >= lpr->lprLocaleCount )
	    {
	    if  ( taChosen->taLocaleId != 0x400 )
		{
		XLLDEB(taChosen->taLocaleId,lpr->lprLocaleCount,listIdx);
		}

	    listIdx= lt->ltUnknownIdx;
	    }
	}

    lt->ltInProgrammaticChange++;

    guiSelectPositionInListWidget( lt->ltLocaleList, listIdx );

    guiSetToggleState( lt->ltDoNotCheckToggle, taChosen->taNoProof );

    guiEnableWidget( lt->ltChangeLanguageButton,
					lt->ltCanChangeSelection &&
					taChosen->taLocaleId > 0 );
    guiEnableWidget( lt->ltChangeDeflangButton,
					lt->ltCanChangeDocument &&
					taChosen->taLocaleId > 0 &&
					! taChosen->taNoProof );

    lt->ltInProgrammaticChange--;
    }

/************************************************************************/
/*									*/
/*  Adapt to the focus of the selection.				*/
/*									*/
/************************************************************************/

static void tedFormatShowWhat(	LanguageTool *	lt,
				int		what )
    {
    ExpandedTextAttribute *	etaC= &(lt->ltTextAttributeChosen);
    ExpandedTextAttribute *	etaS= &(lt->ltTextAttributeSet);
    TextAttribute *		taChosen= &(etaC->etaTextAttribute);

    switch( what )
	{
	case ShowLocale:
	    lt->ltShowWhat= ShowLocale;
	    taChosen->taLocaleId= lt->ltLocaleLanguageSet;
	    taChosen->taNoProof= 0;
	    break;

	case ShowDocument:
	    lt->ltShowWhat= ShowDocument;
	    taChosen->taLocaleId= lt->ltDocumentLanguageSet;
	    taChosen->taNoProof= 0;
	    break;

	case ShowSelection:
	    lt->ltShowWhat= ShowSelection;
	    docCopyExpandedTextAttribute( etaC, etaS );
	    break;

	default:
	    LDEB(what); return;
	}

    lt->ltInProgrammaticChange++;

    guiEnableWidget( lt->ltShowLocaleToggle, what != ShowLocale );
    guiEnableWidget( lt->ltShowDocumentToggle, what != ShowDocument );
    guiEnableWidget( lt->ltShowSelectionToggle, what != ShowSelection );

    if  ( what != ShowLocale )
	{ guiSetToggleState( lt->ltShowLocaleToggle, 0 );	}
    if  ( what != ShowDocument )
	{ guiSetToggleState( lt->ltShowDocumentToggle, 0 );	}
    if  ( what != ShowSelection )
	{ guiSetToggleState( lt->ltShowSelectionToggle, 0 );	}

    tedRefreshLanguageControls( lt );

    lt->ltInProgrammaticChange--;

    return;
    }
 
/************************************************************************/
/*									*/
/*  Adapt the what was just set.					*/
/*									*/
/************************************************************************/

static void tedLanguagetToolRevertToSet(	LanguageTool *	lt )
    {
    ExpandedTextAttribute *		etaC= &(lt->ltTextAttributeChosen);

    if  ( etaC->etaTextAttribute.taLocaleId > 0 )
	{
	lt->ltInProgrammaticChange++;
	tedFormatShowWhat( lt, ShowSelection );
	guiSetToggleState( lt->ltShowSelectionToggle, 1 );
	lt->ltInProgrammaticChange--;
	}
    else{
	if  ( lt->ltDocumentLanguageSet > 0 )
	    {
	    lt->ltInProgrammaticChange++;
	    tedFormatShowWhat( lt, ShowDocument );
	    guiSetToggleState( lt->ltShowDocumentToggle, 1 );
	    lt->ltInProgrammaticChange--;
	    }
	else{
	    lt->ltInProgrammaticChange++;
	    tedFormatShowWhat( lt, ShowLocale );
	    guiSetToggleState( lt->ltShowLocaleToggle, 1 );
	    lt->ltInProgrammaticChange--;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the tabs tool to the current selection in the document.	*/
/*									*/
/************************************************************************/

static void tedRefreshLanguageTool(
				void *				vlt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    LanguageTool *			lt= (LanguageTool *)vlt;
    const DocumentProperties *		dp= bd->bdProperties;

    PropertyMask			updMask;
    PropertyMask			doneMask;

    ExpandedTextAttribute *		etaC= &(lt->ltTextAttributeChosen);
    ExpandedTextAttribute *		etaS= &(lt->ltTextAttributeSet);

    if  ( sd->sdIsObjectSelection || sd->sdIsListBullet )
	{ *pEnabled= 0; return;	}

    utilPropMaskClear( &updMask );
    PROPmaskADD( &updMask, TApropLOCALE );
    PROPmaskADD( &updMask, TApropNOPROOF );
    utilPropMaskOr( &updMask, &updMask, &(sd->sdTextAttributeMask) );

    utilPropMaskClear( &doneMask );
    docExpandTextAttribute( &doneMask, etaS,
			&(sd->sdTextAttribute), &updMask,
			dp->dpFontList, dp->dpColorPalette );
    docCopyExpandedTextAttribute( etaC, etaS );

    lt->ltDocumentLanguageSet= dp->dpDefaultLocaleId;

    /**/

    lt->ltCanChangeSelection= cmdEnabled[EDITcmdUPD_SPAN_PROPS];
    lt->ltCanChangeDocument= cmdEnabled[EDITcmdUPD_DOC_PROPS];

    tedLanguagetToolRevertToSet( lt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the tabs tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevert, w, voidlt )
    {
    LanguageTool *		lt= (LanguageTool *)voidlt;

    tedLanguagetToolRevertToSet( lt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedLanguageDoNotCheckToggled, w, voidlt, voidtbcs )
    {
    LanguageTool *	lt= (LanguageTool *)voidlt;
    TextAttribute *	taChosen= &(lt->ltTextAttributeChosen.etaTextAttribute);
    int			set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    taChosen->taNoProof= ( set != 0 );

    tedRefreshLanguageControls( lt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatShowLocaleChosen, w, voidlt, voidtbcs )
    {
    LanguageTool *	lt= (LanguageTool *)voidlt;
    int			set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( lt->ltInProgrammaticChange || ! set )
	{ return;	}

    tedFormatShowWhat( lt, ShowLocale );
    }

static APP_TOGGLE_CALLBACK_H( tedFormatShowDocumentChosen, w, voidlt, voidtbcs )
    {
    LanguageTool *	lt= (LanguageTool *)voidlt;
    int			set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( lt->ltInProgrammaticChange || ! set )
	{ return;	}

    tedFormatShowWhat( lt, ShowDocument );
    }

static APP_TOGGLE_CALLBACK_H( tedFormatShowSelectionChosen, w, voidlt, voidtbcs )
    {
    LanguageTool *	lt= (LanguageTool *)voidlt;
    int			set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( lt->ltInProgrammaticChange || ! set )
	{ return;	}

    tedFormatShowWhat( lt, ShowSelection );
    }

/************************************************************************/
/*									*/
/*  A language was selected in the list of positions.			*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedLanguageToolLanguageChosen, w, voidlt, voidlcs )
    {
    LanguageTool *			lt= (LanguageTool *)voidlt;
    const LanguagePageResources *	lpr= lt->ltPageResources;
    int					position;

    TextAttribute *	taChosen= &(lt->ltTextAttributeChosen.etaTextAttribute);

    if  ( lt->ltInProgrammaticChange )
	{ return;	}

    position= guiGetPositionFromListCallback( w, voidlcs );

    if  ( position < 0 || position >= lpr->lprLocaleCount )
	{ LLDEB(position,lpr->lprLocaleCount); return;	}

    taChosen->taLocaleId= lpr->lprLocales[position].mlId;

    tedRefreshLanguageControls( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the language of the selection.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeLanguage, w, voidlt )
    {
    LanguageTool *		lt= (LanguageTool *)voidlt;
    struct EditApplication *		ea= lt->ltApplication;

    PropertyMask		taSetMask;

    utilPropMaskClear( &taSetMask );
    PROPmaskADD( &taSetMask, TApropNOPROOF );

    if  ( lt->ltTextAttributeChosen.etaTextAttribute.taLocaleId > 0 )
	{ PROPmaskADD( &taSetMask, TApropLOCALE );	}

    tedAppFontToolSet( ea, &taSetMask, &(lt->ltTextAttributeChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' default language for the document has been pushed.	*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeDeflang, w, voidlt )
    {
    LanguageTool *		lt= (LanguageTool *)voidlt;
    struct EditApplication *		ea= lt->ltApplication;

    DocumentProperties		dpSet;
    PropertyMask		dpSetMask;

    docInitDocumentProperties( &dpSet );

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropDEFLANG );

    dpSet.dpDefaultLocaleId=
		lt->ltTextAttributeChosen.etaTextAttribute.taLocaleId;

    tedAppSetDocumentProperties( ea, &dpSetMask, &dpSet );

    docCleanDocumentProperties( &dpSet );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the language page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildLanguagePage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr )
    {
    const LanguagePageResources * lpr= &TED_LanguagePageResources;

    APP_WIDGET		row;

    const int		heightResizable= 0;
    const int		visibleItems= 5;
    int			i;

    LanguageTool *	lt= malloc( sizeof(LanguageTool) );

    if  ( ! lt )
	{ XDEB(lt); return lt;	}

    tedInitLanguageTool( lt );

    /**/
    lt->ltPageResources= lpr;
    lt->ltInspector= ai;
    lt->ltApplication= ea;

    /**/
    docInitExpandedTextAttribute( &(lt->ltTextAttributeSet) );
    docInitExpandedTextAttribute( &(lt->ltTextAttributeChosen) );

    lt->ltInProgrammaticChange= 0;

    guiMakeListInColumn( &(lt->ltLocaleList),
				pageWidget, visibleItems,
				tedLanguageToolLanguageChosen,
				(APP_BUTTON_CALLBACK_T)0, (void *)lt );

    row= guiMakeRowInColumn( pageWidget, 1, heightResizable );
    lt->ltDoNotCheckToggle= guiMakeToggleInRow( row, lpr->lprDoNotCheck,
		tedLanguageDoNotCheckToggled, (void *)lt, 0, 1 );

    guiMakeColumnFrameInColumn( &(lt->ltShowFrame), &(lt->ltShowPaned),
						pageWidget, lpr->lprShowText );

    guiToolMake3ToggleRow( &row, lt->ltShowPaned,
				&(lt->ltShowLocaleToggle),
				&(lt->ltShowDocumentToggle),
				&(lt->ltShowSelectionToggle),
				lpr->lprShowWhat[ShowLocale],
				lpr->lprShowWhat[ShowDocument],
				lpr->lprShowWhat[ShowSelection],
				tedFormatShowLocaleChosen,
				tedFormatShowDocumentChosen,
				tedFormatShowSelectionChosen,
				(void *)lt );

    guiToolMake2BottonRow( &row, pageWidget,
				&(lt->ltRevertButton),
				&(lt->ltChangeLanguageButton),
				isr->isrRevert,
				lpr->lprChangeSelectionText,
				tedFormatRevert, /* same revert call */
				tedFormatChangeLanguage,
				(void *)lt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
				&(is->isRevertButton),
				&(is->isApplyButton),
				isr->isrRevert,
				isr->isrApplyToSubject,
				tedFormatRevert, /* same revert call */
				tedFormatChangeDeflang,
				(void *)lt );

    lt->ltRevertButton2= is->isRevertButton;
    lt->ltChangeDeflangButton= is->isApplyButton;

    for ( i= 0; i < lpr->lprLocaleCount; i++ )
	{
	guiAddValueToListWidget( lt->ltLocaleList,
				    i, lpr->lprLocales[i].mlLocaleName );
	}

    lt->ltUnknownIdx= i++;

    guiAddValueToListWidget( lt->ltLocaleList,
				       lt->ltUnknownIdx, lpr->lprUnknown );

    lt->ltLocaleLanguageSet= 0;
    if  ( ea->eaLocaleTag )
	{
	int	lcId;

	lcId= textGetMsLocaleIdByTag( ea->eaLocaleTag );
	if  ( lcId < 0 )
	    { SLDEB(ea->eaLocaleTag,lcId);	}
	else{ lt->ltLocaleLanguageSet= lcId;	}
	}

    return (void *)lt;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a language tool.			*/
/*									*/
/************************************************************************/

static void tedFormatFreeLanguageTool(	void *	vlt )
    {
    LanguageTool *			lt= (LanguageTool *)vlt;

    docCleanExpandedTextAttribute( &(lt->ltTextAttributeSet) );
    docCleanExpandedTextAttribute( &(lt->ltTextAttributeChosen) );

    free( lt );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get tabs tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedLanguageSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolLanguage",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Language" ),

    APP_RESOURCE( "formatToolChangeDefaultLanguage",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "formatToolRevertLanguage",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedLanguageToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolChangeLanguage",
		offsetof(LanguagePageResources,lprChangeSelectionText),
		"Apply to Selection" ),

    APP_RESOURCE( "formatToolLanguage",
		offsetof(LanguagePageResources,lprLanguage),
		"Language" ),
    APP_RESOURCE( "languageToolDoNotCheck",
		offsetof(LanguagePageResources,lprDoNotCheck),
		"Do not Check" ),

    APP_RESOURCE( "languageToolUnknown",
		offsetof(LanguagePageResources,lprUnknown),
		"Unknown" ),

    APP_RESOURCE( "languageToolShow",
		offsetof(LanguagePageResources,lprShowText),
		"Show" ),
    APP_RESOURCE( "languageToolShowLocale",
		offsetof(LanguagePageResources,lprShowWhat[ShowLocale]),
		"Locale" ),
    APP_RESOURCE( "languageToolShowDocument",
		offsetof(LanguagePageResources,lprShowWhat[ShowDocument]),
		"Document" ),
    APP_RESOURCE( "languageToolShowSelection",
		offsetof(LanguagePageResources,lprShowWhat[ShowSelection]),
		"Selection" ),
    };

static int tedSpellCompareLocaleNames(	const void *	vml1,
					const void *	vml2 )
    {
    const MsLcid *	ml1= (const MsLcid *)vml1;
    const MsLcid *	ml2= (const MsLcid *)vml2;

    return strcmp( ml1->mlLocaleName, ml2->mlLocaleName );
    }

static void tedFormatToolGetLanguageResourceTable(
					struct EditApplication *	ea,
					LanguagePageResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	MsLcid *	ml;
	int		n;

	appGuiGetResourceValues( &gotToolResources, ea, (void *)lpr,
				TED_TedLanguageToolResourceTable,
				sizeof(TED_TedLanguageToolResourceTable)/
				sizeof(AppConfigurableResource) );

	if  ( ! appGetLocaleNames( &ml, &n, ea ) )
	    {
	    qsort( ml, n, sizeof(MsLcid), tedSpellCompareLocaleNames );

	    lpr->lprLocales= ml;
	    lpr->lprLocaleCount= n;
	    }
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedLanguageSubjectResourceTable,
				sizeof(TED_TedLanguageSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedLanguageToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetLanguageResourceTable( ea,
				&TED_LanguagePageResources,
				&TED_LanguageSubjectResources );

    ist->istResources= &TED_LanguageSubjectResources;

    ist->istBuildTool= tedFormatBuildLanguagePage;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ftst->ftstRefreshPage= tedRefreshLanguageTool;
    ist->istFreeTool= tedFormatFreeLanguageTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }
