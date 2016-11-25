/************************************************************************/
/*									*/
/*  Values and sub structures used in various document property		*/
/*  structs.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PROP_VAL_H
#   define	DOC_PROP_VAL_H

/************************************************************************/
/*									*/
/*  Breaks before sections. DOCibkNONE, DOCibkCOL and DOCibkPAGE are	*/
/*  also used for paragraphs.						*/
/*									*/
/************************************************************************/

typedef enum ItemBreak
    {
    DOCibkNONE= 0,
    DOCibkCOL,
    DOCibkPAGE,
    /*
     *  Only used for sections in documents with different odd
     *  and even pages.
     */
    DOCibkEVEN,
    DOCibkODD,

    DOCibk_COUNT
    } ItemBreak;

#   define DOCibkpara_COUNT DOCibkEVEN
#   define DOCibksect_COUNT DOCibk_COUNT

/************************************************************************/
/*									*/
/*  Horizontal alilignment of paragraphs and cells.			*/
/*									*/
/************************************************************************/

typedef enum TextHorizontalAlignment
    {
	/**
	 *  The alignment of the paragraph is the same as the paragraph 
	 *  direction. [MS-Word displays the paragraph direction in the
	 *  paragraph tool. I.E 'Left' in a left-to-right paragraph and 
	 *  'Right' in a right-to-left paragraph.
	 */
    DOCthaLEFT= 0,

	/**
	 *  The alignment of the paragraph is the opposite of the paragraph 
	 *  direction. [MS-Word displays the opposite paragraph direction in 
	 *  the paragraph tool. I.E 'Right' in a left-to-right paragraph and 
	 *  'Left' in a right-to-left paragraph.
	 */
    DOCthaRIGHT,

	/**
	 *  Text is centered in the paragraph frame
	 */
    DOCthaCENTERED,

	/**
	 *  Text is justified. I.E all lines get extra space to have 
	 *  a left- and a right hand side margin. The last (incomplete) 
	 *  line is left aligned for left-to-right paragraphs and it 
	 *  is right aligned for right-to-left paragraphs
	 */
    DOCthaJUSTIFIED,

    DOCtha_COUNT
    } TextHorizontalAlignment;

/************************************************************************/
/*									*/
/*  Vertical alilignment of cells and rows.				*/
/*									*/
/************************************************************************/

typedef enum TextVerticalAlignment
    {
    DOCtvaTOP= 0,
    DOCtvaCENTERED,
    DOCtvaBOTTOM,

    DOCtva_COUNT
    } TextVerticalAlignment;

typedef enum TextFlowDirection
    {
    TXflowTXLRTB= 0,	/*  left to right,  top to bottom (default).	*/
    TXflowTXTBRL,	/*  right to left,  top to bottom.		*/
    TXflowTXBTLR,	/*  left to right,  bottom to top.		*/
    TXflowTXLRTBV,	/*  left to right,  top to bottom, vertical.	*/
    TXflowTXTBRLV,	/*  top to bottom,  right to left, vertical.	*/

    TXflow_COUNT
    } TextFlowDirection;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameXReference
    {
    FXrefMARGIN= 0,
    FXrefPAGE,
    FXrefCOLUMN,
#   define	FXref__FRAME_COUNT	(FXrefCOLUMN+1)
    FXrefCHARACTER,
    FXrefIGNORE,

    FXref_COUNT
    } FrameXReference;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameYReference
    {
    FYrefMARGIN= 0,
    FYrefPAGE,
    FYrefPARA,
#   define	FYref__FRAME_COUNT	(FYrefPARA+1)
		    /**
		     *  Position relative to the line of the anchor.
		     *  Experimenting with Word 2003 shows that for 
		     *  shapes the deepest level is positioning relative
		     *  to the paragraph. (For shapes) In the user interface,
		     *  it is possible to select the line. If you select
		     *  positioning relative to the line, MS-Word calculates 
		     *  the vertical offset relative to the paragraph.
		     */
    FYrefLINE,
    FYrefIGNORE,

    FYref_COUNT
    } FrameYReference;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameXPosition
    {
    FXposXGIVEN= 0,
    FXposXL,
    FXposXC,
    FXposXR,
    FXposXI,
    FXposXO,

    FXpos_COUNT
    } FrameXPosition;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameYPosition
    {
    FYposYGIVEN= 0,
    FYposYT,
    FYposYC,
    FYposYB,
    FYposYIN,
    FYposYOUT,

    FYpos_COUNT
    } FrameYPosition;

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

#   endif	/*  DOC_PROP_VAL_H  */
