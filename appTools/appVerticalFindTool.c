/************************************************************************/
/*									*/
/*  A find/replace tool. Build a vertical instance			*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	"appFindTool.h"
#   include	"appInspectorSubject.h"
#   include	"appFindToolImpl.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a row of buttons for the Find Tool.				*/
/*									*/
/************************************************************************/

static void appFindToolMakeButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					FindTool *		ft )
    {
    APP_WIDGET		row;

    APP_WIDGET		leftButton;
    APP_WIDGET		rightButton;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;
    const int		colspan= 1;

    row= guiMakeRowInColumn( parent, 2, heightResizable );

    guiMakeButtonInRow( &(leftButton), row, leftLabel,
		    leftCallback, (void *)ft, 0, colspan, showAsDefault );
    guiMakeButtonInRow( &(rightButton), row, rightLabel,
		    rightCallback, (void *)ft, 1, colspan, showAsDefault );

#   if USE_MOTIF
    XtVaSetValues( XtParent( parent ),
			XmNdefaultButton,	leftButton,
			NULL );
#   endif


    *pLeftButton= leftButton, *pRightButton= rightButton; return;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the find Tool.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appFindMakeFindFrame(	APP_WIDGET			parent,
					const FindToolResources *	ftr,
					const InspectorSubjectResources * isr,
					FindTool *			ft )
    {
    APP_WIDGET		frame;
    APP_WIDGET		paned;
    APP_WIDGET		row;

    const int		textEnabled= 1;
    const int		heightResizable= 0;

    guiMakeColumnFrameInColumn( &frame, &paned, parent, ftr->ftrFindTitle );

    guiMakeTextInColumn( &(ft->ftPatternText), paned, 0, textEnabled );

    guiSetTypingCallbackForText( ft->ftPatternText,
				    appFindToolPatternChanged, (void *)ft );

#   if USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	ft->ftPatternText,
			NULL );
#   endif

    /***************/

    row= guiMakeRowInColumn( paned, 2, heightResizable );
    ft->ftAsWordToggle= guiMakeToggleInRow( row, ftr->ftrAsWord,
				appFindAsWordToggled, (void *)ft, 0, 1 );
    ft->ftCaseSensitiveToggle= guiMakeToggleInRow( row, ftr->ftrCaseSensitive,
				appFindCaseSensitiveToggled, (void *)ft, 1, 1 );

    row= guiMakeRowInColumn( paned, 1, heightResizable );
    ft->ftRegexToggle= guiMakeToggleInRow( row, ftr->ftrUseRegex,
				appFindRegexToggled, (void *)ft, 0, 1 );

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(ft->ftFindNextButton), &(ft->ftFindPrevButton),
		isr->isrNextButtonText, isr->isrPrevButtonText,
		appFindToolFindNextPushed, appFindToolFindPrev,
		ft );

    /***************/

    return frame;
    }

/************************************************************************/
/*									*/
/*  Make the replace part of the find tool.				*/
/*									*/
/************************************************************************/

static void appFindMakeReplaceFrame(
					APP_WIDGET			parent,
					const FindToolResources *	ftr,
					FindTool *			ft )
    {
    APP_WIDGET		row;
    APP_WIDGET		paned;

    const int		textEnabled= 1;

    guiMakeColumnFrameInColumn( &(ft->ftReplaceParent), &paned,
					    parent, ftr->ftrReplaceTitle );

    guiMakeTextInColumn( &(ft->ftReplaceText), paned, 0, textEnabled );

    guiSetTypingCallbackForText( ft->ftReplaceText,
				appFindToolReplacementChanged, (void *)ft );

#   if USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	ft->ftReplaceText,
			NULL );
#   endif

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(ft->ftReplaceButton), &(ft->ftReplaceNextButton),
		ftr->ftrReplaceFound, ftr->ftrReplaceNext,
		appFindToolReplaceSelectionPushed, appFindToolReplaceNext,
		ft );

    }

void appFillVerticalFindTool(	FindTool *			ft,
				const FindToolResources *	ftr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr,
				FindToolFind			findNext,
				FindToolFind			findPrev,
				FindToolSetPattern		setPattern,
				FindToolReplace			replaceText,
				void *				target )
    {
    ft->ftInProgrammaticChange= 0;

    ft->ftFindParent= appFindMakeFindFrame( pageWidget, ftr, isr, ft );

    appFindMakeReplaceFrame( pageWidget, ftr, ft );

    ft->ftExplicitFocus= 0;
    if  ( ftr->ftrExplicitFocus				&&
	  ! strcmp( ftr->ftrExplicitFocus, "1" )	)
	{ ft->ftExplicitFocus= 1;	}

    appFindToolReflectPatternText( ft );
    appFindToolReflectReplaceText( ft );

    ft->ftInspector= ai;
    ft->ftApplication= ea;

    ft->ftFindNext= findNext;
    ft->ftFindPrev= findPrev;
    ft->ftSetPattern= setPattern;
    ft->ftReplace= replaceText;
    ft->ftTarget= target;

    return;
    }
