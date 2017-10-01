/************************************************************************/
/*									*/
/*  The 'Paragraph' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<limits.h>

#   include	<geoUnit.h>

#   include	"tedParaLayoutTool.h"
#   include	<tedAppFront.h>
#   include	"tedToolUtil.h"
#   include	"tedHeightTool.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>
#   include	<docPropVal.h>
#   include	<docBreakKind.h>
#   include	<docParaProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<geoDocumentPageSetup.h>
#   include	<docNodeTree.h>
#   include	<docSectProperties.h>
#   include	"tedIndentsTool.h"
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Formatting' page.			*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutPageResources
    {
    const char *		plprParaListLevel;

    IndentsToolResources	plprIndentsToolResources;

    const char *		pprParaSpaceAbove;
    const char *		pprParaSpaceBelow;

    const char *		pprOnNewPage;
    const char *		pprOnOnePage;

    const char *		pprWidctrl;
    const char *		pprKeepWithNext;

    const char *		pprParaLineDistFree;
    const char *		pprParaLineDistAtLeast;
    const char *		pprParaLineDistExactly;

    const char *		pprParaAlignment;
    const char *		pprParaAlignmentMenuTexts[DOCtha_COUNT];

    const char *		pprDirection;
    const char *		pprDirectionMenuTexts[2];
    } ParagraphLayoutPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutTool
    {
    struct EditApplication *			ptApplication;
    struct AppInspector *			ptInspector;
    const ParagraphLayoutPageResources *	ptPageResources;

    unsigned char				pltCanChange;

    int						pltMajorityFontSize;

    ParagraphProperties				ptPropertiesSet;
    ParagraphProperties				ptPropertiesChosen;

    APP_WIDGET					pltListLevelRow;
    APP_WIDGET					pltListLevelText;

    IndentsTool					pltIndentsTool;

    APP_WIDGET					pltPageRow;
    APP_WIDGET					ptOnNewPageToggle;
    APP_WIDGET					ptKeepOnPageToggle;

    APP_WIDGET					pltKeepRow;
    APP_WIDGET					ptWidowControlToggle;
    APP_WIDGET					ptKeepWithNextToggle;

    APP_WIDGET					pltSpaceAboveRow;
    APP_WIDGET					pltSpaceAboveToggle;
    APP_WIDGET					pltSpaceAboveText;

    APP_WIDGET					pltSpaceBelowRow;
    APP_WIDGET					pltSpaceBelowToggle;
    APP_WIDGET					pltSpaceBelowText;

    HeightChooser				ptLineDistChooser;

    APP_WIDGET					pltAlignmentRow;
    APP_WIDGET					pltAlignmentLabel;
    AppOptionmenu				pltAlignmentOptionmenu;

    APP_WIDGET					pltDirectionRow;
    APP_WIDGET					pltDirectionLabel;
    AppOptionmenu				pltDirectionOptionmenu;
    } ParagraphLayoutTool;

static ParagraphLayoutPageResources	TED_ParaLayoutToolResources;
static InspectorSubjectResources	TED_ParaLayoutSubjectResources;

static void tedInitParagraphLayoutTool(	ParagraphLayoutTool *	plt )
    {
    plt->ptApplication= (struct EditApplication *)0;
    plt->ptInspector= (struct AppInspector *)0;
    plt->ptPageResources= (ParagraphLayoutPageResources *)0;

    plt->pltCanChange= 1;
    plt->pltMajorityFontSize= 0;

    docInitParagraphProperties( &(plt->ptPropertiesSet) );
    docInitParagraphProperties( &(plt->ptPropertiesChosen) );

    plt->pltListLevelRow= (APP_WIDGET)0;
    plt->pltListLevelText= (APP_WIDGET)0;

    tedInitIndentsTool( &(plt->pltIndentsTool) );

    plt->pltPageRow= (APP_WIDGET)0;
    plt->ptOnNewPageToggle= (APP_WIDGET)0;
    plt->ptKeepOnPageToggle= (APP_WIDGET)0;

    plt->pltKeepRow= (APP_WIDGET)0;
    plt->ptWidowControlToggle= (APP_WIDGET)0;
    plt->ptKeepWithNextToggle= (APP_WIDGET)0;

    plt->pltSpaceAboveRow= (APP_WIDGET)0;
    plt->pltSpaceAboveToggle= (APP_WIDGET)0;
    plt->pltSpaceAboveText= (APP_WIDGET)0;

    plt->pltSpaceBelowRow= (APP_WIDGET)0;
    plt->pltSpaceBelowToggle= (APP_WIDGET)0;
    plt->pltSpaceBelowText= (APP_WIDGET)0;

    tedInitHeightChooser( &(plt->ptLineDistChooser) );

    plt->pltAlignmentRow= (APP_WIDGET)0;
    plt->pltAlignmentLabel= (APP_WIDGET)0;
    guiInitOptionmenu( &(plt->pltAlignmentOptionmenu) );

    plt->pltDirectionRow= (APP_WIDGET)0;
    plt->pltDirectionLabel= (APP_WIDGET)0;
    guiInitOptionmenu( &(plt->pltDirectionOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Update the controls for SpaceAbove/SpaceBelow.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshSpaceAround(	APP_WIDGET	row,
						APP_WIDGET	toggle,
						APP_WIDGET	text,
						int		space,
						int		canChange )
    {
    if  ( space == 0 )
	{
	guiStringToText( text, "" );
	guiEnableText( text, 0 );

	guiSetToggleState( toggle, 0 );
	}
    else{
	guiLengthToText( text, space, UNITtyPOINTS );
	guiEnableWidget( row, canChange );
	guiEnableText( text, canChange );

	guiSetToggleState( toggle, 1 );
	}

    guiEnableWidget( toggle, canChange );
    }

static void tedFormatToolRefreshParagraphPage(	ParagraphLayoutTool *	plt )
    {
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    guiEnableWidget( plt->pltListLevelRow, plt->pltCanChange );
    guiEnableWidget( plt->pltAlignmentRow, plt->pltCanChange );

    if  ( pp->ppListOverride > 0 )
	{ guiIntegerToText( plt->pltListLevelText, pp->ppListLevel+ 1 ); }
    else{ guiStringToText( plt->pltListLevelText, "" );			}

    tedRefresIndentsTool( &(plt->pltIndentsTool), pp->ppListOverride > 0,
			    plt->pltCanChange, pp->ppRToL,
			    pp->ppFirstIndentTwips,
			    pp->ppLeftIndentTwips,
			    pp->ppRightIndentTwips );


    tedFormatToolRefreshSpaceAround( plt->pltSpaceAboveRow,
					    plt->pltSpaceAboveToggle,
					    plt->pltSpaceAboveText,
					    pp->ppSpaceBeforeTwips,
					    plt->pltCanChange );

    tedFormatToolRefreshSpaceAround( plt->pltSpaceBelowRow,
					    plt->pltSpaceBelowToggle,
					    plt->pltSpaceBelowText,
					    pp->ppSpaceAfterTwips,
					    plt->pltCanChange );

    tedFormatEnableHeightChooser( &(plt->ptLineDistChooser),
						plt->pltCanChange );
    tedFormatRefreshHeightChooser( &(plt->ptLineDistChooser),
						pp->ppLineSpacingTwips );

    guiEnableOptionmenu( &(plt->pltAlignmentOptionmenu), plt->pltCanChange );
    guiSetOptionmenu( &(plt->pltAlignmentOptionmenu), pp->ppAlignment );

    guiEnableOptionmenu( &(plt->pltDirectionOptionmenu), plt->pltCanChange );
    guiSetOptionmenu( &(plt->pltDirectionOptionmenu), pp->ppRToL );

    guiSetToggleState( plt->ptOnNewPageToggle,
					pp->ppBreakKind == DOCibkPAGE );
    guiEnableWidget( plt->ptKeepOnPageToggle, plt->pltCanChange );
    guiSetToggleState( plt->ptKeepOnPageToggle, pp->ppKeepOnPage );

    guiEnableWidget( plt->ptWidowControlToggle, plt->pltCanChange );
    guiSetToggleState( plt->ptWidowControlToggle, pp->ppWidowControl );

    guiEnableWidget( plt->ptKeepWithNextToggle, plt->pltCanChange );
    guiSetToggleState( plt->ptKeepWithNextToggle, pp->ppKeepWithNext );

    return;
    }

static void tedRefreshParaLayoutTool(
				void *				vplt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)vplt;

    PropertyMask			ppSetMask;

    struct BufferItem *			paraNode= ds->dsHead.dpNode;
    const ParagraphProperties *		pp= paraNode->biParaProperties;

    const struct BufferItem *		bodySectNode;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

    bodySectNode= docGetBodySectNode( ds->dsHead.dpNode, bd );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); *pEnabled= 0; return;	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesChosen),
				    &ppSetMask, pp,
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesSet),
				    &ppSetMask, pp,
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    plt->pltCanChange= cmdEnabled[EDITcmdUPD_PARA_PROPS];
    plt->pltMajorityFontSize= sd->sdMajorityFontSize;

    guiEnableWidget( plt->ptOnNewPageToggle,
				    plt->pltCanChange &&
				    paraNode->biTreeType == DOCinBODY &&
				    pp->ppTableNesting == 0 );

    tedIndentsToolSetDocumentGeometry( &(plt->pltIndentsTool),
				    &(bodySectNode->biSectDocumentGeometry) );

    tedRefreshParaSubjectControls( is, ds, sg, sd, cmdEnabled );

    tedFormatToolRefreshParagraphPage( plt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

    docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesChosen),
				    &ppSetMask, &(plt->ptPropertiesSet),
				    (const struct DocumentAttributeMap *)0 );

    tedFormatToolRefreshParagraphPage( plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedParaRToLFirstIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    tedRToLFirstIndentChanged( &(plt->pltIndentsTool),
					pp->ppFirstIndentTwips,
					pp->ppRightIndentTwips );
    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLToRFirstIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    tedLToRFirstIndentChanged( &(plt->pltIndentsTool),
					pp->ppFirstIndentTwips,
					pp->ppLeftIndentTwips );
    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLeftIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    tedLeftIndentChanged( &(plt->pltIndentsTool),
					pp->ppLeftIndentTwips );
    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaRightIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    tedRightIndentChanged( &(plt->pltIndentsTool),
					pp->ppRightIndentTwips );
    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceAboveChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->pltSpaceAboveText, pp->ppSpaceBeforeTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceBelowChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->pltSpaceBelowText, pp->ppSpaceAfterTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLineDistanceChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedHeightToolValidateDimension( &value, &changed,
			&(plt->ptLineDistChooser), pp->ppLineSpacingTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *	ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *	ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *	ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLayoutDeletePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *	ea= plt->ptApplication;

    tedAppDeleteSelectedParagraphs( ea );
    }

static APP_BUTTON_CALLBACK_H( tedLayoutInsertPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *		ea= plt->ptApplication;

    tedAppInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLayoutAppendPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *		ea= plt->ptApplication;

    tedAppInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph page has been pushed.		*/
/*									*/
/************************************************************************/

static int tedFormatToolGetSpaceAround(		int *		pValue,
						int *		pChanged,
						APP_WIDGET	text,
						APP_WIDGET	toggle,
						int		value )
    {
    int			state;
    int			changed= 0;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    state= guiGetToggleState( toggle ) != 0;
    if  ( ! state )
	{ *pChanged= ( value != 0 ); *pValue= 0; return 0;	}

    state= value;
    if  ( guiLengthFromText( text,
			    &state, &changed, UNITtyPOINTS,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( changed )
	{ value= state;	}

    *pChanged= changed; *pValue=value; return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve values from controls.					*/
/*									*/
/************************************************************************/

static int tedParaLayoutGetChosen(	ParagraphLayoutTool *	plt )
    {
    ParagraphProperties *	ppChosen= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

   if  ( tedIndentsToolGetChosen( &(plt->pltIndentsTool), ppChosen->ppRToL,
					&(ppChosen->ppFirstIndentTwips),
					&(ppChosen->ppLeftIndentTwips),
					&(ppChosen->ppRightIndentTwips) ) )
	{ return -1;	}

    value= ppChosen->ppLineSpacingTwips;
    if  ( tedFormatToolGetHeight( &value, &(plt->ptLineDistChooser) ) )
	{ return -1;	}
    ppChosen->ppLineSpacingTwips= value;

    value= ppChosen->ppSpaceBeforeTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->pltSpaceAboveText, plt->pltSpaceAboveToggle, value ) )
	{ return -1;	}
    ppChosen->ppSpaceBeforeTwips= value;

    value= ppChosen->ppSpaceAfterTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->pltSpaceBelowText, plt->pltSpaceBelowToggle, value ) )
	{ return -1;	}
    ppChosen->ppSpaceAfterTwips= value;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change paragraph layout.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLayoutChangePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    struct EditApplication *	ea= plt->ptApplication;
    const ParagraphProperties *	ppSet= &(plt->ptPropertiesSet);
    const ParagraphProperties *	ppChosen= &(plt->ptPropertiesChosen);

    PropertyMask		ppDifMask;
    PropertyMask		ppCmpMask;

    if  ( tedParaLayoutGetChosen( plt ) )
	{ return;	}

    utilPropMaskClear( &ppCmpMask );
    utilPropMaskClear( &ppDifMask );
    utilPropMaskFill( &ppCmpMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppCmpMask, PPpropTABLE_NESTING );

    docParaPropertyDifference( &ppDifMask, ppSet, &ppCmpMask, ppChosen );

    tedAppChangeParagraphProperties( ea, &ppDifMask, ppChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the Space toggles was activated:				*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceAboveToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    int					set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceBeforeTwips != 0 )
	    {
	    pp->ppSpaceBeforeTwips=
			    plt->ptPropertiesSet.ppSpaceBeforeTwips;
	    }
	else{
	    pp->ppSpaceBeforeTwips= plt->pltMajorityFontSize;
	    }
	}
    else{
	pp->ppSpaceBeforeTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->pltSpaceAboveRow,
					    plt->pltSpaceAboveToggle,
					    plt->pltSpaceAboveText,
					    pp->ppSpaceBeforeTwips,
					    plt->pltCanChange );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceBelowToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);
    
    int					set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceAfterTwips != 0 )
	    {
	    pp->ppSpaceAfterTwips= plt->ptPropertiesSet.ppSpaceAfterTwips;
	    }
	else{
	    pp->ppSpaceAfterTwips= plt->pltMajorityFontSize;
	    }
	}
    else{
	pp->ppSpaceAfterTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->pltSpaceBelowRow,
						plt->pltSpaceBelowToggle,
						plt->pltSpaceBelowText,
						pp->ppSpaceAfterTwips,
						plt->pltCanChange );

    return;
    }

static void tedParaAlignmentChosen(	int		alignment,
					void *		voidplt )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    if  ( alignment < 0 || alignment >= DOCtha_COUNT )
	{ LLDEB(alignment,DOCtha_COUNT); return;	}

    pp->ppAlignment= alignment;

    return;
    }

static void tedParaDirectionChosen(	int		rtol,
					void *		voidplt )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    if  ( rtol < 0 || rtol >= 2 )
	{ LLDEB(rtol,2); return;	}

    if  ( pp->ppRToL != rtol )
	{
	pp->ppRToL= rtol;

	tedFormatToolRefreshParagraphPage( plt );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A line Spacing was chosen with the menu.				*/
/*									*/
/************************************************************************/

static void tedParaLineDistChosen(	int		how,
					void *		voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    HeightChooser *		hc= &(plt->ptLineDistChooser);

    int				defaultValue;

    defaultValue= plt->ptPropertiesChosen.ppLineSpacingTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= plt->ptPropertiesSet.ppLineSpacingTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaOnNewPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppBreakKind= DOCibkNONE;
    if  ( ( set != 0 ) && pp->ppTableNesting == 0 )
	{ pp->ppBreakKind= DOCibkPAGE;	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepOnPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepOnPage= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaWidowControlToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppWidowControl= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepWithNextToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepWithNext= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

static void * tedBuildParagraphLayoutPage(
			int					subjectPage,
			struct AppInspector *			ai,
			struct EditApplication *		ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const ParagraphLayoutPageResources *	plpr= 
				    &TED_ParaLayoutToolResources;
    const int			textColumns= 10;

    ParagraphLayoutTool *	plt= malloc( sizeof(ParagraphLayoutTool) );

    if  ( ! plt )
	{ XDEB(plt); return plt;	}

    tedInitParagraphLayoutTool( plt );

    /**/
    plt->ptPageResources= plpr;
    plt->ptInspector= ai;
    plt->ptApplication= ea;

    /**/
    guiToolMakeLabelAndTextRow( &(plt->pltListLevelRow),
		    (APP_WIDGET *)0, &(plt->pltListLevelText),
		    pageWidget, plpr->plprParaListLevel, textColumns, 0 );

    tedFillIndentsTool( &(plt->pltIndentsTool), pageWidget, textColumns,
				&(plpr->plprIndentsToolResources),
				tedParaLToRFirstIndentChanged,
				tedParaLeftIndentChanged,
				tedParaRToLFirstIndentChanged,
				tedParaRightIndentChanged, (void *)plt );

    /**/
    guiToolMakeLabelAndMenuRow( &(plt->pltAlignmentRow),
		&(plt->pltAlignmentOptionmenu), &(plt->pltAlignmentLabel),
		pageWidget, plpr->pprParaAlignment,
		tedParaAlignmentChosen, (void *)plt );

    /**/
    guiToolMakeLabelAndMenuRow( &(plt->pltDirectionRow),
		&(plt->pltDirectionOptionmenu), &(plt->pltDirectionLabel),
		pageWidget, plpr->pprDirection,
		tedParaDirectionChosen, (void *)plt );

    /**/
    tedFormatMakeHeightRow( (void *)plt, pageWidget,
						&(plt->ptLineDistChooser),
						tedParaLineDistanceChanged,
						tedParaLineDistChosen );

    /**/
    guiToolMakeToggleAndTextRow( &(plt->pltSpaceAboveRow),
		    &(plt->pltSpaceAboveToggle), &(plt->pltSpaceAboveText),
		    pageWidget, plpr->pprParaSpaceAbove,
		    tedFormatSpaceAboveToggled, (void *)plt,
		    textColumns, 1 );

    guiSetGotValueCallbackForText( plt->pltSpaceAboveText,
				    tedParaSpaceAboveChanged, (void *)plt );
    /**/
    guiToolMakeToggleAndTextRow( &(plt->pltSpaceBelowRow),
		    &(plt->pltSpaceBelowToggle), &(plt->pltSpaceBelowText),
		    pageWidget, plpr->pprParaSpaceBelow,
		    tedFormatSpaceBelowToggled, (void *)plt,
		    textColumns, 1 );

    guiSetGotValueCallbackForText( plt->pltSpaceBelowText,
				    tedParaSpaceBelowChanged, (void *)plt );

    /**/
    guiToolMake2ToggleRow( &(plt->pltPageRow), pageWidget,
		    &(plt->ptOnNewPageToggle), &(plt->ptKeepOnPageToggle),
		    plpr->pprOnNewPage, plpr->pprOnOnePage,
		    tedParaOnNewPageToggled,
		    tedParaKeepOnPageToggled, (void *)plt, (void *)plt );

    /**/
    guiToolMake2ToggleRow( &(plt->pltKeepRow), pageWidget,
		    &(plt->ptWidowControlToggle), &(plt->ptKeepWithNextToggle),
		    plpr->pprWidctrl, plpr->pprKeepWithNext,
		    tedParaWidowControlToggled,
		    tedParaKeepWithNextToggled, (void *)plt, (void *)plt );

    return (void *)plt;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

static void tedParaLayoutToolFillChoosers( void *	vplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)vplt;
    const ParagraphLayoutPageResources *	plpr= 
				    &TED_ParaLayoutToolResources;

    tedFormatFillHeightChooser( &(plt->ptLineDistChooser),
					    plpr->pprParaLineDistFree,
					    plpr->pprParaLineDistAtLeast,
					    plpr->pprParaLineDistExactly );

    appFillInspectorMenu( DOCtha_COUNT, DOCthaLEFT,
	    plpr->pprParaAlignmentMenuTexts, &(plt->pltAlignmentOptionmenu) );

    appFillInspectorMenu( 2, 0,
	    plpr->pprDirectionMenuTexts, &(plt->pltDirectionOptionmenu) );

    return;
    }

static void tedFinishParaLayoutPage(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	plpr )
    {
    guiOptionmenuRefreshWidth( &(plt->ptLineDistChooser.hcOptionmenu) );

    guiOptionmenuRefreshWidth( &(plt->pltAlignmentOptionmenu) );
    guiOptionmenuRefreshWidth( &(plt->pltDirectionOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the paragraph tool.						*/
/*									*/
/************************************************************************/

static void tedFormatFreeParaLayoutTool(	void *		vplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)vplt;

    docCleanParagraphProperties( &(plt->ptPropertiesSet) );
    docCleanParagraphProperties( &(plt->ptPropertiesChosen) );

    free( plt );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaLayout",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Formatting" ),
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
    APP_RESOURCE( "formatToolParaListLevel",
	offsetof(ParagraphLayoutPageResources,plprParaListLevel),
	"List Level" ),

    APP_RESOURCE( "formatToolParaFirstIndent",
	offsetof(ParagraphLayoutPageResources,
			    plprIndentsToolResources.itrFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "formatToolParaLeftIndent",
	offsetof(ParagraphLayoutPageResources,
			    plprIndentsToolResources.itrLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "formatToolParaRightIndent",
	offsetof(ParagraphLayoutPageResources,
			    plprIndentsToolResources.itrRightIndent),
	"Right Indent" ),

    /**/
    APP_RESOURCE( "formatToolParaAlignment",
	offsetof(ParagraphLayoutPageResources,pprParaAlignment),
	"Alignment" ),
    APP_RESOURCE( "formatToolParaAlignLeft",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignmentMenuTexts[DOCthaLEFT]),
	"Left" ),
    APP_RESOURCE( "formatToolParaAlignRight",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignmentMenuTexts[DOCthaRIGHT]),
	"Right" ),
    APP_RESOURCE( "formatToolParaAlignCentered",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignmentMenuTexts[DOCthaCENTERED]),
	"Centered" ),
    APP_RESOURCE( "formatToolParaAlignJustified",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignmentMenuTexts[DOCthaJUSTIFIED]),
	"Justified" ),

    /**/
    APP_RESOURCE( "formatToolParaDirection",
	offsetof(ParagraphLayoutPageResources,pprDirection),
	"Direction" ),
    APP_RESOURCE( "formatToolParaLeftToRight",
	offsetof(ParagraphLayoutPageResources,
				    pprDirectionMenuTexts[0]),
	"Left to Right" ),
    APP_RESOURCE( "formatToolParaRightToLeft",
	offsetof(ParagraphLayoutPageResources,
				    pprDirectionMenuTexts[1]),
	"Right to Left" ),

    /**/
    APP_RESOURCE( "formatToolLineDistFree",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistFree),
	"Spacing Free" ),
    APP_RESOURCE( "formatToolLineDistExactly",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistExactly),
	"Spacing Exactly" ),
    APP_RESOURCE( "formatToolLineDistAtLeast",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistAtLeast),
	"Spacing at Least" ),
    /**/
    APP_RESOURCE( "formatToolParaSpaceAbove",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceAbove),
	"Space Above" ),
    APP_RESOURCE( "formatToolParaSpaceBelow",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceBelow),
	"Space Below" ),

    /**/
    APP_RESOURCE( "formatToolParaOnNewPage",
	offsetof(ParagraphLayoutPageResources,pprOnNewPage),
	"Start on New Page" ),
    APP_RESOURCE( "formatToolParaOnOnePage",
	offsetof(ParagraphLayoutPageResources,pprOnOnePage),
	"Keep on One Page" ),
    /**/
    APP_RESOURCE( "formatToolParaWidctrl",
	offsetof(ParagraphLayoutPageResources,pprWidctrl),
	"Widow/Orphan Control" ),
    APP_RESOURCE( "formatToolParaKeepWithNext",
	offsetof(ParagraphLayoutPageResources,pprKeepWithNext),
	"Keep with Next" ),
    };

static void tedParaLayoutToolGetResourceTable(
					struct EditApplication *	ea,
					ParagraphLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)plpr,
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

static void tedParaLayoutToolFinishPage(
				void *				vplt )
    {
    tedFinishParaLayoutPage( (ParagraphLayoutTool *)vplt,
						&TED_ParaLayoutToolResources );
    }

int tedParaLayoutToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedParaLayoutToolGetResourceTable( ea,
				&TED_ParaLayoutToolResources,
				&TED_ParaLayoutSubjectResources );

    ist->istResources= &TED_ParaLayoutSubjectResources;

    ist->istBuildTool= tedBuildParagraphLayoutPage;
    ist->istFillChoosers= tedParaLayoutToolFillChoosers;
    ist->istFinishPage= tedParaLayoutToolFinishPage;
    ftst->ftstRefreshPage= tedRefreshParaLayoutTool;
    ist->istFreeTool= tedFormatFreeParaLayoutTool;
    /*fts->ftsToolResetCache*/

    ist->istMoveToNext= tedFormatNextPara;
    ist->istMoveToPrev= tedFormatPrevPara;

    ist->istSelectInstance= tedFormatSelectPara;
    ist->istDeleteInstance= tedLayoutDeletePara;

    ist->istInsertInstance= tedLayoutInsertPara;
    ist->istAppendInstance= tedLayoutAppendPara;

    ist->istRevertChanges= tedFormatParaRevertPushed;
    ist->istApplyChanges= tedLayoutChangePara;

    return 0;
    }
