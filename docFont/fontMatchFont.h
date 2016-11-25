/************************************************************************/
/*									*/
/*  Map between PostScript type and Document type font administrations.	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MATCH_FONT_H
#   define	UTIL_MATCH_FONT_H

struct DocumentFontList;
struct PostScriptFontList;

struct AfmFontInfo;
struct IndexSet;
struct TextAttribute;
struct MemoryBuffer;

typedef const struct AfmFontInfo * (*GetFontForAttribute)(
		const struct IndexSet **		pUnicodesWanted,
		const struct TextAttribute *		ta,
		const struct DocumentFontList *		dfl,
		const struct PostScriptFontList *	psfl );

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int fontAddPsFontsToDocList(
				struct DocumentFontList *		dfl,
				const struct PostScriptFontList *	psfl );

extern int fontAddBase35FontsToDocList(	struct DocumentFontList *	dfl );

extern const char * fontGetEncodingName(
				const struct MemoryBuffer *	fontName,
				int				charset );

#   endif
