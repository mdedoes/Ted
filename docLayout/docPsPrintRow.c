/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docRowProperties.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Block level ones
 */
static const char STRUCTtypeTD[]= "TD";
static const char STRUCTtypeTH[]= "TH";

/************************************************************************/

int docPsMarkRowNode(
	    const struct BufferItem *	rowNode,
	    int *			pAsTableFirst,
	    int *			pAsTableLast )
    {
    const BufferItem *	parentNode= rowNode->biParent;
    const BufferItem *	headRowNode;
    const BufferItem *	tailRowNode;

    if  ( ! docIsRowNode( rowNode ) )
	{ return 0;	}

    headRowNode= parentNode->biChildren[rowNode->biRowTableFirst];
    tailRowNode= parentNode->biChildren[rowNode->biRowTablePast- 1];

    if  ( headRowNode->biTopPosition.lpPage != tailRowNode->biBelowPosition.lpPage )
	{ return 0;	}

    if  ( pAsTableFirst )
	{ *pAsTableFirst= rowNode->biNumberInParent == rowNode->biRowTableFirst;	}

    if  ( pAsTableLast )
	{ *pAsTableLast= rowNode->biNumberInParent == rowNode->biRowTablePast- 1;	}

    return 1;
    }

const char * docPsCellNodeMark(
		const struct BufferItem *	cellNode,
		MemoryBuffer *			structureAttributes )
    {
    const BufferItem *	rowNode= cellNode->biParent;
    int			inHeaderRow= rowNode->biRowProperties->rpIsTableHeader;

    if  ( ! docPsMarkRowNode( rowNode, (int *)0, (int *)0 ) )
	{ return (char *)0;	}

    if  ( structureAttributes )
	{
	int		rowspan= 1;
	int		colspan= 1;

	docTableDetermineCellspans( &rowspan, &colspan, cellNode );

	SimpleOutputStream * sosAttributes= sioOutMemoryOpen( structureAttributes );
	sioOutPrintf( sosAttributes, "/O/Table " );

	if  ( inHeaderRow )
	    { sioOutPrintf( sosAttributes, "/Scope/Column " );	}

	if  ( rowspan > 1 )
	    { sioOutPrintf( sosAttributes, "/RowSpan %d ", rowspan );	}

	if  ( colspan > 1 )
	    { sioOutPrintf( sosAttributes, "/ColSpan %d ", colspan );	}

	sioOutClose( sosAttributes );
	}

    /* I could not find a cell property to decide between td and th (MdD Aug 2023) */
    if  ( inHeaderRow )
	{ return STRUCTtypeTH;	}
    else{ return STRUCTtypeTD;	}
    }

