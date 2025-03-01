#   ifndef	APP_PRINT_DIALOG_H
#   define	APP_PRINT_DIALOG_H

#   include	<guiWidgets.h>

struct EditDocument;
struct DocumentGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appRunPrintDialog(	struct EditDocument *		ed,
				const struct DocumentGeometry * dgDocument,
				int				pageCount,
				int				firstSelected,
				int				lastSelected,
				APP_WIDGET			printOption,
				const char *			pixmapName );

#   endif	/*  APP_PRINT_DIALOG_H  */
