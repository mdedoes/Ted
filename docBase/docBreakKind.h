/************************************************************************/
/*									*/
/*  Breaks before sections. DOCibkNONE, DOCibkCOL and DOCibkPAGE are	*/
/*  also used for paragraphs.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BREAK_KIND_H
#   define	DOC_BREAK_KIND_H

typedef enum BreakKind
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
    } BreakKind;

#   define DOCibkpara_COUNT DOCibkEVEN
#   define DOCibksect_COUNT DOCibk_COUNT

#   endif	/*  DOC_BREAK_KIND_H  */
