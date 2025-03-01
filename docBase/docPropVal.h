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
