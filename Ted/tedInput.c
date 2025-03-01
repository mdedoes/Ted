/************************************************************************/
/*									*/
/*  Ted: Handle user input from the keyboard.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedDraw.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<tedDocFront.h>
#   include	<docEditCommand.h>
#   include	"tedDocMenu.h"
#   include	"tedEdit.h"
#   include	<guiKeys.h>
#   include	<appEditDocument.h>
#   include	"tedKeyboard.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Just log events that pass by for debugging purposes.		*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF_AND_NEEDED

static void tedLogEvent(	Widget		w,
				void *		voided,
				XEvent *	event,
				Boolean *	pRefused )
    {
    EditDocument *		ed= (EditDocument *)voided;

    if  ( ! event )
	{ return;	}

    appDebug( "EVENT \"%s\": %s\n",
			ed->edTitle, APP_X11EventNames[event->type] );

    *pRefused= 1;
    }

#   endif

void tedInputSetSelectedPosition(
				struct EditDocument *		ed,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    int			scrolledX= 0;
    int			scrolledY= 0;

    tedSetSelectedPosition( ed, dp, lastLine, &scrolledX, &scrolledY );

    return;
    }

void tedInputChangeSelection(	struct EditDocument *		ed,
				unsigned int			keyState,
				const DocumentSelection *	dsOrig,
				const DocumentPosition *	dpSet,
				int				lastLine )
    {
    if  ( keyState & KEY_SHIFT_MASK )
	{
	tedExtendSelectionToPosition( ed, &(dsOrig->dsAnchor), dpSet );

	/* tedExtendSelectionToPosition() updates the selection description */

	tedAdaptToolsToSelection( ed );
	return;
	}

    tedInputSetSelectedPosition( ed, dpSet, lastLine );

    return;
    }

/************************************************************************/
/*									*/
/*  Navigation based on a Right arrow key.				*/
/*									*/
/************************************************************************/

int tedMoveRightOnKey(	DocumentPosition *	dpNew,
			struct BufferDocument *	bd,
			int			keyState )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docNextWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docGotoNextPosition( dpNew ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Navigation based on a Left arrow key.				*/
/*									*/
/************************************************************************/

int tedMoveLeftOnKey(	DocumentPosition *	dpNew,
			struct BufferDocument *	bd,
			int			keyState )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docPrevWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docGotoPrevPosition( dpNew ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a modification.						*/
/*									*/
/*  1)  Check if editing is possible and allowed			*/
/*  2)  Stop the blinking cursor.					*/
/*									*/
/************************************************************************/

static int tedRefuseEdit(	struct EditDocument *		ed )
    {
#   ifdef NOT_USE_MOTIF
    const AppDrawingData *	add= &(ed->edDocumentWidget.dwDrawingData);

    XBell( add->addDisplay, 0 );
#   endif

#   if USE_GTK
    gdk_beep();
#   endif

    return 1;
    }

static int tedStartEdit(	struct EditDocument *		ed,
				const SelectionDescription *	sd,
				int				editCommand )
    {
    if  ( ! sd->sdIsSet )
	{ return tedRefuseEdit( ed );	}

    if  ( editCommand >= 0 )
	{
	unsigned char	cmdEnabled[EDITcmd_COUNT];

	docEnableEditCommands( cmdEnabled, sd );

	if  ( ! cmdEnabled[editCommand] )
	    { return tedRefuseEdit( ed );	}

	}

    tedStopCursorBlink( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a key stroke. (Special keys only: Keys that result in text	*/
/*  are not handled here.						*/
/*									*/
/************************************************************************/

void tedDocGotKey(	void *			voided,
			int			keySym,
			unsigned int		state )
    {
    EditDocument *		ed= (EditDocument *)voided;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentTree *	tree;
    struct BufferItem *		bodySectNode;

    TedKeyHandler		keyHandler;
    int				editCommand;

    if  ( tedGetSelection( &ds, &sg, &sd, &tree, &bodySectNode, ed ) )
	{ LDEB(1); goto ready;	}

    /* only interested in these 2 */
    state= state & ( KEY_CONTROL_MASK|KEY_SHIFT_MASK);

    keyHandler= tedKeyGetMoveHandler( ed, keySym, state, &ds, &sd );
    if  ( keyHandler )
	{
	if  ( ! tedStartEdit( ed, &sd, -1 ) )
	    { (*keyHandler)( ed, keySym, state, &ds, &sd, &sg );	}

	goto ready;
	}

    keyHandler= tedKeyGetEditHandler( &editCommand,
					    ed, keySym, state, &ds, &sd );
    if  ( keyHandler )
	{
	if  ( ! tedStartEdit( ed, &sd, editCommand ) )
	    { (*keyHandler)( ed, keySym, state, &ds, &sd, &sg );	}

	goto ready;
	}

#   if USE_GTK
#	if GTK_MAJOR_VERSION < 2
	gtk_accel_group_activate( ed->edToplevel.atAccelGroup,
							    keySym, state );
#	else
	gtk_accel_groups_activate(
		    G_OBJECT( ed->edToplevel.atAccelGroup ), keySym, state );
#	endif
#   endif

  ready:

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

void tedDocGotString(		void *			voided,
				const char *		str,
				int			length )
    {
    EditDocument *		ed= (EditDocument *)voided;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentTree *	tree;
    struct BufferItem *		bodySectNode;

    if  ( tedGetSelection( &ds, &sg, &sd, &tree, &bodySectNode, ed ) )
	{ LDEB(1); goto ready;	}

    if  ( tedStartEdit( ed, &sd, EDITcmdREPLACE ) )
	{ goto ready;	}

    if  ( tedDocReplaceSelectionTyping( ed, str, length ) )
	{ LDEB(length); goto ready;	}

  ready:

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}
    }

APP_EVENT_HANDLER_H( tedScrollEventHandler, w, voided, scrollEvent )
    {
    struct EditDocument *	ed= (struct EditDocument *)voided;
    int				direction= SCROLL_DIRECTION_FROM_EVENT( scrollEvent );


    tedStopCursorBlink( ed );

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

#	if USE_GTK
#	if GTK_MAJOR_VERSION >= 3
	case GDK_SCROLL_SMOOTH:
	    /* Picked up by the scrollbar */
	    break;

#	endif
#	endif

	default:
	    LLDEB(scrollEvent->type,direction);
	    break;
	}

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

