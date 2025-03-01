/************************************************************************/
/*									*/
/*  Table page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"tedTableTool.h"
#   include	<tedAppFront.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docRowProperties.h>
#   include	"tedRowMarginsTool.h"
#   include	<docTableRectangle.h>
#   include	<appInspectorSubject.h>
#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Table' page.					*/
/*									*/
/************************************************************************/

typedef struct TablePageResources
    {
    char *			tprTableNesting;
    RowMarginsToolResources	tprMarginsResources;
    } TablePageResources;

/************************************************************************/
/*									*/
/*  A table tool, i.e. the 'Table' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TableTool
    {
    struct EditApplication *	ttApplication;
    struct AppInspector *	ttInspector;
    const TablePageResources *	ttPageResources;

    RowProperties		ttPropertiesSet;
    RowProperties		ttPropertiesChosen;

    TableRectangle		ttTableRectangle;

    unsigned char		ttCanChange;
    unsigned char		ttTableNesting;

    APP_WIDGET			ttNestingRow;
    APP_WIDGET			ttNestingLabel;
    APP_WIDGET			ttNestingText;

    RowMarginsTool		ttMarginsTool;
    } TableTool;

static TablePageResources		TED_TablePageResources;
static InspectorSubjectResources	TED_TableSubjectResources;

static void tedInitTableTool(	TableTool *	tt )
    {
    memset( tt, 0, sizeof(TableTool) );
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Table' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTablePage(	TableTool *	tt )
    {
    RowProperties *	rp= &(tt->ttPropertiesChosen);

    guiIntegerToText( tt->ttNestingText, tt->ttTableNesting );

    tedRefreshRowMarginsTool( &(tt->ttMarginsTool), rp, tt->ttCanChange );
    }

static void tedRefreshTableTool( void *				vtt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    TableTool *			tt= (TableTool *)vtt;
    struct BufferItem *		rowNode;
    const RowProperties *	rp;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( ! sd->sdInOneTable )
	{ *pEnabled= 0; return;	}

    tt->ttTableRectangle= sd->sdTableRectangle;

    rowNode= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); *pEnabled= 0; return;	}
    rp= rowNode->biRowProperties;

    tedRowMarginToolGetMargins( &(tt->ttMarginsTool), rowNode, bd );

    if  ( docCopyRowProperties( &(tt->ttPropertiesSet), rp, dam0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(tt->ttPropertiesChosen),
					&(tt->ttPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    tt->ttCanChange= cmdEnabled[EDITcmdUPD_TABLE_PROPS]		||
		     cmdEnabled[EDITcmdUPD_ROW_PROPS]		||
		     cmdEnabled[EDITcmdUPD_COLUMN_PROPS]	||
		     cmdEnabled[EDITcmdUPD_CELL_PROPS];

    tt->ttTableNesting= sd->sdHeadTableNesting;

    tedFormatToolRefreshTablePage( tt );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_TABLE] );

    guiEnableWidget( is->isRevertButton, tt->ttCanChange );
    guiEnableWidget( is->isApplyButton, tt->ttCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/

static int tedTableToolGetChosen(	TableTool *		tt )
    {
    RowProperties *		rpChosen= &(tt->ttPropertiesChosen);

    if  ( tedRowMarginToolGetValues( rpChosen, &(tt->ttMarginsTool) ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeTablePushed, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    RowProperties *		rpChosen= &(tt->ttPropertiesChosen);

    PropertyMask		rpCmpMask;
    PropertyMask		cpCmpMask;
    PropertyMask		rpDifMask;

    const int			wholeRow= 1;
    const int			wholeColumn= 1;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    if  ( tedTableToolGetChosen( tt ) )
	{ return;	}

    utilPropMaskClear( &rpCmpMask );
    utilPropMaskClear( &cpCmpMask );
    utilPropMaskClear( &rpDifMask );

    utilPropMaskFill( &rpCmpMask, RPprop_FULL_COUNT );
    utilPropMaskFill( &cpCmpMask, CLprop_COUNT );

    if  ( docRowPropertyDifference( &rpDifMask, (PropertyMask *)0,
				&(tt->ttPropertiesSet),
				&rpCmpMask, &cpCmpMask,
				rpChosen, 0, rpChosen->rpCellCount, dam0 ) )
	{ LDEB(rpChosen->rpCellCount); return;	}

    tedAppSetTableProperties( tt->ttApplication, wholeRow, wholeColumn,
			(const PropertyMask *)0, (const CellProperties *)0,
			&rpDifMask, rpChosen );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertTablePushed, w, voidtt )
    {
    TableTool *	tt= (TableTool *)voidtt;

    docCopyRowProperties( &(tt->ttPropertiesChosen), &(tt->ttPropertiesSet),
				(const struct DocumentAttributeMap *)0 );

    tedFormatToolRefreshTablePage( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check the cell margin that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableCellMarginChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;

    tedMarginToolCheckCellMargin( &(tt->ttMarginsTool),
					    &(tt->ttPropertiesChosen) );
    }

/************************************************************************/
/*									*/
/*  Check the left indent that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableLeftIndentChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;

    tedMarginToolCheckLeftIndent( &(tt->ttMarginsTool),
						&(tt->ttPropertiesChosen) );
    }

/************************************************************************/
/*									*/
/*  Delete/Select Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;

    tedAppDeleteTable( ea );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;

    tedAppSelectTable( ea );
    }

/************************************************************************/
/*									*/
/*  Make the 'Table' page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildTablePage(
				int				subjectPage,
				struct AppInspector *			ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const TablePageResources *	tpr= &TED_TablePageResources;

    const int			textColumns= 10;

    TableTool *			tt= malloc( sizeof(TableTool) );

    if  ( ! tt )
	{ XDEB(tt); return tt;	}

    tedInitTableTool( tt );

    /**/
    tt->ttPageResources= tpr;
    tt->ttInspector= ai;
    tt->ttApplication= ea;
    tt->ttCanChange= 1;

    /**/
    docInitTableRectangle( &(tt->ttTableRectangle) );

    docInitRowProperties( &(tt->ttPropertiesSet) );
    docInitRowProperties( &(tt->ttPropertiesChosen) );

    /**************/
    guiToolMakeLabelAndTextRow( &(tt->ttNestingRow),
			&(tt->ttNestingLabel), &(tt->ttNestingText),
			pageWidget, tpr->tprTableNesting, textColumns, 0 );

    /**************/
    tedFillRowMarginsTool( &(tt->ttMarginsTool), &(tpr->tprMarginsResources),
    			textColumns,
			tedTableLeftIndentChanged, tedTableCellMarginChanged,
			(void *)tt, pageWidget );

    return (void *)tt;
    }

/************************************************************************/
/*									*/
/*  Clean the table tool.						*/
/*									*/
/************************************************************************/

static void tedFormatFreeTableTool(	void *	vtt )
    {
    TableTool *		tt= (TableTool *)vtt;

    docCleanRowProperties( &(tt->ttPropertiesSet) );
    docCleanRowProperties( &(tt->ttPropertiesChosen) );

    free( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Table tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTableSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolTable",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Table" ),
    APP_RESOURCE( "tableToolChangeTable",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Table" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Table */
    /*  no Prev Table */
    APP_RESOURCE( "tableToolSelectTable",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Table" ),
    APP_RESOURCE( "tableToolDeleteTable",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Table" ),
    /*  no Insert Table */
    /*  no Append Table */
    };

static AppConfigurableResource TED_TedTableToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "tableToolTableNesting",
		offsetof(TablePageResources,tprTableNesting),
		"Table Nesting" ),
    /**/
    APP_RESOURCE( "tableToolCellMargin",
		offsetof(TablePageResources,tprMarginsResources.rmtrCellMargin),
		"Cell Margin" ),
    APP_RESOURCE( "tableToolLeftIndent",
		offsetof(TablePageResources,tprMarginsResources.rmtrLeftIndent),
		"Left Margin" ),
    };

static void tedFormatToolGetTableResourceTable(
					struct EditApplication *	ea,
					TablePageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTableToolResourceTable,
				sizeof(TED_TedTableToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTableSubjectResourceTable,
				sizeof(TED_TedTableSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedTableToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetTableResourceTable( ea,
				    &TED_TablePageResources,
				    &TED_TableSubjectResources );

    ist->istResources= &TED_TableSubjectResources;

    ist->istBuildTool= tedFormatBuildTablePage;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ist->istFreeTool= tedFormatFreeTableTool;

    ftst->ftstRefreshPage= tedRefreshTableTool;
    /*fts->ftsToolResetCache*/

    ist->istSelectInstance= tedTableSelectTable;
    ist->istDeleteInstance= tedTableDeleteTable;

    ist->istRevertChanges= tedFormatRevertTablePushed;
    ist->istApplyChanges= tedTableChangeTablePushed;

    return 0;
    }
