/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedLayout.h"
#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Defines for the key names. This keeps #ifdef's out of the actual	*/
/*  code.								*/
/*									*/
/************************************************************************/

# ifdef USE_MOTIF

#   include	<X11/keysym.h>

#  ifdef XK_ISO_Left_Tab
#   define KEY_ISO_Left_Tab	XK_ISO_Left_Tab
#  endif
#   define KEY_i		XK_i
#   define KEY_Tab		XK_Tab
#   define KEY_j		XK_j
#   define KEY_m		XK_m
#   define KEY_KP_Enter		XK_KP_Enter
#   define KEY_Return		XK_Return
#   define KEY_KP_Delete	XK_KP_Delete

#   ifdef __VMS
	/* VMS normally has only a "Delete" configured which is on the same
	 * location on the keyboard and serves the same purposes as the
	 * BackSpace on Unix. To simulate a BackSpace on VMS normally
	 * <CTRL>H is used.
	 */
#	define KEY_Delete	XK_BackSpace
#	define KEY_BackSpace	XK_Delete
#   else
#	define KEY_Delete	XK_Delete
#	define KEY_BackSpace	XK_BackSpace
#   endif

#   define KEY_KP_Home		XK_KP_Home
#   define KEY_Home		XK_Home
#   define KEY_KP_End		XK_KP_End
#   define KEY_End		XK_End
#   define KEY_KP_Left		XK_KP_Left
#   define KEY_Left		XK_Left
#   define KEY_KP_Right		XK_KP_Right
#   define KEY_Right		XK_Right
#   define KEY_KP_Up		XK_KP_Up
#   define KEY_Up		XK_Up
#   define KEY_KP_Down		XK_KP_Down
#   define KEY_Down		XK_Down
#   define KEY_KP_Prior		XK_KP_Prior
#   define KEY_Prior		XK_Prior
#   define KEY_KP_Next		XK_KP_Next
#   define KEY_Next		XK_Next
#   define KEY_c		XK_c
#   define KEY_x		XK_x
#   define KEY_v		XK_v
#   define KEY_y		XK_y
#   define KEY_Shift_L		XK_Shift_L
#   define KEY_Shift_R		XK_Shift_R
#   define KEY_Alt_L		XK_Alt_L
#   define KEY_Alt_R		XK_Alt_R
#   define KEY_Control_L	XK_Control_L
#   define KEY_Control_R	XK_Control_R
#   define KEY_Caps_Lock	XK_Caps_Lock
#   define KEY_Insert		XK_Insert
#   define KEY_KP_Insert	XK_KP_Insert
#   define KEY_Num_Lock		XK_Num_Lock
# endif

# ifdef USE_GTK

#   include	<gdk/gdkkeysyms.h>

#   define KEY_ISO_Left_Tab	GDK_ISO_Left_Tab
#   define KEY_i		GDK_i
#   define KEY_Tab		GDK_Tab
#   define KEY_j		GDK_j
#   define KEY_m		GDK_m
#   define KEY_KP_Enter		GDK_KP_Enter
#   define KEY_Return		GDK_Return
#   define KEY_KP_Delete	GDK_KP_Delete
#   define KEY_Delete		GDK_Delete
#   define KEY_BackSpace	GDK_BackSpace
#   define KEY_KP_Home		GDK_KP_Home
#   define KEY_Home		GDK_Home
#   define KEY_KP_End		GDK_KP_End
#   define KEY_End		GDK_End
#   define KEY_KP_Left		GDK_KP_Left
#   define KEY_Left		GDK_Left
#   define KEY_KP_Right		GDK_KP_Right
#   define KEY_Right		GDK_Right
#   define KEY_KP_Up		GDK_KP_Up
#   define KEY_Up		GDK_Up
#   define KEY_KP_Down		GDK_KP_Down
#   define KEY_Down		GDK_Down
#   define KEY_KP_Prior		GDK_KP_Prior
#   define KEY_Prior		GDK_Prior
#   define KEY_KP_Next		GDK_KP_Next
#   define KEY_Next		GDK_Next
#   define KEY_c		GDK_c
#   define KEY_x		GDK_x
#   define KEY_v		GDK_v
#   define KEY_y		GDK_y
#   define KEY_Shift_L		GDK_Shift_L
#   define KEY_Shift_R		GDK_Shift_R
#   define KEY_Alt_L		GDK_Alt_L
#   define KEY_Alt_R		GDK_Alt_R
#   define KEY_Control_L	GDK_Control_L
#   define KEY_Control_R	GDK_Control_R
#   define KEY_Caps_Lock	GDK_Caps_Lock
#   define KEY_Insert		GDK_Insert
#   define KEY_KP_Insert	GDK_KP_Insert
#   define KEY_Num_Lock		GDK_Num_Lock
# endif

/************************************************************************/
/*									*/
/*  Just log events that pass by for debugging purposes.		*/
/*									*/
/*  NOTE the silly constuction to do away with the 'unused' compiler	*/
/*	 warning.							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF

static void tedLogEvent(	Widget		w,
				void *		voided,
				XEvent *	event,
				Boolean *	pRefused )
    {
    EditDocument *		ed= (EditDocument *)voided;

    appDebug( "EVENT \"%s\": %s\n",
			ed->edTitle, APP_X11EventNames[event->type] );

    *pRefused= 1;

    if  ( ! event )
	{ return;	}
    if  ( ! event )
	{ tedLogEvent( w, voided, event, pRefused );	}
    }

#   endif

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

#   define	CLICK2_TO_CHANGE_ROOT	0

typedef struct DraggingContext
    {
    int				dcMouseX;
    int				dcMouseY;
    DocumentPosition		dcAnchorPosition;
    BufferItem *		dcRootItem;
    EditDocument *		dcEd;
    } DraggingContext;

static int tedSelectMousePosition(
				unsigned int *			pKeyState,
				APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				APP_EVENT *			downEvent )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    ScreenFontList *		sfl= &(td->tdScreenFontList);
    BufferDocument *		bd= td->tdDocument;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				page;
    int				sectNr;

    int				otherRoot= 0;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PositionGeometry		pgClick;

    ExternalItem *		eiSet= (ExternalItem *)0;
    BufferItem *		bodySectBiSet;
    BufferItem *		rootBiSet;

    if  ( tedGetSelection( &dsOld, &sg, &sd, td ) )
	{ LDEB(1); return 1;	}

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return 1;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    dc->dcRootItem= &(bd->bdItem);

    if  ( tedFindRootForPosition( &eiSet, &rootBiSet, &bodySectBiSet,
					    &sectNr, &page, ed, docX, docY ) )
	{ LLDEB(docX,docY); return 1;	}

    dc->dcRootItem= rootBiSet;

    otherRoot= docSelectionDifferentRoot( &dsOld, rootBiSet );

    if  ( otherRoot )
	{
	DocumentPosition		dp;
	int				inHeadFoot;
	DocumentRectangle		drChanged;

#	if  CLICK2_TO_CHANGE_ROOT
	if  ( seq < 2 )
	    { return 1;	}
#	endif

	docInitDocumentPosition( &dp );

	inHeadFoot= dc->dcRootItem->biInExternalItem;

	switch( inHeadFoot )
	    {
	    case DOCinBODY:
	    case DOCinFOOTNOTE:
	    case DOCinENDNOTE:
		break;

	    case DOCinSECT_HEADER:
	    case DOCinFIRST_HEADER:
	    case DOCinLEFT_HEADER:
	    case DOCinRIGHT_HEADER:
	    case DOCinSECT_FOOTER:
	    case DOCinFIRST_FOOTER:
	    case DOCinLEFT_FOOTER:
	    case DOCinRIGHT_FOOTER:

		if  ( ! eiSet )
		    { XDEB(eiSet); return 1;	}
		if  ( eiSet->eiPageFormattedFor != page )
		    {
		    if  ( docLayoutExternalItem( eiSet, &drChanged,
				page, eiSet->eiY0UsedTwips,
				bd, bodySectBiSet, add, sfl,
				tedInitLayoutExternalItem, tedCloseObject ) )
			{ LDEB(page); return 1; }

		    eiSet->eiPageSelectedUpon= page;
		    }
		break;

	    case DOCinFTNSEP:
	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
	    case DOCinAFTNSEP:
	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:

		if  ( ! eiSet )
		    { XDEB(eiSet); return 1;	}
		if  ( eiSet->eiPageFormattedFor != page )
		    {
		    LLDEB(eiSet->eiPageFormattedFor,page);

		    if  ( docLayoutExternalItem( eiSet, &drChanged,
				page, eiSet->eiY0UsedTwips,
				bd, bodySectBiSet, add, sfl,
				tedInitLayoutExternalItem, tedCloseObject ) )
			{ LDEB(page); return 1; }
		    }
		break;

	    default:
		LDEB(dc->dcRootItem->biInExternalItem);
	    }

	if  ( eiSet )
	    { eiSet->eiPageSelectedUpon= page;	}

#	if CLICK2_TO_CHANGE_ROOT
	{
	PositionGeometry	pg;

	if  ( tedFindPosition( &dp, &pg, dc->dcRootItem, add, docX, docY ) )
	    { LLDEB(docX,docY); return 1;	}

xxx
	tedSetSelectedPosition( ed, &dp, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	appExposeRectangle( add, 0, 0, 0, 0 );

	dc->dcEd= ed;
	dc->dcMouseX= mouseX;
	dc->dcMouseY= mouseY;

	*pKeyState= keyState;
	return 0;
	}
#	endif

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    if  ( seq > 1 )
	{
	int			wasObject= 0;

	DocumentSelection	dsNew;

	docInitDocumentSelection( &dsNew );

	if  ( tedFindPosition( &(dc->dcAnchorPosition), &pgClick,
				bd, dc->dcRootItem, add, sfl, docX, docY ) )
	    { LLDEB(docX,docY); return 1; }

	docSetIBarSelection( &dsNew, &(dc->dcAnchorPosition) );

	if  ( seq > 2 )
	    {
	    int		partLineBegin;
	    int		partLineEnd;

	    docLineSelection( &dsNew, &partLineBegin, &partLineEnd,
				dc->dcAnchorPosition.dpBi, pgClick.pgLine );
	    }
	else{
	    docWordSelection( &dsNew, &wasObject, &(dc->dcAnchorPosition) );
	    }

	tedSetSelection( ed, &dsNew, pgClick.pgAtLineHead,
						    &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	if  ( wasObject )
	    { tedMoveObjectWindows( ed );	}

	dc->dcEd= ed;
	dc->dcMouseX= mouseX;
	dc->dcMouseY= mouseY;

	*pKeyState= keyState;
	return 0;
	}

    if  ( tedFindPosition( &(dc->dcAnchorPosition), &pgClick,
				bd, dc->dcRootItem, add, sfl, docX, docY ) )
	{ LLDEB(docX,docY); return 1;	}

    if  ( ! otherRoot && ( keyState & KEY_SHIFT_MASK ) )
	{
	dc->dcAnchorPosition= dsOld.dsAnchor;

	if  ( tedExtendSelectionToXY( ed, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	    { LDEB(1); return 1;	}

	tedAdaptToolsToSelection( ed );

	dc->dcEd= ed;
	dc->dcMouseX= mouseX;
	dc->dcMouseY= mouseY;

	*pKeyState= keyState;
	return 0;
	}

    if  ( docParaHeadFieldKind( dc->dcAnchorPosition.dpBi, bd ) >= 0 )
	{
	int		FieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( ! docDelimitParaHeadField( &FieldNr, &partBegin, &partEnd,
					&stroffBegin, &stroffEnd,
					dc->dcAnchorPosition.dpBi, bd ) )
	    {
	    if  ( dc->dcAnchorPosition.dpStroff < stroffEnd )
		{ dc->dcAnchorPosition.dpStroff=  stroffEnd;	}
	    }
	}

    tedSetSelectedPosition( ed, &(dc->dcAnchorPosition), pgClick.pgAtLineHead,
						    &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    dc->dcEd= ed;
    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

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

    if  ( appGetCoordinatesFromMouseMoveEvent( &mouseX, &mouseY, w, event ) )
	{ return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    if  ( tedExtendSelectionToXY( dc->dcEd, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	{ LDEB(1); return;	}

    return;
    }

static APP_EVENT_HANDLER_H( tedInputDragMouseUp, w, vdc, event )
    {
    DraggingContext *		dc= (DraggingContext *)vdc;
    EditDocument *		ed= dc->dcEd;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    docDescribeSelection( &(td->tdSelectionDescription),
				&(td->tdDocumentSelection),
				bd, ed->edDocumentId, ed->edIsReadonly );

    tedAdaptToolsToSelection( ed );

    return;
    }

static APP_TIMER_HANDLER( tedTick, voiddc )
    {
    DraggingContext *		dc= (DraggingContext *)voiddc;
    EditDocument *		ed= dc->dcEd;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;

    int				scrolledX= 0;
    int				scrolledY= 0;

#   ifdef USE_MOTIF
    {
    Window		root;
    Window		child;
    int			rootX;
    int			rootY;
    int			winX;
    int			winY;
    unsigned int	mask;

    XQueryPointer( add->addDisplay, add->addDrawable,
						    &root, &child,
						    &rootX, &rootY,
						    &winX, &winY, &mask );
    mouseX= winX; mouseY= winY;
    }
#   endif

#   ifdef USE_GTK
    {
    gint		winX;
    gint		winY;
    GdkModifierType	mask;

    gdk_window_get_pointer( add->addDrawable, &winX, &winY, &mask );

    mouseX= winX; mouseY= winY;
    }
#   endif

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}
    if  ( mouseX > add->addBackRect.drX1- ox )
	{ mouseX=  add->addBackRect.drX1- ox;	}
    if  ( mouseY > add->addBackRect.drY1- oy )
	{ mouseY=  add->addBackRect.drY1- oy;	}

    appScrollToRectangle( ed, mouseX+ ox, mouseY+ oy,
					mouseX+ ox, mouseY+ oy,
					&scrolledX, &scrolledY );

    if  ( scrolledX || scrolledY )
	{
	if  ( tedExtendSelectionToXY( ed, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	    { LDEB(1);	}
	}

#   ifdef USE_MOTIF
    return;
#   endif
#   ifdef USE_GTK
    return 0;
#   endif
    }

static void tedButton1Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DraggingContext		dc;
    unsigned int		keyState;

    if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	{ return;	}

    if  ( ed->edFileReadOnly || ( keyState & KEY_CONTROL_MASK ) )
	{
	DocumentSelection	dsHyperlink;
	int			startPart;
	int			endPart;

	const char *		fileName;
	int			fileSize;
	const char *		markName;
	int			markSize;

	if  ( ! docGetHyperlinkForPosition( bd, &(dc.dcAnchorPosition),
				&dsHyperlink, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	    {
	    tedDocFollowLink( (APP_WIDGET)0, ed,
				    fileName, fileSize, markName, markSize );

	    return;
	    }
	}

    appRunDragLoop( w, ed->edApplication, downEvent,
				tedInputDragMouseUp,
				tedInputDragMouseMove,
				TED_DRAG_INTERVAL, tedTick,
				(void *)&dc );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle keyboard input.						*/
/*									*/
/*  a)  Handle miscelaneous keysyms as keysyms, even if they have a	*/
/*	string representation.						*/
/*									*/
/************************************************************************/

static void tedInputSetSelectedPosition(
				EditDocument *			ed,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    int			scrolledX= 0;
    int			scrolledY= 0;

    tedSetSelectedPosition( ed, dp, lastLine, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

static void tedInputExtendSelection(	EditDocument *			ed,
					const DocumentPosition *	dpSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    tedExtendSelectionToPosition( ed, &(ds.dsAnchor), dpSet );

    tedAdaptToolsToSelection( ed );

    return;
    }

static void tedInputChangeSelection(
				EditDocument *			ed,
				unsigned int			keyState,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    if  ( keyState & KEY_SHIFT_MASK )
	{
	tedInputExtendSelection( ed, dp );
	return;
	}

    tedInputSetSelectedPosition( ed, dp, lastLine );

    return;
    }

static void tedInputAvoidHeadField(	DocumentPosition *	dpNew,
					const BufferDocument *	bd )
    {
    int		fieldNr= -1;
    int		partBegin= -1;
    int		partEnd= -1;
    int		stroffBegin= -1;
    int		stroffEnd= -1;

    int	fieldKind= docParaHeadFieldKind( dpNew->dpBi, bd );
    if  ( fieldKind < 0 )
	{ return;	}

    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
				&stroffBegin, &stroffEnd, dpNew->dpBi, bd ) )
	{ LDEB(1);	}
    else{
	if  ( dpNew->dpStroff < stroffEnd )
	    { dpNew->dpStroff=  stroffEnd;	}
	}

    return;
    }

static int tedMoveRightOnKey(	DocumentPosition *	dpNew,
				const BufferDocument *	bd,
				int			keyState,
				const AppDrawingData *	add )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docNextWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docNextPosition( dpNew ) )
	    { return -1;	}
	}

    if  ( ! ( keyState & KEY_SHIFT_MASK ) )
	{ tedInputAvoidHeadField( dpNew, bd );	}

    return 0;
    }

static int tedMoveLeftOnKey(	DocumentPosition *	dpNew,
				const BufferDocument *	bd,
				int			keyState,
				const AppDrawingData *	add )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docPrevWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docPrevPosition( dpNew ) )
	    { return -1;	}
	}

    if  ( ! ( keyState & KEY_SHIFT_MASK )	&&
	  dpNew->dpBi->biParaListOverride > 0	)
	{
	int		fieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
					&stroffBegin, &stroffEnd,
					dpNew->dpBi, bd ) )
	    { LDEB(1);	}
	else{
	    if  ( dpNew->dpStroff < stroffEnd )
		{
		dpNew->dpStroff= 0;

		if  ( docPrevPosition( dpNew ) )
		    { return -1;	}
		}
	    }
	}

    return 0;
    }

static void tedProcessKeyEvent(	EditDocument *		ed,
				APP_EVENT *		event,
				APP_KEY_VALUE		keySym,
				unsigned char		scratch[40],
				int			gotString,
				int			gotKey,
				unsigned int		state )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const ScreenFontList *	sfl= &(td->tdScreenFontList);
    const AppDrawingData *	add= &(ed->edDrawingData);

    int				res;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( gotString > 0 )
	{
	DrawScreenFont *	apf;

	if  ( ! td->tdCanReplaceSelection )
	    { return;	}

	if  ( td->tdCurrentScreenFont < 0				||
	      td->tdCurrentScreenFont >=
				add->addScreenFontList.apflFontCount	)
	    {
	    LDEB(td->tdCurrentScreenFont);
	    LDEB(add->addScreenFontList.apflFontCount);
	    return;
	    }

	apf= add->addScreenFontList.apflFonts+ td->tdCurrentScreenFont;

	if  ( appCharExistsInFont( apf->apfFontStruct, scratch[0] ) )
	    {
	    tedDocReplaceSelection( ed, scratch, gotString );
	    return;
	    }
	}

    if  ( gotKey == 0 )
	{ return;	}

    switch( keySym )
	{
	DocumentPosition		dpNew;
	const PositionGeometry *	pgRef;

#	ifdef KEY_ISO_Left_Tab
	case  KEY_ISO_Left_Tab:
	    if  ( ds.dsBegin.dpBi			&&
		  ds.dsBegin.dpBi->biParaInTable	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{ goto shiftTab;	}
	    else{ return;		}
#	endif

	case KEY_i:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_Tab:
	    if  ( ds.dsBegin.dpBi			&&
		  ds.dsBegin.dpBi->biParaInTable	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{
		if  ( state & KEY_SHIFT_MASK )
		    {
		  shiftTab:
		    if  ( docFirstPosition( &dpNew,
					ds.dsBegin.dpBi->biParent )	||
			  docPrevPosition( &dpNew )			)
			{ return;	}
		    }
		else{
		    if  ( docLastPosition( &dpNew,
					ds.dsBegin.dpBi->biParent )	||
			  docNextPosition( &dpNew )			)
			{ return;	}
		    }

		{
		    const int	lastLine= 0;

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		}

		return;
		}

	    if  ( state & KEY_SHIFT_MASK )
		{ return;	}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    tedEditReplaceSelectionWithTab( ed );

	    return;

	case KEY_j: case KEY_m:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_KP_Enter:
	case KEY_Return:
	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    tedSplitParagraph( ed,
			    state == KEY_CONTROL_MASK	&&
			    keySym != KEY_j		&&
			    keySym != KEY_m		);

	    appDocumentChanged( ed, 1 );

	    return;

	case KEY_KP_Delete:
	case KEY_Delete:

	    res= tedDeleteTableSliceSelection( ed );
	    if  ( res < 0 )
		{ LDEB(res); return;	}
	    if  ( res == 0 )
		{ return;		}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsBegin;

		if  ( tedMoveRightOnKey( &dpNew, bd, state, add ) )
		    { return;	}

		if  ( ! docPositionsInsideParagraph( &(ds.dsBegin), &dpNew ) )
		    { return;	}

		if  ( tedMoveRightOnKey( &(ds.dsEnd), bd, state, add ) )
		    { return;	}

		    {
		    int		scrolledX= 0;
		    int		scrolledY= 0;

		    tedSetSelection( ed, &ds, sg.sgBegin.pgAtLineHead,
						    &scrolledX, &scrolledY );
		    }
		}

	    tedDocReplaceSelection( ed, (const unsigned char *)0, 0 );

	    return;

	case KEY_BackSpace:

	    res= tedDeleteTableSliceSelection( ed );
	    if  ( res < 0 )
		{ LDEB(res); return;	}
	    if  ( res == 0 )
		{ return;		}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsBegin;

		if  ( tedMoveLeftOnKey( &dpNew, bd, state, add ) )
		    { return;	}

		if  ( ! docPositionsInsideCell( &dpNew, &(ds.dsBegin) ) )
		    {
		    const int		keyState= 0;
		    const int		lastLine= 1;

		    tedInputChangeSelection( ed, keyState, &dpNew, lastLine );
		    return;
		    }

		if  ( tedMoveLeftOnKey( &(ds.dsBegin), bd, state, add ) )
		    { return;	}

		    {
		    int		scrolledX= 0;
		    int		scrolledY= 0;

		    tedSetSelection( ed, &ds, sg.sgBegin.pgAtLineHead,
						    &scrolledX, &scrolledY );
		    }
		}

	    tedDocReplaceSelection( ed, (const unsigned char *)0, 0 );
	    return;

	case KEY_KP_Home:
	case KEY_Home:
	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{
		dpNew= ds.dsEnd;

		if  ( docBeginOfLine( &dpNew, sg.sgEnd.pgAtLineHead ) )
		    { return;	}
		}
	    else{
		dpNew= ds.dsBegin;

		if  ( docBeginOfLine( &dpNew, sg.sgBegin.pgAtLineHead ) )
		    { return;	}
		}

	    tedInputAvoidHeadField( &dpNew, bd );

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_End:
	case KEY_End:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{
		dpNew= ds.dsEnd;

		if  ( docEndOfLine( &dpNew, sg.sgEnd.pgAtLineHead ) )
		    { return;	}
		}
	    else{
		dpNew= ds.dsBegin;

		if  ( docEndOfLine( &dpNew, sg.sgBegin.pgAtLineHead ) )
		    { return;	}
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Left:
	case KEY_Left:
	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( state != 0 || sd.sdIsIBarSelection )
		{
		if  ( tedMoveLeftOnKey( &dpNew, bd, state, add ) )
		    { return;	}
		}

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Right:
	case KEY_Right:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( state != 0 || sd.sdIsIBarSelection )
		{
		if  ( tedMoveRightOnKey( &dpNew, bd, state, add ) )
		    { return;	}
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Up:
	case KEY_Up:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgEnd); dpNew= ds.dsEnd;		}
	    else{ pgRef= &(sg.sgBegin); dpNew= ds.dsBegin;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff == 0 )
		    {
		    dpNew.dpBi= docPrevParagraph( dpNew.dpBi );
		    if  ( ! dpNew.dpBi )
			{ return;	}

		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		else{
		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		}
	    else{
		if  ( tedArrowUp( &dpNew, pgRef, bd, add, sfl ) )
		    { return;	}
		}

	    {
	    const int	lastLine= sg.sgBegin.pgAtLineHead;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Down:
	case KEY_Down:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgEnd); dpNew= ds.dsEnd;		}
	    else{ pgRef= &(sg.sgBegin); dpNew= ds.dsBegin;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff < dpNew.dpBi->biParaStrlen		&&
		      ( state & KEY_SHIFT_MASK )			)
		    {
		    if  ( docParaEnd( &dpNew ) )
			{ return;	}
		    }
		else{
		    dpNew.dpBi= docNextParagraph( dpNew.dpBi );
		    if  ( ! dpNew.dpBi )
			{ return;	}

		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		}
	    else{
		if  ( tedArrowDown( &dpNew, pgRef, bd, add, sfl ) )
		    { return;	}
		}

	    {
	    const int	lastLine= sg.sgBegin.pgAtLineHead;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Prior:
	case KEY_Prior:
	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( dpNew.dpBi->biInExternalItem != DOCinBODY )
		{
		if  ( ! docPrevSimilarRoot( &dpNew, bd ) )
		    {
		    const int	lastLine= 0;

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		return;
		}

	    if  ( docPrevPosition( &dpNew )			&&
		  docFirstDocumentPosition( &dpNew, bd )	)
		{ return;	}

	    {
		PositionGeometry	pg;
		TextLine *		tl;
		const int		lastLine= 1;
		int			partNew;

		int			page;

		tedPositionGeometry( &pg, &dpNew, 0, bd, add, sfl );

		tl= dpNew.dpBi->biParaLines+ pg.pgLine;

		page= tl->tlTopPosition.lpPage;
		while( page >= 0 )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &partNew, bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); return;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state, &dpNew, lastLine );
			return;
			}

		    page--;
		    }
	    }
	    return;

	case KEY_KP_Next:
	case KEY_Next:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( dpNew.dpBi->biInExternalItem != DOCinBODY )
		{
		if  ( ! docNextSimilarRoot( &dpNew, bd ) )
		    {
		    const int	lastLine= 0;

		    tedInputAvoidHeadField( &dpNew, bd );

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		return;
		}

	    {
		PositionGeometry	pg;
		TextLine *		tl;
		const int		lastLine= 1;
		int			partNew;

		int			page;

		tedPositionGeometry( &pg, &dpNew, lastLine, bd, add, sfl );

		tl= dpNew.dpBi->biParaLines+ pg.pgLine;

		page= tl->tlTopPosition.lpPage+ 1;
		while( page < bd->bdItem.biBelowPosition.lpPage )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &partNew, bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); return;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state, &dpNew, lastLine );
			return;
			}

		    page++;
		    }
	    }
	    return;

#	if 0
	case KEY_c:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    tedDocCopy( ed, (XEvent *)keyEvent );
	    return;
	case KEY_x:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    tedDocCut( ed, (XEvent *)keyEvent );
	    return;
	case KEY_v:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    appDocAskForPaste( ed, "PRIMARY", keyEvent->time );
	    return;
#	endif

#	if 0
	case KEY_y:
	    docListItem(0,ds.dsBegin.dpBi);
	    break;
#	endif

	case KEY_Shift_L:
	case KEY_Shift_R:
	case KEY_Alt_L:
	case KEY_Alt_R:
	case KEY_Control_L:
	case KEY_Control_R:
	case KEY_Caps_Lock:
	case KEY_Insert:
	case KEY_KP_Insert:
	case KEY_Num_Lock:
	    return;

	default: unknown:
#	    ifdef USE_GTK
#		if GTK_MAJOR_VERSION < 2
		gtk_accel_group_activate( ed->edToplevel.atAccelGroup,
							    keySym, state );
#		else
		gtk_accel_groups_activate(
			    G_OBJECT( ed->edToplevel.atAccelGroup ),
							    keySym, state );
#		endif
#	    endif

#	    ifdef APP_DEBUG
#	    ifdef USE_MOTIF
	    appDebug( "INPUT \"%s\": %s (%s%s%s%s%s%s%s%s<Key>%s) \"%.*s\"\n",
			ed->edTitle, APP_X11EventNames[event->type],
			(state&KEY_SHIFT_MASK)?"Shift":"",
			(state&KEY_CONTROL_MASK)?"Ctrl":"",
			(state&LockMask)?"Lock":"",
			(state&Mod1Mask)?"Mod1":"",
			(state&Mod2Mask)?"Mod2":"",
			(state&Mod3Mask)?"Mod3":"",
			(state&Mod4Mask)?"Mod4":"",
			(state&Mod5Mask)?"Mod5":"",
			XKeysymToString( keySym ), gotString, scratch );
#	    endif
#	    endif
	}

    return;
    }

APP_EVENT_HANDLER_H( tedKeyPressed, w, voided, keyEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    APP_KEY_VALUE		keySym;
    unsigned char		scratch[40];
    int				gotString;
    int				gotKey;
    unsigned int		state;

    if  ( ! tedHasSelection( td ) )
	{
#	ifdef USE_MOTIF
	const AppDrawingData *	add= &(ed->edDrawingData);

	XBell( add->addDisplay, 0 );
#	endif

#	ifdef USE_GTK
	gdk_beep();
#	endif

	return;
	}

    appGuiGetStringFromKeyboardEvent( ed->edInputContext, w, keyEvent,
				&gotString, &gotKey, &state,
				scratch, sizeof(scratch), &keySym );

    tedStopCursorBlink( ed );

    if  ( gotString > 0 || gotKey > 0 )
	{
	tedProcessKeyEvent( ed, keyEvent,
				keySym, scratch, gotString, gotKey, state );
	}

    if  ( tedHasIBarSelection( td ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Drag routine for inserted objects.					*/
/*									*/
/************************************************************************/

typedef struct TedObjectDrag
    {
    EditDocument *	todEd;

    InsertedObject *	todIo;
    DocumentPosition	todDp;
    PositionGeometry	todPg;
    int			todParticule;

    int			todMouseX;
    int			todMouseY;

    int			todRight;
    int			todBottom;
    } TedObjectDrag;

static void tedObjectHandleMove(	TedObjectDrag *		tod,
					int			mouseX,
					int			mouseY )
    {
    EditDocument *		ed= tod->todEd;
    AppDrawingData *		add= &(ed->edDrawingData);
    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    const BufferItem *		bi;
    const TextParticule *	tp;
    const TextLine *		tl;

    int				moved= 0;

    int				wide= tod->todIo->ioDragWide;
    int				high= tod->todIo->ioDragHigh;

    bi= tod->todDp.dpBi;
    tp= bi->biParaParticules+ tod->todParticule;
    tl= bi->biParaLines+ tod->todPg.pgLine;

    if  ( tod->todRight && tod->todMouseX != mouseX )
	{ tod->todIo->ioDragWide += mouseX- tod->todMouseX; moved= 1; }

    if  ( tod->todBottom && tod->todMouseY != mouseY )
	{ tod->todIo->ioDragHigh += mouseY- tod->todMouseY; moved= 1; }

    if  ( wide < tod->todIo->ioDragWide )
	{ wide=  tod->todIo->ioDragWide;	}
    if  ( high < tod->todIo->ioDragHigh )
	{ high=  tod->todIo->ioDragHigh;	}

    if  ( moved )
	{
	int		baseline= TL_BASE_PIXELS( add, tl );
	int		x= tp->tpX0- ox;
	int		y= baseline- tod->todIo->ioPixelsHigh- oy;

	baseline= TL_BASE_PIXELS( add, tl );

	appExposeRectangle( add, x, y, wide, high );
	}

    tod->todMouseX= mouseX;
    tod->todMouseY= mouseY;
    }

static APP_EVENT_HANDLER_H( tedObjectDragMouseMove, w, vtod, event )
    {
    TedObjectDrag *		tod= (TedObjectDrag *)vtod;

    int				x;
    int				y;

    if  ( appGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    tedObjectHandleMove( tod, x, y );

    return;
    }

static APP_EVENT_HANDLER_H( tedObjectDragMouseUp, w, vtod, event )
    {
    TedObjectDrag *		tod= (TedObjectDrag *)vtod;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent( &x, &y,
			    &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    tedObjectHandleMove( tod, x, y );

    return;
    }

static int tedObjectDrag(	APP_WIDGET	w,
				EditDocument *	ed,
				APP_EVENT *	downEvent )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const ScreenFontList *	sfl= &(td->tdScreenFontList);
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				wide;
    int				high;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    int				x;
    int				y;

    const int			lastOne= 1;
    TedObjectDrag		tod;

    tod.todEd= ed;

    tod.todBottom= tod.todRight= 0;

    if  ( appGetCoordinatesFromMouseButtonEvent(
				    &(tod.todMouseX), &(tod.todMouseY),
				    &button, &upDown, &seq, &keyState,
				    w, downEvent ) )
	{ LDEB(1); return -1;	}

    docInitDocumentPosition( &(tod.todDp) );
    if  ( tedGetObjectSelection( td,
			&(tod.todParticule), &(tod.todDp), &(tod.todIo) ) )
	{ return 2;	}

    tedPositionGeometry( &(tod.todPg), &(tod.todDp), lastOne, bd, add, sfl );

    wide= tod.todIo->ioPixelsWide;
    high= tod.todIo->ioPixelsHigh;
    x= tod.todPg.pgXPixels;
    y= tod.todPg.pgBaselinePixels;

    if  ( seq > 1					||
	  upDown < 1					||
	  tod.todMouseX+ ox < x				||
	  tod.todMouseX+ ox > x+ wide			||
	  tod.todMouseY+ oy > y				||
	  tod.todMouseY+ oy < y- high			)
	{ return 2;	}

    if  ( tod.todMouseX+ ox >= x + wide/ 2- RESIZE_BLOCK/ 2	&&
	  tod.todMouseX+ ox <  x + wide/ 2+ RESIZE_BLOCK/ 2	&&
	  tod.todMouseY+ oy >= y- RESIZE_BLOCK		)
	{ tod.todBottom= 1;	}

    if  ( tod.todMouseX+ ox >= x + wide- RESIZE_BLOCK		&&
	  tod.todMouseY+ oy >= y - high/ 2- RESIZE_BLOCK/ 2 	&&
	  tod.todMouseY+ oy <  y - high/ 2+ RESIZE_BLOCK/ 2	)
	{ tod.todRight= 1;	}

    if  ( tod.todMouseX+ ox >= x + wide- RESIZE_BLOCK		&&
	  tod.todMouseY+ oy >= y- RESIZE_BLOCK			)
	{ tod.todRight= 1; tod.todBottom= 1;	}

    if  ( ! tod.todRight && ! tod.todBottom )
	{ return 1;	}

    tod.todIo->ioDragWide= tod.todIo->ioPixelsWide;
    tod.todIo->ioDragHigh= tod.todIo->ioPixelsHigh;

    appRunDragLoop( w, ea, downEvent,
				tedObjectDragMouseUp,
				tedObjectDragMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&tod );

    if  ( tod.todIo->ioDragWide != tod.todIo->ioPixelsWide	||
	  tod.todIo->ioDragHigh != tod.todIo->ioPixelsHigh	)
	{
	double			xfac= add->addMagnifiedPixelsPerTwip;

	if  ( tod.todIo->ioDragWide != tod.todIo->ioPixelsWide )
	    {
	    TextParticule *	tp;

	    tod.todIo->ioScaleX= ( 100.0* tod.todIo->ioDragWide )/
			    TWIPStoPIXELS( xfac, tod.todIo->ioTwipsWide );

	    tod.todIo->ioPixelsWide= TWIPStoPIXELS( xfac,
		    ( tod.todIo->ioScaleX* tod.todIo->ioTwipsWide )/ 100 );

	    tp= tod.todDp.dpBi->biParaParticules+ tod.todParticule;

	    tp->tpPixelsWide= tod.todIo->ioPixelsWide;
	    }

	if  ( tod.todIo->ioDragHigh != tod.todIo->ioPixelsHigh )
	    {
	    tod.todIo->ioScaleY= ( 100.0* tod.todIo->ioDragHigh )/
			    TWIPStoPIXELS( xfac, tod.todIo->ioTwipsHigh );

	    tod.todIo->ioPixelsHigh= TWIPStoPIXELS( xfac,
		    ( tod.todIo->ioScaleY* tod.todIo->ioTwipsHigh )/ 100 );
	    }

	if  ( tedResizeObject( ed, tod.todParticule,
					    &(tod.todDp), &(tod.todPg) ) )
	    { LDEB(1);	}

	tedPositionGeometry( &(tod.todPg), &(tod.todDp), lastOne,
							    bd, add, sfl );

	appDocumentChanged( ed, 1 );
	}

    tod.todIo->ioDragWide= 0;
    tod.todIo->ioDragHigh= 0;

    ox= ed->edVisibleRect.drX0;
    oy= ed->edVisibleRect.drY0;

    tedSetObjectWindows( ed, &(tod.todPg), tod.todIo, ox, oy );

    return 0;
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
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    DraggingContext	dc;
    unsigned int	keyState;

    if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	{ return;	}

    tedShowFormatTool( td->tdToolsFormatToolOption, ed->edApplication );

    tedAdaptFormatToolToDocument( ed, 1 );

    return;
    }

APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

#   ifdef USE_MOTIF
    switch( downEvent->xbutton.button )
	{
	case Button1:
	    if  ( ! ed->edFileReadOnly		&&
		  downEvent->xbutton.subwindow	)
		{
		tedObjectDrag( w, ed, downEvent );
		return;
		}

	    tedButton1Pressed( w, ed, downEvent );
	    break;
	case Button2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;
	case Button3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

	case Button4:
	    appMouseWheelUp( ed );
	    break;

	case Button5:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LDEB(downEvent->xbutton.button);
	    break;
	}
#   endif

#   ifdef USE_GTK
    switch( downEvent->button.button )
	{
	int	res;

	case 1:
	    if  ( ! ed->edFileReadOnly )
		{
		res= tedObjectDrag( w, ed, downEvent );
		if  ( res < 1 )
		    { return;	}
		}

	    tedButton1Pressed( w, ed, downEvent );
	    break;
	case 2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;
	case 3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

	case 4:
	    appMouseWheelUp( ed );
	    break;

	case 5:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LDEB(downEvent->button.button);
	    break;
	}
#   endif

    if  ( tedHasIBarSelection( td ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

