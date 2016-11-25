/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>

#   include	"appInspectorSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize an inspector subject.					*/
/*									*/
/************************************************************************/

void appInitInspectorSubject(	InspectorSubject *	is )
    {
    is->isPage= (APP_WIDGET)0;
    is->isPageParent= (APP_WIDGET)0;
    is->isTool= (void *)0;
    is->isTarget= (void *)0;
    is->isEnabled= 1;

    is->isNextPrevRow= (APP_WIDGET)0;
    is->isPrevButton= (APP_WIDGET)0;
    is->isNextButton= (APP_WIDGET)0;

    is->isMoveUpButton= (APP_WIDGET)0;
    is->isMoveDownButton= (APP_WIDGET)0;

    is->isSelectDeleteRow= (APP_WIDGET)0;
    is->isSelectButton= (APP_WIDGET)0;
    is->isDeleteButton= (APP_WIDGET)0;

    is->isInsertAppendRow= (APP_WIDGET)0;
    is->isInsertButton= (APP_WIDGET)0;
    is->isAppendButton= (APP_WIDGET)0;

    is->isApplyRow= (APP_WIDGET)0;
    is->isRevertButton= (APP_WIDGET)0;
    is->isApplyButton= (APP_WIDGET)0;

    is->isSubjectType= (const struct InspectorSubjectType *)0;
    }

