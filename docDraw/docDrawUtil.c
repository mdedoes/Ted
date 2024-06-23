/************************************************************************/
/*									*/
/*  Drawing functionality: Utility functions.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<layoutContext.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a DrawingContext						*/
/*									*/
/************************************************************************/

void docInitDrawingContext(	DrawingContext *	dc )
    {
    dc->dcDocument= (struct BufferDocument *)0;

    dc->dcDrawParticulesSeparately= 0;

    dc->dcCurrentTextAttributeSet= 0;
    textInitTextAttribute( &(dc->dcCurrentTextAttribute) );

    dc->dcCurrentColorSet= 0;
    utilInitRGB8Color( &(dc->dcCurrentColor) );

    dc->dcLayoutContext= (struct LayoutContext *)0;
    dc->dcBodySectNode= (struct BufferItem *)0;
    dc->dcTree= (struct DocumentTree *)0;

    dc->dcClipRect= (DocumentRectangle *)0;
    dc->dcSelection= (struct DocumentSelection *)0;
    dc->dcSelectionHeadPositionFlags= 0;
    dc->dcSelectionTailPositionFlags= 0;
    dc->dcSelectionIsTableRectangle= 0;

    dc->dcFirstPage= -1;
    dc->dcLastPage= -1;
    dc->dcDrawOtherTrees= 0;
    dc->dcPostponeHeadersFooters= 0;
    dc->dcDocHasPageHeaders= 0;
    dc->dcDocHasPageFooters= 0;

    dc->dcDrawTableGrid= 0;

    dc->dcSetColorRgb= (SET_COLOR_RGB)0;
    dc->dcSetFont= (SET_FONT)0;
    dc->dcDrawShape= (DRAW_SHAPE)0;
    dc->dcDrawInlineObject= (DRAW_INLINE_OBJECT)0;
    dc->dcStartField= (START_FIELD)0;
    dc->dcFinishField= (FINISH_FIELD)0;
    dc->dcDrawTab= (DRAW_TAB)0;
    dc->dcDrawFtnsep= (DRAW_FTNSEP)0;
    dc->dcDrawUnderline= (DRAW_UNDERLINE)0;
    dc->dcDrawStrikethrough= (DRAW_STRIKETHROUGH)0;
    dc->dcDrawTextRun= (DRAW_TEXT_RUN)0;

    dc->dcStartTextLine= (START_TEXT_LINE)0;
    dc->dcFinishTextLine= (FINISH_TEXT_LINE)0;
    dc->dcDrawOrnaments= (DRAW_ORNAMENTS)0;

    dc->dcFinishPage= (FINISH_PAGE)0;
    dc->dcStartPage= (START_PAGE)0;
    dc->dcStartTreeLayout= (START_TREE_LAYOUT)0;

    dc->dcStartNode= (START_NODE)0;
    dc->dcFinishNode= (FINISH_NODE)0;
    dc->dcStartLines= (START_LINES)0;
    dc->dcFinishLines= (FINISH_LINES)0;
    dc->dcStartTree= (START_TREE)0;
    dc->dcFinishTree= (FINISH_TREE)0;

    return;
    }

void docResetDrawingContextState(	DrawingContext *	dc )
    {
    dc->dcCurrentTextAttributeSet= 0;
    dc->dcCurrentColorSet= 0;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

void docDrawSetColorRgb(	DrawingContext *	dc,
				void *			through,
				const RGB8Color *	rgb8 )
    {
    if  ( ! dc->dcCurrentColorSet				||
	  dc->dcCurrentColor.rgb8Red != rgb8->rgb8Red		||
	  dc->dcCurrentColor.rgb8Green != rgb8->rgb8Green	||
	  dc->dcCurrentColor.rgb8Blue != rgb8->rgb8Blue		)
	{
	if  ( ! dc->dcSetColorRgb			||
	      (*dc->dcSetColorRgb)( dc, through, rgb8 )	)
	    { LDEB(1); return;	}

	dc->dcCurrentColor= *rgb8;
	dc->dcCurrentColorSet= 1;
	}

    return;
    }

void docDrawSetColorNumber(	DrawingContext *	dc,
				void *			through,
				int			colorNumber )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    RGB8Color			rgb8;

    docGetColorByNumber( &rgb8, lc->lcDocument, colorNumber );
    docDrawSetColorRgb( dc, through, &rgb8 );

    return;
    }

void docDrawSetFont(		DrawingContext *	dc,
				void *			through,
				int			textAttr,
				const TextAttribute *	newTa )
    {
    TextAttribute *	curTa= &(dc->dcCurrentTextAttribute);

    if  ( ! dc->dcCurrentTextAttributeSet				||
	  curTa->taFontNumber != newTa->taFontNumber			||
	  curTa->taFontSizeHalfPoints != newTa->taFontSizeHalfPoints	||
	  curTa->taFontIsBold != newTa->taFontIsBold			||
	  curTa->taFontIsSlanted != newTa->taFontIsSlanted		||
	  curTa->taSmallCaps != newTa->taSmallCaps			||
	  curTa->taSuperSub != newTa->taSuperSub			)
	{
	if  ( ! dc->dcSetFont					||
	      (*dc->dcSetFont)( dc, through, textAttr, newTa )	)
	    { LDEB(1); return;	}

	dc->dcCurrentTextAttributeSet= 1;
	*curTa= *newTa;
	}

    return;
    }

