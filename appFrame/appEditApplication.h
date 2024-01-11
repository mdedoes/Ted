/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_EDIT_APPLICATION_H
#   define	APP_EDIT_APPLICATION_H

#   include	<guiBase.h>
#   include	"appFileMessageResources.h"

#   include	<geoRectangle.h>
#   include	<geoDocumentPageSetup.h>
#   include	<drawDrawingSurface.h>
#   include	<psPostScriptFontList.h>

# ifdef __cplusplus
extern "C"
    {
# endif

struct PrintGeometry;
struct PrintJob;
struct AppFileExtension;
struct NamedImage;
struct EditDocument;
struct AppMenu;
struct PropertyMask;
struct AppConfigurableResource;
struct SimpleOutputStream;
struct SimpleLocale;
struct EditApplication;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Special calls to the aplication.					*/
/*									*/
/************************************************************************/

typedef struct SpecialCall
    {
    const char *	scCall;
    int			(*scExecuteCall)(
					struct EditApplication *	ea,
					const char *			prog,
					const char *			call,
					int				argc,
					char **				argv );
				/****************************************/
				/*  Returns the number of parameters	*/
				/*  consumed or -1 on failure.		*/
				/****************************************/
    } SpecialCall;

/************************************************************************/
/*									*/
/*  Describes the application.						*/
/*									*/
/*  1)  Descriptive members.						*/
/*  2)  Allocated at run time.						*/
/*									*/
/************************************************************************/

typedef struct EditApplication
    {
    /*  1  */
    const char *	eaApplicationName;
    const char *	eaOptionalComponents;
    const char *	eaNameAndVersion;
    const char *	eaReference;
    const char *	eaPlatformCompiled;
    const char *	eaHostDateCompiled;

			/**
			 *  The Application Icon
			 */
    const char *	eaMainIcon;
			/**
			 *  The Image on the Splash Screen
			 */
    const char *	eaMainPicture;
    struct AppFileExtension *	eaFileExtensions;
    int			eaFileExtensionCount;
    const char *	eaDefaultFileFilter;
    void *			eaResourceData;
    struct AppConfigurableResource *	eaResourceTable;
    int				eaResourceCount;
    struct AppConfigurableResource *	eaFileMessageResourceTable;
    int				eaFileMessageResourceCount;
    const SpecialCall *		eaSpecialCalls;
    int				eaSpecialCallCount;
    int				eaCreateNewFromCommand;

    double		eaMagnification;

    int			eaLeftRulerWidthMM;
    int			eaTopRulerHeightMM;
    int			eaRightRulerWidthMM;
    int			eaBottomRulerHeightMM;

    int			eaLeftRulerWidthMultiple;
    int			eaTopRulerHeightMultiple;
    int			eaRightRulerWidthMultiple;
    int			eaBottomRulerHeightMultiple;

    const char *	eaUnitString;
    const char *	eaPaperString;
    const char *	eaLeftMarginString;
    const char *	eaTopMarginString;
    const char *	eaRightMarginString;
    const char *	eaBottomMarginString;
    int			eaUnitInt;
    DocumentGeometry	eaDefaultDocumentGeometry;

    struct AppMenu *	eaAppFileMenu;
    struct AppMenu *	eaAppWinMenu;
    struct AppMenu *	eaDocFileMenu;
    struct AppMenu *	eaDocEditMenu;
    struct AppMenu *	eaDocWindowMenu;
    struct AppMenu *	eaDocHelpMenu;
    struct AppMenu *	eaAppHelpMenu;

    void *		(*eaMakePrivateData)(
				const struct EditApplication *	ea );

#   if ! USE_HEADLESS

    int			(*eaMakeDocumentWidget)(
				struct EditDocument *		ed );
    int			(*eaOpenDocument)(
				struct EditApplication *	ea,
				void *				privateData,
				int *				pFormat,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				int				suggestStdin,
				int				formatHint,
				const struct MemoryBuffer *	filename );
#   endif

    int			(*eaNewDocument)(
				struct EditDocument *		ed,
				const struct MemoryBuffer *	filename );
    int			(*eaLayoutDocument)(
				const struct EditApplication *	ea,
				DocumentRectangle *		drScreen,
				DocumentRectangle *		drVisible,
				void *				privateData,
				int				format,
				DrawingSurface			ds,
				const struct PostScriptFontList *	psfl,
				const DocumentGeometry *	defDg );
    int			(*eaFinishDocumentSetup)(
				struct EditDocument *		ed );
    void		(*eaDocumentFirstVisible)(
				struct EditDocument *		ed );
    int			(*eaCanSaveDocument)(
				const void *			privateData,
				int				format );
    int			(*eaSaveDocument)(
				struct EditApplication *	ea,
				DrawingSurface			ds,
				const void *			privateData,
				int				format,
				const struct MemoryBuffer *	documentTitle,
				int				suggestStdout,
				const struct MemoryBuffer *	filename,
				int				isDocName );
    void		(*eaFreeDocument)(
				void *				privateData,
				int				format );
    void		(*eaSuggestPageSetup)(
				struct PrintGeometry *		pg,
				void *				privateData,
				int				sheetSize );
    int			(*eaPreparePrint)(
				const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );
    int			(*eaPrintDocument)(
				struct SimpleOutputStream *	sos,
				const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );
#   if ! USE_HEADLESS
    void		(*eaDrawRectangle)(
				struct EditDocument *		ed,
				DocumentRectangle *		drClip );

    void		(*eaVisibleDocumentCountChanged)(
				    struct EditApplication *	ea,
				    int				from,
				    int				to );

    void		(*eaMakePrivateApplicationMenus)(
				    struct EditApplication *	ea,
				    APP_WIDGET			menubar );

    void		(*eaMakePrivateDocumentMenus)(
				    struct EditApplication *	ea,
				    struct EditDocument *	ed,
				    APP_WIDGET			menubar );
#   endif

    void		(*eaDocCopy)( struct EditDocument *);
    void		(*eaDocCut)( struct EditDocument *);
    void		(*eaDocSelAll)( struct EditDocument * );

    void		(*eaSetPageLayout)(
				    struct EditDocument *	ed,
				    const struct PropertyMask *	setMask,
				    const DocumentGeometry *	dgNew,
				    int				wholeDocument );

			/************************************************/
			/*  User input on the document widget:		*/
			/************************************************/
#   if ! USE_HEADLESS

    APP_EVENT_HANDLER_T	eaDocumentMouseHandler;
    APP_EVENT_HANDLER_T	eaDocumentScrollHandler;
    void		(*eaDocGotString)(	void *		voided,
						const char *	value,
						int		length );
    void		(*eaDocGotKey)(		void *		voided,
						int		key,
						unsigned int	state );
    APP_EVENT_HANDLER_T	eaObserveFocus;

    APP_SCROLLBAR_CALLBACK_T	eaDocHorizontalScrollbarCallback;
    APP_SCROLLBAR_CALLBACK_T	eaDocVerticalScrollbarCallback;

    void		(*eaScrollTopRuler) (	void *, APP_WIDGET, int );
				    /**
				     *  Adapt to a change in size of the 
				     *  document: Remember the visible range.
				     */
    void		(*eaSetTopRulerRange)(	void *		voidtr,
						APP_WIDGET	w,
						int		docX1,
						int		docVisX0,
						int		docVisX1 );

    void		(*eaScrollLeftRuler) (	void *, APP_WIDGET, int );
    void		(*eaSetLeftRulerRange)(	void *		voidtr,
						APP_WIDGET	w,
						int		docY1,
						int		docVisY0,
						int		docVisY1 );

    void		(*eaScrollRightRuler) (	void *, APP_WIDGET, int );
    void		(*eaSetRightRulerRange)(	void *	voidtr,
						APP_WIDGET	w,
						int		docY1,
						int		docVisY0,
						int		docVisY1 );

    void		(*eaScrollBottomRuler) ( void *, APP_WIDGET, int );
				    /**
				     *  Adapt to a change in size of the 
				     *  document: Remember the visible range.
				     */
    void		(*eaSetBottomRulerRange)( void *	voidtr,
						APP_WIDGET	w,
						int		docX1,
						int		docVisX0,
						int		docVisX1 );

    int			(*eaSetTopRuler)(	struct EditDocument *	ed );
    int			(*eaSetLeftRuler)(	struct EditDocument *	ed );
    int			(*eaSetRightRuler)(	struct EditDocument *	ed );
    int			(*eaSetBottomRuler)(	struct EditDocument *	ed );

    void		(*eaFreeTopRuler)(
					void *			topRuler );
    void		(*eaFreeLeftRuler)(
					void *			leftRuler );
    void		(*eaFreeRightRuler)(
					void *			rightRuler );
    void		(*eaFreeBottomRuler)(
					void *			bottomRuler );
#   endif

    struct AppSelectionType *	eaDocSelectionTypes;
    int				eaDocSelectionTypeCount;

    struct AppSelectionType *	eaAppSelectionTypes;
    int				eaAppSelectionTypeCount;

    struct PrintDestination *	eaPrintDestinations;

    int				eaPrintDestinationCount;
    int				eaDefaultPrintDestination;
    int				eaPrintDestinationsCollected;

    struct NamedImage *		eaNamedImages;
    int				eaNamedImageCount;

    unsigned int	eaNextDocumentId;

				/**
				 *  Culture tag (Language and teritory) 
				 *  determined at startup.
				 */
    const char *		eaLocaleTag;

				/**
				 *  Minimal locale information for formatting
				 *  values.
				 */
    struct SimpleLocale *	eaLocale;

    int				eaArgc;
    char **			eaArgv;

    /*  2  */
#   if ! USE_HEADLESS
    APP_CONTEXT			eaContext;

    int				eaScreenPixelsWide;
    int				eaScreenPixelsHigh;
    double			eaPixelsPerTwip;

    AppToplevel			eaToplevel;
	APP_INPUT_METHOD	eaInputMethod;
	APP_CURSOR		eaDocumentCursor;
    APP_WIDGET			eaMainWindow;
    APP_WIDGET			eaMenuBar;
    APP_WIDGET			eaFileMenu;
    APP_WIDGET			eaFileMenuButton;
    APP_WIDGET			eaWinMenu;
    APP_WIDGET			eaWinMenuButton;
    APP_WIDGET			eaHelpMenu;
    APP_WIDGET			eaHelpMenuButton;

    void *			eaPageTool;
    void *			eaPrintDialog;

    struct EditDocument *	eaCurrentDocument;
    struct EditDocument **	eaOpenDocuments;
    int				eaOpenCount;
    int				eaVisibleDocumentCount;
    int				eaMainVisibleAsAbout;

#   endif

    const char *		eaAfmDirectory;
    const char *		eaFontDirectory;
    const char *		eaGhostscriptFontmap;
    const char *		eaGhostscriptFontToXmapping;
    int				eaGhostscriptMappingsRead;
    const char *		eaCustomPrintCommand;
    const char *		eaCustomPrinterName;
    const char *		eaCustomPrintCommand2;
    const char *		eaCustomPrinterName2;
    const char *		eaAuthor;
    const char *		eaFocusColor;

    int				eaUsePostScriptFiltersInt;
    int				eaUsePostScriptIndexedImagesInt;
    int				eaSkipEmptyPagesInt;
    int				eaSkipBlankPagesInt;
    int				eaOmitHeadersOnEmptyPagesInt;
    int				eaAvoidFontconfigInt;
    int				eaPreferBase35FontsInt;
    int				eaEmbedFontsInt;
    int				eaUseKerningInt;
    int				eaStyleToolInt;

    AppFileMessageResources	eaFileMessageResources;

    const char *		eaMagnificationString;
    const char *		eaUsePostScriptFiltersString;
    const char *		eaUsePostScriptIndexedImagesString;
    const char *		eaSkipEmptyPagesString;
    const char *		eaSkipBlankPagesString;
    const char *		eaOmitHeadersOnEmptyPagesString;
    const char *		eaAvoidFontconfigString;
    const char *		eaPreferBase35FontsString;
    const char *		eaEmbedFontsString;
    const char *		eaUseKerningString;
    const char *		eaStyleToolString;

    const char *		eaCustomPsSetupFilename;

    const char *		eaLeftRulerWidthMMString;
    const char *		eaTopRulerHeightMMString;
    const char *		eaRightRulerWidthMMString;
    const char *		eaBottomRulerHeightMMString;

    int				eaGotPaste;

				/**
				 *  List of fonts. These are the fonts 
				 *  that are available for the application.
				 *  It is the responsibility of the application 
				 *  to map document fonts to the fonts here.
				 *  
				 *  This is the set of fonts that is used to
				 *  print documents. [Or produce PDF.]
				 *
				 *  The set of fonts that is used on-screen 
				 *  may or may not be the same one. If the 
				 *  collection of screen fonts is different,
				 *  the application must match screen fonts 
				 *  to the fonts here. The library and 
				 *  fontconfig have abundant facilities to 
				 *  match.
				 *
				 *  The document fonts 
				 */
    PostScriptFontList		eaPostScriptFontList;

    void *			eaSystemProperties;
    void *			eaUserProperties;

    int				eaGotResourceTable;
    int				eaGotFileMessageResourceTable;
    int				eaGotApplicationResources;

				/**
				 *  Set of tools to manage properties
				 *  The entry point is the application, 
				 *  but it will often pass the calls on 
				 *  to the current document.
				 */
    struct AppInspector *	eaInspector;
    } EditApplication;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appNewDocument(	EditApplication *		ea,
				const struct MemoryBuffer *	filename );

extern int appMain(		EditApplication *		ea,
				int				argc,
				char *				argv[] );

extern void appSetDocument(	EditApplication *		ea,
				struct EditDocument *		ed );

extern void appRemoveDocument(	EditApplication *		ea,
				struct EditDocument *		ed );

extern void appSetCurrentDocument(
				EditApplication *		ea,
				struct EditDocument *		ed );

extern void appDocVisible(	EditApplication *		ea,
				struct EditDocument *		ed,
				int				visible );

extern void appMakeDocVisible(	EditApplication *	ea,
				struct EditDocument *		ed );

extern int appSetupDocument(		EditApplication *	ea,
					struct EditDocument *	ed );

extern void appRenameDocumentOptions(	EditApplication *	ea,
					struct EditDocument *	ed,
					const struct MemoryBuffer *	title );

extern int appAppAskForPaste(	EditApplication *	ea,
				const char *		selection );

extern void * appOpenDocumentInput(	EditApplication *	ea,
					int *			pFromFormat,
					int *			pSuggestStdin,
					int			refuseStdin,
					const struct MemoryBuffer *	fromName );

extern int appSaveDocumentOutput(	EditApplication*	ea,
					void *			privateData,
					int			fromFormat,
					const struct MemoryBuffer *	toName );

extern int appFileConvert(	EditApplication *	ea,
				const struct MemoryBuffer *	fromName,
				const struct MemoryBuffer *	toName );

extern int appGuiInitApplication(	EditApplication *	ea,
					int *			pArgc,
					char ***		pArgv );

extern int appFormatDocumentTitle(	struct MemoryBuffer *		windowTitle,
					struct MemoryBuffer *		iconTitle,
					EditApplication *	ea,
					const struct MemoryBuffer *	title );

extern void appAllocateCopyPasteTargetAtoms(	EditApplication *	ea );

extern void appGetApplicationResourceValues(	EditApplication *	ea );


extern void appApplicationSettingsToPrintGeometry(
					struct PrintGeometry *	pg,
					EditApplication *	ea );

extern int appDocumentTestCanSave(
				EditApplication *		ea,
				const struct AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern int appDocumentGetSaveFormat(
				int *				pSuggestStdout,
				EditApplication *		ea,
				const struct MemoryBuffer *		filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern int appFileCanOpen(	const EditApplication *		ea,
				int				format );

extern int appPostScriptFontCatalog(	EditApplication *	ea );

extern int appGetPrintDestinations(	EditApplication *	ea );

extern int appMakeDocumentWindow(	struct EditDocument **	pEd,
					EditApplication *	ea,
					int			readOnly,
					const struct MemoryBuffer *	title,
					const struct MemoryBuffer *	filename );

extern void appGetPixelsPerTwip(	EditApplication *	ea );

extern int appDetermineBoolean(	int *			pIval,
				const char *		sVal );

extern int appMainOpenDocument(		EditApplication *	ea,
					int			refuseStdin,
					const struct MemoryBuffer *	filename );

extern void appExitApplication(		EditApplication *	ea );

extern int appMainHandleSpecialCalls(
				EditApplication *	ea,
				const char *		prefix,
				int			getResources,
				const char *		prog,
				int			argc,
				char *			argv[] );

# ifdef __cplusplus
    }
# endif

#   endif
