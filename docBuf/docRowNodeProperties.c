/************************************************************************/
/*									*/
/*  Manage the poroperties of table row nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docRowNodeProperties.h"
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<docPropertiesAdmin.h>
#   include	<docRowPropertyAdmin.h>
#   include	"docParaNodeProperties.h"
#   include	<docParaProperties.h>
#   include	"docSelect.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set the properties of a table row.					*/
/*									*/
/*  1)  Adapt to an explicit \trkeepfollow.				*/
/*  2)  Adapt to an implicit \trkeepfollow.				*/
/*									*/
/************************************************************************/

static void docRefresh_trkeepfollow(
				const PropertyMask *		rpSetMask,
				RowProperties *			rpTo,
				struct BufferItem *		rowNode,
				const struct BufferDocument *	bd )
    {
    DocumentPosition		dp;

    if  ( ! docHeadPosition( &dp, rowNode ) )
	{
	struct BufferItem *	paraNode= dp.dpNode;

	/*  1  */
	if  ( ! rpSetMask					||
	      PROPmaskISSET( rpSetMask, RPprop_KEEPFOLLOW )	)
	    {
	    docParaNodeSetKeepWithNext( paraNode, rpTo->rp_Keepfollow, bd );
	    }

	/*  2  */
	rpTo->rp_Keepfollow= paraNode->biParaProperties->ppKeepWithNext;
	}

    return;
    }

static int docSetNewRowNodeProperties(
				struct BufferItem *		rowNode,
				const PropertyMask *		rpSetMask,
				RowProperties *			rp,
				const struct BufferDocument *	bd )
    {
    DocumentPropertyLists *	dpl= bd->bdPropertyLists;

    int				n;
    const RowProperties *	rpN;

    int				c;

    if  ( rowNode->biRowForTable || rp->rpCellCount > 0 )
	{ docRefresh_trkeepfollow( rpSetMask, rp, rowNode, bd );	}

    if  ( rp->rpCellCount != 0				&&
	  rowNode->biChildCount != 0			&&
	  rp->rpCellCount != rowNode->biChildCount	)
	{ LLDEB(rp->rpCellCount,rowNode->biChildCount); return -1;	}

    if  ( rp->rpCellCount == 0 )
	{ n= 0;		}
    else{
	n= docRowPropertiesNumberImpl( &(dpl->dplRowPropertyList), rp );
	if  ( n < 0 )
	    { LDEB(n); return -1;	}
	}

    rpN= docGetRowPropertiesByNumberImpl( &(dpl->dplRowPropertyList), n );
    if  ( ! rpN )
	{ LXDEB(n,rpN); return -1;	}

    rowNode->BIU.biuRow.brRowPropertyNumber= n;
    rowNode->BIU.biuRow.brProperties= rpN;

    /* Initial setting from RTF parser */
    if  ( rowNode->biChildCount == 0 )
	{ return 0;	}

    if  ( rpN->rpCellCount > 0 )
	{
	int			spanHead= -1;
	int			hx= rpN->rpLeftIndentTwips;
	int			tx= rpN->rpLeftIndentTwips;
	const CellProperties *	cpCol= rpN->rpCells;

	for ( c= 0; c < rpN->rpCellCount; cpCol++, c++ )
	    {
	    tx += cpCol->cpWide;

	    rowNode->biChildren[c]->biCellHeadX= hx;
	    rowNode->biChildren[c]->biCellTailX= tx;
	    rowNode->biChildren[c]->biCellProperties= cpCol;

	    switch( cpCol->cpHorizontalMerge )
		{
		case CELLmergeNONE:
		    if  ( spanHead >= 0 )
			{
			rowNode->biChildren[spanHead]->biCellColspan=
								c- spanHead;
			rowNode->biChildren[spanHead]->biCellTailX= hx;
			spanHead= -1;
			}
		    rowNode->biChildren[c]->biCellColspan= 1;
		    break;
		case CELLmergeHEAD:
		    if  ( spanHead >= 0 )
			{
			rowNode->biChildren[spanHead]->biCellColspan=
								c- spanHead;
			rowNode->biChildren[spanHead]->biCellTailX= hx;
			}
		    spanHead= c;
		    rowNode->biChildren[c]->biCellColspan= 1;
		    break;
		case CELLmergeFOLLOW:
		    if  ( spanHead < 0 )
			{ LLLDEB(spanHead,c,cpCol->cpHorizontalMerge);	}

		    rowNode->biChildren[c]->biCellColspan= 0;
		    break;

		default:
		    LLDEB(c,cpCol->cpHorizontalMerge);
		    return -1;
		}

	    hx= tx;
	    }

	if  ( spanHead >= 0 )
	    {
	    rowNode->biChildren[spanHead]->biCellColspan= c- spanHead;
	    rowNode->biChildren[spanHead]->biCellTailX= hx;
	    }
	}
    else{
	for ( c= 0; c < rowNode->biChildCount; c++ )
	    {
	    rowNode->biChildren[c]->biCellHeadX= 0;
	    rowNode->biChildren[c]->biCellTailX= 0;
	    rowNode->biChildren[c]->biCellProperties= (const CellProperties *)0;
	    rowNode->biChildren[c]->biCellColspan= 0;
	    }
	}

    return 0;
    }

int docChangeRowNodeProperties(	PropertyMask *			rpDoneMask,
				PropertyMask *			cpDoneMask,
				struct BufferItem *		rowNode,
				const PropertyMask *		rpSetMask,
				const PropertyMask *		cpSetMask,
				const RowProperties *		rpSet,
				int				colTo,
				int				colCount,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap *	dam )
    {
    int				rval= 0;
    const RowProperties *	rpNode= rowNode->BIU.biuRow.brProperties;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( rpNode )
	{
	if  ( docCopyRowProperties( &rp, rpNode,
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docUpdRowProperties( rpDoneMask, cpDoneMask,
					&rp, rpSetMask, cpSetMask,
					rpSet, colTo, colCount, dam ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSetNewRowNodeProperties( rowNode, rpSetMask, &rp, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

static int docSetRowNodePropertiesX(
				struct BufferItem *		rowNode,
				const RowProperties *		rpSet,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap *	dam )
    {
    int				rval= 0;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( rpSet )
	{
	if  ( docCopyRowProperties( &rp, rpSet, dam ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docSetNewRowNodeProperties( rowNode,
				    (const PropertyMask *)0, &rp, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

int docSetRowNodeProperties(	struct BufferItem *		rowNode,
				const RowProperties *		rpSet,
				const struct BufferDocument *	bd )
    {
    return docSetRowNodePropertiesX( rowNode, rpSet, bd,
				(const struct DocumentAttributeMap *)0 );
    }

int docCopyRowNodeProperties(	struct BufferItem *		rowNodeTo,
				const struct BufferItem *	rowNodeFrom,
				const struct BufferDocument *		bd,
				const struct DocumentAttributeMap *	dam )
    {
    const RowProperties * rpFrom= rowNodeFrom->BIU.biuRow.brProperties;

    if  ( utilCopyMemoryBuffer( &(rowNodeTo->biRowSummary), &(rowNodeFrom->biRowSummary) ) )
	{ LDEB(1); return -1;	}

    return docSetRowNodePropertiesX( rowNodeTo, rpFrom, bd, dam );
    }

/************************************************************************/
# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docRowNodeResetRowProperties(	struct BufferItem *		rowNode,
					const struct BufferDocument *	bd )
    {
    return docSetRowNodePropertiesX( rowNode, (const RowProperties *)0, bd,
					    (const struct DocumentAttributeMap *)0 );
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Set the trkeepfollow on the basis of the paragraph properties of	*/
/*  its first child. (So there is no need to update the child as well)	*/
/*									*/
/************************************************************************/

int docRowNodeSetKeepFollow(	struct BufferItem *		rowNode,
				int			keepFollow,
				const struct BufferDocument *	bd )
    {
    int				rval= 0;
    const RowProperties *	rpNode= rowNode->BIU.biuRow.brProperties;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( rpNode )
	{
	if  ( docCopyRowProperties( &rp, rpNode,
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( rp.rp_Keepfollow != keepFollow )
	{
	rp.rp_Keepfollow= keepFollow;

	if  ( docSetNewRowNodeProperties( rowNode,
					(const PropertyMask *)0, &rp, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	rval= 1;
	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make sure the the row properties of a row have the same number of	*/
/*  columns as the row node has children.				*/
/*									*/
/************************************************************************/

int docRowNodeFixColumnCount(		struct BufferItem *		rowNode,
					const struct BufferDocument *	bd )
    {
    int				rval= 0;
    const RowProperties *	rpNode= rowNode->BIU.biuRow.brProperties;
    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( rpNode )
	{
	if  ( docCopyRowProperties( &rp, rpNode, dam0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  1  */
    if  ( rp.rpCellCount < rowNode->biChildCount )
	{
	CellProperties	cp;

	if  ( rp.rpCellCount == 0 )
	    { LDEB(rp.rpCellCount); rval= -1; goto ready;	}

	docInitCellProperties( &cp );
	docCopyCellProperties( &cp, &(rp.rpCells[rp.rpCellCount-1]), dam0 );
	cp.cpWide= 0;

	/* Add zero width dummy columns to avoid crashes */
	while( rp.rpCellCount < rowNode->biChildCount )
	    {
	    if  ( docInsertRowColumn( &rp, rp.rpCellCount, &cp, dam0 ) )
		{ LDEB(rp.rpCellCount); return -1;	}
	    }

	/*docCleanCellProperties( &cp );*/
	}

    /*  2  */
    if  ( rp.rpCellCount > rowNode->biChildCount )
	{
	docDeleteColumnsFromRow( &rp, rowNode->biChildCount,
				    rp.rpCellCount- rowNode->biChildCount );
	}

    if  ( docSetNewRowNodeProperties( rowNode,
					(const PropertyMask *)0, &rp, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

int docInsertColumnInRowNode(	struct BufferItem *			rowNode,
				int					col,
				const CellProperties *			cp,
				const struct BufferDocument *		bd,
				const struct DocumentAttributeMap *	dam )
    {
    int				rval= 0;
    const RowProperties *	rpNode= rowNode->biRowProperties;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( rpNode )
	{
	if  ( docCopyRowProperties( &rp, rpNode,
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(rpNode->rpCellCount); rval= -1; goto ready;	}
	}

    if  ( col > rp.rpCellCount )
	{ LLDEB(col,rp.rpCellCount); rval= -1; goto ready;	}

    if  ( docInsertRowColumn( &rp, col, cp, dam ) )
	{ LDEB(col); rval= -1; goto ready;	}

    if  ( docSetNewRowNodeProperties( rowNode,
					(const PropertyMask *)0, &rp, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

int docInsertColumnProperties(	struct BufferItem *		rowNodeTo,
				const struct BufferItem *	rowNodeFrom,
				int				colTo,
				int				colFrom,
				const struct BufferDocument *		bd,
				const struct DocumentAttributeMap *	dam )
    {
    const RowProperties *	rpFrom= rowNodeFrom->BIU.biuRow.brProperties;

    return docInsertColumnInRowNode( rowNodeTo, colTo,
					&(rpFrom->rpCells[colFrom]), bd, dam );
    }

int docDeleteColumnsFromRowNode(struct BufferItem *			rowNode,
				int				col,
				int				count,
				const struct BufferDocument *		bd )
    {
    int				rval= 0;
    const RowProperties *	rpNode= rowNode->BIU.biuRow.brProperties;

    RowProperties		rp;

    docInitRowProperties( &rp );

    if  ( docCopyRowProperties( &rp, rpNode, (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDeleteColumnsFromRow( &rp, col, count ) )
	{ LLDEB(col,count); return -1;	}

    if  ( docSetNewRowNodeProperties( rowNode,
					(const PropertyMask *)0, &rp, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

