/************************************************************************/
/*									*/
/*  Text attributes without a reference to palettes or font lists.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXPANDED_TEXT_ATTRIBUTE_H
#   define	DOC_EXPANDED_TEXT_ATTRIBUTE_H

#   include	<textAttribute.h>
#   include	<utilColor.h>

struct PropertyMask;
struct DocumentFontList;
struct ColorPalette;

typedef struct ExpandedTextAttribute
    {
    TextAttribute	etaTextAttribute;

    RGB8Color		etaTextColor;
    int			etaTextColorExplicit;

    char *		etaFontName;
    } ExpandedTextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void docInitExpandedTextAttribute(	ExpandedTextAttribute * eta );
extern void docCleanExpandedTextAttribute(	ExpandedTextAttribute * eta );

extern int docExpandedTextAttributeSetFontName(
				ExpandedTextAttribute *		etaTo,
				int *				pChanged,
				const char *			fontName );

extern int docUpdateExpandedTextAttribute(
				struct PropertyMask *		pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom,
				const struct PropertyMask *	setMask );

extern int docCopyExpandedTextAttribute(
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom );

extern int docExpandTextAttribute(
				struct PropertyMask *		pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const TextAttribute *		taFrom,
				const struct PropertyMask *	setMask,
				const struct DocumentFontList *	dfl,
				const struct ColorPalette *	cp );

extern void docIndirectTextAttribute(
				struct PropertyMask *		pDoneMask,
				TextAttribute *			taTo,
				const ExpandedTextAttribute *	etaFrom,
				const struct PropertyMask *	setMask,
				struct DocumentFontList *	dfl,
				struct ColorPalette *		cp );

#   endif	/*  DOC_EXPANDED_TEXT_ATTRIBUTE_H  */
