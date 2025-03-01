/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<limits.h>

#   include	<docPageGrid.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docParaProperties.h>
#   include	<docSelect.h>
#   include	"docSelectionGeometry.h"
#   include	<docSectProperties.h>
#   include	<layoutContext.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docLayout.h>
#   include	"docPositionFindJob.h"
#   include	"docLineRectangles.h"
#   include	"docScreenDraw.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise Position/Selection.					*/
/*									*/
/************************************************************************/

void docInitPositionGeometry(	PositionGeometry *		pg )
    {
    pg->pgPositionFlags= 0;

    pg->pgXTwips= -1;
    pg->pgXPixels= -1;
    docInitLayoutPosition( &pg->pgTopPosition );
    docInitLayoutPosition( &pg->pgBaselinePosition );
    docInitLayoutPosition( &pg->pgBelowLinePosition );
    docInitLayoutPosition( &pg->pgNextLinePosition );

    pg->pgParentFrameX0= 0;
    pg->pgParentFrameX1= 0;

    return;
    }

void docInitSelectionGeometry(	SelectionGeometry *		sg )
    {
    geoInitRectangle( &(sg->sgRectangle) );

    docInitPositionGeometry( &(sg->sgHead) );
    docInitPositionGeometry( &(sg->sgTail) );
    }

/************************************************************************/

static void docSetParaXRange(	DocumentRectangle *		dr,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				BufferItem *			paraNode )
    {
    int				paraFirstIndentTwips;
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);

    paraFirstIndentTwips= paraNode->biParaProperties->ppFirstIndentTwips;
    if  ( paraFirstIndentTwips < 0 )
	{
	dr->drX0= docLayoutXPixels( lc, drParaContent->drX0+
						paraFirstIndentTwips );
	}
    else{ dr->drX0= docLayoutXPixels( lc, drParaContent->drX0 );	}

    dr->drX1= docLayoutXPixels( lc, drParaContent->drX1 );

    return;
    }

/************************************************************************/

static void docIncludeParentCellRectangle(
				DocumentRectangle *		drSel,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				const DocumentSelection *	ds,
				struct BufferItem *		paraNode )
    {
    DocumentRectangle	dr;

    geoInitRectangle( &dr );

    docSetParaXRange( &dr, bf, pf, lc, paraNode );

    if  ( docSelectionInsideCell( ds ) )
	{
	dr.drY0= docLayoutYPixels( lc, &(paraNode->biTopPosition) );
	dr.drY1= docLayoutYPixels( lc, &(paraNode->biBelowPosition) );
	}
    else{
	const struct BufferItem *	rowNode= docGetRowNode( paraNode );

	dr.drY0= docLayoutYPixels( lc, &(rowNode->biTopPosition) );
	dr.drY1= docLayoutYPixels( lc, &(rowNode->biBelowPosition) );
	}

    geoUnionRectangle( drSel, drSel, &dr );

    return;
    }

/************************************************************************/

static void docSingleLineRectangle(
				DocumentRectangle *		drSel,
				PositionGeometry *		pgHead,
				const LayoutContext *		lc,
				const DocumentSelection *	ds,
				int				lineHead,
				const ParagraphFrame *		pfHead )
    {
    struct TextLine *		tl;

    CollectRectangles		cr;
    LineRectangles		lr;

    int				yPixels;
    DocumentRectangle		drLineTwips;
    DocumentRectangle		drLineFramePixels;
    const BlockOrigin *		bo= (const BlockOrigin *)0;
    int				partsSeparately;

    tl= ds->dsHead.dpNode->biParaLines+ lineHead;

    yPixels= docLayoutYPixels( lc, &(pgHead->pgBaselinePosition) );

    partsSeparately= docScreenDrawParticulesSeparately( ds->dsHead.dpNode, tl );

    docLineTextRectangleTwips( &drLineTwips,
		    &(pgHead->pgTopPosition),
		    &(pgHead->pgNextLinePosition),
		    tl, pfHead );
    docLineFrameRectanglePixels( &drLineFramePixels, 
		    &(pgHead->pgTopPosition),
		    &(pgHead->pgNextLinePosition),
		    tl, bo, pfHead, lc );

    docInitCollectRectangles( &cr );
    docSetLineRectangles( &lr, lc, ds, ds->dsHead.dpNode,
					tl, yPixels, &drLineFramePixels );

    docTextLineSelectedCollectRectangles( &lr, drLineTwips.drX0,
						partsSeparately, &cr );

    if  ( cr.crCount == 0 )
	{ LDEB(cr.crCount);		}
    else{ *drSel= cr.crUnion;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the smallest rectangle that contains the selection.	*/
/*									*/
/*  1)  Same paragraph?							*/
/*  1b) For a single position, widen to contain the whole I-Bar.	*/
/*  2)  Same table row?							*/
/*  3)  Expose the whole cell.						*/
/*									*/
/************************************************************************/

#   define	IW		5

static void docSelectionRectangle(
				SelectionGeometry *		sg,
				const LayoutContext *		lc,
				const DocumentSelection *	ds,
				int				lineHead,
				int				lineTail,
				const BlockFrame *		bfHead,
				const BlockFrame *		bfTail,
				const ParagraphFrame *		pfHead,
				const ParagraphFrame *		pfTail,
				const struct BufferItem *	bodySectNode )
    {
    DocumentRectangle *		drSel= &(sg->sgRectangle);

    docPixelRectangleForPositions( drSel, &(sg->sgHead), &(sg->sgTail), lc );

    if  ( sg->sgHead.pgTopPosition.lpPage != sg->sgTail.pgTopPosition.lpPage ||
	  sg->sgHead.pgTopPosition.lpColumn != sg->sgTail.pgTopPosition.lpColumn )
	{
	LayoutPosition		lp;
	int			yPixels;

	int			useColumns;

	useColumns= docTreeInColumnType( ds->dsHead.dpNode->biTreeType );

	drSel->drX0= 0;
	drSel->drX1= INT_MAX;

	if  ( useColumns						&&
	      sg->sgHead.pgTopPosition.lpColumn <
				bodySectNode->biSectColumnCount- 1	)
	    {
	    lp= sg->sgHead.pgTopPosition;
	    lp.lpPageYTwips= bfHead->bfContentRect.drY1;
	    lp.lpAtTopOfColumn= 0;

	    yPixels= docLayoutYPixels( lc, &lp );
	    if  ( drSel->drY1 < yPixels )
		{ drSel->drY1=  yPixels;	}
	    }

	if  ( useColumns						&&
	      sg->sgTail.pgTopPosition.lpColumn > 0			)
	    {
	    lp= sg->sgTail.pgTopPosition;
	    lp.lpPageYTwips= bfTail->bfContentRect.drY0;
	    lp.lpAtTopOfColumn= 1;

	    yPixels= docLayoutYPixels( lc, &lp );
	    if  ( drSel->drY0 > yPixels )
		{ drSel->drY0=  yPixels;	}
	    }

	return;
	}

    /*  1  */
    if  ( docSelectionSingleParagraph( ds ) )
	{
	/*  In same line? */
	if  ( lineHead == lineTail )
	    {
	    /*  1b  */
	    if  ( docSamePosition( &(ds->dsTail), &(ds->dsHead) ) )
		{
		drSel->drX0= sg->sgHead.pgXPixels- IW;
		drSel->drX1= sg->sgTail.pgXPixels+ IW;
		}
	    else{
		docSingleLineRectangle( drSel, &(sg->sgHead),
						    lc, ds, lineHead, pfHead );
		}

	    return;
	    }

	docSetParaXRange( drSel, bfHead, pfHead, lc, ds->dsHead.dpNode );

	return;
	}

    /*  2  */
    if  ( ds->dsHead.dpNode->biParaProperties->ppTableNesting == 0	||
	  ds->dsTail.dpNode->biParaProperties->ppTableNesting == 0	||
	  ! docSelectionInsideRow( ds )				)
	{
	struct BufferItem *	parentNode;

	parentNode= docGetCommonParent( ds->dsHead.dpNode, ds->dsTail.dpNode );
	if  ( ! parentNode )
	    { XDEB(parentNode);	}
	else{
	    DocumentRectangle	parentRect;

	    docNodeRectangle( &parentRect, parentNode, lc,
					(const BlockOrigin *)0 );

	    drSel->drX0= parentRect.drX0;
	    drSel->drX1= parentRect.drX1;
	    }
	}

    /*  3  */
    if  ( ds->dsHead.dpNode->biParaProperties->ppTableNesting > 0 )
	{
	docIncludeParentCellRectangle( drSel, bfHead, pfHead, lc,
						ds, ds->dsHead.dpNode );
	}

    /*  3  */
    if  ( ds->dsTail.dpNode->biParaProperties->ppTableNesting > 0 )
	{
	docIncludeParentCellRectangle( drSel, bfTail, pfTail, lc,
						ds, ds->dsTail.dpNode );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the position on paper/screen of a certain position in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void docSetPositionGeometry(	PositionGeometry *		pg,
				const ParagraphFrame *		pf,
				const TextLine *		tl,
				int				xPixels,
				int				xTwips,
				int				flags )
    {
    pg->pgPositionFlags= flags;

    pg->pgXPixels= xPixels;
    pg->pgXTwips= xTwips;

    pg->pgParentFrameX0= pf->pfParentContentRect.drX0;
    pg->pgParentFrameX1= pf->pfParentContentRect.drX1;

    pg->pgTopPosition= tl->tlTopPosition;

    docTextLineGetBaseline( &(pg->pgBaselinePosition), tl );
    docTextLineGetBelowLine( &(pg->pgBelowLinePosition), tl );
    docTextLineGetNextLineTop( &(pg->pgNextLinePosition), tl );

    return;
    }

static int docPositionGeometryLow(	PositionGeometry *		pg,
					int *				pLine,
					ParagraphFrame *		pf,
					BlockFrame *			bf,
					const DocumentPosition *	dp,
					int				lastOne,
					const LayoutContext *		lc )
    {
    int			res;
    int			part;
    int			line= 0;

    TextLine *		tl;

    int			docXTwips= 0;
    int			docXPixels= 0;

    int			partFlags= 0;
    int			lineFlags= 0;

    if  ( docFindParticuleOfPosition( &part, &partFlags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return -1;	}

    if  ( docFindLineOfPosition( &line, &lineFlags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return -1;	}

    tl= dp->dpNode->biParaLines+ line;

    if  ( dp->dpStroff == tl->tlStroff+ tl->tlStrlen	&&
	  part < dp->dpNode->biParaParticuleCount- 1	&&
	  line < dp->dpNode->biParaLineCount- 1		&&
	  ( partFlags & POSflagPART_AFTER_BREAK ) != 0	)
	{
	line++; tl++; part++;

	docSetParticuleFlags( &partFlags, dp->dpNode, part, dp->dpStroff );
	docSetLineFlags( &lineFlags, dp->dpNode, dp->dpStroff, tl );
	}

    docBlockFrameTwips( bf, dp->dpNode, lc->lcDocument,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn );

    docParagraphFrameTwips( pf, bf, dp->dpNode );

    res= docLayoutLineXOfPosition( &docXPixels, &docXTwips,
					    dp, lc, pf, dp->dpNode, tl );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    docSetPositionGeometry( pg, pf, tl, docXPixels, docXTwips,
						    partFlags | lineFlags );

    *pLine= line;

    return 0;
    }

void docSelectionGeometry(	SelectionGeometry *		sg,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				int				lastLine,
				const LayoutContext *		lc )
    {
    int			lastOneHead= PARAfindFIRST;
    int			lastOneTail= PARAfindFIRST;

    int			lineHead= -1;
    int			lineTail= -1;

    BlockFrame		bfHead;
    BlockFrame		bfTail;
    ParagraphFrame	pfHead;
    ParagraphFrame	pfTail;

    docLayoutInitBlockFrame( &bfHead );
    docLayoutInitBlockFrame( &bfTail );

    if  ( ds->dsDirection != 0 )
	{ lastLine= 0;	}

    if  ( ds->dsDirection > 0 || lastLine )
	{ lastOneHead= PARAfindLAST;	}
    if  ( lastLine )
	{ lastOneTail= PARAfindLAST;	}

    docPositionGeometryLow( &(sg->sgHead), &lineHead, &pfHead, &bfHead,
					&(ds->dsHead), lastOneHead, lc );
    docPositionGeometryLow( &(sg->sgTail), &lineTail, &pfTail, &bfTail,
					&(ds->dsTail), lastOneTail, lc );

    docSelectionRectangle( sg, lc, ds,
				lineHead, lineTail,
				&bfHead, &bfTail,
				&pfHead, &pfTail,
				bodySectNode );

    docLayoutCleanBlockFrame( &bfHead );
    docLayoutCleanBlockFrame( &bfTail );

    return;
    }

void docPositionGeometry(	PositionGeometry *		pg,
				const DocumentPosition *	dp,
				int				lastOne,
				const LayoutContext *		lc )
    {
    int		line;

    BlockFrame		bf;
    ParagraphFrame	pf;

    docLayoutInitBlockFrame( &bf );

    docPositionGeometryLow( pg, &line, &pf, &bf, dp, lastOne, lc );

    docLayoutCleanBlockFrame( &bf );
    }

/************************************************************************/
/*									*/
/*  Find the document position corresponding to the x coordinate in	*/
/*  pixels in a line of text.						*/
/*									*/
/*  NOTE: As docXPixels has been corrected for the scrollbars,		*/
/*	scrolling is irrelevant.					*/
/*									*/
/************************************************************************/

int docFindPositionInLine(	DocumentPosition *	dp,
				PositionGeometry *	pg,
				const LayoutContext *	lc,
				struct BufferItem *	paraNode,
				TextLine *		tl,
				int			docXPixels )
    {
    int				res= 0;

    BlockFrame			bf;
    ParagraphFrame		pf;

    int				docYPixels;
    PositionFindJob		pfj;

    LayoutPosition		lpTop;
    LayoutPosition		lpBaseline;
    LayoutPosition		lpBelow;

    DocumentRectangle		drLineFramePixels;

    const struct BlockOrigin * const bo= (const struct BlockOrigin *)0;

    docLayoutInitBlockFrame( &bf );

    docTextLineGetShiftedTop( &lpTop, bo, tl );
    docTextLineGetShiftedBaseline( &lpBaseline, bo, tl );
    docTextLineGetShiftedBelowLine( &lpBelow, bo, tl );

    docBlockFrameTwips( &bf, paraNode, lc->lcDocument,
				    lpBaseline.lpPage, lpBaseline.lpColumn );
    docParagraphFrameTwips( &pf, &bf, paraNode );

    docLineFrameRectanglePixels( &drLineFramePixels, &lpTop, &lpBelow,
							tl, bo, &pf, lc );

    docYPixels= docLayoutYPixels( lc, &lpBaseline );
    docSetFindPositionJob( &pfj, lc, docXPixels, docYPixels );

    res= docDrawTextLine( paraNode, tl, &pf, &drLineFramePixels, &pfj,
					    &(pfj.pfjDrawingContext), bo );
    if  ( res < 0 )
	{ LDEB(res); goto ready;	}

    if  ( ! pfj.pfjFound )
	{ LDEB(pfj.pfjFound); res= 1; goto ready;	}

    res= 0;

    *dp= pfj.pfjDocumentPosition;
    *pg= pfj.pfjPositionGeometry;

  ready:

    docLayoutCleanBlockFrame( &bf );

    return res;
    }

