/************************************************************************/
/*									*/
/*  Intermediary functionality for the page layout page.		*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"appPageLayoutTool.h"
#   include	"appInspectorSubject.h"
#   include	"appPageLayoutSubject.h"

#   include	<appDebugon.h>

PageLayoutPageResources		APP_PageLayoutPageResources;
InspectorSubjectResources	APP_PageLayoutSubjectResources;

/************************************************************************/
/*									*/
/*  Make a page layout page customized for Ted.				*/
/*									*/
/************************************************************************/

void * appFormatBuildPageLayoutPage(
			int					subjectPage,
			struct AppInspector *			ai,
			struct EditApplication *		ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const PageLayoutPageResources *	plpr= &APP_PageLayoutPageResources;

    const int		doHdFt= 1;
    const int		doSelection= 1;

    PageLayoutTool *	plt= malloc( sizeof(PageLayoutTool) );

    appInitPageLayoutTool( plt );

    if  ( ! plt )
	{ XDEB(plt); return plt;	}

    appPageLayoutPageFillPage( plt, doHdFt, doSelection,
					    plpr, ai, ea, is, pageWidget, isr );

    return (void *)plt;
    }


/************************************************************************/
/*									*/
/*  Clean the page layout tool.						*/
/*									*/
/************************************************************************/

void appFreePageLayoutSubjectTool(	void *	vplt )
    {
    PageLayoutTool *	plt= (PageLayoutTool *)vplt;

    appCleanPageLayoutTool( plt );

    free( plt );

    return;
    }

void appPageLayoutSubjectFillChoosers( void *		vplt )
    {
    PageLayoutTool *			plt= (PageLayoutTool *)vplt;
    const PageLayoutPageResources *	plpr= &APP_PageLayoutPageResources;

    appPageLayoutPageFillChoosers( plt, plpr );
    }

void appPageLayoutSubjectToolFinishPage(
				void *				vplt )
    {
    appPageLayoutPageFinishPage( (PageLayoutTool *)vplt,
						&APP_PageLayoutPageResources );
    }
