/************************************************************************/
/*									*/
/*  Manage documner node hierarchy from the rtf parser.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docNodeTree.h>
#   include	<docRowNodeProperties.h>
#   include	<docTreeNode.h>
#   include	<docAttributes.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Apply the row properties that have been collected to a row.		*/
/*									*/
/************************************************************************/

static int docRtfSetRowProperties(	RtfReader *		rr,
					struct BufferItem *	rowNode )
    {
    PropertyMask	rpDoneMask;
    PropertyMask	cpSetMask;

    RtfTreeStack *	rts= rr->rrTreeStack;
    RowProperties *	rp= &(rts->rtsRowProperties);

    rp->rpShadingNumber= docItemShadingNumber(
				rr->rrDocument, &(rr->rrRowShading) );

    rp->rpFrameNumber= docFramePropertiesNumber(
			    rr->rrDocument, &(rr->rrRowFrameProperties) );

    utilPropMaskClear( &rpDoneMask );
    utilPropMaskClear( &cpSetMask );

    utilPropMaskFill( &cpSetMask, CLprop_COUNT );

    if  ( docChangeRowNodeProperties( &rpDoneMask, (PropertyMask *)0,
	    rowNode,
	    &(rr->rrRowPropertyMask), &cpSetMask, rp,
	    0, rp->rpCellCount,
	    rr->rrDocument, (DocumentAttributeMap *)0 ) )
	{ LDEB(rp->rpCellCount); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(rts->rtsRowSummary) )	&&
	  utilCopyMemoryBuffer( &(rowNode->biRowSummary),
					&(rts->rtsRowSummary) )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a row in the document.					*/
/*									*/
/*  In this order: (1) can make that (2) is needed.			*/
/*  1)  To avoid crashes, make sure that the row does not have more	*/
/*	children than columns: Split it until this is the case.		*/
/*  2)  To avoid crashes, make sure that the row does not have more	*/
/*	columns than children: Remove the surplus.			*/
/*									*/
/************************************************************************/

int docRtfCloseRow(	RtfReader *		rr,
			BufferItem *		rowNode )
    {
    if  ( rowNode->biRowForTable )
	{
	RtfTreeStack *	rts= rr->rrTreeStack;
	RowProperties *	rp= &(rts->rtsRowProperties);

	if  ( docRtfSetRowProperties( rr, rowNode ) )
	    { LDEB(1); return -1;	}

	if  ( rp->rpCellCount != rowNode->biChildCount			&&
	      docRowNodeFixColumnCount( rowNode, rr->rrDocument )	)
	    {
	    LLDEB(rp->rpCellCount,rowNode->biChildCount);
	    return -1;
	    }

	utilEmptyMemoryBuffer( &(rts->rtsRowSummary) );
	}
    else{
	docRowNodeResetRowProperties( rowNode, rr->rrDocument );
	}

    return 0;
    }

/************************************************************************/

void docRtfSetForRow(	struct BufferItem *	node )
    {
    node= docGetRowLevelNode( node );
    while( node )
	{
	if  ( ! node->biRowForTable )
	    { node->biRowForTable= 1;	}

	node= docGetRowLevelNode( node->biParent );
	}
    }

/************************************************************************/
/*									*/
/*  Start a row.							*/
/*									*/
/************************************************************************/

int docRtfStartRow(	RtfReader *	rr,
			int		forTable )
    {
    struct BufferItem *		rowNode;
    RtfTreeStack *		rts= rr->rrTreeStack;

    rowNode= docInsertNode( rr->rrDocument, rts->rtsNode, -1, DOClevROW );
    if  ( ! rowNode )
	{ SXDEB(docLevelStr(rts->rtsLevel),rowNode); return -1; }
    rowNode->biRowForTable= forTable;
    rts->rtsNode= rowNode;

    if  ( forTable )
	{
	/* Is an artifact that we only pick up if it is explicitly set */
	PROPmaskUNSET(  &(rr->rrRowPropertyMask), RPprop_KEEPFOLLOW );

	if  ( docRtfSetRowProperties( rr, rowNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docRowNodeResetRowProperties( rowNode, rr->rrDocument );
	}

    return 0;
    }
