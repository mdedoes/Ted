/************************************************************************/
/*									*/
/*  Application inspector with different subject pages.			*/
/*									*/
/************************************************************************/

#   ifndef	APP_INSPECTOR_SUBJECT_H
#   define	APP_INSPECTOR_SUBJECT_H

#   include	<guiBase.h>

struct RGB8Color;
struct InspectorSubjectResources;
struct InspectorSubject;
struct AppInspector;
struct EditApplication;

/**
 *  Function pointer that is passed to the RGB chooser page to 
 *  allow it to return the color that the user has selected.
 */
typedef void (*InspectorSubjectGotColor)(
			void *				through,
			int				property,
			const struct RGB8Color *	rgb8 );

/**
 *  Function pointer that is invoked to build a tool and to populate the 
 *  subject page with the necessary controls.
 *  The subject page widget itself is owned by the inspector or by 
 *  the document
 */
typedef void * (*SubjectBuildTool)(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

/**
 *  Function pointer to fill the choosers (dropdowns) on the page.
 *  This is a separate step in the construction of the page to cope 
 *  with the peculiarities of Motif.
 */
typedef void (*SubjectFillChoosers)( void *			tool );

/**
 *  Function pointer to finish the tool construction process. Again this 
 *  is a step to cope with Motifs peculiarities.
 */
typedef void (*SubjectFinishPage)( void *			tool );

/**
 *  Function to clean the tool: Free all kind of resources. The 
 *  inspector takes care of the destruction of the tools GUI widgets.
 */
typedef void (*SubjectFreeTool)(	void *			tool );

/************************************************************************/
/*									*/
/*  Facilities for subjects in inspectors.				*/
/*									*/
/************************************************************************/

typedef struct InspectorSubject
    {
				/**
				 *  The widget that holds all controls 
				 *  for this subject.
				 */
    APP_WIDGET			isPage;

				/**
				 *  Used in horizontal inspectors to 
				 *  hold the parent of the page. (frame)
				 */
    APP_WIDGET			isPageParent;

				/**
				 *  The actual (private) tool implementation
				 */
    void *			isTool;

				/**
				 *  The 'target' of the tool. Usually an 
				 *  EditApplication or an EditDocument.
				 */
    void *			isTarget;

				/**
				 *  Is the subject in the inspector 
				 *  enabled? I.E. Does it apply in the 
				 *  current context?
				 */
    int				isEnabled;

    APP_WIDGET			isNextPrevRow;
    APP_WIDGET			isPrevButton;
    APP_WIDGET			isNextButton;

    APP_WIDGET			isMoveUpButton;
    APP_WIDGET			isMoveDownButton;

    APP_WIDGET			isSelectDeleteRow;
    APP_WIDGET			isSelectButton;
    APP_WIDGET			isDeleteButton;

    APP_WIDGET			isInsertAppendRow;
    APP_WIDGET			isInsertButton;
    APP_WIDGET			isAppendButton;

    APP_WIDGET			isApplyRow;
    APP_WIDGET			isRevertButton;
    APP_WIDGET			isApplyButton;

    const struct InspectorSubjectType *
				isSubjectType;
    } InspectorSubject;

typedef struct InspectorSubjectType
    {
				/**
				 *  Configurable resources such as texts.
				 */
    const struct InspectorSubjectResources *
				istResources;

				/**
				 *  Notify the subject thet the user 
				 *  selected a color
				 */
    InspectorSubjectGotColor	istGotColor;

				/**
				 *  Populate a 'page' widget with our 
				 *  tool controls
				 */
    SubjectBuildTool		istBuildTool;

				/**
				 *  Fill the dropdowns on the page
				 */
    SubjectFillChoosers		istFillChoosers;

				/**
				 *  Finish building the page
				 */
    SubjectFinishPage		istFinishPage;

				/**
				 *  And do any cleanup of tool resources if
				 *  needed
				 */
    SubjectFreeTool		istFreeTool;

				/**
				 *  Callback to navigate to the next 
				 *  similar instance.
				 */
    APP_BUTTON_CALLBACK_T	istMoveToNext;

				/**
				 *  Callback to navigate to the 
				 *  previous similar instance.
				 */
    APP_BUTTON_CALLBACK_T	istMoveToPrev;

				/**
				 *  Callback to extend the  selection 
				 *  to entire instances
				 */
    APP_BUTTON_CALLBACK_T	istSelectInstance;

				/**
				 *  Callback to delete the instances 
				 *  that intersect the selection
				 */
    APP_BUTTON_CALLBACK_T	istDeleteInstance;

				/**
				 *  Callback to insert an  instance 
				 *  before the head of the selection
				 */
    APP_BUTTON_CALLBACK_T	istInsertInstance;

				/**
				 *  Callback to append an instance 
				 *  after the tail of the selection
				 */
    APP_BUTTON_CALLBACK_T	istAppendInstance;

				/**
				 *  Revert changes made in the  tool: 
				 *  Reset it to the values that come 
				 *  from the selection in the document.
				 */
    APP_BUTTON_CALLBACK_T	istRevertChanges;

				/**
				 *  Apply changes made in the tool: 
				 *  Set the values in the  selection 
				 *  in the document.
				 */
    APP_BUTTON_CALLBACK_T	istApplyChanges;

    } InspectorSubjectType;

typedef struct InspectorSubjectResources
    {
    const char *	isrSubjectName;
    const char *	isrApplyToSubject;
    const char *	isrRevert;

    const char *	isrNextButtonText;
    const char *	isrPrevButtonText;

    const char *	isrMoveDownButtonText;
    const char *	isrMoveUpButtonText;

    const char *	isrSelectButtonText;
    const char *	isrDeleteButtonText;

    const char *	isrInsertButtonText;
    const char *	isrAppendButtonText;
    } InspectorSubjectResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInitInspectorSubject(	InspectorSubject *	is );

#   endif
