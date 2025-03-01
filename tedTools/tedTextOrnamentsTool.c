/************************************************************************/
/*									*/
/*  The 'Text' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"tedTextOrnamentsTool.h"
#   include	<tedAppFront.h>
#   include	<docEditCommand.h>
#   include	<appColorChooser.h>
#   include	"tedShadingTool.h"
#   include	"tedBorderTool.h"
#   include	<docDocumentProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	"tedFormatToolSubject.h"
#   include	<docExpandedTextAttribute.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the text colors and borders tool.			*/
/*									*/
/************************************************************************/

typedef struct TextOrnamentsPageResources
    {
    char *			totrTextColor;
    char *			totrTextBorder;
    char *			totrTextShading;

    ColorChooserResources	totrTextColorChooserResources;
    ShadingToolResources	totrShadingResources;
    BorderToolResources		totrBorderToolResources;
    } TextOrnamentsPageResources;

/************************************************************************/
/*									*/
/*  A text color tool, i.e. the 'Text Colors' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct TextOrnamentsTool
    {
    struct EditApplication *			totApplication;
    struct AppInspector *			totInspector;
    int						totSubjectPage;
    int						totCurrentDocumentId;
    const TextOrnamentsPageResources *		totPageResources;

    unsigned char				totCanChange;
    unsigned char				totIsListBullet;

    PropertyMask				totSetMask;
    ExpandedTextAttribute			totAttributeSet;
    PropertyMask				totChosenMask;
    ExpandedTextAttribute			totAttributeChosen;

    APP_WIDGET					totColorRow;
    APP_WIDGET					totTextColorToggle;
    ColorChooser				totTextColorChooser;

    BorderTool					totBorderTool;
    ShadingTool					totShadingTool;

    APP_WIDGET					totSetButton;
    } TextOrnamentsTool;

static TextOrnamentsPageResources	TED_TextOrnamentsPageResources;
static InspectorSubjectResources	TED_TextOrnamentsSubjectResources;

/************************************************************************/
/*									*/
/*  Init/Clean the text ornaments tool.					*/
/*									*/
/************************************************************************/

static void tedInitTextOrnamentsTool(	TextOrnamentsTool *	tot )
    {
    memset( tot, 0, sizeof(TextOrnamentsTool) );
    }

static void tedFreeTextOrnamentsTool(	void *	vtot )
    {
    TextOrnamentsTool *	tot= (TextOrnamentsTool *)vtot;

    docCleanExpandedTextAttribute( &(tot->totAttributeSet) );
    docCleanExpandedTextAttribute( &(tot->totAttributeChosen) );

    appCleanColorChooser( &(tot->totTextColorChooser) );
    tedCleanBorderTool( &(tot->totBorderTool) );
    tedCleanShadingTool( &(tot->totShadingTool) );

    free( tot );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the controls on the paragraph properties page.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTextOrnamentsPage(
					TextOrnamentsTool *	tot )
    {
    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, tot->totApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    /*  6  */
    if  ( PROPmaskISSET( &(tot->totChosenMask), TApropTEXT_COLOR ) )
	{
	appColorChooserSetColor( &(tot->totTextColorChooser),
						etaC->etaTextColorExplicit,
						&(etaC->etaTextColor) );
	}
    else{ appColorChooserUnset( &(tot->totTextColorChooser) );	}

    /*  7  */
    guiSetToggleState( tot->totTextColorToggle,
					etaC->etaTextColorExplicit );
    appShowColorChooser( &(tot->totTextColorChooser),
					etaC->etaTextColorExplicit );

    if  ( ! tot->totIsListBullet )
	{
	/*  13  */
	tedBorderToolSetPropertiesByNumber( &(tot->totBorderTool), bd,
							taC->taBorderNumber );

	/*  14  */
	tedSetShadingToolByNumber( &(tot->totShadingTool), bd,
							taC->taShadingNumber );
	}


    return;
    }

static void tedRefreshTextOrnamentsTool(
				void *				vtot,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const struct DocumentSelection * ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    TextOrnamentsTool *		tot= (TextOrnamentsTool *)vtot;
    const DocumentProperties *	dp= bd->bdProperties;

    PropertyMask		doneMask;

    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);
    ExpandedTextAttribute *	etaS= &(tot->totAttributeSet);

    tot->totIsListBullet= sd->sdIsListBullet;
    if  ( sd->sdIsListBullet )
        { tot->totCanChange= cmdEnabled[EDITcmdUPD_LIST];	}
    else{
	tot->totCanChange= cmdEnabled[EDITcmdUPD_SPAN_PROPS];
        (*pPref)++;
        }

    if  ( tot->totCurrentDocumentId != sd->sdDocumentId )
	{
	const int		avoidZero= 1;

	appColorChooserSuggestPalette( &(tot->totTextColorChooser),
					    avoidZero, dp->dpColorPalette );

	tot->totCurrentDocumentId= sd->sdDocumentId;
	}

    utilPropMaskClear( &doneMask );
    docExpandTextAttribute( &doneMask, etaS,
			&(sd->sdTextAttribute), &(sd->sdTextAttributeMask),
			dp->dpFontList, dp->dpColorPalette );
    docCopyExpandedTextAttribute( etaC, etaS );

    if  ( sd->sdIsListBullet )
        {
	TextAttribute *		taC= &(etaC->etaTextAttribute);

	taC->taBorderNumber= taC->taShadingNumber= 0;

	tedBorderToolSetPropertiesByNumber( &(tot->totBorderTool), bd,
							taC->taBorderNumber );

	/*  14  */
	tedSetShadingToolByNumber( &(tot->totShadingTool), bd,
							taC->taShadingNumber );
	}


    tot->totSetMask= sd->sdTextAttributeMask;
    tot->totChosenMask= tot->totSetMask;

    tedFormatToolRefreshTextOrnamentsPage( tot );

    guiEnableWidget( tot->totColorRow, tot->totCanChange );
    tedEnableBorderTool( &(tot->totBorderTool),
					tot->totCanChange &&
					! tot->totIsListBullet );

    tedEnableShadingTool( &(tot->totShadingTool),
					tot->totCanChange &&
					! tot->totIsListBullet );

    appEnableColorChooser( &(tot->totTextColorChooser), tot->totCanChange );

    guiEnableWidget( is->isRevertButton, tot->totCanChange );
    guiEnableWidget( is->isApplyButton, tot->totCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertTextOrnaments, w, voidtot )
    {
    TextOrnamentsTool *	tot= (TextOrnamentsTool *)voidtot;

    docCopyExpandedTextAttribute( &(tot->totAttributeChosen),
						    &(tot->totAttributeSet) );

    tot->totChosenMask= tot->totSetMask;

    tedFormatToolRefreshTextOrnamentsPage( tot );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the text color page has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeTextOrnaments, w, voidtot )
    {
    TextOrnamentsTool *		tot= (TextOrnamentsTool *)voidtot;
    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);

    int				changed;

    PropertyMask		ornamentMask;

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, tot->totApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    changed= 0;
    if  ( tedBorderToolGetNumber( &(taC->taBorderNumber), &changed,
						&(tot->totBorderTool), bd ) )
	{ LDEB(1);	}
    if  ( changed )
	{
	PROPmaskADD( &(tot->totChosenMask), TApropBORDER );
	}

    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    if  ( tedShadingToolGetShadingNumber( &(taC->taShadingNumber), &isSetMask,
						&(tot->totShadingTool), bd ) )
	{ return;	}

    if  ( ! utilPropMaskIsEmpty( &isSetMask ) )
	{
	PROPmaskADD( &(tot->totChosenMask), TApropSHADING );
	}
    }

    utilPropMaskClear( &ornamentMask );
    PROPmaskADD( &ornamentMask, TApropTEXT_COLOR );
    PROPmaskADD( &ornamentMask, TApropBORDER );
    PROPmaskADD( &ornamentMask, TApropSHADING );
    utilPropMaskAnd( &ornamentMask, &ornamentMask, &(tot->totChosenMask) );

    if  ( tot->totIsListBullet )
        { tedAppListFontToolSet( tot->totApplication, &ornamentMask, etaC ); }
    else{ tedAppFontToolSet( tot->totApplication, &ornamentMask, etaC ); }

    return;
    }

/************************************************************************/
/*									*/
/*  A text color was selected.						*/
/*									*/
/************************************************************************/

static void tedTextTextColorChosen(
				ColorChooser *			cc,
				int				which,
				void *				voidtot,
				int				choice,
				const RGB8Color *		rgb8 )
    {
    TextOrnamentsTool *		tot= (TextOrnamentsTool *)voidtot;
    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	if  ( which == TApropTEXT_COLOR )
	    {
	    appInspectorShowRgbPage( tot->totInspector,
			    tot->totSubjectPage, which, &(etaC->etaTextColor) );
	    return;
	    }

	LLDEB(CHOICEccMORE,which);
	return;
	}

    /*  2  */
    if  ( which == TApropTEXT_COLOR )
	{
	int		colorExplicit= choice != CHOICEccDEFAULT;

	PropertyMask	doneMask;

	utilPropMaskClear( &doneMask );

	appColorChooserColorChosen( &doneMask, &changed,
			&(etaC->etaTextColor), &(etaC->etaTextColorExplicit),
			rgb8, colorExplicit, TApropTEXT_COLOR );

	PROPmaskADD( &(tot->totChosenMask), TApropTEXT_COLOR );

	appColorChooserSetColor( &(tot->totTextColorChooser),
							colorExplicit, rgb8 );

	guiEnableWidget( tot->totSetButton,
			    ! utilPropMaskIsEmpty( &(tot->totChosenMask) ) );

	return;
	}

    LDEB(which); return;
    }

static APP_TOGGLE_CALLBACK_H( tedTextTextColorToggled, w, voidtot, voidtbcs )
    {
    TextOrnamentsTool *		tot= (TextOrnamentsTool *)voidtot;
    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    etaC->etaTextColorExplicit= set != 0;
    appShowColorChooser( &(tot->totTextColorChooser), set );

    if  ( set )
	{
	appColorChooserSetColor( &(tot->totTextColorChooser),
					    etaC->etaTextColorExplicit,
					    &(etaC->etaTextColor) );
	}
    }


static void tedTextSetExplicitColorChoice(
					TextOrnamentsTool *	tot,
					const RGB8Color *	rgb8 )
    {
    const int			colorExplicit= 1;
    PropertyMask		doneMask;
    int				changed= 0;
    ExpandedTextAttribute *	etaC= &(tot->totAttributeChosen);

    utilPropMaskClear( &doneMask );

    appColorChooserColorChosen( &doneMask, &changed,
		&(etaC->etaTextColor), &(etaC->etaTextColorExplicit),
		rgb8, colorExplicit, TApropTEXT_COLOR );

    if  ( ! PROPmaskISSET( &(tot->totChosenMask), TApropTEXT_COLOR )	||
	  changed							)
	{
	appColorChooserSetColor( &(tot->totTextColorChooser),
					etaC->etaTextColorExplicit,
					&(etaC->etaTextColor) );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real TApropSOMETHING values */
# define TAprop_SHADE_FORE_COLOR TAprop_COUNT+ 1
# define TAprop_SHADE_BACK_COLOR TAprop_COUNT+ 2

static void tedTextOrnamentsGotColor(	void *			voidtot,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    TextOrnamentsTool *	tot= (TextOrnamentsTool *)voidtot;

    switch( which )
	{
	case TApropTEXT_COLOR:
	    tedTextSetExplicitColorChoice( tot, rgb8 );

	    PROPmaskADD( &(tot->totChosenMask), which );
	    guiEnableWidget( tot->totSetButton,
			! utilPropMaskIsEmpty( &(tot->totChosenMask) ) );
	    break;

	case TApropBORDER:
	    tedBorderSetExplicitColorChoice( &(tot->totBorderTool), rgb8 );
	    PROPmaskADD( &(tot->totChosenMask), which );
	    guiEnableWidget( tot->totSetButton,
			! utilPropMaskIsEmpty( &(tot->totChosenMask) ) );
	    break;

	case TAprop_SHADE_FORE_COLOR:
	case TAprop_SHADE_BACK_COLOR:
	    tedShadeSetExplicitColorChoice( &(tot->totShadingTool),
								which, rgb8 );
	    PROPmaskADD( &(tot->totChosenMask), TApropSHADING );
	    guiEnableWidget( tot->totSetButton,
			! utilPropMaskIsEmpty( &(tot->totChosenMask) ) );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

static void * tedFormatBuildTextOrnamentsPage(
			int					subjectPage,
			struct AppInspector *			ai,
				struct EditApplication *	ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const TextOrnamentsPageResources *	totr= &TED_TextOrnamentsPageResources;

    const int			textHasAutomaticColor= 0;

    TextOrnamentsTool *		tot= malloc( sizeof(TextOrnamentsTool) );

    if  ( ! tot )
	{ XDEB(tot); return tot;	}

    tedInitTextOrnamentsTool( tot );

    tot->totSubjectPage= subjectPage;
    tot->totInspector= ai;
    tot->totApplication= ea;
    tot->totPageResources= totr;

    appInitColorChooser( &(tot->totTextColorChooser) );
    tedInitBorderTool( &(tot->totBorderTool) );
    tedInitShadingTool( &(tot->totShadingTool) );

    guiMakeToggleAndColorChooserRow( &(tot->totColorRow),
		    &(tot->totTextColorToggle),
		    &(tot->totTextColorChooser), textHasAutomaticColor,
		    pageWidget, totr->totrTextColor,
		    &(totr->totrTextColorChooserResources),
		    tedTextTextColorToggled, tedTextTextColorChosen,
		    TApropTEXT_COLOR, (void *)tot );

    /**/
    tedMakeBorderTool( &(tot->totBorderTool), ai, pageWidget,
		totr->totrTextBorder, &(totr->totrBorderToolResources),
		subjectPage, TApropBORDER );

    /**/
    tedFormatMakeShadingTool( &(tot->totShadingTool), ai, pageWidget,
		/*totr->totrTextShading,*/ (const char *)0,
		&(totr->totrShadingResources),
		subjectPage, TAprop_SHADE_FORE_COLOR, TAprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)tot );


    /**/

    tot->totSetButton= is->isApplyButton;

    docInitExpandedTextAttribute( &(tot->totAttributeSet) );
    docInitExpandedTextAttribute( &(tot->totAttributeChosen) );

    tot->totCurrentDocumentId= 0;

    utilPropMaskClear( &(tot->totChosenMask) );
    utilPropMaskClear( &(tot->totSetMask) );

    return (void *)tot;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

static void tedFinishTextOrnamentsPage(
			    TextOrnamentsTool *			tot,
			    const TextOrnamentsPageResources *	topr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(tot->totApplication->eaPostScriptFontList);

    appFinishColorChooser( &(tot->totTextColorChooser), psfl,
						tot->totTextColorToggle );

    tedFinishBorderTool( &(tot->totBorderTool), psfl );
    tedFinishShadingTool( &(tot->totShadingTool), psfl );

    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph borders and shading tool resources.			*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTextSubjectResourceTable[]=
    {
    APP_RESOURCE( "textColorToolTextColor",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Text Color" ),
    APP_RESOURCE( "textColorToolSet",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set" ),
    APP_RESOURCE( "textColorToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedTextToolResourceTable[]=
    {
    APP_RESOURCE( "fontToolTextColor",
		offsetof(TextOrnamentsPageResources,totrTextColor),
		"Text Color" ),
    APP_RESOURCE( "fontToolTextBorder",
		offsetof(TextOrnamentsPageResources,totrTextBorder),
		"Border" ),
    APP_RESOURCE( "fontToolTextShading",
		offsetof(TextOrnamentsPageResources,totrTextShading),
		"Shading" ),

    APP_RESOURCE( "fontToolTextColorChooserAutomatic",
	offsetof(TextOrnamentsPageResources,totrTextColorChooserResources.
							ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "fontToolTextColorChooserMoreColors",
	offsetof(TextOrnamentsPageResources,totrTextColorChooserResources.
							ccrMoreColors),
	"More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolTextBorderWidth",
	    offsetof(TextOrnamentsPageResources, totrBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolTextBorderStyle",
	    offsetof(TextOrnamentsPageResources, totrBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolTextBorderColor",
	    offsetof(TextOrnamentsPageResources, totrBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolTextBorderColorChooserAutomatic",
	    offsetof(TextOrnamentsPageResources,totrBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolTextBorderColorChooserMoreColors",
	    offsetof(TextOrnamentsPageResources,totrBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),


    /**/
    APP_RESOURCE( "formatToolTextShadingPattern",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolTextShadingLevel",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolTextShadingForeColor",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolTextShadingBackColor",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolTextShadingBackColorChooserNoBackground",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolTextShadingBackColorChooserMoreColors",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolTextShadingForeColorChooserAutomatic",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolTextShadingForeColorChooserMoreColors",
	    offsetof(TextOrnamentsPageResources,totrShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),


    };

static void tedFormatToolGetTextOrnamentsResourceTable(
			    EditApplication *			ea,
			    TextOrnamentsPageResources *	topr,
			    InspectorSubjectResources *		isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)topr,
				TED_TedTextToolResourceTable,
				sizeof(TED_TedTextToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTextSubjectResourceTable,
				sizeof(TED_TedTextSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

static void tedTextOrnamentsToolFinishPage(
				void *				vtot )
    {
    tedFinishTextOrnamentsPage( (TextOrnamentsTool *)vtot,
					    &TED_TextOrnamentsPageResources );
    }

int tedTextOrnamentsToolFillSubjectTypes(
				struct EditApplication *	ea,
				struct InspectorSubjectType *	ist,
				struct FormatToolSubjectType *	ftst )
    {
    tedFormatToolGetTextOrnamentsResourceTable( ea,
				    &TED_TextOrnamentsPageResources,
				    &TED_TextOrnamentsSubjectResources );

    ist->istResources= &TED_TextOrnamentsSubjectResources;

    ist->istBuildTool= tedFormatBuildTextOrnamentsPage;
    /*ftst->ftstToolFillChoosers*/
    ist->istFinishPage= tedTextOrnamentsToolFinishPage;
    ist->istFreeTool= tedFreeTextOrnamentsTool;

    ftst->ftstRefreshPage= tedRefreshTextOrnamentsTool;
    /*ftst->ftstToolResetCache*/

    ist->istGotColor= tedTextOrnamentsGotColor;

    ist->istRevertChanges= tedFormatRevertTextOrnaments;
    ist->istApplyChanges= tedFormatChangeTextOrnaments;

    return 0;
    }
