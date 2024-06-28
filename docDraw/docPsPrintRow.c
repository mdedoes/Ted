/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/************************************************************************/

/**
 * Is a row a part of a plain table? I.E. It gives no visual clue about being a row.
 * Then probably, it is only used for formatting purposes. If this is the 
 * case for all rows in the table, then we might not want to mark the
 * table as a real table in the generated PDF.
 */
static int docTableIsPlain( 
	    const struct BufferItem *	rowNode )
    {
    const BufferItem *	parentNode= rowNode->biParent;
    int			row;

    for ( row= rowNode->biRowTableFirst; row < rowNode->biRowTablePast; row++ )
	{
	const BufferItem *	node= parentNode->biChildren[row];

	if  ( ! node->biRowIsPlain )
	    { return 0;	}
	}

    return 1;
    }

int docPsMarkRowNode(
	    const PrintingState *	ps,
	    const struct BufferItem *	rowNode,
	    int *			pAsTableFirst,
	    int *			pAsTableLast,
	    MemoryBuffer *		tableStructureAttributes,
	    MemoryBuffer *		structureAttributes )
    {
    const BufferItem *	parentNode= rowNode->biParent;

    if  ( ! docIsRowNode( rowNode )					||
	  rowNode->biRowProperties->rpFlatInPDF				||
	  ( ps->psFlattenPlainTables && docTableIsPlain( rowNode ) )	)
	{ return 0;	}

    if  ( pAsTableFirst )
	{
	if  ( rowNode->biNumberInParent == rowNode->biRowTableFirst )
	    { *pAsTableFirst= 1;	}
	else{
	    const BufferItem *	prevNode= parentNode->biChildren[rowNode->biNumberInParent- 1];

	    *pAsTableFirst= prevNode->biRowProperties->rpFlatInPDF;
	    }
	}

    if  ( pAsTableLast )
	{
	if  ( rowNode->biNumberInParent == rowNode->biRowTablePast- 1 )
	    { *pAsTableLast= 1;	}
	else{
	    const BufferItem *	nextNode= parentNode->biChildren[rowNode->biNumberInParent+ 1];

	    *pAsTableLast= nextNode->biRowProperties->rpFlatInPDF;
	    }
	}

    if  ( tableStructureAttributes )
	{
	int			row;

	for ( row= rowNode->biRowTableFirst; row < rowNode->biRowTablePast; row++ )
	    {
	    const BufferItem *	node= parentNode->biChildren[row];

	    if  ( ! utilMemoryBufferIsEmpty( &(node->biRowSummary) ) )
		{
		SimpleOutputStream * sosAttributes= sioOutMemoryOpen( tableStructureAttributes );

		sioOutPrintf( sosAttributes, "/O/Table " );

		if  ( psPrintPdfmarkTextEntry( sosAttributes, "Summary", &(node->biRowSummary) ) )
		    { LDEB(1); return -1;	}

		sioOutClose( sosAttributes );
		break;
		}
	    }
	}

    return 1;
    }

const char * docPsCellNodeMark(
		const PrintingState *		ps,
		const struct BufferItem *	cellNode,
		MemoryBuffer *			structureAttributes )
    {
    const BufferItem *	rowNode= cellNode->biParent;
    int			inHeaderRow= rowNode->biRowProperties->rpIsTableHeader;
    int			inHeaderColumn;

    if  ( ! docPsMarkRowNode( ps, rowNode, (int *)0, (int *)0, (MemoryBuffer *)0, (MemoryBuffer *)0 ) )
	{ return (char *)0;	}

    inHeaderColumn= cellNode->biCellProperties->cpIsRowHeader;

    if  ( structureAttributes )
	{
	int		rowspan= 1;
	int		colspan= 1;

	docTableDetermineCellspans( &rowspan, &colspan, cellNode );

	if  ( rowspan > 1 || colspan > 1 || inHeaderRow || inHeaderColumn )
	    {
	    SimpleOutputStream * sosAttributes= sioOutMemoryOpen( structureAttributes );
	    sioOutPrintf( sosAttributes, "/O/Table " );

	    if  ( inHeaderRow )
		{ sioOutPrintf( sosAttributes, "/Scope/Column " );	}
	    else{
		if  ( inHeaderColumn )
		    { sioOutPrintf( sosAttributes, "/Scope/Row " );	}
		}

	    if  ( rowspan > 1 )
		{ sioOutPrintf( sosAttributes, "/RowSpan %d ", rowspan );	}

	    if  ( colspan > 1 )
		{ sioOutPrintf( sosAttributes, "/ColSpan %d ", colspan );	}

	    sioOutClose( sosAttributes );
	    }
	}

    if  ( inHeaderRow || inHeaderColumn )
	{ return STRUCTtypeTH;	}
    else{ return STRUCTtypeTD;	}
    }

