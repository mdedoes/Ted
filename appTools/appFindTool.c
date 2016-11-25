/************************************************************************/
/*									*/
/*  A find/replace tool.						*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>

#   include	"appFindTool.h"
#   include	"appInspectorSubject.h"
#   include	<appGuiResource.h>
#   include	"appFindToolImpl.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Turn buttons on and off depending on the situation.			*/
/*									*/
/************************************************************************/

void appFindToolReflectPatternText(	FindTool *	ft )
    {
    int		sensitive;
    char *	pattern;

    ft->ftInProgrammaticChange++;

    pattern= guiStringFromText( ft->ftPatternText );

    sensitive= pattern[0] != '\0';

    guiFreeStringFromTextWidget( pattern );

    guiEnableWidget( ft->ftFindNextButton, sensitive );
    guiEnableWidget( ft->ftFindPrevButton, sensitive );

    ft->ftInProgrammaticChange--;

    return;
    }

void appFindToolReflectReplaceText(	FindTool *	ft )
    {
    int		sensitive;
    char *	replacement;

    ft->ftInProgrammaticChange++;

    replacement= guiStringFromText( ft->ftReplaceText );

    sensitive= ( replacement[0] != '\0' || ft->ftUseRegex )	&&
		 ft->ftLastFindResult == 0;

    guiFreeStringFromTextWidget( replacement );

    guiEnableWidget( ft->ftReplaceButton, sensitive );
    guiEnableWidget( ft->ftReplaceNextButton, sensitive );

    ft->ftInProgrammaticChange--;

    return;
    }

static void appFindToolSetFindResult(	FindTool *	ft,
					int		result )
    {
    int		sensitive;

    ft->ftInProgrammaticChange++;

    ft->ftLastFindResult= result;

    if  ( result )
	{ sensitive= 0;	}
    else{
	char *	replacement;

	guiEnableWidget( ft->ftReplaceParent, ft->ftReplaceEnabled );
	guiEnableText( ft->ftReplaceText, ft->ftReplaceEnabled );

	replacement= guiStringFromText( ft->ftReplaceText );

	sensitive= ( replacement[0] != '\0' || ft->ftUseRegex );

	guiFreeStringFromTextWidget( replacement );
	}

    guiEnableWidget( ft->ftReplaceButton, sensitive );
    guiEnableWidget( ft->ftReplaceNextButton, sensitive );

    ft->ftInProgrammaticChange--;

    return;
    }

APP_TXTYPING_CALLBACK_H( appFindToolPatternChanged, w, voidaft )
    {
    FindTool *	ft= (FindTool *)voidaft;

    if  ( ft->ftInProgrammaticChange )
	{ return;	}

    appFindToolReflectPatternText( ft );

    return;
    }

APP_TXTYPING_CALLBACK_H( appFindToolReplacementChanged, w, voidaft )
    {
    FindTool *	ft= (FindTool *)voidaft;

    if  ( ft->ftInProgrammaticChange )
	{ return;	}

    appFindToolReflectReplaceText( ft );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find' button has been pushed.					*/
/*									*/
/************************************************************************/

static int appFindToolSetProgram(	FindTool *	ft )
    {
    char *		pattern;
    int			res;

    if  ( ! ft->ftSetPattern || ! ft->ftTarget )
	{ XXDEB(ft->ftSetPattern,ft->ftTarget); return -1;	}

    pattern= guiStringFromText( ft->ftPatternText );

    res= (*ft->ftSetPattern)( ft->ftTarget, pattern,
						    ft->ftUseRegex,
						    ft->ftAsWord,
						    ft->ftCaseSensitive );

    guiFreeStringFromTextWidget( pattern );

    return res;
    }

static void appFindToolFindNext(	FindTool *	ft )
    {
    int			result;

    if  ( ! ft->ftFindNext || ! ft->ftTarget )
	{
	XXDEB(ft->ftFindNext,ft->ftTarget);
	appFindToolSetFindResult( ft, -1 ); return;
	}

    if  ( appFindToolSetProgram( ft ) )
	{ appFindToolSetFindResult( ft, -1 ); return; }

    result= (*ft->ftFindNext)( ft->ftTarget );
    appFindToolSetFindResult( ft, result );

    return;
    }

APP_BUTTON_CALLBACK_H( appFindToolFindNextPushed, w, voidft )
    {
    FindTool *	ft= (FindTool *)voidft;

    appFindToolFindNext( ft );

    return;
    }

APP_BUTTON_CALLBACK_H( appFindToolFindPrev, w, voidft )
    {
    FindTool *	ft= (FindTool *)voidft;
    int			result;

    if  ( ! ft->ftFindPrev || ! ft->ftTarget )
	{
	XXDEB(ft->ftFindPrev,ft->ftTarget);
	appFindToolSetFindResult( ft, -1 ); return;
	}

    if  ( appFindToolSetProgram( ft ) )
	{ appFindToolSetFindResult( ft, -1 ); return; }

    result= (*ft->ftFindPrev)( ft->ftTarget );
    appFindToolSetFindResult( ft, result );

    return;
    }

/************************************************************************/
/*									*/
/*  'Replace' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appFindToolReplaceSelection( FindTool *	ft )
    {
    char *		replacement;

    if  ( ! ft->ftReplace || ! ft->ftTarget )
	{ XXDEB(ft->ftReplace,ft->ftTarget); return;	}

    replacement= guiStringFromText( ft->ftReplaceText );

    (*ft->ftReplace)( ft->ftTarget, replacement );

    guiFreeStringFromTextWidget( replacement );

    guiEnableWidget( ft->ftReplaceButton, 0 );

    return;
    }

APP_BUTTON_CALLBACK_H( appFindToolReplaceSelectionPushed, w, voidft )
    {
    FindTool *	ft= (FindTool *)voidft;

    appFindToolReplaceSelection( ft );
    }

APP_BUTTON_CALLBACK_H( appFindToolReplaceNext, w, voidft )
    {
    FindTool *	ft= (FindTool *)voidft;

    appFindToolReplaceSelection( ft );

    appFindToolFindNext( ft );

    return;
    }

static void appFindToolRefreshOptionToggles(	FindTool *	ft )
    {
    ft->ftInProgrammaticChange++;

    guiSetToggleState( ft->ftAsWordToggle, ft->ftAsWord );
    guiSetToggleState( ft->ftCaseSensitiveToggle, ft->ftCaseSensitive );
    guiSetToggleState( ft->ftRegexToggle, ft->ftUseRegex );

    ft->ftInProgrammaticChange--;
    }

APP_TOGGLE_CALLBACK_H( appFindAsWordToggled, w, voidaft, voidtbcs )
    {
    FindTool *		ft= (FindTool *)voidaft;
    int			set;

    if  ( ft->ftInProgrammaticChange )
	{ return;	}

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    ft->ftAsWord= ( set != 0 );

    appFindToolReflectReplaceText( ft );

    if  ( ft->ftAsWord && ft->ftUseRegex )
	{
	ft->ftUseRegex= 0;
	appFindToolRefreshOptionToggles( ft );
	}

    return;
    }

APP_TOGGLE_CALLBACK_H( appFindCaseSensitiveToggled, w, voidaft, voidtbcs )
    {
    FindTool *		ft= (FindTool *)voidaft;
    int			set;

    if  ( ft->ftInProgrammaticChange )
	{ return;	}

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    ft->ftCaseSensitive= ( set != 0 );

    appFindToolReflectReplaceText( ft );

    return;
    }

APP_TOGGLE_CALLBACK_H( appFindRegexToggled, w, voidaft, voidtbcs )
    {
    FindTool *		ft= (FindTool *)voidaft;
    int			set;

    if  ( ft->ftInProgrammaticChange )
	{ return;	}

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    ft->ftUseRegex= ( set != 0 );

    appFindToolReflectReplaceText( ft );

    if  ( ft->ftAsWord && ft->ftUseRegex )
	{
	ft->ftAsWord= 0;
	appFindToolRefreshOptionToggles( ft );
	}

    return;
    }

void appFindToolEnableReplace(	FindTool *	ft,
				int		enabled )
    {
    ft->ftReplaceEnabled= enabled;

    guiEnableWidget( ft->ftReplaceParent, ft->ftReplaceEnabled );
    guiEnableText( ft->ftReplaceText, ft->ftReplaceEnabled );

    return;
    }

void appFindToolSetPattern(	FindTool *		ft,
				const char *		pattern,
				int			useRegex )
    {
    ft->ftAsWord= 0;
    ft->ftCaseSensitive= 1;
    ft->ftUseRegex= useRegex;

    ft->ftInProgrammaticChange++;

    guiStringToText( ft->ftPatternText, (const char *)pattern );

    appFindToolRefreshOptionToggles( ft );

    appFindToolReflectPatternText( ft );

    ft->ftInProgrammaticChange--;

    return;
    }

void appFindToolFocusToPattern(	FindTool *		ft )
    {
    guiFocusToWidget( ft->ftPatternText );
    }

void appCleanFindTool(		FindTool *		ft )
    {
    return;
    }

void appInitFindTool(		FindTool *		ft )
    {
    ft->ftApplication= (struct EditApplication *)0;
    ft->ftInspector= (struct AppInspector *)0;
    ft->ftSubjectPage= -1;

    ft->ftInProgrammaticChange= 0;

    ft->ftFindParent= (APP_WIDGET)0;
    ft->ftPatternText= (APP_WIDGET)0;
    ft->ftFindNextButton= (APP_WIDGET)0;
    ft->ftFindPrevButton= (APP_WIDGET)0;

    ft->ftReplaceParent= (APP_WIDGET)0;
    ft->ftReplaceText= (APP_WIDGET)0;
    ft->ftReplaceButton= (APP_WIDGET)0;
    ft->ftReplaceNextButton= (APP_WIDGET)0;

    ft->ftAsWordToggle= (APP_WIDGET)0;
    ft->ftCaseSensitiveToggle= (APP_WIDGET)0;
    ft->ftRegexToggle= (APP_WIDGET)0;

    ft->ftFindNext= (FindToolFind)0;
    ft->ftFindPrev= (FindToolFind)0;
    ft->ftReplace= (FindToolReplace)0;
    ft->ftSetPattern= (FindToolSetPattern)0;
    ft->ftTarget= (void *)0;

    ft->ftReplaceEnabled= 1;
    ft->ftLastFindResult= 1;

    ft->ftUseRegex= 0;
    ft->ftAsWord= 0;
    ft->ftCaseSensitive= 0;

    ft->ftExplicitFocus= 0;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_FindToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "findToolFindTitle",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Find" ),
    APP_RESOURCE( "findToolFindNext",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Find" ),
    APP_RESOURCE( "findToolFindPrevious",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous" ),
    };

static AppConfigurableResource APP_FindToolResourceTable[]=
    {
    APP_RESOURCE( "findToolFindTitle",
		offsetof(FindToolResources,ftrFindTitle),
		"Find" ),

    APP_RESOURCE( "findToolAsWord",
		offsetof(FindToolResources,ftrAsWord),
		"As Word" ),
    APP_RESOURCE( "findToolCaseSensitive",
		offsetof(FindToolResources,ftrCaseSensitive),
		"Case Sensitive" ),
    APP_RESOURCE( "findToolUseRegex",
		offsetof(FindToolResources,ftrUseRegex),
		"Regular Expression" ),

    APP_RESOURCE( "findToolReplaceTitle",
		offsetof(FindToolResources,ftrReplaceTitle),
		"Replace" ),

    APP_RESOURCE( "findToolReplaceFound",
		offsetof(FindToolResources,ftrReplaceFound),
		"Replace" ),
    APP_RESOURCE( "findToolReplaceNext",
		offsetof(FindToolResources,ftrReplaceNext),
		"Replace, Next" ),

    APP_RESOURCE( "findToolExplicitFocus",
		offsetof(FindToolResources,ftrExplicitFocus),
		"1" ),
    };

void appFindToolGetResourceTable(	struct EditApplication *	ea,
					FindToolResources *		spr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)spr,
					APP_FindToolResourceTable,
					sizeof(APP_FindToolResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_FindToolSubjectResourceTable,
				sizeof(APP_FindToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
