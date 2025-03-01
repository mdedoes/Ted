/************************************************************************/
/*									*/
/*  Callbacks/Event handlers for the document window.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appDocument.h"
#   include	"appEditDocument.h"
#   include	"appGuiDocument.h"
#   include	"appDocFront.h"
#   include	<guiWidgetDrawingSurface.h>

#   include	<appDebugon.h>

#   define	LOG_REDRAWS	0

/************************************************************************/
/*									*/
/*  'Close' callback for the file menu and for the window manager menu	*/
/*									*/
/*  1)  As 'mwm' sometimes sends a 'Save yourself' when 'Close' is	*/
/*	selected from the window manager menu, deactivate the protocol.	*/
/*	B.T.W. This does not help.					*/
/*									*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocFileClose, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( ed->edHasBeenChanged )
	{ appRunReallyCloseDialog( option, ed ); return; }

    appCloseDocument( ed );

    return;
    }

APP_CLOSE_CALLBACK_H( appDocFileCloseCallback, w, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( ed->edHasBeenChanged )
	{ appRunReallyCloseDialog( ed->edFileCloseOption, ed ); return; }

    appCloseDocument( ed );
    }

APP_MENU_CALLBACK_H( appDocFileNew, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    const MemoryBuffer *	filename= (const MemoryBuffer *)0;

    if  ( appNewDocument( ea, filename ) )
	{ LDEB(1); }

    return;
    }

APP_MENU_CALLBACK_H( appDocFileQuit, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appQuitApplication( option, ed->edToplevel.atTopWidget, ea );

    return;
    }

/************************************************************************/
/*									*/
/*  The user selected the 'about' option.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocAbout, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    ea->eaMainVisibleAsAbout= 1;

    appShowShellWidget( ea, ea->eaToplevel.atTopWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Callbacks for the edit menu.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocEditCopy, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocCopy )
	{ XDEB(ea->eaDocCopy); return;	}

    (*ea->eaDocCopy)( ed ); return;
    }

APP_MENU_CALLBACK_H( appDocEditCut, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocCut )
	{ XDEB(ea->eaDocCut); return;	}

    (*ea->eaDocCut)( ed ); return;
    }

APP_MENU_CALLBACK_H( appDocEditSelAll, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocSelAll )
	{ XDEB(ea->eaDocSelAll); return;	}

    ea->eaDocSelAll( ed ); return;
    }

/************************************************************************/
/*									*/
/*  Destroy callback on a document widget.				*/
/*									*/
/************************************************************************/

APP_DESTROY_CALLBACK_H( appDestroyEditDocument, w, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edPrivateData )
	{
	(*ea->eaFreeDocument)( ed->edPrivateData, ed->edFormat );
	ed->edPrivateData= (void *)0;
	}

    free( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw a rectangle in the document widget.				*/
/*									*/
/************************************************************************/

APP_REDRAW_HANDLER_H( appDocExposeHandler, w, voided, exposeEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentRectangle		drClip;

    if  ( ed->edNotYetDrawn				&&
	  ea->eaDocumentFirstVisible			)
	{ (*ea->eaDocumentFirstVisible)( ed );	}

    ed->edNotYetDrawn= 0;

    /*  1  */
    guiStartDrawing( &drClip, ed->edDrawingSurface,
			    ed->edDocumentWidget.dwWidget, exposeEvent );

    geoShiftRectangle( &drClip, ox, oy );

    /*  2,3,4  */
    (*ea->eaDrawRectangle)( ed, &drClip );

    drawNoClipping( ed->edDrawingSurface );

    return;
    }

#   endif
