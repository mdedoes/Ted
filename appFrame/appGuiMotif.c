#   include	"appFrameConfig.h"

#   if USE_MOTIF

#   include	"appEditApplication.h"
#   include	<guiBase.h>
#   include	<guiWidgets.h>
#   include	<guiWidgetsImpl.h>
#   include	<guiImageImpl.h>
#   include	"appGuiApplication.h"

#   include	<appDebugon.h>

#   include	<X11/Xatom.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>

/************************************************************************/
/*									*/
/*  Show a shell widget. Because of the RIDICULOUS and ABSURD situation	*/
/*  the you cannot rely on a shell window to be visible after we tell	*/
/*  it to become visible, we handle a series of events in a loop until	*/
/*  it becomes visible.							*/
/*									*/
/************************************************************************/

void appShowShellWidget(		EditApplication *	ea,
					APP_WIDGET		shell )
    {
    XtRealizeWidget( shell );

    XtVaSetValues( shell, XmNinitialState, NormalState, NULL );
    XtMapWidget( shell );
    XRaiseWindow( XtDisplay( shell ), XtWindow( shell ) );

    XSync( XtDisplay( shell ), False );

    guiMotifWaitForWindow( shell, ea->eaContext );
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetPixelsPerTwip(	EditApplication *	ea )
    {
#   if 0
    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );
    int			screen= DefaultScreen( display );

    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= ( (double)DisplayWidth( display, screen ) )/
					DisplayWidthMM( display, screen );
    verPixPerMM= ( (double)DisplayHeight( display, screen ) )/
					DisplayHeightMM( display, screen );

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    ea->eaPixelsPerTwip= xfac;
#   else
    ea->eaPixelsPerTwip= 96.0/ ( 20* 72 );
#   endif

    return;
    }

#   endif
