#   ifndef	TED_DOC_FRONT_H
#   define	TED_DOC_FRONT_H

#   include	<guiBase.h>

struct ParagraphProperties;
struct TabStopList;
struct DocumentStatistics;
struct CellProperties;
struct PictureProperties;
struct HyperlinkField;
struct NoteProperties;
struct SectionProperties;
struct TocField;
struct ExpandedTextAttribute;
struct RowProperties;
struct DocumentProperties;
struct DocumentList;
struct DocumentField;
struct TextAttribute;
struct PropertyMask;
struct EditDocument;
struct DocumentGeometry;
struct EditApplication;
struct MemoryBuffer;
struct DocumentSelection;
struct BufferDocument;
struct SelectionDescription;
struct SelectionGeometry;
struct DocumentTree;
struct AppInspector;
struct DocumentPosition;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedAdaptFontIndicatorsToSelection(	struct EditDocument *	ed );
extern void tedAdaptToolsToSelection(		struct EditDocument *	ed );

extern int tedDocReplace(		struct EditDocument *		ed,
					const char *			word );

extern int tedDocAddRowToTable(		struct EditDocument *		ed,
					int			after,
					int			traced );

extern int tedDocAddColumnToTable(	struct EditDocument *	ed,
					int			after,
					int			traced );

extern int tedDocSetTableProperties(
				struct EditDocument *		ed,
				int				wholeRow,
				int				wholeColumn,
				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,
				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet );

extern int tedDocSetImageProperties(
				struct EditDocument *			ed,
				const struct PropertyMask *		pipSetMask,
				const struct PictureProperties *	pip,
				int				traced );

extern int tedDocSetHyperlink(	struct EditDocument *		ed,
				const struct HyperlinkField *	hf,
				int			traced );

extern int tedDocRemoveHyperlink(	struct EditDocument *		ed,
					int			traced );

extern int tedDocFollowLink(	APP_WIDGET		option,
				struct EditDocument *		ed,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	markName );

extern int tedDocDeleteSelectedParagraphs(
				struct EditDocument *		ed,
				int			traced );

extern int tedDocDeleteSelectedSections(
				struct EditDocument *		ed,
				int			traced );

extern int tedDocInsertParagraph(	struct EditDocument *		ed,
					int			after,
					int			traced );

extern int tedDocInsertSection(	struct EditDocument *		ed,
				int			after,
				int			traced );

extern int tedDocSetDocumentProperties(
				struct EditDocument *		ed,
				const struct PropertyMask *	dpSetMask,
				const struct DocumentProperties * dpSet );

extern int tedDocChangeCurrentNote(
				struct EditDocument *		ed,
				const struct PropertyMask *	npSetMask,
				const struct NoteProperties *	npSet,
				int				traced );

extern int tedDocChangeSectionProperties(
				struct EditDocument *			ed,
				const struct PropertyMask *		spUpdMask,
				const struct SectionProperties *	spNew,
				int				traced );

extern int tedDocChangeAllSectionProperties(
				struct EditDocument *			ed,
				const struct PropertyMask *		spUpdMask,
				const struct SectionProperties * spNew,
				const struct PropertyMask *		dpSetMask,
				const struct DocumentProperties * dpNew,
				int				traced );

extern int tedDocChangeParagraphProperties(
				struct EditDocument *		ed,
				const struct PropertyMask *	ppUpdMask,
				const struct ParagraphProperties * ppNew,
				int				traced );

extern int tedDocChangeParagraphPropertyValue(
				struct EditDocument *		ed,
				int				prop,
				int				value );

extern int tedDocSetParagraphTabs(
				struct EditDocument *		ed,
				const struct TabStopList *	tsl,
				int				traced );

extern int tedDocSetNewList(	struct EditDocument *		ed,
				int				traced );

extern int tedDocChangeCurrentList(
				struct EditDocument *		ed,
				const struct DocumentList *	dlNew,
				int				traced );

extern int tedDocDeleteHeaderFooter(	struct EditDocument *		ed,
					int			treeType,
					int			traced );

extern int tedDocInsertHeaderFooter(	struct EditDocument *		ed,
					int			treeType,
					int			traced );

extern void tedDocEditHeaderFooter(	struct EditDocument *		ed,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType );

extern int tedDocInsertNote(		struct EditDocument *		ed,
					int			noteTreeType,
					int			traced );

extern void tedDocGotoNoteDef(		struct EditDocument *		ed );
extern void tedDocGotoNoteRef(		struct EditDocument *		ed );

extern void tedGotoNoteOther(	struct EditDocument *		ed,
				const struct DocumentField *	dfChftn,
				const struct DocumentPosition *	dpClick );

extern void tedDocNextNote(		struct EditDocument *		ed );
extern void tedDocPrevNote(		struct EditDocument *		ed );

extern void tedDocSelectTable(		struct EditDocument *		ed );
extern void tedDocSelectRow(		struct EditDocument *		ed,
					int			direction,
					int			allColumns );
extern void tedDocSelectWholeCell(	struct EditDocument *		ed,
					int			direction,
					int			allRows );

extern int tedDocSelectWholeSection(	struct EditDocument *	ed,
					int			direction );

extern int tedDocSelectWholeParagraph(	struct EditDocument *	ed,
					int			direction );

extern int tedDocSelectColumn(		struct EditDocument *		ed,
					int			col );

extern int tedDocDeleteTable(		struct EditDocument *	d,
					int			traced );
extern int tedDocDeleteRows(		struct EditDocument *	ed,
					int			traced );
extern int tedDocDeleteColumns(		struct EditDocument *	ed,
					int			traced );

extern int tedDocSelectCurrentFrame(	struct EditDocument *	ed );
extern int tedDocDeleteCurrentFrame(	struct EditDocument *	ed,
					int			traced );

extern int tedDocSetBookmark(	struct EditDocument *		ed,
				const struct MemoryBuffer *	markName,
				int			traced );

extern int tedDocGoToBookmark(	struct EditDocument *		ed,
				const struct MemoryBuffer *	markName );

extern int tedDocRemoveBookmark(struct EditDocument *		ed,
				int			traced );

extern int tedDocFindBookmarkField(
				struct DocumentField **		pDf,
				struct EditDocument *			ed,
				const struct MemoryBuffer *		markName );

extern int tedDocSetTocField(	struct EditDocument *			ed,
				const struct TocField *		tf,
				int				traced );

extern void tedDocDeleteTocField( struct EditDocument *		ed,
				int				traced );

extern void tedDocAddTocField(	struct EditDocument *			ed,
				const struct TocField *		tf,
				int				traced );

extern void tedDocChangeTextAttribute(	struct EditDocument *		ed,
					const struct PropertyMask *	taSetMask,
					const struct TextAttribute *	taSet,
					int			traced );

extern int tedDocShiftRowsInTable(	struct EditDocument *		ed,
					int			direction );

extern int tedDocListFontToolSet( struct EditDocument *		ed,
				const struct PropertyMask *	taSetMask,
				const struct ExpandedTextAttribute *	etaSet );

extern int tedDocFontToolSet(	struct EditDocument *		ed,
				const struct PropertyMask *	taSetMask,
				const struct ExpandedTextAttribute *	etaSet );

extern void tedDocSetPageLayout( struct EditDocument *		ed,
				const struct PropertyMask *	dgSetMask,
				const struct DocumentGeometry *	dgSet,
				int				wholeDocument,
				int				traced );

extern int tedFormatShiftIndent( struct EditDocument *		ed,
				int				step,
				int				traced );

extern int tedDocInsertImageFile(
				struct EditDocument *		ed,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				const struct MemoryBuffer *	filename );

extern void tedDocSplitParagraph( struct EditDocument *		ed,
				int				breakKind,
				int				editCommand,
				int				traced );

extern int tedDocInsertSectBreak( struct EditDocument *		ed,
				int				traced );

extern void tedAdaptFormatToolToDocument(	struct EditDocument *	ed,
						int		choosePage );

extern void tedDocChangeTextAttributeValue(
					struct EditDocument *		ed,
					int				prop,
					int				value );

extern int tedDocFindNext(		struct EditDocument *		ed );
extern int tedDocFindPrev(		struct EditDocument *		ed );

extern int tedDocFindToolSetPattern(	void *		voided,
					const char *	pattern,
					int		useRegex,
					int		asWord,
					int		caseSensitive );

extern void tedAdaptPageToolToDocument(	struct EditDocument *		ed );

extern int tedDocUnlock(		struct EditDocument *		ed );
extern int tedDocRecover(		struct EditDocument *		ed );

extern void tedDocSelAll(		struct EditDocument *		ed );

extern void tedObserveFocus(		struct EditDocument *		ed,
					int			inout );

extern int tedRunPropertyDialog( struct EditDocument *			ed,
				const struct DocumentProperties *	dpSet,
				const struct DocumentStatistics * ds,
				APP_WIDGET			option );

extern void tedDocInsertStringWithAttribute(
					struct EditDocument *	ed,
					const char *		bytes,
					int			size,
					const struct TextAttribute *	taSet,
					const struct PropertyMask *	taSetMask,
					int			traced );

extern void tedDocUndo(			struct EditDocument *	ed );
extern int tedDocRepeat(		struct EditDocument *	ed );

extern void tedDocToolShowFindTool(
				struct EditDocument *			ed,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd );

extern void tedDocRefreshFormatTool(
				struct EditDocument *			ed,
				struct AppInspector *			ai,
				struct BufferDocument *			bd,
				struct DocumentTree *			ei,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedDocBuildFormatTool(
				struct EditDocument *			ed );

#   endif	/*  TED_DOC_FRONT_H	*/
