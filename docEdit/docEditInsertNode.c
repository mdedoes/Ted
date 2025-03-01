/************************************************************************/
/*									*/
/*  Buffer: Insert nodes in the tree.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docNodeTree.h>
#   include	"docEdit.h"
#   include	"docEditOperation.h"
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docParaParticuleAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a fresh paragraph near an existing one.			*/
/*									*/
/************************************************************************/

int docInsertParagraph(		EditOperation *		eo,
				DocumentPosition *	dpNew,
				struct BufferItem *	paraNode,
				int			after,
				int			textAttrNr )
    {
    int			pos= paraNode->biNumberInParent+ ( after != 0 );
    struct BufferItem *	parentNode= paraNode->biParent;

    struct BufferItem *	newParaNode;
    int			paraNr;

    newParaNode= docInsertNode( eo->eoDocument, parentNode, pos, DOClevPARA );
    if  ( ! newParaNode )
	{ XDEB(newParaNode); return -1;	}

    paraNr= docNumberOfParagraph( newParaNode );

    {
    const int	sectShift= 0;
    const int	stroffFrom= 0;
    const int	stroffShift= 0;

    const int	paraShift= 1;

    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr+ 1, stroffFrom,
				    sectShift, paraShift, stroffShift );
    }

    if  ( docEditTransferParaProperties( eo, newParaNode, paraNode, 1,
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    if  ( ! docInsertTextParticule( newParaNode, 0, 0, 0,
					TPkindSPAN, textAttrNr ) )
	{ LDEB(1); return -1;	}

    docEditIncludeNodeInReformatRange( eo, paraNode );
    docEditIncludeNodeInReformatRange( eo, newParaNode );

    if  ( docHeadPosition( dpNew, newParaNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a fresh section near an existing one.			*/
/*									*/
/************************************************************************/

int docInsertSection(		EditOperation *		eo,
				DocumentPosition *	dpBeforeSplit,
				DocumentPosition *	dpAfterSplit,
				struct BufferItem *		paraNode,
				int			split,
				int			after,
				int			textAttrNr )
    {
    struct BufferItem *	sectNode;

    sectNode= docGetSectNode( paraNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); return -1;	}

    if  ( split )
	{
	int	atExtremity;

	if  ( docNodeAtExtremity( &atExtremity, sectNode, paraNode, after ) )
	    { LDEB(after); return -1;	}
	if  ( atExtremity )
	    { split= 0;	}
	}

    if  ( split )
	{
	struct BufferItem *		beforeNode;
	struct BufferItem *		afterNode;

	if  ( docEditSplitParaParent( eo, &beforeNode, &afterNode,
					    paraNode, after, DOClevSECT ) )
	    { LDEB(DOClevSECT); return -1;	}

	if  ( docTailPosition( dpBeforeSplit, beforeNode ) )
	    { LDEB(1); return -1;	}
	if  ( docHeadPosition( dpAfterSplit, afterNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	struct BufferItem *	parentNode;
	struct BufferItem *	newSectNode;
	struct BufferItem *	newParaNode;
	const int	sectShift= 1;

	int		pos= sectNode->biNumberInParent+ ( after != 0 );

	parentNode= sectNode->biParent;

	newSectNode= docInsertNode( eo->eoDocument,
						parentNode, pos, DOClevSECT );
	if  ( ! newSectNode )
	    { XDEB(newSectNode); return -1;	}
	eo->eoSectionsAdded++;

	if  ( docCopySectDescription( newSectNode, eo->eoDocument,
						    sectNode, eo->eoDocument ) )
	    { LDEB(1);	}

	if  ( docSectionParagraph( eo, &newParaNode, newSectNode, sectShift,
			paraNode->biParaProperties,
			textAttrNr ) )
	    { LDEB(1); return -1;	}

	docEditIncludeNodeInReformatRange( eo, sectNode );
	docEditIncludeNodeInReformatRange( eo, newSectNode );

	if  ( after )
	    {
	    if  ( docTailPosition( dpBeforeSplit, sectNode ) )
		{ LDEB(1); return -1;	}
	    if  ( docHeadPosition( dpAfterSplit, newSectNode ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( docTailPosition( dpBeforeSplit, newSectNode ) )
		{ LDEB(1); return -1;	}
	    if  ( docHeadPosition( dpAfterSplit, sectNode ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

