#   ifndef	DOC_DRAW_PARA_H
#   define	DOC_DRAW_PARA_H

#   include	<docLayoutPosition.h>
#   include	<docSelect.h>

typedef struct ParagraphDrawingStrip
    {
    			/**
			 *  The number (in the formatted paragraph) of
			 *  the first line to draw.
			 */
    int			pdsLineFrom;

    			/**
			 *  The number (in the formatted paragraph) beyond
			 *  the last line to draw.
			 */
    int			pdsLineUpto;

			/**
			 *  Does the selection start at the top of the paragraph?
			 *  This determines whether the top border of the paragraph is
			 *  drawn.
			 */
    int			pdsAtParaTop;
			/**
			 *  Does the selection end at the bottom of the paragraph?
			 *  This determines whether the bottom border of the paragraph is
			 *  drawn.
			 */
    int			pdsAtParaBottom;

			/**
			 *  The kind of EXPLICIT break that 
			 *  finishes this strip. Other reasons than
			 *  an explicit break are represented by 
			 *  DOCibkNONE. (E.G. end of paragraph, page full,
			 *  end of selection.)
			 */
    int			pdsBreakKind;

    LayoutPosition	pdsShadeTop;
    LayoutPosition	pdsShadeBelow;

    DocumentSelection	pdsStripSelection;
    } ParagraphDrawingStrip;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
