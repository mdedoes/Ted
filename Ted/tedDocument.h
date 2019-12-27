#   ifndef	TED_DOCUMENT_H
#   define	TED_DOCUMENT_H

#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>
#   include	<utilIndexMapping.h>
#   include	<docEditTrace.h>
#   include	<utilColor.h>
#   include	<guiWidgets.h>
#   include	<docResizeHandles.h>

struct TedDocument;
struct EditDocument;
struct SimpleLocale;
struct BufferDocument;
struct RasterImage;
struct RecalculateFields;
struct EditApplication;

/************************************************************************/
/*									*/
/*  Private data for a document.					*/
/*									*/
/************************************************************************/

typedef struct TedDocument
    {
    struct BufferDocument *	tdDocument;

    unsigned char		tdTraced;

				    /**
				     *  If this is true, an attempt is made 
				     *  to activate overstrike. by selecting 
				     *  and replacing the next character.
				     */
    unsigned char		tdOverstrike;

				    /**
				     *  The name of the document that is 
				     *  recovered. This is to smuggle the 
				     *  original document name through the 
				     *  generic interface.
				     */
    MemoryBuffer		tdRecoveredName;

    DocumentSelection		tdSelection;
    SelectionGeometry		tdSelectionGeometry;
    SelectionDescription	tdSelectionDescription;
    int				tdBodySectionNumber;
				    /**
				     *  The screen font number corresponding 
				     *  to the text attribute in
				     *  tdSelectionDescription
				     */
    int				tdCurrentScreenFont;

    IndexMapping		tdAttributeToScreenFont;
				    /**
				     *  The magnification factor of the
				     *  document. 1.0 means no magnification.
				     */
    double			tdMagnification;
				    /**
				     *  The number of pixels per twip. The
				     *  magnification of the document is taken 
				     *  into account.
				     */
    double			tdPixelsPerTwip;
				    /**
				     *  The height of the gray strip between 
				     *  pages.
				     */
    int				tdPageGapTwips;
    int				tdPageGapPixels;

    EditTrace			tdEditTrace;

    const struct SimpleLocale *	tdLocale;

    APP_WIDGET			tdFileOpenOption;
    APP_WIDGET			tdFileSaveOption;
    APP_WIDGET			tdFileSaveAsOption;
    APP_WIDGET			tdFileUnlockOption;
    APP_WIDGET			tdFileRecoverOption;
    APP_WIDGET			tdFilePropertiesOption;
    APP_WIDGET			tdFileCloseOption;
    APP_WIDGET			tdFileQuitSeparator;
    APP_WIDGET			tdFileQuitOption;

    APP_WIDGET			tdUndoOption;
    APP_WIDGET			tdRepeatOption;

    APP_WIDGET			tdCopyOption;
    APP_WIDGET			tdCutOption;
    APP_WIDGET			tdPasteOption;

    APP_WIDGET			tdInsertMenu;
    APP_WIDGET			tdInsertMenuButton;
    APP_WIDGET			tdInsPictOption;
    APP_WIDGET			tdInsFileOption;
    APP_WIDGET			tdInsSymbolOption;
    APP_WIDGET			tdInsHyperlinkOption;
    APP_WIDGET			tdInsBookmarkOption;
    APP_WIDGET			tdInsInsertFootnoteOption;
    APP_WIDGET			tdInsInsertEndnoteOption;
    APP_WIDGET			tdInsInsertChftnsepOption;
    APP_WIDGET			tdInsInsertTableOption;
    APP_WIDGET			tdInsInsertPageNumberOption;
    APP_WIDGET			tdInsInsertLineBreakOption;
    APP_WIDGET			tdInsInsertPageBreakOption;
    APP_WIDGET			tdInsInsertColumnBreakOption;
    APP_WIDGET			tdInsInsertSectBreakOption;

    APP_WIDGET			tdTableMenu;
    APP_WIDGET			tdTableMenuButton;
    APP_WIDGET			tdTabInsertTableOption;
    APP_WIDGET			tdTabAddRowOption;
    APP_WIDGET			tdTabAddColumnOption;
    APP_WIDGET			tdTabInsertSeparator;

    APP_WIDGET			tdTabSelectTableOption;
    APP_WIDGET			tdTabSelectRowOption;
    APP_WIDGET			tdTabSelectColumnOption;
    APP_WIDGET			tdTabSelectSeparator;

    APP_WIDGET			tdTabDeleteTableOption;
    APP_WIDGET			tdTabDeleteRowOption;
    APP_WIDGET			tdTabDeleteColumnOption;
    APP_WIDGET			tdTabDeleteSeparator;

    APP_WIDGET			tdFormatMenu;
    APP_WIDGET			tdFormatMenuButton;
    APP_WIDGET			tdFormatOneParaOption;
    APP_WIDGET			tdFormatOneParaSeparator;

    APP_WIDGET			tdFormatPasteRulerOption;
    APP_WIDGET			tdFormatIncreaseIndentOption;
    APP_WIDGET			tdFormatDecreaseIndentOption;
    APP_WIDGET			tdFormatRulerSeparator;

    APP_WIDGET			tdFormatAlignLeftOption;
    APP_WIDGET			tdFormatAlignRightOption;
    APP_WIDGET			tdFormatAlignCenterOption;
    APP_WIDGET			tdFormatAlignJustifyOption;
    APP_WIDGET			tdFormatAlignSeparator;

    APP_WIDGET			tdDrawTableGridOption;

    APP_WIDGET			tdFontMenu;
    APP_WIDGET			tdFontMenuButton;
    APP_WIDGET			tdFontBoldOption;
    APP_WIDGET			tdFontItalicOption;
    APP_WIDGET			tdFontUnderlinedOption;
    APP_WIDGET			tdFontSuperscriptOption;
    APP_WIDGET			tdFontSubscriptOption;
    APP_WIDGET			tdFontToggleSeparator;
    APP_WIDGET			tdFontPasteOption;

    APP_WIDGET			tdToolsMenu;
    APP_WIDGET			tdToolsMenuButton;
    APP_WIDGET			tdToolsFormatToolOption;

    APP_WIDGET			tdToolsFontOption;
    APP_WIDGET			tdToolsFindOption;
    APP_WIDGET			tdToolsSpellingOption;
    APP_WIDGET			tdToolsPageLayoutOption;
    APP_WIDGET			tdToolsSymbolOption;

    RGB8Color			tdTableColor;
    RGB8Color			tdSelColor;
    RGB8Color			tdCopiedSelColor;

    int				tdVisibleSelectionCopied;
    MemoryBuffer		tdCopiedSelection;
    MemoryBuffer		tdCopiedFont;
    MemoryBuffer		tdCopiedRuler;
    struct RasterImage *	tdCopiedImage;
    int				tdOwnsPrimarySelection;

    void *			tdFindProg;

#   if USE_MOTIF
    XtIntervalId		tdHideIBarId;
    XtIntervalId		tdShowIBarId;
#   endif

#   if USE_GTK
    guint			tdHideIBarId;
    guint			tdShowIBarId;
#   endif

#   ifdef USE_QT
    int				tdHideIBarId;
    int				tdShowIBarId;
#   endif

    APP_WINDOW			tdObjectWindow;
    APP_WINDOW			tdObjectResizeWindows[RESIZE_COUNT];
    int				tdObjectCornerMovedX;
    int				tdObjectCornerMovedY;
    int				tdScaleChangedX;
    int				tdScaleChangedY;
    int				tdObjectResizeCorner;

    int				tdDrawTableGrid;
    int				tdDrawMonochrome;

    int				tdInProgrammaticChange;
    } TedDocument;

/************************************************************************/

extern const char TedTraceExtension[];

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedFirstRecalculateFields(	struct RecalculateFields *	rf,
					struct BufferDocument *		bd );

extern int tedObjectDrag(	APP_WIDGET			w,
				struct EditDocument *		ed,
				APP_EVENT *			downEvent );

extern void tedFreeDocument(		void *			voidtd,
					int			format );

extern void tedManagePrimarySelection(	struct EditDocument *		ed );

extern void tedSetTracedChangedFlag(	struct EditDocument *		ed );

extern void tedRedoDocumentLayout(	struct EditDocument *		ed );

extern void tedUndrawIBar(		const struct EditDocument *	ed );

extern void tedMoveObjectWindows(	struct EditDocument *		ed );

extern void tedSetupDocumentColors(	struct EditDocument *	ed,
					struct TedDocument *	td );

extern void * tedMakePrivateData( const struct EditApplication *	ea  );

#   endif	/*  TED_DOCUMENT_H	*/
