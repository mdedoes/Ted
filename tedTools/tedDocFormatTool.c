/************************************************************************/
/*									*/
/*  The Application wide 'Format Tool':					*/
/*									*/
/*  An inpector with pages for the different formatting properties of	*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>

#   include	"tedDocFormatTool.h"

#   include	<appInspectorSubject.h>
#   include	<appEditApplication.h>
#   include	"tedFormatToolSubject.h"
#   include	"tedFindTool.h"
#   include	<appEditDocument.h>
#   include	<tedDocFront.h>
#   include	<appInspector.h>

#   include	<appDebugon.h>

/***********************/

static InspectorSubjectType  TED_DOC_InspectorSubjectTypes[TED_DOCtsi_COUNT];
static FormatToolSubjectType TED_DOC_FormatToolSubjectTypes[TED_DOCtsi_COUNT];

static void tedDocCollectFormatToolSubjectTypes(
				struct EditApplication *	ea )
    {
#   if USE_FIND_TOOL
    tedDocFindToolFillSubjectTypes( ea,
			&(TED_DOC_InspectorSubjectTypes[TED_DOCtsiFIND]),
			&(TED_DOC_FormatToolSubjectTypes[TED_DOCtsiFIND]) );
#   endif
    }

static AppInspector * tedDocGetInspector(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;

    static int			typesCollected= 0;

    if  ( ! typesCollected )
	{
	tedDocCollectFormatToolSubjectTypes( ea );
	typesCollected= 1;
	}

    if  ( ! ed->edInspector )
	{
	ed->edInspector= appMakeInspector( ea, (void *)ed,
			    TED_DOC_InspectorSubjectTypes, TED_DOCtsi_COUNT );
	if  ( ! ed->edInspector )
	    { XDEB(ed->edInspector); return (AppInspector *)0;	}
	}

    return ed->edInspector;
    }

void tedDocBuildFormatTool(	EditDocument *				ed )
    {
    struct AppInspector *	ai;

    ai= tedDocGetInspector( ed );
    if  ( ! ai )
	{ XDEB(ai); return;	}

    return;
    }

void tedDocToolShowFindTool(	struct EditDocument *			ed,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd )
    {
    struct AppInspector *		ai;
    const int				subject= TED_DOCtsiFIND;

    InspectorSubject *			is;
    const InspectorSubjectType *	ist;
    const InspectorSubjectResources *	isr;

    ai= tedDocGetInspector( ed );
    if  ( ! ai )
	{ XDEB(ai); return;	}

    is= ai->aiSubjects+ subject;
    ist= is->isSubjectType;
    isr= ist->istResources;

    guiMakeColumnFrameInColumn( &(is->isPageParent), &(is->isPage),
					ed->edToolbar, isr->isrSubjectName );

    appInspectorBuildSubjectPage( ai, is->isPage, subject );

    ai->aiCurrentSubject= subject;

    tedFindToolSetSelectionAsPattern( is->isTool, sd, ds, bd );

    }

void tedDocRefreshFormatTool(	struct EditDocument *			ed,
				struct AppInspector *			ai,
				struct BufferDocument *			bd,
				struct DocumentTree *			tree,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    int			enabled= 1;
    int			pref= 5;

    int			subject= ai->aiCurrentSubject;

    InspectorSubject *			is= ai->aiSubjects+ subject;
    const FormatToolSubjectType *	ftst=
					TED_DOC_FormatToolSubjectTypes+ subject;

    if  ( subject < 0			||
	  ! is->isTool			||
	  ! ftst->ftstRefreshPage	)
	{ enabled= 0; pref= 0;	}
    else{
	(*ftst->ftstRefreshPage)( is->isTool,
				    &enabled, &pref, is,
				    ds, sg, sd, bd, cmdEnabled );
	}

    if  ( is->isPageParent )
	{
	guiEnableWidget( is->isPageParent, enabled != 0 );
	}

    return;
    }
