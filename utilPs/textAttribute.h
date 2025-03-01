/************************************************************************/
/*									*/
/*  Attributes of text.							*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_H
#   define	UTIL_TEXT_ATTRIBUTE_H

struct PropertyMask;

typedef struct TextAttribute
    {
    int			taFontNumber;
    int			taFontSizeHalfPoints;
    int			taTextColorNumber;
    int			taTextStyleNumber;

    int			taBorderNumber;
    int			taShadingNumber;
    int			taBaselineShiftHalfPoints;

    unsigned char	taFontIsBold;
    unsigned char	taFontIsSlanted;
    unsigned char	taTextIsUnderlined;

    unsigned char	taSmallCaps;
    unsigned char	taHasStrikethrough;
    unsigned char	taCapitals;

    unsigned char	taSuperSub;

			/**
			 *  The Microsoft local/cultureID. (MS-lcid)
			 *  Inside older RTF documents, noproof is 
			 *  implemented as culture code 1024. We do 
			 *  not follow this convention here, but as 1024
			 *  is an unknown culture code, the text will not 
			 *  be spell checked anyway.
			 */
    int			taLocaleId;
#   define			DOClocaleNOTSET	0

			/**
			 *  Do not check spelling
			 */
    unsigned char	taNoProof;
    } TextAttribute;

typedef enum TextSuperSub
    {
    TEXTvaREGULAR= 0,
    TEXTvaSUPERSCRIPT,
    TEXTvaSUBSCRIPT,

    TEXTva_COUNT
    } TextSuperSub;

typedef enum TextUnderline
    {
    DOCunderlineNONE= 0,
    DOCunderlineSINGLE,
    DOCunderlineDOUBLE,

    TEXTunderline_COUNT
    } TextUnderline;

typedef enum TextProperty
    {
    TAprop_NONE= -1,

    TApropFONT_NUMBER= 0,
    TApropFONTSIZE,
    TApropFONTBOLD,
    TApropFONTSLANTED,
    TApropTEXTUNDERLINED,
    TApropSUPERSUB,
    TApropSMALLCAPS,
    TApropCAPITALS,
    TApropSTRIKETHROUGH,
    TApropTEXT_COLOR,
    TApropTEXT_STYLE,

    TApropBORDER,
    TApropSHADING,

    TApropBASELINE_SHIFT,

    TApropLOCALE,
    TApropNOPROOF,

    TAprop_BASELINE_SHIFT_DOWN,
#   define TAprop_COUNT TAprop_BASELINE_SHIFT_DOWN

    TAprop_FULL_COUNT
    } TextProperty;

#   define	textEqualFontStyle(ta1,ta2) ( \
		(ta1)->taFontIsBold == (ta2)->taFontIsBold && \
		(ta1)->taFontIsSlanted == (ta2)->taFontIsSlanted )

		/**
		 *  Determines what font face is used to draw the text.
		 *  (Size counts as well)
		 */
#   define	textEqualFontFace(ta1,ta2) ( \
		(ta1)->taFontNumber == (ta2)->taFontNumber && \
		(ta1)->taFontSizeHalfPoints == (ta2)->taFontSizeHalfPoints && \
		(ta1)->taSuperSub == (ta2)->taSuperSub && \
		(ta1)->taSmallCaps == (ta2)->taSmallCaps && \
		textEqualFontStyle( (ta1), (ta2) ) )

		/**
		 *  Determines the way the text is drawn.
		 *  (Excluding language or position relative to the line)
		 */
#   define	textEqualTextAttributes(ta1,ta2) ( \
		textEqualFontFace((ta1),(ta2)) && \
		(ta1)->taTextIsUnderlined == (ta2)->taTextIsUnderlined && \
		(ta1)->taHasStrikethrough == (ta2)->taHasStrikethrough && \
		(ta1)->taCapitals == (ta2)->taCapitals && \
		(ta1)->taTextColorNumber == (ta2)->taTextColorNumber && \
		(ta1)->taBorderNumber == (ta2)->taBorderNumber && \
		(ta1)->taShadingNumber == (ta2)->taShadingNumber )

#define FACE_INDEX( isS, isB ) ( 2*( (isS) != 0 )+ ( (isB) != 0 ) )
#define FACE_BOLD( idx ) ( (idx) % 2 != 0 )
#define FACE_SLANTED( idx ) ( ((idx)/2) % 2 != 0 )

#define TA_FONT_SIZE_TWIPS( ta ) ( 10* (ta)->taFontSizeHalfPoints )

/************************************************************************/
/*									*/
/*  Default attributes.							*/
/*									*/
/************************************************************************/

extern TextAttribute   DocDefaultAttributes;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void textInitTextAttribute(	TextAttribute *		ta );

extern void textUpdateTextAttribute(
				struct PropertyMask *		pDoneMask,
				TextAttribute *			ta,
				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet );

extern void textAttributeDifference(
				struct PropertyMask *		pDifMask,
				const TextAttribute *		ta1,
				const struct PropertyMask *	cmpMask,
				const TextAttribute *		ta2 );

extern int textSetTextProperty(		TextAttribute *		ta,
					int			prop,
					int			arg );

extern int textGetTextProperty(		const TextAttribute *	ta,
					int			prop );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_H	*/
