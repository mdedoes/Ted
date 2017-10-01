/************************************************************************/
/*									*/
/*  Manage the properties of tree nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docTreeNode.h"
#   include	<docBreakKind.h>
#   include	<docSectProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Are the columns of a section node to be balanced?			*/
/*									*/
/************************************************************************/

int docSectColumnsAreBalanced(	const struct BufferItem *	sectNode )
    {
    const SectionProperties *	sp= sectNode->biSectProperties;
    int				nrInParent= sectNode->biNumberInParent;

    if  ( sp->spColumnCount > 1					&&
	  sectNode->biParent					&&
	  nrInParent < sectNode->biParent->biChildCount- 1	)
	{
	const struct BufferItem *	nextSectNode;

	nextSectNode= sectNode->biParent->biChildren[nrInParent+ 1];

	if  ( nextSectNode->biSectBreakKind == DOCibkNONE )
	    { return 1;	}
	}

    return 0;
    }

