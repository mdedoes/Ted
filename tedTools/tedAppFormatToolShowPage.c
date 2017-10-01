/************************************************************************/
/*									*/
/*  Select a particular format tool page.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>

#   include	"tedAppFormatTool.h"
#   include	<tedToolFront.h>
#   include	<appEditApplication.h>
#   include	<appInspector.h>
#   include	<appInspectorSubject.h>
#   include	"tedFindTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Show the page layout page.						*/
/*									*/
/************************************************************************/

void tedAppFormatShowPagePage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_PAGE_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiPAGE, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiPAGE );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Show the Hyperlink page.						*/
/*									*/
/************************************************************************/

void tedAppFormatShowLinkPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_LINK_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiLINK, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiLINK );
#   endif

    return;
    }

void tedAppFormatShowTablePage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiTABLE, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiTABLE );

    return;
    }

void tedAppFormatShowColumnPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiCOLUMN, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiCOLUMN );

    return;
    }

void tedAppFormatShowBookmarkPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_BOOKMARK_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiBOOKMARK, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiBOOKMARK );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the symbol page of the inspector.				*/
/*									*/
/************************************************************************/

void tedAppFormatShowSymbolPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiSYMBOL, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiSYMBOL );

    return;
    }

void tedAppFormatShowFontPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_FONT_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiFONT, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiFONT );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the spell page of the inspector.				*/
/*									*/
/************************************************************************/

void tedAppFormatShowSpellPage(	EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_SPELL_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiSPELL, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiSPELL );
#   endif

    return;
    }

void tedAppFormatShowParaLayoutPage( EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiPARA_LAY, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiPARA_LAY );
    }

void tedAppFormatShowParaTabsPage( EditApplication *	ea )
    {
    if  ( ! ea->eaInspector )
	{ XDEB(ea->eaInspector); return;	}

#   if USE_TABS_TOOL
    appEnableInspectorSubject( ea->eaInspector, TED_APPtsiTABS, 1 );

    appInspectorSelectSubject( ea->eaInspector, TED_APPtsiTABS );
#   endif
    }

/************************************************************************/
/*									*/
/*  Switch to the find page of the inspector.				*/
/*									*/
/************************************************************************/

void tedAppToolShowFindTool(	EditApplication *			ea,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd )
    {
    AppInspector *	ai= ea->eaInspector;

    if  ( ! ai )
	{ XDEB(ai); return;	}

#   if USE_FIND_TOOL
    appEnableInspectorSubject( ai, TED_APPtsiFIND, 1 );

    appInspectorSelectSubject( ai, TED_APPtsiFIND );

    tedFindToolSetSelectionAsPattern(
		    ai->aiSubjects[TED_APPtsiFIND].isTool, sd, ds, bd );
#   endif

    return;
    }

