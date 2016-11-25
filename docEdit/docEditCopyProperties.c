/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>
#   include	<stdlib.h>

#   include	"docEdit.h"
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docEditSetProperties.h"
#   include	<docRowProperties.h>
#   include	<docSelect.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<utilPropMask.h>
#   include	<docCellProperties.h>
#   include	<docBuf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

typedef struct CollectedNode
    {
    int			cnDirection;
    struct BufferItem *	cnNode;
    } CollectedNode;

typedef struct CollectNodes
    {
    int			cnNodeCount;
    CollectedNode *	cnNodes;
    } CollectNodes;

static void docInitCollectNodes(	CollectNodes *	cn )
    {
    cn->cnNodeCount= 0;
    cn->cnNodes= (CollectedNode *)0;

    return;
    }

static void docCleanCollectNodes(	CollectNodes *	cn )
    {
    if  ( cn->cnNodes )
	{ free( cn->cnNodes );	}

    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docCopyPropsCollectEnterNode(
				struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				through )
    {
    CollectNodes *	cn= (CollectNodes *)through;

    if  ( cn->cnNodes )
	{
	cn->cnNodes[cn->cnNodeCount].cnDirection= +1;
	cn->cnNodes[cn->cnNodeCount].cnNode= node;
	cn->cnNodeCount++;
	}
    else{ cn->cnNodeCount++;	}

    return SCANadviceOK;
    }

static int docCopyPropsCollectLeaveNode(
				struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				through )
    {
    CollectNodes *	cn= (CollectNodes *)through;

    if  ( cn->cnNodes )
	{
	cn->cnNodes[cn->cnNodeCount].cnDirection= -1;
	cn->cnNodes[cn->cnNodeCount].cnNode= node;
	cn->cnNodeCount++;
	}
    else{ cn->cnNodeCount++;	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int docCopyPropsCollectNodes(	CollectNodes *			cn,
					struct BufferDocument *		bd,
					struct DocumentTree *		tree,
					const DocumentSelection *	ds )
    {
    const int			flags= FLAGtsSCAN_MERGED_CELLS;

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
			    docCopyPropsCollectEnterNode,
			    docCopyPropsCollectLeaveNode,
			    (TreeVisitor)0, (TreeVisitor)0, 
			    flags, (void *)cn ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( bd, tree,
			    docCopyPropsCollectEnterNode,
			    docCopyPropsCollectLeaveNode,
			    (TreeVisitor)0, (TreeVisitor)0, 
			    flags, (void *)cn ) < 0 )
	    { LDEB(1); return -1;	}
	}

    if  ( cn->cnNodeCount > 0 )
	{
	cn->cnNodes= (CollectedNode *)malloc(
				    cn->cnNodeCount* sizeof(CollectedNode) );
	if  ( ! cn->cnNodes )
	    { LXDEB(cn->cnNodeCount,cn->cnNodes); return -1;	}

	cn->cnNodeCount= 0;

	if  ( ds )
	    {
	    if  ( docScanSelection( bd, ds,
			    docCopyPropsCollectEnterNode,
			    docCopyPropsCollectLeaveNode,
			    (TreeVisitor)0, (TreeVisitor)0, 
			    flags, (void *)cn ) < 0 )
		{ LDEB(cn->cnNodeCount); return -1;	}
	    }
	else{
	    if  ( docScanTree( bd, tree,
			    docCopyPropsCollectEnterNode,
			    docCopyPropsCollectLeaveNode,
			    (TreeVisitor)0, (TreeVisitor)0, 
			    flags, (void *)cn ) < 0 )
		{ LDEB(cn->cnNodeCount); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/

static int docStartCopyRowNodeProperties(
				SetProperties *			setProps,
				struct BufferItem *		rowNodeTo,
				const struct BufferItem *	rowNodeFrom )
    {
    const RowProperties *	oldRpTo= rowNodeTo->biRowProperties;
    const RowProperties *	rpFrom= rowNodeFrom->biRowProperties;
    const DocumentAttributeMap * dam0= (const DocumentAttributeMap *)0;

    int colTo= setProps->spTableRectangle.trCol0;
    int colCount= setProps->spTableRectangle.trCol1- colTo+ 1;

    /* Do not map attributes here: Are in 'to' format */
    if  ( docCopyRowProperties( &(setProps->spCurrentRowProperties),
							    oldRpTo, dam0 ) )
	{ LDEB(rpFrom->rpCellCount); return -1;	}

    /* Map attributes from 'from' to 'to' format */
    if  ( setProps->sp_rpSetMask )
	{
	if  ( docUpdRowProperties(
			(struct PropertyMask *)0, (struct PropertyMask *)0,
			&(setProps->spCurrentRowProperties),
			setProps->sp_rpSetMask, setProps->sp_cpSetMask,
			rpFrom, colTo, colCount, setProps->spAttributeMap ) )
	    { LDEB(rpFrom->rpCellCount); return -1;	}
	}

    return 0;
    }

static int docCopyCellNodeProperties(
				SetProperties *			setProps,
				struct BufferItem *		cellNodeTo,
				const struct BufferItem *	cellNodeFrom )
    {
    const CellProperties *	cpFrom;
    int				col;
    CellProperties *		cpTo;

    cpFrom= cellNodeFrom->biCellProperties;
    if  ( ! cpFrom )
	{ SXDEB(docLevelStr(cellNodeFrom->biLevel),cpFrom); return -1;	}

    col= cellNodeTo->biNumberInParent;
    if  ( col >= setProps->spCurrentRowProperties.rpCellCount )
	{ LLDEB(col,setProps->spCurrentRowProperties.rpCellCount); return -1; }

    cpTo= &(setProps->spCurrentRowProperties.rpCells[col]);

    /* Map attributes from 'from' to 'to' format */
    if  ( docUpdCellProperties( (struct PropertyMask *)0, cpTo,
		setProps->sp_cpSetMask, cpFrom, setProps->spAttributeMap ) )
	{ LDEB(col); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy the properties of an individual node.				*/
/*									*/
/************************************************************************/

static int docCopyNodePropertiesEnter(
				SetProperties *			setProps,
				const DocumentSelection *	dsTo,
				struct BufferItem *		nodeTo,
				const struct BufferItem *	nodeFrom )
    {
    switch( nodeTo->biLevel )
	{
	case DOClevPARA:
	    setProps->spGotPara++;

	    if  ( setProps->sp_taSetMask && ! setProps->sp_taSet )
		{ XXDEB(setProps->sp_taSetMask,setProps->sp_taSet); return -1; }

	    if  ( docEditChangeParaProperties( setProps, dsTo, nodeTo,
				    setProps->sp_taSet,
				    nodeFrom->biParaProperties ) )
		{ LDEB(nodeTo->biLevel); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( nodeTo->biParent ) )
		{
		if  ( ! setProps->spEnteredRow )
		    {
		    if  ( docStartCopyRowNodeProperties( setProps,
							nodeTo->biParent,
							nodeFrom->biParent ) )
			{ LDEB(nodeTo->biLevel); return -1;	}

		    setProps->spEnteredRow++;
		    }

		if  ( setProps->sp_cpSetMask				&&
		      docCopyCellNodeProperties( setProps, nodeTo, nodeFrom ) )
		    { LDEB(1); return -1;	}

		setProps->spGotCell++;
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( nodeTo ) )
		{
		if  ( docStartCopyRowNodeProperties( setProps,
							nodeTo, nodeFrom ) )
		    { LDEB(nodeTo->biLevel); return -1;	}

		setProps->spEnteredRow++;
		}
	    break;

	case DOClevSECT:
	    if  ( setProps->sp_spSetMask )
		{
		EditOperation *	eo= setProps->spEditOperation;

		if  ( docEditUpdSectProperties( eo, setProps->sp_spDoneMask,
					    nodeTo, setProps->sp_spSetMask,
					    nodeFrom->biSectProperties ) )
		    { XDEB(setProps->sp_spSetMask); return -1;	}
		}
	    setProps->spGotSect++;
	    break;

	default:
	     SDEB(docLevelStr(nodeTo->biLevel));
	     return -1;
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docCopyNodePropertiesLeave(
				SetProperties *			setProps,
				const DocumentSelection *	dsTo,
				struct BufferItem *		nodeTo,
				const struct BufferItem *	nodeFrom )
    {
    switch( nodeTo->biLevel )
	{
	case DOClevPARA:
	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( nodeTo ) )
		{
		PropertyMask		rpSetMask;

		utilPropMaskClear( &rpSetMask );
		if  ( setProps->sp_rpSetMask )
		    { rpSetMask= *setProps->sp_rpSetMask;	}

		if  ( setProps->sp_cpSetMask )
		    { PROPmaskADD( &rpSetMask, RPprop_CELL_PROPS );	}

		/* prevent crashes, and other inconsistencies */
		if  ( PROPmaskISSET( &rpSetMask, RPprop_CELL_COUNT ) )
		    {
		    LDEB(RPprop_CELL_COUNT);
		    PROPmaskUNSET( &rpSetMask, RPprop_CELL_COUNT );
		    }

		if  ( docEditChangeRowProperties( setProps,
						    nodeTo, &rpSetMask ) )
		    { LDEB(nodeTo->biLevel); return -1;	}

		setProps->spLeftRow++;
		}
	    break;

	case DOClevSECT:
	    break;

	default:
	     SDEB(docLevelStr(nodeTo->biLevel));
	     return -1;
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int docCopySelectionProperties(
			DocumentCopyJob *		dcj,
			const DocumentSelection *	dsTo,
			const DocumentSelection *	dsFrom,
			const struct TextAttribute *	taSet,
			const PropertyMask *		taSetMask,
			const PropertyMask *		ppSetMask,
			const PropertyMask *		cpSetMask,
			const PropertyMask *		rpSetMask,
			const PropertyMask *		spSetMask,
			const PropertyMask *		dpSetMask )

    {
    int				rval= 0;
    EditOperation *		eo= dcj->dcjEditOperation;
    SetProperties		setProps;

    CollectNodes		cnTo;
    CollectNodes		cnFrom;

    docInitCollectNodes( &cnTo );
    docInitCollectNodes( &cnFrom );
    docEditInitSetProperties( &setProps );

    if  ( ! docSelectionIsSet( dsFrom ) )
	{
	XXDEB(dsFrom->dsHead.dpNode,dsFrom->dsTail.dpNode);
	rval= -1; goto ready;
	}
    if  ( ! docSelectionIsSet( dsTo ) )
	{
	XXDEB(dsTo->dsHead.dpNode,dsTo->dsTail.dpNode);
	rval= -1; goto ready;
	}

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    setProps.spEditOperation= eo;
    setProps.spRedoParaLayout= 1;
    setProps.spAttributeMap= &(dcj->dcjAttributeMap);

    setProps.sp_taSet= taSet;

    setProps.sp_taSetMask= taSetMask;
    setProps.sp_ppSetMask= ppSetMask;
    setProps.sp_cpSetMask= cpSetMask;
    setProps.sp_rpSetMask= rpSetMask;
    setProps.sp_spSetMask= spSetMask;

    if  ( cpSetMask || rpSetMask )
	{
	if  ( docGetTableRectangle( &(setProps.spTableRectangle), dsTo ) )
	    { XXDEB(cpSetMask,rpSetMask); rval= -1; goto ready;	}
	}
    else{
	docInitTableRectangle( &(setProps.spTableRectangle) );
	}

    if  ( dpSetMask							&&
	  docChangeDocumentProperties( eo, (PropertyMask *)0,
			    dpSetMask, dcj->dcjSourceDocument->bdProperties,
			    setProps.spAttributeMap )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( taSetMask || ppSetMask || cpSetMask || rpSetMask || spSetMask )
	{
	int		n;

	if  ( docCopyPropsCollectNodes( &cnFrom, dcj->dcjSourceDocument,
						dcj->dcjSourceTree, dsFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docCopyPropsCollectNodes( &cnTo, eo->eoDocument,
							eo->eoTree, dsTo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( cnFrom.cnNodeCount != cnTo.cnNodeCount )
	    {
	    LLDEB(cnFrom.cnNodeCount,cnTo.cnNodeCount);
	    rval= -1; goto ready;
	    }

	for ( n= 0; n < cnTo.cnNodeCount; n++ )
	    {
	    if  ( cnTo.cnNodes[n].cnNode->biLevel !=
				      cnFrom.cnNodes[n].cnNode->biLevel )
		{
		SDEB(docLevelStr(cnTo.cnNodes[n].cnNode->biLevel));
		SDEB(docLevelStr(cnFrom.cnNodes[n].cnNode->biLevel));
		rval= -1; goto ready;
		}

	    if  ( cnTo.cnNodes[n].cnDirection > 0 )
		{
		if  ( docCopyNodePropertiesEnter( &setProps, dsTo,
					cnTo.cnNodes[n].cnNode,
					cnFrom.cnNodes[n].cnNode ) )
		    { LDEB(n); rval= -1; goto ready;	}
		}

	    if  ( cnTo.cnNodes[n].cnDirection < 0 )
		{
		if  ( docCopyNodePropertiesLeave( &setProps, dsTo,
					cnTo.cnNodes[n].cnNode,
					cnFrom.cnNodes[n].cnNode ) )
		    { LDEB(n); rval= -1; goto ready;	}
		}
	    }
	}

    if  ( ( rpSetMask || cpSetMask ) && ! setProps.spEnteredRow )
	{
	struct BufferItem *	rowNodeTo= docGetRowNode( dsTo->dsHead.dpNode );
	struct BufferItem *	rowNodeFrom= docGetRowNode( dsFrom->dsHead.dpNode );

	if  ( ! rowNodeTo || ! rowNodeFrom )
	    { XXDEB(rowNodeTo,rowNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodePropertiesEnter( &setProps, dsTo,
						rowNodeTo, rowNodeFrom ) )
	    { XDEB(rpSetMask); rval= -1; goto ready;	}
	}

    if  ( cpSetMask && ! setProps.spGotCell )
	{
	struct BufferItem *	cellNodeTo= docGetCellNode( dsTo->dsHead.dpNode );
	struct BufferItem *	cellNodeFrom= docGetCellNode( dsFrom->dsHead.dpNode );

	if  ( ! cellNodeTo || ! cellNodeFrom )
	    { XXDEB(cellNodeTo,cellNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodePropertiesEnter( &setProps, dsTo,
						cellNodeTo, cellNodeFrom ) )
	    { XDEB(cpSetMask); rval= -1; goto ready;	}
	}

    if  ( ( rpSetMask || cpSetMask ) && ! setProps.spLeftRow )
	{
	struct BufferItem *	rowNodeTo= docGetRowNode( dsTo->dsHead.dpNode );
	struct BufferItem *	rowNodeFrom= docGetRowNode( dsFrom->dsHead.dpNode );

	if  ( ! rowNodeTo || ! rowNodeFrom )
	    { XXDEB(rowNodeTo,rowNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodePropertiesLeave( &setProps, dsTo,
						rowNodeTo, rowNodeFrom ) )
	    { XDEB(rpSetMask); rval= -1; goto ready;	}
	}

    if  ( spSetMask && ! setProps.spGotSect )
	{
	struct BufferItem *	sectNodeTo= docGetSectNode( dsTo->dsHead.dpNode );
	struct BufferItem *	sectNodeFrom= docGetSectNode( dsFrom->dsHead.dpNode );

	if  ( ! sectNodeTo || ! sectNodeFrom )
	    { XXDEB(sectNodeTo,sectNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodePropertiesEnter( &setProps, dsTo,
						sectNodeTo, sectNodeFrom ) )
	    { XDEB(spSetMask); rval= -1; goto ready;	}
	}

  ready:

    docCleanCollectNodes( &cnTo );
    docCleanCollectNodes( &cnFrom );
    docEditCleanSetProperties( &setProps );

    return rval;
    }

