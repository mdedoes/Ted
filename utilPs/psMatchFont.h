/************************************************************************/
/*									*/
/*  Map between PostScript type and Document type font administrations.	*/
/*									*/
/************************************************************************/

#   ifndef	PS_MATCH_FONT_H
#   define	PS_MATCH_FONT_H

struct IndexSet;
struct TextAttribute;
struct PostScriptFontList;
struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern struct AfmFontInfo * psGetPsFontInfoForAttribute(
			const char *				familyName,
			int					styleInt,
			const struct IndexSet *			unicodesUsed,
			const struct TextAttribute *		ta,
			const struct PostScriptFontList *	psfl );

#   endif
