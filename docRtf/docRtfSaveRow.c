/************************************************************************/
/*									*/
/*  Save the nodes in a BufferDocument to an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<docScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docSelect.h>
#   include	<docCellProperties.h>
#   include	<utilPropMask.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfFlags.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static int docRtfSavePropertiesOfRow(
				RtfWriter *			rw,
				const RowProperties *		rpSet )
    {
    int			rval= 0;
    int			col0= -1;
    int			col1= -1;

    RowProperties	rpRef;
    PropertyMask	rpDifMask;
    PropertyMask	rpAllMask;
    PropertyMask	cpAllMask;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    docInitRowProperties( &rpRef );
    utilPropMaskClear( &rpDifMask );
    utilPropMaskClear( &rpAllMask );
    utilPropMaskClear( &cpAllMask );

    utilPropMaskFill( &rpAllMask, RPprop_FULL_COUNT );
    utilPropMaskFill( &cpAllMask, CLprop_COUNT );

    if  ( rw->rwCurrentTree->ptSelection )
	{
	col0= rw->rwCurrentTree->ptSelection->dsCol0;
	col1= rw->rwCurrentTree->ptSelection->dsCol1;
	}

    if  ( docRowPropertyDifference( &rpDifMask, (PropertyMask *)0,
				&rpRef, &rpAllMask, &cpAllMask,
				rpSet, 0, rpSet->rpCellCount, dam0 ) )
	{ LDEB(rpSet->rpCellCount); return -1;	}

    if  ( rw->rwRpExtraMask )
	{ utilPropMaskOr( &rpDifMask, &rpDifMask, rw->rwRpExtraMask );	}

    docRtfWriteTag( rw, "trowd" );

    docRtfSaveRowProperties( rw, &rpDifMask, rpSet, col0, col1 );

    if  ( docCopyRowProperties( &(rw->rwRowProperties), rpSet, dam0 ) )
	{ LDEB(rpSet->rpCellCount); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rpRef );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Enter/Leave a table.						*/
/*									*/
/************************************************************************/

int docRtfPushTable(	RtfWriter *		rw,
			const BufferItem *	rowNode )
    {
    PushedTree *		pt= rw->rwCurrentTree;

    PropertyMask		ppSetMask;
    const RowProperties *	rp;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, "" );

    pt->ptTableNesting++;

    if  ( docRtfPushAttribute( rw ) )
	{ LDEB(1); return -1;	}

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
				    &(rw->rwcOutsideTableParagraphProperties),
				    &ppSetMask, &(pt->ptParagraphProperties),
				    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    rp= rowNode->biRowProperties;
    docRtfSavePropertiesOfRow( rw, rp );

    return 0;
    }

int docRtfPopTable(	RtfWriter *	rw )
    {
    PushedTree *	pt= rw->rwCurrentTree;

    PropertyMask	ppSetMask;

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    pt->ptTableNesting--;

    docCleanRowProperties( &(rw->rwRowProperties) );
    docInitRowProperties( &(rw->rwRowProperties) );

    if  ( docRtfPopAttribute( rw ) )
	{ LDEB(1); return -1;	}

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
			    &(pt->ptParagraphProperties), &ppSetMask,
			    &(rw->rwcOutsideTableParagraphProperties),
			    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a Row level node.						*/
/*									*/
/************************************************************************/

int docRtfEnterRowNode(	RtfWriter *		rw,
			const BufferItem *	rowNode )
    {
    int			tableNesting= docTableNesting( rowNode );

    int			flattenCell= 0;

    if  ( ! ( rw->rwSaveFlags & RTFflagEMIT_CELL ) )
	{
	if  ( rw->rwCurrentTree->ptSelectionIsSingleCell )
	    { flattenCell= 1;	}
	}

    if  ( tableNesting == 1 && ! flattenCell )
	{
	PropertyMask		rpDifMask;
	PropertyMask		rpAllMask;
	PropertyMask		cpAllMask;
	const RowProperties *	rp;

	const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

	rp= rowNode->biRowProperties;

	utilPropMaskClear( &rpDifMask );
	utilPropMaskClear( &rpAllMask );
	utilPropMaskClear( &cpAllMask );

	utilPropMaskFill( &rpAllMask, RPprop_FULL_COUNT );
	utilPropMaskFill( &cpAllMask, CLprop_COUNT );

	if  ( docRowPropertyDifference( &rpDifMask, (PropertyMask *)0,
		    &(rw->rwRowProperties), &rpAllMask, &cpAllMask,
		    rp, 0, rp->rpCellCount, dam0 ) )
	    { LDEB(rp->rpCellCount); return -1;	}

	if  ( ! utilPropMaskIsEmpty( &rpDifMask ) )
	    {
	    if  ( rw->rwCurrentTree->ptTableNesting == 1	&&
		  docRtfPopTable( rw )				)
		{ LDEB(1);	}
	    }

	if  ( rw->rwCurrentTree->ptTableNesting == 0	&&
	      docRtfPushTable( rw, rowNode )	)
	    { LDEB(1);	}
	}

    if  ( ! utilMemoryBufferIsEmpty( &(rowNode->biRowSummary) ) )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedBufferDestination( rw, "trsummary",
					    &(rowNode->biRowSummary), addSemicolon );
	}

    return SCANadviceOK;
    }

int docRtfLeaveRowNode(	RtfWriter *		rw,
			const BufferItem *	rowNode )
    {
    int			tableNesting= docTableNesting( rowNode );

    if  ( tableNesting > 1 )
	{
	int			nip= rowNode->biNumberInParent;
	const struct BufferItem *	cellNode= rowNode->biParent;
	const RowProperties *	rp;

	rp= rowNode->biRowProperties;

	docRtfWriteNextLine( rw );
	docRtfWriteDestinationBegin( rw, "*\\nesttableprops" );

	docRtfSavePropertiesOfRow( rw, rp );

	docRtfWriteTag( rw, "nestrow" );
	docRtfWriteNextLine( rw );

	docRtfWriteDestinationEnd( rw );
	docRtfWriteNextLine( rw );

	if  ( nip == cellNode->biChildCount- 1				||
	      ( nip < cellNode->biChildCount- 1			&&
	        cellNode->biChildren[nip+1]->biLevel != DOClevROW	) )
	    {
	    const struct BufferItem *	parRowNode= cellNode->biParent;
	    const RowProperties * parRp= parRowNode->biRowProperties;

	    docRtfSavePropertiesOfRow( rw, parRp );
	    }
	}
    else{
	docRtfWriteTag( rw, RTFtag_row );
	docRtfWriteNextLine( rw );
	}

    return 0;
    }
