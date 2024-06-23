#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	<docPageGrid.h>
#   include	<layoutContext.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docSelectLayout.h>
#   include	<docSelect.h>
#   include	<docBlockOrnaments.h>
#   include	<docSectProperties.h>
#   include	<docBlockFrame.h>
#   include	<docBorderProperties.h>
#   include	<docBreakKind.h>
#   include	<docParaProperties.h>
#   include	<docBuf.h>
#   include	<docSectLayout.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw nodes in a particular (newspaper style) column.		*/
/*									*/
/************************************************************************/

static int docDrawLineBeforeColumn(	struct BufferItem *	nextBodyNode,
					void *			through,
					LayoutPosition *	lpHere,
					BlockFrame *		bf,
					DrawingContext *	dc )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    const SectionProperties *	sp= nextBodyNode->biSectProperties;

    DocumentRectangle		drLLine;
    DocumentRectangle		drRLine;
    int				x0;
    int				x1;

    BlockOrnaments		ornaments;
    BorderProperties		bpLeft;

    struct DocumentNote *	dn0;
    const struct DocumentField *	df0;

    int				alongNotes= 0;
    struct BufferItem *		biColBottomRoot;

    DocumentPosition		dpBottom;
    int				lineBottom;
    int				partBottom;
    const TextLine *		tlBottom= (const TextLine *)0;

    if  ( ! docIsSectNode( nextBodyNode ) )
	{ LDEB(nextBodyNode->biLevel); return -1;	}

    df0= docGetFirstNoteOnPage( &dn0, lc->lcDocument,
					    lpHere->lpPage, DOCinFOOTNOTE );
    if  ( df0 )
	{
	if  ( dn0->dnDocumentTree.dtRoot->biSectSelectionScope.ssOwnerSectNr >=
						nextBodyNode->biNumberInParent )
	    { alongNotes= 1;	}
	}

    docInitBorderProperties( &bpLeft );
    bpLeft.bpStyle= DOCbsS;
    bpLeft.bpPenWideTwips= 15;

    docInitBlockOrnaments( &ornaments );
    PROPmaskADD( &(ornaments.boPropMask), ORNdrawLEFT_BORDER );
    ornaments.boLeftBorder= &bpLeft;
    ornaments.boLeftBorderNumber= 1; /* FAKE */

    drLLine= bf->bfFlowRect;

    if  ( nextBodyNode->biTopPosition.lpPage == lpHere->lpPage )
	{ drLLine.drY0= nextBodyNode->biTopPosition.lpPageYTwips; }

    biColBottomRoot= nextBodyNode;
    if  ( alongNotes )
	{
	DocumentNote *		dnz;
	const struct DocumentField *	dfz;

	dfz= docGetLastNoteInColumn( &dnz, lc->lcDocument,
			    lpHere->lpPage, lpHere->lpColumn, DOCinFOOTNOTE );
	if  ( dfz )
	    { biColBottomRoot= dnz->dnDocumentTree.dtRoot;	}
	}

    if  ( nextBodyNode->biTopPosition.lpPage <= lpHere->lpPage )
	{
	if  ( docGetLastInColumnForNode( &dpBottom, &lineBottom, &partBottom,
			biColBottomRoot, lpHere->lpPage, lpHere->lpColumn ) )
	    { LLDEB(lpHere->lpPage,lpHere->lpColumn); return -1;	}

	tlBottom= dpBottom.dpNode->biParaLines+ lineBottom;
	drLLine.drY1= tlBottom->tlTopPosition.lpPageYTwips+
						    tlBottom->tlLineStride;

	if  ( nextBodyNode->biBelowPosition.lpPage == lpHere->lpPage )
	    { drLLine.drY1= nextBodyNode->biBelowPosition.lpPageYTwips; }

	docSectGetColumnX( &(drLLine.drX0), &x0, &x1, sp,
				&(bf->bfPageGeometry), lpHere->lpColumn );
	drRLine= drLLine;
	drRLine.drX0= drLLine.drX0- ornaments.boLeftBorder->bpPenWideTwips;

	if  ( (*dc->dcDrawOrnaments)( &ornaments, lpHere->lpPage,
					    &drRLine, &drLLine, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    docCleanBorderProperties( &bpLine );
    return 0;
    }

/************************************************************************/

/**
 *  Finish all nodes on the current page. We close then, and then
 *  reopen them to deal with a limitation of the way in which we build 
 *  the structure tree for the PDF output.
 */
static int docPsPrintFinishNodes(
				struct BufferItem *	node,
				void *			through,
				DrawingContext *	dc )
    {
    while( node )
	{
	if  ( (*dc->dcFinishNode)( through, dc, 0, 1, node ) )
	    { LDEB(node->biLevel); return -1; }

	node= node->biParent;
	}

    return 0;
    }

/**
 *  Start all nodes on the next page. We have closed then, and now
 *  we reopen nodes to deal with a limitation of the way in which we build 
 *  the structure tree for the PDF output.
 */
static int docPsPrintStartNodes(
				struct BufferItem *	node,
				void *			through,
				DrawingContext *	dc )
    {
    if  ( node->biParent && docPsPrintStartNodes( node->biParent, through, dc ) )
	{ PDEB(node->biParent); return -1;	}

    if  ( (*dc->dcStartNode)( through, dc, 0, 1, node ) )
	{ LDEB(node->biLevel); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page. (Actually the next newspaper style column on	*/
/*  the page)								*/
/*									*/
/*  A)  Draw the shapes for this page.					*/
/*  1)  Just to make the compiler happy, if nothing is to be printed	*/
/*	anymore, the current position is not going to be used.		*/
/*									*/
/************************************************************************/

int docDrawToNextColumn(	struct BufferItem *	thisNode,
				struct BufferItem *	nextNode,
				void *			through,
				LayoutPosition *	lpHere,
				BlockFrame *		bf,
				DrawingContext *	dc,
				const char *		why )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;

    const int			asLast= 0;

    int				startNewPage= 0;
    int				nextPage;
    int				nextColumn;
    int				isPageBreak= 0;

    BufferItem *		thisBodySectNode= docGetBodySectNode( thisNode, bd );
    BufferItem *		nextBodySectNode= docGetBodySectNode( nextNode, bd );

    nextPage= lpHere->lpPage;
    nextColumn= lpHere->lpColumn;

    nextColumn++;

    if  ( nextBodySectNode )
	{
	if  ( nextColumn >= nextBodySectNode->biSectColumnCount )
	    { nextPage++; nextColumn= 0; }
	}
    else{
	if  ( nextColumn >= thisBodySectNode->biSectColumnCount )
	    { nextPage++; nextColumn= 0; }
	}

    if  ( nextPage != lpHere->lpPage )
	{ isPageBreak= 1; }

    if  ( dc->dcLastPage >= 0		&&
	  nextPage > dc->dcLastPage	)
	{ return 1; }

    if  ( dc->dcFirstPage < 0				||
	  lpHere->lpPage >= dc->dcFirstPage		)
	{
	const int	belowText= 0;

	startNewPage= 1;

	if  ( thisBodySectNode && thisBodySectNode->biTreeType != DOCinBODY )
	    { SDEB(docTreeTypeStr(thisBodySectNode->biTreeType));	}

	if  ( thisBodySectNode					&&
              thisBodySectNode->biTreeType == DOCinBODY		&&
	      dc->dcDrawOtherTrees				)
	    {
	    if  ( docDrawFootnotesForColumn( lpHere->lpPage, lpHere->lpColumn,
								through, dc ) )
		{ LDEB(lpHere->lpPage); return -1;	}

	    if  ( ! dc->dcPostponeHeadersFooters			&&
		  isPageBreak						&&
		  docDrawPageFooter( thisBodySectNode, through, dc,
						    lpHere->lpPage )	)
		{ LDEB(lpHere->lpPage); return -1;	}
	    }

	/*  A  */
	if  ( docDrawShapesForPage( through, dc, belowText, lpHere->lpPage ) )
	    { LDEB(lpHere->lpPage);	}

	if  ( isPageBreak )
	    {
	    if  ( dc->dcFinishNode					&&
		  docPsPrintFinishNodes( thisNode, through, dc )	)
		{ LDEB(isPageBreak); return -1;	}

	    if  ( dc->dcFinishPage						&&
		  (*dc->dcFinishPage)( through, dc, thisBodySectNode,
						lpHere->lpPage, asLast )	)
		{ LDEB(isPageBreak); return -1;	}
	    }
	}

    if  ( dc->dcLastPage < 0		||
	  nextPage <= dc->dcLastPage	)
	{
	lpHere->lpColumn= nextColumn;
	lpHere->lpPage= nextPage;

	if  ( nextBodySectNode )
	    {
	    const SectionProperties *	sp= nextBodySectNode->biSectProperties;
	    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
	    const int			belowText= 1;

	    docLayoutSectColumnTop( lpHere, bf, nextBodySectNode, bd );

	    if  ( isPageBreak && startNewPage )
		{
		const int asArtifact= docDrawHeaderAsArtifact(
					    lpHere->lpPage, nextBodySectNode );
		const int postponeHeader= dc->dcPostponeHeadersFooters && asArtifact;

		if  ( dc->dcStartPage					&&
		      (*dc->dcStartPage)( through, dg, dc, why,
						    lpHere->lpPage )	)
		    { LDEB(1); return -1;	}

		if  ( dc->dcStartNode					&&
		      docPsPrintStartNodes( nextNode->biParent, through, dc )	)
		    { LDEB(isPageBreak); return -1;	}

		if  ( docDrawShapesForPage(
				    through, dc, belowText, lpHere->lpPage ) )
		    { LDEB(lpHere->lpPage);	}

		if  ( dc->dcDrawOtherTrees				&&
		      ! postponeHeader					&&
		      docDrawPageHeader( nextBodySectNode, through, dc,
						    lpHere->lpPage )	)
		    { LDEB(lpHere->lpPage); return -1;	}
		}

	    if  ( lpHere->lpColumn > 0					&&
		  sp->spLineBetweenColumns				&&
		  dc->dcDrawOrnaments					&&
		  docDrawLineBeforeColumn( nextBodySectNode, through,
						    lpHere, bf, dc )	)
		{ LDEB(sp->spLineBetweenColumns);	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Report why a page break is inserted.				*/
/*									*/
/************************************************************************/

static const char * docDrawGetBreakWhy(
				const DrawingContext *	dc,
				const BufferItem *	thisNode )
    {
    if  ( thisNode->biTreeType == DOCinBODY )
	{
	switch( thisNode->biLevel )
	    {
	    case DOClevPARA:
		switch( thisNode->biParaProperties->ppBreakKind )
		    {
		    case DOCibkNONE:
			return "paragraph nopagebb";
		    case DOCibkCOL:
			return "paragraph \\col";
		    case DOCibkPAGE:
			return "paragraph pagebb";
		    default:
			SDEB(docLevelStr(thisNode->biLevel));
			SDEB(docBreakKindStr(
			    thisNode->biParaProperties->ppBreakKind));
		    }
		break;
	    case DOClevSECT:
		{
		const LayoutContext *		lc= dc->dcLayoutContext;
		struct BufferDocument *		bd= lc->lcDocument;

		int	breakKind= docSectGetBreakKind(
					thisNode->biSectProperties,
					bd->bdProperties,
					lc->lcHonourSpecialSectBreaks );

		switch( breakKind )
		    {
		    case DOCibkNONE:
			return "section sbknone";
		    case DOCibkCOL:
			return "section sbkcol";
		    case DOCibkPAGE:
			return "section sbkpage";
		    case DOCibkEVEN:
			return "section sbkeven";
		    case DOCibkODD:
			return "section sbkodd";
		    default:
			SDEB(docLevelStr(thisNode->biLevel));
			SDEB(docBreakKindStr(breakKind));
		    }
		}
		break;
	    case DOClevROW:
		if  ( docIsRowNode( thisNode ) )
		    { return "row head flow";	}
		else{ return "section flow";	}
		break;
	    default:
		SDEB(docLevelStr(thisNode->biLevel));
		break;
	    }
	}

    return "UNKNOWN";
    }

/************************************************************************/
/*									*/
/*  Skip to the column where a document node begins.			*/
/*									*/
/*  1)  No activity before the first page to be printed.		*/
/*									*/
/************************************************************************/

int docDrawToColumnOfNode(	struct BufferItem *	prevNode,
				struct BufferItem *	thisNode,
				void *			through,
				LayoutPosition *	lpHere,
				DrawingContext *	dc,
				const BlockOrigin *	bo )
    {
    int			rval= 0;
    BlockFrame		bf;
    LayoutPosition	lpTop;

    docLayoutInitBlockFrame( &bf );

if  ( thisNode->biTreeType == DOCinSHPTXT )
    { goto ready;	}

    docShiftPosition( &lpTop, bo, &(thisNode->biTopPosition) );

    /*  1  */
    if  ( lpTop.lpPage < dc->dcFirstPage )
	{ goto ready; }

    if  ( docCompareLayoutPositionFrames( lpHere, &lpTop ) < 0 )
	{
	const char *	breakWhy= docDrawGetBreakWhy( dc, thisNode );

	while( docCompareLayoutPositionFrames( lpHere, &lpTop ) < 0 )
	    {
	    if  ( docDrawToNextColumn( prevNode, thisNode, through,
					    lpHere, &bf, dc, breakWhy )	)
		{ SDEB(docLevelStr(thisNode->biLevel)); rval= -1; goto ready; }
	    }
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }
