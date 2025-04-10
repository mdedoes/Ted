#   include	"guiBaseConfig.h"

#   include	"guiWidgets.h"
#   include	"guiDrawnPulldown.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   if USE_MOTIF

#   include	<Xm/DrawingA.h>

/************************************************************************/
/*									*/
/*  Display the popup that belongs to this drawn popdown.		*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( guiDrawnPulldownPulldown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;

    Position		inplaceX= 0;
    Position		inplaceY= 0;
    Position		screenX;
    Position		screenY;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    Arg			al[20];
    int			ac= 0;

    if  ( mouseEvent->xbutton.button != Button1 )
	{ return;	}

    /******/

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return;	}

    XtTranslateCoords( adp->adpInplaceDrawing,
				mouseX, mouseY,
				&screenX, &screenY );

    adp->adpFirstDownTime= mouseEvent->xbutton.time;
    adp->adpMouseX= screenX;
    adp->adpMouseY= screenY;

    /******/

    XtTranslateCoords( adp->adpInplaceDrawing,
				inplaceX, inplaceY,
				&screenX, &screenY );

    ac= 0;
    XtSetArg( al[ac], XmNx,		screenX ); ac++;
    XtSetArg( al[ac], XmNy,		screenY ); ac++;

    XtSetValues( adp->adpPulldownShell, al, ac );

    if  ( adp->adpPulldown )
	{ (*adp->adpPulldown)( adp->adpThrough );	}

    XtPopupSpringLoaded( adp->adpPulldownShell );

    guiMotifSetFocusToWindow( adp->adpPulldownShell );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse events on the pulldown part.				*/
/*									*/
/*  1)  Get the mouse coordinates and translate to pulldown widow	*/
/*	coordinates.							*/
/*  2)  Not the click that popped up the pulldown? (sic)		*/
/*  3)  Not a mouse release at the location where we just clicked to	*/
/*	pop up the pulldown.						*/
/*  4)  Not outside the window?						*/
/*  5)  Hide pulldown window.						*/
/*  6)  Pass event to application handler.				*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( guiDrawnPulldownMouseUpDown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;

    Position		screenX;
    Position		screenY;

    Dimension		wide;
    Dimension		high;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    /*  1  */
    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ XtPopdown( w ); return;	}

    XtTranslateCoords( adp->adpInplaceDrawing,
				mouseX, mouseY,
				&screenX, &screenY );

    /*  2  */
    if  ( mouseEvent->xbutton.time == adp->adpFirstDownTime )
	{ return;	}

    /*  3  */
    if  ( upDown < 0			&&
	  adp->adpMouseX == screenX	&&
	  adp->adpMouseY == screenY	)
	{ return;	}

    /*  4  */
    XtVaGetValues( w,
			    XmNwidth,		&wide,
			    XmNheight,		&high,
			    NULL );

    /*  5  */
    XtPopdown( w );

    /*  4  */
    if  ( mouseX < 0		||
	  mouseY < 0		||
	  mouseX >= wide	||
	  mouseY >= high	)
	{ return;	}

    /*  6  */
    if  ( adp->adpClickHandler )
	{
	(*adp->adpClickHandler)( w, adp->adpThrough, mouseEvent, pRefused );
	}

    return;
    }

static APP_EVENT_HANDLER_H( guiDrawnPulldownFocusChange, w, voidadp, focusEvent )
    {
    XFocusChangeEvent *	xfocus= &(focusEvent->xfocus);

    if  ( xfocus->type == FocusOut )
	{ XtPopdown( w );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the drawn pulldown. Attach the pulldown to the in place	*/
/*  widget and do administration.					*/
/*									*/
/************************************************************************/

static void guiFinishDrawnPulldown(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				void *				through )
    {
    Arg				al[20];
    int				ac= 0;

    adp->adpClickHandler= clickedPulldown;
    adp->adpPulldown= pullDown;
    adp->adpThrough= through;

    /******/

    if  ( redrawInplace )
	{
	XtAddEventHandler( adp->adpInplaceDrawing, ExposureMask,
					    False, redrawInplace, through );
	}

    XtAddCallback( adp->adpInplaceDrawing, XmNresizeCallback,
				    guiMotifSendExposeForResize, (void *)0 );

    /******/

    ac= 0;
    XtSetArg( al[ac], XmNmwmDecorations, 0 ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions, 0 ); ac++;
    XtSetArg( al[ac], XmNoverrideRedirect, True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize, True ); ac++;

    adp->adpPulldownShell= XtCreatePopupShell( WIDGET_NAME,
					    transientShellWidgetClass,
					    adp->adpInplaceDrawing, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNwidth,		100 ); ac++;
    XtSetArg( al[ac], XmNheight,	100 ); ac++;

    adp->adpPulldownDrawing= XmCreateDrawingArea(
				adp->adpPulldownShell, WIDGET_NAME, al, ac );

    if  ( redrawPulldown )
	{
	XtAddEventHandler( adp->adpPulldownDrawing, ExposureMask,
					    False, redrawPulldown, through );
	}

    XtAddCallback( adp->adpPulldownDrawing, XmNresizeCallback,
				    guiMotifSendExposeForResize, (void *)0 );
    /******/

    XtAddEventHandler( adp->adpPulldownShell,
			ButtonReleaseMask|ButtonPressMask,
			False, guiDrawnPulldownMouseUpDown, (void *)adp );

    XtAddEventHandler( adp->adpPulldownShell,
			FocusChangeMask,
			False, guiDrawnPulldownFocusChange, (void *)adp );

    XtAddEventHandler( adp->adpInplaceDrawing, ButtonPressMask,
				False, guiDrawnPulldownPulldown, (void *)adp );

    /******/

    XtManageChild( adp->adpPulldownDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a pulldown where the values are drawings.. Not Text.		*/
/*									*/
/************************************************************************/

void guiMakeDrawnPulldownInColumn(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			column,
				void *				through )
    {
    Arg				al[20];
    int				ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;

    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,	0 ); ac++;

    adp->adpInplaceDrawing= XmCreateDrawingArea( column, WIDGET_NAME, al, ac );

    XtManageChild( adp->adpInplaceDrawing );

    guiMotifTurnOfSashTraversal( column );

    guiFinishDrawnPulldown( adp, redrawInplace, redrawPulldown,
					clickedPulldown, pullDown, through );

    return;
    }

void guiMakeDrawnPulldownInRow(	AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickedPulldown,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			row,
				int				column,
				int				colspan,
				void *				through )
    {
    Arg				al[20];
    int				ac= 0;

    ac= 0;
    ac= guiMotifSetInRowConstraints( al, ac, column, colspan );

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNentryBorder,		0 ); ac++;

    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;

    adp->adpInplaceDrawing= XmCreateDrawingArea( row, WIDGET_NAME, al, ac );

    XtManageChild( adp->adpInplaceDrawing );

    guiFinishDrawnPulldown( adp, redrawInplace, redrawPulldown,
					clickedPulldown, pullDown, through );

    return;
    }

void guiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
				int			enabled )
    {
    if  ( adp->adpEnabled != enabled )
	{
	adp->adpEnabled= enabled;
	XtSetSensitive( adp->adpInplaceDrawing, enabled != 0 );

	guiExposeDrawnPulldownInplace( adp );
	}
    }

void guiSetDrawnPulldownHeight(	AppDrawnPulldown *	adp,
					int			height )
    {
    XtVaSetValues( adp->adpPulldownShell,
			    XmNheight,		height,
			    NULL );
    return;
    }

void guiSetDrawnPulldownStrips(	AppDrawnPulldown *	adp,
					int			strips )
    {
    Dimension		inplaceHeight;
    Dimension		inplaceWidth;

    XtVaGetValues( adp->adpInplaceDrawing,
			    XmNwidth,		&inplaceWidth,
			    XmNheight,		&inplaceHeight,
			    NULL );

    XtVaSetValues( adp->adpPulldownShell,
			    XmNwidth,		inplaceWidth,
			    XmNheight,		strips* inplaceHeight,
			    NULL );
    return;
    }

int guiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent )
    {
    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    Dimension		inplaceHeight;
    Dimension		inplaceWidth;

    XtVaGetValues( adp->adpInplaceDrawing,
			    XmNheight,		&inplaceHeight,
			    XmNwidth,		&inplaceWidth,
			    NULL );

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return -1;	}

    *pStrip= mouseY/ inplaceHeight;
    return 0;
    }

#   endif
