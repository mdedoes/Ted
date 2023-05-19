/************************************************************************/
/*									*/
/*  Geometry administration for 'DocumentTrees' I.E. headers/footers	*/
/*  and foonotes/endnotes.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docBuf.h>
#   include	<docTextLine.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docSelectLayout.h"
#   include	<docSelect.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Go to the top/bottom of a certain page.				*/
/*									*/
/*  1)  Sections that start on an odd/even page may skip a page.	*/
/*									*/
/************************************************************************/

static int docParaGetFirstInColumn(
			DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			struct BufferItem *		paraNode,
			int				lineUpto,
			int				page,
			int				column )
    {
    int			i;
    const TextLine *	tl= paraNode->biParaLines;

    for ( i= 0; i < lineUpto; tl++, i++ )
	{
	if  ( docCompareLayoutPositionToFrame( &(tl->tlTopPosition),
							page, column ) >= 0 )
	    { break;	}
	}

    if  ( paraNode->biParaLineCount > 0 && i >= paraNode->biParaLineCount )
	{
	tl= paraNode->biParaLines+ paraNode->biParaLineCount- 1;

	if  ( tl->tlFlags & TLflagBLOCKBREAK )
	    {
	    struct BufferItem *	nextBi= docNextParagraph( paraNode );

	    if  ( nextBi && nextBi->biParaLineCount > 0 )
		{ i= 0; paraNode= nextBi; tl= paraNode->biParaLines;	}
	    }
	}

    if  ( i >= paraNode->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{
	SDEB(docTreeTypeStr(paraNode->biTreeType));
	LLDEB(page,column);
	LDEB(paraNode->biBelowPosition.lpPage);
	LDEB(paraNode->biBelowPosition.lpColumn);
	/*docListNode(bd,0,paraNode);*/
	LDEB(docNumberOfParagraph(paraNode));
	LLDEB(i,paraNode->biParaLineCount);
	return -1;
	}

    dp->dpNode= paraNode;
    dp->dpStroff= tl->tlStroff;

    *pLineTop= tl- paraNode->biParaLines;
    *pPartTop= tl->tlFirstParticule;

    return 0;
    }

int docGetFirstInColumnForNode(
			DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			struct BufferItem *		node,
			int				page,
			int				column )
    {
    int			i;

    while( node && node->biLevel != DOClevPARA )
	{
	/*  1  */
	if  ( node->biTopPosition.lpPage > page )
	    {
	    /* LLDEB(node->biTopPosition.lpPage,page); */
	    return 1;
	    }
	if  ( node->biBelowPosition.lpPage < page )
	    {
	    /* LLDEB(node->biTopPosition.lpPage,page); */
	    return 1;
	    }

	for ( i= 0; i < node->biChildCount; i++ )
	    {
	    if  ( docCompareLayoutPositionToFrame(
			    &(node->biChildren[i]->biBelowPosition),
			    page, column ) >= 0 )
		{ break;	}
	    }

	if  ( i >= node->biChildCount )
	    {
	    /*  NO! is possible e.g. when endnotes continue beyond the 
	        last page with body content.
	    for ( i= 0; i < node->biChildCount; i++ )
		{ LLDEB(i,node->biChildren[i]->biBelowPosition.lpPage); }
	    LLLDEB(node->biTopPosition.lpPage,node->biBelowPosition.lpPage,page);
	    return -1;
	    */
	    return 1;
	    }

	node= node->biChildren[i];
	}

    if  ( ! node || node->biLevel != DOClevPARA )
	{ XDEB(node); return -1;	}

    return docParaGetFirstInColumn( dp, pLineTop, pPartTop,
				    node, node->biParaLineCount, page, column );
    }

int docGetTopOfColumn(	DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			struct BufferDocument *		bd,
			int				page,
			int				column )
    {
    return docGetFirstInColumnForNode( dp, pLineTop, pPartTop,
					    bd->bdBody.dtRoot, page, column );
    }

int docGetLastInColumnForNode(	DocumentPosition *		dp,
				int *				pLineBot,
				int *				pPartBot,
				struct BufferItem *		nodeIn,
				int				page,
				int				column )
    {
    int			i;
    const TextLine *	tl;
    struct BufferItem *	node= nodeIn;

    while( node && node->biLevel != DOClevPARA )
	{
	if  ( node->biTopPosition.lpPage > page )
	    { /*LLDEB(node->biBelowPosition.lpPage,page);*/ return 1;	}
	if  ( node->biBelowPosition.lpPage < page )
	    { /*LLDEB(node->biBelowPosition.lpPage,page);*/ return 1;	}

	for ( i= node->biChildCount- 1; i >= 0; i-- )
	    {
	    if  ( docCompareLayoutPositionToFrame(
			    &(node->biChildren[i]->biTopPosition),
			    page, column ) <= 0 )
		{ break;	}
	    }

	if  ( i < 0 )
	    {
	    /*
	    for ( i= 0; i < node->biChildCount; i++ )
		{ LLDEB(i,node->biChildren[i]->biTopPosition.lpPage); }
	    LLLDEB(node->biTopPosition.lpPage,node->biBelowPosition.lpPage,page);
	    */
	    return 1;
	    }

	node= node->biChildren[i];
	}

    if  ( ! node || node->biLevel != DOClevPARA )
	{ XDEB(node); return -1;	}

    tl= node->biParaLines+ node->biParaLineCount- 1;
    for ( i= node->biParaLineCount- 1; i >= 0; tl--, i-- )
	{
	if  ( docCompareLayoutPositionToFrame( &(tl->tlTopPosition),
							page, column ) <= 0 )
	    { break;	}
	}

    if  ( i < 0					||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{
	SSDEB(docTreeTypeStr(node->biTreeType),docLevelStr(node->biLevel));
	LLDEB(page,column);
	LDEB(node->biBelowPosition.lpPage);
	LDEB(node->biBelowPosition.lpColumn);
	/*docListNode(bd,0,nodeIn,0);*/
	LDEB(docNumberOfParagraph(node));
	LLDEB(i,node->biParaLineCount);
	return -1;
	}

    dp->dpNode= node;
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;

    *pLineBot= tl- node->biParaLines;
    *pPartBot= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return 0;
    }

int docGetBottomOfColumn(
			DocumentPosition *		dp,
			int *				pPartBot,
			struct BufferDocument *		bd,
			int				page,
			int				column )
    {
    int		lineBot;

    return docGetLastInColumnForNode( dp, &lineBot, pPartBot,
					    bd->bdBody.dtRoot, page, column );
    }

/************************************************************************/

int docParaFindLastLineInFrame(
			const struct BufferItem *	paraNode,
			int				lineFrom,
			const LayoutPosition *		lpThisFrame,
			const BlockOrigin *		bo )
    {
    int			l= lineFrom;
    int			r= paraNode->biParaLineCount;
    int			m= ( l+ r )/ 2;

    LayoutPosition	lp;

    if  ( paraNode->biParaLineCount == 0 )
	{ return paraNode->biParaLineCount;	}

    while( l < m )
	{
	docTextLineGetShiftedTop( &lp, bo, &(paraNode->biParaLines[m]) );

	if  ( docCompareLayoutPositionFrames( &lp, lpThisFrame ) > 0 )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    docTextLineGetShiftedTop( &lp, bo, &(paraNode->biParaLines[m]) );
    if  ( docCompareLayoutPositionFrames( &lp, lpThisFrame ) > 0 )
	{ return m- 1;	}
    else{ return m;	}
    }

int docParaFindFirstLineInFrame(
			const struct BufferItem *	paraNode,
			int				lineFrom,
			const LayoutPosition *		lpThisFrame,
			const BlockOrigin *		bo )
    {
    int			l= lineFrom;
    int			r= paraNode->biParaLineCount;
    int			m= ( l+ r )/ 2;

    LayoutPosition	lp;

    if  ( paraNode->biParaLineCount == 0 )
	{ return paraNode->biParaLineCount;	}

    while( l < m )
	{
	docTextLineGetShiftedTop( &lp, bo, &(paraNode->biParaLines[m]) );

	if  ( docCompareLayoutPositionFrames( lpThisFrame, &lp ) > 0 )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    docTextLineGetShiftedTop( &lp, bo, &(paraNode->biParaLines[m]) );
    if  ( docCompareLayoutPositionFrames( lpThisFrame, &lp ) > 0 )
	{ return m+ 1;	}
    else{ return m;	}
    }
