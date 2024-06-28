/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_PROPS_H
#   define	DOC_ROW_PROPS_H

struct PropertyMask;
struct DocumentAttributeMap;
struct CellProperties;

/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

typedef struct RowProperties
    {
    int				rpCellCount;
    struct CellProperties *	rpCells;

    int				rpHalfGapWidthTwips;
    int				rpLeftIndentTwips;
    int				rpHeightTwips;

    int				rpRowNumber;
    int				rpRowBandNumber;
    int				rpRowStyle;

				/**
				 *  Row Borders.
				 *  Row borders are not stored in the word
				 *  binary format, and seem to be irrelevant.
				 */
    int				rpTopBorderNumber;
    int				rpLeftBorderNumber;
    int				rpRightBorderNumber;
    int				rpBottomBorderNumber;
    int				rpVerticalBorderNumber;
    int				rpHorizontalBorderNumber;

    int				rpShadingNumber;
    int				rpFrameNumber;

    unsigned char		rpAlignment;

				/**
				 *  Is this row a table header row? 
				 *  Ted might handle header rows that are 
				 *  not a contiguous range at the top of the 
				 *  table incorrectly.
				 */
    unsigned char		rpIsTableHeader;

				/**
				 *  Try to keep the row on one page?
				 */
    unsigned char		rpKeepOnOnePage;

				/**
				 *  Try to keep the row on the same page 
				 *  as the subsequent one. Newer versions 
				 *  of MS-Word derive this from the same 
				 *  property of the first paragraph in the row.
				 */
    unsigned char		rp_Keepfollow;

    unsigned char		rpAutofit;
				/**
				 * 0:	Cells in this table row will have 
				 *	left-to-right *	precedence (the default)
				 * 1:	Cells in this table row will have 
				 *	right-to-left precedence
				 */
    unsigned char		rpRToL;

    int				rpPreferredWidth;

    int				rpTopCellSpacing;
    int				rpLeftCellSpacing;
    int				rpRightCellSpacing;
    int				rpBottomCellSpacing;

    int				rpTopCellPadding;
    int				rpLeftCellPadding;
    int				rpRightCellPadding;
    int				rpBottomCellPadding;

    int				rpCellWidthBefore;
    int				rpCellWidthAfter;

    int				rpAuthor;

				/**
				 *  Do not emit PDF content marks that mark this
				 *  table row in RTF as a table row in PDF. Independent of
				 *  its actual ornaments, row is also counted as plain when 
				 *  we check whether the table that holds it is plain.
				 *
				 *  I.E. the row is excluded from the table. Flat rows at
				 *  the head and tail are pruned from the table. Flat rows
				 *  in the interior of the RTF table divide the RTf table 
				 *  in multiple PDF tables.
				 */
    unsigned char		rpFlatInPDF;

				/* TableAutoFormatUnit */
    unsigned char		rpPreferredWidthUnit;

				/* TableAutoFormatUnit */
    unsigned char		rpTopCellSpacingUnit;
    unsigned char		rpLeftCellSpacingUnit;
    unsigned char		rpRightCellSpacingUnit;
    unsigned char		rpBottomCellSpacingUnit;

				/* TableAutoFormatUnit */
    unsigned char		rpTopCellPaddingUnit;
    unsigned char		rpLeftCellPaddingUnit;
    unsigned char		rpRightCellPaddingUnit;
    unsigned char		rpBottomCellPaddingUnit;

				/* TableAutoFormatUnit */
    unsigned char		rpCellWidthBeforeUnit;
    unsigned char		rpCellWidthAfterUnit;

    unsigned char		rpAutoformatBorders;
    unsigned char		rpAutoformatShading;
    unsigned char		rpAutoformatFont;
    unsigned char		rpAutoformatColor;
    unsigned char		rpAutoformatApplyBestFit;
    unsigned char		rpAutoformatFirstRow;
    unsigned char		rpAutoformatLastRow;
    unsigned char		rpAutoformatFirstColumn;
    unsigned char		rpAutoformatLastColumn;

    unsigned char		rpIsLastRow;
    } RowProperties;

typedef enum RowProperty
    {
    RPprop_NONE= -1,

    RPpropGAP_WIDTH= 0,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,

    RPpropTOP_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropVERT_BORDER,
    RPpropHORIZ_BORDER,

    RPpropSHADING,
    RPpropFRAME,

    RPpropALIGNMENT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPpropAUTOFIT,
    RPpropRTOL,

    /**/
    RPpropTRW_WIDTH,
    RPpropTRFTS_WIDTH,

    /**/
    RPpropTRSPDT,
    RPpropTRSPDL,
    RPpropTRSPDR,
    RPpropTRSPDB,

    RPpropTRPADDT,
    RPpropTRPADDL,
    RPpropTRPADDR,
    RPpropTRPADDB,

    /**/
    RPpropTRSPDFT,
    RPpropTRSPDFL,
    RPpropTRSPDFR,
    RPpropTRSPDFB,

    RPpropTRPADDFT,
    RPpropTRPADDFL,
    RPpropTRPADDFR,
    RPpropTRPADDFB,

    /**/
    RPpropTRW_WIDTHB,
    RPpropTRW_WIDTHA,

    RPpropTRFTS_WIDTHB,
    RPpropTRFTS_WIDTHA,

    /**/
    RPpropAUTOFORMAT_BORDERS,
    RPpropAUTOFORMAT_SHADING,
    RPpropAUTOFORMAT_FONT,
    RPpropAUTOFORMAT_COLOR,
    RPpropAUTOFORMAT_APPLY_BEST_FIT,
    RPpropAUTOFORMAT_FIRST_ROW,
    RPpropAUTOFORMAT_LAST_ROW,
    RPpropAUTOFORMAT_FIRST_COLUMN,
    RPpropAUTOFORMAT_LAST_COLUMN,

    RPpropROW_NUMBER,
    RPpropROW_BAND_NUMBER,
    RPpropROW_STYLE,

    RPpropIS_LAST_ROW,
    RPpropTRAUTH,

    RPpropTRFLAT,

    RPprop_COUNT,
					/* For read/write/tool only */
#   define	RPprop_KEEPFOLLOW	RPprop_COUNT
    RPprop_ADMIN_COUNT,
		/**
		 *  The number of cells changes: This affects the whole 
		 *  row layout. Protruding cells are removed at the end 
		 *  or extra cells are copied after the existing cells.
		 *  The RowPropertiesAdmin relies on the fact that 
		 *  RPprop_CELL_COUNT == RPprop_ADMIN_COUNT
		 */
#   define RPprop_CELL_COUNT	RPprop_ADMIN_COUNT

    RPprop_IGNORED,		/*  For read/write/tool only	*/

		/**
		 *  Other cell properties such as borders and shading.
		 *  are included. The logical OR of all changes is included 
		 *  in the result cell mask. All properties in the input cell 
		 *  mask are applied.
		 */
    RPprop_CELL_PROPS,
    RPprop_SUBJECT,

    RPprop_FULL_COUNT
    } RowProperty;

# define RP_IS_ONE_PAGE( rp ) ( \
		    (rp)->rpKeepOnOnePage	|| \
		    (rp)->rp_Keepfollow		|| \
		    (rp)->rpHeightTwips != 0	|| \
		    (rp)->rpIsTableHeader	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRowProperties(	RowProperties *		rp );
extern void docCleanRowProperties(	RowProperties *		rp );

extern int docCopyRowProperties( RowProperties *		to,
				const RowProperties *		from,
				const struct DocumentAttributeMap * dam );

extern int docInsertRowColumn(	RowProperties *			rp,
				int				n,
				const struct CellProperties *	cp,
				const struct DocumentAttributeMap * dam );

extern int docUpdRowProperties(	struct PropertyMask *		pRpDoneMask,
				struct PropertyMask *		pCpDoneMask,
				RowProperties *			rpTo,
				const struct PropertyMask *	rpSetMask,
				const struct PropertyMask *	cpSetMask,
				const RowProperties *		rpSet,
				int				colTo,
				int				colCount,
				const struct DocumentAttributeMap * dam );

extern int docDeleteColumnsFromRow(
				RowProperties *			rp,
				int				col0,
				int				count );

extern int docSetRowProperty(		RowProperties *		rp,
					int			prop,
					int			arg );

extern int docGetRowProperty(		const RowProperties *	rp,
					int			prop );

extern int docEqualWidthColumns(	RowProperties *		rp,
					int			columns,
					int			wide,
					int			fsHalfPoints );

extern int docRowGetColumnX(		int *			pX0,
					int *			pX1,
					int *			pX11,
					int *			pColspan,
					const RowProperties *	rp,
					int			col );

extern int docRowPropertiesMakeColWider( RowProperties *	rp,
					int			col,
					int			wider );

extern int docRowPropertyDifference(
				struct PropertyMask *		pRpDifMask,
				struct PropertyMask *		pCpDifMask,
				const RowProperties *		rp1,
				const struct PropertyMask *	rpCmpMask,
				const struct PropertyMask *	cpCmpMask,
				const RowProperties *		rp2,
				int				colTo,
				int				colCount,
				const struct DocumentAttributeMap * dam );

extern void docRowPropertiesSetCellDefaults(
				struct CellProperties *		cp,
				const RowProperties *		rp );

#   endif	/*  DOC_ROW_PROPS_H  */
