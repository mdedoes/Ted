/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  appExposeRectangle().						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"docDraw.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/cursorfont.h>

/************************************************************************/
/*									*/
/*  Create the windows that are used to reshape selected objects.	*/
/*									*/
/************************************************************************/

void tedSetObjectWindows(	EditDocument *			ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				int				ox,
				int				oy )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    Display *			display= add->addDisplay;
    Window			win= add->addDrawable;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				wide= io->ioPixelsWide;
    int				high= io->ioPixelsHigh;

    int				x= pg->pgXPixels- ox;
    int				y= pg->pgBaselinePixels- high- oy;

    XSetWindowAttributes	xswa;

    static Cursor		moveCursor;
    static Cursor		bottomCursor;
    static Cursor		rightCursor;
    static Cursor		cornerCursor;

    if  ( ed->edFileReadOnly )
	{ td->tdObjectSelected= 1; return; }

    if  ( ! moveCursor )
	{
	moveCursor= XCreateFontCursor( display, XC_fleur );
	bottomCursor= XCreateFontCursor( display, XC_bottom_side );
	rightCursor= XCreateFontCursor( display, XC_right_side );
	cornerCursor= XCreateFontCursor( display, XC_bottom_right_corner );
	}

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	td->tdObjectWindow= XCreateWindow( display, win,
			    x, y, wide, high,
			    0, CopyFromParent, InputOnly, CopyFromParent,
			    0L, &xswa );

	xswa.cursor= bottomCursor;
	td->tdObjectBottomWindow= XCreateWindow( display, td->tdObjectWindow,
			    wide/ 2- RESIZE_BLOCK/ 2, high- RESIZE_BLOCK,
			    RESIZE_BLOCK, RESIZE_BLOCK,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    CWCursor, &xswa );

	xswa.cursor= rightCursor;
	td->tdObjectRightWindow= XCreateWindow( display, td->tdObjectWindow,
			    wide- RESIZE_BLOCK, high/ 2- RESIZE_BLOCK/ 2,
			    RESIZE_BLOCK, RESIZE_BLOCK,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    CWCursor, &xswa );

	xswa.cursor= cornerCursor;
	td->tdObjectCornerWindow= XCreateWindow( display, td->tdObjectWindow,
			    wide- RESIZE_BLOCK, high- RESIZE_BLOCK,
			    RESIZE_BLOCK, RESIZE_BLOCK,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    CWCursor, &xswa );

	XMapRaised( display, td->tdObjectBottomWindow );
	XMapRaised( display, td->tdObjectRightWindow );
	XMapRaised( display, td->tdObjectCornerWindow );
	}
    else{
	XMoveResizeWindow( display, td->tdObjectWindow, x, y, wide, high );

	XMoveWindow( display, td->tdObjectBottomWindow,
				wide/ 2- RESIZE_BLOCK/ 2, high- RESIZE_BLOCK );
	XMoveWindow( display, td->tdObjectRightWindow,
				wide- RESIZE_BLOCK, high/ 2- RESIZE_BLOCK/ 2 );
	XMoveWindow( display, td->tdObjectCornerWindow,
				wide- RESIZE_BLOCK, high- RESIZE_BLOCK );
	}

    XMapRaised( display, td->tdObjectWindow );

    td->tdObjectSelected= 1;
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ed->edFileReadOnly )
	{ td->tdObjectSelected= 0; return; }

    XUnmapWindow( XtDisplay( ed->edDocumentWidget ), td->tdObjectWindow );
    td->tdObjectSelected= 0;
    }

#   endif
