/************************************************************************/
/*									*/
/*  The 'Font Tool':							*/
/*									*/
/*  Most of the ont tool calls directly go to the AppFontTool.		*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"tedFontTool.h"
#   include	"tedFormatToolSubject.h"
#   include	<tedAppFront.h>
#   include	<docListOverride.h>
#   include	<docDocumentList.h>
#   include	<docEditCommand.h>
#   include	<docDocumentProperties.h>
#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appInspectorSubject.h>
#   include	<appGuiResource.h>
#   include	<appFontTool.h>

#   include	<appDebugon.h>

struct SelectionGeometry;

static AppFontToolResources		TED_FontToolResources;
static InspectorSubjectResources	TED_FontSubjectResources;

/************************************************************************/
/*									*/
/*  Intermediary routine to refresh the text font tool.			*/
/*									*/
/************************************************************************/

static void tedRefreshFontTool(	void *				vafc,
				int *				pEnabled,
				int *				pPref,
				struct InspectorSubject *	is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    AppFontChooser *		afc= (AppFontChooser *)vafc;
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentFontList *	dfl= dp->dpFontList;

    if  ( sd->sdIsObjectSelection )
	{ *pEnabled= 0; return;	}

    if  ( sd->sdIsListBullet )
	{
	ListOverride *		lo;
	DocumentList *		dl;
	ListLevel *		ll;

	afc->afcSetFont= tedAppListFontToolSet;

	if  ( docGetListOfParagraph( &lo, &dl, sd->sdListOverride, bd ) )
	    { LDEB(1); *pEnabled= 0; return;	}
	ll= &(dl->dlLevels[sd->sdListLevel]);

	if  ( appFontToolShowCurrentFont( afc, &(ll->llTextAttributeMask),
				    &(ll->llTextAttribute), sd->sdDocumentId,
				    cmdEnabled[EDITcmdUPD_LIST],
				    dfl, dp->dpColorPalette ) )
	    { LDEB(1);	}

	*pEnabled= 1;
	}
    else{
	afc->afcSetFont= tedAppFontToolSet;
	(*pPref)++;

	if  ( appFontToolShowCurrentFont( afc, &(sd->sdTextAttributeMask),
				    &(sd->sdTextAttribute), sd->sdDocumentId,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS],
				    dfl, dp->dpColorPalette ) )
	    { LDEB(1);	}

	*pEnabled= 1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_FontToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "fontToolFont",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Font" ),
    APP_RESOURCE( "fontToolSet",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set" ),
    APP_RESOURCE( "fontToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static void tedFontToolGetResourceTable( struct EditApplication *	ea,
					AppFontToolResources *		aftr,
					InspectorSubjectResources *	isr )
    {
    static int	gotSubjectResources= 0;

    appFontToolGetResourceTable( ea, aftr );

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_FontToolSubjectResourceTable,
				sizeof(TED_FontToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

static void * tedFontToolBuildPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const AppFontToolResources * aftr= &TED_FontToolResources;

    AppFontChooser *		afc= malloc( sizeof(AppFontChooser) );

    if  ( ! afc )
	{ XDEB(afc); return afc;	}

    appFontToolFillPage( afc, aftr, subjectPage, ai, ea, is, pageWidget, isr );

    return (void *)afc;
    }

static void tedFontToolFillChoosers( void *	vafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)vafc;
    const AppFontToolResources * aftr= &TED_FontToolResources;

    appFontToolFillChoosers( afc, aftr );
    }

static void tedFontToolFinishPage( void *	vafc )
    {
    appFontToolFinishPage( (AppFontChooser *)vafc, &TED_FontToolResources );
    }

static void tedFontChooserFreePage( void *			vafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)vafc;

    appFontChooserCleanPage( afc );

    free( afc );
    }

int tedFontToolFillSubjectTypes( struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFontToolGetResourceTable( ea,
			&TED_FontToolResources,
			&TED_FontSubjectResources );

    ist->istResources= &TED_FontSubjectResources;

    ist->istBuildTool= tedFontToolBuildPage;
    ist->istFillChoosers= tedFontToolFillChoosers;
    ist->istFinishPage= tedFontToolFinishPage;
    ftst->ftstRefreshPage= tedRefreshFontTool;
    ist->istFreeTool= tedFontChooserFreePage;
    /*fts->ftsToolResetCache*/

    return 0;
    }
