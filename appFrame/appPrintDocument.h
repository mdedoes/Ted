/************************************************************************/
/*									*/
/*  Printing of Documents						*/
/*									*/
/************************************************************************/

#   ifndef	APP_PRINT_DOCUMENT_H
#   define	APP_PRINT_DOCUMENT_H

#   include	<drawDrawingSurface.h>

# ifdef __cplusplus
extern "C"
    {
# endif

struct EditApplication;
struct EditDocument;
struct SimpleOutputStream;
struct MemoryBuffer;
struct PrintGeometry;
struct PrintJob;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appPrintToFile(	struct EditApplication *	ea,
				const char *			fromName,
				const char *			toName,
				const char *			paperString );

extern int appPrintToPrinter(	struct EditApplication *	ea,
				const char *			fromName,
				const char *			toName,
				const char *			paperString );

extern int appSaveToPs(		struct EditApplication *	ea,
				DrawingSurface			ds,
				struct SimpleOutputStream *	sos,
				void *				privateData,
				const struct MemoryBuffer *	documentTitle,
				int				format );

extern void appPrintJobForEditDocument(	struct PrintJob *	pj,
					struct EditDocument *		ed,
					const struct PrintGeometry *	pg );

# ifdef __cplusplus
extern "C"
    }
# endif

#   endif
