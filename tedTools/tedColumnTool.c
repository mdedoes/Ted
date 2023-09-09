/************************************************************************/
/*									*/
/*  The Column page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<geoUnit.h>

#   include	<tedAppFront.h>
#   include	"tedColumnTool.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	<docCellProperties.h>
#   include	<docRowProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docTableRectangle.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

struct EditApplication;

#   define CLprop_SHADE_FORE_COLOR	CLprop_COUNT
#   define CLprop_SHADE_BACK_COLOR	CLprop_COUNT+ 1

/************************************************************************/
/*									*/
/*  Resources for the 'Column' page.					*/
/*									*/
/************************************************************************/

typedef struct ColumnPageResources
    {
    char *			cprColumn;

    char *			cprWidth;
    char *			cprBorders;
    char *			cprLeftBorder;
    char *			cprRightBorder;

    char *			cprIsRowHeader;

    ShadingToolResources	cprShadingResources;
    BorderToolResources		cprBorderToolResources;
    } ColumnPageResources;

/************************************************************************/
/*									*/
/*  A Column tool, i.e. the 'Column' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ColumnTool
    {
    struct EditApplication *	ctApplication;
    struct AppInspector *	ctInspector;
    const ColumnPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;

				/**
				 *  The width of the frame that determines 
				 *  the formatting of the row that holds 
				 *  the cell. It controls the maximum width 
				 *  of the cell.
				 */
    int				ctParentFrameWide;

    CellProperties		ctPropertiesSet;
    CellProperties		ctPropertiesChosen;
    int				ctHalfGapWidthTwips;
    unsigned char		ctCanChange;

    APP_WIDGET			ctColumnRow;
    APP_WIDGET			ctColumnLabel;
    APP_WIDGET			ctColumnText;

    APP_WIDGET			ctWidthRow;
    APP_WIDGET			ctWidthLabel;
    APP_WIDGET			ctWidthText;

    APP_WIDGET			ctIsRowHeaderRow;
    APP_WIDGET			ctIsRowHeaderToggle;
    APP_WIDGET			ctIsRowHeaderLabel;

    APP_WIDGET			ctBordersFrame;
    APP_WIDGET			ctBordersPaned;

    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;

    ShadingTool			ctShadingTool;
    } ColumnTool;

static ColumnPageResources		TED_ColumnPageResources;
static InspectorSubjectResources	TED_ColumnSubjectResources;

/************************************************************************/
/*									*/
/*  Init/Clean a column tool						*/
/*									*/
/************************************************************************/

static void tedInitColumnTool(	ColumnTool *	ct )
    {
    memset( ct, 0, sizeof(ColumnTool) );
    }

static void tedFreeColumnTool(	void *	vct )
    {
    ColumnTool *		ct= (ColumnTool *)vct;

    /*docCleanCellProperties( &(ct->ctPropertiesSet) );*/
    /*docCleanCellProperties( &(ct->ctPropertiesChosen) );*/

    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    tedCleanShadingTool( &(ct->ctShadingTool) );

    free( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Column' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshColumnPage(	ColumnTool *	ct )
    {
    CellProperties *		cp= &(ct->ctPropertiesChosen);
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    guiSubIntervalToText( ct->ctColumnText,
				    tr->trCol0+ 1,
				    tr->trCol1+ 1,
				    1,
				    tr->trCol11+ 1 );

    guiLengthToText( ct->ctWidthText, cp->cpWide, UNITtyPOINTS );

    /*
    guiEnableText( ct->ctWidthText,
			    ct->ctCanChange && tr->trCol0 == tr->trCol1 );
    */
    guiEnableText( ct->ctWidthText, ct->ctCanChange );

    guiEnableWidget( ct->ctIsRowHeaderToggle, ct->ctCanChange );
    guiSetToggleState( ct->ctIsRowHeaderToggle, cp->cpIsRowHeader );

    guiEnableWidget( ct->ctBordersFrame, ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctLeftBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctRightBorderTool), ct->ctCanChange );

    tedBorderToolSetPropertiesByNumber( &(ct->ctLeftBorderTool), bd,
						    cp->cpLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctRightBorderTool), bd,
						    cp->cpRightBorderNumber );

    tedEnableShadingTool( &(ct->ctShadingTool), ct->ctCanChange );
    tedSetShadingToolByNumber( &(ct->ctShadingTool), bd, cp->cpShadingNumber );
    }

static void tedRefreshColumnTool( void *			vct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    ColumnTool *		ct= (ColumnTool *)vct;

    const struct BufferItem *	rowNode;
    const RowProperties *	rp;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( ! sd->sdInOneTable )
	{
	docInitTableRectangle( &(ct->ctTableRectangle) );
	*pEnabled= 0; return;
	}
    ct->ctTableRectangle= sd->sdTableRectangle;

    rowNode= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); *pEnabled= 0; return;	}
    rp= rowNode->biRowProperties;

    ct->ctParentFrameWide= sg->sgHead.pgParentFrameX1-
						sg->sgHead.pgParentFrameX0;

    if  ( docCopyCellProperties( &(ct->ctPropertiesSet),
					&(sd->sdCellProperties), dam0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyCellProperties( &(ct->ctPropertiesChosen),
					&(ct->ctPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    ct->ctHalfGapWidthTwips= rp->rpHalfGapWidthTwips;

    guiEnableWidget( is->isPrevButton, ct->ctTableRectangle.trCol0 > 0 );
    guiEnableWidget( is->isNextButton,
		ct->ctTableRectangle.trCol1 < ct->ctTableRectangle.trCol11 );

    ct->ctCanChange= cmdEnabled[EDITcmdUPD_COLUMN_PROPS] ||
		     cmdEnabled[EDITcmdUPD_CELL_PROPS];

    tedFormatToolRefreshColumnPage( ct );

    guiEnableWidget( ct->ctColumnRow, ct->ctCanChange );
    guiEnableWidget( ct->ctWidthRow, ct->ctCanChange );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_COLUMN] );

    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_COLUMN] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_COLUMN] );

    guiEnableWidget( is->isRevertButton, ct->ctCanChange );
    guiEnableWidget( is->isApplyButton, ct->ctCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the width of the column(s) from the width widget.		*/
/*									*/
/************************************************************************/

static int tedFormatToolGetColumnWidth(	ColumnTool *		ct )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    int				width= cpChosen->cpWide;
    int				changed;

    const int			minValue= 20* 5+ 2* ct->ctHalfGapWidthTwips;
    const int			adaptToMin= 0;
    const int			maxValue= ct->ctParentFrameWide;
    const int			adaptToMax= 1;

    /*  2  */
    if  ( guiLengthFromText( ct->ctWidthText,
				&width, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1; }

    if  ( changed )
	{ cpChosen->cpWide= width;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the shading for a table rectangle from a shading tool.	*/
/*									*/
/************************************************************************/

static int tedFormatToolGetColumnShading( CellProperties *	cp,
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

static int tedColumnToolGetChosen(	ColumnTool *		ct,
					struct BufferDocument *	bd )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);
    int				res;

    res= tedFormatToolGetColumnWidth( ct );
    if  ( res != 0 )
	{ return -1;	}

    if  ( tedBorderToolGetNumber( &(cpChosen->cpLeftBorderNumber), (int *)0,
					    &(ct->ctLeftBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpRightBorderNumber), (int *)0,
					    &(ct->ctRightBorderTool), bd ) )
	{ return -1;	}

    if  ( tedFormatToolGetColumnShading( cpChosen, bd, &(ct->ctShadingTool) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static APP_TOGGLE_CALLBACK_H( tedColumnToogleIsRowHeader, w, voidct, voidtbcs )
    {
    ColumnTool *			ct= (ColumnTool *)voidct;
    CellProperties *			cp= &(ct->ctPropertiesChosen);
    int					set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    cp->cpIsRowHeader= set;

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeColumnPushed, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    PropertyMask		cpCmpMask;
    PropertyMask		cpDifMask;

    const int			wholeRow= 0;
    const int			wholeColumn= 1;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    /**/
    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    if  ( tedColumnToolGetChosen( ct, bd ) )
	{ goto ready;	}

    utilPropMaskClear( &cpCmpMask );
    utilPropMaskClear( &cpDifMask );
    utilPropMaskFill( &cpCmpMask, CLprop_COUNT );

    docCellPropertyDifference( &cpDifMask, &(ct->ctPropertiesSet),
						&cpCmpMask, cpChosen, dam0 );

    tedAppSetTableProperties( ct->ctApplication, wholeRow, wholeColumn,
			&cpDifMask, cpChosen,
			(const PropertyMask *)0, (const RowProperties *)0 );

  ready:

    docCleanBorderProperties( &bp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertColumnPushed, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    docCopyCellProperties( &(ct->ctPropertiesChosen), &(ct->ctPropertiesSet),
									dam0 );

    tedFormatToolRefreshColumnPage( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Check a number as typed by the user.				*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedColumnWidthChanged, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    if  ( ! tedFormatToolGetColumnWidth( ct ) )
	{
	CellProperties *	cpChosen= &(ct->ctPropertiesChosen);

	guiLengthToText( ct->ctWidthText,
					cpChosen->cpWide, UNITtyPOINTS );
	}
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedColumnPreviousColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= 0;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedColumnNextColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete Column/Row/Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteColumn, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;
    struct EditApplication *		ea= ct->ctApplication;

    tedAppDeleteColumn( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableInsertColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		after= 0;

    tedAppAddColumnToTable( ea, after );
    }

static APP_BUTTON_CALLBACK_H( tedTableAppendColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    struct EditApplication *	ea= ct->ctApplication;

    const int		after= 1;

    tedAppAddColumnToTable( ea, after );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedColumnToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ColumnTool *			ct= (ColumnTool *)voidct;

    switch( which )
	{
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

static void tedColumnToolMakeBorderFrame(
				ColumnTool *			ct,
				const ColumnPageResources *	cpr,
				struct AppInspector *			ai,
				int				subjectPage,
				APP_WIDGET			pageWidget )
    {
    guiMakeColumnFrameInColumn( &(ct->ctBordersFrame), &(ct->ctBordersPaned),
						pageWidget, cpr->cprBorders );

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
/*  Make a column tool, I.E. the 'Column' page of the format tool.	*/
/*									*/
/************************************************************************/

static void * tedFormatBuildColumnPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const ColumnPageResources *	cpr= &TED_ColumnPageResources;

    const int	textColumns= 10;

    ColumnTool *		ct= malloc( sizeof(ColumnTool) );

    if  ( ! ct )
	{ XDEB(ct); return ct;	}

    tedInitColumnTool( ct );

    /**/
    ct->ctPageResources= cpr;
    ct->ctCanChange= 1;
    ct->ctInspector= ai;
    ct->ctApplication= ea;

    /**/
    guiToolMakeLabelAndTextRow( &(ct->ctColumnRow),
			    &(ct->ctColumnLabel), &(ct->ctColumnText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(ct->ctWidthRow),
			    &(ct->ctWidthLabel), &(ct->ctWidthText),
			    pageWidget, cpr->cprWidth, textColumns, 1 );

    guiSetGotValueCallbackForText( ct->ctWidthText,
					tedColumnWidthChanged, (void *)ct );

    guiToolMakeToggleAndLabelRow( &(ct->ctIsRowHeaderRow), pageWidget,
	    &(ct->ctIsRowHeaderToggle), &(ct->ctIsRowHeaderLabel),
	    cpr->cprIsRowHeader, "",
	    tedColumnToogleIsRowHeader, (void *)ct );

    tedColumnToolMakeBorderFrame( ct, cpr, ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(ct->ctShadingTool), ai, pageWidget,
		/* rpr->rprShading, */ (const char *)0,
		&(cpr->cprShadingResources),
		subjectPage, CLprop_SHADE_FORE_COLOR, CLprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)ct );


    return (void *)ct;
    }

static void tedFormatFinishColumnPage(	ColumnTool *			ct,
					const ColumnPageResources *	cpr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(ct->ctApplication->eaPostScriptFontList);

    tedFinishBorderTool( &(ct->ctLeftBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctRightBorderTool), psfl );

    tedFinishShadingTool( &(ct->ctShadingTool), psfl );
    }

/************************************************************************/
/*									*/
/*  Get column tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedColumnSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolColumn",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Column" ),
    APP_RESOURCE( "tableToolChangeColumn",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Column" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectColumn",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Column" ),
    APP_RESOURCE( "tableToolDeleteColumn",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Column" ),
    APP_RESOURCE( "tableToolInsertColumn",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Column" ),
    APP_RESOURCE( "tableToolAppendColumn",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Column" ),
    };

static AppConfigurableResource TED_TedColumnToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolWidth",
		offsetof(ColumnPageResources,cprWidth),
		"Width" ),

    APP_RESOURCE( "formatToolColumnIsRowHeader",
		offsetof(ColumnPageResources,cprIsRowHeader),
		"Is Row Header" ),

    /**/
    APP_RESOURCE( "tableToolColumnBorders",
		offsetof(ColumnPageResources,cprBorders),
		"Borders" ),
    APP_RESOURCE( "tableToolColumnLeftBorder",
		offsetof(ColumnPageResources,cprLeftBorder),
		"Left" ),
    APP_RESOURCE( "tableToolColumnRightBorder",
		offsetof(ColumnPageResources,cprRightBorder),
		"Right" ),

    APP_RESOURCE( "formatToolColumnShadingPattern",
	    offsetof(ColumnPageResources,cprShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolColumnShadingLevel",
	    offsetof(ColumnPageResources,cprShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolColumnShadingForeColor",
	    offsetof(ColumnPageResources,cprShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolColumnShadingBackColor",
	    offsetof(ColumnPageResources,cprShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolColumnShadingBackColorChooserNoBackground",
	    offsetof(ColumnPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolColumnShadingBackColorChooserMoreColors",
	    offsetof(ColumnPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolColumnShadingForeColorChooserAutomatic",
	    offsetof(ColumnPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolColumnShadingForeColorChooserMoreColors",
	    offsetof(ColumnPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolColumnBorderWidth",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolColumnBorderStyle",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolColumnBorderColor",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserAutomatic",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserMoreColors",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),
    };

static void tedFormatToolGetColumnResourceTable(
					struct EditApplication *		ea,
					ColumnPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedColumnToolResourceTable,
				sizeof(TED_TedColumnToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedColumnSubjectResourceTable,
				sizeof(TED_TedColumnSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }


static void tedColumnLayoutToolFinishPage(
				void *				vct )
    {
    tedFormatFinishColumnPage( (ColumnTool *)vct,
						&TED_ColumnPageResources );
    }

int tedColumnToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetColumnResourceTable( ea,
			    &TED_ColumnPageResources,
			    &TED_ColumnSubjectResources );

    ist->istResources= &TED_ColumnSubjectResources;

    ist->istBuildTool= tedFormatBuildColumnPage;
    /*fts->ftsToolFillChoosers*/
    ist->istFinishPage= tedColumnLayoutToolFinishPage;
    ftst->ftstRefreshPage= tedRefreshColumnTool;
    ist->istFreeTool= tedFreeColumnTool;
    /*fts->ftsToolResetCache*/

    ist->istGotColor= tedColumnToolGotColor;

    ist->istMoveToNext= tedColumnNextColumn;
    ist->istMoveToPrev= tedColumnPreviousColumn;

    ist->istSelectInstance= tedTableSelectColumn;
    ist->istDeleteInstance= tedTableDeleteColumn;

    ist->istInsertInstance= tedTableInsertColumn;
    ist->istAppendInstance= tedTableAppendColumn;

    ist->istRevertChanges= tedFormatRevertColumnPushed;
    ist->istApplyChanges= tedTableChangeColumnPushed;

    return 0;
    }
