#   ifndef	DOC_LIST_FONTS_H
#   define	DOC_LIST_FONTS_H

/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*									*/
/************************************************************************/

#   include	"docFind.h"
#   include	"docTreeNode.h"

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of fonts that is used in a document.	*/
/*									*/
/************************************************************************/

typedef int (*DocListObjectFonts)(
				const InsertedObject *		io,
				const char *			prefix,
				void *				through );

typedef int (*DocListSpanFont)(
				const BufferDocument *		bd,
				BufferItem *			paraBi,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				from,
				int				upto,
				void *				through );

typedef int (*DocListObject)(
				const BufferDocument *		bd,
				BufferItem *			paraBi,
				const TextAttribute *		ta,
				void *				through );

typedef struct ScanDocumentFonts
    {
    DocListObjectFonts	sdfListObjectFonts;
    DocListSpanFont	sdfDocListSpanFont;
    DocListObject	sdfListObject;
    void *		sdfThrough;
    } ScanDocumentFonts;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitScanDocumentFonts(	ScanDocumentFonts *		sdf );

extern int docListDocumentFonts(	BufferDocument *		bd,
					ScanDocumentFonts *		sdf );

extern int docListTreeFonts(		BufferDocument *		bd,
					const DocumentTree *		dt,
					ScanDocumentFonts *		sdf );

#   endif	/*  DOC_LIST_FONTS_H	*/
