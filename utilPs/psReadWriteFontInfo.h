/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_READ_WRITE_FONT_INFO_H
#   define	PS_READ_WRITE_FONT_INFO_H

struct SimpleInputStream;
struct SimpleOutputStream;
struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psWriteAfmFile(		struct SimpleOutputStream *	sos,
					int			omitKernPairs,
					const struct AfmFontInfo *	afi );

extern void psWriteFontInfoDict(	struct SimpleOutputStream *	sos,
					const struct AfmFontInfo *	afi );

extern int psAfmReadAfm(		struct SimpleInputStream *	sisAfm,
					struct AfmFontInfo *		afi,
					int			deferMetrics );

#   endif
