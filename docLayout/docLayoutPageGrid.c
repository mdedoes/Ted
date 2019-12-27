/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docBuf.h>
#   include	"docLayout.h"
#   include	"docStripLayoutJob.h"
#   include	"layoutContext.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docBreakKind.h>
#   include	<docFrameProperties.h>
#   include	<docSectProperties.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

void docLayoutBlockFrame(	BlockFrame *			bf,
				struct BufferItem *		node,
				const LayoutJob *		lj,
				int				page,
				int				column )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;

    docBlockFrameTwips( bf, node, bd, page, column );

    if  ( page == lj->ljBalancePage )
	{
	bf->bfContentRect.drY1= lj->ljBalanceY1;
	bf->bfFlowRect.drY1= lj->ljBalanceY1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text in a subsequent column, or on a	*/
/*  subsequent page.							*/
/*									*/
/*  1)  Continuous sections wrap to the same position as where they	*/
/*	started on the page.						*/
/*									*/
/************************************************************************/

static void docLayoutSetColumnTop(	LayoutPosition *	lpTop,
					const BlockFrame *	bf,
					struct BufferItem *	bodySectNode )
    {
    lpTop->lpPageYTwips= bf->bfContentRect.drY0;
    lpTop->lpAtTopOfColumn= 1;

    /*  1  */
    if  ( bodySectNode->biSectBreakKind == DOCibkNONE		&&
	  lpTop->lpPage == bodySectNode->biTopPosition.lpPage	&&
	  bodySectNode->biSectColumnCount > 1			&&
	  lpTop->lpColumn > 0					)
	{
	lpTop->lpPageYTwips= bodySectNode->biTopPosition.lpPageYTwips;
	/* NO: we are at the top for this section lpTop->lpAtTopOfColumn= 0; */
	}

    }

void docLayoutColumnTop(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				struct BufferItem *	bodySectNode,
				const LayoutJob *	lj )
    {
    if  ( bodySectNode->biTreeType != DOCinBODY )
	{ SDEB(docTreeTypeStr(bodySectNode->biTreeType));	}

    /* Can actually happen while formatting endnotes
    if  ( lj->ljBodySectNode != bodySectNode )
	{ XXDEB(lj->ljBodySectNode,bodySectNode);	}
    */

    docLayoutBlockFrame( bf, bodySectNode, lj, lpTop->lpPage, lpTop->lpColumn );

    docLayoutSetColumnTop( lpTop, bf, bodySectNode );

    return;
    }

/************************************************************************/
/*									*/
/*  Skip to the next column. Without newspaper style columns, that is	*/
/*  the next page.							*/
/*									*/
/************************************************************************/

void docLayoutToNextColumn(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				struct BufferItem *	node,
				const LayoutJob *	lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    const struct BufferDocument * bd= lc->lcDocument;
    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;
    struct BufferItem *		bodySectNode;

    node= docGetSectNode( node );
    if  ( ! node )
	{ XDEB(node); return;	}

    switch( node->biTreeType )
	{
	SelectionScope *	ss;
	int			extTo;

	case DOCinBODY:
	    bodySectNode= node;
	    break;

	case DOCinAFTNSEP:
	    extTo= bodyNode->biChildCount- 1;
	    bodySectNode= bodyNode->biChildren[extTo];
	    break;

	default:
	    ss= &(node->biSectSelectionScope);
	    extTo= ss->ssOwnerSectNr;
	    if  ( extTo < 0 )
		{
		SLDEB(docTreeTypeStr(node->biTreeType),extTo);
		return;
		}
	    bodySectNode= bodyNode->biChildren[extTo];
	    break;
	}

    lpTop->lpColumn++;
    if  ( lpTop->lpColumn >= bodySectNode->biSectColumnCount )
	{ lpTop->lpPage++; lpTop->lpColumn= 0; }

    if  ( lj->ljBodySectNode != bodySectNode		&&
	  node->biTreeType != DOCinENDNOTE		)
	{
	XXDEB(lj->ljBodySectNode,bodySectNode);
	SDEB(docTreeTypeStr(node->biTreeType));
	}

    docLayoutColumnTop( lpTop, bf, bodySectNode, lj );

    return;
    }

void docLayoutToNextPage(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				struct BufferItem *	node,
				const LayoutJob *	lj )
    {
    docLayoutToNextColumn( lpTop, bf, node, lj );

    while( lpTop->lpColumn > 0 )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	}
    }

int docLayoutToFirstColumn(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				struct BufferItem *	node,
				const LayoutJob *	lj )
    {
    int	changedFrame= 0;

    while( lpTop->lpColumn > 0 )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	changedFrame= 1;
	}

    return changedFrame;
    }

int docLayoutToEvenPage(	struct LayoutPosition *		lpTop,
				struct BlockFrame *		bf,
				struct BufferItem *		node,
				const LayoutJob *		lj )
    {
    int	changedFrame= 0;

    while( lpTop->lpColumn > 0 )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	changedFrame= 1;
	}

    while( ! ( lpTop->lpPage % 2 ) )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	changedFrame= 1;
	}

    return changedFrame;
    }

int docLayoutToOddPage(		struct LayoutPosition *		lpTop,
				struct BlockFrame *		bf,
				struct BufferItem *		node,
				const LayoutJob *		lj )
    {
    int	changedFrame= 0;

    while( lpTop->lpColumn > 0 )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	changedFrame= 1;
	}

    while( lpTop->lpPage % 2 )
	{
	docLayoutToNextColumn( lpTop, bf, node, lj );
	changedFrame= 1;
	}

    return changedFrame;
    }

/************************************************************************/
/*									*/
/*  Adjust geometry to position paragraphs in a text frame.		*/
/*									*/
/************************************************************************/

void docLayoutFinishFrame(	const FrameProperties *		fp,
				BlockFrame *			bfTextFrame,
				const BlockFrame *		bfFlow,
				const LayoutJob *		lj,
				const ParagraphLayoutPosition *	plpFlow,
				struct BufferItem *		cellNode,
				int				paraFrom,
				int				paraUpto )
    {
    struct BufferItem *		paraNode0= cellNode->biChildren[paraFrom];
    struct BufferItem *		paraNode1= cellNode->biChildren[paraUpto- 1];

    int				y0= bfTextFrame->bfContentRect.drY0;
    int				y1= paraNode1->biBelowPosition.lpPageYTwips;

    int				frameHeight;
    BlockFrame			bfRedo;

    if  ( ! DOCisFRAME( fp ) )
	{ LDEB(1); return;	}

    if  ( fp->fpHighTwips < 0 )
	{
	y1= y0- fp->fpHighTwips;

	/*
	if  ( paraNode1->biBelowPosition.lpPageYTwips > y1 )
	    { LLDEB(paraNode1->biBelowPosition.lpPageYTwips,y1);	}
	*/
	}

    if  ( fp->fpHighTwips > 0 )
	{ y1= y0+ fp->fpHighTwips;	}

    if  ( paraNode1->biBelowPosition.lpPageYTwips < y1 )
	{ paraNode1->biBelowPosition.lpPageYTwips=  y1;	}

    frameHeight= y1- y0;
    docLayoutInitBlockFrame( &bfRedo );
    docLayoutSetTextFrame( &bfRedo, &(plpFlow->plpPos),
						bfFlow, fp, frameHeight );

    if  ( bfRedo.bfContentRect.drY0		!=
	  bfTextFrame->bfContentRect.drY0	)
	{
	int		yBelow;
	LayoutPosition	lpTop= paraNode0->biTopPosition;

	lpTop.lpPageYTwips= bfRedo.bfContentRect.drY0;

	yBelow= bfRedo.bfContentRect.drY0+ frameHeight;

	docRedoParaStripLayout( lj, bfTextFrame, &lpTop,
						cellNode, paraFrom, paraUpto );

	paraNode1->biBelowPosition.lpPageYTwips= yBelow;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text on a subsequent page.			*/
/*									*/
/*  1)  Continuous section wrap to the same position as where they	*/
/*	started on the page.						*/
/*									*/
/************************************************************************/

void docLayoutSectColumnTop(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				struct BufferItem *	bodySectNode,
				struct BufferDocument *	bd )
    {
    if  ( bodySectNode->biTreeType != DOCinBODY )
	{ SDEB(docTreeTypeStr(bodySectNode->biTreeType));	}

    docSectionBlockFrameTwips( bf, bodySectNode, bodySectNode, bd,
					    lpTop->lpPage, lpTop->lpColumn );

    docLayoutSetColumnTop( lpTop, bf, bodySectNode );

    return;
    }

