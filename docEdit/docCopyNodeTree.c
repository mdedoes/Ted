/************************************************************************/
/*									*/
/*  Buffer administration routines. Functionality related to the node	*/
/*  tree.								*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docRowNodeProperties.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

#   define	VALIDATE_TREE	0

/************************************************************************/

static struct BufferItem * docCopyXNode(
				DocumentCopyJob *		dcj,
				const SelectionScope *		ssRoot,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	nodeFrom );

/************************************************************************/

static int docCopyXNodeChilden(	DocumentCopyJob *	dcj,
				const SelectionScope *	ssRoot,
				struct BufferItem *	parentNodeTo,
				int			to,
				const struct BufferItem *	parentNodeFrom,
				int			from,
				int			n )
    {
    int		i;

    if  ( parentNodeTo->biLevel == DOClevSECT )
	{ ssRoot= &(parentNodeTo->biSectSelectionScope);	}

    for ( i= 0; i < n; i++ )
	{
	const struct BufferItem *	childFrom= parentNodeFrom->biChildren[from];

	if  ( ! docCopyXNode( dcj, ssRoot, parentNodeTo, to, childFrom ) )
	    { LDEB(i); return -1;	}

	/*  Compensate for the shift.	*/
	if  ( parentNodeTo == parentNodeFrom	&&
	      to <= from			)
	    { from++;	}

	from++; to++;
	}

    return 0;
    }


int docCopyNodeChildren(	DocumentCopyJob *	dcj,
				struct BufferItem *		parentNodeTo,
				int			to,
				const struct BufferItem *	parentNodeFrom,
				int			from,
				int			n )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    const SelectionScope *	ssRoot= &(eo->eoSelectionScope);

    return docCopyXNodeChilden( dcj, ssRoot,
				parentNodeTo, to, parentNodeFrom, from, n );
    }

/************************************************************************/

static struct BufferItem * docCopyCellNode(
				DocumentCopyJob *		dcj,
				const SelectionScope *		ssRoot,
				struct BufferItem *		rowNodeTo,
				int				colTo,
				const struct BufferItem *	cellNodeFrom,
				int				copyProperties )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;

    struct BufferItem *	cellNodeTo;
    int			child;

    cellNodeTo= docInsertNode( bdTo, rowNodeTo, colTo, DOClevCELL );
    if  ( ! cellNodeTo )
	{ XDEB(cellNodeTo); return (struct BufferItem *)0;	}

    if  ( copyProperties )
	{
	const struct BufferItem *	rowNodeFrom= cellNodeFrom->biParent;
	int				colFrom= cellNodeFrom->biNumberInParent;

	if  ( docInsertColumnProperties( rowNodeTo, rowNodeFrom,
			    colTo, colFrom, bdTo, &(dcj->dcjAttributeMap) ) )
	    { LLDEB(colTo,colFrom); return (struct BufferItem *)0;	}
	}

    for ( child= 0; child < cellNodeFrom->biChildCount; child++ )
	{
	struct BufferItem *	childNode= cellNodeFrom->biChildren[child];

	if  ( ! docCopyXNode( dcj, ssRoot, cellNodeTo, child, childNode ) )
	    { LDEB(child); return (struct BufferItem *)0;	}
	}

    return cellNodeTo;
    }

/************************************************************************/
/*									*/
/*  Copy a row node. As an optimisation, we copy the cell properties	*/
/*  at once.								*/
/*									*/
/*  8)  Force the redo of the per cell administration.			*/
/*									*/
/************************************************************************/

static struct BufferItem * docCopyRowNode(
				DocumentCopyJob *		dcj,
				const SelectionScope *		ssRoot,
				struct BufferItem *		sectNodeTo,
				int				n,
				const struct BufferItem *	rowNodeFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;

    struct BufferItem *	rowNodeTo;
    int			col;

    rowNodeTo= docInsertNode( bdTo, sectNodeTo, n, DOClevROW );
    if  ( ! rowNodeTo )
	{ XDEB(rowNodeTo); return rowNodeTo;	}

    if  ( docCopyRowNodeProperties( rowNodeTo, rowNodeFrom,
					    bdTo, &(dcj->dcjAttributeMap ) ) )
	{ LDEB(1); return (struct BufferItem *)0;	}

    for ( col= 0; col < rowNodeFrom->biChildCount; col++ )
	{
	struct BufferItem *	cellNodeTo;
	const int		copyCellProperties= 0;

	cellNodeTo= docCopyCellNode( dcj, ssRoot, rowNodeTo, col,
			    rowNodeFrom->biChildren[col], copyCellProperties );
	if  ( ! cellNodeTo )
	    { XDEB(cellNodeTo); return (struct BufferItem *)0;	}
	}

    /*  8  */
    if  ( docSetRowNodeProperties( rowNodeTo,
				    rowNodeTo->biRowProperties, bdTo ) )
	{ LDEB(1);	}

    return rowNodeTo;
    }

/************************************************************************/
/*									*/
/*  Copy a section.							*/
/*									*/
/************************************************************************/

static struct BufferItem * docCopySectNode(
			DocumentCopyJob *		dcj,
			const SelectionScope *		ssRoot,
			struct BufferItem *		parentNodeTo,
			int				n,
			const struct BufferItem *	sectNodeFrom )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;

    struct BufferItem *	rval= (struct BufferItem *)0;
    struct BufferItem *	sectNodeTo= (struct BufferItem *)0;

    if  ( parentNodeTo )
	{
	if  ( parentNodeTo->biTreeType != ssRoot->ssTreeType )
	    {
	    LDEB(parentNodeTo->biTreeType);
	    LDEB(ssRoot->ssTreeType);
	    goto ready;
	    }

	sectNodeTo= docInsertNode( bdTo, parentNodeTo, n, DOClevSECT );
	if  ( ! sectNodeTo )
	    { XDEB(sectNodeTo); goto ready;	}
	}
    else{
	n= 0;
	sectNodeTo= (struct BufferItem *)malloc( sizeof(struct BufferItem) );

	if  ( ! sectNodeTo )
	    { XDEB(sectNodeTo); goto ready;	}

	docInitNode( sectNodeTo, (struct BufferItem *)0, bdTo, n,
				DOClevSECT, ssRoot->ssTreeType );
	}

    sectNodeTo->biSectSelectionScope= *ssRoot;

    if  ( docCopySectDescription( sectNodeTo, bdTo,
				    sectNodeFrom, dcj->dcjSourceDocument ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopyNodeChildren( dcj, sectNodeTo, 0,
			    sectNodeFrom, 0, sectNodeFrom->biChildCount ) )
	{ LDEB(1); goto ready;	}

    if  ( ! dcj->dcjInExternalTree && parentNodeTo )
	{
	DocumentPosition		dp;

	if  ( docTailPosition( &dp, sectNodeTo ) )
	    { LDEB(1);	}
	else{
	    int		paraNr;
	    const int	stroff= 0;
	    const int	sectShift= 1;
	    const int	paraShift= 0;

	    paraNr= docNumberOfParagraph( dp.dpNode )+ 1;

	    docEditShiftReferences( eo, ssRoot, paraNr, stroff,
					    sectShift, paraShift, -stroff );
	    }
	}

    rval= sectNodeTo; sectNodeTo= (struct BufferItem *)0; /* steal */

  ready:

    if  ( sectNodeTo )
	{ docDeleteNode( eo->eoDocument, eo->eoTree, sectNodeTo );	}

    return rval;
    }

static struct BufferItem * docCopyXNode(
				DocumentCopyJob *		dcj,
				const SelectionScope *		ssRoot,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	nodeFrom )
    {
    const int		copyCellProperties= 1;
    struct BufferItem *	res;

    switch( nodeFrom->biLevel )
	{
	case DOClevPARA:
	    res= docCopyParaNode( dcj, ssRoot, parentNodeTo, n, nodeFrom );
	    if  ( ! res )
		{ LXDEB(nodeFrom->biLevel,res);	}
	    return res;

	case DOClevCELL:
	    res= docCopyCellNode( dcj, ssRoot, parentNodeTo, n, nodeFrom,
							copyCellProperties );
	    if  ( ! res )
		{ LXDEB(nodeFrom->biLevel,res);	}
	    return res;

	case DOClevROW:
	    res= docCopyRowNode(  dcj, ssRoot, parentNodeTo, n, nodeFrom );
	    if  ( ! res )
		{ LXDEB(nodeFrom->biLevel,res);	}
	    return res;

	case DOClevSECT:
	    res= docCopySectNode( dcj, ssRoot, parentNodeTo, n, nodeFrom );
	    if  ( ! res )
		{ LXDEB(nodeFrom->biLevel,res);	}
	    return res;

	default:
	    LDEB(nodeFrom->biLevel); return (struct BufferItem *)0;
	}
    }

struct BufferItem * docCopyNode( DocumentCopyJob *		dcj,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	nodeFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    const SelectionScope *	ssRoot= &(eo->eoSelectionScope);
    struct BufferItem *		res;

    res= docCopyXNode( dcj, ssRoot, parentNodeTo, n, nodeFrom );
    if  ( ! res )
	{XDEB(res);	}
    return res;
    }

int docCopyDocumentTree( 	DocumentCopyJob *		dcj,
				struct DocumentTree *		treeTo,
				const SelectionScope *		ssRoot,
				struct DocumentTree *		treeFrom )
    {
    int				rval= 0;
    struct FieldStackLevel *	fcsl= dcj->dcjFieldStack;
    SelectionScope		ssSave= dcj->dcjTargetSelectionScope;
    struct DocumentTree *	targetTreeSave= dcj->dcjTargetTree;
    struct DocumentTree *	sourceTreeSave= dcj->dcjSourceTree;

    dcj->dcjTargetSelectionScope= *ssRoot;
    dcj->dcjTargetTree= treeTo;
    dcj->dcjSourceTree= treeFrom;
    dcj->dcjFieldStack= (struct FieldStackLevel *)0;
    dcj->dcjInExternalTree++;

    treeTo->dtRoot= docCopySectNode( dcj, ssRoot, (struct BufferItem *)0, 0,
							    treeFrom->dtRoot );
    if  ( ! treeTo->dtRoot )
	{ XDEB(treeTo->dtRoot); rval= -1;	}

    if  ( dcj->dcjFieldStack )
	{ XDEB(dcj->dcjFieldStack);	}

    dcj->dcjInExternalTree--;
    dcj->dcjFieldStack= fcsl;
    dcj->dcjTargetTree= targetTreeSave;
    dcj->dcjSourceTree= sourceTreeSave;
    dcj->dcjTargetSelectionScope= ssSave;

    return rval;
    }

