/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"docParagraphLayout.h"
#   include	"layoutContext.h"
#   include	"docStripLayoutJob.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docBreakKind.h>
#   include	<docRowProperties.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Place successive nodes, after the predecessor. This routine is	*/
/*  called for the tail of the document after a real formatting change.	*/
/*									*/
/*  Exceptions are covered by the caller: docAdjustParentLayout()	*/
/*  or by the calls to docLayoutNodeImplementation(). The only added	*/
/*  knowledge here is that as long the stacking of items ends up in the	*/
/*  same position, subsequent siblings will not change either.		*/
/*  The only exception to this rule: A change to a table header is	*/
/*  covered by another exception: Children of a section are handled by	*/
/*  docLayoutSectNode()/docLayoutSectChildren().			*/
/*									*/
/************************************************************************/

static int docReLayoutStackedChildren(
				LayoutPosition *		lpBelow,
				const LayoutPosition *		lpTop,
				struct BufferItem *		parentNode,
				int				from,
				BlockFrame *			bf,
				LayoutJob *			lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    int				i;
    int				changed;
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;

    LayoutPosition		lpHere= *lpTop;

    for ( i= from; i < parentNode->biChildCount; i++ )
	{
	struct BufferItem *	child= parentNode->biChildren[i];

	if  ( DOC_SAME_POSITION( &(child->biTopPosition), &lpHere ) )
	    {
	    const struct BufferItem *	last;

	    last= parentNode->biChildren[parentNode->biChildCount- 1];
	    lpHere= last->biBelowPosition;
	    break;
	    }

	child->biTopPosition= lpHere;

	if  ( docLayoutNodeImplementation( &lpHere, &lpHere, child, bf, lj ) )
	    { LDEB(i); return -1;	}

	docLayoutSetNodeBottom( &changed, child, &lpHere, lc, drChanged );
	}

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Reformat the document body because a header/footer/note changed	*/
/*  size.								*/
/*									*/
/************************************************************************/

static int docRedoBodyLayout(		struct BufferItem *	bodyNode,
					const LayoutJob *	ljRef )
    {
    LayoutJob			bodyLj;

    if  ( bodyNode->biTreeType != DOCinBODY )
	{ LDEB(bodyNode->biTreeType);	}

    bodyLj= *ljRef;
    /* bodyLj.ljContext= ljRef->ljContext; */

    bodyLj.ljChangedNode= bodyNode;

    if  ( docLayoutNodeAndParents( bodyNode, &bodyLj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docRedoRowLayout(	LayoutPosition *	lpHere,
				struct BufferItem *	cellNode,
				struct BufferItem *	rowNode,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    const RowProperties *	rp;
    int				keepRowOnPage;
    int				rowspan= 1;

    rp= rowNode->biRowProperties;
    keepRowOnPage= RP_IS_ONE_PAGE( rp );

    if  ( rowNode->biNumberInParent > 0	&&
	  rowNode->biParent		&&
	  keepRowOnPage			)
	{
	struct BufferItem *	pp= rowNode->biParent;
	struct BufferItem *	ch;

	ch= pp->biChildren[rowNode->biNumberInParent- 1];

	rowNode->biTopPosition= ch->biBelowPosition;
	}

    *lpHere= rowNode->biTopPosition;

    if  ( bf->bfPage != lpHere->lpPage		||
	  bf->bfColumn != lpHere->lpColumn	)
	{
	if  ( docLayoutGetInitialFrame( bf, lj, lpHere, rowNode ) )
	    { LDEB(1); return -1;	}
	}

    /*  Continue with subsequent rows after a row with a rowspan.	*/
    if  ( cellNode->biCellRowspan > 1 )
	{ rowspan= cellNode->biCellRowspan;	}

    if  ( docLayoutTableSlice( lpHere, lpHere, rowNode->biParent,
				rowNode->biNumberInParent,
				rowNode->biNumberInParent+ rowspan, bf, lj ) )
	{ LDEB(rowspan); return -1;	}

    return rowspan- 1;
    }

/************************************************************************/
/*									*/
/*  Fixup routine that is called when the first child item does not	*/
/*  have the same top position as its parent.				*/
/*									*/
/*  This can happen when during a reformat of part of the document	*/
/*  the child moves to a different page. E.G: When a property change	*/
/*  of a paragraph makes it higher or lower.				*/
/*									*/
/************************************************************************/

static int docPsFixupParentGeometry(	struct BufferItem *	node,
					struct BufferItem *	parentNode )
    {
    LayoutPosition	lpNode= node->biTopPosition;
    LayoutPosition	lpPa= parentNode->biTopPosition;

    if  ( ! DOC_SAME_POSITION( &lpPa, &lpNode ) )
	{
#	if 1
	SSDEB(docLevelStr(parentNode->biLevel),docLevelStr(node->biLevel));
	LLDEB(lpPa.lpPage,lpNode.lpPage);
	LLDEB(lpPa.lpPageYTwips,lpNode.lpPageYTwips);
#	endif

	parentNode->biTopPosition= lpNode;

	return 1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the position in the document and the position on the page	*/
/*  to start adjusting the layout of a paragraph.			*/
/*									*/
/************************************************************************/

static int docAdjustParaParentLayout(
				LayoutPosition *		lpHere,
				const struct BufferItem *	node,
				const struct BufferItem *	parentNode,
				struct LayoutJob *		lj )
    {
    int			from;
    ParagraphLayoutJob	plj;
    const int		line= 0;
    const int		part= 0;

    docInitParagraphLayoutJob( &plj );

    docBeginParagraphLayoutProgress( &plj, lj,
				node->biNumberInParent+ 1, line, part,
				parentNode->biChildCount, lpHere );

    docFindStripLayoutOrigin( &plj, lpHere, parentNode );

    if  ( node->biParaProperties->ppBreakKind != DOCibkNONE	&&
	  node->biParaProperties->ppTableNesting == 0		)
	{
	const struct BufferItem *	nde= node;

	while( nde->biNumberInParent == 0 )
	    {
	    if  ( ! nde->biParent )
		{ break;	}

	    nde->biParent->biTopPosition= node->biTopPosition;
	    nde= nde->biParent;
	    }
	}

    if  ( plj.pljPos0.pspChild >= node->biNumberInParent+ 1 )
	{
	if  ( plj.pljPos0.pspChild > node->biNumberInParent+ 1 )
	    { LLDEB(plj.pljPos0.pspChild,node->biNumberInParent);	}

	from= node->biNumberInParent+ 1;
	}
    else{
	from= plj.pljPos0.pspChild;

	if  ( from == 0 )
	    { *lpHere= parentNode->biChildren[from]->biTopPosition;	}
	else{ *lpHere= parentNode->biChildren[from- 1]->biBelowPosition; }
	}

    docCleanParagraphLayoutJob( &plj );

    return from;
    }

/************************************************************************/
/*									*/
/*  The height of an external tree changed: This possibly makes it	*/
/*  necessary to recalculate the layout of the document.		*/
/*									*/
/*  Do so if necessary.							*/
/*									*/
/************************************************************************/

int docAdjustLayoutToChangedTree(	struct DocumentTree *	tree,
					struct BufferItem *	bodyNode,
					LayoutJob *		lj )
    {
    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    lj->ljReachedDocumentBottom= 1;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    if  ( lj->ljChangedNode->biTreeType == DOCinBODY )
		{ break; }

	    if  ( docRedoBodyLayout( bodyNode, lj ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    if  ( lj->ljChangedNode->biTreeType == DOCinBODY )
		{ break; }

	    if  ( docRedoBodyLayout( bodyNode, lj ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( lj->ljChangedNode->biTreeType == DOCinBODY )
		{ break; }

	    if  ( docRedoBodyLayout( bodyNode, lj ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinSHPTXT:
	    /*nothing*/
	    break;

	default:
	    LDEB(tree->dtRoot->biTreeType);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the geometry of a parent node to changes in a child.		*/
/*									*/
/*  This actually is a full layout action of everything below the	*/
/*  recently reformatted part. There are two differences:		*/
/*  a)  Reformatting stops when a buffer item lands in the same		*/
/*	position where it was before the change that forced us to redo	*/
/*	part of the layout.						*/
/*  b)  We try not to recalculate the layout of the text inside the	*/
/*	individual text lines.						*/
/*									*/
/************************************************************************/

int docAdjustParentLayout(	const LayoutPosition *	lpTop,
				struct BufferItem *	node,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    struct BufferItem *		parentNode= node->biParent;
    int				from;

    LayoutPosition		lpHere= *lpTop;


if(bf->bfPage!=lpHere.lpPage||
   bf->bfColumn!=lpHere.lpColumn)
  {
  SDEB("!!!!!!!!!!!!!!");
  LLDEB(bf->bfPage,lpHere.lpPage);
  LLDEB(bf->bfColumn,lpHere.lpColumn);
  }

    if  ( parentNode && parentNode->biLevel == DOClevCELL )
	{ from= docAdjustParaParentLayout( &lpHere, node, parentNode, lj );	}
    else{ from= node->biNumberInParent+ 1;				}

    while( parentNode )
	{
	int		changed= 0;

	switch( parentNode->biLevel )
	    {
	    case DOClevBODY:
		if  ( node->biNumberInParent == 0		&&
		      docPsFixupParentGeometry( node, parentNode )	)
		    { from= 0; lpHere= node->biTopPosition; }

		if  ( from <= parentNode->biChildCount- 1 )
		    {
		    docReLayoutStackedChildren( &lpHere, &lpHere,
						    parentNode, from, bf, lj );
		    }
		else{ lpHere= node->biBelowPosition;		}

		if  ( parentNode->biTreeType == DOCinBODY		&&
		      npEndnotes->npPlacement == FTNplaceDOC_END	)
		    {
		    if  ( docLayoutEndnotesForDocument( &lpHere, &lpHere,
								    bf, lj ) )
			{ LDEB(1); return -1;	}
		    }

		break;

	    case DOClevSECT:
		if  ( node->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( node, parentNode )	)
		    { from= 0; lpHere= node->biTopPosition; }

		if  ( from <= parentNode->biChildCount- 1 )
		    {
		    docLayoutSectChildren( &lpHere, &lpHere,
						    parentNode, from, bf, lj );
		    }
		else{ lpHere= node->biBelowPosition;		}

		if  ( docLayoutFinishSectNode( &lpHere, &lpHere,
							parentNode, bf, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    parentAsGroup:
		if  ( node->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( node, parentNode )	)
		    { from= 0; lpHere= node->biTopPosition; }

		if  ( from <= parentNode->biChildCount- 1 )
		    {
		    docReLayoutStackedChildren( &lpHere, &lpHere,
						    parentNode, from, bf, lj );
		    }
		else{ lpHere= node->biBelowPosition;		}

		break;

	    case DOClevCELL:
		if  ( node->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( node, parentNode )	)
		    { from= 0; lpHere= node->biTopPosition; }

		if  ( from <= parentNode->biChildCount- 1 )
		    {
		    if  ( docLayoutCellNode( &lpHere, &lpHere,
						    parentNode, from, bf, lj ) )
			{ LDEB(1); return -1;	}
		    }
		else{ lpHere= node->biBelowPosition;		}

		break;

	    case DOClevROW:

		if  ( ! docIsRowNode( parentNode ) )
		    {
		    if  ( parentNode->biNumberInParent > 0 )
			{
			docLayoutCalculateAfterRowTopInset( parentNode, bd );
			}

		    goto parentAsGroup;
		    }
		else{
		    int		extra;

		    extra= docRedoRowLayout( &lpHere,
					    node, parentNode, bf, lj );
		    if  ( extra < 0 )
			{ LDEB(extra); return -1;	}
		    if  ( extra > 0 )
			{
			parentNode= parentNode->biParent->biChildren[
					parentNode->biNumberInParent+ extra];
			}
		    }

		break;

	    default:
		LDEB(parentNode->biLevel); return -1;
	    }

	docLayoutSetNodeBottom( &changed, parentNode, &lpHere, lc, drChanged );

	if  ( ! changed )
	    { break;	}

	node= parentNode;
	parentNode= node->biParent;
	from= node->biNumberInParent+ 1;
	}

    if  ( node->biTreeType == DOCinBODY		&&
	  BF_HAS_FOOTNOTES( bf )		&&
	  ! bf->bfFootnotesPlaced		)
	{
	const int		belowText= 0;
	LayoutPosition		lpBelowNotes;

	if  ( docLayoutFootnotesForColumn( &lpBelowNotes, &lpHere, bf,
							    belowText, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( ! parentNode )
	{
	DocumentTree *		tree;
	struct BufferItem *	bodyNode;

	if  ( docGetTreeOfNode( &tree, &bodyNode, bd, node ) )
	    { LDEB(node->biTreeType); return -1; }
	if  ( ! bodyNode )
	    { bodyNode= bd->bdBody.dtRoot;	}

	if  ( lpHere.lpPageYTwips != tree->dtY1UsedTwips		&&
	      docAdjustLayoutToChangedTree( tree, bodyNode, lj )	)
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

