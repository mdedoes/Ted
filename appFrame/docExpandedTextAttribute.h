/************************************************************************/
/*									*/
/*  Text attributes without a reference to palettes or font lists.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXPANDED_TEXT_ATTRIBUTE_H
#   define	DOC_EXPANDED_TEXT_ATTRIBUTE_H

#   include	<utilTextAttribute.h>
#   include	<docFont.h>
#   include	<psFont.h>
#   include	<bmcolor.h>

typedef struct ExpandedTextAttribute
    {
    char *		etaFontFamilyName;
    int			etaFontEncoding;

    int			etaFontSizeHalfPoints;

    RGB8Color		etaTextColor;
    int			etaTextColorExplicit;

    unsigned int	etaFontIsBold;
    unsigned int	etaFontIsSlanted;
    unsigned int	etaTextIsUnderlined;

    unsigned int	etaSmallCaps;
    unsigned int	etaHasStrikethrough;
    unsigned int	etaCapitals;

    unsigned int	etaSuperSub;
    } ExpandedTextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void docInitExpandedTextAttribute(	ExpandedTextAttribute * eta );
extern void docCleanExpandedTextAttribute(	ExpandedTextAttribute * eta );

extern void docUpdateExpandedTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask );

extern int docCopyExpandedTextAttribute(
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom );

extern void docExpandTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const TextAttribute *		taFrom,
				const PropertyMask *		setMask,
				const DocumentFontList *	dfl,
				const RGB8Color *		colors,
				int				colorCount );

extern void docIndirectTextAttribute(
				PropertyMask *			pDoneMask,
				TextAttribute *			taTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask,
				DocumentFontList *		dfl,
				RGB8Color **			pColors,
				int *				pColorCount );

extern void docExpandedAttributeToString(
				char *				target,
				const PropertyMask *		updMask,
				const ExpandedTextAttribute *	etaNew );

extern int docExpandedTextAttributeSetFamilyName(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const char *			familyName );

extern int docExpandedTextAttributeSetFamilyAndEncoding(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const AppFontFamily *		aff,
			    int					encoding );

extern void docExpandedTextAttributeSetFace(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const AppFontTypeface *		aft );

extern int docExpandedAttributeFromString(
			    const char *			attributeString,
			    PropertyMask *			pSetMask,
			    ExpandedTextAttribute *		pEtaSet );

#   endif	/*  DOC_EXPANDED_TEXT_ATTRIBUTE_H  */
