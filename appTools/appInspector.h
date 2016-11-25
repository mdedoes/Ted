/************************************************************************/
/*									*/
/*  Application inspector with different subject pages.			*/
/*									*/
/************************************************************************/

#   ifndef	APP_INSPECTOR_H
#   define	APP_INSPECTOR_H

#   include	<guiBase.h>
#   include	<appTool.h>
#   include	<guiOptionmenu.h>

/************************************************************************/
/*									*/
/*  Facilities to make property inspectors.				*/
/*									*/
/************************************************************************/

struct AppInspector;
struct EditApplication;
struct RGB8Color;
struct InspectorSubject;
struct InspectorResources;
struct InspectorSubjectResources;
struct InspectorSubjectType;

typedef void (*InspectorNotifySubject)(
			struct AppInspector *		ai,
			int				from,
			int				to );

typedef struct AppInspector
    {
    struct EditApplication *	aiApplication;

    const struct InspectorResources *	aiResources;

    APP_WIDGET			aiTopWidget;
    APP_WIDGET			aiPaned;

    AppOptionmenu		aiSubjectOptionmenu;

    APP_WIDGET			aiSeparator;
    APP_WIDGET			aiPageParent;
    APP_WIDGET			aiSeparator2;

    APP_WIDGET			aiCloseButton;

    InspectorNotifySubject	aiNotifySubject;

    void *			aiRgbChooser;		/*  RgbChooser */
    int				aiRgbSubjectNumber;

    AppToolDestroy		aiDestroy;
    void *			aiTarget;

    int				aiSubjectCount;
    int				aiCurrentSubject;

    struct InspectorSubject *	aiSubjects;
    } AppInspector;

typedef struct InspectorResources
    {
    char *		airCloseText;
    } InspectorResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInspectorSelectSubject(	AppInspector *		ai,
					int			subject );

extern void appFinishInspectorTool(		AppInspector *		ai );

extern void appEnableInspector(		AppInspector *		ai,
					int			enabled );

extern void appEnableInspectorSubject(	AppInspector *		ai,
					int			subject ,
					int			enabled );

extern void appInspectorGotColor(
				AppInspector *			ai,
				int				subjectPage,
				int				property,
				const struct RGB8Color *	rgb8 );

extern void appInspectorShowRgbPage(	AppInspector *		ai,
					int			fromSubject,
					int			fromProperty,
					const struct RGB8Color * rgb8 );

extern void appInspectorSetRgbPage(	AppInspector *		ai,
					void *			vrcp,
					int			subject );

extern int appInspectorToolAddSubject(
			AppInspector *					ai,
			const struct InspectorSubjectResources *	isr );

extern void appInspectorDeleteSubject(	AppInspector *		ai,
					int			subject );

extern void appInspectorPageChosen(	int			subject,
					void *			vai );

extern void appInspectorChoosePage(	AppInspector *		ai,
					int			andMenu,
					int			pageNumber );

extern AppInspector * 	appMakeInspectorTool(
			    struct EditApplication *		ea,
			    APP_WIDGET				option,
			    const struct InspectorSubjectType * ist,
			    int					subjectCount,
			    int					rgbSubject,
			    AppToolDestroy			closeInspector,
			    void *				through,
			    void *				target );

extern AppInspector * appMakeInspector(
			struct EditApplication *		ea,
			void *					target,
			const struct InspectorSubjectType *	ist,
			int					subjectCount );

extern void appInspectorMakePageParent( AppInspector *	ai );

extern int appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
						AppInspector *	ai,
						const char *	label );

extern void appInspectorEnablePage(	AppInspector *	ai,
					int		pageNumber,
					int		enabled );

extern void appInspectorInitializeFlags(	int *		enabled,
						int *		prefs,
						int		subjectCount );

extern void appInspectorApplyFlags(	AppInspector *	ai,
					int *		enabled,
					int *		prefs,
					int		subjectCount,
					int		choosePage );

extern int appInspectorBuildSubjectPage( AppInspector *		ai,
					APP_WIDGET		toolWidget,
					int			subject );

#   endif
