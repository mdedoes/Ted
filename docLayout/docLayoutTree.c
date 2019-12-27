/************************************************************************/
/*									*/
/*  Geometry calculations about external items.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docSelectLayout.h"
#   include	"docLayoutDocumentTree.h"
#   include	"layoutContext.h"
#   include	<docPageGrid.h>
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docNotes.h>
#   include	<docRecalculateFields.h>
#   include	<docDocumentField.h>
#   include	<docSelect.h>
#   include	<docSectProperties.h>
#   include	<docBlockFrame.h>
#   include	<docNodeTree.h>
#   include	<docDocumentTree.h>
#   include	<docTreeNode.h>
#   include	<docBuf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine the box around a header or a footer.			*/
/*									*/
/************************************************************************/

static int docNoteSeparatorBox(	DocumentRectangle *		dr,
				int				noteItKind,
				int				noteSepKind,
				int				page,
				int				column,
				const LayoutContext *		lc )
    {
    struct DocumentTree *	noteSepTree;
    int				y0Twips;
    struct DocumentNote *	dnFirstNote;
    DocumentField *		dfNote;

    const struct BufferItem *	bodySectNode;

    dfNote= docGetFirstNoteInColumn( &dnFirstNote, lc->lcDocument, 
						page, column, noteItKind );

    if  ( ! dfNote )
	{ XDEB(dfNote); return -1;	}

    bodySectNode= docGetBodySectNodeOfScope( &(dfNote->dfSelectionScope),
							    lc->lcDocument );

    if  ( docNoteSeparatorRectangle( dr, &noteSepTree, &y0Twips,
				dnFirstNote, noteSepKind, bodySectNode, lc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docGetBoxAroundTree(	DocumentRectangle *		dr,
				const struct DocumentTree *	tree,
				int				justUsed,
				int				page,
				int				column,
				const LayoutContext *		lc )
    {
    int				rval= 0;
    struct BufferItem *		treeRoot= tree->dtRoot;

    BlockFrame			bf;
    DocumentRectangle		drTwips;
    DocumentRectangle		drBox;

    docLayoutInitBlockFrame( &bf );

    if  ( ! treeRoot )
	{ XDEB(treeRoot); rval= -1; goto ready;	}

    switch( treeRoot->biTreeType )
	{
	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    docBlockFrameTwips( &bf, treeRoot, lc->lcDocument, page, column );

	    drTwips= bf.bfContentRect;

	    if  ( justUsed )
		{
		drTwips.drY0= tree->dtY0UsedTwips;
		drTwips.drY1= tree->dtY1UsedTwips;
		}
	    else{
		drTwips.drY0= tree->dtY0ReservedTwips;
		drTwips.drY1= tree->dtY1ReservedTwips;
		}

	    docGetPixelRect( &drBox, lc, &drTwips, page );

	    *dr= drBox; break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    {
	    DocumentPosition	dpTop;
	    DocumentPosition	dpBot;

	    int			lineTop;
	    int			lineBot;

	    int			partTop;
	    int			partBot;

	    const TextLine *	tlTop;
	    const TextLine *	tlBot;

	    LayoutPosition	lpTop;
	    LayoutPosition	lpBot;

	    docBlockFrameTwips( &bf, tree->dtRoot, lc->lcDocument,
							    page, column );

	    if  ( docGetFirstInColumnForNode( &dpTop, &lineTop, &partTop,
						tree->dtRoot, page, column ) )
		{
		LLDEB(page,column);
		SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
		rval= -1; goto ready;
		}

	    if  ( docGetLastInColumnForNode( &dpBot, &lineBot, &partBot,
						tree->dtRoot, page, column ) )
		{ LDEB(page); rval= -1; goto ready;	}

	    tlTop= dpTop.dpNode->biParaLines+ lineTop;
	    tlBot= dpBot.dpNode->biParaLines+ lineBot;

	    lpTop= tlTop->tlTopPosition;
	    lpBot= tlBot->tlTopPosition;
	    lpBot.lpPageYTwips += tlBot->tlLineStride;

	    docGetPixelRectForPos( &drBox, lc,
			     bf.bfContentRect.drX0, bf.bfContentRect.drX1,
			     &lpTop, &lpBot );
	    }

	    *dr= drBox; break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:

	    if  ( docNoteSeparatorBox( &drBox,
			    DOCinFOOTNOTE, treeRoot->biTreeType,
			    page, column, lc ) )
		{ LLDEB(page,column); rval= -1; goto ready;	}

	    *dr= drBox; break;

	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( docNoteSeparatorBox( &drBox,
			    DOCinENDNOTE, treeRoot->biTreeType,
			    page, column, lc ) )
		{ LLDEB(page,column); rval= -1; goto ready;	}

	    *dr= drBox; break;

	default:
	    LDEB(treeRoot->biTreeType);
	    rval= -1; goto ready;
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/

static void docPlaceHeader(	struct DocumentTree *		tree,
				const DocumentGeometry *	dgSect )
    {
    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;

    tree->dtY0ReservedTwips= dgSect->dgHeaderPositionTwips;
    tree->dtY1ReservedTwips= dgSect->dgMargins.roTopOffset;

    if  ( tree->dtY1ReservedTwips < tree->dtY1UsedTwips )
	{ tree->dtY1ReservedTwips=  tree->dtY1UsedTwips;	}

    return;
    }

static void docPlaceFooter(	struct DocumentTree *		tree,
				const DocumentGeometry *	dgSect )
    {
    int	high= tree->dtRoot->biBelowPosition.lpPageYTwips-
			    tree->dtRoot->biTopPosition.lpPageYTwips;

    tree->dtY1UsedTwips=
		dgSect->dgPageHighTwips- dgSect->dgFooterPositionTwips;
    tree->dtY0UsedTwips= tree->dtY1UsedTwips- high;

    tree->dtY0ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgMargins.roBottomOffset;
    tree->dtY1ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgFooterPositionTwips;

    if  ( tree->dtY0ReservedTwips > tree->dtY0UsedTwips )
	{ tree->dtY0ReservedTwips=  tree->dtY0UsedTwips;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Do a preliminary layout of a document tree.				*/
/*									*/
/*  Inside the tree, geometry is correct. Some trees are however used	*/
/*  in different positions and here we just calculate the layout in	*/
/*  order to know the size of the tree to use it in geometry		*/
/*  calculations about the document as a whole.				*/
/*									*/
/*  1)  Remove the bottom of the page master frame. This is the routine	*/
/*	that calculates it for future use.				*/
/*									*/
/************************************************************************/

int docTreePrelayout(		struct DocumentTree *		tree,
				const struct BufferItem *	bodySectNode,
				LayoutJob *			lj )
    {
    int				rval= 0;
    const LayoutContext *	lc= lj->ljContext;
    const DocumentGeometry *	dgSect= &(bodySectNode->biSectDocumentGeometry);
    LayoutJob			treeLj;
    LayoutPosition		treeLp;

    BlockFrame			bf;

    const int			recursively= 0;

    docLayoutInitBlockFrame( &bf );

    docInvalidateTreeLayout( tree );

    if  ( ! tree->dtRoot )
	{ goto ready;	}

    docDelimitTables( tree->dtRoot, recursively );

    tree->dtRoot->biSectDocumentGeometry.dgPageWideTwips=
						dgSect->dgPageWideTwips;
    tree->dtRoot->biSectDocumentGeometry.dgMargins.roLeftOffset=
						dgSect->dgMargins.roLeftOffset;
    tree->dtRoot->biSectDocumentGeometry.dgMargins.roRightOffset=
						dgSect->dgMargins.roRightOffset;

    treeLp.lpPage= tree->dtRoot->biTopPosition.lpPage;
    treeLp.lpColumn= 0;
    treeLp.lpPageYTwips= dgSect->dgHeaderPositionTwips;
    treeLp.lpAtTopOfColumn= 1; /* not really */

    treeLj= *lj;
    treeLj.ljChangedRectanglePixels= (DocumentRectangle *)0;

    treeLj.ljBodySectNode= bodySectNode;
    treeLj.ljChangedNode= lj->ljChangedNode;

    docBlockFrameTwips( &bf, tree->dtRoot, lc->lcDocument,
					treeLp.lpPage, treeLp.lpColumn );
    bf.bfContentRect.drY0= treeLp.lpPageYTwips;
    bf.bfFlowRect.drY0= bf.bfContentRect.drY0;

    /*  1  */
    bf.bfContentRect.drY1= dgSect->dgPageHighTwips;
    bf.bfFlowRect.drY1= bf.bfContentRect.drY1;

    if  ( docLayoutNodeImplementation( &treeLp, &treeLp,
						tree->dtRoot, &bf, &treeLj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	    docPlaceHeader( tree, dgSect );
	    break;

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    docPlaceFooter( tree, dgSect );
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  temporarily: will be placed later on */
	    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
	    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    /*  temporarily */
	    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
	    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;
	    break;

	default:
	    LDEB(tree->dtRoot->biTreeType); rval= -1; goto ready;
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Do the preliminary layout of the headers and footers of a section	*/
/*									*/
/************************************************************************/

int docSectHeaderFooterPrelayout(	struct BufferItem *	bodySectNode,
					LayoutJob *		lj )
    {
    int				hdft;
    int				redraw= 0;

    for ( hdft= 0; hdft < DOC_HeaderFooterTypeCount; hdft++ )
	{
	struct DocumentTree *		dtHdFt;
	unsigned char		applies;

	int			resY0;
	int			resY1;

	dtHdFt= docSectionHeaderFooter( bodySectNode, &applies,
				    lj->ljContext->lcDocument->bdProperties,
				    DOC_HeaderFooterTypes[hdft] );
	if  ( ! dtHdFt )
	    { XDEB(dtHdFt); return -1;	}

	resY0= dtHdFt->dtY0ReservedTwips;
	resY1= dtHdFt->dtY1ReservedTwips;

	if  ( ! dtHdFt->dtRoot || ! applies )
	    {
	    dtHdFt->dtY0ReservedTwips= 0;
	    dtHdFt->dtY1ReservedTwips= 0;
	    }
	else{
	    if  ( docTreePrelayout( dtHdFt, bodySectNode, lj ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( lj->ljChangedRectanglePixels			&&
	      ( dtHdFt->dtY0ReservedTwips != resY0	||
	        dtHdFt->dtY1ReservedTwips != resY1	)	)
	    { redraw++;	}
	}

    if  ( redraw )
	{
	DocumentRectangle	drPixels;

	docGetPixelRectangleForPages( &drPixels, lj->ljContext,
					bodySectNode->biTopPosition.lpPage,
					bodySectNode->biBelowPosition.lpPage );

	geoUnionRectangle( lj->ljChangedRectanglePixels,
				    lj->ljChangedRectanglePixels, &drPixels );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset page dependent layout administration of headers/footers etc	*/
/*  fo force a subsequent recalculation.				*/
/*									*/
/************************************************************************/

void docResetExternalTreeLayout(	struct BufferDocument *	bd )
    {
    int		i;

    docInvalidateTreeLayout( &(bd->bdFtnsep) );
    docInvalidateTreeLayout( &(bd->bdFtnsepc) );
    docInvalidateTreeLayout( &(bd->bdFtncn) );

    docInvalidateTreeLayout( &(bd->bdAftnsep) );
    docInvalidateTreeLayout( &(bd->bdAftnsepc) );
    docInvalidateTreeLayout( &(bd->bdAftncn) );

    for ( i= 0; i < bd->bdBody.dtRoot->biChildCount; i++ )
	{
	struct BufferItem *	bodySectNode=  bd->bdBody.dtRoot->biChildren[i];

	docInvalidateSectHeaderFooterLayout( bodySectNode );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Verify that the root of a selection is formatted for the current	*/
/*  page, if not format it.						*/
/*									*/
/************************************************************************/

static int docGetY0ForSelectedNoteSeparator(
				int *			pY0Twips,
				struct BufferItem **	pBodySectNode,
				const LayoutContext *	lc,
				const struct DocumentTree *	selTree,
				int			noteItKind,
				int			sepItKind )
    {
    struct DocumentNote *	dnFirstNote;
    struct DocumentTree *	bodyTree;
    struct DocumentTree *	noteSepTree;
    int				y0Twips;

    DocumentRectangle		drExtern;

    DocumentField *		dfNote;
    struct BufferItem *		bodySectNode;

    dfNote= docGetFirstNoteInColumn( &dnFirstNote, lc->lcDocument, 
					    selTree->dtPageSelectedUpon,
					    selTree->dtColumnSelectedIn,
					    noteItKind );
    if  ( ! dfNote )
	{
	LLDEB(selTree->dtPageSelectedUpon,selTree->dtColumnSelectedIn);
	return -1;
	}

    if  ( docGetRootOfSelectionScope( &bodyTree, &bodySectNode, lc->lcDocument,
						&(dfNote->dfSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( docNoteSeparatorRectangle( &drExtern, &noteSepTree,
			&y0Twips, dnFirstNote, sepItKind, bodySectNode, lc ) )
	{ LDEB(1); return -1;	}

    *pY0Twips= y0Twips;
    *pBodySectNode= bodySectNode;
    return 0;
    }

int docCheckPageOfSelectedTree(	int *			pChanged,
				struct BufferItem **	pBodySectNode,
				DocumentRectangle *	drChanged,
				struct DocumentTree *	selTree,
				const LayoutContext *	lc,
				START_TREE_LAYOUT	startTreeLayout )
    {
    int				y0Twips;
    struct BufferItem *		selRootBodySectNode= (struct BufferItem *)0;
    const SelectionScope *	selRootScope;
    const int			adjustDocument= 0;

    if  ( ! selTree->dtRoot )
	{ XDEB(selTree->dtRoot); return -1;	}

    selRootScope= &(selTree->dtRoot->biSectSelectionScope);

    switch( selTree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    selRootBodySectNode= docGetBodySectNodeOfScope( selRootScope,
							    lc->lcDocument );

	    y0Twips= selTree->dtY0UsedTwips;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    selRootBodySectNode= docGetBodySectNodeOfScope( selRootScope,
							    lc->lcDocument );

	    *pBodySectNode= selRootBodySectNode;
	    *pChanged= 0;
	    return 0;

	case DOCinFTNSEP:

	    if  ( selTree->dtPageSelectedUpon < 0 )
		{ LDEB(selTree->dtPageSelectedUpon); return -1;	}

	    if  ( docGetY0ForSelectedNoteSeparator( &y0Twips,
						&selRootBodySectNode,
						lc, selTree,
						DOCinFOOTNOTE, DOCinFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFTNSEPC:
	case DOCinFTNCN:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;

	case DOCinAFTNSEP:

	    if  ( selTree->dtPageSelectedUpon < 0 )
		{ LDEB(selTree->dtPageSelectedUpon); return -1;	}

	    if  ( docGetY0ForSelectedNoteSeparator( &y0Twips,
						&selRootBodySectNode,
						lc, selTree,
						DOCinENDNOTE, DOCinAFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    SDEB(docTreeTypeStr(selTree->dtRoot->biTreeType));
	    return -1;

	default:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;
	}

    if  ( selTree->dtPageSelectedUpon < 0 )
	{ LSDEB(selTree->dtPageSelectedUpon,docTreeTypeStr(selTree->dtRoot->biTreeType)); return -1;	}

    if  ( selTree->dtPageFormattedFor == selTree->dtPageSelectedUpon &&
	  selTree->dtColumnFormattedFor == selTree->dtColumnSelectedIn )
	{
	*pBodySectNode= selRootBodySectNode;
	*pChanged= 0;
	return 0;
	}

    /*  We do not expect the tree to change height here	*/
    if  ( docLayoutDocumentTree( selTree, drChanged,
				    selTree->dtPageSelectedUpon,
				    selTree->dtColumnSelectedIn,
				    y0Twips, selRootBodySectNode, lc,
				    startTreeLayout, adjustDocument ) )
	{ LDEB(selTree->dtPageSelectedUpon); return -1; }

    *pBodySectNode= selRootBodySectNode;
    *pChanged= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a document tree such as a page header or	*/
/*  footer. NEVER called for the document body.				*/
/*									*/
/************************************************************************/

int docLayoutDocumentTree(	struct DocumentTree *	tree,
				DocumentRectangle *	drChanged,
				int			page,
				int			column,
				int			y0Twips,
				const struct BufferItem * bodySectNode,
				const LayoutContext *	lc,
				START_TREE_LAYOUT	startTreeLayout,
				int			adjustDocument )
    {
    int				rval= 0;

    RecalculateFields		rf;
    LayoutJob			lj;
    BlockFrame			bf;

    LayoutPosition		lpHere;

    int				oldY1Used= tree->dtY1UsedTwips;

    docLayoutInitBlockFrame( &bf );
    docInitRecalculateFields( &rf );
    docInitLayoutJob( &lj );

    rf.rfDocument= lc->lcDocument;
    rf.rfUpdateFlags=
	    FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoPAGE_NUMBER;
    rf.rfFieldsUpdated= 0;
    rf.rfBodySectNode= bodySectNode;
    rf.rfLocale= lc->lcLocale;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( &rf, tree,
						    bodySectNode, page ) )
	{ LDEB(page); rval= -1; goto ready;	}

    lpHere.lpPage= page;
    lpHere.lpColumn= column;
    lpHere.lpPageYTwips= y0Twips;
    lpHere.lpAtTopOfColumn= 1; /* not really */

    lj.ljBodySectNode= bodySectNode;

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljContext= lc;
    lj.ljChangedNode= tree->dtRoot;

    if  ( startTreeLayout 				&&
	  (*startTreeLayout)( &lj, tree, page, column )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docLayoutGetInitialFrame( &bf, &lj, &lpHere, tree->dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docLayoutAdjustFrame( &bf, tree->dtRoot );

    if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
						tree->dtRoot, &bf, &lj ) )
	{ LDEB(1); rval= -1; goto ready; }

    tree->dtPageFormattedFor= page;
    tree->dtColumnFormattedFor= column;
    tree->dtY0UsedTwips= y0Twips;
    tree->dtY1UsedTwips= lpHere.lpPageYTwips;

    if  ( adjustDocument			&&
	  lpHere.lpPageYTwips != oldY1Used	)
	{
	const DocumentGeometry * dgSect= &(bodySectNode->biSectDocumentGeometry);
	struct BufferItem *	bodyNode;
	struct BufferDocument *	bd= lj.ljContext->lcDocument;

	if  ( docIsHeaderType( tree->dtRoot->biTreeType ) )
	    {
	    docPlaceHeader( tree, dgSect );
	    }

	if  ( docIsFooterType( tree->dtRoot->biTreeType ) )
	    {
	    docPlaceFooter( tree, dgSect );
	    }

	if  ( docGetTreeOfNode( (struct DocumentTree **)0,
				&bodyNode, bd, tree->dtRoot ) )
	    { LDEB(tree->dtRoot->biTreeType); rval= -1; goto ready; }
	if  ( ! bodyNode )
	    { bodyNode= bd->bdBody.dtRoot;	}
	if  ( docAdjustLayoutToChangedTree( tree, bodyNode, &lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanLayoutJob( &lj );
    docCleanRecalculateFields( &rf );

    return rval;
    }

