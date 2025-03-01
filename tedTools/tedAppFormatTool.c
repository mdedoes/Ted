/************************************************************************/
/*									*/
/*  The Application wide 'Format Tool':					*/
/*									*/
/*  An inpector with pages for the different formatting properties of	*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stdlib.h>
#   include	<stddef.h>

#   include	"tedAppFormatTool.h"

#   include	"tedFontTool.h"
#   include	"tedTextOrnamentsTool.h"
#   include	"tedParaLayoutTool.h"
#   include	"tedTabsTool.h"
#   include	"tedParaOrnamentsTool.h"
#   include	"tedListTool.h"
#   include	"tedSectionTool.h"
#   include	"tedTableTool.h"
#   include	"tedRowTool.h"
#   include	"tedColumnTool.h"
#   include	"tedCellTool.h"
#   include	"tedPageLayoutTool.h"
#   include	"tedHeaderFooterTool.h"
#   include	"tedThisNoteTool.h"
#   include	"tedNotesTool.h"
#   include	"tedLinkTool.h"
#   include	"tedBookmarkTool.h"
#   include	"tedImageTool.h"
#   include	"tedFrameTool.h"
#   include	"tedTocTool.h"
#   include	"tedSymbolPicker.h"
#   include	"tedFindTool.h"
#   include	"tedSpellTool.h"
#   include	"tedLanguageTool.h"
#   include	"tedRgbChooserPage.h"
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<appEditApplication.h>
#   include	<appGuiApplication.h>
#   include	<appInspector.h>
#   include	"tedFormatToolSubject.h"

#   include	<tedToolFront.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  The user destroys ( IE closes ) the format tool.			*/
/*									*/
/************************************************************************/

static void tedAppDestroyFormatTool(	void *	voidtft )
    {
    TedFormatTool *	tft= (TedFormatTool *)voidtft;
    EditApplication *	ea= tft->tftApplication;

    free( tft );

    ea->eaInspector= (void *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  make/show the format tool.						*/
/*									*/
/************************************************************************/

static InspectorSubjectType  TED_APP_InspectorSubjectTypes[TED_APPtsi_COUNT];
static FormatToolSubjectType TED_APP_FormatToolSubjectTypes[TED_APPtsi_COUNT];

static void tedAppCollectFormatToolSubjectTypes(
				struct EditApplication *	ea )
    {

#   if USE_FONT_TOOL
    tedFontToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiFONT,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiFONT );
#   endif

    tedTextOrnamentsToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiTEXT_ORN,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiTEXT_ORN );

    tedParaLayoutToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiPARA_LAY,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiPARA_LAY );

#   if USE_TABS_TOOL
    tedTabsToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiTABS,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiTABS );
#   endif

    tedParaOrnamentsToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiPARA_ORN,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiPARA_ORN );

#   if USE_LIST_TOOL
    tedListToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiLISTS,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiLISTS );
#   endif

    tedSectionToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiSECT,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiSECT );

    tedTableToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiTABLE,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiTABLE );

    tedRowToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiROW,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiROW );

    tedColumnToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiCOLUMN,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiCOLUMN );

    tedCellToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiCELL,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiCELL );

#   if USE_PAGE_TOOL
    tedPageLayoutToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiPAGE,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiPAGE );
#   endif

    tedHeaderFooterToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiHEADFOOT,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiHEADFOOT );

    tedThisNoteToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiTHIS_NOTE,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiTHIS_NOTE );

    tedFootnotesToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiFOOT_NOTES,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiFOOT_NOTES );

    tedEndnotesToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiEND_NOTES,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiEND_NOTES );

#   if USE_LINK_TOOL
    tedLinkToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiLINK,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiLINK );
#   endif

#   if USE_BOOKMARK_TOOL
    tedBookmarkToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiBOOKMARK,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiBOOKMARK );
#   endif

    tedImageToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiIMAGE,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiIMAGE );

#   if USE_FRAME_TOOL
    tedFrameToolFillSubjectTypes( ea,
			TED_APP_InspectorSubjectTypes+ TED_APPtsiFRAME,
			TED_APP_FormatToolSubjectTypes+ TED_APPtsiFRAME );
#   endif

    tedTocToolFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiTOC]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiTOC]) );

    tedSymbolPickerFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiSYMBOL]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiSYMBOL]) );

#   if USE_FIND_TOOL
    tedAppFindToolFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiFIND]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiFIND]) );
#   endif

#   if USE_SPELL_TOOL
    tedSpellToolFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiSPELL]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiSPELL]) );
#   endif

#   if USE_LANGUAGE_TOOL
    tedLanguageToolFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiLANGUAGE]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiLANGUAGE]) );
#   endif

    tedRgbChooserFillSubjectTypes( ea,
			&(TED_APP_InspectorSubjectTypes[TED_APPtsiRGB]),
			&(TED_APP_FormatToolSubjectTypes[TED_APPtsiRGB]) );

    return;
    }

int tedAppShowFormatTool(	APP_WIDGET		option,
				EditApplication *	ea )
    {
    int				rval= 0;
    struct AppInspector *	ai= (struct AppInspector *)0;
    TedFormatTool *		tft= (TedFormatTool *)0;

    static int				typesCollected= 0;

    if  ( ea->eaInspector )
	{
	appShowShellWidget( ea->eaInspector->aiApplication,
					ea->eaInspector->aiTopWidget );

	goto ready;
	}

    if  ( ! typesCollected )
	{
	tedAppCollectFormatToolSubjectTypes( ea );
	typesCollected= 1;
	}

    tft= (TedFormatTool *)malloc( sizeof(TedFormatTool) );
    if  ( ! tft )
	{ XDEB(tft); rval= -1; goto ready;	}

    tft->tftApplication= ea;

    ai= appMakeInspectorTool( ea, option,
			TED_APP_InspectorSubjectTypes,
			TED_APPtsi_COUNT, TED_APPtsiRGB,
			tedAppDestroyFormatTool,
			(void *)tft, (void *)ea );

    if  ( ! ai )
	{ XDEB(ai); rval= -1; goto ready; }

    tft->tftInspector= ai; 

    /******/

    ea->eaInspector= ai;
    ai= (struct AppInspector *)0;
    tft= (TedFormatTool *)0;

  ready:

    if  ( tft )
	{ tedAppDestroyFormatTool( tft );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Ask the table tool to adapt itself to the selection.		*/
/*									*/
/************************************************************************/

static void tedFormatRefreshToolPages(
				int *				enabled,
				int *				prefs,
				TedFormatTool *			tft,
				struct AppInspector *		ai,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    int					i;
    InspectorSubject *			is;
    const FormatToolSubjectType *	ftst;

    is= ai->aiSubjects;
    ftst= TED_APP_FormatToolSubjectTypes;
    for ( i= 0; i < TED_APPtsi_COUNT; ftst++, is++, i++ )
	{
	if  ( is->isTool		&&
	      ftst->ftstRefreshPage	)
	    {
	    (*ftst->ftstRefreshPage)( is->isTool,
				    enabled+ i, prefs+ i, is,
				    ds, sg, sd, bd, cmdEnabled );
	    }
	}

    /*  no refresh  */
    enabled[TED_APPtsiRGB]= 0;

    return;
    }

void tedAppRefreshFormatTool(	struct AppInspector *		ai,
				int				choosePage,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    TedFormatTool *		tft= (TedFormatTool *)ai->aiTarget;

    int				enabled[TED_APPtsi_COUNT];
    int				prefs[TED_APPtsi_COUNT];

    appInspectorInitializeFlags( enabled, prefs, TED_APPtsi_COUNT );

    if  ( ! docSelectionIsSet( ds ) )
	{ appEnableInspector( ai, 0 ); return; }
    else{
	tedFormatRefreshToolPages( enabled, prefs, tft, ai,
					    bd, tree, ds, sg, sd, cmdEnabled );
	}

    appInspectorApplyFlags( ai, enabled, prefs, TED_APPtsi_COUNT, choosePage );

# if 0
    /*  Actually, this is too late the first time  */
    guiEnableWidget( td->tdToolsSpellingOption,
				    enabled[TED_APPtsiSPELL] );
    guiEnableWidget( td->tdToolsSymbolOption,
				    enabled[TED_APPtsiSYMBOL] );
# endif
    }

void tedAppFormatFieldListChanged(	EditApplication *	ea )
    {
    int					i;
    InspectorSubject *			is;
    const FormatToolSubjectType *	ftst;

    AppInspector *			ai= ea->eaInspector;

    if  ( ! ai )
	{ return;	}

    is= ai->aiSubjects;
    ftst= TED_APP_FormatToolSubjectTypes;
    for ( i= 0; i < TED_APPtsi_COUNT; ftst++, is++, i++ )
	{
	if  ( is->isTool		&&
	      ftst->ftstResetCache	)
	    {
	    (*ftst->ftstResetCache)( is->isTool );
	    }
	}

    return;
    }

