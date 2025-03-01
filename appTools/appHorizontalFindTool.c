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
#   include	<appEditApplication.h>
#   include	<guiNamedImages.h>

#   include	<appDebugon.h>

#   define	COLUMN_COUNT	10

#   define	TEXT_COLUMN	0
#   define	TEXT_COLSPAN	6
#   define	TEXT_CHARS_WIDE	15

#   define	BUT0_COLUMN	(TEXT_COLUMN+ TEXT_COLSPAN)
#   define	BUT0_COLSPAN	2

#   define	BUT1_COLUMN	(BUT0_COLUMN+ BUT0_COLSPAN)
#   define	BUT1_COLSPAN	2

#   define	TOG0_COLUMN	0
#   define	TOG0_COLSPAN	3

#   define	TOG1_COLUMN	(TOG0_COLUMN+ TOG0_COLSPAN)
#   define	TOG1_COLSPAN	3

#   define	TOG2_COLUMN	(TOG1_COLUMN+ TOG1_COLSPAN)
#   define	TOG2_COLSPAN	3

/************************************************************************/
/*									*/
/*  Make the 'Find' part of the find tool.				*/
/*									*/
/************************************************************************/

static void appFindMakeFindControls(	EditApplication *		ea,
					APP_WIDGET			findRow,
					const FindToolResources *	ftr,
					const InspectorSubjectResources * isr,
					FindTool *			ft )
    {
    const int		textEnabled= 1;

    guiMakeTextInRow( &(ft->ftPatternText), findRow,
				    TEXT_COLUMN, TEXT_COLSPAN,
				    TEXT_CHARS_WIDE, textEnabled );

    guiSetTypingCallbackForText( ft->ftPatternText,
				    appFindToolPatternChanged, (void *)ft );

#   if 0
    {
    const int		showAsDefault= 0;

    guiMakeButtonInRow( &(ft->ftFindNextButton), findRow,
		    isr->isrNextButtonText,
		    appFindToolFindNextPushed, (void *)ft,
		    BUT0_COLUMN, BUT0_COLSPAN, showAsDefault );

    guiMakeButtonInRow( &(ft->ftFindPrevButton), findRow,
		    isr->isrPrevButtonText,
		    appFindToolFindPrev, (void *)ft,
		    BUT1_COLUMN, BUT1_COLSPAN, showAsDefault );
    }
#   else

    {
    struct NamedImage *	niNext;
    struct NamedImage *	niPrev;

    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
		    "navigate_next", &niNext ) )
	{ LDEB(1); return;	}
    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
		    "navigate_before", &niPrev ) )
	{ LDEB(1); return;	}

    guiMakeImageButtonInRow( &(ft->ftFindNextButton), findRow,
		    niNext, isr->isrNextButtonText,
		    appFindToolFindNextPushed, (void *)ft,
		    BUT0_COLUMN, BUT0_COLSPAN );

    guiMakeImageButtonInRow( &(ft->ftFindPrevButton), findRow,
		    niPrev, isr->isrPrevButtonText,
		    appFindToolFindPrev, (void *)ft,
		    BUT1_COLUMN, BUT1_COLSPAN );
    }

#   endif
    }

static void appFindMakeFindToggles(
				APP_WIDGET			toggleRow,
				const FindToolResources *	ftr,
				const InspectorSubjectResources * isr,
				FindTool *			ft )
    {
    ft->ftAsWordToggle= guiMakeToggleInRow( toggleRow, ftr->ftrAsWord,
				appFindAsWordToggled, (void *)ft,
				TOG0_COLUMN, TOG0_COLSPAN );

    ft->ftCaseSensitiveToggle= guiMakeToggleInRow( toggleRow,
				ftr->ftrCaseSensitive,
				appFindCaseSensitiveToggled, (void *)ft,
				TOG1_COLUMN, TOG1_COLSPAN );

    ft->ftRegexToggle= guiMakeToggleInRow( toggleRow, ftr->ftrUseRegex,
				appFindRegexToggled, (void *)ft,
				TOG2_COLUMN, TOG2_COLSPAN );

    }

/************************************************************************/
/*									*/
/*  Make the replace part of the find tool.				*/
/*									*/
/************************************************************************/

static void appFindMakeReplaceControls(
				EditApplication *		ea,
				APP_WIDGET			replaceRow,
				const FindToolResources *	ftr,
				FindTool *			ft )
    {
    const int		textEnabled= 1;

    guiMakeTextInRow( &(ft->ftReplaceText), replaceRow,
				    TEXT_COLUMN, TEXT_COLSPAN,
				    TEXT_CHARS_WIDE, textEnabled );

    guiSetTypingCallbackForText( ft->ftReplaceText,
				appFindToolReplacementChanged, (void *)ft );

    /***************/

#   if 0
    {
    const int		showAsDefault= 0;

    guiMakeButtonInRow( &(ft->ftReplaceButton), replaceRow,
		    ftr->ftrReplaceFound,
		    appFindToolReplaceSelectionPushed, (void *)ft,
		    BUT0_COLUMN, BUT0_COLSPAN, showAsDefault );

    guiMakeButtonInRow( &(ft->ftReplaceNextButton), replaceRow,
		    ftr->ftrReplaceNext,
		    appFindToolReplaceNext, (void *)ft,
		    BUT1_COLUMN, BUT1_COLSPAN, showAsDefault );
    }
#   else
    {
    struct NamedImage *	niReplace;
    struct NamedImage *	niReplaceFind;

    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
		    "replace", &niReplace ) )
	{ LDEB(1); return;	}
    if  ( guiGetNamedImage( ea->eaNamedImages, ea->eaNamedImageCount,
		    "replace_find", &niReplaceFind ) )
	{ LDEB(1); return;	}

    guiMakeImageButtonInRow( &(ft->ftReplaceButton), replaceRow,
		    niReplace, ftr->ftrReplaceFound,
		    appFindToolReplaceSelectionPushed, (void *)ft,
		    BUT0_COLUMN, BUT0_COLSPAN );

    guiMakeImageButtonInRow( &(ft->ftReplaceNextButton), replaceRow,
		    niReplaceFind, ftr->ftrReplaceNext,
		    appFindToolReplaceNext, (void *)ft,
		    BUT1_COLUMN, BUT1_COLSPAN );
    }
#   endif

    }

void appFillHorizontalFindTool(	
				FindTool *			ft,
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
    const int		heightResizable= 0;

    APP_WIDGET		toggleRow;

    ft->ftInProgrammaticChange= 0;

    ft->ftFindParent= guiMakeRowInColumn( pageWidget,
					    COLUMN_COUNT, heightResizable );

    appFindMakeFindControls( ea, ft->ftFindParent, ftr, isr, ft );

    toggleRow= guiMakeRowInColumn( pageWidget,
					    COLUMN_COUNT, heightResizable );
    appFindMakeFindToggles( toggleRow, ftr, isr, ft );

    ft->ftReplaceParent= guiMakeRowInColumn( pageWidget,
					    COLUMN_COUNT, heightResizable );

    appFindMakeReplaceControls( ea, ft->ftReplaceParent, ftr, ft );

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
