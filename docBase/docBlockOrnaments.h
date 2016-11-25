/************************************************************************/
/*									*/
/*  Ornaments of a block as it is drawn.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BLOCK_ORNAMENTS_H
#   define	DOC_BLOCK_ORNAMENTS_H

#   include	<utilPropMask.h>

struct BorderProperties;
struct ItemShading;

typedef enum DrawOrnament
    {
    ORNdrawSHADE= 0,

    ORNdrawTOP_BORDER,
    ORNdrawLEFT_BORDER,
    ORNdrawRIGHT_BORDER,
    ORNdrawBOTTOM_BORDER,

    ORNdrawTOP_GRID,
    ORNdrawLEFT_GRID,
    ORNdrawRIGHT_GRID,
    ORNdrawBOTTOM_GRID,

    ORNdraw_COUNT
    } DrawOrnament;

typedef struct BlockOrnaments
    {
					/**
					 *  Flags from DrawOrnament
					 */
    PropertyMask			boPropMask;

					/**
					 *  Top border number.
					 */
    int					boTopBorderNumber;

					/**
					 *  Left border number. (geometric)
					 *
					 *  This the geometric left. It is 
					 *  the responsibility of the setter 
					 *  to swap left and right border of 
					 *  the cells in a right-to-left table 
					 *  row.
					 */
    int					boLeftBorderNumber;

					/**
					 *  Right border number. (geometric)
					 */
    int					boRightBorderNumber;

					/**
					 *  Bottom border number.
					 */
    int					boBottomBorderNumber;

					/**
					 *  Top border properties.
					 */
    const struct BorderProperties *	boTopBorder;

					/**
					 *  Left border properties. (geometric)
					 */
    const struct BorderProperties *	boLeftBorder;

					/**
					 *  Right border properties. (geometric)
					 */
    const struct BorderProperties *	boRightBorder;

					/**
					 *  Bottom border properties.
					 */
    const struct BorderProperties *	boBottomBorder;

    const struct ItemShading *		boShading;
    } BlockOrnaments;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBlockOrnaments(	BlockOrnaments *	bo );

#   endif	/*  DOC_BLOCK_ORNAMENTS_H  */
