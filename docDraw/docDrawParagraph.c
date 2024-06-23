#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	"docDrawPara.h"
#   include	<docSelectLayout.h>
#   include	<docLayout.h>
#   include	<layoutContext.h>
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>
#   include	<docParagraphLayout.h>
#   include	<docTextParticule.h>
#   include	<docRowProperties.h>
#   include	<docStripFrame.h>
#   include	<docParaProperties.h>
#   include	<docBlockFrame.h>
#   include	<docSelect.h>
#   include	<docScanner.h>
#   include	<docBlockOrnaments.h>
#   include	<docBreakKind.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Paragraph drawing machinery.					*/
/*									*/
/************************************************************************/

static int docDrawTextLines(
			void *				through,
			const ParagraphDrawingStrip *	pds,
			struct BufferItem *		paraNode,
			const ParagraphFrame *		pf,
			DrawingContext *		dc,
			const BlockOrigin *		bo );

/************************************************************************/
/*									*/
/*  Draw paragraph borders and shading.					*/
/*									*/
/************************************************************************/

static int docDrawParaOrnaments( void *				through,
				const ParagraphDrawingStrip *	pds,
				const struct BufferItem * const	paraNode,
				const ParagraphFrame *		pf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *		lc= dc->dcLayoutContext;
    const struct BufferDocument *	bd= lc->lcDocument;

    LayoutPosition			lpTop;
    LayoutPosition			lpBelow;

    BlockOrnaments			ornaments;
    DocumentRectangle			drPara;
    DocumentRectangle			drOutside;
    DocumentRectangle			drInside;

    int					paraFirstIndentTwips;

    docInitBlockOrnaments( &ornaments );

    drPara= pf->pfParaContentRect;
    paraFirstIndentTwips= paraNode->biParaProperties->ppFirstIndentTwips;
    if  ( paraFirstIndentTwips < 0 )
	{ drPara.drX0 += paraFirstIndentTwips;	}

    docShiftPosition( &lpTop, bo, &(pds->pdsShadeTop) );
    docShiftPosition( &lpBelow, bo, &(pds->pdsShadeBelow) );

    drPara.drY0= lpTop.lpPageYTwips;
    drPara.drY1= lpBelow.lpPageYTwips;

    if  ( paraNode->biParaProperties->ppTableNesting > 0 )
	{
	const BufferItem *	rowNode;

	rowNode= docGetRowNode( (BufferItem *)paraNode );
	if  ( ! rowNode )
	    { XDEB(rowNode);	}
	else{
	    const RowProperties * rp= rowNode->biRowProperties;

	    int	x0= pf->pfParaContentRect.drX0- rp->rpHalfGapWidthTwips;
	    int	x1= pf->pfParaContentRect.drX1+ rp->rpHalfGapWidthTwips;

	    if  ( drPara.drX0 < x0 )
		{ drPara.drX0=  x0;	}
	    if  ( drPara.drX1 > x1 )
		{ drPara.drX1=  x1;	}
	    }
	}

    docGetParaOrnaments( &ornaments, &drOutside, &drInside, &drPara,
			bd, paraNode, pds->pdsAtParaTop, pds->pdsAtParaBottom );

    if  ( ! utilPropMaskIsEmpty( &(ornaments.boPropMask) ) )
	{
	if  ( (*dc->dcDrawOrnaments)( &ornaments, lpTop.lpPage,
						&drOutside, &drInside,
						through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Count the number of lines in this paragraph on the current page.	*/
/*									*/
/************************************************************************/

static int docDelimitParagraphDrawingStrip(
			ParagraphDrawingStrip *		pds,
			const struct BufferItem *	paraNode,
			int				countAfter,
			const LayoutPosition *		lpShadeTop,
			int				lineFrom,
			const LayoutPosition *		lpThisFrame,
			const BlockOrigin *		bo )
    {
    int			line= lineFrom;
    int			atTop= lineFrom == 0;
    int			atBottom= 0;

    LayoutPosition	lpBelow= *lpShadeTop;
    int			lineUpto= paraNode->biParaLineCount;
    int			breakKind= DOCibkNONE;

    line= docParaFindLastLineInFrame( paraNode, lineFrom, lpThisFrame, bo );
    if  ( line < lineFrom )
	{
	/*
	LLLLDEB(lpThisFrame->lpPage,lpThisFrame->lpColumn,line,lineFrom);
	docListNode(bd,0,paraNode,0);
	*/
	pds->pdsLineUpto= line;
	return 1;
	}

    if  ( line < paraNode->biParaLineCount )
	{
	const TextLine *	tl= paraNode->biParaLines+ line;
	const TextParticule *	tpLast;

	tpLast= paraNode->biParaParticules+
				tl->tlFirstParticule+ tl->tlParticuleCount -1;

	if  ( tpLast->tpKind == TPkindPAGEBREAK )
	    { breakKind= DOCibkPAGE;	}
	if  ( tpLast->tpKind == TPkindCOLUMNBREAK )
	    { breakKind= DOCibkCOL;	}

	docTextLineGetShiftedNextLineTop( &lpBelow, bo, tl );

	if  ( line+ 1 == paraNode->biParaLineCount )
	    {
	    if  ( breakKind != DOCibkNONE )
		{
		LayoutPosition	lp;

		docShiftPosition( &lp, bo, &(paraNode->biBelowPosition) );

		if  ( ! countAfter )
		    {
		    lp.lpPageYTwips -=
				paraNode->biParaProperties->ppSpaceAfterTwips;
		    }

		docLayoutPushBottomDown( &lpBelow, &lp );
		}

	    atBottom= 1;
	    }

	lineUpto= line+ 1;
	}

    docTextLineSelection( &(pds->pdsStripSelection), paraNode,
				paraNode->biParaLines+ lineFrom,
				paraNode->biParaLines+ lineUpto- 1 );

    pds->pdsLineFrom= lineFrom;
    pds->pdsLineUpto= lineUpto;
    pds->pdsAtParaTop= atTop;
    pds->pdsAtParaBottom= atBottom;

    pds->pdsBreakKind= breakKind;

    pds->pdsShadeTop= *lpShadeTop;
    pds->pdsShadeBelow= lpBelow;

    return 0;
    }

/************************************************************************/

int docDrawParagraphStrip(		void *			through,
					ParagraphDrawingStrip *	pds,
					struct BufferItem *	paraNode,
					int			countAfter,
					const LayoutPosition *	lpShadeTop,
					int			lineFrom,
					const ParagraphFrame *	pf,
					DrawingContext *	dc,
					const LayoutPosition *	lpThisFrame,
					const BlockOrigin *	bo )
    {
    int		ret;

    ret= docDelimitParagraphDrawingStrip( pds, paraNode, countAfter,
				    lpShadeTop, lineFrom, lpThisFrame, bo );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 ) /* nothing to draw */
	{ return 0;	}

    if  ( pds->pdsLineUpto > lineFrom && dc->dcDrawOrnaments )
	{
	if  ( docDrawParaOrnaments( through, pds, paraNode, pf, dc, bo ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docDrawTextLines( through, pds, paraNode, pf, dc, bo ) < 0 )
	{ LDEB(lpThisFrame->lpPage); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a node in the node hierarchy of the document tree.		*/
/*									*/
/*  1)  The last page is finished by the caller.			*/
/*									*/
/************************************************************************/

static const char DRAW_LINE_BREAK_FLOW[]= "paragraph flow";
static const char DRAW_LINE_BREAK_PAGE[]= "page break";
static const char DRAW_LINE_BREAK_COLUMN[]=  "column break";

int docDrawParaNode(		LayoutPosition *		lpBelow,
				struct BufferItem *		paraNode,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    int				rval= 0;
    ParagraphFrame		pf;

    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;

    BlockFrame			bf;
    LayoutPosition		lpTop;
    LayoutPosition		lpShadeTop;

    int				line= 0;

    if  ( dc->dcStartNode && (*dc->dcStartNode)( through, dc,
					bo->boOverrideFrame, 0, paraNode ) )
	{ LDEB(paraNode->biLevel); return -1; }

    docLayoutInitBlockFrame( &bf );
    docParaBlockFrameTwips( &bf, paraNode, dc->dcBodySectNode, bd,
					    &(paraNode->biTopPosition) );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    docShiftPosition( &lpTop, bo, &(paraNode->biTopPosition) );
    lpShadeTop= lpTop;

    if  ( ! lpShadeTop.lpAtTopOfColumn )
	{
	lpShadeTop.lpPageYTwips +=
			    paraNode->biParaProperties->ppSpaceBeforeTwips;
	}

    while( line < paraNode->biParaLineCount )
	{
	const int		countAfter= 0;

	ParagraphDrawingStrip	pds;

	if  ( docDrawParagraphStrip( through, &pds, paraNode, countAfter,
			    &lpShadeTop, line, &pf, dc, &lpTop, bo ) )
	    {
	    LLDEB(docNumberOfParagraph(paraNode),line);
	    LLDEB(lpTop.lpPage,lpTop.lpColumn);
	    rval= -1; goto ready;
	    }

	line= pds.pdsLineUpto;
	if  ( lpBelow )
	    { docLayoutPushBottomDown( lpBelow, &(pds.pdsShadeBelow) );	}

	/*  1  */
if  ( paraNode->biTreeType == DOCinSHPTXT )
    { break;	}

	if  ( docCompareLayoutPositionFrames( &(paraNode->biBelowPosition),
								&lpTop ) > 0 )
	    {
	    int			ret;
	    const char *	breakWhy;

	    switch( pds.pdsBreakKind )
		{
		default:
		    LDEB(pds.pdsBreakKind);
		    /*fallthrough*/
		case DOCibkNONE:
		    breakWhy= DRAW_LINE_BREAK_FLOW;
		    break;
		case DOCibkCOL:
		    breakWhy= DRAW_LINE_BREAK_COLUMN;
		    break;
		case DOCibkPAGE:
		    breakWhy= DRAW_LINE_BREAK_PAGE;
		    break;
		}

	    ret= docDrawToNextColumn( paraNode, paraNode,
				through, &lpTop, &bf, dc, breakWhy );
	    if  ( ret < 0 )
		{
		SLDEB(docLevelStr(paraNode->biLevel),ret);
		rval= -1; goto ready;
		}
	    if  ( ret > 0 )
		{ break;						}

	    docParagraphFrameTwips( &pf, &bf, paraNode );
	    lpShadeTop= lpTop;
	    }
	}

    if  ( lpBelow && line >= paraNode->biParaLineCount )
	{
	LayoutPosition	lp;

	docShiftPosition( &lp, bo, &(paraNode->biBelowPosition) );

	docLayoutPushBottomDown( lpBelow, &lp );
	}

    if  ( dc->dcFinishNode && (*dc->dcFinishNode)( through, dc, bo->boOverrideFrame, 0, paraNode ) )
	{ LDEB(paraNode->biLevel); return -1; }

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw a series of text lines in a paragraph.				*/
/*									*/
/*  1)  For all lines that apply: Those in paraNode in the current	*/
/*	page/column combination.					*/
/*  2)  If the line is on a subsequent page.. stop.			*/
/*  3)  Is line is on a page/column before the pages/column to draw?	*/
/*  4)  Is the line in the rectangle to redraw?				*/
/*	As this is inside a column, a line that is outside the clipping	*/
/*	rectangle vertically is out of range inside this column.	*/
/*  5)  Printing a page range.. and not in range?			*/
/*  6)  Drawing a selection? E.G. To highlight it?			*/
/*									*/
/************************************************************************/

static int docDrawTextLines(
			void *				through,
			const ParagraphDrawingStrip *	pds,
			struct BufferItem *		paraNode,
			const ParagraphFrame *		pf,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				pastSelectionEnd= 0;

    int				line= pds->pdsLineFrom;
    TextLine *			tl= paraNode->biParaLines+ line;

    if  ( pds->pdsShadeTop.lpPage != pds->pdsShadeBelow.lpPage		||
	  pds->pdsShadeTop.lpColumn != pds->pdsShadeBelow.lpColumn	)
	{
	LLDEB(pds->pdsShadeTop.lpPage,pds->pdsShadeBelow.lpPage);
	LLDEB(pds->pdsShadeTop.lpColumn,pds->pdsShadeBelow.lpColumn);
	}

    if  ( dc->dcStartLines && (*dc->dcStartLines)( through, dc, paraNode, line, &(pds->pdsStripSelection) ) )
	{ LDEB(1); return -1;	}

    /*  1  */
    while( line < paraNode->biParaLineCount )
	{
	int			beforeSelectionBegin= 0;

	LayoutPosition		lpTop;
	LayoutPosition		lpNextLine;
	DocumentRectangle	drLineFramePixels;
	int			res;

	docTextLineGetShiftedTop( &lpTop, bo, tl );
	docTextLineGetShiftedNextLineTop( &lpNextLine, bo, tl );

	docLineFrameRectanglePixels( &drLineFramePixels, &lpTop, &lpNextLine,
							tl, bo, pf, lc );

	/*  2  */
	if  ( docCompareLayoutPositionFrames( &lpTop,
						&(pds->pdsShadeTop) ) > 0 )
	    { break;	}
	/*  3  */
	if  ( docCompareLayoutPositionFrames( &lpTop,
						&(pds->pdsShadeTop) ) < 0 )
	    { beforeSelectionBegin= 1;	}

	/*  4  */
	if  ( dc->dcClipRect )
	    {
	    if  ( drLineFramePixels.drY0 > dc->dcClipRect->drY1 )
		{ pastSelectionEnd= 1;	}
	    if  ( drLineFramePixels.drY1 < dc->dcClipRect->drY0 )
		{ beforeSelectionBegin= 1;	}
	    }

	/*  5  */
	if  ( dc->dcFirstPage >= 0			&&
	      pds->pdsShadeTop.lpPage < dc->dcFirstPage	)
	    { beforeSelectionBegin= 1;	}

	/*  6  */
	if  ( dc->dcSelection						&&
	      ! beforeSelectionBegin					&&
	      ! pastSelectionEnd					&&
	      ( paraNode == dc->dcSelection->dsHead.dpNode	||
	        paraNode == dc->dcSelection->dsTail.dpNode	)	)
	    {
	    const DocumentSelection *	ds= dc->dcSelection;

	    DocumentSelection		dsLine;

	    docLineSelection( &dsLine, paraNode, line );

	    if  ( paraNode == ds->dsTail.dpNode				&&
		  docComparePositions( &(dsLine.dsHead), &(ds->dsTail) ) > 0 )
		{ pastSelectionEnd= 1;	}

	    if  ( paraNode == ds->dsHead.dpNode				&&
		  docComparePositions( &(dsLine.dsTail), &(ds->dsHead) ) < 0 )
		{ beforeSelectionBegin= 1;	}
	    }

	/****************************************************************/
	/*  Do not clip here: The find code might want to look for	*/
	/*  lines in the immediate proximity of a mouse click.		*/
	/****************************************************************/
	if  ( beforeSelectionBegin	||
	      pastSelectionEnd		||
	      ! dc->dcStartTextLine	)
	    { res= SCANadviceSKIP;		}
	else{
	    res= docDrawTextLine( paraNode, tl, pf, &drLineFramePixels,
							    through, dc, bo );
	    }

	if  ( res < 0 )
	    {
	    LLDEB(line,res);
	    docListNode(lc->lcDocument,0,paraNode,0);
	    return -1;
	    }
	if  ( res == SCANadviceSTOP )
	    {
	    if  ( dc->dcFinishLines && (*dc->dcFinishLines)( through, dc, paraNode, line, &(pds->pdsStripSelection) ) )
		{ LDEB(line); return -1;	}

	    return res;
	    }

	line++; tl++;
	}

    if  ( dc->dcFinishLines && (*dc->dcFinishLines)( through, dc, paraNode, line- 1, &(pds->pdsStripSelection) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

