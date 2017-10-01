/************************************************************************/
/*									*/
/*  A tool to manage the headers and footers of a Document/Section.	*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"tedHeaderFooterTool.h"
#   include	<tedAppFront.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docHeaderFooterScopes.h>
#   include	<docDocumentProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>
#   include	<appInspector.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<docSectProperties.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A Header/Footer Tool. I.E. The header footer page on the 'Format	*/
/*  Tool'								*/
/*									*/
/************************************************************************/

typedef struct HeaderFooterPageResources
    {
    const char *	hfprSectionHeaderText;
    const char *	hfprSectionNumberText;
    const char *	hfprTitlepgText;
    const char *	hfprEndpgText;

    const char *	hfprDocumentHeaderText;
    const char *	hfprFacingpText;
    const char *	hfprChangeDocText;
    const char *	hfprRevertDocText;

    const char *	hfprPagesHeaderText;
    const char *	hfprPagesOptionTexts[PAGES__COUNT];

    const char *	hfprDeleteHeaderText;
    const char *	hfprEditHeaderText;
    const char *	hfprDeleteFooterText;
    const char *	hfprEditFooterText;

    const char *	hfprSupportEndpgString;
    } HeaderFooterPageResources;

typedef struct HeaderFooterTool
    {
    struct EditApplication *		hftApplication;
    struct AppInspector *		hftInspector;
    const HeaderFooterPageResources *	hftPageResources;

    int					hftSectionNumber;

    unsigned char			hftCanInsertHeader;
    unsigned char			hftCanDeleteHeader;
    unsigned char			hftCanInsertFooter;
    unsigned char			hftCanDeleteFooter;

    DocumentProperties			hftDocPropertiesChosen;
    DocumentProperties			hftDocPropertiesSet;

    SectionProperties			hftSectPropertiesChosen;
    SectionProperties			hftSectPropertiesSet;
    int					hftPagesChosen;
    int					hftInTreeType;
    unsigned char			hftPagesOptionsEnabled[PAGES__COUNT];
    unsigned char			hftPagesHeaderExists[PAGES__COUNT];
    unsigned char			hftPagesFooterExists[PAGES__COUNT];
    unsigned char			hftPagesHeaderApplies[PAGES__COUNT];
    unsigned char			hftPagesFooterApplies[PAGES__COUNT];

    APP_WIDGET				hftSectionFrame;
    APP_WIDGET				hftSectionPaned;
    APP_WIDGET				hftSectionNumberLabel;
    APP_WIDGET				hftSectionNumberText;

    APP_WIDGET				hftPagesRow;
    APP_WIDGET					hftTitlepgToggle;
    APP_WIDGET					hftEndpgToggle;

    APP_WIDGET				hftDocumentFrame;
    APP_WIDGET				hftDocumentPaned;
    APP_WIDGET				hftFacingpToggle;
    APP_WIDGET				hftRevertDocButton;
    APP_WIDGET				hftChangeDocButton;

    APP_WIDGET				hftPagesFrame;
    APP_WIDGET				hftPagesPaned;
    AppOptionmenu			hftPagesOptionmenu;

    APP_WIDGET				hftDeleteHeaderButton;
    APP_WIDGET				hftEditHeaderButton;
    APP_WIDGET				hftDeleteFooterButton;
    APP_WIDGET				hftEditFooterButton;
    } HeaderFooterTool;

static HeaderFooterPageResources	TED_HeaderFooterPageResources;
static InspectorSubjectResources	TED_HeaderFooterSubjectResources;

/************************************************************************/
/*									*/
/*  Clean the header footer tool.					*/
/*									*/
/************************************************************************/

static void tedInitHeaderFooterTool(	HeaderFooterTool *	hft )
    {
    memset( hft, 0, sizeof( HeaderFooterTool ) );
    }

static void tedFreeHeaderFooterTool(	void *	vhft )
    {
    HeaderFooterTool *			hft= (HeaderFooterTool *)vhft;

    docCleanDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docCleanDocumentProperties( &(hft->hftDocPropertiesSet) );
    docCleanSectionProperties( &(hft->hftSectPropertiesChosen) );
    docCleanSectionProperties( &(hft->hftSectPropertiesSet) );

    free( hft );

    return;
    }

/************************************************************************/
/*									*/
/*  Enable/Disable the options for the set of pages that apply		*/
/*  depending on the section- and document properties.			*/
/*									*/
/************************************************************************/

static void docHeaderFooterToolEnablePagesOptions(
				HeaderFooterTool *		hft )
    {
    const DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);
    const SectionProperties *	sp= &(hft->hftSectPropertiesChosen);
    int				i;

    hft->hftPagesOptionsEnabled[PAGES_FIRST_PAGE]=
					sp->spHasTitlePage;
    hft->hftPagesOptionsEnabled[PAGES_SUBSEQUENT_PAGES]=
					sp->spHasTitlePage	&&
					! dp->dpHasFacingPages;
    hft->hftPagesOptionsEnabled[PAGES_ALL_PAGES]=
					! sp->spHasTitlePage	&&
					! dp->dpHasFacingPages;
    hft->hftPagesOptionsEnabled[PAGES_ODD_PAGES]=
					dp->dpHasFacingPages;
    hft->hftPagesOptionsEnabled[PAGES_EVEN_PAGES]=
					dp->dpHasFacingPages;
    hft->hftPagesOptionsEnabled[PAGES_LAST_PAGE]=
					sp->spHasEndPage;

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	guiOptionMenuEnablePosition( &(hft->hftPagesOptionmenu), i,
					hft->hftPagesOptionsEnabled[i] );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set all the controls on the Header/Footer tool, to reflect the	*/
/*  section properties currently chosen.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshHeadFootPage(	HeaderFooterTool *	hft )
    {
    const SectionProperties *	sp= &(hft->hftSectPropertiesChosen);
    const DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    guiIntegerToText( hft->hftSectionNumberText,
						hft->hftSectionNumber+ 1 );

    guiSetToggleState( hft->hftTitlepgToggle, sp->spHasTitlePage );
    guiSetToggleState( hft->hftEndpgToggle, sp->spHasEndPage );
    guiSetToggleState( hft->hftFacingpToggle, dp->dpHasFacingPages );

    docHeaderFooterToolEnablePagesOptions( hft );

    if  ( hft->hftPagesChosen < 0				||
	  hft->hftPagesChosen >= PAGES__COUNT			||
	  ! hft->hftPagesOptionsEnabled[hft->hftPagesChosen]	)
	{
	int		i;

	for ( i= 0; i < PAGES__COUNT; i++ )
	    {
	    if  ( hft->hftPagesOptionsEnabled[i] )
		{
		guiSetOptionmenu( &(hft->hftPagesOptionmenu), i );
		hft->hftPagesChosen= i;
		break;
		}
	    }
	}

    if  ( hft->hftPagesChosen >= 0 )
	{
	int headerType= DOC_HeaderScopes[hft->hftPagesChosen];
	int footerType= DOC_FooterScopes[hft->hftPagesChosen];

	guiEnableWidget( hft->hftDeleteHeaderButton,
			    hft->hftCanDeleteHeader &&
			    hft->hftPagesHeaderExists[hft->hftPagesChosen] );
	guiEnableWidget( hft->hftDeleteFooterButton,
			    hft->hftCanDeleteFooter &&
			    hft->hftPagesFooterExists[hft->hftPagesChosen] );

	guiEnableWidget( hft->hftEditHeaderButton,
			hft->hftInTreeType != headerType &&
			(  hft->hftCanInsertHeader ||
			   hft->hftPagesHeaderExists[hft->hftPagesChosen] ) );
	guiEnableWidget( hft->hftEditFooterButton,
			hft->hftInTreeType != footerType &&
			(  hft->hftCanInsertFooter ||
			   hft->hftPagesFooterExists[hft->hftPagesChosen] ) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the Header/Footer tool to the current selection in the text.	*/
/*									*/
/************************************************************************/

static void tedRefreshHeaderFooterTool(
				void *				vhft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)vhft;

    const DocumentProperties *	dpSet= bd->bdProperties;
    PropertyMask		dpSetMask;

    struct BufferItem *		selSectNode;
    const struct BufferItem *		bodySectNode;

    const SectionProperties *	spSet;
    PropertyMask		spSetMask;

    int				whatHeaderFooter;
    int				i;

    const struct DocumentAttributeMap * const	dam0=
				(const struct DocumentAttributeMap *)0;

    selSectNode= docGetSectNode( ds->dsHead.dpNode );
    if  ( ! selSectNode )
	{ XDEB(selSectNode); return;	}
    bodySectNode= docGetBodySectNode( selSectNode, bd );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); *pEnabled= 0; return;	}

    whatHeaderFooter= hft->hftInTreeType= selSectNode->biTreeType;
    if  ( sd->sdInHeaderFooter )
	{
	whatHeaderFooter= selSectNode->biTreeType;
	hft->hftSectionNumber= bodySectNode->biNumberInParent;
	spSet= bodySectNode->biSectProperties;

	(*pPref) += 2;
	}
    else{
	if  ( ! sd->sdInDocumentBody )
	    { *pEnabled= 0; return;	}

	whatHeaderFooter= sd->sdHeaderTypeForSelection;
	hft->hftSectionNumber= selSectNode->biNumberInParent;
	spSet= selSectNode->biSectProperties;
	}

    /**********/

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( (PropertyMask *)0,
		    &(hft->hftDocPropertiesChosen), &dpSetMask, dpSet, dam0 ) )
	{ LDEB(1); return;	}
    if  ( docUpdDocumentProperties( (PropertyMask *)0,
		    &(hft->hftDocPropertiesSet), &dpSetMask, dpSet, dam0 ) )
	{ LDEB(1); return;	}

    utilPropMaskClear( &spSetMask );
    PROPmaskADD( &spSetMask, SPpropTITLEPG );
    PROPmaskADD( &spSetMask, SPpropENDPG );

    if  ( docUpdSectProperties( (PropertyMask *)0,
		    &(hft->hftSectPropertiesChosen), &spSetMask, spSet ) )
	{ LDEB(1); return;	}
    if  ( docUpdSectProperties( (PropertyMask *)0,
		    &(hft->hftSectPropertiesSet), &spSetMask, spSet ) )
	{ LDEB(1); return;	}

    /**********/

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	hft->hftPagesHeaderExists[i]= 0;
	hft->hftPagesFooterExists[i]= 0;

	if  ( docSectionHasHeaderFooter( bodySectNode,
					&(hft->hftPagesHeaderApplies[i]),
					&(hft->hftDocPropertiesSet),
					DOC_HeaderScopes[i] ) )
	    { hft->hftPagesHeaderExists[i]= 1;	}
	if  ( docSectionHasHeaderFooter( bodySectNode,
					&(hft->hftPagesFooterApplies[i]),
					&(hft->hftDocPropertiesSet),
					DOC_FooterScopes[i] ) )
	    { hft->hftPagesFooterExists[i]= 1;	}
	}

    if  ( sd->sdInHeaderFooter )
	{
	guiEnableWidget( hft->hftSectionFrame, 0 );
	guiEnableWidget( hft->hftDocumentFrame, 0 );

	guiEnableOptionmenu( &(hft->hftPagesOptionmenu), 0 );
	}
    else{
	guiEnableWidget( hft->hftSectionFrame,
					cmdEnabled[EDITcmdUPD_SECT_PROPS] );
	guiEnableWidget( hft->hftDocumentFrame,
					cmdEnabled[EDITcmdUPD_DOC_PROPS] );

	guiEnableOptionmenu( &(hft->hftPagesOptionmenu), 1 );
	}

    if  ( sd->sdInHeaderFooter )
	{
	int	pages;

	pages= docWhatPagesForHeaderFooter( dpSet, spSet, whatHeaderFooter );
	if  ( pages < 0 )
	    { LLDEB(whatHeaderFooter,pages); *pEnabled= 0; return;	}
	hft->hftPagesChosen= pages;
	hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;

	guiSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );
	}

    hft->hftCanInsertHeader= cmdEnabled[EDITcmdINSERT_HEADER];
    hft->hftCanDeleteHeader= cmdEnabled[EDITcmdDELETE_HEADER];
    hft->hftCanInsertFooter= cmdEnabled[EDITcmdINSERT_FOOTER];
    hft->hftCanDeleteFooter= cmdEnabled[EDITcmdDELETE_FOOTER];

    tedFormatToolRefreshHeadFootPage( hft );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dpSet= &(hft->hftDocPropertiesChosen);

    PropertyMask		dpSetMask;

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    tedAppSetDocumentProperties( hft->hftApplication, &dpSetMask, dpSet );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);

    PropertyMask		update;

    utilPropMaskClear( &update );
    PROPmaskADD( &update, SPpropTITLEPG );
    PROPmaskADD( &update, SPpropENDPG );

    tedAppChangeSectionProperties( hft->hftApplication, &update, sp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    PropertyMask		dpSetMask;

    const struct DocumentAttributeMap * const	dam0=
				(const struct DocumentAttributeMap *)0;

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( (PropertyMask *)0, dp,
			    &dpSetMask, &(hft->hftDocPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    PropertyMask		spSetMask;

    utilPropMaskClear( &spSetMask );
    PROPmaskADD( &spSetMask, SPpropTITLEPG );
    PROPmaskADD( &spSetMask, SPpropENDPG );

    if  ( docUpdSectProperties(
			    (PropertyMask *)0, &(hft->hftSectPropertiesChosen),
			    &spSetMask, &(hft->hftSectPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Titlepg' toggle was pushed.					*/
/*  The 'Facingp' toggle was pushed.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleTitlepg, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    sp->spHasTitlePage= ( set != 0 );

    docHeaderFooterToolEnablePagesOptions( hft );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleEndpg, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    sp->spHasEndPage= ( set != 0 );

    docHeaderFooterToolEnablePagesOptions( hft );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleFacingp, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    dp->dpHasFacingPages= ( set != 0 );

    docHeaderFooterToolEnablePagesOptions( hft );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of pages was chosen in the optionmenu.			*/
/*									*/
/************************************************************************/

static void tedHeadFootPagesChosen(	int		pages,
					void *		voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( pages < 0 || pages >= PAGES__COUNT )
	{ LLDEB(pages,PAGES__COUNT); return;		}

    hft->hftPagesChosen= pages;

    return;
    }


/************************************************************************/
/*									*/
/*  Delete/Edit Headers/Footers.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditHeaderButton,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditFooterButton,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'HeaderFooter' page of the format tool.			*/
/*									*/
/************************************************************************/

static void * tedFormatBuildHeaderFooterPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const HeaderFooterPageResources *	hfpr= &TED_HeaderFooterPageResources;

    APP_WIDGET	row= (APP_WIDGET )0;

    const int	heightResizable= 0;
    const int	textColumns= 5;

    HeaderFooterTool *	hft= malloc( sizeof(HeaderFooterTool) );

    if  ( ! hft )
	{ XDEB(hft); return hft;	}

    tedInitHeaderFooterTool( hft );

    /**/
    hft->hftPageResources= hfpr;
    hft->hftInspector= ai;
    hft->hftApplication= ea;

    /**/
    docInitDocumentProperties( &(hft->hftDocPropertiesSet) );
    docInitDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docInitSectionProperties( &(hft->hftSectPropertiesSet) );
    docInitSectionProperties( &(hft->hftSectPropertiesChosen) );

    /**/

    guiMakeColumnFrameInColumn( &(hft->hftSectionFrame),
		&(hft->hftSectionPaned),
		pageWidget, hfpr->hfprSectionHeaderText );

    guiToolMakeLabelAndTextRow( &row, &(hft->hftSectionNumberLabel),
		&(hft->hftSectionNumberText),
		hft->hftSectionPaned, hfpr->hfprSectionNumberText,
		textColumns, 0 );

    guiToolMake2ToggleRow( &(hft->hftPagesRow), hft->hftSectionPaned,
		&(hft->hftTitlepgToggle), &(hft->hftEndpgToggle),
		hfpr->hfprTitlepgText, hfpr->hfprEndpgText,
		tedHeaderFooterToggleTitlepg, tedHeaderFooterToggleEndpg,
		(void *)hft, (void *)hft );

    guiToolMake2BottonRow( &(is->isApplyRow), hft->hftSectionPaned,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedHeadFootRevertSectPushed, tedHeadFootChangeSectPushed,
		(void *)hft );

    /**************/

    guiMakeColumnFrameInColumn( &(hft->hftDocumentFrame),
		&(hft->hftDocumentPaned),
		pageWidget, hfpr->hfprDocumentHeaderText );

    row= guiMakeRowInColumn( hft->hftDocumentPaned, 1, heightResizable );
    hft->hftFacingpToggle= guiMakeToggleInRow( row, hfpr->hfprFacingpText,
		tedHeaderFooterToggleFacingp, (void *)hft, 0, 1 );

    guiToolMake2BottonRow( &row, hft->hftDocumentPaned,
		&(hft->hftRevertDocButton), &(hft->hftChangeDocButton),
		hfpr->hfprRevertDocText, hfpr->hfprChangeDocText,
		tedHeadFootRevertDocPushed, tedHeadFootChangeDocPushed,
		(void *)hft );

    /**************/

    guiMakeColumnFrameInColumn( &(hft->hftPagesFrame),
			    &(hft->hftPagesPaned),
			    pageWidget, hfpr->hfprPagesHeaderText );

    guiMakeOptionmenuInColumn( &(hft->hftPagesOptionmenu), hft->hftPagesPaned,
					tedHeadFootPagesChosen, (void *)hft );

    guiToolMake2BottonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteHeaderButton), &(hft->hftEditHeaderButton),
		hfpr->hfprDeleteHeaderText, hfpr->hfprEditHeaderText,
		tedHeadFootDeleteHeaderPushed, tedHeadFootEditHeaderPushed,
		(void *)hft );

    guiToolMake2BottonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteFooterButton), &(hft->hftEditFooterButton),
		hfpr->hfprDeleteFooterText, hfpr->hfprEditFooterText,
		tedHeadFootDeleteFooterPushed, tedHeadFootEditFooterPushed,
		(void *)hft );

    /**************/

    guiEnableWidget( hft->hftEndpgToggle,
			hfpr->hfprSupportEndpgString &&
			! strcmp( hfpr->hfprSupportEndpgString, "1" ) );

    return (void *)hft;
    }

/************************************************************************/
/*									*/
/*  Finish the Header/Footer page.					*/
/*									*/
/************************************************************************/

static void tedFormatFillHeaderFooterChoosers( void *		vhft )
    {
    HeaderFooterTool *			hft= (HeaderFooterTool *)vhft;
    const HeaderFooterPageResources *	hfpr= &TED_HeaderFooterPageResources;

    appFillInspectorMenu( PAGES__COUNT, PAGES_ALL_PAGES,
		    hfpr->hfprPagesOptionTexts, &(hft->hftPagesOptionmenu) );

    hft->hftPagesChosen= PAGES_ALL_PAGES;

    return;
    }

static void tedFormatFinishHeaderFooterPage(	void *	vhft )
    {
    HeaderFooterTool *			hft= (HeaderFooterTool *)vhft;

    guiOptionmenuRefreshWidth( &(hft->hftPagesOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Header/Footer tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedHeaderFooterSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeaderFooter",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Headers and Footers" ),

    APP_RESOURCE( "formatToolHeadFootChangeSection",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Section" ),
    APP_RESOURCE( "formatToolHeadFootRevertSection",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedHeaderFooterToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeadFootSectionHeader",
		offsetof(HeaderFooterPageResources,hfprSectionHeaderText),
		"Section Properties" ),
    APP_RESOURCE( "formatToolHeadFootSectionNumber",
		offsetof(HeaderFooterPageResources,hfprSectionNumberText),
		"Section" ),
    APP_RESOURCE( "formatToolHeadFootTitlepgText",
		offsetof(HeaderFooterPageResources,hfprTitlepgText),
		"Different first Page" ),
    APP_RESOURCE( "formatToolHeadFootEndpgText",
		offsetof(HeaderFooterPageResources,hfprEndpgText),
		"Different last Page" ),

    APP_RESOURCE( "formatToolHeadFootDocumentHeader",
		offsetof(HeaderFooterPageResources,hfprDocumentHeaderText),
		"Document Properties" ),
    APP_RESOURCE( "formatToolHeadFootFacingpText",
		offsetof(HeaderFooterPageResources,hfprFacingpText),
		"Different odd and even Pages" ),
    APP_RESOURCE( "formatToolHeadFootChangeDocument",
		offsetof(HeaderFooterPageResources,hfprChangeDocText),
		"Apply to Document" ),
    APP_RESOURCE( "formatToolHeadFootRevertDocument",
		offsetof(HeaderFooterPageResources,hfprRevertDocText),
		"Revert" ),

    APP_RESOURCE( "formatToolHeadFootPagesHeader",
		offsetof(HeaderFooterPageResources,hfprPagesHeaderText),
		"Pages in Section" ),

    APP_RESOURCE( "formatToolHeadFootFirstPage",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_FIRST_PAGE]),
		"First Page" ),
    APP_RESOURCE( "formatToolHeadFootSubsequentPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_SUBSEQUENT_PAGES]),
		"Subsequent Pages" ),
    APP_RESOURCE( "formatToolHeadFootAllPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ALL_PAGES]),
		"All Pages" ),
    APP_RESOURCE( "formatToolHeadFootOddPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ODD_PAGES]),
		"Odd Pages" ),
    APP_RESOURCE( "formatToolHeadFootEvenPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_EVEN_PAGES]),
		"Even Pages" ),
    APP_RESOURCE( "formatToolHeadFootLastPage",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_LAST_PAGE]),
		"Last Page" ),

    APP_RESOURCE( "formatToolHeadFootDeleteHeader",
		offsetof(HeaderFooterPageResources,hfprDeleteHeaderText),
		"Delete Header" ),
    APP_RESOURCE( "formatToolHeadFootEditHeader",
		offsetof(HeaderFooterPageResources,hfprEditHeaderText),
		"Edit Header" ),
    APP_RESOURCE( "formatToolHeadFootDeleteFooter",
		offsetof(HeaderFooterPageResources,hfprDeleteFooterText),
		"Delete Footer" ),
    APP_RESOURCE( "formatToolHeadFootEditFooter",
		offsetof(HeaderFooterPageResources,hfprEditFooterText),
		"Edit Footer" ),

    APP_RESOURCE( "formatToolSupportEndpg",
		offsetof(HeaderFooterPageResources,hfprSupportEndpgString),
		"0" ),
    };

static void tedFormatToolGetHeaderFooterResourceTable(
					struct EditApplication *		ea,
					HeaderFooterPageResources *	hfpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)hfpr,
				TED_TedHeaderFooterToolResourceTable,
				sizeof(TED_TedHeaderFooterToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedHeaderFooterSubjectResourceTable,
				sizeof(TED_TedHeaderFooterSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedHeaderFooterToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetHeaderFooterResourceTable( ea,
					&TED_HeaderFooterPageResources,
					&TED_HeaderFooterSubjectResources );

    ist->istResources= &TED_HeaderFooterSubjectResources;

    ist->istBuildTool= tedFormatBuildHeaderFooterPage;
    ist->istFillChoosers= tedFormatFillHeaderFooterChoosers;
    ist->istFinishPage= tedFormatFinishHeaderFooterPage;
    ist->istFreeTool= tedFreeHeaderFooterTool;

    ftst->ftstRefreshPage= tedRefreshHeaderFooterTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }
