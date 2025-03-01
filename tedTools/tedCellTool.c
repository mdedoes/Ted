/************************************************************************/
/*									*/
/*  The 'Cell' page of the 'Ted' format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<tedAppFront.h>
#   include	"tedCellTool.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	<docPropVal.h>
#   include	<docCellProperties.h>
#   include	<docSelect.h>
#   include	<docTableRectangle.h>
#   include	<appInspectorSubject.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	<docSelectionDescription.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Cell' page.					*/
/*									*/
/************************************************************************/

typedef struct CellPageResources
    {
    char *			cprRowColumn;

    char *			cprMerged;

    const char *		cprValignItemTexts[DOCtva_COUNT];

    char *			cprWidth;
    char *			cprBorders;

    char *			cprLeftBorder;
    char *			cprRightBorder;
    char *			cprTopBorder;
    char *			cprBottomBorder;

    char *			cprShadingPattern;
    char *			cprValign;

    char *			cprShading;

    char *			cprNextRow;
    char *			cprPrevRow;

    ShadingToolResources	cprShadingResources;
    BorderToolResources		cprBorderToolResources;
    } CellPageResources;

/************************************************************************/
/*									*/
/*  A Cell tool, i.e. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct CellTool
    {
    struct EditApplication *	ctApplication;
    struct AppInspector *	ctInspector;
    const CellPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;
    unsigned char		ctCanMerge;
				/**
				 *  The value of the horizontal merge 
				 *  property that is applied for the 
				 *  upper left cell in the table rectange 
				 *  of selected cells. (NONE or HEAD)
				 */
    unsigned char		ctHorMerge;
				/**
				 *  The value of the vertical merge 
				 *  property that is applied for the 
				 *  upper left cell in the table rectange 
				 *  of selected cells. (NONE or HEAD)
				 */
    unsigned char		ctVerMerge;
    unsigned char		ctCanChange;

    APP_WIDGET			ctRowColumnRow;
    APP_WIDGET			ctRowColumnLabel;
    APP_WIDGET			ctRowColumnText;

    APP_WIDGET			ctValignRow;
    AppOptionmenu		ctValignMenu;

    APP_WIDGET			ctMergedRow;
    APP_WIDGET			ctMergedLabel; /* to fill the place */
    APP_WIDGET			ctMergedToggle;

    CellProperties		ctPropertiesSet;
    CellProperties		ctPropertiesChosen;

    APP_WIDGET			ctBordersFrame;
    APP_WIDGET			ctBordersPaned;
    BorderTool			ctTopBorderTool;
    BorderTool			ctBottomBorderTool;
    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;

    ShadingTool			ctShadingTool;

    APP_WIDGET			ctPrevRowButton;
    APP_WIDGET			ctNextRowButton;
    } CellTool;

static CellPageResources		TED_CellPageResources;
static InspectorSubjectResources	TED_CellSubjectResources;

/************************************************************************/
/*									*/
/*  Initialize/Clean a cell Tool.					*/
/*									*/
/************************************************************************/

static void tedInitCellTool(	CellTool *	ct )
    {
    ct->ctApplication= (struct EditApplication *)0;
    ct->ctInspector= (struct AppInspector *)0;
    ct->ctPageResources= (const CellPageResources *)0;

    docInitTableRectangle( &(ct->ctTableRectangle) );
    ct->ctCanMerge= 0;
    ct->ctHorMerge= CELLmergeNONE;
    ct->ctVerMerge= CELLmergeNONE;
    ct->ctCanChange= 0;

    ct->ctRowColumnRow= (APP_WIDGET)0;
    ct->ctRowColumnLabel= (APP_WIDGET)0;
    ct->ctRowColumnText= (APP_WIDGET)0;

    ct->ctMergedRow= (APP_WIDGET)0;
    ct->ctMergedToggle= (APP_WIDGET)0;
    ct->ctMergedLabel= (APP_WIDGET)0;

    guiInitOptionmenu( &(ct->ctValignMenu) );

    docInitCellProperties( &(ct->ctPropertiesSet) );
    docInitCellProperties( &(ct->ctPropertiesChosen) );

    tedInitBorderTool( &(ct->ctTopBorderTool) );
    tedInitBorderTool( &(ct->ctBottomBorderTool) );
    tedInitBorderTool( &(ct->ctLeftBorderTool) );
    tedInitBorderTool( &(ct->ctRightBorderTool) );

    tedInitShadingTool( &(ct->ctShadingTool) );

    return;
    }

static void tedFreeCellTool(	void *	vct )
    {
    CellTool *			ct= (CellTool *)vct;

    /*docCleanCellProperties( &(ct->ctPropertiesSet) );*/
    /*docCleanCellProperties( &(ct->ctPropertiesChosen) );*/

    tedCleanBorderTool( &(ct->ctTopBorderTool) );
    tedCleanBorderTool( &(ct->ctBottomBorderTool) );
    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    tedCleanShadingTool( &(ct->ctShadingTool) );

    free( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Cell' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshCellPage(	CellTool *	ct )
    {
    const TableRectangle *	tr= &(ct->ctTableRectangle);
    const CellProperties *	cp= &(ct->ctPropertiesChosen);

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    int				row0= tr->trRow0;
    int				row1= tr->trRow1;
    int				col0= tr->trCol0;
    int				col1= tr->trCol1;

    if  ( ! bd )
	{ XDEB(bd); return;	}

    guiSetOptionmenu( &(ct->ctValignMenu), cp->cpValign );

    if  ( tr->trIsSingleCell )
	{
	guiSetToggleState( ct->ctMergedToggle,
				    tr->trCellColspan > 1 ||
				    tr->trCellRowspan > 1 );

	col1= col0+ tr->trCellColspan- 1;
	row1= row0+ tr->trCellRowspan- 1;
	}
    else{
	guiSetToggleState( ct->ctMergedToggle, 0 );
	}

    guiEnableWidget( ct->ctMergedToggle,
				ct->ctCanChange && ct->ctCanMerge );

    guiRectangleToText( ct->ctRowColumnText,
				    row0- tr->trRow00+ 1,
				    row1- tr->trRow00+ 1,
				    col0+ 1, col1+ 1 );

    guiEnableWidget( ct->ctBordersFrame, ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctTopBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctBottomBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctLeftBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctRightBorderTool), ct->ctCanChange );

    tedBorderToolSetPropertiesByNumber( &(ct->ctTopBorderTool), bd,
						cp->cpTopBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctBottomBorderTool), bd,
						cp->cpBottomBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctLeftBorderTool), bd,
						cp->cpLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctRightBorderTool), bd,
						cp->cpRightBorderNumber );

    tedEnableShadingTool( &(ct->ctShadingTool), ct->ctCanChange );
    tedSetShadingToolByNumber( &(ct->ctShadingTool), bd, cp->cpShadingNumber );
    }

static void tedRefreshCellTool(
			void *					vct,
			int *					pEnabled,
			int *					pPref,
			InspectorSubject *			is,
			const DocumentSelection *		ds,
			const struct SelectionGeometry *	sg,
			const struct SelectionDescription *	sd,
			const struct BufferDocument *		bd,
			const unsigned char *			cmdEnabled )
    {
    CellTool *			ct= (CellTool *)vct;
    const TableRectangle *	tr;

    int				row0;
    int				row1;
    int				col0;
    int				col1;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( ! sd->sdInOneTable )
	{
	docInitTableRectangle( &(ct->ctTableRectangle) );
	*pEnabled= 0;
	return;
	}
    ct->ctTableRectangle= sd->sdTableRectangle;

    tr= &(ct->ctTableRectangle);
    row0= tr->trRow0;
    row1= tr->trRow1;
    col0= tr->trCol0;
    col1= tr->trCol1;

    if  ( tr->trIsSingleCell )
	{
	col1= col0+ tr->trCellColspan- 1;
	row1= row0+ tr->trCellRowspan- 1;
	}

    {
    ct->ctCanMerge= ( col1 > col0 || row1 > row0 ) &&
			    (
			    tr->trRow1 < tr->trPastHeaderRow ||
			    tr->trRow0 >= tr->trPastHeaderRow
			    );
    }

    ct->ctHorMerge= CELLmergeNONE;
    ct->ctVerMerge= CELLmergeNONE;

    if  ( ct->ctCanMerge )
	{
	if  ( col1 > col0 )
	    { ct->ctHorMerge= CELLmergeHEAD;	}
	if  ( row1 > row0 )
	    { ct->ctVerMerge= CELLmergeHEAD;	}
	}

    if  ( docCopyCellProperties( &(ct->ctPropertiesSet),
					&(sd->sdCellProperties), dam0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyCellProperties( &(ct->ctPropertiesChosen),
					&(ct->ctPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    guiEnableWidget( is->isPrevButton, tr->trCol0 > 0 );
    guiEnableWidget( is->isNextButton, tr->trCol1 < tr->trCol11 );

    guiEnableWidget( ct->ctPrevRowButton, tr->trRow0 > tr->trRow00 );
    guiEnableWidget( ct->ctNextRowButton, tr->trRow1 < tr->trRow11 );

    ct->ctCanChange= cmdEnabled[EDITcmdUPD_CELL_PROPS];

    tedFormatToolRefreshCellPage( ct );

    guiEnableWidget( ct->ctRowColumnRow, ct->ctCanChange );

    guiEnableWidget( is->isRevertButton, ct->ctCanChange );
    guiEnableWidget( is->isApplyButton, ct->ctCanChange );
    guiEnableWidget( ct->ctValignRow, ct->ctCanChange );

    *pEnabled= 1;
    return;
    }


/************************************************************************/
/*									*/
/*  Retrieve the shading for a table rectangle from a shading tool.	*/
/*									*/
/************************************************************************/

static int tedFormatToolGetCellShading(	CellProperties *	cp,
					struct BufferDocument *	bd,
					ShadingTool *		st )
    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    /*  1  */
    if  ( tedShadingToolGetShadingNumber( &(cp->cpShadingNumber), &isSetMask,
								    st, bd  ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int tedCellToolGetChosen(	CellTool *		ct,
					struct BufferDocument *	bd )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    if  ( tedBorderToolGetNumber( &(cpChosen->cpTopBorderNumber), (int *)0,
					    &(ct->ctTopBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpBottomBorderNumber), (int *)0,
					    &(ct->ctBottomBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpLeftBorderNumber), (int *)0,
					    &(ct->ctLeftBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpRightBorderNumber), (int *)0,
					    &(ct->ctRightBorderTool), bd ) )
	{ return -1;	}

    if  ( tedFormatToolGetCellShading( cpChosen, bd, &(ct->ctShadingTool) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeCellPushed, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    PropertyMask		cpDifMask;
    PropertyMask		cpCmpMask;

    const int			wholeRow= 0;
    const int			wholeColumn= 0;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    /**/
    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    if  ( tedCellToolGetChosen( ct, bd ) )
	{ goto ready;	}

    utilPropMaskClear( &cpCmpMask );
    utilPropMaskFill( &cpCmpMask, CLprop_COUNT );
    utilPropMaskClear( &cpDifMask );

    docCellPropertyDifference( &cpDifMask, &(ct->ctPropertiesSet),
						&cpCmpMask, cpChosen, dam0 );
    tedAppSetTableProperties( ct->ctApplication, wholeRow, wholeColumn,
		    &cpDifMask, cpChosen,
		    (const PropertyMask *)0, (const struct RowProperties *)0 );

  ready:

    return;
    }

static void tedCellValignChosen(	int		valign,
					void *		voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cp= &(ct->ctPropertiesChosen);

    if  ( valign < 0 || valign >= DOCtva_COUNT )
	{ LLDEB(valign,DOCtva_COUNT); return;	}

    cp->cpValign= valign;

    return;
    }


static APP_BUTTON_CALLBACK_H( tedFormatRevertCellPushed, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    docCopyCellProperties( &(ct->ctPropertiesChosen),
						&(ct->ctPropertiesSet), dam0 );

    tedFormatToolRefreshCellPage( ct );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedCellToggleMerged, w, voidct, voidtbcs )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cp= &(ct->ctPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	cp->cpHorizontalMerge= ct->ctHorMerge;
	cp->cpVerticalMerge= ct->ctVerMerge;
	}
    else{
	cp->cpHorizontalMerge= CELLmergeNONE;
	cp->cpVerticalMerge= CELLmergeNONE;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedCellPreviousColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }

# if 0
static APP_BUTTON_CALLBACK_H( tedTableSelectCell, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;

    const int		direction= 0;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }
# endif

static APP_BUTTON_CALLBACK_H( tedCellNextColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }

static APP_BUTTON_CALLBACK_H( tedCellPreviousRow, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allColumns= 0;

    tedAppSelectRow( ea, direction, allColumns );
    }

static APP_BUTTON_CALLBACK_H( tedCellNextRow, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allColumns= 0;

    tedAppSelectRow( ea, direction, allColumns );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real CLpropSOMETHING values */
# define CLprop_SHADE_BACK_COLOR CLprop_COUNT+ 1
# define CLprop_SHADE_FORE_COLOR CLprop_COUNT+ 2

static void tedCellToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    CellTool *			ct= (CellTool *)voidct;

    switch( which )
	{
	case CLpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctTopBorderTool), rgb8 );
	    break;

	case CLpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctBottomBorderTool), rgb8 );
	    break;

	case CLpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctLeftBorderTool), rgb8 );
	    break;

	case CLpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctRightBorderTool), rgb8 );
	    break;

	case CLprop_SHADE_BACK_COLOR:
	case CLprop_SHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(ct->ctShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedCellToolMakeBorderFrame(
				CellTool *			ct,
				const CellPageResources *	cpr,
				struct AppInspector *			ai,
				int				subjectPage,
				APP_WIDGET			pageWidget )
    {
    guiMakeColumnFrameInColumn( &(ct->ctBordersFrame), &(ct->ctBordersPaned),
						pageWidget, cpr->cprBorders );

    tedMakeBorderTool( &(ct->ctTopBorderTool), ai, ct->ctBordersPaned,
		cpr->cprTopBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropTOP_BORDER );

    tedMakeBorderTool( &(ct->ctBottomBorderTool), ai, ct->ctBordersPaned,
		cpr->cprBottomBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropBOTTOM_BORDER );

    tedMakeBorderTool( &(ct->ctLeftBorderTool), ai, ct->ctBordersPaned,
		cpr->cprLeftBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropLEFT_BORDER );

    tedMakeBorderTool( &(ct->ctRightBorderTool), ai, ct->ctBordersPaned,
		cpr->cprRightBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropRIGHT_BORDER );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a Cell tool, I.E. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

static void * tedFormatBuildCellPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const CellPageResources *	cpr= &TED_CellPageResources;

    APP_WIDGET	label;

    APP_WIDGET	row= (APP_WIDGET)0;

    const int	textColumns= 10;

    CellTool *			ct= malloc( sizeof(CellTool) );

    if  ( ! ct )
	{ XDEB(ct); return ct;	}

    tedInitCellTool( ct );

    /**/
    ct->ctPageResources= cpr;
    ct->ctInspector= ai;
    ct->ctApplication= ea;
    ct->ctCanChange= 1;

    /**/

    docInitTableRectangle( &(ct->ctTableRectangle) );

    docInitCellProperties( &(ct->ctPropertiesSet) );
    docInitCellProperties( &(ct->ctPropertiesChosen) );

    /**/

    guiToolMakeLabelAndTextRow( &(ct->ctRowColumnRow),
			    &(ct->ctRowColumnLabel), &(ct->ctRowColumnText),
			    pageWidget, cpr->cprRowColumn, textColumns, 0 );

    guiToolMakeToggleAndLabelRow( &(ct->ctMergedRow), pageWidget,
			    &(ct->ctMergedToggle), &(ct->ctMergedLabel),
			    cpr->cprMerged, "",
			    tedCellToggleMerged, (void *)ct );
    /**/
    guiToolMakeLabelAndMenuRow( &(ct->ctValignRow), &(ct->ctValignMenu),
				&label, pageWidget, cpr->cprValign,
				tedCellValignChosen, (void *)ct );

    /**/
    tedCellToolMakeBorderFrame( ct, cpr, ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(ct->ctShadingTool), ai, pageWidget,
		/* cpr->cprShading, */ (const char *)0,
		&(cpr->cprShadingResources),
		subjectPage, CLprop_SHADE_FORE_COLOR, CLprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)ct );

    /**/
    guiToolMake2BottonRow( &row, pageWidget,
		    &(ct->ctPrevRowButton), &(ct->ctNextRowButton),
		    cpr->cprPrevRow, cpr->cprNextRow,
		    tedCellPreviousRow, tedCellNextRow, ct );

    /* ?
    guiToolMake2BottonRow( &row, pageWidget,
		    &(is->isSelectButton), &(is->isDeleteButton),
		    isr->isrSelectButtonText, isr->isrDeleteButtonText,
		    tedTableSelectColumn, tedTableDeleteColumn, ct );
    */

    return (void *)ct;
    }

static void tedCellToolFillChoosers(	void *		vct )
    {
    CellTool *			ct= (CellTool *)vct;
    const CellPageResources *	cpr= &TED_CellPageResources;

    appFillInspectorMenu( DOCtva_COUNT, DOCtvaTOP,
					    cpr->cprValignItemTexts,
					    &(ct->ctValignMenu) );
    return;
    }

static void tedFormatFinishCellPage(	void *	vct )
    {
    CellTool *			ct= (CellTool *)vct;
    const PostScriptFontList *	psfl;

    psfl= &(ct->ctApplication->eaPostScriptFontList);

    guiOptionmenuRefreshWidth( &(ct->ctValignMenu) );

    tedFinishBorderTool( &(ct->ctTopBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctBottomBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctLeftBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctRightBorderTool), psfl );

    tedFinishShadingTool( &(ct->ctShadingTool), psfl );
    }

/************************************************************************/
/*									*/
/*  Get row tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedCellSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolCell",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Cell" ),
    APP_RESOURCE( "tableToolChangeCell",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Cell" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolCellNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolCellPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectCell",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Cell" ),
    };

static AppConfigurableResource TED_TedCellToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolCellRowColumn",
	    offsetof(CellPageResources,cprRowColumn),
	    "Row, Column" ),

    /**/
    APP_RESOURCE( "tableToolCellBorders",
	    offsetof(CellPageResources,cprBorders),
	    "Borders" ),
    APP_RESOURCE( "tableToolCellTopBorder",
	    offsetof(CellPageResources,cprTopBorder),
	    "Top" ),
    APP_RESOURCE( "tableToolCellBottomBorder",
	    offsetof(CellPageResources,cprBottomBorder),
	    "Bottom" ),
    APP_RESOURCE( "tableToolCellLeftBorder",
	    offsetof(CellPageResources,cprLeftBorder),
	    "Left" ),
    APP_RESOURCE( "tableToolCellRightBorder",
	    offsetof(CellPageResources,cprRightBorder),
	    "Right" ),

    APP_RESOURCE( "tableToolCellMerged",
	    offsetof(CellPageResources,cprMerged),
	    "Merged" ),

    /**/
    APP_RESOURCE( "formatToolCellBorderWidth",
	    offsetof(CellPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolCellBorderStyle",
	    offsetof(CellPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolCellBorderColor",
	    offsetof(CellPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserAutomatic",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserMoreColors",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolCellShading",
	    offsetof(CellPageResources,cprShading),
	    "Shading" ),

    APP_RESOURCE( "formatToolCellShadingPattern",
	    offsetof(CellPageResources,cprShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolCellShadingLevel",
	    offsetof(CellPageResources,cprShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolCellShadingForeColor",
	    offsetof(CellPageResources,cprShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolCellShadingBackColor",
	    offsetof(CellPageResources,cprShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolCellShadingBackColorChooserNoBackground",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolCellShadingBackColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolCellShadingForeColorChooserAutomatic",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellShadingForeColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "tableToolCellVerticalAlignment",
	    offsetof(CellPageResources,cprValign),
	    "Vertical Alignment" ),
    APP_RESOURCE( "tableToolCellValignTop",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaTOP]),
		"Top" ),
    APP_RESOURCE( "tableToolCellValignCenter",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaCENTERED]),
		"Center" ),
    APP_RESOURCE( "tableToolCellValignBottom",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaBOTTOM]),
		"Bottom" ),

    APP_RESOURCE( "cellToolNextRow",
		offsetof(CellPageResources,cprNextRow),
		"Next Row" ),
    APP_RESOURCE( "cellToolPreviousRow",
		offsetof(CellPageResources,cprPrevRow),
		"Previous Row" ),
    };

static void tedFormatToolGetCellResourceTable(
					struct EditApplication *		ea,
					CellPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedCellToolResourceTable,
				sizeof(TED_TedCellToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedCellSubjectResourceTable,
				sizeof(TED_TedCellSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedCellToolFillSubjectTypes( struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetCellResourceTable( ea,
				    &TED_CellPageResources,
				    &TED_CellSubjectResources );

    ist->istResources= &TED_CellSubjectResources;

    ist->istBuildTool= tedFormatBuildCellPage;
    ist->istFillChoosers= tedCellToolFillChoosers;
    ist->istFinishPage= tedFormatFinishCellPage;
    ftst->ftstRefreshPage= tedRefreshCellTool;
    ist->istFreeTool= tedFreeCellTool;
    /*fts->ftsToolResetCache*/

    ist->istGotColor= tedCellToolGotColor;

    ist->istMoveToNext= tedCellNextColumn;
    ist->istMoveToPrev= tedCellPreviousColumn;

    ist->istRevertChanges= tedFormatRevertCellPushed;
    ist->istApplyChanges= tedTableChangeCellPushed;

    return 0;
    }
