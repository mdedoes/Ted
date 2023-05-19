/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	<docTreeType.h>
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docSelect.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  What struct BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

int docGetTreeOfNode(		struct DocumentTree **		pTree,
				struct BufferItem **		pBodySectNode,
				struct BufferDocument *		bd,
				struct BufferItem *		node )
    {
    struct BufferItem *		selSectNode;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;
    struct DocumentTree *	tree;

    if  ( node->biLevel == DOClevBODY && node->biChildCount > 0 )
	{ selSectNode= node->biChildren[0];	}
    else{
	struct BufferItem *	sectNode= docGetSectNode( node );
	if  ( ! sectNode )
	    {
	    XXDEB(node,sectNode);
	    if  ( node )
		{ docListNode(bd,0,node,0);	}
	    return -1;
	    }

	selSectNode= sectNode;
	}

    if  ( docGetRootOfSelectionScope( &tree, &bodySectNode,
				    bd, &(selSectNode->biSectSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( pTree )
	{ *pTree= tree;			}
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  What struct BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

struct BufferItem * docGetSelectionRoot(
			struct DocumentTree **		pTree,
			struct BufferItem **		pBodySectNode,
			struct BufferDocument *		bd,
			const DocumentSelection *	ds )
    {
    struct BufferItem *	selRootNode= (struct BufferItem *)0;

    if  ( ! docSelectionIsSet( ds ) )
	{
	XXDEB(ds->dsHead.dpNode,ds->dsTail.dpNode);
	return (struct BufferItem *)0;
	}

    selRootNode= docGetCommonParent( ds->dsHead.dpNode, ds->dsTail.dpNode );

    if  ( pTree || pBodySectNode )
	{
	if  ( docGetTreeOfNode( pTree, pBodySectNode, bd, selRootNode ) )
	    { LDEB(1); return (struct BufferItem *)0;	}
	}

    return selRootNode;
    }

/************************************************************************/

struct BufferItem * docGetBodySectNodeOfScope(	const SelectionScope *	ss,
					const struct BufferDocument *	bd )
    {
    struct BufferItem *		sectNode;
    int				sect;

    if  ( ss->ssTreeType == DOCinBODY )
	{ sect= ss->ssSectNr;		}
    else{ sect= ss->ssOwnerSectNr;	}

    if  ( sect < 0 || sect >= bd->bdBody.dtRoot->biChildCount )
	{
	SLLDEB(docTreeTypeStr(ss->ssTreeType),sect,bd->bdBody.dtRoot->biChildCount);
	return (struct BufferItem *)0;
	}

    sectNode= bd->bdBody.dtRoot->biChildren[sect];

    if  ( sectNode->biLevel != DOClevSECT )
	{ LDEB(sectNode->biLevel); return (struct BufferItem *)0;	}

    return sectNode;
    }

struct BufferItem * docGetBodySectNode(	struct BufferItem *		node,
					const struct BufferDocument *	bd )
    {
    int				sect;

    if  ( node->biLevel != DOClevSECT )
	{
	node= docGetSectNode( node );
	if  ( ! node )
	    { XDEB(node); return (struct BufferItem *)0;	}
	}

    if  ( node->biTreeType == DOCinBODY )
	{ sect= node->biNumberInParent;			}
    else{ sect= node->biSectSelectionScope.ssOwnerSectNr;	}

    if  ( sect < 0 || sect >= bd->bdBody.dtRoot->biChildCount )
	{
	LLDEB(sect,bd->bdBody.dtRoot->biChildCount);
	return (struct BufferItem *)0;
	}

    return bd->bdBody.dtRoot->biChildren[sect];
    }

