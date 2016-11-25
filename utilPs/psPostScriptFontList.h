/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_LIST_H
#   define	PS_FONT_LIST_H

struct PsFontFamily;
struct AfmFontInfo;
struct MemoryBuffer;

typedef struct PostScriptFontList
    {
    struct PsFontFamily **	psflFamilies;
    int			psflFamilyCount;
    struct AfmFontInfo **	psflInfos;
    int			psflInfoCount;
    int			psflAvoidFontconfig;
				/*  Do not use fontconfig. This is a	*/
				/*  strange place to store this piece	*/
				/*  of information. However: everywhere	*/
				/*  where we need it, we have a font	*/
				/*  list available.			*/

    void *		psflFontNameToInfo;
    void *		psflFontFileToInfo;
    } PostScriptFontList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void psInitPostScriptFontList(	PostScriptFontList *	psfl );
extern void psCleanPostScriptFontList(	PostScriptFontList *	psfl );

extern int psPostScriptFontListAddInfo(	PostScriptFontList *	psfl,
					struct AfmFontInfo *		afi );

extern int psPostScriptFontListAddFamily( PostScriptFontList *	psfl,
					struct PsFontFamily *		aff );

extern int psPostScriptFontListInfosToFamilies(
					PostScriptFontList *	psfl );

extern int psPostScriptFontListGetFamilyByName(
					const PostScriptFontList *	psfl,
					const char *			name );

extern struct AfmFontInfo * psPostScriptFontListGetFontInfoByFaceFile(
					const PostScriptFontList *	psfl,
					const char *			file );

extern int psPostScriptFontListSetFontInfoForFaceFile(
					PostScriptFontList *		psfl,
					struct AfmFontInfo *			afi,
					const char *			file );

extern struct AfmFontInfo * psPostScriptFontListGetFontInfoByFontName(
					const PostScriptFontList *	psfl,
					const char *			name );

extern int psSaveAfms(		const PostScriptFontList *	psfl,
				int				omitKernPairs,
				const struct MemoryBuffer *		afmDirectory );

extern int psFontCatalog(		PostScriptFontList *	psfl,
					int			ignoreKerning,
					const struct MemoryBuffer *	afmDirectory );

#   endif
