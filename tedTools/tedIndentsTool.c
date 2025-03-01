/************************************************************************/
/*									*/
/*  The Left/Right/First indent part of the paragraph layout and list	*/
/*  tool.								*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	<geoUnit.h>

#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<guiWidgets.h>
#   include	<geoDocumentPageSetup.h>
#   include	"tedIndentsTool.h"

#   include	<appDebugon.h>

void tedInitIndentsTool(	IndentsTool *	it )
    {
    geoInitDocumentGeometry( &(it->itDocumentGeometry) );

    it->itLToRFirstIndentRow= (APP_WIDGET)0;
    it->itLToRFirstIndentLabel= (APP_WIDGET)0;
    it->itLToRFirstIndentText= (APP_WIDGET)0;

    it->itRToLFirstIndentRow= (APP_WIDGET)0;
    it->itRToLFirstIndentLabel= (APP_WIDGET)0;
    it->itRToLFirstIndentText= (APP_WIDGET)0;

    it->itLeftIndentRow= (APP_WIDGET)0;
    it->itLeftIndentLabel= (APP_WIDGET)0;
    it->itLeftIndentText= (APP_WIDGET)0;

    it->itRightIndentRow= (APP_WIDGET)0;
    it->itRightIndentLabel= (APP_WIDGET)0;
    it->itRightIndentText= (APP_WIDGET)0;
    }

void tedRefresIndentsTool(		IndentsTool *		it,
					int			inList,
					int			canChange,
					int			rToL,
					int			firstIndent,
					int			leftIndent,
					int			rightIndent )
    {
    if  ( inList )
	{
	if  ( rToL )
	    {
	    guiEnableText( it->itRToLFirstIndentText, canChange );
	    }
	else{
	    guiEnableText( it->itLToRFirstIndentText, canChange );
	    }

	guiEnableText( it->itLeftIndentText, canChange );
	guiEnableText( it->itRightIndentText, canChange );
	}
    else{
	if  ( rToL )
	    {
	    guiSetWidgetVisible( it->itLToRFirstIndentRow, 0 );

	    guiLengthToText( it->itRToLFirstIndentText,
				rightIndent+ firstIndent, UNITtyPOINTS );

	    guiSetWidgetVisible( it->itRToLFirstIndentRow, 1 );
	    guiEnableWidget( it->itRToLFirstIndentRow, canChange );
	    }
	else{
	    guiSetWidgetVisible( it->itRToLFirstIndentRow, 0 );

	    guiLengthToText( it->itLToRFirstIndentText,
				leftIndent+ firstIndent, UNITtyPOINTS );

	    guiSetWidgetVisible( it->itLToRFirstIndentRow, 1 );
	    guiEnableWidget( it->itLToRFirstIndentRow, canChange );
	    }
	}

    guiEnableWidget( it->itLeftIndentRow, canChange );
    guiLengthToText( it->itLeftIndentText, leftIndent, UNITtyPOINTS );

    guiEnableWidget( it->itRightIndentRow, canChange );
    guiLengthToText( it->itRightIndentText, rightIndent, UNITtyPOINTS );

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

void tedRToLFirstIndentChanged(	IndentsTool *		it,
				int			firstIndent,
				int			rightIndent )
    {
    int			changed;
    int			ripfi= firstIndent+ rightIndent;

    tedFormatValidateDimension( &ripfi, &changed,
				    it->itRToLFirstIndentText, ripfi );

    return;
    }

void tedLToRFirstIndentChanged(	IndentsTool *		it,
				int			firstIndent,
				int			leftIndent )
    {
    int			changed;
    int			lipfi= firstIndent+ leftIndent;

    tedFormatValidateDimension( &lipfi, &changed,
				    it->itLToRFirstIndentText, lipfi );

    return;
    }

void tedLeftIndentChanged(	IndentsTool *		it,
				int			leftIndent )
    {
    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
					it->itLeftIndentText, leftIndent );

    return;
    }

void tedRightIndentChanged(	IndentsTool *		it,
				int			rightIndent )
    {
    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
					it->itRightIndentText, rightIndent );

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve values from controls.					*/
/*									*/
/************************************************************************/

int tedIndentsToolGetChosen(	IndentsTool *		it,
				int			rToL,
				short int *		pFirstIndent,
				short int *		pLeftIndent,
				short int *		pRightIndent )
    {
    const DocumentGeometry *	dg= &(it->itDocumentGeometry);
    const int			adaptToMin= 1;
    const int			adaptToMax= 1;

    int				value;
    int				changed;

    if  ( rToL )
	{
	const int	minValue= -dg->dgMargins.roRightOffset;
	const int	maxValue= dg->dgPageWideTwips- dg->dgMargins.roLeftOffset;

	int		ripfi;
	int		ri;

	value= *pLeftIndent;
	if  ( guiLengthFromText( it->itLeftIndentText,
					&value, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	*pLeftIndent= value;

	ripfi= *pRightIndent+ *pFirstIndent;

	if  ( guiLengthFromText( it->itRToLFirstIndentText,
					&ripfi, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	ri= *pRightIndent;
	if  ( guiLengthFromText( it->itRightIndentText,
					&ri, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	*pRightIndent= ri;
	*pFirstIndent= ripfi- ri;
	}
    else{
	const int	minValue= -dg->dgMargins.roLeftOffset;
	const int	maxValue= dg->dgPageWideTwips- dg->dgMargins.roRightOffset;
	int		lipfi;
	int		li;

	lipfi= *pLeftIndent+ *pFirstIndent;

	if  ( guiLengthFromText( it->itLToRFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	li= *pLeftIndent;
	if  ( guiLengthFromText( it->itLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	*pLeftIndent= li;
	*pFirstIndent= lipfi- li;

	value= *pRightIndent;
	if  ( guiLengthFromText( it->itRightIndentText,
					&value, &changed, UNITtyPOINTS,
					minValue, adaptToMin,
					maxValue, adaptToMax ) )
	    { return -1;	}

	*pRightIndent= value;
	}

    return 0;
    }

int tedFillIndentsTool(		IndentsTool *			it,
				APP_WIDGET			pageWidget,
				int				textColumns,
				const IndentsToolResources *	itr,
				APP_TXACTIVATE_CALLBACK_T	ltrFirstChanged,
				APP_TXACTIVATE_CALLBACK_T	leftChanged,
				APP_TXACTIVATE_CALLBACK_T	rtlFirstChanged,
				APP_TXACTIVATE_CALLBACK_T	rightChanged,
				void *				through )
    {
    /**/
    guiToolMakeLabelAndTextRow( &(it->itLToRFirstIndentRow),
	    &(it->itLToRFirstIndentLabel), &(it->itLToRFirstIndentText),
	    pageWidget, itr->itrFirstIndent, textColumns, 1 );

    if  ( ltrFirstChanged )
	{
	guiSetGotValueCallbackForText( it->itLToRFirstIndentText,
					    ltrFirstChanged, through );
	}

    /**/
    guiToolMakeLabelAndTextRow( &(it->itLeftIndentRow),
		    &(it->itLeftIndentLabel), &(it->itLeftIndentText),
		    pageWidget, itr->itrLeftIndent, textColumns, 1 );

    if  ( leftChanged )
	{
	guiSetGotValueCallbackForText( it->itLeftIndentText,
					    leftChanged, through );
	}

    /**/
    guiToolMakeLabelAndTextRow( &(it->itRToLFirstIndentRow),
	    &(it->itRToLFirstIndentLabel), &(it->itRToLFirstIndentText),
	    pageWidget, itr->itrFirstIndent, textColumns, 1 );

    if  ( rtlFirstChanged )
	{
	guiSetGotValueCallbackForText( it->itRToLFirstIndentText,
					    rtlFirstChanged, through );
	}

    /**/
    guiToolMakeLabelAndTextRow( &(it->itRightIndentRow),
		    &(it->itRightIndentLabel), &(it->itRightIndentText),
		    pageWidget, itr->itrRightIndent, textColumns, 1 );

    if  ( rightChanged )
	{
	guiSetGotValueCallbackForText( it->itRightIndentText,
					    rightChanged, through );
	}

    return 0;
    }

void tedIndentsToolSetDocumentGeometry(
				IndentsTool *			it,
				const struct DocumentGeometry *	dg )
    {
    it->itDocumentGeometry= *dg;
    }

