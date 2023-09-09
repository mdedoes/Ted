/************************************************************************/
/*									*/
/*  Cell Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CELL_PROPS_H
#   define	DOC_CELL_PROPS_H

struct PropertyMask;
struct DocumentAttributeMap;

/************************************************************************/
/*									*/
/*  Cell Properties.							*/
/*									*/
/************************************************************************/

typedef enum CellMergePos
    {
    CELLmergeNONE= 0,
    CELLmergeHEAD,
    CELLmergeFOLLOW,

    CELLmerge_COUNT
    } CellMergePos;

typedef struct CellProperties
    {
			/**
			 *  The width of the cell. The rtf readers/writers 
			 *  translate back and forth between right boundary 
			 *  and width.
			 */
    int			cpWide;

			/**
			 *  The top border of the cell.
			 */
    int			cpTopBorderNumber;

			/**
			 *  The left border of the cell.
			 *  In right-to-left rows, left and right border 
			 *  are swapped.
			 */
    int			cpLeftBorderNumber;

			/**
			 *  The right border of the cell.
			 *  In right-to-left rows, left and right border 
			 *  are swapped.
			 */
    int			cpRightBorderNumber;

			/**
			 *  The bottom border of the cell.
			 */
    int			cpBottomBorderNumber;

			/**
			 *  The (potential) top-left to bottom-right diagonal
			 *  style. This is a border style. (\\cldglu)
			 */
    int			cpLtrDiagonalBorderNumber;

			/**
			 *  The (potential) top-right to bottom-left diagonal
			 *  style. This is a border style. (\\cldgll)
			 */
    int			cpRtlDiagonalBorderNumber;

			/**
			 *  The (potential) shading style of the cell.
			 */
    int			cpShadingNumber;

    int			cpPreferredWidth;

			/**
			 *  Padding at the top of the cell. 
			 *  Is only used if cpTopPaddingUnit == 3.
			 *  NOTE that for compatibility with MS-Word 
			 *  2003 and 2010, the RTF reader and writer 
			 *  swap left and top padding.
			 */
    short int		cpTopPadding;

			/**
			 *  Padding at the left of the cell. 
			 *  MS-Word swaps left and top padding in the RTF 
			 *  file.
			 */
    short int		cpLeftPadding;

			/**
			 *  Padding at the right of the cell. 
			 */
    short int		cpRightPadding;

			/**
			 *  Padding at the bottom of the cell. 
			 */
    short int		cpBottomPadding;

			/* TableAutoFormatUnit */
    unsigned char	cpPreferredWidthUnit;

    unsigned char	cpTopPaddingUnit;
    unsigned char	cpBottomPaddingUnit;
    unsigned char	cpLeftPaddingUnit;
    unsigned char	cpRightPaddingUnit;

    unsigned char	cpHorizontalMerge;
    unsigned char	cpVerticalMerge;
    unsigned char	cpNoWrap;

    unsigned char	cpNoShading; /* clshdrawnil */

			/**
			 *  Vertical alignment of the contents of the cell.
			 *  The default is DOCtvaTOP. Other possible values are 
			 *  DOCtvaCENTERED and DOCtvaBOTTOM.
			 */
    unsigned char	cpValign;

    unsigned char	cpTextFlow;			/*  (enum)	*/

			/**
			 * In analogy with \\trhdr, \\clhdr tells that the cell
			 * is a row header. This is an extension of the RTF file format
			 * to support HTML and tagged PDF generation.
			 */
    unsigned char	cpIsRowHeader;
    } CellProperties;

#   define CELL_MERGED( cp ) \
		( (cp)->cpHorizontalMerge == CELLmergeFOLLOW || \
		  (cp)->cpVerticalMerge == CELLmergeFOLLOW )

typedef enum CellProperty
    {
    CLprop_NONE= -1,

    CLpropWIDTH= 0,

    CLpropTOP_BORDER,
    CLpropLEFT_BORDER,
    CLpropRIGHT_BORDER,
    CLpropBOTTOM_BORDER,

    CLpropLEFT_RIGHT_DIAGONAL,
    CLpropRIGHT_LEFT_DIAGONAL,

    CLpropSHADING,
    CLpropCLW_WIDTH,
    CLpropCLFTS_WIDTH,

    CLpropHOR_MERGE,
    CLpropVERT_MERGE,
    CLpropNO_WRAP,

    CLpropVALIGN,
    CLpropTEXT_FLOW,

    CLpropTOP_PADDING,
    CLpropBOTTOM_PADDING,
    CLpropLEFT_PADDING,
    CLpropRIGHT_PADDING,

    CLpropTOP_PADDING_UNIT,
    CLpropBOTTOM_PADDING_UNIT,
    CLpropLEFT_PADDING_UNIT,
    CLpropRIGHT_PADDING_UNIT,

    CLpropNO_SHADING,
    CLpropROW_HEADER,

# define CLprop_COUNT CLprop_NOT_SUPPORTED
    CLprop_NOT_SUPPORTED,

    CLprop_FULL_COUNT
    } CellProperty;

/************************************************************************/
/*									*/
/*  Application of table styles.					*/
/*									*/
/************************************************************************/

typedef enum CellInstanceProperty
    {
    CIpropYTS= 0,

    CIpropFIRST_ROW,
    CIpropLAST_ROW,

    CIpropFIRST_COLUMN,
    CIpropLAST_COLUMN,

    CIpropODD_ROW_BAND,
    CIpropEVEN_ROW_BAND,

    CIpropODD_COLUMN_BAND,
    CIpropEVEN_COLUMN_BAND,

    CIpropNW_CELL,
    CIpropNE_CELL,
    CIpropSW_CELL,
    CIpropSE_CELL,

    CIprop_COUNT
    } CellInstanceProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCellProperties(	CellProperties *	cp );

extern int docUpdCellProperties(struct PropertyMask *		pCpDoneMask,
				CellProperties *		cp,
				const struct PropertyMask *	cpSetMask,
				const CellProperties *		cpSet,
				const struct DocumentAttributeMap * dam );

extern void docCellPropertyDifference(	
				struct PropertyMask *		pDifMask,
				const CellProperties *		cp1,
				const struct PropertyMask *	cpCmpMask,
				const CellProperties *		cp2,
				const struct DocumentAttributeMap * dam );

extern int docCopyCellProperties(	
				CellProperties *		cpTo,
				const CellProperties *		cpFrom,
				const struct DocumentAttributeMap * dam );

extern int docSetCellProperty(		CellProperties *	cp,
					int			prop,
					int			arg );

extern int docGetCellProperty(		const CellProperties *	cp,
					int			prop );

#   endif	/*  DOC_CELL_PROPS_H  */
