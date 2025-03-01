#   ifndef	TED_APP_FRONT_H
#   define	TED_APP_FRONT_H

#   include	<guiBase.h>

struct ParagraphProperties;
struct TabStopList;
struct ExpandedTextAttribute;
struct TextAttribute;
struct EditDocument;
struct BufferDocument;
struct EditApplication;
struct SpellScanJob;
struct CellProperties;
struct RowProperties;
struct PictureProperties;
struct HyperlinkField;
struct DocumentProperties;
struct NoteProperties;
struct SectionProperties;
struct DocumentList;
struct DocumentField;
struct TocField;
struct PropertyMask;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedAppReplace(		void *			voidea,
					const char *		replacement );

extern void tedAppDoNotSpellCheck(	void *			voidea );
extern void tedAppSetLocale(		void *			voidea,
					const char *		localeTag );

extern void tedAppAddRowToTable(	struct EditApplication * ea,
					int			after );

extern void tedAppAddColumnToTable(	struct EditApplication *	ea,
					int			after );

extern void tedAppSetTableProperties(
				struct EditApplication *	ea,
				int				wholeRow,
				int				wholeColumn,
				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,
				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet );

extern void tedAppSetImageProperties(
				struct EditApplication *	ea,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pip );

extern void tedAppShiftRowsInTable(	struct EditApplication *	ea,
					int			direction );

extern void tedAppSetHyperlink(	struct EditApplication *	ea,
				const struct HyperlinkField *	hf );

extern void tedAppRemoveHyperlink(	struct EditApplication *	ea );

extern int tedAppFollowLink(	APP_WIDGET		option,
				struct EditApplication *	ea,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	markName );

extern void tedAppDeleteSelectedParagraphs(	struct EditApplication *	ea );
extern void tedAppDeleteSelectedSections(	struct EditApplication *	ea );

extern void tedAppInsertParagraph(	struct EditApplication *	ea,
					int			after );

extern void tedAppInsertSection(	struct EditApplication *	ea,
				int			after );

extern void tedAppSetDocumentProperties(
			struct EditApplication *		ea,
			const struct PropertyMask *		dpSetMask,
			const struct DocumentProperties *	dpSet );

extern void tedAppChangeCurrentNote(
				struct EditApplication *		ea,
				const struct PropertyMask *		npSetMask,
				const struct NoteProperties *		npSet );

extern void tedAppChangeSectionProperties(
				struct EditApplication *		ea,
				const struct PropertyMask *		spUpdMask,
				const struct SectionProperties *	spNew );

extern void tedAppChangeAllSectionProperties(
				struct EditApplication *		ea,
				const struct PropertyMask *		spUpdMask,
				const struct SectionProperties *	spNew,
				const struct PropertyMask *		dpSetMask,
				const struct DocumentProperties *	dpNew );

extern void tedAppChangeParagraphProperties(
			struct EditApplication *			ea,
			const struct PropertyMask *			ppUpdMask,
			const struct ParagraphProperties *	ppNew );

extern void tedAppSetParagraphTabs(
				struct EditApplication *		ea,
				const struct TabStopList *	tsl );

extern void tedAppSetNewList(	struct EditApplication *		ea );

extern void tedAppChangeCurrentList(
				struct EditApplication *		ea,
				const struct DocumentList *	dlNew );

extern void tedAppDeleteHeaderFooter(	struct EditApplication *	ea,
					int			treeType );

extern void tedAppEditHeaderFooter(	struct EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType );

extern void tedAppGotoNoteDef(		struct EditApplication *	ea );
extern void tedAppGotoNoteRef(		struct EditApplication *	ea );
extern void tedAppNextNote(		struct EditApplication *	ea );
extern void tedAppPrevNote(		struct EditApplication *	ea );

extern void tedAppSelectTable(		struct EditApplication *	ea );
extern void tedAppSelectRow(		struct EditApplication *	ea,
					int			direction,
					int			allColumns );
extern void tedAppSelectWholeCell(	struct EditApplication *	ea,
					int			direction,
					int			allRows );

extern void tedAppDeleteTable(		struct EditApplication *	ea );
extern void tedAppDeleteRow(		struct EditApplication *	ea );
extern void tedAppDeleteColumn(		struct EditApplication *	ea );

extern void tedAppSelectCurrentFrame(	struct EditApplication *	ea );
extern void tedAppDeleteCurrentFrame(	struct EditApplication *	ea );

extern void tedAppSetBookmark(	struct EditApplication *	ea,
				const struct MemoryBuffer *	markName );

extern int tedAppGoToBookmark(	struct EditApplication *	ea,
				const struct MemoryBuffer *	markName );

extern void tedAppRemoveBookmark(struct EditApplication *	ea );

extern int tedAppFindBookmarkField(
				struct DocumentField **		pDf,
				struct EditApplication *		ea,
				const struct MemoryBuffer *		markName );

extern void tedAppSetTocField(	struct EditApplication *		ea,
				const struct TocField *		tf );

extern void tedAppDeleteTocField( struct EditApplication *		ea );

extern void tedAppAddTocField(	struct EditApplication *		ea,
				const struct TocField *		tf );

extern int tedAppFindNext(		void *		voidea );

extern int tedAppFindPrev(		void *		voidea );

extern int tedAppListFontToolSet(
				void *				voidea,
				const struct PropertyMask *		taSetMask,
				const struct ExpandedTextAttribute *	etaSet );

extern int tedAppFontToolSet(
			void *					voidea,
			const struct PropertyMask *		taSetMask,
			const struct ExpandedTextAttribute *	etaSet );

extern void tedAppSetParaOutlineLevel(
				struct EditApplication *	 ea,
				int				level );

extern int tedAppSelectWholeSection(	struct EditApplication *	ea,
					int			direction );

extern int tedAppSelectWholeParagraph(	struct EditApplication * ea,
					int			direction );

extern int tedAppFindToolSetPattern(	void *		voidea,
					const char *	pattern,
					int		useRegex,
					int		asWord,
					int		caseSensitive );

extern void tedAppInsertStringWithAttribute( struct EditApplication *	ea,
					const char *		word,
					int			len,
					const struct TextAttribute *	taSet,
					const struct PropertyMask *	taSetMask );

extern struct BufferDocument * tedFormatCurDoc(
					struct EditDocument **		pEd,
					int *				pTraced,
					struct EditApplication *		ea );

extern int tedSpellFindNext(		void *				voidea,
					struct MemoryBuffer *			mbGuess,
					struct SpellScanJob *		ssj );

#   endif	/*  TED_APP_FRONT_H	*/
