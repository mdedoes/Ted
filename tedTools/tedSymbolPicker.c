/************************************************************************/
/*									*/
/*  Ted: Interface to the generic symbol picker.			*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<tedAppFront.h>
#   include	"tedSymbolPicker.h"
#   include	<appSymbolPicker.h>
#   include	<docEditCommand.h>
#   include	<docDocumentProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

struct SelectionGeometry;

static SymbolPickerResources		TED_SymbolPickerPageResources;
static InspectorSubjectResources	TED_SymbolPickerSubjectResources;

/************************************************************************/
/*									*/
/*  Adapt symbol picker to the current document.			*/
/*									*/
/************************************************************************/

static void tedRefreshSymbolPicker(
			void *					vsp,
			int *					pEnabled,
			int *					pPref,
			InspectorSubject *			is,
			const struct DocumentSelection *	ds,
			const struct SelectionGeometry *	sg,
			const SelectionDescription *		sd,
			const struct BufferDocument *		bd,
			const unsigned char *			cmdEnabled )
    {
    SymbolPicker *	sp= (SymbolPicker *)vsp;

    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentFontList *	dfl= dp->dpFontList;

    if  ( sd->sdIsListBullet )
	{ *pEnabled= 0; return;	}

    if  ( appAdaptSymbolPickerToFontFamily( sp, sd->sdDocumentId, dfl,
			&(sd->sdTextAttributeMask), &(sd->sdTextAttribute) ) )
	{ LDEB(1);	}

    appEnableSymbolPicker( sp, cmdEnabled[EDITcmdREPLACE] );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Insert callback for the symbol picker.				*/
/*									*/
/************************************************************************/

static void tedSymbolPickerInsert(
				void *			voidea,
				const char *		bytes,
				int			size,
				const TextAttribute *	taSet,
				const PropertyMask *	taSetMask )
    {
    struct EditApplication *	ea= (struct EditApplication *)voidea;

    tedAppInsertStringWithAttribute( ea, bytes, size, taSet, taSetMask );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the symbol picker in the parent widget.			*/
/*									*/
/************************************************************************/

static void * tedBuildSymbolPicker(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const SymbolPickerResources *	spr= &TED_SymbolPickerPageResources;

    SymbolPicker *			sp= malloc( sizeof(SymbolPicker) );

    if  ( ! sp )
	{ XDEB(sp); return sp;	}

    appInitSymbolPicker( sp );

    appFillSymbolPicker( sp, spr, ai, ea, is, pageWidget, isr );

    sp->spTarget= sp->spApplication;
    sp->spInsert= tedSymbolPickerInsert;

    return (void *)sp;
    }

static void tedFinishSymbolPicker(	void *		vsp )
    {
    appFinishSymbolPicker( (SymbolPicker *)vsp );
    }

static void tedSymbolPickerFillChoosers(
				void *				vsp )
    {
    SymbolPicker *			sp= (SymbolPicker *)vsp;
    const SymbolPickerResources *	spr= &TED_SymbolPickerPageResources;

    appSymbolPickerFillChoosers( sp, spr );
    }

static void tedFreeSymbolPicker(	void *	vsp )
    {
    SymbolPicker *		sp= (SymbolPicker *)vsp;

    appCleanSymbolPicker( sp );
    free( sp );
    }

int tedSymbolPickerFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appSymbolPickerGetResourceTable( ea,
			    &TED_SymbolPickerPageResources,
			    &TED_SymbolPickerSubjectResources );

    ist->istResources= &TED_SymbolPickerSubjectResources;

    ist->istBuildTool= tedBuildSymbolPicker;
    ist->istFillChoosers= tedSymbolPickerFillChoosers;
    ist->istFinishPage= tedFinishSymbolPicker;
    ist->istFreeTool= tedFreeSymbolPicker;

    ftst->ftstRefreshPage= tedRefreshSymbolPicker;
    /*fts->ftsToolResetCache*/

    return 0;
    }
