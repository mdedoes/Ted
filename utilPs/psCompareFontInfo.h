/************************************************************************/
/*									*/
/*  Comparisons between postscript font infos. For sorting and		*/
/*  classification.							*/
/*									*/
/************************************************************************/

#   ifndef	PS_CMP_FONT_INFO_H
#   define	PS_CMP_FONT_INFO_H

struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psFontCompareInfosIncludingFamily(	const void *	voidpafi1,
						const void *	voidpafi2 );

extern int psFontCompareInfosExcludingFamily(	const void *	voidpafi1,
						const void *	voidpafi2 );

extern void psFontFaceDistance(		int *			pDifCount,
					double *		pDistance,
					const struct AfmFontInfo *	afi1,
					int			isSlanted,
					int			weight );

#   endif
