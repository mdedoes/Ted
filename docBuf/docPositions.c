/************************************************************************/
/*									*/
/*  Set DocumentPosition and EditPosition values.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docSelect.h"
#   include	<docEditPosition.h>
#   include	<docEditRange.h>
#   include	"docNodeTree.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  From DocumentPosition to EditPosition.				*/
/*									*/
/************************************************************************/

void docSetEditPosition(	EditPosition *			ep,
				const DocumentPosition *	dp )
    {
    ep->epParaNr= docNumberOfParagraph( dp->dpNode );
    ep->epStroff= dp->dpStroff;
    }

void docSetEditRange(		EditRange *			er,
				const DocumentSelection *	ds )
    {
    docSetEditPosition( &(er->erHead), &(ds->dsHead) );
    docSetEditPosition( &(er->erTail), &(ds->dsTail) );
    }

void docSetIBarRange(		EditRange *			er,
				const DocumentPosition *	dp )
    {
    docSetEditPosition( &(er->erHead), dp );
    er->erTail= er->erHead;
    }

int docComparePositions(	const DocumentPosition *	dp1,
				const DocumentPosition *	dp2 )
    {
    int		n1;
    int		n2;

    n1= docNumberOfParagraph( dp1->dpNode );
    n2= docNumberOfParagraph( dp2->dpNode );

    if  ( n1 > n2 )
	{ return  1;	}
    if  ( n1 < n2 )
	{ return -1;	}

    if  ( dp1->dpStroff > dp2->dpStroff )
	{ return  1;	}
    if  ( dp1->dpStroff < dp2->dpStroff )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the position of a node compared to a selection.		*/
/*									*/
/*  Return negative values If the content of the node completely is	*/
/*	before the selection.						*/
/*  Return  0 If there is some overlap between the node and the		*/
/*	selection. Also return 0 if there is no selection. As all	*/
/*	callers would have to implement this, it can be done here as	*/
/*	well.								*/
/*  Return positive values If the content of the node completely is	*/
/*	after the selection.						*/
/*									*/
/************************************************************************/

int docCompareNodePositionToSelection(	const struct BufferItem *	node,
					const DocumentSelection *	ds )
    {
    if  ( ds )
	{
	DocumentPosition	dp;

	if  ( docTailPosition( &dp, (struct BufferItem *)node ) )
	    { LDEB(1); return 0;	}
	if  ( docComparePositions( &dp, &(ds->dsHead) ) < 0 )
	    { return -1;	}

	if  ( docHeadPosition( &dp, (struct BufferItem *)node ) )
	    { LDEB(1); return 0;	}
	if  ( docComparePositions( &(ds->dsTail), &dp ) < 0 )
	    { return  1;	}
	}

    return 0;
    }

int docCompareCellPositionToSelection(
				int *				pSelected,
				const struct BufferItem *	cellNode,
				const DocumentSelection *	ds )
    {
    int			col= cellNode->biNumberInParent;
    int			cmp= docCompareNodePositionToSelection( cellNode, ds );

    if  ( cmp != 0 )
	{ *pSelected= 0; return cmp;	}

    if  ( ! ds							||
	  ( ( ds->dsCol0 < 0 || col >= ds->dsCol0 )	&&
	    ( ds->dsCol1 < 0 || col <= ds->dsCol1 )	)	)
	{ *pSelected= 1;	}
    else{ *pSelected= 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  From EditPosition to DocumentPosition.				*/
/*									*/
/************************************************************************/

int docPositionForEditPosition(	DocumentPosition *		dpNew,
				const EditPosition *		ep,
				const struct DocumentTree *	tree )
    {
    struct BufferItem *		paraNode;

    if  ( ! tree )
	{ XDEB(tree); return -1;	}

    paraNode= docGetParagraphByNumber( tree, ep->epParaNr );
    if  ( ! paraNode )
	{ LXDEB(ep->epParaNr,paraNode); return -1;	}
    if  ( ep->epStroff > docParaStrlen( paraNode ) )
	{
	LLDEB(ep->epStroff,docParaStrlen(paraNode));
	if  ( docTailPosition( dpNew, paraNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docSetDocumentPosition( dpNew, paraNode, ep->epStroff );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a DocumentPosition.						*/
/*									*/
/************************************************************************/

int docSetDocumentPosition(	DocumentPosition *	dp,
				struct BufferItem *	node,
				int			stroff )
    {
    dp->dpNode= node;
    dp->dpStroff= stroff;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialise a DocumentPosition.					*/
/*									*/
/************************************************************************/

void docInitDocumentPosition(	DocumentPosition *	dp )
    {
    dp->dpNode= (struct BufferItem *)0;
    dp->dpStroff= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an EditRange to a selection for a particular document.	*/
/*									*/
/************************************************************************/

int docSelectionForEditPositionsInTree(	DocumentSelection *		dsNew,
					const struct DocumentTree *	tree,
					const EditPosition *		epHead,
					const EditPosition *		epTail )
    {
    DocumentPosition		dpHead;
    DocumentPosition		dpTail;

    if  ( docPositionForEditPosition( &dpHead, epHead, tree ) )
	{ LDEB(1); return -1;	}
    if  ( docPositionForEditPosition( &dpTail, epTail, tree ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( dsNew, &dpHead, &dpTail, 1 );

    return 0;
    }

int docSelectionForEditPositionsInDoc(	DocumentSelection *		dsNew,
					const struct BufferDocument *	bd,
					const SelectionScope *		ss,
					const EditPosition *		epHead,
					const EditPosition *		epTail )
    {
    struct DocumentTree *	tree;
    struct BufferItem *		bodySectNode;

    if  ( docGetRootOfSelectionScope( &tree, &bodySectNode, 
					(struct BufferDocument *)bd, ss ) )
	{ LDEB(1); return -1;	}

    if  ( docSelectionForEditPositionsInTree( dsNew, tree, epHead, epTail ) )
	{ LDEB(1); return -1;	}

#   if 0
    TODO: Must be done while setting the selection.

    {
    TableRectangle		tr;

    if  ( ! docSelectionInsideCell( dsNew )	&&
	  ! docGetTableRectangle( &tr, dsNew )	)
	{
	struct BufferItem *	selParentNode;

	if  ( docTableRectangleSelection( dsNew, &selParentNode,
					(struct BufferDocument *)bd, &tr ) )
	    { LDEB(1);	}
	}
    }
#   endif

    return 0;
    }

