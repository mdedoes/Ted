/************************************************************************/
/*									*/
/*  Derive text metrics from font metrics.				*/
/*									*/
/************************************************************************/

#   ifndef	PS_TEXT_EXTENTS_H
#   define	PS_TEXT_EXTENTS_H

struct DocumentRectangle;
struct TextAttribute;
struct AfmFontInfo;

#   define	SCAPS_SIZE( sz )	( 8*(sz)/10 )
#   define	SUPERSUB_SIZE( sz )	( 6*(sz)/10 )

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psTextExtents(	struct DocumentRectangle *	dr,
				const struct TextAttribute *	ta,
				int				fontSizeTwips,
				const struct AfmFontInfo *	afi,
				const char *			printString,
				int				len );

extern int psMakeCapsString(	char **			pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const struct TextAttribute *	ta,
				const char *		printString,
				int			len );

extern int psMakeOutputString(	const char **		pOutputString,
				char **			pScratchString,
				int **			pSegments,
				int *			pSegmentCount,
				const struct TextAttribute *	ta,
				int			direction,
				const char *		outputString,
				int			len );

extern int psSegmentedTextExtents(
				struct DocumentRectangle *	dr,
				const char *			outputString,
				int				len,
				const int *			segments,
				int				segmentCount,
				int				fontSizeTwips,
				const struct AfmFontInfo *	afi );

extern int psCalculateStringExtents(
				struct DocumentRectangle *	dr,
				const char *			outputString,
				int				len,
				int				twipsSize,
				int				withKerning,
				int				vswap,
				const struct AfmFontInfo *	afi );

extern void psUnderlineGeometry( int *				pPos,
				int *				pThick,
				int				baseline,
				int				twipsSize,
				const struct AfmFontInfo *	afi );

extern void psStrikethroughGeometry(
				int *				pPos,
				int *				pThick,
				int				baseline,
				int				twipsSize,
				const struct AfmFontInfo *	afi );

extern int psFontBBox(		struct DocumentRectangle *	drBBox,
				struct DocumentRectangle *	drAscDesc,
				int				twipsSize,
				int				vswap,
				const struct AfmFontInfo *	afi );

extern int psGetSuperBaseline(	int *			pSuperBaseline,
				int			baseline,
				int			fontSizeTwips,
				const struct AfmFontInfo *	afi );

extern int psGetSubBaseline(	int *			pSubBaseline,
				int			baseline,
				int			fontSizeTwips,
				const struct AfmFontInfo *	afi );

extern int psGetSmallcapsSize(	int *			pSmallcapsSize,
				int			fontSizeTwips,
				const struct AfmFontInfo *	afi );

#   endif
