#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appAppFront.h"
#   include	<guiWidgets.h>
#   include	<guiNamedImages.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Use a named picture as the window icon.				*/
/*									*/
/************************************************************************/

void appSetNamedWindowIcon(	struct EditApplication *	ea,
				APP_WIDGET			win,
				const char *			iconName )
    {
    struct NamedImage *	ni;

    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
							iconName, &ni ) )
	{ SDEB(iconName); return;	}

    guiSetWindowIcon( win, ni );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback that is invoked when the application is closed through	*/
/*  the window manager.							*/
/*									*/
/************************************************************************/

APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    if  ( ea->eaMainVisibleAsAbout )
	{
	ea->eaMainVisibleAsAbout= 0;

	if  ( ea->eaVisibleDocumentCount > 0 )
	    { guiHideShellWidget( ea->eaToplevel.atTopWidget ); }
	}
    else{
	appQuitApplication( (APP_WIDGET)0,
				    ea->eaToplevel.atTopWidget, ea );
	}
    }

#   endif
