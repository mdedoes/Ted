/************************************************************************/
/*									*/
/*  Convert True Type Fonts.						*/
/*									*/
/************************************************************************/

#   ifndef	PS_TTF_H
#   define	PS_TTF_H

struct SimpleInputStream;
struct SimpleOutputStream;
struct MemoryBuffer;
struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psTtfToPt42(		struct SimpleOutputStream *	sosPf42,
				struct SimpleInputStream *	sisTtf );

extern int psTtcToPt42(		struct SimpleOutputStream *	sosPf42,
				int				fontFileIndex,
				struct SimpleInputStream *	sisTtf );

extern int psTtfToAfi(		struct AfmFontInfo *		afi,
				const struct MemoryBuffer *	fontFileName,
				struct SimpleInputStream *	sisTtf );

#   endif
