/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"layoutContext.h"
#   include	"docParagraphLayout.h"
#   include	"docStripLayoutJob.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docParaProperties.h>
#   include	<docSelect.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph after an editing action.		*/
/*									*/
/*  To avoid screen flashing, a minimalistic approach is used: As	*/
/*  little of the layout is redone, and an administration is kept of	*/
/*  what has been changed, in order to redraw a minimal screen		*/
/*  rectangle.								*/
/*									*/
/*  a)  Shift the offsets of the lines in the rest of the paragraph.	*/
/*	This means that we optimistically assume that the change in the	*/
/*	text of the paragraph only affected the current line, and that	*/
/*	the lines below are unchanged. This assumption is verified by	*/
/*	docLayoutParaLines() so out naive optimism does no harm.	*/
/*  b)  Invalidate the column with of the lines that are to be		*/
/*	reformatted anyway to force a reformat by docLayoutParaLines().	*/
/*	[stroffUpto is in terms of the new line and particule offsets.]	*/
/*	Below, fromLine can change value. This forced invalidation	*/
/*	refers to the original value however: The beginning of the	*/
/*	edited stretch of text.						*/
/*  c)  Remember the end of the reformatting range.			*/
/*  d)  With widow/orphan control enabled, and after a replacement, it	*/
/*	might be possible that the current paragraph becomes lower and	*/
/*	can be moved to the previous page. So start a little earlier to	*/
/*	make sure that all special cases are covered by the regular	*/
/*	formatter algorithm.						*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  Reserve space for the footnotes upto the beginning of this line	*/
/*	and subtract the height from the buttom of the frame.		*/
/*  5)  Pagination of footnotes, headers and footers and separators is	*/
/*	handled after we find that the height of the external item has	*/
/*	changed.							*/
/*  6)  Set the start position for the layout operation to the current	*/
/*	line. Try only to redo the layout of the current paragraph.	*/
/*									*/
/************************************************************************/

int docAdjustParaLayout(	BufferItem * 		paraNode,
				int			afterReplace,
				int			stroffFrom,
				int			stroffShift,
				int			stroffUpto,
				LayoutJob *		lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    int				rval= 0;
    struct BufferDocument *	bd= lc->lcDocument;
    int				fontSize;

    BlockFrame			bf;
    int				fromPart;
    int				line;

    ParagraphLayoutJob		plj;

    LayoutPosition		oldLpBelow;
    int				paraUpto;
    const struct BufferItem *	prevParaNode= (const struct BufferItem *)0;
    struct BufferItem *		cellNode= paraNode->biParent;

    int				fromPara;
    int				fromLine= 0;
    struct BufferItem *		fromParaNode;

    LayoutPosition		lpHere;

    docInitParagraphLayoutJob( &plj );
    docLayoutInitBlockFrame( &bf );

    plj.pljLayoutJob= lj;

    if  ( paraNode->biLevel != DOClevPARA )
	{ LLDEB(paraNode->biLevel,DOClevPARA); rval= -1; goto ready; }
    /*
    if  ( fromLine < 0 || fromLine >= paraNode->biParaLineCount )
	{ LLDEB(fromLine, paraNode->biParaLineCount); rval= -1; goto ready; }
    */

    /*  a,b  */
    {
    TextLine *	tlShift;

    tlShift= paraNode->biParaLines;
    for ( fromLine= 0; fromLine < paraNode->biParaLineCount;
						    tlShift++, fromLine++ )
	{
	if  ( tlShift->tlStroff+ tlShift->tlStrlen >= stroffFrom )
	    {
	    docInvalidateTextLine( tlShift );
	    break;
	    }
	}

    tlShift= paraNode->biParaLines+ fromLine;
    docInvalidateTextLine( tlShift );
    tlShift++;
    for ( line= fromLine+ 1;
          line < paraNode->biParaLineCount;
	  tlShift++, line++ )
	{
	tlShift->tlStroff += stroffShift;

	if  ( tlShift->tlStroff < stroffUpto )
	    { docInvalidateTextLine( tlShift );	}
	}
    }

    paraNode->biParaMajorityFontAscY0= 0;
    paraNode->biParaMajorityFontDescY1= 0;
    paraNode->biParaMajorityFontSize= 0;

    /*  c  */
    oldLpBelow= paraNode->biBelowPosition;
    paraUpto= paraNode->biNumberInParent+ 1;

    if  ( paraNode->biNumberInParent > 0 )
	{ prevParaNode= cellNode->biChildren[paraNode->biNumberInParent- 1]; }

    /*  d  */
    fromParaNode= paraNode;
    fromPara= fromParaNode->biNumberInParent;
    if  ( afterReplace					&&
	  paraNode->biParaProperties->ppWidowControl	&&
	  fromLine < 3					&&
	  prevParaNode					&&
	  paraNode->biNumberInParent > 0		)
	{
	fromLine= 0;
	fromPart= 0;

	while( prevParaNode->biNumberInParent > 0 )
	    {
	    prevParaNode= cellNode->biChildren[prevParaNode->biNumberInParent- 1];

	    fromPara--;
	    fromParaNode= cellNode->biChildren[fromPara];

	    if  ( ! prevParaNode->biParaProperties->ppKeepWithNext )
		{ break;	}
	    }

	lpHere= prevParaNode->biBelowPosition;
	}
    else{
	const TextLine *	tl;

	tl= paraNode->biParaLines+ fromLine;
	fromPart= tl->tlFirstParticule;
	if  ( fromPart == 0 )
	    {
	    if  ( paraNode->biNumberInParent == 0 || ! prevParaNode )
		{ lpHere= paraNode->biTopPosition;	}
	    else{ lpHere= prevParaNode->biBelowPosition;	}
	    }
	else{ lpHere= tl->tlTopPosition;		}
	}

    /*  2  */
    docLayoutBlockFrame( &bf, fromParaNode, lj, lpHere.lpPage, lpHere.lpColumn );

    /*  3  */
    if  ( fromParaNode->biTreeType == DOCinBODY )
	{
	TextLine *			tlHere;
	DocumentPosition		dpHere;

	tlHere= fromParaNode->biParaLines+ fromLine;
	dpHere.dpNode= fromParaNode;
	dpHere.dpStroff= tlHere->tlStroff;

	if  ( docCollectFootnotesFromColumn( &bf, &dpHere, fromPart, bd,
					    lpHere.lpPage, lpHere.lpColumn ) )
	    { LDEB(lpHere.lpPage); rval= -1; goto ready;	}
	}

    /*  4  */
    docLayoutAdjustFrame( &bf, fromParaNode );

    docParagraphFrameTwips( &(plj.pljPos.plpParagraphFrame),
						    &bf, fromParaNode );

    if  ( docLayoutParagraphLineExtents( &fontSize, lc, fromParaNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docLayoutCalculateParaTopInset( bd, paraNode );
    docLayoutCalculateParaBottomInset( bd, paraNode );

    if  ( lj->ljStartScreenParagraph					&&
	  (*lj->ljStartScreenParagraph)( fromParaNode,
			&(plj.pljPos.plpParagraphFrame), lc )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  6  */
    docBeginParagraphLayoutProgress( &plj, lj,
			fromParaNode->biNumberInParent, fromLine, fromPart,
			paraUpto, &lpHere );

    docFindStripLayoutOrigin( &plj, &lpHere, cellNode );

    if  ( docLayoutStackedStrip( cellNode, &bf, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    lpHere= plj.pljPos.plpPos;

    if  ( paraNode->biParaLineCount < 1 )
	{
	LDEB(paraNode->biParaLineCount); docListNode(bd,0,paraNode,0);
	rval= -1; goto ready;
	}

    if  ( ! DOC_SAME_POSITION( &(paraNode->biBelowPosition), &lpHere ) )
	{
	LLDEB(paraNode->biBelowPosition.lpPage,lpHere.lpPage);
	LDEB(paraNode->biBelowPosition.lpPageYTwips);
	LDEB(lpHere.lpPageYTwips);

	paraNode->biBelowPosition= lpHere;
	}

    if  ( ! DOC_SAME_POSITION( &(paraNode->biBelowPosition), &oldLpBelow ) )
	{
	/*  5  */
	if  ( docAdjustParentLayout( &lpHere, paraNode, &bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docLayoutCleanBlockFrame( &bf );
    docCleanParagraphLayoutJob( &plj );

    return rval;
    }
