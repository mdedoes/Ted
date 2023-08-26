/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PROP_H
#   define	DOC_PARA_PROP_H

struct PropertyMask;
struct DocumentAttributeMap;

/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/************************************************************************/

typedef struct ParagraphProperties
    {
			/**
			 *  The tab stops in the paragraph. The positions 
			 *  are relative to the paragraph frame. Every time, 
			 *  a tab control appears in the text, the formatting
			 *  of the text proceeds to the first tab stop 
			 *  position to the right of the current position.

			 *  Orientation is relative to the paragraph direction:
			 *  For right-to-left paragraphs, the positions are 
			 *  from the right hand side of the paragraph frame 
			 *  and formatting of the text proceeds to the right 
			 *  of the current position.
			 */
    short int		ppTabStopListNumber;

			/**
			 *  The indentation of the first line of the 
			 *  paragraph. The value is relative to the 
			 *  left or right indent of the paragraph.
			 *
			 *  So for left-to-right paragraphs, the value of 
			 *  the first line indentation is added to the left 
			 *  hand side indentation, and for right-to-left 
			 *  paragraphs the the first line indentation is 
			 *  added to the right hand side indentation.
			 */
    short int		ppFirstIndentTwips;

			/**
			 *  The indentation on the left hand side of the 
			 *  lines of the paragraph relative to the frame of 
			 *  the paragraph. For a paragraph outside a table, 
			 *  the frame of the paragraph is given by the page 
			 *  margins. 
			 *
			 *  Orientation is geometric: It is the indentation 
			 *  at the left hand side of the paragraph frame.
			 *  The meaning is independent of the paragraph
			 *  direction.
			 */
    short int		ppLeftIndentTwips;

			/**
			 *  The indentation on the right hand side of the 
			 *  lines of the paragraph relative to the frame of 
			 *  the paragraph. For a paragraph outside a table, 
			 *  the frame of the paragraph is given by the page 
			 *  margins.
			 *
			 *  Orientation is geometric: It is the indentation 
			 *  at the right hand side of the paragraph frame.
			 *  The meaning is independent of the paragraph
			 *  direction.
			 */
    short int		ppRightIndentTwips;

			/**
			 *  The amount of white space before (above) the lines 
			 *  of the paragraph.
			 */
    short int		ppSpaceBeforeTwips;

			/**
			 *  The amount of white space after (below) the lines 
			 *  of the paragraph.
			 */
    short int		ppSpaceAfterTwips;

			/**
			 *  Line spacing: the distance of the lines of the 
			 *  paragraph. (\\sl0 means automatic.) Positive 
			 *  values are a minimum value for spacing. 
			 *  Negative values give an exact value. The 
			 *  absolute value is used even if the characters in 
			 *  the line are higher. (\\slmult: see below 
			 *  controls where white space is added)
			 */
    short int		ppLineSpacingTwips;

			/**
			 *  Reference to a style in the style sheet.
			 *  Style 0 is not used: It is used to indicate that 
			 *  no style applies.
			 */
    short int		ppStyleNumber;

			/**
			 *  Reference to a list style override in the list
			 *  override table. Override 0 is not used: It is 
			 *  used to indicate that the paragraph is not part 
			 *  of a list.
			 */
    short int		ppListOverride;

			/**
			 *  Offset in the (Document global) list of 
			 *  border properties of the border properties 
			 *  of the top border of this paragraph.
			 */
    int			ppTopBorderNumber;

			/**
			 *  Offset in the (Document global) list of 
			 *  border properties of the border properties 
			 *  of the bottom border of this paragraph.
			 */
    int			ppBottomBorderNumber;

			/**
			 *  Offset in the (Document global) list of 
			 *  border properties of the border properties 
			 *  of the left border of this paragraph.
			 */
    int			ppLeftBorderNumber;

			/**
			 *  Offset in the (Document global) list of 
			 *  border properties of the border properties 
			 *  of the right border of this paragraph.
			 */
    int			ppRightBorderNumber;

    int			ppBetweenBorderNumber;
    int			ppBarNumber;

			/**
			 *  Offset in the (Document global) list of 
			 *  shading values of the shading behind the text
			 *  of this paragraph.
			 */
    int			ppShadingNumber;

    /*  7  */
    int			ppFrameNumber;

			/**
			 *  The nesting level of the paragraph in tables.
			 *  At level 1, this is given by the intbl control.
			 *  Deeper levels with the itapN control.
			 */
    unsigned char	ppTableNesting;

			/**
			 *  The kind of break before the paragraph.
			 *  Possible values are:
			 *  - DOCibkNONE No break before
			 *  - DOCibkCOL Column break
			 *  - DOCibkPAGE Page break
			 *
			 *  As the rtf file format does not support paragraphs
			 *  with column breaks, a paragraph with a column break 
			 *  is saved with a column break at its beginning.
			 */
    unsigned char	ppBreakKind;

			/**
			 *  The outline level of the paragraph. (outlinelevel)
			 *  Value go from 0 to 8 for actually outlined text.
			 *  level 9 is the default and refers to body text.
			 *  Levels 0 .. 5 correspond to HTML headers H1..H6
			 */
    unsigned char	ppOutlineLevel;

			/**
			 *  The list level of the paragraph. (ilvl)
			 *  Values range from 0 to 9.
			 */
    unsigned char	ppListLevel;

			/**
			 *  The alignment of the paragraph. (ql, qr , qc, qj)
			 *  The alignment is a physical alignment as it is 
			 *  displayed on screen: It is NOT relative to the 
			 *  paragraph direction.
			 *
			 *  The MS-Word user interface swaps the values 
			 *  for left and right in the user interface in a 
			 *  RightToLeft paragraph, but it stores the alignment
			 *  as it is actually displayed in the file.
			 */
    unsigned char	ppAlignment;

			/**
			 *  With \\slmult0 (the default) white space is 
			 *  added at the top of the lines of the paragraph 
			 *  to get the desired (\\slN) height of the line.
			 *  With \\slmult1 and a positive value for \\slN,
			 *  white space is added at the bottom.
			 */
    unsigned char	ppLineSpacingIsMultiple;

			/**
			 *  If true, (try to) keep the text of this 
			 *  paragraph on one page, or rather in one 
			 *  newspaper style column. (boolean)
			 */
    unsigned char	ppKeepOnPage;

			/**
			 *  If true, (try to) keep the text of this 
			 *  paragraph on the same page as the first line of 
			 *  the subsequent paragraph in the document. (Or 
			 *  rather the same newspaper style column) (boolean)
			 *  This is the way RTF implements paragraph headers.
			 */
    unsigned char	ppKeepWithNext;

    unsigned char	ppWidowControl;			/*  boolean	*/
    unsigned char	ppHyphenateParagraph;		/*  boolean	*/

			/**
			 * 0: Text in this paragraph will display with
			 *	left-to-right precedence (the default)
			 * 1: Text in this paragraph will display with
			 *	right-to-left precedence
			 */
    unsigned char	ppRToL;
    } ParagraphProperties;

/************************************************************************/
/*									*/
/*  Numbers of paragraph properies.					*/
/*									*/
/*  NOTE That the order determines the order in which the tags are	*/
/*  emitted to the RTF file.						*/
/*									*/
/************************************************************************/

typedef enum ParagraphProperty
    {
    PPprop_NONE= -1,

    PPpropSTYLE= 0,
    PPpropLISTOVERRIDE,
    PPpropLISTLEVEL,
    PPpropTABLE_NESTING,
    PPpropHYPHPAR,
    PPpropWIDCTLPAR,
    PPpropRTOL,
    PPpropKEEP,
    PPpropKEEPN,
    PPpropOUTLINELEVEL,
    PPpropFIRST_INDENT,
    PPpropLEFT_INDENT,
    PPpropRIGHT_INDENT,
    PPpropSPACE_BEFORE,
    PPpropSPACE_AFTER,
    PPpropLINE_SPACING_DIST,
    PPpropLINE_SPACING_MULT,
    PPpropALIGNMENT,
    PPpropTAB_STOPS,

    PPpropTOP_BORDER,
    PPpropBOTTOM_BORDER,
    PPpropLEFT_BORDER,
    PPpropRIGHT_BORDER,
    PPpropBETWEEN_BORDER,
    PPpropBAR_BORDER,

    PPpropFRAME,
    PPpropSHADING,

	/**
	 *  MUST be the last property that is actually emitted to 
	 *  RTF as DOCibkCOL results in a particule.
	 */
    PPpropBREAK_KIND,

    PPprop_COUNT,

    PPprop_IN_TABLE,			/* For read/write/tools only	*/

    PPprop_IGNORED,

	/**
	 *  Not real properties. But used for managing the ruler of a
	 *  document window.
	 */
    PPprop_COLUMNS,
    PPprop_TAB_KIND_BUTTON,

	/**
	 *  Not a real property. But used to support the obsolete \\box tag.
	 *
	 */
    PPprop_BOX_BORDER,

    PPprop_LISB,			/*  ignored.			*/
    PPprop_LISA,			/*  ignored.			*/

    PPprop_FULL_COUNT
    } ParagraphProperty;

# define PP_IS_ONE_PAGE( pp ) ( \
		    (pp)->ppKeepOnPage		|| \
		    (pp)->ppKeepWithNext	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docUpdParaProperties(
			struct PropertyMask *			pPpDoneMask,
			ParagraphProperties *			pp,
			const struct PropertyMask *		ppSetMask,
			const ParagraphProperties *		ppSet,
			const struct DocumentAttributeMap *	dam );

extern void docParaPropertyDifference(
			struct PropertyMask *			pDifMask,
			const ParagraphProperties *		pp1,
			const struct PropertyMask *		cmpMask,
			const ParagraphProperties *		pp2 );

extern void docCleanParagraphProperties(	ParagraphProperties *	pp );

extern void docInitParagraphProperties(		ParagraphProperties *	pp );

extern int docCopyParagraphProperties(
				ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom );

extern int docSetParaProperty(		ParagraphProperties *	pp,
					int			prop,
					int			value );

extern int docGetParaProperty(		const ParagraphProperties *	pp,
					int				prop );

#   endif	/*  DOC_PARA_PROP_H	*/
