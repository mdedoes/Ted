/************************************************************************/
/*									*/
/*  Layout of node in a document.					*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	"docParagraphLayout.h"
#   include	"docStripLayoutJob.h"
#   include	"layoutContext.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDocumentProperties.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  layout a paragraph or a cell: Both are shortcuts to			*/
/*  docLayoutStackedStrip(): trkeep and widow/orphan control makes the	*/
/*  formatting of paragraphs dependent on eachother.			*/
/*									*/
/************************************************************************/

static int docLayoutParaNode(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				struct BufferItem *	paraNode,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int				rval= 0;
    ParagraphLayoutJob		plj;
    const int			line= 0;
    const int			part= 0;

    docInitParagraphLayoutJob( &plj );

    docBeginParagraphLayoutProgress( &plj, lj,
			paraNode->biNumberInParent, line, part,
			paraNode->biNumberInParent+ 1,
			lpTop );

    if  ( docLayoutStackedStrip( paraNode->biParent, bf, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( paraNode->biParaLineCount < 1 )
	{
	LDEB(paraNode->biParaLineCount);
	docListNode(lj->ljContext->lcDocument,0,paraNode,0);
	rval= -1; goto ready;
	}

    *lpBelow= plj.pljPos.plpPos;

  ready:

    docCleanParagraphLayoutJob( &plj );

    return rval;
    }

int docLayoutCellNode(	LayoutPosition *	lpBelow,
			const LayoutPosition *	lpTop,
			struct BufferItem *	cellNode,
			int			from,
			BlockFrame *		bf,
			LayoutJob *		lj )
    {
    int				rval= 0;
    ParagraphLayoutJob		plj;
    const int			line= 0;
    const int			part= 0;

    docInitParagraphLayoutJob( &plj );

    docBeginParagraphLayoutProgress( &plj, lj,
				from, line, part, cellNode->biChildCount,
				lpTop );

    if  ( docLayoutStackedStrip( cellNode, bf, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *lpBelow= plj.pljPos.plpPos;

  ready:

    docCleanParagraphLayoutJob( &plj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a section because one of its external items	*/
/*  changed size.							*/
/*									*/
/************************************************************************/

static void docNodeLayoutStartPosition(	LayoutPosition *		lp,
					const struct BufferItem *	node )
    {
    if  ( node->biNumberInParent == 0 )
	{
	if  ( node->biParent )
	    { *lp= node->biParent->biTopPosition;	}
	else{
	    if  ( node->biTreeType == DOCinBODY )
		{ docInitLayoutPosition( lp );		}
	    else{ *lp= node->biTopPosition;		}
	    }
	}
    else{
	const struct BufferItem *	prevNode;

	prevNode= node->biParent->biChildren[node->biNumberInParent- 1];

	*lp= prevNode->biBelowPosition;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Do the layout of the document root item.				*/
/*									*/
/************************************************************************/

static int docLayoutBodyNode(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				struct BufferItem *	docNode,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    LayoutPosition		lpHere= *lpTop;

    int				i;

    if  ( docNode->biChildCount > 0 )
	{
	lj->ljBodySectNode= docNode->biChildren[0];

	if  ( docTreePrelayout( &(bd->bdFtnsep), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdFtnsepc), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdFtncn), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}

	if  ( docTreePrelayout( &(bd->bdAftnsep), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdAftnsepc), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdAftncn), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	}

    for ( i= 0; i < docNode->biChildCount; i++ )
	{
	lj->ljBodySectNode= docNode->biChildren[i];

	if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
					    docNode->biChildren[i], bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	if  ( docLayoutEndnotesForDocument( &lpHere, &lpHere, bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a node that consists of a series of stacked	*/
/*  children.								*/
/*									*/
/************************************************************************/

static int docLayoutStackedNodeChildren(
				LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				struct BufferItem *	node,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    LayoutPosition		lpHere= *lpTop;
    int				child;

    for ( child= 0; child < node->biChildCount; child++ )
	{
	if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
					node->biChildren[child], bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( node->biChildCount > 0 )
	{ node->biTopPosition= node->biChildren[0]->biTopPosition; }

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a single row node. This is to support the layout	*/
/*  of any node in the document tree. We cover a few special cases that	*/
/*  relate to trkeep and trkeepnext. It is not the intention of this	*/
/*  code to cover the full logic of table formatting: To cover that,	*/
/*  invoke docLayoutTableSlice() for the appropriate table slice rather	*/
/*  than docLayoutNode() for a particular row node.			*/
/*									*/
/*  1)  Regular code for a row.						*/
/*									*/
/************************************************************************/

static int docLayoutSingleRowNode(	LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					struct BufferItem *	rowNode,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    int			stopCode= FORMATstopREADY;
    const int		inNewFrame= rowNode->biRowPrecededByHeader;
    const int		startInThisColumn= 0;

    LayoutPosition	lpHere= *lpTop;

    /*  1  */
    if  ( docLayoutRowNode( &stopCode, &lpHere, &lpHere,
		    rowNode, bf, inNewFrame, startInThisColumn, lj ) )
	{ LDEB(1); return -1;	}

    *lpBelow= lpHere;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a node in the document tree.			*/
/*									*/
/*  This is the main entry poin of the formatter.			*/
/*									*/
/*  1)  While balancing columns in the last page of a section.. Do not	*/
/*	reformat nodes that are irrelevant because they are on an	*/
/*	earlier page.							*/
/*									*/
/************************************************************************/

int docLayoutNodeImplementation(	LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					struct BufferItem *	node,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    const LayoutContext *	lc= lj->ljContext;

    LayoutPosition		lpHere= *lpTop;

    /*  1  */
    if  ( lj->ljBalancePage >= 0				&&
	  node->biBelowPosition.lpPage < lj->ljBalancePage	)
	{ *lpBelow= node->biBelowPosition; return 0;	}

    docLayoutStartNodeLayout( node, lj, &lpHere );

    switch( node->biLevel )
	{
	case DOClevBODY:

	    if  ( docLayoutBodyNode( &lpHere, &lpHere, node, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docLayoutCellNode( &lpHere, &lpHere, node, 0, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( ! node->biParent ) /* IE A header, footer, note etc */
		{
		if  ( docLayoutStackedNodeChildren(
					&lpHere, &lpHere, node, bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( docLayoutSectNode( &lpHere, &lpHere, node, bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( ! docIsRowNode( node ) )
		{
		if  ( node->biNumberInParent > 0 )
		    {
		    docLayoutCalculateAfterRowTopInset( node, lc->lcDocument );
		    }

		if  ( docLayoutStackedNodeChildren(
					&lpHere, &lpHere, node, bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( docLayoutSingleRowNode( &lpHere, &lpHere, node, bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( docLayoutParaNode( &lpHere, &lpHere, node, bf, lj ) )
		{ LDEB(1); return -1;	}

	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    docLayoutFinishNodeLayout( (int *)0, node, lj, &lpHere );

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout for a block in the document hierarchy.	*/
/*									*/
/*  1)  If we do not have to recalculate the layout of the document as	*/
/*	a whole..							*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  If the preceding paragraph ends on the same page where this	*/
/*	nodes begins, reserve space for the footnotes upto the		*/
/*	beginning of this block and subtract the height from the buttom	*/
/*	of the frame.							*/
/*  4)  Perform the actual layout operation.				*/
/*  5)  Adjust the positions of the parent nodes and any children of	*/
/*	the parent below this node.					*/
/*									*/
/************************************************************************/

int docLayoutNodeAndParents(	struct BufferItem *	node,
				LayoutJob *		lj )
    {
    int				rval= 0;
    BlockFrame			bf;
    LayoutPosition		lpHere;
    const LayoutContext *	lc= lj->ljContext;
    const struct BufferItem *	bodyNode= lc->lcDocument->bdBody.dtRoot;
    const struct BufferItem *	sectNode= docGetSectNode( node );

    docLayoutInitBlockFrame( &bf );

    docNodeLayoutStartPosition( &lpHere, node );

    if  ( node->biTreeType == DOCinBODY )
	{
	if  ( ! sectNode )
	    { sectNode= bodyNode->biChildren[0];		}

	lj->ljBodySectNode= sectNode;
	}
    else{
	const struct BufferItem *		bodySectNode;

	if  ( ! sectNode )
	    { XDEB(sectNode); bodySectNode= bodyNode->biChildren[0];	}
	else{
	    const SelectionScope *	ss= &(sectNode->biSectSelectionScope);

	    bodySectNode= bodyNode->biChildren[ss->ssOwnerSectNr];
	    }

	lj->ljBodySectNode= bodySectNode;
	}

    /*  1  */
    if  ( node->biLevel != DOClevBODY				&&
	  docLayoutGetInitialFrame( &bf, lj, &lpHere, node )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  4  */
    if  ( docLayoutNodeImplementation( &lpHere, &lpHere, node, &bf, lj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  5  */
    if  ( docAdjustParentLayout( &lpHere, node, &bf, lj ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }
