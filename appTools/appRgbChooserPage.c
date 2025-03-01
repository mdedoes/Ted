/************************************************************************/
/*									*/
/*  Page Layout page on an inspector:					*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	<appEditApplication.h>
#   include	"appRgbChooserPage.h"
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<guiWidgets.h>
#   include	"appInspector.h"
#   include	"appInspectorSubject.h"
#   include	<appGuiResource.h>

#   include	<appDebugon.h>

void appInitRgbChooser(	RgbChooser *			rcp )
    {
    memset( rcp, 0, sizeof(RgbChooser) );
    }

/************************************************************************/
/*									*/
/*  Refresh the section page.						*/
/*									*/
/************************************************************************/

static void appRgbChooserRefreshTexts(	RgbChooser *	rcp )
    {
    guiIntegerToText( rcp->rcpRedText, rcp->rcpRGBChosen.rgb8Red );
    guiIntegerToText( rcp->rcpGreenText, rcp->rcpRGBChosen.rgb8Green );
    guiIntegerToText( rcp->rcpBlueText, rcp->rcpRGBChosen.rgb8Blue );
    }

static void appRgbChooserRefreshCube(	RgbChooser *	rcp )
    {
    appRgbCubeSelectColor( &(rcp->rcpRgbCube), &(rcp->rcpRGBChosen) );

    if  ( rcp->rcpDrawing )
	{ guiExposeDrawingWidget( rcp->rcpDrawing );	}

    return;
    }

static void appRgbChooserRefreshPage(	RgbChooser *	rcp )
    {
    appRgbChooserRefreshTexts( rcp );
    appRgbChooserRefreshCube( rcp );

    return;
    }

void appRgbChooserRefresh(	RgbChooser *			rcp,
				int *				pEnabled,
				struct InspectorSubject *	is,
				const RGB8Color *		rgbSet )
    {
    if  ( ! rgbSet )
	{ return;	}

    rcp->rcpRGBChosen= *rgbSet;
    rcp->rcpRGBSet= *rgbSet;

    appRgbChooserRefreshPage( rcp );

    *pEnabled= 1;

    return;
    }

void appRgbChooserSetContext(	RgbChooser *	rcp,
				int			subjectPage,
				int			property )
    {
    rcp->rcpSubject= subjectPage;
    rcp->rcpProperty= property;

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appRgbChooserChangeRGB, w, voidrcp )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    int				subject= rcp->rcpSubject;
    int				property= rcp->rcpProperty;

    const int			adaptToMin= 0;
    const int			adaptToMax= 0;

    int				r= rcp->rcpRGBChosen.rgb8Red;
    int				g= rcp->rcpRGBChosen.rgb8Green;
    int				b= rcp->rcpRGBChosen.rgb8Blue;

    if  ( guiIntegerFromText( rcp->rcpRedText, &r,
					0, adaptToMin, 255, adaptToMax ) )
	{ return;	}
    if  ( guiIntegerFromText( rcp->rcpGreenText, &g,
					0, adaptToMin, 255, adaptToMax ) )
	{ return;	}
    if  ( guiIntegerFromText( rcp->rcpBlueText, &b,
					0, adaptToMin, 255, adaptToMax ) )
	{ return;	}

    rcp->rcpRGBChosen.rgb8Red= r;
    rcp->rcpRGBChosen.rgb8Green= g;
    rcp->rcpRGBChosen.rgb8Blue= b;

    appInspectorGotColor( rcp->rcpInspector, subject, property,
						    &(rcp->rcpRGBChosen) );

    return;
    }

static APP_BUTTON_CALLBACK_H( appRgbChooserCancelRGB, w, voidrcp )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    int				subject= rcp->rcpSubject;
    int				property= rcp->rcpProperty;

    appInspectorGotColor( rcp->rcpInspector, subject, property,
						    (const RGB8Color *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the RGB chooser was pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appRgbChooserRevertRGB, w, voidrcp )
    {
    RgbChooser *	rcp= (RgbChooser *)voidrcp;

    rcp->rcpRGBChosen= rcp->rcpRGBSet;

    appRgbChooserRefreshPage( rcp );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the color cube.						*/
/*									*/
/************************************************************************/

static APP_REDRAW_HANDLER_H( appRGBDrawChooser, w, voidrcp, exposeEvent )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    int				wide;
    int				high;
    DocumentRectangle		drClip;

    guiStartDrawing( &drClip, rcp->rcpDrawingSurface, rcp->rcpDrawing, exposeEvent );

    guiDrawGetSizeOfWidget( &wide, &high, w );

    appRedrawRgbCube( &(rcp->rcpRgbCube), wide, high, &drClip,
						    rcp->rcpDrawingSurface );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse clicks on the color cube.				*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appRGBMouseClick, w, voidrcp, downEvent )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    int				mouseX;
    int				mouseY;

    int				wide;
    int				high;

    int				canSplit= 0;

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return;	}

    guiDrawGetSizeOfWidget( &wide, &high, w );

    if  ( appRgbCubeFindColor( &(rcp->rcpRGBChosen), &canSplit,
					    &(rcp->rcpRgbCube),
					    mouseX, mouseY, wide, high ) )
	{
	appRotateRgbCube( &(rcp->rcpRgbCube), mouseX, mouseY, wide, high );

	if  ( rcp->rcpDrawing )
	    { guiExposeDrawingWidget( rcp->rcpDrawing );	}
	}
    else{
	RgbCube *	rc= &(rcp->rcpRgbCube);

	int		refresh= 1;

	appRgbChooserRefreshTexts( rcp );

	appRgbCubeSelectColor( rc, &(rcp->rcpRGBChosen) );

	if  ( canSplit )
	    {
	    appRgbCubeRefreshSplit( rc, &(rcp->rcpRGBChosen) );
	    refresh= 1;
	    }

	if  ( refresh && rcp->rcpDrawing )
	    { guiExposeDrawingWidget( rcp->rcpDrawing );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Validate an RGB color component.					*/
/*									*/
/************************************************************************/

static void appRgbChooserGotColor(	RgbChooser *	rcp,
					unsigned char *		pValue,
					APP_WIDGET		w )
    {
    int				value;
    const int			adaptToMin= 0;
    const int			adaptToMax= 0;

    if  ( guiIntegerFromText( w, &value,
					0, adaptToMin, 255, adaptToMax ) )
	{ return;	}

    *pValue= value;

    appRgbCubeSelectColor( &(rcp->rcpRgbCube), &(rcp->rcpRGBChosen) );

    appRgbChooserRefreshCube( rcp );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( appRgbChooserGotRed, w, voidrcp )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    appRgbChooserGotColor( rcp, &(rcp->rcpRGBChosen.rgb8Red),
							rcp->rcpRedText );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( appRgbChooserGotGreen, w, voidrcp )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    appRgbChooserGotColor( rcp, &(rcp->rcpRGBChosen.rgb8Green),
							rcp->rcpGreenText );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( appRgbChooserGotBlue, w, voidrcp )
    {
    RgbChooser *		rcp= (RgbChooser *)voidrcp;

    appRgbChooserGotColor( rcp, &(rcp->rcpRGBChosen.rgb8Blue),
							rcp->rcpBlueText );

    return;
    }

static void appRgbChooserColorToggled(	RgbChooser *	rcp,
					int			set,
					int			splitColor )
    {
    RgbCube *		rc= &(rcp->rcpRgbCube);
    int			oldSplitColor= rc->rcSplitColor;
    int			newSplitColor= oldSplitColor;

    if  ( ! set && oldSplitColor != splitColor )
	{ return;	}
    if  ( ! set && oldSplitColor == splitColor )
	{ newSplitColor= RCsplitNONE;	}
    if  ( set )
	{ newSplitColor= splitColor;	}

    appRgbCubeSetSplit( &(rcp->rcpRgbCube),
				    newSplitColor, &(rc->rcSplitValues) );

    guiSetToggleState( rcp->rcpRedToggle, newSplitColor == RCsplitRED );
    guiSetToggleState( rcp->rcpGreenToggle, newSplitColor == RCsplitGREEN );
    guiSetToggleState( rcp->rcpBlueToggle, newSplitColor == RCsplitBLUE );

    appRgbChooserRefreshCube( rcp );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appRgbChooserRedToggled, w, voidrcp, voidtbcs )
    {
    RgbChooser *	rcp= (RgbChooser *)voidrcp;

    int			set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    appRgbChooserColorToggled( rcp, set, RCsplitRED );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appRgbChooserGreenToggled, w, voidrcp, voidtbcs )
    {
    RgbChooser *	rcp= (RgbChooser *)voidrcp;

    int			set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    appRgbChooserColorToggled( rcp, set, RCsplitGREEN );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appRgbChooserBlueToggled, w, voidrcp, voidtbcs )
    {
    RgbChooser *	rcp= (RgbChooser *)voidrcp;

    int			set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    appRgbChooserColorToggled( rcp, set, RCsplitBLUE );

    return;
    }

/************************************************************************/
/*									*/
/*  Make an RGB color chooser page in an inspector tool.		*/
/*									*/
/************************************************************************/

void appRgbChooserPageFillPage(	RgbChooser *			rcp,
				const RgbChooserPageResources *	rcpr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET		row;

    const int		textColumns= 5;
    const int		textEnabled= 1;

    /**/

    /**/
    rcp->rcpPageResources= rcpr;
    rcp->rcpInspector= ai;
    rcp->rcpApplication= ea;
    rcp->rcpSubject= -1;
    rcp->rcpProperty= -1;

    /**/

    rcp->rcpDrawingSurface= (DrawingSurface)0;
    appInitRgbCube( &(rcp->rcpRgbCube) );

    /**************/

    {
    const int	wide= -1;
    const int	high= -1;
    const int	heightResizable= 1;

    guiMakeDrawingAreaInColumn( &(rcp->rcpDrawing), pageWidget,
			    wide, high, heightResizable,
			    appRGBDrawChooser, (void *)rcp );

    guiDrawSetButtonPressHandler( rcp->rcpDrawing, appRGBMouseClick,
							    (void *)rcp );
    }

    /**/
    guiToolMakeToggleAndTextRow( &row, &(rcp->rcpRedToggle), &(rcp->rcpRedText),
		    pageWidget, rcpr->rcprRedText,
		    appRgbChooserRedToggled, (void *)rcp,
		    textColumns, textEnabled );

    guiSetGotValueCallbackForText( rcp->rcpRedText,
				    appRgbChooserGotRed, (void *)rcp );

    /**/
    guiToolMakeToggleAndTextRow( &row,
		    &(rcp->rcpGreenToggle), &(rcp->rcpGreenText),
		    pageWidget, rcpr->rcprGreenText,
		    appRgbChooserGreenToggled, (void *)rcp,
		    textColumns, textEnabled );

    guiSetGotValueCallbackForText( rcp->rcpGreenText,
				    appRgbChooserGotGreen, (void *)rcp );

    /**/
    guiToolMakeToggleAndTextRow( &row,
		    &(rcp->rcpBlueToggle), &(rcp->rcpBlueText),
		    pageWidget, rcpr->rcprBlueText,
		    appRgbChooserBlueToggled, (void *)rcp,
		    textColumns, textEnabled );

    guiSetGotValueCallbackForText( rcp->rcpBlueText,
				    appRgbChooserGotBlue, (void *)rcp );

    /**/
    guiToolMake3ButtonRow( &(is->isApplyRow), pageWidget,
					&(is->isRevertButton),
					&(rcp->rcpCancelButton),
					&(is->isApplyButton),

					isr->isrRevert,
					rcpr->rcprCancelText,
					isr->isrApplyToSubject,

					appRgbChooserRevertRGB,
					appRgbChooserCancelRGB,
					appRgbChooserChangeRGB,
					(void *)rcp );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the RGB chooser page.					*/
/*									*/
/************************************************************************/

void appRgbChooserPageFinishPage(	RgbChooser *		rcp )
    {
    const int		avoidFontconfig= 0;

    if  ( ! rcp->rcpDrawing )
	{ XDEB(rcp->rcpDrawing); return;	}

    rcp->rcpDrawingSurface= guiDrawingSurfaceForNativeWidget(
					rcp->rcpDrawing, avoidFontconfig );
    if  ( ! rcp->rcpDrawingSurface )
	{ XDEB(rcp->rcpDrawingSurface);	}

    if  ( appPrepareRgbCube( &(rcp->rcpRgbCube), rcp->rcpDrawingSurface,
								6, 6, 6 ) )
	{ LDEB(6);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the RGB chooser page.						*/
/*									*/
/************************************************************************/

void appRgbChooserPageCleanPage(	RgbChooser *		rcp )
    {
    appCleanRgbCube( &(rcp->rcpRgbCube) );

    if  ( rcp->rcpDrawingSurface )
	{ drawFreeDrawingSurface( rcp->rcpDrawingSurface );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get page RGB chooser resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_RGBSubjectResourceTable[]=
    {
    APP_RESOURCE( "rgbChooserColor",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Choose Color" ),

    APP_RESOURCE( "rgbChooserChooseColor",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Choose Color" ),
    APP_RESOURCE( "rgbChooserRevertColor",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource APP_RGBToolResourceTable[]=
    {
    APP_RESOURCE( "rgbChooserCancel",
		offsetof(RgbChooserPageResources,rcprCancelText),
		"Cancel" ),

    APP_RESOURCE( "rgbChooserRedComponent",
		offsetof(RgbChooserPageResources,rcprRedText),
		"Red" ),
    APP_RESOURCE( "rgbChooserGreenComponent",
		offsetof(RgbChooserPageResources,rcprGreenText),
		"Green" ),
    APP_RESOURCE( "rgbChooserBlueComponent",
		offsetof(RgbChooserPageResources,rcprBlueText),
		"Blue" ),
    };

void appRgbChooserPageGetResourceTable(	EditApplication *		ea,
					RgbChooserPageResources *	rcpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources;
    static int	gotSubjectResources;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)rcpr,
				APP_RGBToolResourceTable,
				sizeof(APP_RGBToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_RGBSubjectResourceTable,
				sizeof(APP_RGBSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
