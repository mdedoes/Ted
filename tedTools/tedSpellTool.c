/************************************************************************/
/*									*/
/*  Ted: Interface to the generic spell tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<tedAppFront.h>
#   include	"tedSpellTool.h"
#   include	<docEditCommand.h>
#   include	<indSpellChecker.h>
#   include	<appSpellTool.h>
#   include	<appInspectorSubject.h>
#   include	<textMsLocale.h>
#   include	<docSelectionDescription.h>
#   include	<docSelect.h>
#   include	<docBuf.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

struct SelectionGeometry;

/************************************************************************/
/*									*/
/*  Adapt spell tool to the current document.				*/
/*									*/
/************************************************************************/

static void tedRefreshSpellTool( void *				vst,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    SpellTool *		st= (SpellTool *)vst;
    int			localeId;
    const char *	localeTag;

    if  ( ! st->stSpellChecker				||
	  st->stSpellChecker->scDictionaryCount == 0	)
	{ *pEnabled= 0; return;		}

    appSpellToolSetReadOnly( st, ! cmdEnabled[EDITcmdREPLACE] ||
				! cmdEnabled[EDITcmdUPD_SPAN_PROPS] );

    if  ( sd->sdTextAttribute.taLocaleId < 1 )
	{ localeId= bd->bdLocaleId;			}
    else{ localeId= sd->sdTextAttribute.taLocaleId;	}

    localeTag= textGetMsLocaleTagById( localeId );
    if  ( localeTag )
	{ appSpellToolSelectLocaleTag( st, localeTag );	}

    appSpellToolSetTextSelected( st, ds && ! docIsIBarSelection( ds ) );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Build a spell tool and connect it to Ted.				*/
/*									*/
/************************************************************************/

static SpellChecker TED_APP_SpellChecker;
static int TED_APP_SpellCheckerInitialized;

static void * tedBuildSpellTool( int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    SpellTool *	st= malloc( sizeof(SpellTool) );

    if  ( ! st )
	{ XDEB(st); return st;	}

    appInitSpellTool( st );

    appFillSpellTool( &TED_APP_SpellChecker,
			    st, ai, ea, is, subjectPage, pageWidget, isr );

    return (void *)st;
    }

static void tedSpellToolFillChoosers(
				void *			vst )
    {
    SpellTool *	st= (SpellTool *)vst;

    appSpellToolFillChoosers( st );
    }

static void tedFinishSpellTool( void *			vst )
    {
    SpellTool *	st= (SpellTool *)vst;

    appFinishSpellTool( st );
    }

static void tedFreeSpellTool( void *			vst )
    {
    SpellTool *	st= (SpellTool *)vst;

    appCleanSpellTool( st );
    free( st );
    }

static InspectorSubjectResources	TED_SpellSubjectResources;

int tedSpellToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appSpellToolGetResourceTable( ea, &TED_SpellSubjectResources );

    /*fts->ftsToolResources*/
    ist->istResources= &TED_SpellSubjectResources;

    ist->istBuildTool= tedBuildSpellTool;
    ist->istFillChoosers= tedSpellToolFillChoosers;
    ist->istFinishPage= tedFinishSpellTool;
    ist->istFreeTool= tedFreeSpellTool;

    ftst->ftstRefreshPage= tedRefreshSpellTool;
    /*fts->ftsToolResetCache*/

    if  ( ! TED_APP_SpellCheckerInitialized )
	{
	indInitSpellChecker( &TED_APP_SpellChecker );

	TED_APP_SpellChecker.scSetup= spellSetup;
	TED_APP_SpellChecker.scFindNext= tedSpellFindNext;
	TED_APP_SpellChecker.scCorrect= tedAppReplace;
	TED_APP_SpellChecker.scDoNotCheck= tedAppDoNotSpellCheck;
	TED_APP_SpellChecker.scSetLocale= tedAppSetLocale;

	TED_APP_SpellCheckerInitialized= 1;
	}

    return 0;
    }
