/************************************************************************/
/*									*/
/*  Ted: Interface to the generic RGB chooser page.			*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"tedRgbChooserPage.h"
#   include	<appRgbChooserPage.h>
#   include	<appInspectorSubject.h>

#   include	<appDebugon.h>

static RgbChooserPageResources		TED_RgbChooserPageResources;
static InspectorSubjectResources	TED_RgbChooserSubjectResources;

/************************************************************************/
/*									*/
/*  Build an RGB chooser.						*/
/*									*/
/************************************************************************/

static void * tedBuildRgbChooser(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const RgbChooserPageResources *	rcpr= &TED_RgbChooserPageResources;

    RgbChooser *	rcp= malloc( sizeof(RgbChooser) );

    if  ( ! rcp )
	{ XDEB(rcp); return rcp;	}

    appInitRgbChooser( rcp );

    appRgbChooserPageFillPage( rcp, rcpr, subjectPage, ai, ea, is, pageWidget, isr );

    return (void *)rcp;
    }

static void tedFinishRgbChooser( void *			vrcp )
    {
    RgbChooser *	rcp= (RgbChooser *)vrcp;

    appRgbChooserPageFinishPage( rcp );
    }

static void tedFreeRgbChooser( void *			vrcp )
    {
    RgbChooser *	rcp= (RgbChooser *)vrcp;

    appRgbChooserPageCleanPage( rcp );

    free( rcp );
    }

int tedRgbChooserFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appRgbChooserPageGetResourceTable( ea,
			    &TED_RgbChooserPageResources,
			    &TED_RgbChooserSubjectResources );

    ist->istResources= &TED_RgbChooserSubjectResources;

    ist->istBuildTool= tedBuildRgbChooser;
    /*fts->ftsToolFillChoosers*/
    ist->istFinishPage= tedFinishRgbChooser;
    /*fts->ftsToolRefreshPage*/
    ist->istFreeTool= tedFreeRgbChooser;
    /*fts->ftsToolResetCache*/

    return 0;
    }
