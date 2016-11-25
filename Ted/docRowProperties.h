/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_PROPS_H
#   define	DOC_ROW_PROPS_H

/************************************************************************/
/*									*/
/*  Units for table autoformatting:					*/
/*									*/
/************************************************************************/

typedef enum TableAutoFormatUnit
    {
    TRautoNONE= 0,
    TRautoAUTO,
    TRautoPERCENT_50,
    TRautoTWIPS,

    TRauto_COUNT
    } TableAutoFormatUnit;

#   define TRauto_BITS	2

#   if  (1<<TRauto_BITS) < TRauto_COUNT
    This will crash: Increase TRauto_BITS
#   endif

typedef enum CellTextFlow
    {
    CLflowTXLRTB= 0,	/*  left to right,  top to bottom (default).	*/
    CLflowTXTBRL,	/*  right to left,  top to bottom.		*/
    CLflowTXBTLR,	/*  left to right,  bottom to top.		*/
    CLflowTXLRTBV,	/*  left to right,  top to bottom, vertical.	*/
    CLflowTXTBRLV,	/*  top to bottom,  right to left, vertical.	*/

    CLflow_COUNT
    } CellTextFlow;

/************************************************************************/
/*									*/
/*  Cell Properties.							*/
/*									*/
/*  9)  Not real properties. Only used in setter routine(s)		*/
/*									*/
/************************************************************************/

typedef struct CellProperties
    {
    int			cpRightBoundaryTwips;
    int			cpRightBoundaryPixels;

    /**/
    BorderProperties	cpTopBorder;
    BorderProperties	cpBottomBorder;
    BorderProperties	cpLeftBorder;
    BorderProperties	cpRightBorder;

    ItemShading		cpShading;

    int			cpPreferredWidth;

    int			cpTopPadding;
    int			cpBottomPadding;
    int			cpLeftPadding;
    int			cpRightPadding;

			/* TableAutoFormatUnit */
    unsigned int	cpPreferredWidthUnit:TRauto_BITS;

    unsigned int	cpTopPaddingUnit:TRauto_BITS;
    unsigned int	cpBottomPaddingUnit:TRauto_BITS;
    unsigned int	cpLeftPaddingUnit:TRauto_BITS;
    unsigned int	cpRightPaddingUnit:TRauto_BITS;

    unsigned int	cpLeftInMergedRow:1;
    unsigned int	cpMergedWithLeft:1;
    unsigned int	cpTopInMergedColumn:1;
    unsigned int	cpMergedWithAbove:1;
    unsigned int	cpNoWrap:1;

    unsigned int	cpTextVerticalAlignment:3;	/*  (enum)	*/
    unsigned int	cpTextFlow:3;			/*  (enum)	*/
    } CellProperties;

#   define	docCleanCellProperties( cp ) ( ( *(cp)= *(cp) ), 0 )

typedef enum CellProperty
    {
    CLprop_NONE= -1,

    CLpropRIGHT_BOUNDARY= 0,

    CLpropTOP_BORDER,
    CLpropBOTTOM_BORDER,
    CLpropLEFT_BORDER,
    CLpropRIGHT_BORDER,

    CLpropSHADE_FORE_COLOR,
    CLpropSHADE_BACK_COLOR,
    CLpropSHADE_LEVEL,
    CLpropSHADE_PATTERN,

    CLpropCLW_WIDTH,
    CLpropCLFTS_WIDTH,

    CLpropLEFT_IN_MERGED,
    CLpropMERGED_WITH_LEFT,
    CLpropTOP_IN_MERGED,
    CLpropMERGED_WITH_ABOVE,
    CLpropNO_WRAP,

    CLpropTEXT_VERT_ALIGN,
    CLpropTEXT_FLOW,

    CLpropTOP_PADDING,
    CLpropBOTTOM_PADDING,
    CLpropLEFT_PADDING,
    CLpropRIGHT_PADDING,

    CLpropTOP_PADDING_UNIT,
    CLpropBOTTOM_PADDING_UNIT,
    CLpropLEFT_PADDING_UNIT,
    CLpropRIGHT_PADDING_UNIT,

    CLprop_ROWSPAN,			/*  9  */
    CLprop_COLSPAN,			/*  9  */

    CLprop_COUNT
    } CellProperty;

/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

typedef struct RowProperties
    {
    int			rpCellCount;
    CellProperties *	rpCells;

    int			rpHalfGapWidthTwips;
    int			rpHalfGapWidthPixels;

    int			rpLeftIndentTwips;
    int			rpLeftIndentPixels;

    int			rpHeightTwips;

			/************************************************/
			/*  Row borders are not stored in the word	*/
			/*  binary format, and seem to be irrelevant:	*/
			/************************************************/
    BorderProperties	rpTopBorder;
    BorderProperties	rpBottomBorder;
    BorderProperties	rpLeftBorder;
    BorderProperties	rpRightBorder;
    BorderProperties	rpHorizontalBorder;
    BorderProperties	rpVerticalBorder;

    ItemShading		rpShading;

    ItemAlignment	rpAlignment:3;

    unsigned int	rpIsTableHeader:1;
    unsigned int	rpKeepOnOnePage:1;
    unsigned int	rpKeepWithNext:1;
    unsigned int	rpAutofit:1;

    unsigned int	rpHasTableParagraphs:1;

    int			rpPreferredWidth;

    int			rpLeftDefaultCellSpacing;
    int			rpRightDefaultCellSpacing;
    int			rpTopDefaultCellSpacing;
    int			rpBottomDefaultCellSpacing;

    int			rpLeftDefaultCellMargin;
    int			rpRightDefaultCellMargin;
    int			rpTopDefaultCellMargin;
    int			rpBottomDefaultCellMargin;

    int			rpCellWidthBefore;
    int			rpCellWidthAfter;

			/* TableAutoFormatUnit */
    unsigned int	rpPreferredWidthUnit:TRauto_BITS;

			/* TableAutoFormatUnit */
    unsigned int	rpLeftDefaultCellSpacingUnit:TRauto_BITS;
    unsigned int	rpRightDefaultCellSpacingUnit:TRauto_BITS;
    unsigned int	rpTopDefaultCellSpacingUnit:TRauto_BITS;
    unsigned int	rpBottomDefaultCellSpacingUnit:TRauto_BITS;

			/* TableAutoFormatUnit */
    unsigned int	rpLeftDefaultCellMarginUnit:TRauto_BITS;
    unsigned int	rpRightDefaultCellMarginUnit:TRauto_BITS;
    unsigned int	rpTopDefaultCellMarginUnit:TRauto_BITS;
    unsigned int	rpBottomDefaultCellMarginUnit:TRauto_BITS;

			/* TableAutoFormatUnit */
    unsigned int	rpCellWidthBeforeUnit:TRauto_BITS;
    unsigned int	rpCellWidthAfterUnit:TRauto_BITS;
    } RowProperties;

typedef enum RowProperty
    {
    RPprop_NONE= -1,

    RPpropCOLUMNS= 0,

    RPpropGAP_WIDTH,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,

    RPpropTOP_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropHORIZ_BORDER,
    RPpropVERT_BORDER,

    RPpropSHADE_FORE_COLOR,
    RPpropSHADE_BACK_COLOR,
    RPpropSHADE_LEVEL,
    RPpropSHADE_PATTERN,

    RPpropALIGNMENT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPpropKEEP_WITH_NEXT,
    RPpropAUTOFIT,

    /**/
    RPpropTRW_WIDTH,
    RPpropTRFTS_WIDTH,

    /**/
    RPpropTRSPDL,
    RPpropTRSPDR,
    RPpropTRSPDT,
    RPpropTRSPDB,

    RPpropTRPADDL,
    RPpropTRPADDR,
    RPpropTRPADDT,
    RPpropTRPADDB,

    /**/
    RPpropTRSPDFL,
    RPpropTRSPDFR,
    RPpropTRSPDFT,
    RPpropTRSPDFB,

    RPpropTRPADDFL,
    RPpropTRPADDFR,
    RPpropTRPADDFT,
    RPpropTRPADDFB,

    /**/
    RPpropTRW_WIDTHB,
    RPpropTRW_WIDTHA,

    RPpropTRFTS_WIDTHB,
    RPpropTRFTS_WIDTHA,

    RPprop_COUNT
    } RowProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docShadingMaskToCellMask(	PropertyMask *		cpPropMask,
					const PropertyMask *	isPropMask );

extern void docShadingMaskFromCellMask(	PropertyMask *		isPropMask,
					const PropertyMask *	cpPropMask );

extern void docInitRowProperties(	RowProperties *		rp );
extern void docCleanRowProperties(	RowProperties *		rp );
extern void docCleanInitRowProperties(	RowProperties *		rp );

extern int docCopyRowProperties(	RowProperties *		to,
					const RowProperties *	from,
					const int *		colorMap );

extern int docInsertRowColumn(	RowProperties *			rp,
				int				n,
				const CellProperties *		cp,
				const int *			colorMap );

extern int docApproximatelyAlignedColumns(
				const RowProperties *	rp1,
				const RowProperties *	rp2 );

extern int docEqualRowBorders(	const RowProperties *	rp1,
				const RowProperties *	rp2 );

extern int docEqualRows(	const RowProperties *	rp1,
				const RowProperties *	rp2 );

extern void docInitCellProperties(	CellProperties *	cp );

extern int docUpdCellProperties(PropertyMask *			pCpChgPask,
				CellProperties *		cp,
				const PropertyMask *		cpUpdMask,
				const CellProperties *		cpNew,
				const int *			colorMap );

extern void docCellPropertyDifference(	
				PropertyMask *			pChgMask,
				const CellProperties *		cp1,
				const CellProperties *		cp2,
				const PropertyMask *		updMask,
				const int *			colorMap );

extern void docCopyCellProperties(	
				CellProperties *		cpTo,
				const CellProperties *		cpFrom,
				const int *			colorMap );

extern void docShadingMaskToRowMask(	PropertyMask *		rpPropMask,
					const PropertyMask *	isPropMask );

extern void docShadingMaskFromRowMask(	PropertyMask *		isPropMask,
					const PropertyMask *	rpPropMask );

extern int docUpdRowProperties(	PropertyMask *			pRpChgPask,
				RowProperties *			rp,
				const PropertyMask *		rpUpdMask,
				const RowProperties *		rpNew,
				const int *			colorMap );

extern void docRowPropertyDifference(
				PropertyMask *			pRpChgPask,
				const RowProperties *		rp1,
				const RowProperties *		rp2,
				const PropertyMask *		rpUpdMask,
				const int *			colorMap );

extern void docRowSetTopBorderInCols(	
				RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet );

extern void docRowSetBottomBorderInCols(	
				RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet );

extern void docRowSetLeftBorderInCols(	
				RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet );

extern void docRowSetRightBorderInCols(	
				RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet );

extern void docRowSetShadingInCols(
				RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		isSetMask,
				const ItemShading *		isSet );

extern int docDeleteColumnsFromRow(
				RowProperties *			rp,
				int				col0,
				int				count );

#   endif	/*  DOC_ROW_PROPS_H  */
