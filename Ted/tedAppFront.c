/************************************************************************/
/*									*/
/*  Ted: application level API for menu and tools.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<tedAppFront.h>
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedDocument.h"
#   include	"tedApp.h"
#   include	<appEditApplication.h>
#   include	<appGuiApplication.h>
#   include	<appEditDocument.h>
#   include	<textMsLocale.h>
#   include	<docParaProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a replace from one of the tools.				*/
/*  (Used from the Find and Spell tools)				*/
/*									*/
/************************************************************************/

int tedAppReplace(	void *		voidea,
			const char *	replacement )
    {
    int			rval;
    EditApplication *	ea= (EditApplication *)voidea;
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    rval= tedDocReplace( ed, replacement );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return rval;
    }

void tedAppDoNotSpellCheck(	void *		voidea )
    {
    EditApplication *	ea= (EditApplication *)voidea;
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    (void) tedDocChangeTextAttributeValue( ed, TApropNOPROOF, 1 );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppSetLocale(	void *		voidea,
			const char *	localeTag )
    {
    EditApplication *	ea= (EditApplication *)voidea;
    EditDocument *	ed= ea->eaCurrentDocument;
    int			lcId;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    lcId= textMatchMsLocaleIdByTag( localeTag );
    if  ( lcId < 0 )
	{ SLDEB(localeTag,lcId); return;	}

    (void) tedDocChangeTextAttributeValue( ed, TApropLOCALE, lcId );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

int tedAppSelectWholeSection(	struct EditApplication *	ea,
				int				direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    tedDocSelectWholeSection( ed, direction );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

int tedAppSelectWholeParagraph(	struct EditApplication *	ea,
				int				direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    tedDocSelectWholeParagraph( ed, direction );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

void tedAppSelectCurrentFrame(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectCurrentFrame( ed );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return;
    }

void tedAppSelectWholeCell(	EditApplication *	ea,
				int			direction,
				int			allRows )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectWholeCell( ed, direction, allRows );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSelectRow(		EditApplication *	ea,
				int			direction,
				int			allColumns )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectRow( ed, direction, allColumns );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSelectTable(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectTable( ed );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppGotoNoteDef(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocGotoNoteDef( ed );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteCurrentFrame(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteCurrentFrame( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteSelectedParagraphs(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteSelectedParagraphs( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteSelectedSections(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteSelectedSections( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppInsertParagraph(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocInsertParagraph( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppInsertSection(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocInsertSection( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetTocField(		EditApplication *	ea,
				const struct TocField *	tf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetTocField( ed, tf, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppAddTocField(		EditApplication *	ea,
				const struct TocField *	tf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocAddTocField( ed, tf, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteTocField(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteTocField( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetHyperlink(	EditApplication *	ea,
			const struct HyperlinkField *	hf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetHyperlink( ed, hf,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppRemoveHyperlink(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocRemoveHyperlink( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

int tedAppFindBookmarkField(	struct DocumentField **		pDf,
				EditApplication *		ea,
				const MemoryBuffer *		markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocFindBookmarkField( pDf, ed, markName ) )
	{ return -1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return 0;
    }

void tedAppSetBookmark(	EditApplication *	ea,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetBookmark( ed, markName,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppRemoveBookmark(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocRemoveBookmark( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

int tedAppGoToBookmark(	EditApplication *	ea,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocGoToBookmark( ed, markName ) )
	{ return -1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

void tedAppSetParaOutlineLevel(	EditApplication *	ea,
				int			level )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeParagraphPropertyValue( ed, PPpropOUTLINELEVEL, level );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppDeleteTable(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteTable( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteRow(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteRows( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppDeleteColumn(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteColumns( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppAddRowToTable(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocAddRowToTable( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppAddColumnToTable(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocAddColumnToTable( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppDeleteHeaderFooter(		EditApplication *	ea,
					int			treeType )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteHeaderFooter( ed, treeType,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppEditHeaderFooter(		EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocEditHeaderFooter( ed, relative, option, treeType );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppSetMask,
				const struct ParagraphProperties * ppNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeParagraphProperties( ed, ppSetMask, ppNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppChangeSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spSetMask,
				const struct SectionProperties *	spNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeSectionProperties( ed, spSetMask, spNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppChangeAllSectionProperties(
			EditApplication *			ea,
			const PropertyMask *			spSetMask,
			const struct SectionProperties *	spNew,
			const PropertyMask *			dpSetMask,
			const struct DocumentProperties *	dpSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeAllSectionProperties( ed,
				spSetMask, spNew, dpSetMask, dpSet,
				((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetParagraphTabs(	EditApplication *		ea,
				const struct TabStopList *	tsl )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetParagraphTabs( ed, tsl,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetDocumentProperties(
			EditApplication *			ea,
			const PropertyMask *			dpSetMask,
			const struct DocumentProperties *	dpSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetDocumentProperties( ed, dpSetMask, dpSet );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetNewList(	EditApplication *		ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetNewList( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetImageProperties(	EditApplication *		ea,
				const PropertyMask *		pipSetMask,
				const struct PictureProperties *	pip )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetImageProperties( ed, pipSetMask, pip,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppChangeCurrentNote(	EditApplication *	ea,
				const PropertyMask *	npSetMask,
				const struct NoteProperties *	npSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeCurrentNote( ed, npSetMask, npSet,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppGotoNoteRef(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocGotoNoteRef( ed );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppNextNote(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocNextNote( ed );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppPrevNote(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocPrevNote( ed );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

void tedAppShiftRowsInTable(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocShiftRowsInTable( ed, direction );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

void tedAppSetTableProperties(	EditApplication *		ea,
				int				wholeRow,
				int				wholeColumn,
				const PropertyMask *		cpSetMask,
				const struct CellProperties *	cpSet,
				const PropertyMask *		rpSetMask,
				const struct RowProperties *	rpSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetTableProperties( ed, wholeRow, wholeColumn,
				cpSetMask, cpSet, rpSetMask, rpSet );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return;
    }

int tedAppFollowLink(	APP_WIDGET		option,
			EditApplication *	ea,
			const MemoryBuffer *	fileName,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocFollowLink( option, ed, fileName, markName ) )
	{ return -1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

void tedAppChangeCurrentList(	EditApplication *		ea,
				const struct DocumentList *	dlNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeCurrentList( ed, dlNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    }

int tedAppFindNext(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocFindNext( ed ) )
	{ return 1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

int tedAppFindPrev(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocFindPrev( ed ) )
	{ return 1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

int tedAppListFontToolSet(	void *				voidea,
				const PropertyMask *		taSetMask,
				const struct ExpandedTextAttribute *	etaSet )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocListFontToolSet( ed, taSetMask, etaSet ) )
	{ LDEB(1); return -1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

int tedAppFontToolSet(		void *				voidea,
				const PropertyMask *		taSetMask,
				const struct ExpandedTextAttribute *	etaSet )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedDocFontToolSet( ed, taSetMask, etaSet ) )
	{ LDEB(1); return -1;	}

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

int tedAppFindToolSetPattern(	void *		voidea,
				const char *	pattern,
				int		useRegex,
				int		asWord,
				int		caseSensitive )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    tedDocFindToolSetPattern( (void *)ed,
				pattern, useRegex, asWord, caseSensitive );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );

    return 0;
    }

void tedAppInsertStringWithAttribute(	EditApplication *	ea,
					const char *		word,
					int			len,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocInsertStringWithAttribute( ed, word, len, taSet, taSetMask, 
				((TedDocument *)ed->edPrivateData)->tdTraced );

    appShowShellWidget( ea, ed->edToplevel.atTopWidget );
    return;
    }

struct BufferDocument * tedFormatCurDoc( EditDocument **	pEd,
					int *			pTraced,
					EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    struct BufferDocument *		bd;

    if  ( ! ed )
	{ XDEB(ed); return (struct BufferDocument *)0;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( pTraced )
	{ *pTraced= td->tdTraced;	}

    *pEd= ed; return bd;
    }


/************************************************************************/
/*									*/
/*  Display the online manual.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedAppManual, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    tedManual( option, ea, ea->eaToplevel.atTopWidget );
    }

