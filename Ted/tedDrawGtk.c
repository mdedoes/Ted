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

#   ifdef USE_GTK

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
    GdkWindow *			win= add->addDrawable;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				wide= io->ioPixelsWide;
    int				high= io->ioPixelsHigh;

    GdkWindowAttr		xswa;
    gint			attributesMask;

    static GdkCursor *		moveCursor;
    static GdkCursor *		bottomCursor;
    static GdkCursor *		rightCursor;
    static GdkCursor *		cornerCursor;

    if  ( ed->edFileReadOnly )
	{ td->tdObjectSelected= 1; return; }

    if  ( ! moveCursor )
	{
	moveCursor= gdk_cursor_new( GDK_FLEUR );
	bottomCursor= gdk_cursor_new( GDK_BOTTOM_SIDE );
	rightCursor= gdk_cursor_new( GDK_RIGHT_SIDE );
	cornerCursor= gdk_cursor_new( GDK_BOTTOM_RIGHT_CORNER );
	}

    xswa.wclass= GDK_INPUT_ONLY;
    xswa.window_type= GDK_WINDOW_CHILD;
    attributesMask= GDK_WA_X | GDK_WA_Y;

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	xswa.x= pg->pgXPixels- ox;
	xswa.y= pg->pgBaselinePixels- high- oy;
	xswa.width= wide;
	xswa.height= high;
	td->tdObjectWindow= gdk_window_new( win, &xswa, attributesMask );

	xswa.cursor= bottomCursor;
	xswa.x= wide/ 2- RESIZE_BLOCK/ 2;
	xswa.y= high- RESIZE_BLOCK;
	xswa.width= RESIZE_BLOCK;
	xswa.height= RESIZE_BLOCK;
	td->tdObjectBottomWindow= gdk_window_new( td->tdObjectWindow,
					 &xswa, attributesMask|GDK_WA_CURSOR );

	xswa.cursor= rightCursor;
	xswa.x= wide- RESIZE_BLOCK;
	xswa.y= high/ 2- RESIZE_BLOCK/ 2;
	xswa.width= RESIZE_BLOCK;
	xswa.height= RESIZE_BLOCK;
	td->tdObjectRightWindow= gdk_window_new( td->tdObjectWindow,
					 &xswa, attributesMask|GDK_WA_CURSOR );

	xswa.cursor= cornerCursor;
	xswa.x= wide- RESIZE_BLOCK;
	xswa.y= high- RESIZE_BLOCK;
	xswa.width= RESIZE_BLOCK;
	xswa.height= RESIZE_BLOCK;
	td->tdObjectCornerWindow= gdk_window_new( td->tdObjectWindow,
					 &xswa, attributesMask|GDK_WA_CURSOR );

	gdk_window_show( td->tdObjectBottomWindow );
	gdk_window_show( td->tdObjectRightWindow );
	gdk_window_show( td->tdObjectCornerWindow );
	}
    else{
	gdk_window_move_resize( td->tdObjectWindow,
			    pg->pgXPixels- ox, pg->pgBaselinePixels- high- oy,
				wide, high );

	gdk_window_move( td->tdObjectBottomWindow,
				wide/ 2- RESIZE_BLOCK/ 2, high- RESIZE_BLOCK );
	gdk_window_move( td->tdObjectRightWindow,
				wide- RESIZE_BLOCK, high/ 2- RESIZE_BLOCK/ 2 );
	gdk_window_move( td->tdObjectCornerWindow,
				wide- RESIZE_BLOCK, high- RESIZE_BLOCK );
	}

    gdk_window_show( td->tdObjectWindow );

    td->tdObjectSelected= 1;
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ed->edFileReadOnly )
	{ td->tdObjectSelected= 0; return; }

    gdk_window_hide( td->tdObjectWindow );
    td->tdObjectSelected= 0;
    }
#   endif
