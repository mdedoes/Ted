/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedDraw.h"
#   include	"tedSelect.h"
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<tedToolFront.h>
#   include	<docField.h>
#   include	<docHyperlinkField.h>
#   include	<guiDrawingWidget.h>
#   include	<docParaParticules.h>
#   include	<docFieldKind.h>
#   include	<docLayout.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	<appEditApplication.h>
#   include	<docBuf.h>
#   include	<appDocFront.h>
#   include	<layoutContext.h>

#   include	<guiKeys.h>

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

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

typedef struct DraggingContext
    {
    int				dcMouseX;
    int				dcMouseY;
    DocumentPosition		dcAnchorPosition;
    struct BufferItem *		dcSelRootNode;
    struct BufferItem *		dcSelBodySectNode;
    struct EditDocument *	dcEd;
    struct DocumentTree *	dcTree;
    LayoutContext		dcLayoutContext;
    } DraggingContext;

/************************************************************************/
/*									*/
/*  Find the document position under the mouse and extend the selection	*/
/*  to that position.							*/
/*									*/
/************************************************************************/

static int tedExtendSelectionToXY( DraggingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    EditDocument *		ed= dc->dcEd;

    DocumentPosition		dpTo;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    dpTo= dc->dcAnchorPosition;

    if  ( tedFindPosition( &dpTo, dc->dcSelRootNode, dc->dcSelBodySectNode,
			    lc, dc->dcMouseX+ ox, dc->dcMouseY+ oy ) )
	{ /*LLDEB(mouseX,mouseY);*/ return 0; }

    if  ( tedExtendSelectionToPosition( ed, &(dc->dcAnchorPosition), &dpTo ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a multi-click.						*/
/*									*/
/************************************************************************/

static void tedHandleMultiClick(EditDocument *			ed,
				const DocumentSelection *	dsWord,
				const DocumentPosition *	dpClick,
				const PositionGeometry *	pgClick,
				int				seq )
    {
    DocumentSelection	dsNew;

    docInitDocumentSelection( &dsNew );

    docSetIBarSelection( &dsNew, dpClick );

    if  ( seq > 2 )
	{
	int		line= 0;

	if  ( docGetLineOfPosition( &line, dpClick, pgClick->pgPositionFlags ) )
	    { LDEB(1); return;	}

	docLineSelection( &dsNew, dpClick->dpNode, line );
	}
    else{ dsNew= *dsWord;	}

    /*  Caller describes and adapts tools */
    tedSetSelectionLow( ed, &dsNew,
		    pgClick->pgPositionFlags & POSflagLINE_HEAD,
		    (int *)0, (int *)0 );

    }

/************************************************************************/
/*									*/
/*  Select the object that we just clicked.				*/
/*									*/
/************************************************************************/

static int tedSelectObject(	EditDocument *			ed,
				const LayoutContext *		lc,
				const DocumentSelection *	dsWord,
				const PositionGeometry *	pgClick,
				int				afterObject,
				int				docX )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= lc->lcDocument;

    int				partObject;
    DocumentPosition		dpObject;
    struct InsertedObject *	io;

    if  ( ! docGetObjectSelection( &partObject, &dpObject, &io, bd, dsWord ) )
	{
	DocumentRectangle	drObject;
	int			marg= 0;
	int			wide;

	tedGetObjectRectangle( &drObject, (struct Point2DI *)0,
				    io, pgClick, lc, afterObject, td );

	wide= drObject.drX1- drObject.drX0+ 1;
	if  ( wide > 10 )
	    { marg++;	}
	if  ( wide > 20 )
	    { marg++;	}

	if  ( docX >= drObject.drX0+ marg	&&
	      docX <= drObject.drX1- marg	)
	    {
	    /*  Caller describes and adapts tools */
	    tedSetSelectionLow( ed, dsWord,
				pgClick->pgPositionFlags & POSflagLINE_HEAD,
				(int *)0, (int *)0 );
	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Locate a mouse click.						*/
/*									*/
/************************************************************************/

static int tedFindMousePosition( APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				const DocumentSelection *	dsOld,
				int *				pSameRoot,
				int *				pSameInstance,
				PositionGeometry *		pgClick,
				int *				pDocX,
				int *				pSeq,
				unsigned int *			pKeyState,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				sameRoot= 1;
    int				sameInstance= 1;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    struct BufferItem *		rootNodeFound;
    struct BufferItem *		bodySectNodeFound;

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return 1;	}

    /********************************************************************/
    /*  Cope with GTK, that returns events for clicks in the ruler and	*/
    /*  in the menu bar.						*/
    /********************************************************************/
    if  ( mouseX < 0 || mouseY < 0 )
	{ return 1;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcEd= ed;
    tedSetScreenLayoutContext( &(dc->dcLayoutContext), ed );
    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    dc->dcSelRootNode= bd->bdBody.dtRoot;
    dc->dcTree= &(bd->bdBody);

    if  ( tedFindPositionForCoordinates( &(dc->dcAnchorPosition), pgClick,
			    &(dc->dcTree), &rootNodeFound, &bodySectNodeFound,
			    ed, docX, docY ) )
	{ /*LLDEB(docX,docY);*/ return 1;	}

    dc->dcSelRootNode= rootNodeFound;
    dc->dcSelBodySectNode= bodySectNodeFound;

    sameRoot= docSelectionSameRoot( dsOld, rootNodeFound );
    if  ( sameRoot )
	{
	sameInstance= docSelectionSameInstance( dc->dcTree,
					pgClick->pgTopPosition.lpPage,
					pgClick->pgTopPosition.lpColumn );
	}

    if  ( ! sameRoot || ! sameInstance )
	{
	SelectionScope		ss;
	struct BufferItem *	bodySectNode;

	docGetSelectionScope( &ss, rootNodeFound );

	if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0,
						    &bodySectNode, bd, &ss ) )
	    { LDEB(pgClick->pgTopPosition.lpPage); return 1;	}

	if  ( docLayoutSelectedRoot( &(dc->dcLayoutContext), dc->dcTree,
					rootNodeFound, bodySectNode,
					pgClick->pgTopPosition.lpPage,
					pgClick->pgTopPosition.lpColumn ) )
	    { LDEB(pgClick->pgTopPosition.lpPage); return 1;	}

	guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );
	}

    *pSameRoot= sameRoot;
    *pSameInstance= sameInstance;
    *pDocX= docX;
    *pSeq= seq;
    *pKeyState= keyState;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to use the click event for navigation (follow link etc)		*/
/*									*/
/************************************************************************/

static int tedUseClickForNavigation(
				EditDocument *			ed,
				const DraggingContext *		dc,
				unsigned int			keyState )
    {
    int				rval= 0;
    HyperlinkField		hf;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    docInitHyperlinkField( &hf );

    if  ( ed->edFileReadOnly || ( keyState & KEY_CONTROL_MASK ) )
	{
	struct DocumentField *		dfHyperlink;
	struct DocumentField *		dfChftn;

	dfHyperlink= docFindTypedFieldForPosition( bd,
				&(dc->dcAnchorPosition), DOCfkHYPERLINK, 0 );

	if  ( dfHyperlink )
	    {
	    if  ( ! docGetHyperlinkField( &hf, dfHyperlink ) )
		{
		tedDocFollowLink( (APP_WIDGET)0, ed,
					    &(hf.hfFile), &(hf.hfBookmark) );
		rval= 1; goto ready;
		}
	    }

	dfChftn= docFindTypedFieldForPosition( bd,
				&(dc->dcAnchorPosition), DOCfkCHFTN, 0 );
	if  ( dfChftn )
	    {
	    tedGotoNoteOther(ed, dfChftn, &(dc->dcAnchorPosition) );
	    rval= 1; goto ready;
	    }
	}

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Select the current position of the mouse.				*/
/*									*/
/************************************************************************/

static int tedSelectMousePosition(
				unsigned int *			pKeyState,
				APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				docX= 0;

    int				sameRoot= 1;
    int				sameInstance= 1;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				seq;
    unsigned int		keyState= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PositionGeometry		pgClick;
    DocumentSelection		dsWord;
    int				wordIsObject= 0;
    int				afterObject= 0;

    if  ( tedGetSelection( &dsOld, &sg, &sd,
		    (struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return 1;	}

    if  ( tedFindMousePosition( w, ed, dc, &dsOld, &sameRoot, &sameInstance,
				&pgClick, &docX, &seq, &keyState, downEvent ) )
	{ return 1;	}

    docWordSelection( &dsWord, &wordIsObject, &afterObject,
						&(dc->dcAnchorPosition) );
    if  ( seq > 1 )
	{
	tedHandleMultiClick( ed, &dsWord,
				    &(dc->dcAnchorPosition), &pgClick, seq );
	goto ready;
	}

    if  ( sameRoot && sameInstance && ( keyState & KEY_SHIFT_MASK ) )
	{
	dc->dcAnchorPosition= dsOld.dsAnchor;

	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1); return 1;	}

	goto ready;
	}

    if  ( tedUseClickForNavigation( ed, dc, keyState ) )
	{ return 1;	}

    docAvoidParaHeadField( &(dc->dcAnchorPosition), (int *)0, bd );

    if  ( wordIsObject							&&
	  ! tedSelectObject( ed, &(dc->dcLayoutContext), 
				&dsWord, &pgClick, afterObject, docX )	)
	{ goto ready;	}

    if  ( ! docIsIBarSelection( &dsOld )				||
	  ! docSamePosition( &(dsOld.dsHead), &(dc->dcAnchorPosition) )	||
	  ! sameInstance						)
	{
	tedSetSelectedPosition( ed, &(dc->dcAnchorPosition),
				    pgClick.pgPositionFlags & POSflagLINE_HEAD,
				    &scrolledX, &scrolledY );
	}

  ready:

    tedDescribeSelection( ed );
    tedAdaptToolsToSelection( ed );

    *pKeyState= keyState;
    return 0;
    }

#   define	TED_DRAG_INTERVAL	(150L)

static APP_EVENT_HANDLER_H( tedInputDragMouseMove, w, vdc, event )
    {
    DraggingContext *	dc= (DraggingContext *)vdc;
    EditDocument *	ed= dc->dcEd;

    int			ox= ed->edVisibleRect.drX0;
    int			oy= ed->edVisibleRect.drY0;

    int			mouseX;
    int			mouseY;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &mouseX, &mouseY, w, event ) )
	{ return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    if  ( dc->dcMouseX != mouseX || dc->dcMouseY != mouseY )
	{
	dc->dcMouseX= mouseX;
	dc->dcMouseY= mouseY;

	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1); return;	}
	}

    return;
    }

static void tedTick(	void *		voiddc,
			int		mouseX,
			int		mouseY )
    {
    DraggingContext *		dc= (DraggingContext *)voiddc;
    EditDocument *		ed= dc->dcEd;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				scrolledX= 0;
    int				scrolledY= 0;

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}
    if  ( mouseX > ed->edFullRect.drX1- ox )
	{ mouseX=  ed->edFullRect.drX1- ox;	}
    if  ( mouseY > ed->edFullRect.drY1- oy )
	{ mouseY=  ed->edFullRect.drY1- oy;	}

    appScrollToPosition( &scrolledX, &scrolledY, ed, mouseX+ ox, mouseY+ oy );

    if  ( scrolledX || scrolledY )
	{
	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1);	}
	}

    return;
    }

static void tedButton1Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DraggingContext		dc;
    unsigned int		keyState;

    int				startedWithObject;

    guiFocusToWidget( ed->edDocumentWidget.dwWidget );

    if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	{ return;	}

    startedWithObject= td->tdSelectionDescription.sdIsObjectSelection;

    guiRunDragLoop( w, ed->edApplication->eaContext, downEvent,
				(APP_EVENT_HANDLER_T)0, tedInputDragMouseMove,
				TED_DRAG_INTERVAL, tedTick, (void *)&dc );

    tedDescribeSelection( ed );
    tedAdaptToolsToSelection( ed );

    if  ( startedWithObject					&&
	  ! td->tdSelectionDescription.sdIsObjectSelection	)
	{ tedHideObjectWindows( ed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse button down events for the document widget.		*/
/*									*/
/************************************************************************/

static void tedButton3Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				otherRoot= 0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    DraggingContext		dc;

    int				selectMousePosition= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentTree *	treeFound= (struct DocumentTree *)0;
    struct BufferItem *		rootNodeFound;
    struct BufferItem *		bodySectNodeFound;

    LayoutContext		lc;
    DocumentPosition		dpClick;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsOld, &sg, &sd,
		    (struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    if  ( tedFindPositionForCoordinates( &dpClick, (PositionGeometry *)0,
				&treeFound, &rootNodeFound, &bodySectNodeFound,
				ed, docX, docY ) )
	{ LLDEB(docX,docY); return;	}

    otherRoot= ! docSelectionSameRoot( &dsOld, rootNodeFound );

    if  ( otherRoot )
	{ selectMousePosition= 1;	}
    else{
	int			cmp1;
	int			cmp2;

	cmp1= docComparePositions( &dpClick, &(dsOld.dsHead) );
	cmp2= docComparePositions( &dpClick, &(dsOld.dsTail) );

	if  ( cmp1 < 0 || cmp2 > 0 )
	    { selectMousePosition= 1;	}
	}

    if  ( selectMousePosition )
	{
	if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	    { return;	}
	}

    tedAppShowFormatTool( td->tdToolsFormatToolOption, ed->edApplication );

    tedAdaptFormatToolToDocument( ed, 1 );

    return;
    }

APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    int			button= MOUSE_BUTTON_FROM_EVENT( downEvent );

    tedStopCursorBlink( ed );

    switch( button )
	{
	case MOUSE_BUTTON_1:
#	    if USE_MOTIF
	    if  ( ! ed->edIsReadonly		&&
		  downEvent->xbutton.subwindow	)
		{
		int	res= tedObjectDrag( w, ed, downEvent );

		if  ( res < 1 )
		    { return;	}
		}
#	    endif

#	    if USE_GTK
	    if  ( ! ed->edIsReadonly )
		{
		int	res= tedObjectDrag( w, ed, downEvent );

		if  ( res < 1 )
		    { return;	}
		}
#	    endif

	    tedButton1Pressed( w, ed, downEvent );
	    break;

	case MOUSE_BUTTON_2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;

	case MOUSE_BUTTON_3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

#	if USE_MOTIF
	case Button4:
	case Button5:
	    break;
#	endif

#	if USE_GTK
#	if GTK_MAJOR_VERSION < 2

	case SCROLL_UP:
	    appMouseWheelUp( ed );
	    break;

	case SCROLL_DOWN:
	    appMouseWheelDown( ed );
	    break;

#	endif
#	endif

	default:
	    LDEB(button);
	    break;
	}

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

