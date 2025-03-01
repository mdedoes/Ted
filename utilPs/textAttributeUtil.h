/************************************************************************/
/*									*/
/*  Attributes of text.							*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_UTIL_H
#   define	UTIL_TEXT_ATTRIBUTE_UTIL_H

struct TextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int textGetPixelSize(	double				pixelsPerTwip,
				const struct TextAttribute *		ta );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_UTIL_H	*/
