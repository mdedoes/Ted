/************************************************************************/
/*									*/
/*  Manage a zoom menu.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"appGuiApplication.h"
#   include	"appZoomMenu.h"
#   include	"appEditDocument.h"
#   include	<guiMenuItem.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Normalize a zoom factor.						*/
/*									*/
/************************************************************************/

static double appZoomFactor(	int	logs2mag )
    {
    if  ( logs2mag == 0 )
	{ return 1.0;	}

    if  ( logs2mag > 0 )
	{ return	pow( sqrt( 2.0 ),  (double)logs2mag );	}
    else{ return 1.0/	pow( sqrt( 2.0 ), -(double)logs2mag );	}
    }

/************************************************************************/
/*									*/
/*  Adapt the zoom menu to the current factor.				*/
/*									*/
/************************************************************************/

void appUpdateZoomMenu(	APP_WIDGET	menu,
			APP_WIDGET *	options,
			int		optionCount,
			int		logs2mag )
    {
    int				i;

    for ( i= 0; i < optionCount; i++ )
	{
	char		scratch[40];

	sprintf( scratch, "%6.0f %%",
		    100.0 * appZoomFactor( logs2mag+ i - optionCount/ 2 ) );

	guiSetToggleItemLabel( options[i], scratch );

	guiSetToggleItemState( options[i], i == optionCount/ 2 );
	}
    }

/************************************************************************/
/*									*/
/*  The user has chosen another magnification:				*/
/*									*/
/************************************************************************/

void appZoomChangeFactor(	APP_WIDGET		option,
				APP_WIDGET		menu,
				APP_WIDGET *		options,
				int			optionCount,
				int *			pLogSqrt2Magnification,
				double *		pMagnification,
				DocumentRectangle *	drSelected )

    {
    short			num= -1;
    double			magStep;

    for ( num= 0; num < optionCount; num++ )
	{
	if  ( options[num] == option )
	    { break;	}
	}

    if  ( /* num < 0 || */ num >= optionCount )
	{ LDEB(num); return;	}

    if  ( num == optionCount/2 )
	{ return;	}

    *pLogSqrt2Magnification += num- optionCount/2;
    magStep= appZoomFactor( *pLogSqrt2Magnification )/ *pMagnification;
    *pMagnification= appZoomFactor( *pLogSqrt2Magnification );

    appUpdateZoomMenu( menu, options, optionCount, *pLogSqrt2Magnification );

    if  ( drSelected )
	{
	if  ( drSelected->drX0 >= 0 )
	    { drSelected->drX0 *= magStep;	}
	if  ( drSelected->drX1 >= 0 )
	    { drSelected->drX1 *= magStep;	}
	if  ( drSelected->drY0 >= 0 )
	    { drSelected->drY0 *= magStep;	}
	if  ( drSelected->drY1 >= 0 )
	    { drSelected->drY1 *= magStep;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a zoom menu.							*/
/*									*/
/************************************************************************/

void appMakeZoomMenu(		EditDocument *		ed,
				APP_WIDGET *		pZoomMenu,
				APP_WIDGET *		options,
				APP_WIDGET		menubar,
				int			logSqrt2Magnification,
				AppMenu *		am )
    {
    int			i;
    APP_WIDGET		menu;

    APP_WIDGET		button;

    for ( i= 1; i < am->amItemCount; i++ )
	{
	am->amItems[i]= am->amItems[0];
	am->amItems[i].amiItemType= ITEMtyTOGGLE_OFF;
	}
    am->amItems[am->amItemCount/2].amiItemType= ITEMtyTOGGLE_ON;

    menu= appMakeMenu( &button, &(ed->edToplevel), ed->edApplication, menubar, am, (void *)ed );

    for ( i= 0; i < am->amItemCount; i++ )
	{
	options[i]= am->amItems[i].amiOptionWidget;
	}
    *pZoomMenu= menu;

    appUpdateZoomMenu( menu, options, am->amItemCount, logSqrt2Magnification );

    return;
    }

#   endif
