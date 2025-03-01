/************************************************************************/
/*									*/
/*  Producte rtf files with the code charts for all known fonts.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONTS_DOCUMENTS_H
#   define	DOC_FONTS_DOCUMENTS_H

struct PostScriptFontList;
struct MemoryBuffer;
struct SimpleLocale;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFontsDocuments(
			const struct PostScriptFontList *	psfl,
			const struct SimpleLocale *		sl,
			const struct MemoryBuffer *		templDir,
			const struct MemoryBuffer *		outputDir );

#    endif	/*  DOC_FONTS_DOCUMENTS_H	*/
