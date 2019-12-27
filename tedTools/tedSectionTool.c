/************************************************************************/
/*									*/
/*  The Section page of the Format Tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	<limits.h>
#   include	<stdio.h>

#   include	<geoUnit.h>

#   include	"tedSectionTool.h"
#   include	<tedAppFront.h>
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docBreakKind.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<utilPropMask.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<docSectProperties.h>
#   include	<docDocumentProperties.h>
#   include	<docBuf.h>
#   include	"tedFormatToolSubject.h"
#   include	"tedBreakTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Section; page.					*/
/*									*/
/************************************************************************/

typedef struct SectionPageResources
    {
    const char *	sprUnitType;

    const char *	sprNumberingRestarts;

    const char *	sprPageNumberStyle;
    const char *	sprNumberStyleMenuTexts[DOCpgn_COUNT];

    BreakToolResources	sprBreakToolResources;

    const char *	sprDirection;
    const char *	sprDirectionMenuTexts[2];

    const char *	sprColumns;
    const char *	sprColumnCount;
    const char *	sprColumn;
    const char *	sprColumnWidth;
    const char *	sprColumnSpacing;
    const char *	sprLineBetweenColumns;
    const char *	sprColumnsEqualWidth;
    } SectionPageResources;

/************************************************************************/
/*									*/
/*  A section tool, i.e. the 'Section' page of the format tool.		*/
/*									*/
/************************************************************************/

# define SECT_MAX_COLUMNS	4

typedef struct SectionTool
    {
    struct EditApplication *		stApplication;
    struct AppInspector *		stInspector;
    const SectionPageResources *	stPageResources;

    int					stUnitInt;

    SectionProperties			stPropertiesSet;
    SectionProperties			stPropertiesChosen;

    int					stSectionNumber;
    int					stCanChange;

    APP_WIDGET				stNumberRow;
    APP_WIDGET				stNumberLabel;
    APP_WIDGET				stNumberText;

    APP_WIDGET				stPageRestartRow;
    APP_WIDGET				stPageRestartToggle;
    APP_WIDGET				stPageRestartText;

    APP_WIDGET				stNumberStyleRow;
    AppOptionmenu			stNumberStyleOptionmenu;

    BreakTool				stBreakTool;

    APP_WIDGET				stDirectionRow;
    APP_WIDGET				stDirectionLabel;
    AppOptionmenu			stDirectionOptionmenu;

    APP_WIDGET				stColumnsFrame;
    APP_WIDGET				stColumnsPaned;

    AppOptionmenu			stColumnCountMenu;

    APP_WIDGET				stLineBetweenColumnsToggle;
    APP_WIDGET				stEqualWidthToggle;

    APP_WIDGET				stColumnRows[SECT_MAX_COLUMNS];
    APP_WIDGET				stColnoLabels[SECT_MAX_COLUMNS];
    APP_WIDGET				stColwTexts[SECT_MAX_COLUMNS];
    APP_WIDGET				stColsrTexts[SECT_MAX_COLUMNS];
    } SectionTool;

static SectionPageResources		TED_SectionPageResources;
static InspectorSubjectResources	TED_SectionSubjectResources;

static void tedInitSectionTool(	SectionTool *	st )
    {
    memset( st, 0, sizeof(SectionTool) );
    }

/************************************************************************/
/*									*/
/*  Refresh the section page.						*/
/*									*/
/************************************************************************/

static void tedSectionToolRefreshPageRestart(	SectionTool *	st )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);

    if  ( sp->spRestartPageNumbers )
	{
	guiIntegerToText( st->stPageRestartText,
						sp->spStartPageNumber+ 1 );
	guiEnableText( st->stPageRestartText, st->stCanChange );

	guiSetToggleState( st->stPageRestartToggle, 1 );
	}
    else{
	guiStringToText( st->stPageRestartText, "" );
	guiEnableText( st->stPageRestartText, 0 );

	guiSetToggleState( st->stPageRestartToggle, 0 );
	}
    }

static void tedSectionToolRefreshColumns(	SectionTool *	st )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);
    int				col;
    int				emptyFrom= sp->spColumnCount;
    int				disabledFrom= sp->spColumnCount;

    if  ( sp->spColumnCount < 2 )
	{
	guiEnableWidget( st->stEqualWidthToggle, 0 );
	guiSetToggleState( st->stEqualWidthToggle, 0 );

	guiEnableWidget( st->stLineBetweenColumnsToggle, 0 );
	guiSetToggleState( st->stLineBetweenColumnsToggle, 0 );

	guiLengthToText( st->stColwTexts[0],
			docSectGetColumnWidth( (int *)0, (int *)0, sp, 0 ),
			st->stUnitInt );

	guiStringToText( st->stColsrTexts[0], "" );
	emptyFrom= 0;
	disabledFrom= 0;
	}
    else{
	int	haveFixedWidth= docSectPropsFixedWidthColumns( sp );

	guiEnableWidget( st->stEqualWidthToggle, 1 );
	guiSetToggleState( st->stEqualWidthToggle, haveFixedWidth );

	guiEnableWidget( st->stLineBetweenColumnsToggle, 1 );
	guiSetToggleState( st->stLineBetweenColumnsToggle,
					    sp->spLineBetweenColumns != 0 );

	if  ( haveFixedWidth )
	    {
	    int		fixedWidth;

	    fixedWidth= docSectGetColumnWidth( (int *)0, (int *)0, sp, 0 ),

	    guiEnableText( st->stColwTexts[0], 1 );
	    guiEnableText( st->stColsrTexts[0], 1 );

	    guiLengthToText( st->stColwTexts[0],
				    fixedWidth, st->stUnitInt );
	    guiLengthToText( st->stColsrTexts[0],
				    sp->spColumnSpacingTwips, st->stUnitInt );

	    for ( col= 1; col < sp->spColumnCount- 1; col++ )
		{
		if  ( col >= SECT_MAX_COLUMNS )
		    { LLDEB(col,SECT_MAX_COLUMNS); break;	}

		guiLengthToText( st->stColwTexts[col],
				    fixedWidth, st->stUnitInt );
		guiLengthToText( st->stColsrTexts[col],
				    sp->spColumnSpacingTwips, st->stUnitInt );
		}

	    col= sp->spColumnCount- 1;
	    if  ( col < SECT_MAX_COLUMNS )
		{
		guiLengthToText( st->stColwTexts[col],
						fixedWidth, st->stUnitInt );

		guiStringToText( st->stColsrTexts[col], "" );
		guiEnableText( st->stColwTexts[col], 0 );
		guiEnableText( st->stColsrTexts[col], 0 );
		}

	    disabledFrom= 1;
	    }
	else{
	    for ( col= 0; col < sp->spColumnCount- 1; col++ )
		{
		if  ( col >= SECT_MAX_COLUMNS )
		    { LLDEB(col,SECT_MAX_COLUMNS); break;	}

		guiLengthToText( st->stColwTexts[col],
					sp->spColumns[col].scColumnWidthTwips,
					st->stUnitInt );
		guiLengthToText( st->stColsrTexts[col],
					sp->spColumns[col].scSpaceAfterTwips,
					st->stUnitInt );

		guiEnableText( st->stColwTexts[col], 1 );
		guiEnableText( st->stColsrTexts[col], 1 );
		}

	    col= sp->spColumnCount- 1;
	    if  ( col < SECT_MAX_COLUMNS )
		{
		guiLengthToText( st->stColwTexts[col],
					sp->spColumns[col].scColumnWidthTwips,
					st->stUnitInt );
		guiStringToText( st->stColsrTexts[col], "" );

		guiEnableText( st->stColwTexts[col], 1 );
		guiEnableText( st->stColsrTexts[col], 0 );
		}
	    }
	}

    for ( col= emptyFrom; col < SECT_MAX_COLUMNS; col++ )
	{
	guiStringToText( st->stColwTexts[col], "" );
	guiStringToText( st->stColsrTexts[col], "" );
	}

    for ( col= disabledFrom; col < SECT_MAX_COLUMNS; col++ )
	{
	guiEnableText( st->stColwTexts[col], 0 );
	guiEnableText( st->stColsrTexts[col], 0 );
	}

    return;
    }

static void tedFormatToolRefreshSectionPage(	SectionTool *	st )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);

    guiIntegerToText( st->stNumberText, st->stSectionNumber+ 1 );

    guiSetOptionmenu( &(st->stNumberStyleOptionmenu), sp->spPageNumberStyle );

    tedSetBreakTool( &(st->stBreakTool), sp->spBreakKind );

    guiSetOptionmenu( &(st->stDirectionOptionmenu), sp->spRToL );

    tedSectionToolRefreshPageRestart( st );

    if  ( sp->spColumnCount > SECT_MAX_COLUMNS )
	{ guiSetOptionmenu( &(st->stColumnCountMenu), SECT_MAX_COLUMNS ); }
    else{ guiSetOptionmenu( &(st->stColumnCountMenu), sp->spColumnCount- 1 ); }

    tedSectionToolRefreshColumns( st );

    return;
    }

static void tedRefreshSectionTool(
			void *					vst,
			int *					pEnabled,
			int *					pPref,
			InspectorSubject *			is,
			const DocumentSelection *		ds,
			const struct SelectionGeometry *	sg,
			const struct SelectionDescription *	sd,
			const struct BufferDocument *		bd,
			const unsigned char *			cmdEnabled )
    {
    const DocumentProperties *	dp= bd->bdProperties;
    SectionTool *		st= (SectionTool *)vst;
    struct BufferItem *		sectNode;

    sectNode= docGetSectNode( ds->dsHead.dpNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); return;	}

    st->stSectionNumber= sectNode->biNumberInParent;

    if  ( docCopySectionProperties( &(st->stPropertiesSet),
					    sectNode->biSectProperties ) )
	{ LDEB(1); return ;	}

    if  ( docCopySectionProperties( &(st->stPropertiesChosen),
					    &(st->stPropertiesSet) ))
	{ LDEB(1); return ;	}

    st->stCanChange= cmdEnabled[EDITcmdUPD_SECT_PROPS];

    guiEnableWidget( is->isPrevButton,
			docPrevSection( sectNode ) != (struct BufferItem *)0 );
    guiEnableWidget( is->isNextButton,
			docNextSection( sectNode ) != (struct BufferItem *)0 );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_SECT] );
    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_SECT] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_SECT] );

    guiEnableWidget( is->isRevertButton, st->stCanChange );
    guiEnableWidget( is->isApplyButton, st->stCanChange );

    guiEnableWidget( st->stNumberRow, st->stCanChange );

    guiEnableWidget( st->stPageRestartRow, st->stCanChange );
    guiEnableWidget( st->stNumberStyleRow, st->stCanChange );

    tedEnableBreakTool( &(st->stBreakTool),
			st->stCanChange && st->stSectionNumber > 0 );
    tedEnableBreakToolOption( &(st->stBreakTool),
		    DOCibkCOL, sectNode->biSectProperties->spColumnCount > 1 );
    tedEnableBreakToolOption( &(st->stBreakTool),
					DOCibkODD, dp->dpHasFacingPages );
    tedEnableBreakToolOption( &(st->stBreakTool),
					DOCibkEVEN, dp->dpHasFacingPages );

    guiEnableWidget( st->stColumnsFrame, st->stCanChange );

    guiEnableWidget( st->stNumberStyleRow, st->stCanChange );
    guiEnableOptionmenu( &(st->stNumberStyleOptionmenu), st->stCanChange );

    guiEnableOptionmenu( &(st->stDirectionOptionmenu), st->stCanChange );

    tedFormatToolRefreshSectionPage( st );

    *pEnabled= ds->dsSelectionScope.ssTreeType == DOCinBODY;

    return;
    }

/************************************************************************/
/*									*/
/*  Extract the value from one of the spacing text widgets.		*/
/*									*/
/************************************************************************/

static int tedSectToolGetColSpacing(	int *			pChanged,
					SectionTool *		st,
					int			col,
					APP_WIDGET		w )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				value;
    int				changed= 0;

    const int			adaptToMin= 1;
    const int			adaptToMax= 1;
    int				minValue;
    int				maxValue;

    value= docSectGetColumnSpacing( &minValue, &maxValue, sp, col );
    if  ( value < 0 )
	{ LDEB(value); return -1;	}

    if  ( guiLengthFromText( w,
			&value, &changed, st->stUnitInt,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return 1;	}
    if  ( ! changed )
	{ *pChanged= 0; return 0;	}

    if  ( docSectSetColumnSpacing( sp, col, value ) )
	{ LLDEB(col,value); return -1;	}

    *pChanged= 1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract the value from one of the column width text widgets.	*/
/*									*/
/************************************************************************/

static int tedSectToolGetColWidth(	int *			pChanged,
					SectionTool *		st,
					int			col,
					APP_WIDGET		w )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				value;
    int				changed= 0;

    const int			adaptToMin= 1;
    const int			adaptToMax= 1;
    int				minValue;
    int				maxValue;

    value= docSectGetColumnWidth( &minValue, &maxValue, sp, col );
    if  ( value < 0 )
	{ LDEB(value); return -1;	}

    if  ( guiLengthFromText( w,
			&value, &changed, st->stUnitInt,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return 1;	}
    if  ( ! changed )
	{ *pChanged= 0; return 0;	}

    if  ( docSectSetColumnWidth( sp, col, value ) )
	{ LLDEB(col,value); return -1;	}

    *pChanged= 1;
    return 0;
    }


/************************************************************************/

static int tedSectToolGetChosen(	SectionTool *		st )
    {
    SectionProperties *		spChosen= &(st->stPropertiesChosen);

    spChosen->spRestartPageNumbers=
		guiGetToggleState( st->stPageRestartToggle ) != 0;

    if  ( spChosen->spRestartPageNumbers )
	{
	int			n;

	if  ( guiIntegerFromText( st->stPageRestartText, &n,
							1, 0, INT_MAX, 0 ) )
	    { return -1;	}

	spChosen->spStartPageNumber= n- 1;
	}

    if  ( spChosen->spColumnCount > 1 )
	{
	int		col;
	int		upto;
	int		changed= 0;
	int		haveFixedWidth;

	haveFixedWidth= docSectPropsFixedWidthColumns( spChosen );

	upto= 1;
	if  ( ! haveFixedWidth )
	    { upto= spChosen->spColumnCount- 1;	}

	for ( col= 0; col < upto; col++ )
	    {
	    if  ( tedSectToolGetColSpacing( &changed, st,
						col, st->stColsrTexts[col] ) )
		{ return -1;	}
	    }

	upto= 1;
	if  ( ! haveFixedWidth )
	    { upto= spChosen->spColumnCount;	}

	for ( col= 0; col < upto; col++ )
	    {
	    if  ( tedSectToolGetColWidth( &changed, st,
						col, st->stColwTexts[col] ) )
		{ return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeSectPushed, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		spChosen= &(st->stPropertiesChosen);
    SectionProperties *		spSet= &(st->stPropertiesSet);

    PropertyMask		spCmpMask;
    PropertyMask		spDifMask;

    if  ( tedSectToolGetChosen( st ) )
	{ return;	}

    utilPropMaskClear( &spCmpMask );
    utilPropMaskClear( &spDifMask );
    utilPropMaskFill( &spCmpMask, SPprop_COUNT );

    docSectPropertyDifference( &spDifMask, spChosen, &spCmpMask, spSet );

    tedAppChangeSectionProperties( st->stApplication, &spDifMask, spChosen );

    return;
    }

static void tedSectNumberStyleChosen(	int		style,
					void *		voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    if  ( style < 0 || style >= DOCpgn_COUNT )
	{ LLDEB(style,DOCpgn_COUNT); return;	}

    sp->spPageNumberStyle= style;

    return;
    }

static void tedSectBreakKindChosen(	int		breakKind,
					void *		voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    if  ( breakKind < 0 || breakKind >= DOCibk_COUNT )
	{ LLDEB(breakKind,DOCibk_COUNT); return;	}

    sp->spBreakKind= breakKind;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatPageRestartToggled, w, voidst, voidtbcs )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    sp->spRestartPageNumbers= ( set != 0 );

    tedSectionToolRefreshPageRestart( st );

    return;
    }

static void tedColumnCountChosen(	int		val,
					void *		voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    if  ( val < 0 || val >= SECT_MAX_COLUMNS )
	{ LLDEB(val,SECT_MAX_COLUMNS); return;	}

    if  ( sp->spColumnCount != val+ 1 )
	{
	if  ( docSectionPropertiesSetColumnCount( sp, val+ 1 ) )
	    { LDEB(val+1); return;	}

	docSectSetEqualColumnWidth( sp );

	tedSectionToolRefreshColumns( st );

	tedEnableBreakToolOption( &(st->stBreakTool),
					    DOCibkCOL, sp->spColumnCount > 1 );
	}

    return;
    }

static void tedSectDirectionChosen(	int		direction,
					void *		voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    if  ( direction < 0 || direction >= 2 )
	{ LLDEB(direction,2); return;	}

    sp->spRToL= direction;

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedSectColWidthChanged, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);
    int				col;
    int				changed= 0;

    if  ( sp->spColumnCount < 2 )
	{ LDEB(sp->spColumnCount); return;	}

    for ( col= 0; col < sp->spColumnCount; col++ )
	{
	if  ( w == st->stColwTexts[col] )
	    { break;	}
	}

    if  ( col >= sp->spColumnCount )
	{ LLDEB(col,sp->spColumnCount); return;	}

    if  ( tedSectToolGetColWidth( &changed, st, col, w ) || ! changed )
	{ return;	}

    tedSectionToolRefreshColumns( st );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedSectColSpacingChanged, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);
    int				col;
    int				changed= 0;

    if  ( sp->spColumnCount < 2 )
	{ LDEB(sp->spColumnCount); return;	}

    for ( col= 0; col < sp->spColumnCount; col++ )
	{
	if  ( w == st->stColsrTexts[col] )
	    { break;	}
	}

    if  ( col >= sp->spColumnCount )
	{ LLDEB(col,sp->spColumnCount); return;	}

    if  ( tedSectToolGetColSpacing( &changed, st, col, w ) || ! changed )
	{ return;	}

    tedSectionToolRefreshColumns( st );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertSectPushed, w, voidst )
    {
    SectionTool *	st= (SectionTool *)voidst;

    docCopySectionProperties( &(st->stPropertiesChosen),
						    &(st->stPropertiesSet) );
    tedFormatToolRefreshSectionPage( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Section buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;

    tedAppSelectWholeSection( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;

    tedAppSelectWholeSection( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;

    tedAppSelectWholeSection( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeleteSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;

    tedAppDeleteSelectedSections( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;
    const int			after= 0;

    tedAppInsertSection( ea, after );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    struct EditApplication *	ea= st->stApplication;
    const int			after= 1;

    tedAppInsertSection( ea, after );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedSectLineBetweenToggled, w, voidst, voidtbcs )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    sp->spLineBetweenColumns= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedSectEqualWidthToggled, w, voidst, voidtbcs )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{ docSectSetEqualColumnWidth( sp );	}
    else{
	if  ( sp->spColumnCount > 1 )
	    {
	    docSectSetExplicitColumnWidth( sp );
	    }
	}

    tedSectionToolRefreshColumns( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a section tool. I.E. the 'Section' page of the format tool.	*/
/*									*/
/************************************************************************/

static void tedSectionToolFillColumnsFrame(
				SectionTool *			st,
				const SectionPageResources *	spr,
				APP_WIDGET			frameColumn )
    {
    int			col;
    APP_WIDGET		row;
    APP_WIDGET		rowLabel;

    /**************/
    guiToolMakeLabelAndMenuRow( &row,
			    &(st->stColumnCountMenu),
			    &rowLabel, frameColumn, spr->sprColumnCount,
			    tedColumnCountChosen, (void *)st );

    guiToolMake2ToggleRow( &row, frameColumn,
		    &(st->stEqualWidthToggle),
		    &(st->stLineBetweenColumnsToggle),
		    spr->sprColumnsEqualWidth,
		    spr->sprLineBetweenColumns,
		    tedSectEqualWidthToggled,
		    tedSectLineBetweenToggled,
		    (void *)st, (void *)st );

    /**************/
    row= guiMakeRowInColumn( frameColumn, 5, 0 );
    guiMakeLabelInRow( &rowLabel, row, 0, 1, spr->sprColumn );
    guiMakeLabelInRow( &rowLabel, row, 1, 2, spr->sprColumnWidth );
    guiMakeLabelInRow( &rowLabel, row, 3, 2, spr->sprColumnSpacing );

    for ( col= 0; col < SECT_MAX_COLUMNS; col++ )
	{
	char		scratch[20];
	const int	textEnabled= 0;
	const int	textColumns= 7;

	st->stColumnRows[col]= guiMakeRowInColumn( frameColumn, 5, 0 );

	sprintf( scratch, "%d", col+ 1 );
	guiMakeLabelInRow( &(st->stColnoLabels[col]),
		    st->stColumnRows[col], 0, 1, scratch );

	guiMakeTextInRow( &(st->stColwTexts[col]),
		    st->stColumnRows[col], 1, 2, textColumns, textEnabled );
	guiMakeTextInRow( &(st->stColsrTexts[col]),
		    st->stColumnRows[col], 3, 2, textColumns, textEnabled );

	guiSetGotValueCallbackForText( st->stColwTexts[col],
				    tedSectColWidthChanged, (void *)st );
	guiSetGotValueCallbackForText( st->stColsrTexts[col],
				    tedSectColSpacingChanged, (void *)st );
	}

    return;
    }

static void * tedFormatBuildSectionPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const SectionPageResources *	spr= &TED_SectionPageResources;

    APP_WIDGET	rowLabel;

    const int	textColumns= 10;

    SectionTool *			st= malloc( sizeof(SectionTool) );

    if  ( ! st )
	{ XDEB(st); return st;	}

    tedInitSectionTool( st );

    /**/

    st->stUnitInt= geoUnitTypeInt( spr->sprUnitType );
    if  ( st->stUnitInt < 0 )
	{ SDEB(spr->sprUnitType); st->stUnitInt= UNITtyPOINTS;	}

    /**/
    st->stPageResources= spr;
    st->stInspector= ai;
    st->stApplication= ea;

    /**/
    st->stSectionNumber= -1;

    /**************/
    guiToolMakeLabelAndTextRow( &(st->stNumberRow),
			    &(st->stNumberLabel), &(st->stNumberText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    /**************/
    tedBuildBreakTool( pageWidget, &(st->stBreakTool),
				&(spr->sprBreakToolResources),
				tedSectBreakKindChosen, (void *)st );

    /**************/
    guiToolMakeLabelAndMenuRow( &(st->stNumberStyleRow),
			&(st->stNumberStyleOptionmenu),
			&rowLabel, pageWidget, spr->sprPageNumberStyle,
			tedSectNumberStyleChosen, (void *)st );
    /**************/
    guiToolMakeToggleAndTextRow( &(st->stPageRestartRow),
			&(st->stPageRestartToggle),
			&(st->stPageRestartText), pageWidget,
			spr->sprNumberingRestarts,
			tedFormatPageRestartToggled, (void *)st,
			textColumns, 1 );

    /**/
    guiToolMakeLabelAndMenuRow( &(st->stDirectionRow),
		&(st->stDirectionOptionmenu), &(st->stDirectionLabel),
		pageWidget, spr->sprDirection,
		tedSectDirectionChosen, (void *)st );

    /**************/
    guiMakeColumnFrameInColumn( &(st->stColumnsFrame),
				&(st->stColumnsPaned),
				pageWidget, spr->sprColumns );

    tedSectionToolFillColumnsFrame( st, spr, st->stColumnsPaned );

    return (void *)st;
    }

/************************************************************************/
/*									*/
/*  Finish the section page.						*/
/*									*/
/************************************************************************/

static void tedFormatFillSectionChoosers(
				void *				vst )
    {
    SectionTool *			st= (SectionTool *)vst;
    const SectionPageResources *	spr= &TED_SectionPageResources;

    int			i;
    char		texts[20][SECT_MAX_COLUMNS+ 1];
    const char *	columnCountTexts[SECT_MAX_COLUMNS+ 1];

    appFillInspectorMenu( DOCpgn_COUNT, DOCpgnDEC,
		spr->sprNumberStyleMenuTexts, &(st->stNumberStyleOptionmenu) );

    tedFillBreakToolChooser( DOCibk_COUNT, DOCibkPAGE,
		&(st->stBreakTool), &(spr->sprBreakToolResources) );

    for ( i= 0; i < SECT_MAX_COLUMNS; i++ )
	{
	sprintf( texts[i], "%d", i+ 1 );
	columnCountTexts[i]= texts[i];
	}
    sprintf( texts[SECT_MAX_COLUMNS], "> %d", SECT_MAX_COLUMNS );
    columnCountTexts[SECT_MAX_COLUMNS]= texts[SECT_MAX_COLUMNS];

    appFillInspectorMenu( SECT_MAX_COLUMNS+ 1, 1,
			columnCountTexts, &(st->stColumnCountMenu) );

    guiOptionMenuEnablePosition( &(st->stColumnCountMenu),
						    SECT_MAX_COLUMNS, 0 );

    appFillInspectorMenu( 2, 0,
	    spr->sprDirectionMenuTexts, &(st->stDirectionOptionmenu) );

    return;
    }

static void tedFormatFinishSectionPage(	void *		vst )
    {
    SectionTool *	st= (SectionTool *)vst;

    guiOptionmenuRefreshWidth( &(st->stNumberStyleOptionmenu) );

    tedFinishBreakTool( &(st->stBreakTool) );

    guiOptionmenuRefreshWidth( &(st->stColumnCountMenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean a section tool						*/
/*									*/
/************************************************************************/

static void tedFormatFreeSectionTool(	void *		vst )
    {
    SectionTool *	st= (SectionTool *)vst;

    docCleanSectionProperties( &(st->stPropertiesSet) );
    docCleanSectionProperties( &(st->stPropertiesChosen) );

    free( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Get section tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedSectionSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolSect",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Section" ),
    APP_RESOURCE( "formatToolChangeSect",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Section" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "formatToolNextSect",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Section" ),
    APP_RESOURCE( "formatToolPreviousSect",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Section" ),
    APP_RESOURCE( "formatToolSelectSect",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Section" ),
    APP_RESOURCE( "formatToolDeleteSect",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Section" ),
    APP_RESOURCE( "formatToolInsertSect",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Section" ),
    APP_RESOURCE( "formatToolAppendSect",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Section" ),
    };

static AppConfigurableResource TED_TedSectionToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "unit",
		offsetof(SectionPageResources,sprUnitType),
		"inch" ),

    APP_RESOURCE( "formatToolSectNumberRestart",
		offsetof(SectionPageResources,sprNumberingRestarts),
		"Start with Page" ),
    /**/
    APP_RESOURCE( "formatToolSectPageNumberStyle",
	    offsetof(SectionPageResources,sprPageNumberStyle),
	    "Page Number Style" ),
    APP_RESOURCE( "formatToolSectNumberStyleDec",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnDEC]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolSectNumberStyleUcrm",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnUCRM]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolSectNumberStyleLcrm",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnLCRM]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolSectNumberStyleUcltr",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnUCLTR]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolSectNumberStyleLcltr",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnLCLTR]),
	    "a, b, c" ),
    /**/
    APP_RESOURCE( "formatToolSectBreakKind",
	    offsetof(SectionPageResources,sprBreakToolResources.btrBreakKind),
	    "Begins" ),
    APP_RESOURCE( "formatToolSectBreakKindNone",
	    offsetof(SectionPageResources,
		    sprBreakToolResources.btrBreakKindMenuTexts[DOCibkNONE]),
	    "Below Previous" ),
    APP_RESOURCE( "formatToolSectBreakKindCol",
	    offsetof(SectionPageResources,
		    sprBreakToolResources.btrBreakKindMenuTexts[DOCibkCOL]),
	    "In Next Column" ),
    APP_RESOURCE( "formatToolSectBreakKindPage",
	    offsetof(SectionPageResources,
		    sprBreakToolResources.btrBreakKindMenuTexts[DOCibkPAGE]),
	    "On New Page" ),
    APP_RESOURCE( "formatToolSectBreakKindEven",
	    offsetof(SectionPageResources,
		    sprBreakToolResources.btrBreakKindMenuTexts[DOCibkEVEN]),
	    "On Even Page" ),
    APP_RESOURCE( "formatToolSectBreakKindOdd",
	    offsetof(SectionPageResources,
		    sprBreakToolResources.btrBreakKindMenuTexts[DOCibkODD]),
	    "On Odd Page" ),

    /**/
    APP_RESOURCE( "formatToolSectDirection",
	offsetof(SectionPageResources,sprDirection),
	"Direction" ),
    APP_RESOURCE( "formatToolSectLeftToRight",
	offsetof(SectionPageResources, sprDirectionMenuTexts[0]),
	"Left to Right" ),
    APP_RESOURCE( "formatToolSectRightToLeft",
	offsetof(SectionPageResources, sprDirectionMenuTexts[1]),
	"Right to Left" ),

    /**/
    /**/
    APP_RESOURCE( "formatToolSectColumns",
	    offsetof(SectionPageResources,sprColumns),
	    "Columns" ),
    APP_RESOURCE( "formatToolSectColumnCount",
	    offsetof(SectionPageResources,sprColumnCount),
	    "Columns" ),
    APP_RESOURCE( "formatToolSectColumn",
	    offsetof(SectionPageResources,sprColumn),
	    "Column" ),
    APP_RESOURCE( "formatToolSectColumnWidth",
	    offsetof(SectionPageResources,sprColumnWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolSectColumnSpacing",
	    offsetof(SectionPageResources,sprColumnSpacing),
	    "Spacing" ),
    APP_RESOURCE( "formatToolSectLineBetweenColumns",
	    offsetof(SectionPageResources,sprLineBetweenColumns),
	    "Line between" ),
    APP_RESOURCE( "formatToolSectColumnsEqualWidth",
	    offsetof(SectionPageResources,sprColumnsEqualWidth),
	    "Equal Width" ),
    };

static void tedFormatToolGetSectResourceTable(
					struct EditApplication *	ea,
					SectionPageResources *		spr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)spr,
				TED_TedSectionToolResourceTable,
				sizeof(TED_TedSectionToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedSectionSubjectResourceTable,
				sizeof(TED_TedSectionSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }


int tedSectionToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetSectResourceTable( ea,
				&TED_SectionPageResources,
				&TED_SectionSubjectResources );

    ist->istResources= &TED_SectionSubjectResources;

    ist->istBuildTool= tedFormatBuildSectionPage;
    ist->istFillChoosers= tedFormatFillSectionChoosers;
    ist->istFinishPage= tedFormatFinishSectionPage;
    ftst->ftstRefreshPage= tedRefreshSectionTool;
    ist->istFreeTool= tedFormatFreeSectionTool;
    /*fts->ftsToolResetCache*/

    ist->istMoveToNext= tedFormatNextSect;
    ist->istMoveToPrev= tedFormatPrevSect;

    ist->istSelectInstance= tedFormatSelectSect;
    ist->istDeleteInstance= tedFormatDeleteSect;

    ist->istInsertInstance= tedFormatInsertSect;
    ist->istAppendInstance= tedFormatAppendSect;

    ist->istRevertChanges= tedFormatRevertSectPushed;
    ist->istApplyChanges= tedFormatChangeSectPushed;

    return 0;
    }
