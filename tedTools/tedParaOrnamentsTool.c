/************************************************************************/
/*									*/
/*  The 'Paragraph' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"tedParaOrnamentsTool.h"
#   include	<tedAppFront.h>
#   include	"tedToolUtil.h"
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>
#   include	"tedShadingTool.h"
#   include	"tedBorderTool.h"
#   include	<docParaProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Borders and Shading' page.		*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsPageResources
    {
    char *			poprShading;
    ShadingToolResources	poprShadingResources;
    BorderToolResources		poprBorderToolResources;

    char *			poprBorders;
    char *			poprTopBorder;
    char *			poprBottomBorder;
    char *			poprLeftBorder;
    char *			poprRightBorder;
    } ParagraphOrnamentsPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsTool
    {
    struct EditApplication *			potApplication;
    struct AppInspector *			potInspector;
    const ParagraphOrnamentsPageResources *	potPageResources;

    ParagraphProperties				potPropertiesSet;
    ParagraphProperties				potPropertiesChosen;

    unsigned char				potCanChange;

    APP_WIDGET					potBordersFrame;
    APP_WIDGET					potBordersPaned;
    BorderTool					potTopBorderTool;
    BorderTool					potBottomBorderTool;
    BorderTool					potLeftBorderTool;
    BorderTool					potRightBorderTool;

    ShadingTool					potShadingTool;
    } ParagraphOrnamentsTool;

static ParagraphOrnamentsPageResources	TED_ParaOrnamentsToolResources;
static InspectorSubjectResources	TED_ParaOrnamentsSubjectResources;

/************************************************************************/
/*									*/
/*  Init/Clean the paragraph ornaments tool.				*/
/*									*/
/************************************************************************/

static void tedInitParagraphOrnamentsTool(
				ParagraphOrnamentsTool *	pot )
    {
    pot->potApplication= (struct EditApplication *)0;
    pot->potInspector= (struct AppInspector *)0;
    pot->potPageResources= (const ParagraphOrnamentsPageResources *)0;

    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    tedInitBorderTool( &(pot->potTopBorderTool) );
    tedInitBorderTool( &(pot->potBottomBorderTool) );
    tedInitBorderTool( &(pot->potLeftBorderTool) );
    tedInitBorderTool( &(pot->potRightBorderTool) );
    
    tedInitShadingTool( &(pot->potShadingTool) );

    return;
    }

static void tedFreeParaOrnamentsTool(	void *	vpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)vpot;

    docCleanParagraphProperties( &(pot->potPropertiesSet) );
    docCleanParagraphProperties( &(pot->potPropertiesChosen) );

    tedCleanBorderTool( &(pot->potTopBorderTool) );
    tedCleanBorderTool( &(pot->potBottomBorderTool) );
    tedCleanBorderTool( &(pot->potLeftBorderTool) );
    tedCleanBorderTool( &(pot->potRightBorderTool) );
    
    tedCleanShadingTool( &(pot->potShadingTool) );

    free( pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the controls on the paragraph properties page.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshParaOrnamentsPage(
					ParagraphOrnamentsTool *	pot )
    {
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);
    struct EditDocument *		ed;
    int				traced;
    struct BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, pot->potApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    guiEnableWidget( pot->potBordersFrame, pot->potCanChange );
    tedEnableBorderTool( &(pot->potTopBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potBottomBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potLeftBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potRightBorderTool), pot->potCanChange );

    tedBorderToolSetPropertiesByNumber( &(pot->potTopBorderTool), bd,
						pp->ppTopBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potBottomBorderTool), bd,
						pp->ppBottomBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potLeftBorderTool), bd,
						pp->ppLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potRightBorderTool), bd,
						pp->ppRightBorderNumber );

    tedEnableShadingTool( &(pot->potShadingTool), pot->potCanChange );
    tedSetShadingToolByNumber( &(pot->potShadingTool),
						bd, pp->ppShadingNumber );

    return;
    }


static void tedRefreshParaOrnamentsTool(
				void *				vpot,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const struct SelectionDescription * sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    ParagraphOrnamentsTool *		pot= (ParagraphOrnamentsTool *)vpot;

    PropertyMask			ppSetMask;

    struct BufferItem *			paraNode= ds->dsHead.dpNode;
    const ParagraphProperties *		pp= paraNode->biParaProperties;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );
    PROPmaskUNSET( &ppSetMask, PPpropTAB_STOPS );

    if  ( docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesChosen),
				    &ppSetMask, pp,
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesSet),
				    &ppSetMask, pp,
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    pot->potCanChange= cmdEnabled[EDITcmdUPD_PARA_PROPS];

    tedRefreshParaSubjectControls( is, ds, sg, sd, cmdEnabled );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesChosen),
				    &ppSetMask, &(pot->potPropertiesSet),
				    (const struct DocumentAttributeMap *)0 );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeletePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppDeleteSelectedParagraphs( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    struct EditApplication *		ea= pot->potApplication;

    tedAppInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph borders and shading page has	*/
/*  been pushed.							*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);

    PropertyMask		ppSetMask;
    int				changed;

    /**/
    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, pot->potApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    /**/
    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropTOP_BORDER );
    PROPmaskADD( &ppSetMask, PPpropBOTTOM_BORDER );
    PROPmaskADD( &ppSetMask, PPpropLEFT_BORDER );
    PROPmaskADD( &ppSetMask, PPpropRIGHT_BORDER );

    PROPmaskADD( &ppSetMask, PPpropSHADING );

    /**/
    if  ( tedBorderToolGetNumber( &(pp->ppTopBorderNumber), &changed,
					    &(pot->potTopBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppBottomBorderNumber), &changed,
					    &(pot->potBottomBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppLeftBorderNumber), &changed,
					    &(pot->potLeftBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppRightBorderNumber), &changed,
					    &(pot->potRightBorderTool), bd ) )
	{ LDEB(1); return;	}

    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    if  ( tedShadingToolGetShadingNumber( &(pp->ppShadingNumber), &isSetMask,
						&(pot->potShadingTool), bd ) )
	{ return;	}

    if  ( ! utilPropMaskIsEmpty( &isSetMask ) )
	{ PROPmaskADD( &ppSetMask, PPpropSHADING );	}
    }

    /**/
    tedAppChangeParagraphProperties( pot->potApplication, &ppSetMask, pp );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real PPpropSOMETHING values */
# define PPprop_SHADE_FORE_COLOR PPprop_FULL_COUNT+ 1
# define PPprop_SHADE_BACK_COLOR PPprop_FULL_COUNT+ 2

static void tedParaOrnamentsGotColor(	void *			voidpot,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    switch( which )
	{
	case PPpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potTopBorderTool), rgb8 );
	    break;
	case PPpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potBottomBorderTool), rgb8 );
	    break;
	case PPpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potLeftBorderTool), rgb8 );
	    break;
	case PPpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potRightBorderTool), rgb8 );
	    break;

	case PPprop_BOX_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBETWEEN_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBAR_BORDER:
	    LDEB(which); return;
	    break;

	case PPprop_SHADE_BACK_COLOR:
	case PPprop_SHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(pot->potShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedParaOrnamentsToolMakeBorderFrame(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			struct AppInspector *				ai,
			int					subjectPage,
			APP_WIDGET				pageWidget )
    {
    guiMakeColumnFrameInColumn( &(pot->potBordersFrame),
		    &(pot->potBordersPaned), pageWidget, popr->poprBorders );

    tedMakeBorderTool( &(pot->potTopBorderTool), ai, pot->potBordersPaned,
	    popr->poprTopBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropTOP_BORDER );
    tedMakeBorderTool( &(pot->potBottomBorderTool), ai, pot->potBordersPaned,
	    popr->poprBottomBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropBOTTOM_BORDER );
    tedMakeBorderTool( &(pot->potLeftBorderTool), ai, pot->potBordersPaned,
	    popr->poprLeftBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropLEFT_BORDER );
    tedMakeBorderTool( &(pot->potRightBorderTool), ai, pot->potBordersPaned,
	    popr->poprRightBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropRIGHT_BORDER );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

static void * tedFormatBuildParagraphOrnamentsPage(
			int					subjectPage,
			struct AppInspector *			ai,
			struct EditApplication *		ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const ParagraphOrnamentsPageResources *	popr= &TED_ParaOrnamentsToolResources;

    ParagraphOrnamentsTool *	pot= malloc( sizeof(ParagraphOrnamentsTool) );

    if  ( ! pot )
	{ XDEB(pot); return pot;	}

    tedInitParagraphOrnamentsTool( pot );

    /**/
    pot->potPageResources= popr;

    pot->potInspector= ai;
    pot->potApplication= ea;

    /**/
    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    /**/
    tedParaOrnamentsToolMakeBorderFrame( pot, popr,
					    ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(pot->potShadingTool), ai, pageWidget,
		/* popr->poprShading, */ (const char *)0,
		&(popr->poprShadingResources),
		subjectPage, PPprop_SHADE_FORE_COLOR, PPprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)pot );


    return (void *)pot;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

static void tedFormatFinishParaOrnamentsPage(
			    ParagraphOrnamentsTool *			pot,
			    const ParagraphOrnamentsPageResources *	popr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(pot->potApplication->eaPostScriptFontList);

    tedFinishBorderTool( &(pot->potTopBorderTool), psfl );
    tedFinishBorderTool( &(pot->potBottomBorderTool), psfl );
    tedFinishBorderTool( &(pot->potLeftBorderTool), psfl );
    tedFinishBorderTool( &(pot->potRightBorderTool), psfl );

    tedFinishShadingTool( &(pot->potShadingTool), psfl );

    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph borders and shading tool resources.			*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaOrnaments",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Borders and Shading" ),
    APP_RESOURCE( "formatToolChangePara",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Apply to Paragraph" ),
    APP_RESOURCE( "tableToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolNextPara",
	offsetof(InspectorSubjectResources,isrNextButtonText),
	"Next Paragraph" ),
    APP_RESOURCE( "formatToolPreviousPara",
	offsetof(InspectorSubjectResources,isrPrevButtonText),
	"Previous Paragraph" ),
    APP_RESOURCE( "formatToolSelectPara",
	offsetof(InspectorSubjectResources,isrSelectButtonText),
	"Select Paragraph" ),
    APP_RESOURCE( "formatToolDeletePara",
	offsetof(InspectorSubjectResources,isrDeleteButtonText),
	"Delete Paragraph" ),
    APP_RESOURCE( "formatToolInsertPara",
	offsetof(InspectorSubjectResources,isrInsertButtonText),
	"Insert Paragraph" ),
    APP_RESOURCE( "formatToolAppendPara",
	offsetof(InspectorSubjectResources,isrAppendButtonText),
	"Append Paragraph" ),
    };

static AppConfigurableResource TED_TedParagraphToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolParaBorders",
	offsetof(ParagraphOrnamentsPageResources,poprBorders),
	"Borders" ),
    APP_RESOURCE( "formatToolParaTopBorder",
	offsetof(ParagraphOrnamentsPageResources,poprTopBorder),
	"Top" ),
    APP_RESOURCE( "formatToolParaBottomBorder",
	offsetof(ParagraphOrnamentsPageResources,poprBottomBorder),
	"Bottom" ),
    APP_RESOURCE( "formatToolParaLeftBorder",
	offsetof(ParagraphOrnamentsPageResources,poprLeftBorder),
	"Left" ),
    APP_RESOURCE( "formatToolParaRightBorder",
	offsetof(ParagraphOrnamentsPageResources,poprRightBorder),
	"Right" ),

    /**/
    APP_RESOURCE( "formatToolParaBorderWidth",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrWidth),
	"Width" ),
    APP_RESOURCE( "formatToolParaBorderStyle",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrStyle),
	"Style" ),
    APP_RESOURCE( "formatToolParaBorderColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrColor),
	"Color" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolParaShading",
	offsetof(ParagraphOrnamentsPageResources,poprShading),
	"Shading" ),

    APP_RESOURCE( "formatToolParaShadingPattern",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strPattern),
	"Pattern" ),
    APP_RESOURCE( "formatToolParaShadingLevel",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strLevel),
	"Level" ),
    APP_RESOURCE( "formatToolParaShadingForeColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strForeColor),
	"Foreground" ),
    APP_RESOURCE( "formatToolParaShadingBackColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strBackColor),
	"Background" ),
    /**/
    APP_RESOURCE( "formatToolParaShadingBackColorChooserNoBackground",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	"No Background" ),
    APP_RESOURCE( "formatToolParaShadingBackColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    APP_RESOURCE( "formatToolParaShadingForeColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaShadingForeColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    };

static void tedFormatToolGetParaOrnamentsResourceTable(
			    struct EditApplication *			ea,
			    ParagraphOrnamentsPageResources *	popr,
			    InspectorSubjectResources *		isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)popr,
				TED_TedParagraphToolResourceTable,
				sizeof(TED_TedParagraphToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedParagraphSubjectResourceTable,
				sizeof(TED_TedParagraphSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

static void tedParaOrnamentsToolFinishPage(
				void *				vpot )
    {
    tedFormatFinishParaOrnamentsPage( (ParagraphOrnamentsTool *)vpot,
					&TED_ParaOrnamentsToolResources );
    }

int tedParaOrnamentsToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetParaOrnamentsResourceTable( ea,
				    &TED_ParaOrnamentsToolResources,
				    &TED_ParaOrnamentsSubjectResources );

    ist->istResources= &TED_ParaOrnamentsSubjectResources;

    ist->istBuildTool= tedFormatBuildParagraphOrnamentsPage;
    /* fts->ftsToolFillChoosers */
    ist->istFinishPage= tedParaOrnamentsToolFinishPage;
    ftst->ftstRefreshPage= tedRefreshParaOrnamentsTool;
    ist->istFreeTool= tedFreeParaOrnamentsTool;
    /*fts->ftsToolResetCache*/

    ist->istGotColor= tedParaOrnamentsGotColor;

    ist->istMoveToNext= tedFormatNextPara;
    ist->istMoveToPrev= tedFormatPrevPara;

    ist->istSelectInstance= tedFormatSelectPara;
    ist->istDeleteInstance= tedFormatDeletePara;

    ist->istInsertInstance= tedFormatInsertPara;
    ist->istAppendInstance= tedFormatAppendPara;

    ist->istRevertChanges= tedFormatParaRevertPushed;
    ist->istApplyChanges= tedFormatChangePara;

    return 0;
    }
