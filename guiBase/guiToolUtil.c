/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	<stddef.h>

#   include	"guiWidgets.h"
#   include	"guiToolUtil.h"

#   include	<appDebugon.h>

#   if ! USE_HEADLESS

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

void guiToolMake2BottonRow(	APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				APP_WIDGET *		pLeftButton,
				APP_WIDGET *		pRightButton,
				const char *		leftLabel,
				const char *		rightLabel,
				APP_BUTTON_CALLBACK_T	leftCallback,
				APP_BUTTON_CALLBACK_T	rightCallback,
				void *			through )
    {
    const int	heightResizable= 0;
    const int	showAsDefault= 0;
    const int	colspan= 1;

    APP_WIDGET	row= guiMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	leftButton;
    APP_WIDGET	rightButton;

    guiMakeButtonInRow( &leftButton, row, leftLabel, leftCallback, through,
						0, colspan, showAsDefault );
    guiMakeButtonInRow( &rightButton, row, rightLabel,rightCallback, through,
						1, colspan, showAsDefault );

    *pRow= row;
    *pLeftButton= leftButton;
    *pRightButton= rightButton;

    return;
    }

void guiToolMake3ButtonRow( APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				APP_WIDGET *		pLeftButton,
				APP_WIDGET *		pMiddleButton,
				APP_WIDGET *		pRightButton,
				const char *		leftLabel,
				const char *		middleLabel,
				const char *		rightLabel,
				APP_BUTTON_CALLBACK_T	leftCallback,
				APP_BUTTON_CALLBACK_T	middleCallback,
				APP_BUTTON_CALLBACK_T	rightCallback,
				void *			through )
    {
    const int	heightResizable= 0;
    const int	showAsDefault= 0;
    const int	colspan= 1;

    APP_WIDGET	row= guiMakeRowInColumn( parent, 3, heightResizable );

    APP_WIDGET	leftButton;
    APP_WIDGET	middleButton;
    APP_WIDGET	rightButton;

    guiMakeButtonInRow( &leftButton, row, leftLabel, leftCallback, through,
						0, colspan, showAsDefault );
    guiMakeButtonInRow( &middleButton, row, middleLabel,middleCallback, through,
						1, colspan, showAsDefault );
    guiMakeButtonInRow( &rightButton, row, rightLabel,rightCallback, through,
						2, colspan, showAsDefault );

    *pRow= row;
    *pLeftButton= leftButton;
    *pMiddleButton= middleButton;
    *pRightButton= rightButton;

    return;
    }

void guiToolMake2ToggleRow(		APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			leftThrough,
					void *			rightThrough )
    {
    const int	heightResizable= 0;
    APP_WIDGET	row= guiMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	leftToggle;
    APP_WIDGET	rightToggle;

    const int	leftColumn= 0;
    const int	rightColumn= 1;

    leftToggle= guiMakeToggleInRow( row, leftText, leftCallback,
						leftThrough, leftColumn, 1 );
    rightToggle= guiMakeToggleInRow( row, rightText, rightCallback,
						rightThrough, rightColumn, 1 );
    *pRow= row;
    *pLeftToggle= leftToggle;
    *pRightToggle= rightToggle;

    return;
    }

void guiToolMake3ToggleRow(		APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pMiddleToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		middleText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	middleCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			through )
    {
    const int	heightResizable= 0;
    APP_WIDGET	row= guiMakeRowInColumn( parent, 3, heightResizable );

    APP_WIDGET	leftToggle;
    APP_WIDGET	middleToggle;
    APP_WIDGET	rightToggle;

    const int	leftColumn= 0;
    const int	middleColumn= 1;
    const int	rightColumn= 2;

    leftToggle= guiMakeToggleInRow( row, leftText, leftCallback,
						through, leftColumn, 1 );
    middleToggle= guiMakeToggleInRow( row, middleText, middleCallback,
						through, middleColumn, 1 );
    rightToggle= guiMakeToggleInRow( row, rightText, rightCallback,
						through, rightColumn, 1 );
    *pRow= row;
    *pLeftToggle= leftToggle;
    *pMiddleToggle= middleToggle;
    *pRightToggle= rightToggle;

    return;
    }

void guiToolMakeToggleAndLabelRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pToggle,
					APP_WIDGET *		pLabel,
					const char *		toggleText,
					const char *		labelText,
					APP_TOGGLE_CALLBACK_T	toggleCallback,
					void *			through )
    {
    const int	heightResizable= 0;
    APP_WIDGET	row= guiMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	toggle;
    APP_WIDGET	label;

    const int	toggleColumn= 0;
    const int	toggleColspan= 1;
    const int	labelColumn= toggleColumn+ toggleColspan;
    const int	labelColspan= 1;

    toggle= guiMakeToggleInRow( row, toggleText, toggleCallback,
					through, toggleColumn, toggleColspan );
    guiMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );

    *pRow= row;
    *pToggle= toggle;
    *pLabel= label;

    return;
    }

void guiToolMakeLabelAndToggleRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLabel,
					APP_WIDGET *		pToggle,
					const char *		labelText,
					const char *		toggleText,
					APP_TOGGLE_CALLBACK_T	toggleCallback,
					void *			through )
    {
    const int	heightResizable= 0;
    APP_WIDGET	row= guiMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	toggle;
    APP_WIDGET	label;

    const int	labelColumn= 0;
    const int	labelColspan= 1;
    const int	toggleColumn= labelColumn+ labelColspan;
    const int	toggleColspan= 1;

    guiMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    toggle= guiMakeToggleInRow( row, toggleText, toggleCallback,
					through, toggleColumn, toggleColspan );

    *pRow= row;
    *pLabel= label;
    *pToggle= toggle;

    return;
    }

void guiToolMakeLabelAndMenuRow(	APP_WIDGET *		pRow,
					AppOptionmenu *		aom,
					APP_WIDGET *		pLabel,
					APP_WIDGET		parent,
					const char *		labelText,
					OptionmenuCallback	callBack,
					void *			target )
    {
    APP_WIDGET	row;
    APP_WIDGET	label;

    const int	labelColumn= 0;
    const int	labelColspan= 1;

    const int	menuColumn= labelColumn+ labelColspan;
    const int	menuColspan= 1;
    const int	heightResizable= 0;

    row= guiMakeRowInColumn( parent, 2, heightResizable );

    /**************/
    guiMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );

    /**************/
    guiMakeOptionmenuInRow( aom, row, menuColumn, menuColspan,
							callBack, target );

    /**************/

    *pRow= row;
    *pLabel= label;

    return;
    }

/************************************************************************/
/*									*/
/*  Fill an inspector menu.						*/
/*									*/
/************************************************************************/

void appFillInspectorMenu(	int			count,
				int			current,
				const char * const *	texts,
				AppOptionmenu *		aom )
    {
    int			i;

    guiEmptyOptionmenu( aom );

    for ( i= 0; i < count; i++ )
	{ guiAddItemToOptionmenu( aom, texts[i] );	}

    guiSetOptionmenu( aom, current );

    guiOptionmenuRefreshWidth( aom );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row consisting of a label and a text widget.			*/
/*									*/
/************************************************************************/

void guiToolMakeLabelAndTextRow( APP_WIDGET *		pRow,
				APP_WIDGET *		pLabel,
				APP_WIDGET *		pText,
				APP_WIDGET		column,
				const char *		labelText,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		label;
    APP_WIDGET		text;
    APP_WIDGET		row;

    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		textColumn= 1;
    const int		textColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= guiMakeRowInColumn( column, columnCount, heightResizable );

    guiMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    guiMakeTextInRow( &text, row, textColumn, textColspan,
						textColumns, textEnabled );

    if  ( pRow )
	{ *pRow= row;		}
    if  ( pLabel )
	{ *pLabel= label;	}
    if  ( pText )
	{ *pText= text;		}

    return;
    }

void guiToolMakeToggleAndTextRow(	APP_WIDGET *		pRow,
					APP_WIDGET *		pToggle,
					APP_WIDGET *		pText,
					APP_WIDGET		column,
					const char *		labelText,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			textColumns,
					int			textEnabled )
    {
    APP_WIDGET		toggle;
    APP_WIDGET		text;
    APP_WIDGET		row;

    const int		textColumn= 1;
    const int		textColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= guiMakeRowInColumn( column, columnCount, heightResizable );

    toggle= guiMakeToggleInRow( row, labelText, callback, through, 0, 1 );
    guiMakeTextInRow( &text, row, textColumn, textColspan,
						    textColumns, textEnabled );

    *pRow= row; *pToggle= toggle; *pText= text; return;
    }

#   endif
