/************************************************************************/
/*									*/
/*  Section Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_PROP_H
#   define	DOC_SECT_PROP_H

#   include	<geoDocumentPageSetup.h>
#   include	"docNotesProperties.h"

struct DocumentProperties;

typedef enum PageNumberStyle
    {
    DOCpgnDEC= 0,
    DOCpgnUCRM,
    DOCpgnLCRM,
    DOCpgnUCLTR,
    DOCpgnLCLTR,

    DOCpgn_COUNT
    } PageNumberStyle;

typedef enum PageNumberHyphen
    {
    DOCpgnhPGNHNSH,
    DOCpgnhPGNHNSP,
    DOCpgnhPGNHNSC,
    DOCpgnhPGNHNSM,
    DOCpgnhPGNHNSN,

    DOCpgnh_COUNT
    } PageNumberHyphen;

typedef struct SectionColumn
    {
		/**
		 *  The width of the column
		 */
    int		scColumnWidthTwips;

		/**
		 *  The space between the column and the subsequent one.
		 *  (So this is irrelevant for the last column.)
		 */
    int		scSpaceAfterTwips;
    } SectionColumn;

typedef struct SectionProperties
    {
				/**
				 *  Page size, margins and header/footer
				 *  positions.
				 */
    DocumentGeometry		spDocumentGeometry;

				/**
				 *  Style of this section. This is a reference 
				 *  to a style in the document style sheet.
				 */
    int				spStyle;

				/**
				 *  The spacing between the newspaper style
				 *  columns of this section. [Only relevant 
				 *  if the section has more than one column.
				 */
    int				spColumnSpacingTwips;
				/**
				 *  If non zero: draw a line between the
				 *  columns of this section. [Only relevant 
				 *  if the section has more than one column.
				 */
    unsigned char		spLineBetweenColumns;

				/**
				 *  Use different headers and footers on the 
				 *  first page of the section.
				 */
    unsigned char		spHasTitlePage;
				/**
				 *  Use different headers and footers on the 
				 *  last page of the section. Unless those for
				 *  the first page apply. NOTE: This is not a 
				 *  MS-Word RTF feature. Limitations apply for
				 *  the height of the last page header.
				 */
    unsigned char		spHasEndPage;
				/**
				 *  The kind of section break before this 
				 *  section. NOTE that MS-Word only implements
				 *  odd and even breaks in documents that have 
				 *  have different odd an even pages in the 
				 *  document properties. And that for 
				 *  compatibility with MS-Word Ted implements 
				 *  the same thing.
				 */
    unsigned char		spBreakKind;

				/**
				 *  Style of the page numbers for this section.
				 */
    unsigned char		spPageNumberStyle;

				/**
				 *  Style of the hyphen in the page numbers 
				 *  for this section.
				 */
    unsigned char		spPageNumberHyphen;

				/**
				 *  Use per section page numbering for this 
				 *  section. (A boolean flag.)
				 *
				 *  Also restart the numbering of the top level of
				 *  list numbers (bullets)
				 */
    unsigned char		spRestartPageNumbers;

				/**
				 * 0: This section will snake (newspaper style)
				 *	columns from left to right (the default)
				 * 1: This section will snake (newspaper style)
				 *	columns from right to left
				 */
    unsigned char		spRToL;

				/**
				 * Do not mark the header of the first page of
				 * this section as an artifact in the PdfMarks
				 * in the PS/PDF output. This is not a regular 
				 * RTF feature. (\sfsthdnopdfart)
				 */
    unsigned char		spFirstPageHeaderNoPdfArtifact;

				/**
				 * Do not mark the footer of the first page of
				 * this section as an artifact in the PdfMarks
				 * in the PS/PDF output. This is not a regular 
				 * RTF feature. (\sfstftnopdfart)
				 */
    unsigned char		spFirstPageFooterNoPdfArtifact;

				/**
				 * Do not mark the header of the last page of
				 * this section as an artifact in the PdfMarks
				 * in the PS/PDF output. This is not a regular 
				 * RTF feature. (\slsthdnopdfart)
				 */
    unsigned char		spLastPageHeaderNoPdfArtifact;

				/**
				 * Do not mark the footer of the last page of
				 * this section as an artifact in the PdfMarks
				 * in the PS/PDF output. This is not a regular 
				 * RTF feature. (\slsftdnopdfart)
				 */
    unsigned char		spLastPageFooterNoPdfArtifact;

				/**
				 *  If this section uses per section page 
				 *  numbering, use this number as the number 
				 *  of the first page. Counts from 1!
				 */
    int				spStartPageNumber;

				/**
				 *  The newspaper style columns of this section.
				 *  Columns are only allocated if there is 
				 *  more than one column. In RToL sections,
				 *  the first column is on the right hand 
				 *  side of the page.
				 */
    int				spColumnCount;
    SectionColumn *		spColumns;

				/**
				 *  Determines the placement and numbering 
				 *  of footnotes and endnotes in this 
				 *  section. (If any)
				 */
    FootEndNotesProperties	spNotesProperties;
    } SectionProperties;

# define DOC_SECTpropsBELOW_PREVIOUS( sp ) \
				( (sp)->spBreakKind == DOCibkNONE )

# define docSectPropsFixedWidthColumns( sp ) \
    ( (sp)->spColumnCount < 2 || (sp)->spColumns[0].scColumnWidthTwips == 0 )

typedef enum SectionProperty
    {
    SPprop_NONE= -1,

    /* document geometry */

    SPpropSTYLE= DGprop_COUNT,
    SPpropTITLEPG,
    SPpropENDPG,
    SPpropBREAK_KIND,
    SPpropNUMBER_STYLE,
    SPpropNUMBER_HYPHEN,
    SPpropPAGE_RESTART, /* Counts from 1! */
    SPpropRTOL,
    SPpropFST_HEADER_NO_PDF_ARTIFACT,
    SPpropFST_FOOTER_NO_PDF_ARTIFACT,
    SPpropLST_HEADER_NO_PDF_ARTIFACT,
    SPpropLST_FOOTER_NO_PDF_ARTIFACT,
    SPpropSTART_PAGE,
    SPpropCOLUMN_COUNT,
    SPpropCOLUMN_SPACING,
    SPpropLINEBETCOL,
    SPpropCOLUMNS,

    SPpropFOOTNOTE_STARTNR,
    SPpropFOOTNOTE_JUSTIFICATION,
    /* No SPpropFOOTNOTE_PLACEMENT, */
    SPpropFOOTNOTE_RESTART,
    SPpropFOOTNOTE_STYLE,

    SPpropENDNOTE_STARTNR,
    /* No SPpropENDNOTE_JUSTIFICATION, */
    /* No SPpropENDNOTE_PLACEMENT, */
    SPpropENDNOTE_RESTART,
    SPpropENDNOTE_STYLE,

    SPprop_COUNT,

		    /****************************************************/
		    /*  Used by RTF parser:				*/
		    /****************************************************/
    SPprop_COLUMN_NUMBER,
    SPprop_COLUMN_WIDTH,
    SPprop_COLUMN_RIGHT,

    SPprop_FULL_COUNT
    } SectionProperty;

extern const int DOCsectNOTE_PROP_MAP[FEPprop_COUNT];
extern const int * const DOCsectFOOTNOTE_PROP_MAP;
extern const int * const DOCsectENDNOTE_PROP_MAP;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSectionProperties(	SectionProperties *	sp );
extern void docCleanSectionProperties(	SectionProperties *	sp );

extern int docCopySectionProperties(	SectionProperties *		to,
					const SectionProperties *	from );

extern int docSectionPropertiesSetColumnCount(
					SectionProperties *	sp,
					int			count );

extern void docSectPropertyDifference(
				struct PropertyMask *		pDiffMask,
				const SectionProperties *	sp1,
				const struct PropertyMask *	cmpMask,
				const SectionProperties *	sp2 );

extern int docUpdSectProperties( struct PropertyMask *		pSpDoneMask,
				SectionProperties *		spTo,
				const struct PropertyMask *	spSetMask,
				const SectionProperties *	spSet );

extern int docSectSetEqualColumnWidth(	SectionProperties *		sp );
extern int docSectSetExplicitColumnWidth( SectionProperties *		sp );

extern void docSectGetColumnX(	int *				pXLine,
				int *				pX0,
				int *				pX1,
				const SectionProperties *	sp,
				const DocumentGeometry *	dgPage,
				int				column );

extern int docSetSectionProperty(	SectionProperties *	sp,
					int			prop,
					int			arg );

extern int docGetSectionProperty(	const SectionProperties *	sp,
					int				prop );

extern void docFillSectNotesMask(	struct PropertyMask *		spMask );

extern int docSectGetColumnSpacing(
				int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col );

extern int docSectSetColumnSpacing(	SectionProperties *	sp,
					int			col,
					int			value );

extern int docSectGetColumnWidth(
				int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col );

extern int docSectSetColumnWidth(	SectionProperties *	sp,
					int			col,
					int			value );

extern int docSectGetBreakKind(
		const struct SectionProperties *	sp,
		const struct DocumentProperties *	dp,
		int					honourSpecialBreaks );

#   endif	/*  DOC_SECT_PROP_H  */
