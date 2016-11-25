#   include	<appFrame.h>
#   include	<appImage.h>

#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>
#   include	<docExpandedTextAttribute.h>

#   include	<geo2DInteger.h>
#   include	<appDraw.h>
#   include	"docBuf.h"

#   ifndef	TED_APP_H
#   define	TED_APP_H

#   define	RESIZE_BLOCK	10

/************************************************************************/
/*									*/
/*  Private data for a document.					*/
/*									*/
/************************************************************************/

typedef struct TedDocument
    {
    BufferDocument *	tdDocument;
    TextAttribute	tdCurrentTextAttribute;
    int			tdCurrentTextAttributeNumber;
    int			tdCurrentPhysicalFont;

    DocumentSelection	tdDocumentSelection;
    SelectionGeometry	tdSelectionGeometry;

    APP_WIDGET		tdCopyWidget;
    APP_WIDGET		tdCutWidget;
    APP_WIDGET		tdPasteWidget;

    APP_WIDGET		tdInsertMenu;
    APP_WIDGET		tdInsertMenuButton;
    APP_WIDGET		tdInsPictOption;
    APP_WIDGET		tdInsFileOption;
    APP_WIDGET		tdInsSymbolOption;
    APP_WIDGET		tdInsHyperlinkOption;
    APP_WIDGET		tdInsBookmarkOption;
    APP_WIDGET		tdInsInsertFootnoteOption;
    APP_WIDGET		tdInsInsertChftnsepOption;
    APP_WIDGET		tdInsInsertTableOption;
    APP_WIDGET		tdInsInsertPageNumberOption;
    APP_WIDGET		tdInsInsertLineBreakOption;
    APP_WIDGET		tdInsInsertPageBreakOption;
    APP_WIDGET		tdInsInsertSectBreakOption;

    APP_WIDGET		tdTableMenu;
    APP_WIDGET		tdTableMenuButton;
    APP_WIDGET		tdTabInsertTableOption;
    APP_WIDGET		tdTabAddRowOption;
    APP_WIDGET		tdTabAddColumnOption;

    APP_WIDGET		tdFormatMenu;
    APP_WIDGET		tdFormatMenuButton;
    APP_WIDGET		tdFormatOneParaOption;

    APP_WIDGET		tdSelectTableWidget;
    APP_WIDGET		tdSelectRowWidget;
    APP_WIDGET		tdSelectColumnOption;
    APP_WIDGET		tdDrawTableGridOption;

    APP_WIDGET		tdFontMenu;
    APP_WIDGET		tdFontMenuButton;
    APP_WIDGET		tdFontBoldOption;
    APP_WIDGET		tdFontItalicOption;
    APP_WIDGET		tdFontUnderlinedOption;
    APP_WIDGET		tdFontSuperscriptOption;
    APP_WIDGET		tdFontSubscriptOption;

    APP_WIDGET		tdToolsMenu;
    APP_WIDGET		tdToolsMenuButton;
    APP_WIDGET		tdToolsFormatToolOption;

    APP_COLOR_RGB	tdFieldColor;

    RGB8Color		tdTableColor;
    RGB8Color		tdSelColor;
    RGB8Color		tdCopiedSelColor;

    int			tdVisibleSelectionCopied;
    int			tdCanReplaceSelection;
    int			tdCopiedSelectionClosed;
    MemoryBuffer	tdCopiedSelection;
    MemoryBuffer	tdCopiedFont;
    MemoryBuffer	tdCopiedRuler;
    AppBitmapImage	tdCopiedImage;

#   ifdef USE_MOTIF
    XtIntervalId	tdHideIBarId;
    XtIntervalId	tdShowIBarId;
#   endif

#   ifdef USE_GTK
    guint		tdHideIBarId;
    guint		tdShowIBarId;
#   endif

    APP_WINDOW		tdObjectWindow;
    APP_WINDOW		tdObjectBottomWindow;
    APP_WINDOW		tdObjectRightWindow;
    APP_WINDOW		tdObjectCornerWindow;
    int			tdObjectSelected;

    int			tdDrawTableGrid;
    } TedDocument;

/************************************************************************/
/*									*/
/*  Ted specific resources.						*/
/*									*/
/*  Also: Some global data items.					*/
/*									*/
/************************************************************************/

typedef struct TedAppResources
    {
				/****************************************/
				/*  Texts.				*/
				/****************************************/

    char *			tarFileNotRtf;
    char *			tarHeadFootNotReachable;

    char *			tarAppFileMenuText;
    char *			tarAppWinMenuText;
    char *			tarDocFileMenuText;
    char *			tarDocEditMenuText;
    char *			tarDocInsertMenuText;
    char *			tarDocWinMenuText;
    char *			tarDocFontMenuText;
    char *			tarDocFormatMenuText;
    char *			tarDocToolMenuText;
    char *			tarDocTableMenuText;
    char *			tarHelpMenuText;
    char *			tarAppHelpFileName;

    char *			tarSelectionColor;
    char *			tarCopiedSelectionColor;

				/****************************************/
				/*  Defaults.				*/
				/****************************************/
    char *			tarDefaultAnsicpgString;
    int				tarDefaultAnsicpgInt;

    char *			tarShowTableGridString;
    int				tarShowTableGridInt;

				/****************************************/
				/*  Global data.			*/
				/****************************************/

    AppInspector *		tarInspector;
    EditDocument *		tarManualDocument;
    } TedAppResources;

/************************************************************************/
/*									*/
/*  Used on 'Hyperlink' and 'Bookmark' dialogs.				*/
/*									*/
/************************************************************************/

typedef enum LinkKind
    {
    LINKkindHYPERLINK,
    LINKkindPAGE_NUMBER,
    LINKkindBOOKMARK_TEXT,
    LINKkindTEXT_PLUS_PAGE,

    LINKkind_COUNT
    } LinkKind;

/************************************************************************/
/*									*/
/*  Supported document formats.						*/
/*  Match an array in tedMain.c.					*/
/*									*/
/************************************************************************/

typedef enum TedDocumentKind
    {
    TEDdockindRTF= 0,
    TEDdockindTEXT_OPEN,
    TEDdockindTEXT_SAVE_FOLDED,
    TEDdockindTEXT_SAVE_WIDE,
    TEDdockindHTML_FILES,
    TEDdockindALL_FILES,

    TEDdockind_COUNT
    } TedDocumentKind;

/************************************************************************/
/*									*/
/*  Shared resources.							*/
/*									*/
/************************************************************************/

extern const int TedPrimaryTextTargetCount;
extern AppSelectionTargetType TedPrimaryTextTargets[];

extern const int TedPrimaryPictureTargetCount;
extern AppSelectionTargetType TedPrimaryPictureTargets[];

extern const int TedRulerTargetCount;
extern AppSelectionTargetType TedRulerTargets[];

/************************************************************************/
/*									*/
/*  Geometry calculations.						*/
/*									*/
/************************************************************************/

extern int tedLayoutItem(	BufferItem *			bi,
				BufferDocument *		bd,
				AppDrawingData *		add,
				DocumentRectangle *		drChanged );

extern int tedLayoutExternalItem(
				int *				pY1Twips,
				ExternalItem *			ei,
				int				page,
				int				y0Twips,
				BufferDocument *		bd,
				AppDrawingData *		add,
				DocumentRectangle *		drChanged );

extern int tedFindPosition(	DocumentPosition *		dp,
				PositionGeometry *		pg,
				const BufferDocument *		bd,
				BufferItem *			rootBi,
				const AppDrawingData *		add,
				int				x,
				int				y );

extern int tedFindParticule(	TextLine *			tl,
				TextParticule *			tp,
				int				x,
				int				y );

extern int tedFindStringOffset(	const BufferDocument *		bd,
				const BufferItem *		paraBi,
				int				part,
				const AppDrawingData *		add,
				int *				pBarX,
				int				x,
				int				y );

extern int tedHasSelection(	const TedDocument *		td );
extern int tedHasIBarSelection(	const TedDocument *		td );

extern int tedArrowDown(	DocumentPosition *		bp,
				const BufferDocument *		bd,
				const AppDrawingData *		add );

extern int tedLineUp(		DocumentPosition *		bp,
				const BufferDocument *		bd,
				const AppDrawingData *		add );

extern int tedPageUp(	DocumentPosition *			dp,
			BufferDocument *			bd,
			const AppDrawingData *			add,
			int					pageHeight );

extern int tedPageDown(	DocumentPosition *			dp,
			BufferDocument *			bd,
			const AppDrawingData *			add,
			int					docHeight,
			int					pageHeight );

extern APP_EVENT_HANDLER_H( tedObserveFocus, w, voided, event );

extern void tedSplitParagraph(		EditDocument *		ed,
					int			onNewPage );

extern void tedEditReplaceSelectionWithTab(	EditDocument *		ed );

extern int tedChangeSelectionProperties(
				EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern void tedRedrawRectangle(	APP_WIDGET		w,
				TedDocument *		td,
				DocumentRectangle *	drClip,
				AppDrawingData *	add,
				AppColors *		ac,
				int			ox,
				int			oy );

extern void tedDelimitCurrentSelection(	TedDocument *		td,
					AppDrawingData *	add );

extern void tedDocToolFind(		APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs );

extern void tedDocToolSpell(		APP_WIDGET	spellOption,
					void *		voided,
					void *		voidcbs );

extern int tedIncludePlainDocument(	APP_WIDGET		w,
					EditDocument *		ed,
					BufferDocument *	bdFrom );

extern int tedIncludeRtfDocument(	APP_WIDGET		w,
					EditDocument *		ed,
					BufferDocument *	bdFrom );

extern int tedGetDocumentAttributeString(	char *		scratch,
						TedDocument *	td );

extern int tedGetDocumentAttributes(
				TedDocument *			td,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew );

extern void tedGetSelectionAttributes(
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew );

extern void tedDocCopy(		EditDocument *	ed );
extern void tedDocCut(		EditDocument *	ed );

extern void tedDocSelAll(	EditDocument *	ed );

extern void tedSetSelection(	EditDocument *			ed,
				const DocumentSelection *	dsSet,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedDocEditPaste(	APP_WIDGET			option,
				void *				voided,
				void *				voidpbcs );

extern void tedDocReplaceSelection(	EditDocument *		ed,
					const unsigned char *	word,
					int			len );

extern int tedDeleteTableSliceSelection(	EditDocument *	ed );

extern void tedSetSelectedPosition(
				EditDocument *			ed,
				const DocumentPosition *	dp,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedAdaptFontIndicatorsToSelection(	EditApplication *	ea,
						EditDocument *		ed );

extern void tedAdaptToolsToSelection(	EditDocument *		ed );

extern int tedExtendSelectionToXY(
				EditDocument *			ed,
				BufferItem *			rootBi,
				const DocumentPosition *	bpAnchor,
				int				mouseX,
				int				mouseY );

extern int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound );

extern void tedScrollToSelection(	EditDocument *		ed,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedDocFontBold(		APP_WIDGET	boldOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontItalic(	APP_WIDGET	italicOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontUnderlined(	APP_WIDGET	underlineOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontSupersub(		APP_WIDGET	option,
					void *		voided,
					void *		voidcbs	 );

extern int tedLayoutDocumentTree(	TedDocument *		td,
					AppDrawingData *	add );

extern void tedScreenRectangles(	AppDrawingData *		add,
					const DocumentGeometry *	dg );

extern void tedAdaptPageToolToDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void tedAppReplace(	void *			voidea,
				const unsigned char *	word	);

extern void tedGetNamedPictures(	EditApplication *	ea );

extern int tedAppChangeSelectionAttributeString( EditDocument *	ed,
					char *		attributeString );

extern int tedDocSetTopRuler(	EditDocument *	ed );
extern int tedDocSetLeftRuler(	EditDocument *	ed );

extern void tedDocAdaptHorizontalRuler(	EditDocument *		ed,
					BufferItem *		bi );

extern void tedUndrawIBar(	const EditDocument *	ed );

extern int tedDrawIBar(		const PositionGeometry *	pg,
				int				ox,
				int				oy,
				AppDrawingData *		add );

extern void tedStartCursorBlink(	EditDocument *	ed );
extern void tedStopCursorBlink(		EditDocument *	ed );
extern void tedCleanCursorBlink(	TedDocument *	td );

extern int tedOpenItemObjects(	BufferItem *		bi,
				AppColors *		ac,
				AppDrawingData *	add );

extern int tedDrawObject(	const BufferItem *	bi,
				const TextParticule *	tp,
				int			baseline,
				int			ox,
				int			oy,
				AppDrawingData *	add );

extern void tedCloseObject(		BufferDocument *	bd,
					BufferItem *		bi,
					TextParticule *		tp,
					void *			voidadd );

extern void tedDocInsertPicture(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertFile(		APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertLink(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs );

extern void tedDocInsertBookmark(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertFootnote(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertChftnsep(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertPageNumber(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertLineBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertPageBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertSectBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void TEDmiDocInsertInsSectBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocFormatOnePara(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern int tedReplaceSelectionWithObject(
					EditDocument *		ed,
					InsertedObject *	io );

extern APP_PASTE_REPLY( tedPastePrimaryRtf, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided );
extern APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteFontTed, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryPixmap, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryPng, w, event, voided );

extern APP_GIVE_COPY( tedCopyPrimaryRtf, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryPng, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryPixmap, w, event, voided );
extern APP_GIVE_COPY( tedCopyFontTed, w, event, voided );
extern APP_GIVE_COPY( tedCopyRulerTed, w, event, voided );

extern int tedCopyAsLink(	EditDocument *			ed,
				const DocumentSelection *	ds,
				int				asRef,
				int				asPageref,
				const char *			file,
				const char *			markName,
				int				markSize );

extern void tedPrimaryLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern InsertedObject * tedObjectMakeBitmapObject(
				    	EditDocument *		ed,
					BufferItem *		bi,
					AppBitmapImage *	abi );

extern int tedGetObjectSelection(	TedDocument *   	td,
					DocumentPosition *	dpObject,
					InsertedObject **	pIo );

extern void tedSetObjectWindows(EditDocument *			ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				int				ox,
				int				oy );

extern void tedMoveObjectWindows(	EditDocument *		ed );

extern int tedResizeObject(		EditDocument *			ed,
					const DocumentPosition *	dpObj,
					const PositionGeometry *	pgObj );

extern int tedReopenObject(	BufferDocument *	bd,
				BufferItem *		bi,
				TextParticule *		tp,
				AppColors *		ac,
				AppDrawingData *	add );

extern int tedSaveObjectPicture(	AppBitmapImage *	abiTo,
					InsertedObject *	io );

extern void tedExposeSelection(	const EditDocument *		ed,
				const DocumentSelection *	ds,
				int				scrolledX,
				int				scrolledY );

extern void tedPositionGeometry(	PositionGeometry *		pg,
					const DocumentPosition *	dp,
					const BufferDocument *		bd,
					const AppDrawingData *		add );

extern void tedSelectionGeometry(	SelectionGeometry *		sg,
					const DocumentSelection *	ds,
					const BufferDocument *		bd,
					const AppDrawingData *		add );

extern void tedDrawRectangle(	APP_WIDGET		w,
				EditDocument *		ed,
				DocumentRectangle *	drClip,
				int			ox,
				int			oy );

extern void tedDocHorizontalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );

extern void tedDocVerticalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );

extern int tedFinishDocumentSetup(	EditApplication *	ea,
					EditDocument *		ed );

extern void tedDocFormatCopyRul(	APP_WIDGET	fontsOption,
					void *		voided,
					void *		voidpbcbs	 );

extern void tedDocFormatPasteRul(	APP_WIDGET	fontsOption,
					void *		voided,
					void *		voidpbcbs	 );

extern int tedLayoutDocument(	void *				privateData,
				int				format,
				AppDrawingData *		add,
				const DocumentGeometry *	defDg );

extern int tedMakeDocumentWidget(	EditApplication *	ea,
					EditDocument *		ed );

extern int tedInsertTable(		EditDocument *		ed,
					int			rows,
					int			columns );

extern int tedInsertColumnInTable(	EditDocument *		ed );
extern int tedAppendColumnToTable(	EditDocument *		ed );
extern int tedInsertRowInTable(		EditDocument *		ed );
extern int tedAppendRowToTable(		EditDocument *		ed );

extern void tedDocTableSelectTable(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs );

extern void tedDocTableSelectRow(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs );

extern void tedDocTableSelectColumn(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs );

extern void tedDocToolInsertSymbol(	APP_WIDGET	symbolOption,
					void *		voided,
					void *		voidcbs );

extern void tedDocSetTableProperties(	EditDocument *		ed,
					const TableRectangle *	tr,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedAppSetTableProperties(	EditApplication *	ea,
					const TableRectangle *	tr,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedAdaptFormatToolToDocument(	EditDocument *	ed );

extern void tedAppSetTableSelection(	EditDocument *		ed,
					const TableRectangle *	tr );

extern void tedDocTableSelectTableRectangle(	EditDocument *		ed,
						const TableRectangle *	tr );

extern void tedChangeTableLayout(	EditDocument *		ed,
					BufferItem *		sectBi,
					int			row0Change,
					int			row1Change,
					int			col0Change,
					int			col1Change,
					int			row1Reformat,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedDocTableDrawGrid(	APP_WIDGET	option,
					void *		voided,
					void *		voidcbs );

extern void tedMergeParagraphsInSelection(	EditDocument *	ed );

extern int tedDeleteRowsFromTable(	EditDocument *		ed,
					int			delRow0,
					int			delRow1 );

extern int tedDeleteColumnsFromRows(	EditDocument *	ed,
					int		delRow0,
					int		delRow1,
					int		delCol0,
					int		delCol1 );

extern int tedRunPropertyDialog( EditDocument *			ed,
				const DocumentStatistics *	ds,
				APP_WIDGET			option,
				const char *			pixmapName );

extern int tedSetHyperlink(	EditDocument *		ed,
				const char *		file,
				const char *		mark,
				int			asRef,
				int			asPageref );

extern int tedSetBookmark(	EditDocument *		ed,
				const char *		mark );

extern int tedRemoveHyperlink(	EditDocument *		ed );

extern int tedRemoveBookmark(	EditDocument *		ed );

extern int tedCopyBookmarkAsLink(	EditDocument *	ed,
					int		asRef,
					int		asPageref,
					const char *	link );

extern void tedRunLinkDialog(	EditApplication *	ea,
				EditDocument *		ed,
				APP_WIDGET		option,
				const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize );

extern void tedRunBookmarkDialog(	EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		option,
					int			nameProposed,
					const char *		markName,
					int			markSize );

extern int tedGoToBookmark(	EditDocument *		ed,
				const char *		markName,
				int			markSize );

extern int tedSaveDocument(	const void *			privateData,
				int				format,
				const char *			applicationId,
				const char *			documentTitle,
				const char *			filename );

extern void * tedMakePrivateData( void );

extern void tedMakeDocumentReadonly(	EditDocument *	ed );

extern int tedFollowLink(	APP_WIDGET		relative,
				APP_WIDGET		option,
				EditDocument *		edFrom,
				const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize );

extern int tedSelectWholeParagraph(	EditApplication *	ea,
					int			direction );

extern int tedSelectWholeSection(	EditApplication *	ea,
					int			direction );

extern int tedDeleteCurrentParagraph(	EditApplication *	ea );
extern int tedDeleteCurrentSection(	EditApplication *	ea );

extern int tedInsertParagraph(	EditApplication *	ea,
				int			after );

extern int tedInsertSection(	EditApplication *	ea,
				int			after );

extern int tedGetParaLineHeight(	int *			pLineHeight,
					EditDocument *		ed );

extern void tedShowFormatTool(	APP_WIDGET		toolOption,
				EditApplication *	ea,
				const char *		widgetName,
				const char *		pixmapName );

extern void tedFormatToolAdaptToSelection( AppInspector *	ai,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				int				fileReadonly );

extern int tedNewDocument(	EditApplication *	ea,
				EditDocument *		ed,
				const char *		filename );

extern int tedPrintDocument(	SimpleOutputStream *		sos,
				const PrintJob *		pj,
				const PrintGeometry *		pg,
				int				firstPage,
				int				lastPage );

extern void tedFreeDocument(		void *			voidtd,
					int			format,
					AppDrawingData *	add );

extern void tedRedoDocumentLayout(	EditDocument *		ed );

extern void tedFormatShowPagePage( EditApplication *		ea );
extern void tedFormatShowFontPage( EditApplication *		ea );

extern void tedSetPageLayout(	EditApplication *		ea,
				const DocumentGeometry *	dg,
				const PropertyMask *		setMask,
				int				whileDocument );

extern int tedSetDocumentProperties(
				EditApplication *		ea,
				const DocumentProperties *	dpNew,
				const PropertyMask *		updMask );

extern int tedChangeCurrentNote(
				EditApplication *		ea,
				int				extItKind );

extern APP_EVENT_HANDLER_H( tedKeyPressed, w, voided, keyEvent );
extern APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent );

extern void tedHideObjectWindows(	EditDocument *	ed );

extern int tedAppChangeSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern int tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew );

extern int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew );

extern void tedAppDeleteHeaderFooter(	EditApplication *	ea,
					int			which );

extern void tedAppEditHeaderFooter(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			which );

extern int tedGetSelection(	DocumentSelection *	ds,
				SelectionGeometry *	sg,
				TedDocument *		td );

extern void tedFillBookmarkList(	APP_WIDGET		list,
					DocumentFieldList *	dfl );

extern int tedTextWidth(		const AppPhysicalFont *		apf,
					const TextAttribute *		ta,
					const BufferDocument *		bd,
					const AppDrawingData *		add,
					const unsigned char *		s,
					int				len );

extern int tedFindRootForPosition(	ExternalItem **		pEi,
					BufferItem **		pRootBi,
					BufferItem **		pBodySectBi,
					int *			pSectNr,
					int *			pPage,
					EditDocument *		ed,
					int			docX,
					int			docY );

extern void tedDetermineDefaultSettings(	TedAppResources *	tar );

extern int tedReplaceSelectionWithBitmapImage(	EditDocument *		ed,
						AppBitmapImage *	abi );

extern void tedAppEditNote(		EditApplication *	ea );
extern void tedAppGotoNoteRef(		EditApplication *	ea );

extern int tedSetIBarSelection(		EditDocument *		ed,
					BufferItem *		bi,
					int			stroff,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedDrawShadedRectangle(
			AppDrawingData *	add,
			APP_BITMAP_IMAGE	shadingPixmaps[DOCsp_COUNT],
			int			pattern,
			int			x0,
			int			x1,
			int			y0,
			int			y1 );

extern void tedDrawHorizontalBorderLine(AppDrawingData *	add,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y );

extern int tedAppAllocateDocumentColor(	EditApplication *	ea,
					const RGB8Color *	rgb8 );

extern int tedDocAllocateDocumentColor(	EditDocument *		ed,
					const RGB8Color *	rgb8 );

extern void tedFontToolSet(	void *				voidea,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

extern int tedOpenDocumentFile(	EditApplication *	ea,
				int *			pFormat,
				BufferDocument **	pBd,
				const char *		filename,
				APP_WIDGET		relative,
				APP_WIDGET		option );

#   endif	/*  TED_APP_H	*/
