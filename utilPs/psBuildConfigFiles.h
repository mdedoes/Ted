/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_CONFIG_H
#   define	PS_CONFIG_H

struct SimpleOutputStream;
struct MemoryBuffer;
struct PostScriptFontList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psFontmapForFiles(	struct SimpleOutputStream *	sosOut,
				int			fileCount,
				const char * const *	fileNames );

extern int psAfmToGSFontmap(	struct SimpleOutputStream *	sosFontDir,
				const char *		afmFileName );

extern int psGSLibAfmDirectory(	struct PostScriptFontList *	psfl,
				int			ignoreKerning,
				const struct MemoryBuffer *	afmDirectory,
				const struct MemoryBuffer *	psDirectory );

extern int psAfmForFontFiles(	struct PostScriptFontList *	psfl,
				int			ignoreKerning,
				int			fileCount,
				char **			fileNames,
				const struct MemoryBuffer *	afmDirectory,
				const struct MemoryBuffer *	psDirectory );

extern int psFontFileToAfm(	struct SimpleOutputStream *	sosAfm,
				int			omitKernPairs,
				const char *		fontFileName,
				const struct MemoryBuffer *	psDirectory );

#   endif
