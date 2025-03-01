/************************************************************************/
/*									*/
/*  Page Layout page on an inspector:					*/
/*									*/
/************************************************************************/

#   ifndef	APP_PAGE_LAYOUT_SUBJECT
#   define	APP_PAGE_LAYOUT_SUBJECT

#   include	"appPageLayoutTool.h"
#   include	"appInspectorSubject.h"

extern PageLayoutPageResources		APP_PageLayoutPageResources;
extern InspectorSubjectResources	APP_PageLayoutSubjectResources;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void appFreePageLayoutSubjectTool(	void *	vplt );
extern void appPageLayoutSubjectFillChoosers(	void *	vplt );
extern void appPageLayoutSubjectToolFinishPage(	void *	vplt );

extern void * appFormatBuildPageLayoutPage(
			int					subjectPage,
			struct AppInspector *			ai,
			struct EditApplication *		ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr );

#   endif	/*  APP_PAGE_LAYOUT_SUBJECT */
