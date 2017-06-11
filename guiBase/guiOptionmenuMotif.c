#   include	"guiBaseConfig.h"

#   if USE_MOTIF

#   include	<Xm/RowColumn.h>
#   include	<Xm/PushBG.h>

#   include	"guiOptionmenu.h"

#   include	<appDebugon.h>

#   define	RECLAIM_FOCUS	0

/************************************************************************/
/*									*/
/*  Get around Motif's ridiculous resize behavior.			*/
/*									*/
/************************************************************************/

static void guiOptionmenuSetWidthMotif(	Widget		menu,
					int		newWidth )
    {
    Dimension		parentMarginWidth= 0;

    Widget		pulldown;
    Widget		buttonGadget;

    XtWidgetGeometry	xwg;

    /*
    SDEB(XtClass(menu)->core_class.class_name);
    */

    XtVaGetValues( menu,
			XmNsubMenuId,		&pulldown,
			NULL );

    XtVaGetValues( XtParent( pulldown ),
			XmNmarginWidth,		&parentMarginWidth,
			NULL );

    if  ( newWidth < 2* parentMarginWidth )
	{ LLDEB(newWidth,parentMarginWidth); return;	}

    XtVaSetValues( pulldown,
			XmNwidth,		newWidth-
						2* parentMarginWidth,
			NULL );

    buttonGadget= XmOptionButtonGadget( menu );

    XtQueryGeometry( buttonGadget, NULL, &xwg );

    XtResizeWidget( buttonGadget, newWidth, xwg.height, xwg.border_width );

    return;
    }

void guiOptionmenuRefreshWidth(	AppOptionmenu *		aom )
    {
    Dimension		width;

    if  ( ! aom->aomInplace )
	{ XDEB(aom->aomInplace); return;	}

    XtVaGetValues( aom->aomInplace,
			    XmNwidth,		&width,
			    NULL );

    guiOptionmenuSetWidthMotif( aom->aomInplace, width );

    return;
    }

/************************************************************************/
/*									*/
/*  Dispatch the callback for an option menu and..			*/
/*									*/
/*  Reclaim focus that drifted away.					*/
/*  This is to work around a bug in the Xfce window manager (Oct 2004).	*/
/*									*/
/************************************************************************/

static void guiOptionMenuMotifCallback(	Widget			w,
					void *			voidaom,
					void *			pbcs )
    {
    AppOptionmenu *	aom= (AppOptionmenu *)voidaom;
    short		idx= -1;

    XtVaGetValues( w,	XmNpositionIndex,	&idx,
			NULL );
    if  ( idx < 0 )
	{ LDEB(idx); return;	}

    if  ( aom->aomCallback )
	{ (*aom->aomCallback)( idx, aom->aomTarget ); }

#   if RECLAIM_FOCUS
    guiMotifSetFocusToWindow( aom->aomInplace );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Make a pulldown that is usable as a child of a form.		*/
/*									*/
/************************************************************************/

static void guiOptionMenuConfigureMotif(	Widget		w,
						void *		through,
						XEvent *	event,
						Boolean *	pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    guiOptionmenuSetWidthMotif( w, cevent->width );

    *pRefused= 1;

    return;
    }

void guiFinishOptionmenuMotif(	Widget		menu,
				Widget		pulldown )
    {
    Widget			labelGadget;
    Widget			buttonGadget;

    XtAddEventHandler( menu, StructureNotifyMask, False,
				    guiOptionMenuConfigureMotif, (void *)0 );

    labelGadget= XmOptionLabelGadget( menu );
    buttonGadget= XmOptionButtonGadget( menu );

    XtUnmanageChild( labelGadget );
    XtManageChild( buttonGadget );

    XtVaSetValues( buttonGadget,
			XmNalignment,		XmALIGNMENT_BEGINNING,
			XmNmarginHeight,	1,
			NULL );

    return;
    }

void guiMakeOptionmenuInColumn(	AppOptionmenu *		aom,
				APP_WIDGET		column,
				OptionmenuCallback	callBack,
				void *			target )
    {
    Widget			pulldown;
    Widget			inplace;

    Arg				al[20];
    int				ac= 0;

    guiInitOptionmenu( aom );

    ac= 0;
    /*
    No! is counter productive..
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    */
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    /*
    Does not make any difference..
    XtSetArg( al[ac], XmNallowShellResize,	False ); ac++;
    */

    pulldown= XmCreatePulldownMenu( column, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNsubMenuId,		pulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNentryBorder,		0 ); ac++;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,		True ); ac++;

    inplace= XmCreateOptionMenu( column, WIDGET_NAME, al, ac );

    guiFinishOptionmenuMotif( inplace, pulldown );

    XtManageChild( inplace );

    guiMotifTurnOfSashTraversal( column );

    aom->aomPulldown= pulldown;
    aom->aomInplace= inplace;
    aom->aomCallback= callBack;
    aom->aomTarget= target;
    }

void guiMakeOptionmenuInRow(		AppOptionmenu *		aom,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					OptionmenuCallback	callBack,
					void *			target )
    {
    Widget			pulldown;
    Widget			inplace;

    Arg				al[20];
    int				ac= 0;

    guiInitOptionmenu( aom );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    /*
    No! is counter productive..
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    */

    pulldown= XmCreatePulldownMenu( row, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    XtSetArg( al[ac], XmNsubMenuId,		pulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNentryBorder,		0 ); ac++;

    inplace= XmCreateOptionMenu( row, WIDGET_NAME, al, ac );

    guiFinishOptionmenuMotif( inplace, pulldown );

    XtManageChild( inplace );

    aom->aomPulldown= pulldown;
    aom->aomInplace= inplace;
    aom->aomCallback= callBack;
    aom->aomTarget= target;

    return;
    }

/************************************************************************/
/*									*/
/*  Add an option to a pulldown.					*/
/*									*/
/************************************************************************/

void guiAddItemToOptionmenu(	AppOptionmenu *		aom,
				const char *		label )
    {
    Widget		fresh;

    WidgetList		children;
    Cardinal		childCount= 0;

    Arg			al[20];
    int			ac= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    XtSetArg( al[ac], XmNmarginHeight,		1 ); ac++;

    if  ( aom->aomOptionsVisible >= childCount )
	{
	fresh= XmCreatePushButtonGadget( aom->aomPulldown, (char *)label,
						    al,	ac );
	}
    else{
	XmString	labelString;

	fresh= children[aom->aomOptionsVisible];

	labelString= XmStringCreateLocalized( (char *)label );

	XtVaSetValues( fresh,
			XmNlabelString,	labelString,
			NULL );

	XmStringFree( labelString );
	}

    if  ( aom->aomCallback )
	{
	XtAddCallback( fresh, XmNactivateCallback,
				    guiOptionMenuMotifCallback, (void *)aom );
	}

#   if RECLAIM_FOCUS
    XtAddCallback( fresh, XmNactivateCallback,
				    appRefocusOptionmenuMotif, (void *)aom );
#   endif

    XtManageChild( fresh );

    aom->aomOptionsVisible++;

    return;
    }

void guiDeleteItemFromOptionmenu(	AppOptionmenu *		aom,
					int			pos )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( pos >= 0 && pos < (int)childCount )
	{
	XtDestroyWidget( children[pos] );
	aom->aomOptionsVisible--;
	return;
	}

    LLDEB(childCount,pos);
    }

/************************************************************************/

void guiOptionMenuEnablePosition( AppOptionmenu *	aom,
				int			pos,
				int			enabled )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( pos >= 0 && pos < (int)childCount )
	{
	XtSetSensitive( children[pos], enabled );
	return;
	}

    LLDEB(childCount,pos);
    }

/************************************************************************/
/*									*/
/*  Select a particular option in an option menu.			*/
/*									*/
/************************************************************************/

void guiSetOptionmenu(	AppOptionmenu *	aom,
			int		pos )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( pos >= 0 && pos < (int)childCount )
	{
	XtVaSetValues( aom->aomInplace,
			XmNmenuHistory,	children[pos],
			NULL );
	}
    else{
	XtVaSetValues( aom->aomInplace,
			XmNmenuHistory,	(Widget)0,
			NULL );
	}

    return;
    }

void guiEmptyOptionmenu(	AppOptionmenu *		aom )
    {
    WidgetList		children;
    Cardinal		childCount;
    int			child;

    XtVaSetValues( aom->aomInplace,
		    XmNmenuHistory,	(Widget)0,
		    NULL );

    /*  Does not work!
    guiEmptyParentWidget( aom->aomPulldown );
    */

    XtVaGetValues( aom->aomPulldown,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    if  ( aom->aomOptionsVisible > childCount )
	{
	LLDEB(aom->aomOptionsVisible,childCount);
	aom->aomOptionsVisible= childCount;
	}

    for ( child= 0; child < aom->aomOptionsVisible; child++ )
	{
	XtRemoveAllCallbacks( children[child], XmNactivateCallback );

#	if 0
	Count callbacks to make sure that only one exists:

	XtCallbackList	callBacks= (XtCallbackList)0;

	XtVaGetValues( children[child],
			XmNactivateCallback,	&callBacks,
			NULL );
	if  ( callBacks )
	    {
	    int		n= 0;

	    while( callBacks[n].callback && callBacks[n].closure )
		{ n++;	}
	    LLDEB(child,n);
	    }
#	endif
	}

    /*  1  */
    XtUnmanageChildren( children, childCount );
    aom->aomOptionsVisible= 0;
    }

void guiEnableOptionmenu(	AppOptionmenu *		aom,
				int			sensitive )
    {
    XtSetSensitive( aom->aomInplace, sensitive != 0 );
    }

void guiInitOptionmenu(		AppOptionmenu *		aom )
    {
    aom->aomPulldown= (APP_WIDGET)0;
    aom->aomInplace= (APP_WIDGET)0;

    aom->aomCallback= (OptionmenuCallback)0;
    aom->aomTarget= (void *)0;

    aom->aomOptionsVisible= 0;
    }

#   endif
