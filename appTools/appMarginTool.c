/************************************************************************/
/*									*/
/*  A Page Layout tool.							*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<limits.h>

#   include	<utilPropMask.h>
#   include	<guiWidgets.h>
#   include	<guiToolUtil.h>
#   include	"appMarginTool.h"
#   include	<guiTextUtil.h>
#   include	<geoDocumentPageSetup.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Turn on/off								*/
/*									*/
/************************************************************************/

void appEnableMarginTool(	AppMarginTool *		amt,
				int			enabled )
    {
    guiEnableWidget( amt->amtMarginFrame, enabled );

    guiEnableText( amt->amtLeftMarginText, enabled );
    guiEnableText( amt->amtTopMarginText, enabled );
    guiEnableText( amt->amtRightMarginText, enabled );
    guiEnableText( amt->amtBottomMarginText, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  Show the size of the paper in the Text.				*/
/*									*/
/************************************************************************/

void appMarginToolShowMargins(	AppMarginTool *			amt,
				int				unitInt,
				const DocumentGeometry *	dg )
    {
    guiLengthToText( amt->amtLeftMarginText,
					dg->dgMargins.roLeftOffset, unitInt );

    guiLengthToText( amt->amtTopMarginText,
					dg->dgMargins.roTopOffset, unitInt );

    guiLengthToText( amt->amtRightMarginText,
					dg->dgMargins.roRightOffset, unitInt );

    guiLengthToText( amt->amtBottomMarginText,
					dg->dgMargins.roBottomOffset, unitInt );

    return;
    }

/************************************************************************/
/*									*/
/*  Get one/all margins from the text widgets.				*/
/*									*/
/************************************************************************/

int appMarginToolGetMargins(	PropertyMask *		pUpdMask,
				const PropertyMask *	chgMask,
				int			unitType,
				AppMarginTool *		amt,
				DocumentGeometry *	dg )
    {
    int			changed;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    PropertyMask	updMask;

    utilPropMaskClear( &updMask );

    if  ( PROPmaskISSET( chgMask, DGpropLEFT_MARGIN ) )
	{
	if  ( guiLengthFromText( amt->amtLeftMarginText,
			&(dg->dgMargins.roLeftOffset), &changed, unitType,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropLEFT_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropTOP_MARGIN ) )
	{
	if  ( guiLengthFromText( amt->amtTopMarginText,
			&(dg->dgMargins.roTopOffset), &changed, unitType,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropTOP_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropRIGHT_MARGIN ) )
	{
	if  ( guiLengthFromText( amt->amtRightMarginText,
			&(dg->dgMargins.roRightOffset), &changed, unitType,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropRIGHT_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropBOTTOM_MARGIN ) )
	{
	if  ( guiLengthFromText( amt->amtBottomMarginText,
			&(dg->dgMargins.roBottomOffset), &changed, unitType,
			minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropBOTTOM_MARGIN );	}
	}

    *pUpdMask= updMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Check margins.							*/
/*									*/
/************************************************************************/

int appMarginToolCheckMargins(	const DocumentGeometry * dg )
    {
    if  ( geoContentWide( dg ) < dg->dgPageWideTwips/ 4 )
	{ return -1;	}

    if  ( geoContentHigh( dg ) < dg->dgPageHighTwips/ 4 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the frame for the margins.					*/
/*									*/
/************************************************************************/

void appMakeMarginToolWidgets(	APP_WIDGET			parent,
				const MarginToolResources *	mtr,
				AppMarginTool *			amt,
				APP_TXACTIVATE_CALLBACK_T	gotValue,
				void *				through )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    const int	textWidth= 6;
    const int	textEnabled= 1;
    const int	colspan= 1;

    guiMakeRowFrameInColumn( &(amt->amtMarginFrame), &(amt->amtInsideWidget),
					    parent, 2, mtr->mtrTitleText );

    guiMakeColumnInRow( &(amt->amtLeftColumn), amt->amtInsideWidget,
								0, colspan );
    guiMakeColumnInRow( &(amt->amtRightColumn), amt->amtInsideWidget,
								1, colspan );

    /**********************/

    guiToolMakeLabelAndTextRow( &row, &label, &(amt->amtLeftMarginText),
				amt->amtLeftColumn, mtr->mtrLeftMarginText,
				textWidth, textEnabled );
    guiToolMakeLabelAndTextRow( &row, &label, &(amt->amtRightMarginText),
				amt->amtLeftColumn, mtr->mtrRightMarginText,
				textWidth, textEnabled );

    guiToolMakeLabelAndTextRow( &row, &label, &(amt->amtTopMarginText),
				amt->amtRightColumn, mtr->mtrTopMarginText,
				textWidth, textEnabled );
    guiToolMakeLabelAndTextRow( &row, &label, &(amt->amtBottomMarginText),
				amt->amtRightColumn, mtr->mtrBottomMarginText,
				textWidth, textEnabled );

    if  ( gotValue )
	{
	guiSetGotValueCallbackForText( amt->amtLeftMarginText,
							gotValue, through );

	guiSetGotValueCallbackForText( amt->amtTopMarginText,
							gotValue, through );

	guiSetGotValueCallbackForText( amt->amtRightMarginText,
							gotValue, through );

	guiSetGotValueCallbackForText( amt->amtBottomMarginText,
							gotValue, through );
	}

    return;
    }
