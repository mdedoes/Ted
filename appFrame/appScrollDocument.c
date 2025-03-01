/************************************************************************/
/*									*/
/*  Document window application independent scrolling functionality.	*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>

#   if ! USE_HEADLESS

#   include	"appEditApplication.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocument.h"
#   include	"appDocFront.h"
#   include	<guiKeys.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<geoUnits.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Callbacks for the scrollbars, and the size admin  of a document.	*/
/*									*/
/************************************************************************/

static void appDocScrollVertically(	EditDocument *		ed,
					int			scrolledY )
    {
    EditApplication *		ea= ed->edApplication;
    DrawingSurface		ds= ed->edDrawingSurface;

    int				ox;
    int				oy;
    int				high;
    int				wide;

    DocumentRectangle		drClip;
    DocumentRectangle		drScreen;

    drawNoClipping( ds );

    drClip= ed->edFullRect;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;

    if  ( scrolledY > 0 )
	{
	if  ( high > scrolledY )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= 0;
	    drSrc.drX1= wide- 1;
	    drSrc.drY0= scrolledY;
	    drSrc.drY1= high- 1;

	    drawMoveArea( ds, 0, 0, &drSrc );

	    drClip.drY0= ed->edVisibleRect.drY1- scrolledY;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	else{
	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	}
    else{
	if  ( high+ scrolledY > 0 )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= 0;
	    drSrc.drX1= wide- 1;
	    drSrc.drY0= 0;
	    drSrc.drY1= high+ scrolledY- 1;

	    drawMoveArea( ds, 0, -scrolledY, &drSrc );

	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY0- scrolledY;
	    }
	else{
	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	}

    ox= ed->edVisibleRect.drX0;
    oy= ed->edVisibleRect.drY0;

    drScreen= drClip;
    geoShiftRectangle( &drScreen, -ox, -oy );

#   if 1
    drawSetClipRect( ds, &drScreen );
    (*ea->eaDrawRectangle)( ed, &drClip );
    drawNoClipping( ds );
#   else
    guiExposeDrawingWidgetRectangle( dw->dwWidget, &drScreen );
#   endif

    if  ( ed->edLeftRuler && ea->eaScrollLeftRuler )
	{
	(*ea->eaScrollLeftRuler)
			( ed->edLeftRuler, ed->edLeftRulerWidget, scrolledY );
	}

    if  ( ed->edRightRuler && ea->eaScrollRightRuler )
	{
	(*ea->eaScrollRightRuler)
		    ( ed->edRightRuler, ed->edRightRulerWidget, scrolledY );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll horizontally.						*/
/*									*/
/************************************************************************/

static void appDocScrollHorizontally(	EditDocument *		ed,
					int			scrolledX )
    {
    EditApplication *		ea= ed->edApplication;
    DrawingSurface		ds= ed->edDrawingSurface;

    int				ox;
    int				oy;
    int				high;
    int				wide;

    DocumentRectangle		drClip;
    DocumentRectangle		drScreen;

    drawNoClipping( ds );

    drClip= ed->edFullRect;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;

    if  ( scrolledX > 0 )
	{
	if  ( wide > scrolledX )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= scrolledX;
	    drSrc.drX1= wide- 1;
	    drSrc.drY0= 0;
	    drSrc.drY1= high- 1;

	    drawMoveArea( ds, 0, 0, &drSrc );

	    drClip.drX0= ed->edVisibleRect.drX1- scrolledX- 1;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	else{
	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	}
    else{
	if  ( wide+ scrolledX > 0 )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= 0;
	    drSrc.drX1= wide+ scrolledX- 1;
	    drSrc.drY0= 0;
	    drSrc.drY1= high- 1;

	    drawMoveArea( ds, -scrolledX, 0, &drSrc );

	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX0- scrolledX;
	    }
	else{
	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	}

    ox= ed->edVisibleRect.drX0;
    oy= ed->edVisibleRect.drY0;

    drScreen= drClip;
    geoShiftRectangle( &drScreen, -ox, -oy );

#   if 1
    drawSetClipRect( ds, &drScreen );
    (*ea->eaDrawRectangle)( ed, &drClip );
    drawNoClipping( ds );
#   else
    guiExposeDrawingWidgetRectangle( dw->dwWidget, &drScreen );
#   endif

    if  ( ed->edTopRuler && ea->eaScrollTopRuler )
	{
	(*ea->eaScrollTopRuler)
		( ed->edTopRuler, ed->edTopRulerWidget, scrolledX );
	}

    if  ( ed->edBottomRuler && ea->eaScrollBottomRuler )
	{
	(*ea->eaScrollBottomRuler)
		( ed->edBottomRuler, ed->edBottomRulerWidget, scrolledX );
	}

    return;
    }

void appDocScrollToY(	EditDocument *		ed,
			int			y )
    {
    int		scrolledY= y- ed->edVisibleRect.drY0;

    if  ( scrolledY == 0 )
	{ return;	}

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    ed->edInProgrammaticChange++;
    appDocScrollVertically( ed, scrolledY );
    ed->edInProgrammaticChange--;

    return;
    }

void appDocScrollToX(	EditDocument *		ed,
			int			x )
    {
    int		scrolledX= x- ed->edVisibleRect.drX0;

    if  ( scrolledX == 0 )
	{ return;	}

    ed->edVisibleRect.drX0 += scrolledX;
    ed->edVisibleRect.drX1 += scrolledX;

    ed->edInProgrammaticChange++;
    appDocScrollHorizontally( ed, scrolledX );
    ed->edInProgrammaticChange--;

    return;
    }

APP_SCROLLBAR_CALLBACK_H( appDocVerticalScrollbarCallback, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( ! ed->edInProgrammaticChange )
	{
	int	y= guiGetScrollbarValueFromCallback( w, e );

	appDocScrollToY( ed, y );
	}
    }

APP_SCROLLBAR_CALLBACK_H( appDocHorizontalScrollbarCallback, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( ! ed->edInProgrammaticChange )
	{
	int	x= guiGetScrollbarValueFromCallback( w, e );

	appDocScrollToX( ed, x );
	}
    }

/************************************************************************/
/*									*/
/*  Scroll the selection into view.					*/
/*									*/
/*  NOTE: sliderSize is passed to XmScrollBarSetValues() because of a	*/
/*	bug in lesstif Release 0.87.0. (Jan 1999).			*/
/*									*/
/************************************************************************/


static void appScrollDocumentToXY( int *		pScrolledX,
				int *			pScrolledY,
				EditDocument *		ed,
				int			oox,
				int			nox,
				int			hSliderSize,
				int			ooy,
				int			noy,
				int			vSliderSize )
    {
    int		scrolledX= nox- oox;
    int		scrolledY= noy- ooy;
	
#   if USE_GTK
    int		changed= 0;
#   endif

    if  ( noy != ooy )
	{
	ed->edInProgrammaticChange++;
	guiSetScrollbarValues( ed->edVerticalScrollbar, noy, vSliderSize );
	ed->edInProgrammaticChange--;
#	if USE_GTK
	changed= 1;
#	endif
	}

    if  ( nox != oox )
	{
	ed->edInProgrammaticChange++;
	guiSetScrollbarValues( ed->edHorizontalScrollbar, nox, hSliderSize );
	ed->edInProgrammaticChange--;
#	if USE_GTK
	changed= 1;
#	endif
	}

    ed->edVisibleRect.drX0 += scrolledX;
    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drX1 += scrolledX;
    ed->edVisibleRect.drY1 += scrolledY;

    if  ( scrolledX != 0 )
	{
	ed->edInProgrammaticChange++;
	appDocScrollHorizontally( ed, scrolledX );
	ed->edInProgrammaticChange--;
	}
    if  ( scrolledY != 0 )
	{
	ed->edInProgrammaticChange++;
	appDocScrollVertically( ed, scrolledY );
	ed->edInProgrammaticChange--;
	}

#   if USE_GTK
    if  ( changed )
	{ guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );	}
#   endif

    if  ( pScrolledX )
	{ *pScrolledX= scrolledX;	}
    if  ( pScrolledY )
	{ *pScrolledY= scrolledY;	}

    return;
    }

void appScrollToRectangle(	int *				pScrolledX,
				int *				pScrolledY,
				EditDocument *			ed,
				const DocumentRectangle *	dr )
    {
    int		vSliderSize;
    int		hSliderSize;

    int		oox= ed->edVisibleRect.drX0;
    int		ooy= ed->edVisibleRect.drY0;
    int		nox= oox;
    int		noy= ooy;

    int		visWide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;
    int		visHigh= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    int		fullWide= ed->edFullRect.drX1- ed->edFullRect.drX0+ 1;
    int		fullHigh= ed->edFullRect.drY1- ed->edFullRect.drY0+ 1;

    guiGetScrollbarValues( &noy, &vSliderSize, ed->edVerticalScrollbar );

    if  ( dr->drY0 <= ed->edVisibleRect.drY0	||
	  dr->drY1 >= ed->edVisibleRect.drY1	)
	{
	noy= ( dr->drY0+ dr->drY1 )/ 2- visHigh/ 2;

	if  ( noy > fullHigh- visHigh )
	    { noy=  fullHigh- visHigh;	}
	if  ( noy < 0 )
	    { noy= 0;	 }
	}

    guiGetScrollbarValues( &nox, &hSliderSize, ed->edHorizontalScrollbar );

    if  ( dr->drX0 <= ed->edVisibleRect.drX0	||
	  dr->drX1 >= ed->edVisibleRect.drX1	)
	{
	nox= ( dr->drX0+ dr->drX1 )/ 2- visWide/ 2;

	if  ( nox > fullWide- visWide )
	    { nox=  fullWide- visWide;	}
	if  ( nox < 0 )
	    { nox= 0;	 }
	}

    appScrollDocumentToXY( pScrolledX, pScrolledY, ed,
					oox, nox, hSliderSize,
					ooy, noy, vSliderSize );

    return;
    }

void appScrollToPosition(	int *				pScrolledX,
				int *				pScrolledY,
				EditDocument *			ed,
				int				x,
				int				y )
    {
    int		vSliderSize;
    int		hSliderSize;

    int		oox= ed->edVisibleRect.drX0;
    int		ooy= ed->edVisibleRect.drY0;
    int		nox= oox;
    int		noy= ooy;

    int		visWide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;
    int		visHigh= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    int		fullWide= ed->edFullRect.drX1- ed->edFullRect.drX0+ 1;
    int		fullHigh= ed->edFullRect.drY1- ed->edFullRect.drY0+ 1;

    guiGetScrollbarValues( &noy, &vSliderSize, ed->edVerticalScrollbar );

    if  ( y <= ed->edVisibleRect.drY0 )
	{ noy= y;	}
    else{
	if  ( y >= ed->edVisibleRect.drY1 )
	    { noy= y- visHigh+ 1;	}
	}

    if  ( noy > fullHigh- visHigh )
	{ noy=  fullHigh- visHigh;	}
    if  ( noy < 0 )
	{ noy= 0;	 }

    guiGetScrollbarValues( &nox, &hSliderSize, ed->edHorizontalScrollbar );

    if  ( x <= ed->edVisibleRect.drX0 )
	{ nox= x;	}
    else{
	if  ( x >= ed->edVisibleRect.drX1 )
	    { nox= x- visWide+ 1;	}
	}

    if  ( nox > fullWide- visWide )
	{ nox=  fullWide- visWide;	}
    if  ( nox < 0 )
	{ nox= 0;	 }

    appScrollDocumentToXY( pScrolledX, pScrolledY, ed,
					oox, nox, hSliderSize,
					ooy, noy, vSliderSize );
    }

/************************************************************************/
/*									*/
/*  Prevent the Shell that contains a document from being resized	*/
/*  beyond normal limits.						*/
/*									*/
/************************************************************************/

static void appFileAdaptHorizontalRulerRange(	EditDocument *	ed,
						int		wide )
    {
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edTopRuler && ea->eaSetTopRulerRange )
	{
	(*ea->eaSetTopRulerRange)( ed->edTopRuler,
					ed->edTopRulerWidget,
					ed->edFullRect.drX1,
					ed->edVisibleRect.drX0,
					ed->edVisibleRect.drX1 );
	}

    if  ( ed->edBottomRuler && ea->eaSetBottomRulerRange )
	{
	(*ea->eaSetBottomRulerRange)( ed->edBottomRuler,
					ed->edBottomRulerWidget,
					ed->edFullRect.drX1,
					ed->edVisibleRect.drX0,
					ed->edVisibleRect.drX1 );
	}

    return;
    }

static void appFileAdaptVerticalRulerRange(	EditDocument *	ed,
						int		high )
    {
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edLeftRuler && ea->eaSetLeftRulerRange )
	{
	(*ea->eaSetLeftRulerRange)( ed->edLeftRuler,
					ed->edLeftRulerWidget,
					ed->edFullRect.drY1,
					ed->edVisibleRect.drY0,
					ed->edVisibleRect.drY1 );
	}

    if  ( ed->edRightRuler && ea->eaSetRightRulerRange )
	{
	(*ea->eaSetRightRulerRange)( ed->edRightRuler,
					ed->edRightRulerWidget,
					ed->edFullRect.drY1,
					ed->edVisibleRect.drY0,
					ed->edVisibleRect.drY1 );
	}

    return;
    }

static void appDocAdaptToWidgetSize(	EditDocument *	ed,
					int		wide,
					int		high )
    {
    int			changed= 0;
    int			d;

    if  ( ed->edVisibleRect.drY1 != ed->edVisibleRect.drY0+ high- 1 )
	{
	ed->edVisibleRect.drY1= ed->edVisibleRect.drY0+ high- 1;
	changed= 1;

	if  ( ed->edVisibleRect.drY1 > ed->edFullRect.drY1 )
	    {
	    d= ed->edVisibleRect.drY1- ed->edFullRect.drY1;

	    if  ( d > ed->edVisibleRect.drY0 )
		{ d= ed->edVisibleRect.drY0;	}

	    ed->edVisibleRect.drY0 -= d;
	    ed->edVisibleRect.drY1 -= d;
	    }

	appFileAdaptVerticalRulerRange( ed, high );

	guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );
	}

    if  ( ed->edVisibleRect.drX1 != ed->edVisibleRect.drX0+ wide+ 1 )
	{
	ed->edVisibleRect.drX1= ed->edVisibleRect.drX0+ wide+ 1;
	changed= 1;

	if  ( ed->edVisibleRect.drX1 > ed->edFullRect.drX1 )
	    {
	    d= ed->edVisibleRect.drX1- ed->edFullRect.drX1;

	    if  ( d > ed->edVisibleRect.drX0 )
		{ d= ed->edVisibleRect.drX0;	}

	    ed->edVisibleRect.drX0 -= d;
	    ed->edVisibleRect.drX1 -= d;
	    }

	appFileAdaptHorizontalRulerRange( ed, wide );

	guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );
	}

    if  ( changed )
	{ appDocSetScrollbarValues( ed );	}

    return;
    }

void appDocumentRulerWidth(	struct EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    int			mult;
    const double	pixPerMM= TWIPS_PER_MM* ea->eaPixelsPerTwip;

    /*  1  */
    ed->edLeftRulerWidePixels=
			(int)( ea->eaLeftRulerWidthMM* pixPerMM );
    ed->edTopRulerHighPixels=
			(int)( ea->eaTopRulerHeightMM* pixPerMM );
    ed->edRightRulerWidePixels=
			(int)( ea->eaRightRulerWidthMM * pixPerMM );
    ed->edBottomRulerHighPixels=
			(int)( ea->eaBottomRulerHeightMM* pixPerMM );

    mult= ea->eaLeftRulerWidthMultiple;
    if  ( mult )
	{
	ed->edLeftRulerWidePixels=
		    mult* ( ( ed->edLeftRulerWidePixels+ mult- 1 )/ mult );
	}

    mult= ea->eaTopRulerHeightMultiple;
    if  ( mult )
	{
	ed->edTopRulerHighPixels=
		    mult* ( ( ed->edTopRulerHighPixels+ mult- 1 )/ mult );
	}

    mult= ea->eaRightRulerWidthMultiple;
    if  ( mult )
	{
	ed->edRightRulerWidePixels=
		    mult* ( ( ed->edRightRulerWidePixels+ mult- 1 )/ mult );
	}

    mult= ea->eaBottomRulerHeightMultiple;
    if  ( mult )
	{
	ed->edBottomRulerHighPixels=
		    mult* ( ( ed->edBottomRulerHighPixels+ mult- 1 )/ mult );
	}

    return;
    }

APP_EVENT_HANDLER_H( appDocConfigure, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    int			wide;
    int			high;

    if  ( guiDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    appDocAdaptToWidgetSize( ed, wide, high );

    return;
    }

void appMouseWheelDown(		EditDocument *	ed )
    {
    int			ooy= ed->edVisibleRect.drY0;
    int			noy= ed->edVisibleRect.drY0;
    int			scrolledY;

    int			sliderSize;

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    noy += ( sliderSize+ SCROLL_WHEEL_STEP- 1 )/ SCROLL_WHEEL_STEP;

    if  ( noy > ed->edFullRect.drY1- sliderSize )
	{ noy=  ed->edFullRect.drY1- sliderSize;	}

    scrolledY= noy- ooy;

    if  ( scrolledY == 0 )
	{ return;	}

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    appDocScrollVertically( ed, scrolledY );

    guiSetScrollbarValues( ed->edVerticalScrollbar, noy, sliderSize );
    }

void appMouseWheelUp(		EditDocument *	ed )
    {
    int			ooy= ed->edVisibleRect.drY0;
    int			noy= ed->edVisibleRect.drY0;
    int			scrolledY;

    int			sliderSize;

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    noy -= ( sliderSize+ SCROLL_WHEEL_STEP- 1 )/ SCROLL_WHEEL_STEP;

    if  ( noy < 0 )
	{ noy=  0;	}

    scrolledY= noy- ooy;

    if  ( scrolledY == 0 )
	{ return;	}

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    appDocScrollVertically( ed, scrolledY );

    guiSetScrollbarValues( ed->edVerticalScrollbar, noy, sliderSize );
    }

APP_EVENT_HANDLER_H( appScrollEventHandler, w, voided, scrollEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    int			direction= SCROLL_DIRECTION_FROM_EVENT( scrollEvent );

    switch( direction )
	{
#	if USE_MOTIF
	case Button1:
	case Button2:
	case Button3:
	    break;
#	endif

	case SCROLL_UP:
	    appMouseWheelUp( ed );
	    break;

	case SCROLL_DOWN:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LDEB(direction);
	    break;
	}

    return;
    }

void appDocSetScrollbarValues(	EditDocument *	ed )
    {
    int			sliderSize;
    int			maximum;
    int			minimum;
    int			value;

    /******/

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    minimum= ed->edFullRect.drY0;
    value= ed->edVisibleRect.drY0;

    if  ( sliderSize >= ed->edFullRect.drY1+ 1 )
	{ maximum= ed->edVisibleRect.drY1+ 1;	}
    else{ maximum= ed->edFullRect.drY1+ 1;	}

    if  ( value+ sliderSize > maximum )
	{ value= maximum- sliderSize; }

    ed->edInProgrammaticChange++;
    appDocSetVerticalScrollbarValues( ed,
				minimum, maximum, value, sliderSize );
    ed->edInProgrammaticChange--;

    /******/

    sliderSize= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;
    minimum= ed->edFullRect.drX0;
    value= ed->edVisibleRect.drX0;

    if  ( sliderSize >= ed->edFullRect.drX1+ 1 )
	{ maximum= ed->edVisibleRect.drX1+ 1;	}
    else{ maximum= ed->edFullRect.drX1+ 1;	}

    ed->edInProgrammaticChange++;
    appDocSetHorizontalScrollbarValues( ed,
				minimum, maximum, value, sliderSize );
    ed->edInProgrammaticChange--;

    return;
    }

#   endif
