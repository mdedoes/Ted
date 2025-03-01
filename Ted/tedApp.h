#   ifndef	TED_APP_H
#   define	TED_APP_H

#   include	<drawDrawingSurface.h>
#   include	<guiWidgets.h>

struct DocumentProperties;
struct EditApplication;
struct EditDocument;
struct SimpleOutputStream;
struct PrintJob;
struct PrintGeometry;
struct PostScriptFontList;
struct DocumentGeometry;
struct MemoryBuffer;
struct BufferDocument;
struct DocumentRectangle;
struct TedDocument;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedFinishDocumentSetup(	struct EditDocument *		ed );
extern void tedDocumentFirstVisible(	struct EditDocument *		ed );

extern int tedMakeDocumentWidget(	struct EditDocument *		ed );

extern int tedSaveDocument(	struct EditApplication *	ea,
				DrawingSurface			ds,
				const void *			privateData,
				int				format,
				const struct MemoryBuffer *	documentTitle,
				int				suggestStdout,
				const struct MemoryBuffer *	filename,
				int				isDocName );

int tedSaveDocumentToStream(	struct EditApplication *	ea,
				DrawingSurface			ds,
				struct SimpleOutputStream *	sos,
				struct TedDocument *		td,
				int				format,
				const struct MemoryBuffer *	documentTitle,
				const struct MemoryBuffer *	filename );

extern int tedNewDocument(	struct EditDocument *		ed,
				const struct MemoryBuffer *	filename );

extern int tedPreparePrint(	const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );

extern int tedPrintDocument(	struct SimpleOutputStream *	sos,
				const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );

extern void tedSuggestPageSetup( struct PrintGeometry *		pg,
				void *				privateData,
				int				sheetSize );

extern int tedOpenDocumentFile(	unsigned char *		digest,
				int *			pFormat,
				struct BufferDocument **	pBd,
				struct EditApplication *	ea,
				int			suggestStdin,
				int			formatHint,
				const struct MemoryBuffer *	filename,
				int			complain,
				APP_WIDGET		relative,
				APP_WIDGET		option );

extern int tedOpenDocument(	struct EditApplication *		ea,
				void *				voidtd,
				int *				pFormat,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				int				suggestStdin,
				int				formatHint,
				const struct MemoryBuffer *	filename );

extern void tedMakeDocumentReadonly(	struct EditDocument *	ed,
					int		readonly );

extern int tedLayoutDocument(	const struct EditApplication *	ea,
				struct DocumentRectangle *	drScreen,
				struct DocumentRectangle *	drVisible,
				void *				privateData,
				int				format,
				DrawingSurface			ds,
				const struct PostScriptFontList *	psfl,
				const struct DocumentGeometry *	defDg );

extern int tedSetGenerator(	struct DocumentProperties *	dp,
				const struct EditApplication *		ea );

extern void tedManual(			APP_WIDGET		option,
					struct EditApplication *	ea,
					APP_WIDGET		relative );

extern void tedVisibleDocumentCountChanged(
				struct EditApplication *	ea,
				int				from,
				int				to );

#   endif	/*  TED_APP_H	*/
