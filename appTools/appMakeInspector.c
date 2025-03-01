/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stdlib.h>
#   include	<stddef.h>

#   include	"appInspector.h"
#   include	"appInspectorSubject.h"
#   include	<guiWidgets.h>
#   include	<appGuiResource.h>
#   include	<appGuiApplication.h>
#   include	<guiToolUtil.h>
#   include	"appVerticalTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Init/Clean an inspector.						*/
/*									*/
/************************************************************************/

static void appInitInspector(		AppInspector *		ai )
    {
    ai->aiApplication= (struct EditApplication *)0;

    ai->aiResources= (const struct InspectorResources *)0;

    ai->aiTopWidget= (APP_WIDGET)0;
    ai->aiPaned= (APP_WIDGET)0;

    guiInitOptionmenu( &(ai->aiSubjectOptionmenu) );

    ai->aiSeparator= (APP_WIDGET)0;
    ai->aiPageParent= (APP_WIDGET)0;
    ai->aiSeparator2= (APP_WIDGET)0;
    ai->aiCloseButton= (APP_WIDGET)0;

    ai->aiNotifySubject= (InspectorNotifySubject)0;

    ai->aiRgbChooser= (void *)0; /* RgbChooser */
    ai->aiRgbSubjectNumber= -1;

    ai->aiSubjects= (InspectorSubject *)0;
    ai->aiSubjectCount= 0;

    ai->aiDestroy= (AppToolDestroy)0;
    ai->aiTarget= (void *)0;

    ai->aiSubjectCount= -1;
    ai->aiCurrentSubject= -1;
    ai->aiSubjects= (InspectorSubject *)0;

    return;
    }

static void appCleanInspector(		AppInspector *		ai )
    {
    int			i;

    InspectorSubject *	is= ai->aiSubjects;
    for ( i= 0; i < ai->aiSubjectCount; is++, i++ )
	{
	const InspectorSubjectType *	ist= is->isSubjectType;

	if  ( is->isTool && ist->istFreeTool )
	    { (*ist->istFreeTool)( is->isTool );	}
	}

    if  ( ai->aiDestroy )
	{ (*ai->aiDestroy)( ai->aiTarget );	}

    if  ( ai->aiSubjects )
	{ free( ai->aiSubjects );	}

    return;
    }

/************************************************************************/
/*									*/
/*  An inspector tool must be destroyed. (Destroy callback)		*/
/*									*/
/************************************************************************/

static void appDestroyInspector(	AppInspector *	ai )
    {
    appCleanInspector( ai );

    if  ( ai->aiTopWidget )
	{ guiDestroyShellWidget( ai->aiTopWidget );	}

    free( ai );

    return;
    }

static APP_CLOSE_CALLBACK_H( appInspectorCloseCall, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

/************************************************************************/
/*									*/
/*  Allocate subjects and make the connection to the subject types.	*/
/*									*/
/************************************************************************/

static int appInspectorAllocateSubjects(
				AppInspector *			ai,
				void *				target,
				int				subjectCount,
				const InspectorSubjectType *	ist )
    {
    if  ( subjectCount > 0 )
	{
	int			subject;
	InspectorSubject *	is;

	ai->aiSubjects= malloc( subjectCount* sizeof(InspectorSubject) );
	if  ( ! ai->aiSubjects )
	    { LXDEB(subjectCount,ai->aiSubjects); return -1;	}

	is= ai->aiSubjects;
	for ( subject= 0; subject < subjectCount; is++, subject++ )
	    {
	    appInitInspectorSubject( is );
	    is->isSubjectType= ist+ subject;
	    is->isTarget= target;
	    }

	ai->aiSubjectCount= subjectCount;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the pages,							*/
/*  Fill the menu for the pages.					*/
/*									*/
/************************************************************************/

static void appInspectorToolMakeSubjectPages(
				AppInspector *			ai )
    {
    int				subject;
    InspectorSubject *		is;

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	{
	const InspectorSubjectType *		ist= is->isSubjectType;
	const InspectorSubjectResources *	isr= ist->istResources;

	if  ( appMakeVerticalInspectorPage( &(is->isPage), ai,
							isr->isrSubjectName ) )
	    { LDEB(subject);	}

	is->isEnabled= 1;
	}
    }

/************************************************************************/
/*									*/
/*  Add a subject to an inspector.					*/
/*									*/
/************************************************************************/

int appInspectorToolAddSubject(	AppInspector *				ai,
				const InspectorSubjectResources *	isr )
    {
    InspectorSubject *	is;

    is= realloc( ai->aiSubjects,
			( ai->aiSubjectCount+ 1 )* sizeof(InspectorSubject) );
    if  ( ! is )
	{ LXDEB(ai->aiSubjectCount,is); return -1;	}
    ai->aiSubjects= is;

    is += ai->aiSubjectCount;
    if  ( appMakeVerticalInspectorPage( &(is->isPage),
						    ai, isr->isrSubjectName ) )
	{ LDEB(1); return -1;	}

    is->isEnabled= 1;
    ai->aiSubjectCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the standard subject related controls for a subject		*/
/*									*/
/************************************************************************/

static int appInspectorMakeSubjectControls(
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget )
    {
    const InspectorSubjectType *	ist= is->isSubjectType;
    const InspectorSubjectResources *   isr= ist->istResources;

    if  ( ist->istMoveToPrev || ist->istMoveToNext )
	{
	guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    ist->istMoveToPrev, ist->istMoveToNext, is->isTool );
	}
	
    if  ( ist->istSelectInstance || ist->istDeleteInstance )
	{
	guiToolMake2BottonRow( &(is->isSelectDeleteRow), pageWidget,
			&(is->isSelectButton), &(is->isDeleteButton),
			isr->isrSelectButtonText, isr->isrDeleteButtonText,
			ist->istSelectInstance, ist->istDeleteInstance,
			is->isTool );
	}

    if  ( ist->istInsertInstance || ist->istAppendInstance )
	{
	guiToolMake2BottonRow( &(is->isInsertAppendRow), pageWidget,
			&(is->isInsertButton), &(is->isAppendButton),
			isr->isrInsertButtonText, isr->isrAppendButtonText,
			ist->istInsertInstance, ist->istAppendInstance,
			is->isTool );
	}

    if  ( ist->istRevertChanges || ist->istApplyChanges )
	{
	guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    ist->istRevertChanges, ist->istApplyChanges,
		    is->isTool );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Called when the user switches to another page of the format tool.	*/
/*									*/
/************************************************************************/

static void appFormatToolNotifySubject(	struct AppInspector *	ai,
					int			subjectFrom,
					int			subjectTo )
    {
    if  ( subjectFrom == ai->aiRgbSubjectNumber	&&
	  subjectTo != ai->aiRgbSubjectNumber	)
	{
	const int	enabled= 0;

	appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, enabled );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Close callback for the inspector tool.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appCloseInspector, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

/************************************************************************/
/*									*/
/*  Build the inspector tool.						*/
/*									*/
/************************************************************************/

static int appInspectorMakeTool(	AppInspector *		ai,
					APP_WIDGET		option )
    {
    const int	userResizable= 0;

    const int	heightResizable= 0;
    const int	showAsDefault= 0;
    const int	colspan= 2;

    APP_WIDGET	row;

    const InspectorResources * air= ai->aiResources;

    appMakeVerticalTool( &(ai->aiTopWidget), &(ai->aiPaned), ai->aiApplication,
				userResizable,
				option, appInspectorCloseCall, (void *)ai );

    guiMakeOptionmenuInColumn( &(ai->aiSubjectOptionmenu), ai->aiPaned,
				    appInspectorPageChosen, (void *)ai );

    guiInsertSeparatorInColumn( &(ai->aiSeparator), ai->aiPaned );

    appInspectorMakePageParent( ai );

    appInspectorToolMakeSubjectPages( ai );

    guiInsertSeparatorInColumn( &(ai->aiSeparator2), ai->aiPaned );

    row= guiMakeRowInColumn( ai->aiPaned, 4, heightResizable );

    guiMakeButtonInRow( &(ai->aiCloseButton), row,
			air->airCloseText, appCloseInspector, (void *)ai,
			1, colspan, showAsDefault );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Resources for the inspector tool.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_InspectorResourceTable[]=
{
    APP_RESOURCE( "inspectorCloseInspector",
		offsetof(InspectorResources,airCloseText),
		"Close" ),
};

AppInspector * appMakeInspector( struct EditApplication *	ea,
				void *				target,
				const InspectorSubjectType *	ist,
				int				subjectCount )
    {
    AppInspector *		rval= (AppInspector *)0;
    AppInspector *		ai;

    static InspectorResources	air;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&air,
					APP_InspectorResourceTable,
					sizeof(APP_InspectorResourceTable)/
					sizeof(AppConfigurableResource) );

	gotResources= 1;
	}

    ai= (AppInspector *)malloc( sizeof(AppInspector) );
    if  ( ! ai )
	{ LXDEB(subjectCount,ai); goto ready;	}

    appInitInspector( ai );

    ai->aiResources= &air;

    if  ( appInspectorAllocateSubjects( ai, target, subjectCount, ist ) )
	{ LDEB(subjectCount); goto ready;	}

    ai->aiApplication= ea;
    rval= ai; ai= (AppInspector *)0; /* steal */

  ready:

    if  ( ai )
	{ appDestroyInspector( ai );	}

    return rval;
    }

int appInspectorBuildSubjectPage(	AppInspector *		ai,
					APP_WIDGET		toolWidget,
					int			subject )
    {
    InspectorSubject *			is= ai->aiSubjects+ subject;
    const InspectorSubjectType *	ist= is->isSubjectType;

    if  ( is->isTool )
	{ return 0;	}

    if  ( ! ist->istBuildTool )
	{ XDEB(ist->istBuildTool); return -1;	}

    is->isTool= (*ist->istBuildTool)( subject, ai, ai->aiApplication, is,
				    toolWidget, ist->istResources );

    if  ( ! is->isTool )
	{
	LSXDEB(subject,ist->istResources->isrSubjectName,is->isTool);
	return -1;
	}

    return 0;
    }

AppInspector * appMakeInspectorTool( struct EditApplication *	ea,
				    APP_WIDGET			option,
				    const InspectorSubjectType * subjectTypes,
				    int				subjectCount,
				    int				rgbSubject,
				    AppToolDestroy		destroy,
				    void *			through,
				    void *			target )
    {
    AppInspector *		rval= (AppInspector *)0;
    AppInspector *		ai;

    ai= appMakeInspector( ea, target, subjectTypes, subjectCount );
    if  ( ! ai )
	{ LXDEB(subjectCount,ai); goto ready;	}

    ai->aiDestroy= destroy;
    ai->aiTarget= through;

    appInspectorMakeTool( ai, option );

    {
    InspectorSubject *			is;
    int					subject;

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	{
	if  ( appInspectorBuildSubjectPage( ai, is->isPage, subject ) )
	    { LDEB(subject); goto ready;	}

	appInspectorMakeSubjectControls( is, is->isPage );
	}

    /**/
    if  ( rgbSubject >= 0 )
	{
	appInspectorSetRgbPage( ai,
			    ai->aiSubjects[rgbSubject].isTool, rgbSubject );
	}

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	{
	const InspectorSubjectType *	ist= is->isSubjectType;

	if  ( is->isTool		&&
	      ist->istFillChoosers	)
	    {
	    (*ist->istFillChoosers)( is->isTool );
	    }
	}

    appFinishInspectorTool( ai );

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	{
	const InspectorSubjectType *	ist= is->isSubjectType;

	if  ( is->isTool		&&
	      ist->istFinishPage	)
	    {
	    (*ist->istFinishPage)( is->isTool );
	    }
	}
    }

    ai->aiNotifySubject= appFormatToolNotifySubject;

    rval= ai; ai= (AppInspector *)0; /* steal */

  ready:

    if  ( ai )
	{ appDestroyInspector( ai );	}

    return rval;
    }

void appFinishInspectorTool(	AppInspector *		ai )
    {
    appShowShellWidget( ai->aiApplication, ai->aiTopWidget );

    guiOptionmenuRefreshWidth( &(ai->aiSubjectOptionmenu) );

    return;
    }

