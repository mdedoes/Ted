/************************************************************************/
/*									*/
/*  Ted: Interface to the generic find tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<tedAppFront.h>
#   include	<tedDocFront.h>
#   include	"tedFindTool.h"
#   include	<docEditCommand.h>
#   include	<appFindTool.h>
#   include	<appInspectorSubject.h>
#   include	"tedFormatToolSubject.h"
#   include	<utilMemoryBuffer.h>
#   include	<docSelectionDescription.h>
#   include	<docSelect.h>

#   include	<appDebugon.h>

static FindToolResources		TED_FindToolResources;
static InspectorSubjectResources	TED_FindSubjectResources;

/************************************************************************/

int tedFindToolSetSelectionAsPattern(
				void *					vft,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd )
    {
    FindTool *		ft= (FindTool *)vft;

    int			rval= 0;
    MemoryBuffer	mbPattern;

    utilInitMemoryBuffer( &mbPattern );

    if  ( sd->sdIsSet			&&
	  ! sd->sdIsIBarSelection	&&
	  sd->sdIsSingleParagraph	&&
	  ! sd->sdIsObjectSelection	)
	{
	if  ( docCollectReference( &mbPattern, ds, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	appFindToolSetPattern( ft,
			utilMemoryBufferGetString( &mbPattern ),
			ft->ftUseRegex );
	}

    appFindToolFocusToPattern( ft );

  ready:

    utilCleanMemoryBuffer( &mbPattern );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Clean and Free a find tool. (Memory only.. Not widgets.		*/
/*									*/
/************************************************************************/

static void tedFreeFindTool(	void *	vft )
    {
    FindTool *			ft= (FindTool *)vft;

    appCleanFindTool( ft );

    free( ft );
    }

/************************************************************************/
/*									*/
/*  Adapt find tool to the current document.				*/
/*									*/
/************************************************************************/

static void tedRefreshFindTool(	void *				vft,
				int *				pEnabled,
				int *				pPref,
				struct InspectorSubject *		is,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    FindTool *	ft= (FindTool *)vft;

    appFindToolEnableReplace( ft, cmdEnabled[EDITcmdREPLACE] );

    return;
    }

/************************************************************************/
/*									*/
/*  Build a find tool and connect it to Ted.				*/
/*									*/
/************************************************************************/

static void * tedAppBuildFindTool( int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr )
    {
    const FindToolResources *	ftr= &TED_FindToolResources;

    FindTool *			ft= malloc( sizeof(FindTool) );

    appInitFindTool( ft );

    if  ( ! ft )
	{ XDEB(ft); return ft;	}

    appFillVerticalFindTool( ft, ftr,
				subjectPage, ai, ea, is, pageWidget, isr,
				tedAppFindNext, tedAppFindPrev,
				tedAppFindToolSetPattern, tedAppReplace,
				is->isTarget );

    return (void *)ft;
    }

static int tedDocToolFindNext(	void *				voided )
    { return tedDocFindNext( (struct EditDocument *)voided );	}

static int tedDocToolFindPrev(	void *				voided )
    { return tedDocFindPrev( (struct EditDocument *)voided );	}

static int tedDocToolReplace(		void *			voided,
					const char *		replacement )
    { return tedDocReplace( (struct EditDocument *)voided, replacement ); }

static void * tedDocBuildFindTool( int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr )
    {
    const FindToolResources *	ftr= &TED_FindToolResources;

    FindTool *			ft= malloc( sizeof(FindTool) );

    appInitFindTool( ft );

    if  ( ! ft )
	{ XDEB(ft); return ft;	}

    appFillHorizontalFindTool( ft, ftr,
				subjectPage, ai, ea, is, pageWidget, isr,
				tedDocToolFindNext, tedDocToolFindPrev,
				tedDocFindToolSetPattern, tedDocToolReplace,
				is->isTarget );

    return (void *)ft;
    }

int tedAppFindToolFillSubjectTypes( struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appFindToolGetResourceTable( ea,
			    &TED_FindToolResources,
			    &TED_FindSubjectResources );

    ist->istResources= &TED_FindSubjectResources;

    ist->istBuildTool= tedAppBuildFindTool;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ftst->ftstRefreshPage= tedRefreshFindTool;
    ist->istFreeTool= tedFreeFindTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }

int tedDocFindToolFillSubjectTypes( struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appFindToolGetResourceTable( ea,
			    &TED_FindToolResources,
			    &TED_FindSubjectResources );

    ist->istResources= &TED_FindSubjectResources;

    ist->istBuildTool= tedDocBuildFindTool;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ftst->ftstRefreshPage= tedRefreshFindTool;
    ist->istFreeTool= tedFreeFindTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }
