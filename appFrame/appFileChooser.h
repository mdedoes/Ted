/************************************************************************/
/*									*/
/*  Header file for internal use by the FileChooser implementation.	*/
/*									*/
/************************************************************************/

#   include	<guiBase.h>

#   if ! USE_HEADLESS

#   include	<guiOptionmenu.h>
#   include	<utilMemoryBuffer.h>

struct AppFileExtension;
struct EditApplication;
struct EditDocument;

/************************************************************************/
/*									*/
/*  For opening/saving documents.					*/
/*									*/
/************************************************************************/

#   define	ACIrespNONE	0
#   define	ACIrespSAVE	1
#   define	ACIrespCANCEL	2
#   define	ACIrespOPEN	3
#   define	ACIrespFAILURE	4

typedef struct AppFileChooserResources
    {
    char *			acrNoFilenameMessage;
    char *			acrIsDirecoryMessage;
    char *			acrNotWritableMessage;
    char *			acrNotReadableMessage;
    char *			acrOverwriteMessage;
    char *			acrNoSuchDirMessage;

    char *			acrPostScriptFiles;
    } AppFileChooserResources;

typedef int (*APP_OPEN_DOCUMENT)(	struct EditApplication * ea,
					void *			through,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename );

typedef int (*APP_SAVE_DOCUMENT)(	struct EditDocument *	ed,
					void *			through,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			format,
					const MemoryBuffer *	filename );

typedef struct AppChooserInformation
    {
#   if USE_MOTIF
    AppDialog				aciDialog;
#   endif
#   if USE_GTK
#	ifdef GTK_RESPONSE_ACCEPT
	    APP_WIDGET			aciWidget;
#	endif
	    AppDialog			aciDialog;
#	    define 			aciWidget aciDialog.adTopWidget
#	else
#   endif

    struct EditApplication *		aciApplication;
    struct EditDocument *		aciDocument;
    MemoryBuffer			aciFilename;
    void *				aciThrough;
    APP_OPEN_DOCUMENT			aciOpenDocument;
    APP_SAVE_DOCUMENT			aciSaveDocument;
    const struct AppFileExtension *	aciExtensions;
    int					aciExtensionCount;
    AppOptionmenu			aciFilterOptionmenu;
    APP_WIDGET				aciOption;
    APP_WIDGET				aciRelativeTo;
    int					aciFormat;

    const AppFileChooserResources *	aciResources;
    } AppChooserInformation;

#   define FILE_CHOOSER_CAN_OPEN( afe ) ( \
      (     (afe)->afeUseFlags & APPFILE_CAN_OPEN )		&& \
      ( ! ( (afe)->afeUseFlags & APPFILE_IS_BASIC_TYPE ) )	&& \
      ( ! ( (afe)->afeUseFlags & APPFILE_HIDE_OPEN ) )		)

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void appFileChooserGetTexts(	struct EditApplication *	ea,
					AppChooserInformation *		aci );

extern int appFileChooserTestNameForOpen(
				const AppChooserInformation *	aci,
				const MemoryBuffer *		filename );

extern int appFileChooserTestNameForWrite(
				const AppChooserInformation *	aci,
				const MemoryBuffer *		filename );

extern int appChooserImplGetFilename(	const AppChooserInformation *	aci,
					MemoryBuffer *		filename );

extern int appChooserImplSetFilename(	AppChooserInformation *	aci,
					const MemoryBuffer *	filename );

extern int appChooserTestFilename(
				const AppChooserInformation *	aci,
				const MemoryBuffer *		filename );

extern int appRunSaveChooser(	APP_WIDGET		option,
				APP_WIDGET		relative,
				APP_SAVE_DOCUMENT	saveDocument,
				struct EditDocument *	ed,
				void *			through );

extern int appRunPrintToFileChooser(
				APP_WIDGET		option,
				APP_WIDGET		relative,
				APP_SAVE_DOCUMENT	saveDocument,
				struct EditDocument *	ed,
				void *			through );

extern void appRunOpenChooser(	APP_WIDGET		option,
				APP_WIDGET		relative,
				int			extensionCount,
				struct AppFileExtension *	openExtensions,
				const char *		defaultFilter,
				const MemoryBuffer *	dir,
				APP_OPEN_DOCUMENT	openDocument,
				struct EditApplication * ea,
				void *			through );

extern int appFileFilterGetDescriptions(
				struct EditApplication *	ea,
				struct AppFileExtension *	extensions,
				int			extensionCount );

extern int appFileChooserConfirmOverWrite(
				const AppChooserInformation *	aci,
				const char *			filename );

extern int appChooserOpenDocument(	struct EditApplication * ea,
					void *			through,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename );

extern int appChooserChangeFilenameExtension(
				const AppChooserInformation *	aci,
				MemoryBuffer *			filename,
				int				newFormat );

extern int appSaveChooserSave(	AppChooserInformation *		aci,
				APP_WIDGET			relative,
				struct EditDocument *		ed,
				const MemoryBuffer *		filename );

extern int appSaveChooserPrint(	AppChooserInformation *		aci,
				const MemoryBuffer *		filename );

extern int appPrintChooserFileName(
				AppChooserInformation *		aci,
				MemoryBuffer *			changed,
				struct EditDocument *		ed,
				const MemoryBuffer *		filename );

#   endif
