/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>

#   include	"appInspector.h"
#   include	"appInspectorSubject.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Turn an inspector or one of its subjects on or off.			*/
/*									*/
/************************************************************************/

void appEnableInspector(	AppInspector *	ai,
				int		enabled )
    {
    guiEnableWidget( ai->aiPaned, enabled != 0 );

    return;
    }

void appEnableInspectorSubject(		AppInspector *		ai,
					int			subject ,
					int			enabled )
    {
    appInspectorEnablePage( ai, subject, enabled );

    ai->aiSubjects[subject].isEnabled= enabled != 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Programmatically pull a particular subject of the inspector to	*/
/*  front.								*/
/*									*/
/************************************************************************/

void appInspectorSelectSubject(	AppInspector *		ai,
				int			subject )
    {
    const int	andMenu= 1;

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;
    }

/************************************************************************/
/*									*/
/*  Callback when an inspector page is chosen by the user.		*/
/*									*/
/************************************************************************/

void appInspectorPageChosen(		int		subject,
					void *		vai )
    {
    AppInspector *	ai= (AppInspector *)vai;

    const int		andMenu= 0;

    if  ( subject >= ai->aiSubjectCount )
	{ LDEB(ai->aiSubjectCount); return;	}

    if  ( ai->aiNotifySubject )
	{ (*ai->aiNotifySubject)( ai, ai->aiCurrentSubject, subject ); }

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;

    return;
    }

void appInspectorInitializeFlags(	int *		enabled,
					int *		prefs,
					int		subjectCount )
    {
    int		subject;

    for ( subject= 0; subject < subjectCount; subject++ )
	{
	enabled[subject]= 1;
	prefs[subject]= 5;
	}

    return;
    }

void appInspectorApplyFlags(		AppInspector *	ai,
					int *		enabled,
					int *		prefs,
					int		subjectCount,
					int		choosePage )
    {
    int		subject;

    int		preferred= -1;
    int		highest= -1;

    for ( subject= 0; subject < subjectCount; subject++ )
	{
	appEnableInspectorSubject( ai, subject, enabled[subject] != 0 );

	if  ( enabled[subject] && prefs[subject] > highest )
	    { preferred= subject; highest= prefs[subject]; }
	}

    if  ( preferred >= 0 )
	{
	if  ( ai->aiCurrentSubject < 0				||
	      choosePage						||
	      ! ai->aiSubjects[ai->aiCurrentSubject].isEnabled	)
	    { appInspectorSelectSubject( ai, preferred );	}
	}

    return;
    }
