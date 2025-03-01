/************************************************************************/
/*									*/
/*  Item Shading.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ITEM_SHADING_H
#   define	DOC_ITEM_SHADING_H

#   include	<utilColor.h>

struct PropertyMask;
struct ColorPalette;

/************************************************************************/
/*									*/
/*  1)  The shading of a BufferItem such as a table cell or a		*/
/*	paragraph.							*/
/*  2)  Numbers for the properties.					*/
/*  3)  Expanded form of the shading for the use in tools. (Remove the	*/
/*	indirection via the palette).					*/
/*									*/
/************************************************************************/

/*  1  */
typedef struct ItemShading
    {
    short		isBackColor;
    short		isForeColor;

    unsigned short	isLevel;
    unsigned char	isPattern;
    } ItemShading;

# define DOCisSHADING(is) ((is)->isBackColor!=0||(is)->isForeColor!=0)

/*  2  */
typedef enum ShadingProperty
    {
    ISprop_NONE= -1,

    ISpropBACK_COLOR= 0,
    ISpropFORE_COLOR,
    ISpropLEVEL,
    ISpropPATTERN,

    ISprop_COUNT,

    ISpropBACK_SOLID,

    ISprop_FULL_COUNT
    } ShadingProperty;

/*  3  */
typedef struct ExpandedItemShading
    {
    int			eisBackColorExplicit;
    int			eisForeColorExplicit;
    RGB8Color		eisBackColor;
    RGB8Color		eisForeColor;

    unsigned int	eisLevel;
    unsigned int	eisPattern;
    } ExpandedItemShading;

#   define	docCopyItemShading( is1, is2 ) *(is1)= *(is2)
#   define	docCleanItemShading( is ) /*void*/

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitItemShading(		ItemShading *		is );
extern void docInitExpandedItemShading(	ExpandedItemShading *	eis );

extern void docUpdateItemShading(
			struct PropertyMask *			pIsDoneMask,
			ItemShading *				isTo,
			const struct PropertyMask *		isSetMask,
			const ItemShading *			isFrom,
			const int *				colorMap );

extern void docUpdateExpandedItemShading(
			struct PropertyMask *			pIsDoneMask,
			ExpandedItemShading *			eisTo,
			const struct PropertyMask *		isSetMask,
			const ExpandedItemShading *		eisFrom );

extern void docExpandItemShading(	ExpandedItemShading *	eis,
					const ItemShading *	is,
					const struct ColorPalette *	cp );

extern int docIndirectItemShading(
			struct PropertyMask *			pDoneMask,
			ItemShading *				is,
			const struct PropertyMask *		setMask,
			const ExpandedItemShading *		eis,
			struct ColorPalette *			cp );

extern int docSetShadingProperty(	ItemShading *		is,
					int			prop,
					int			arg );

extern int docGetShadingProperty(	const ItemShading *	is,
					int			prop );

#   endif	/*  DOC_ITEM_SHADING_H  */
