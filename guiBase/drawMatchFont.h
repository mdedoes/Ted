/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_MATCH_FONT_H
#   define	APP_MATCH_FONT_H

struct IndexSet;
struct PostScriptFontList;
struct DocumentFontList;
struct TextAttribute;
struct AfmFontInfo;
struct MemoryBuffer;

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int drawFcListFonts(	struct PostScriptFontList *		psfl );

extern struct AfmFontInfo * drawFcGetFontInfoForAttribute(
				const char *			familyName,
				int				styleInt,
				const struct IndexSet *		unicodesWanted,
				const struct TextAttribute *		ta,
				const struct PostScriptFontList *	psfl );

extern struct AfmFontInfo * drawGetFontInfo(
				const struct MemoryBuffer *	familyName,
				int				styleInt,
				const struct IndexSet *		unicodesWanted,
				const struct TextAttribute *		ta,
				const struct PostScriptFontList *	psfl );

extern const struct AfmFontInfo * drawGetFontInfoForAttribute(
				const struct IndexSet **	pUnicodesWanted,
				const struct TextAttribute *		ta,
				const struct DocumentFontList *		dfl,
				const struct PostScriptFontList *	psfl );

extern int drawGetDeferredFontMetrics(	struct AfmFontInfo *		afi );
extern int drawGetDeferredFontMetricsForList(
					struct PostScriptFontList *	psfl );

# ifdef __cplusplus
    }
# endif

#   endif
