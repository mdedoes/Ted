#   ifndef	DOC_DRAW_PARA_H
#   define	DOC_DRAW_PARA_H

#   include	<docLayoutPosition.h>

typedef struct ParagraphDrawingStrip
    {
    int			pdsLineFrom;
    int			pdsLineUpto;

    int			pdsAtParaTop;
    int			pdsAtParaBottom;

    int			pdsBreakKind;

    LayoutPosition	pdsShadeTop;
    LayoutPosition	pdsShadeBelow;
    } ParagraphDrawingStrip;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
