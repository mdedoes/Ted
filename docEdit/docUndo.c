/************************************************************************/
/*									*/
/*  Editor Undo/Redo functionality.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docCopyNode.h"
#   include	"docEditStep.h"
#   include	"docTrace.h"
#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docBuf.h>
#   include	<docRowNodeProperties.h>
#   include	<docNodeTree.h>
#   include	<docTreeType.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/************************************************************************/
/*									*/
/*  Reinsert nodes as part of an undo operation.			*/
/*									*/
/************************************************************************/

int docReinsertNodes(	EditOperation *			eo,
			struct BufferItem *		parentTo,
			const struct BufferItem *	parentFrom,
			int				to,
			const EditStep *		es )
    {
    int				rval= 0;
    DocumentCopyJob		dcj;
    const int			from= 0;

    /*  Deletion was at the tail of the parent? */
    switch( es->esSelectionPosition )
	{
	case SELposAFTER:
	    break;
	case SELposBEFORE:
	    to++;
	    break;
	case SELposTAIL: /* was replace whole document */
	    break;
	default:
	    LDEB(es->esSelectionPosition);
	    break;
	}

    docInitDocumentCopyJob( &dcj );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docGetSelectionScope( &(dcj.dcjTargetSelectionScope), parentTo );
    if  ( docGetRootOfSelectionScope( &(dcj.dcjTargetTree), (struct BufferItem **)0,
			    eo->eoDocument, &(dcj.dcjTargetSelectionScope) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docCopyNodeChildren( &dcj, parentTo, to,
				parentFrom, from,  parentFrom->biChildCount ) )
	{ LLDEB(from,to); rval= -1; goto ready;	}

    to +=  parentFrom->biChildCount;

    /* remove the empty child that was added to prevent an empty parent */
    if  ( es->esSelectionPosition == SELposTAIL )
	{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	docEditDeleteNodes( eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentTo, to, 1 );
	}

    docEditIncludeNodeInReformatRange( eo, parentTo );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of columns.				*/
/*									*/
/************************************************************************/

int docUndoDeleteColumn(	EditOperation *		eo,
				const EditStep *	es )
    {
    DocumentPosition		dpFrom;

    int				colFrom;
    int				rowFrom, row0From, row1From;
    struct BufferItem *		parentFrom;

    int				colTo;
    int				rowTo, row0To, row1To;
    struct BufferItem *		parentTo;

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}

    if  ( docDelimitTable( dpFrom.dpNode, &parentFrom,
				&colFrom, &row0From, &rowFrom, &row1From ) )
	{ LDEB(1); return -1;	}

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentTo,
				&colTo, &row0To, &rowTo, &row1To ) )
	{ LDEB(1); return -1;	}

    if  ( row0From != 0 || row1To- row0To != row1From- row0From )
	{ LLLLDEB(row0To,row1To,row0From,row1From); return -1; }

    rowTo= row0To;
    for ( rowFrom= row0From; rowFrom <= row1From; rowTo++, rowFrom++ )
	{
	struct BufferItem *	rowNodeTo= parentTo->biChildren[rowTo];
	struct BufferItem *	rowNodeFrom= parentFrom->biChildren[rowFrom];

	if  ( docReinsertNodes( eo, rowNodeTo, rowNodeFrom, colTo, es ) )
	    { LDEB(colTo); return -1;	}
	}

    return 0;
    }

static int docUndoInsertColumnInRow(
				EditOperation *		eo,
				struct BufferItem *	rowNode,
				const PropertyMask *	rpSetMask,
				const PropertyMask *	cpSetMask,
				RowProperties *		rpSet,
				int			selectionPosition,
				const int		col )
    {
    int			wide;

    const int		count= 1;

    int			sectionsDeleted= 0;
    int			firstParaDeleted= -1;
    int			paragraphsDeleted= 0;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    {
    const RowProperties *	rpOld;

    rpOld= rowNode->biRowProperties;
    wide= rpOld->rpCells[col].cpWide;

    if  ( docCopyRowProperties( rpSet, rpOld, dam0 ) )
	{ LDEB(1); return -1;	}
    }

    docEditDeleteNodes( eo, &sectionsDeleted,
			&firstParaDeleted, &paragraphsDeleted,
			rowNode, col, count );

    docDeleteColumnsFromRow( rpSet, col, count );

    switch( selectionPosition )
	{
	case SELposBEFORE:
	    docRowPropertiesMakeColWider( rpSet, col, wide );
	    break;

	case SELposAFTER:
	    docRowPropertiesMakeColWider( rpSet, col- 1, wide );
	    break;

	default:
	    LDEB(selectionPosition);
	    break;
	}

    if  ( docChangeRowNodeProperties( (PropertyMask *)0, (PropertyMask *)0,
				rowNode, rpSetMask, cpSetMask, rpSet,
				0, rpSet->rpCellCount,
				eo->eoDocument, dam0 ) )
	{ LDEB(wide); return -1;	}

    return 0;
    }

int docUndoInsertColumn(	EditOperation *		eo,
				const EditStep *	es )
    {
    int				rval= 0;

    struct BufferItem *		parentNode;
    int				col;
    int				row;
    int				row0;
    int				row1;

    RowProperties		rpSet;
    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;

    docInitRowProperties( &rpSet );
    utilPropMaskClear( &rpSetMask );
    PROPmaskADD( &rpSetMask, RPprop_CELL_COUNT );
    PROPmaskADD( &rpSetMask, RPprop_CELL_PROPS );

    utilPropMaskClear( &cpSetMask );
    PROPmaskADD( &cpSetMask, CLpropWIDTH );

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docEditIncludeNodeInReformatRange( eo, parentNode );

    for ( row= row0; row <= row1; row++ )
	{
	if  ( docUndoInsertColumnInRow( eo, parentNode->biChildren[row],
					&rpSetMask, &cpSetMask, &rpSet,
					es->esSelectionPosition, col ) )
	    { LLDEB(es->esSelectionPosition,col); rval= -1; goto ready;	}
	}

  ready:

    docCleanRowProperties( &rpSet );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a header or a footer.				*/
/*									*/
/*  1)  Determine wat kind of header or footer the source document	*/
/*	contains.							*/
/*  2)  Find the corresponding header or footer in the target document	*/
/*  3)  Rebuild the header or footer in the target.			*/
/*  4)  And copy the source tree to it.					*/
/*									*/
/************************************************************************/

int docUndoDeleteHeaderFooter(	EditOperation *		eo,
				const EditStep *	es )
    {
    DocumentPosition		dpFrom;

    int				i;
    const struct DocumentTree *	sourceTree= (const struct DocumentTree *)0;
    int				fromCount;

    struct DocumentTree *	targetTree= (struct DocumentTree *)0;
    const int			ownerNumber= -1;
    const int			to= 0;

    struct BufferItem *		sectNodeFrom;

    /*  1  */
    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}

    sectNodeFrom= docGetSectNode( dpFrom.dpNode );

    fromCount= 0;
    for ( i= 0; i < DOC_HeaderFooterTypeCount; i++ )
	{
	const struct DocumentTree *	dt;

	dt= docSectionHeaderFooter( sectNodeFrom, (unsigned char *)0,
					es->esSourceDocument->bdProperties,
					DOC_HeaderFooterTypes[i] );

	if  ( dt && dt->dtRoot )
	    { sourceTree= dt; fromCount++;	}
	}

    if  ( fromCount != 1 || ! sourceTree )
	{ LXDEB(fromCount,sourceTree); return -1;	}

    docMoveEditOperationToBodySect( eo, eo->eoBodySectNode );

    /*  2  */
    targetTree= docSectionHeaderFooter( eo->eoBodySectNode, (unsigned char *)0,
					    eo->eoDocument->bdProperties,
					    sourceTree->dtRoot->biTreeType );
    if  ( ! targetTree || targetTree->dtRoot )
	{ XDEB(targetTree); return -1;	}

    /*  3  */
    targetTree->dtRoot= docMakeTreeRoot( eo->eoDocument, targetTree,
					eo->eoBodySectNode, ownerNumber,
					sourceTree->dtRoot->biTreeType );
    if  ( ! targetTree->dtRoot )
	{ XDEB(targetTree->dtRoot); return -1;	}

    /*  4  */
    if  ( docReinsertNodes( eo, targetTree->dtRoot, sourceTree->dtRoot, to, es ) )
	{ LDEB(to); return -1;	}

    docInvalidateTreeLayout( targetTree );
    docEditIncludeNodeInReformatRange( eo, eo->eoBodySectNode );

    if  ( docSelectionSameScope( &(es->esOldSelectionScope),
			    &(targetTree->dtRoot->biSectSelectionScope) )	&&
	  es->esOldPage >= 0						)
	{
	targetTree->dtPageSelectedUpon= es->esOldPage;

	if  ( es->esOldColumn >= 0 )
	    { targetTree->dtColumnSelectedIn= es->esOldColumn;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo a shift-indent.						*/
/*									*/
/************************************************************************/

int docUndoShiftIndent(	EditOperation *		eo,
			const EditStep *	es )
    {
    int				rval= 0;

    DocumentCopyJob		dcj;

    PropertyMask		ppSetMask;

    DocumentSelection		dsTo;
    DocumentSelection		dsFrom;
    const int			direction= 1;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );
    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );

    docInitDocumentCopyJob( &dcj );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docInitDocumentSelection( &dsTo );
    docInitDocumentSelection( &dsFrom );

    docSetRangeSelection( &dsTo, &(eo->eoHeadDp), &(eo->eoTailDp), direction );
    docSelectWholeBody( &dsFrom, es->esSourceDocument );

    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
	    (const TextAttribute *)0, (const PropertyMask *)0,
	    &ppSetMask, /* pp */
	    (const PropertyMask *)0, /* cp */
	    (const PropertyMask *)0, /* rp */
	    (const PropertyMask *)0, /* sp */
	    (const PropertyMask *)0 ) ) /* dp */
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of paragraphs.				*/
/*									*/
/************************************************************************/

int docUndoDeletePara(	EditOperation *		eo,
			const EditStep *	es )
    {
    DocumentPosition		dpFrom;

    struct BufferItem *		parentTo;
    int				to;
    struct BufferItem *		parentFrom;

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}

    parentFrom= dpFrom.dpNode->biParent;
    parentTo= eo->eoTailDp.dpNode->biParent;
    to= eo->eoTailDp.dpNode->biNumberInParent;

    if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	{ LDEB(to); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of rows.				*/
/*									*/
/************************************************************************/

int docUndoDeleteRow(	EditOperation *		eo,
			const EditStep *	es )
    {
    int				rval= 0;

    DocumentPosition		dpFrom;

    struct BufferItem *		parentTo;
    int				to;
    struct BufferItem *		parentFrom;

    struct BufferItem *		rowNodeFrom;
    struct BufferItem *		rowNodeTo;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rowNodeFrom= docGetRowNode( dpFrom.dpNode );
    if  ( ! rowNodeFrom )
	{ XDEB(rowNodeFrom); rval= -1; goto ready;	}
    parentFrom= rowNodeFrom->biParent;

    rowNodeTo= docGetRowNode( eo->eoTailDp.dpNode );
    if  ( rowNodeTo )
	{
	parentTo= rowNodeTo->biParent;
	to= rowNodeTo->biNumberInParent;

	if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	    { LDEB(to); rval= -1; goto ready;	}
	}
    else{
	if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*  Do not copy paragraph properties */

	if  ( docEditIncludeDocument( eo, &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a property update.						*/
/*									*/
/************************************************************************/

int docUndoUpdProps(	EditOperation *		eo,
			const EditStep *	es )
    {
    int				rval= 0;

    DocumentCopyJob		dcj;

    int				haveTextUpdate;
    int				haveNodeUpdate;
    int				targetIsIbar;

    DocumentSelection		dsTo;
    DocumentSelection		dsFrom;
    const int			direction= 1;

    const PropertyMask *	taSetMask= (const PropertyMask *)0;
    const TextAttribute *	taSet= (const TextAttribute *)0;
    TextAttribute		ta;


    docInitDocumentCopyJob( &dcj );
    textInitTextAttribute( &ta );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    haveTextUpdate= ! utilPropMaskIsEmpty( &(es->esNewStyle.dsTextMask) );

    haveNodeUpdate= ! utilPropMaskIsEmpty( &(es->esNewStyle.dsParaMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsCellMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsRowMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsSectMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewDocPropMask) );

    if  ( ! haveTextUpdate && ! haveNodeUpdate )
	{ goto ready;	}

    docInitDocumentSelection( &dsTo );
    docInitDocumentSelection( &dsFrom );

    docSetRangeSelection( &dsTo, &(eo->eoHeadDp), &(eo->eoTailDp), direction );
    docSelectWholeBody( &dsFrom, es->esSourceDocument );

    /*  Do not base on source: a text-only change results in a 1 space doc */
    targetIsIbar= docIsIBarSelection( &dsTo );
    if  ( targetIsIbar )
	{
	if  ( ! docSelectionSingleParagraph( &dsFrom )			||
	      dsFrom.dsTail.dpStroff > dsFrom.dsHead.dpStroff+ 1	)
	    {
	    LDEB(docSelectionSingleParagraph(&dsFrom));
	    LLDEB(dsFrom.dsTail.dpStroff,dsFrom.dsHead.dpStroff);
	    }
	else{
	    docSetIBarSelection( &dsFrom, &(dsFrom.dsHead) );

	    docGetSelectionAttributes( &ta, (PropertyMask *)0,
					    es->esSourceDocument, &dsFrom );

	    taSet= &ta;
	    taSetMask= &(es->esNewStyle.dsTextMask);
	    }
	}

    if  ( haveTextUpdate )
	{
	if  ( haveNodeUpdate || targetIsIbar )
	    {
	    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
		    taSet, taSetMask, /* ta */
		    &(es->esNewStyle.dsParaMask), /* pp */
		    &(es->esNewStyle.dsCellMask), /* cp */
		    &(es->esNewStyle.dsRowMask), /* rp */
		    &(es->esNewStyle.dsSectMask), /* sp */
		    &(es->esNewDocPropMask) ) ) /* dp */
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	/*  Do not copy paragraph properties */

	if  ( targetIsIbar )
	    {
	    TextAttribute	taIbar;

	    taIbar= eo->eoSavedTextAttribute;
	    textUpdateTextAttribute( (struct PropertyMask *)0, &taIbar,
					&(es->esNewStyle.dsTextMask),
					&(es->esNewStyle.dsTextAttribute) );
	    }
	else{
	    if  ( docEditIncludeDocument( eo, &dcj ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}
    else{
	if  ( haveNodeUpdate )
	    {
	    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
		    (const TextAttribute *)0, (const PropertyMask *)0,
		    &(es->esNewStyle.dsParaMask), /* pp */
		    &(es->esNewStyle.dsCellMask), /* cp */
		    &(es->esNewStyle.dsRowMask), /* rp */
		    &(es->esNewStyle.dsSectMask), /* sp */
		    &(es->esNewDocPropMask) ) ) /* dp */
		{
		LDEB(1);
		docListNode(eo->eoDocument,0,es->esSourceDocument->bdBody.dtRoot,0);
		rval= -1; goto ready;
		}
	    }
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of sections.				*/
/*									*/
/************************************************************************/

int docUndoDeleteSect(	EditOperation *		eo,
			const EditStep *	es )
    {
    DocumentPosition		dpFrom;

    struct BufferItem *		parentTo;
    int				to;
    struct BufferItem *		parentFrom;

    struct BufferItem *		sectNodeFrom;
    struct BufferItem *		sectNodeTo;

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}

    sectNodeFrom= docGetSectNode( dpFrom.dpNode );
    sectNodeTo= docGetSectNode( eo->eoTailDp.dpNode );
    if  ( ! sectNodeFrom || ! sectNodeTo )
	{ XXDEB(sectNodeFrom,sectNodeTo); return -1;	}

    parentFrom= sectNodeFrom->biParent;
    parentTo= sectNodeTo->biParent;
    to= sectNodeTo->biNumberInParent;

    if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	{ LDEB(to); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo a replacement.							*/
/*									*/
/************************************************************************/

int docUndoReplace(	EditOperation *		eo,
			const EditStep *	es,
			int			copyTailParaProps )
    {
    int				rval= 0;

    DocumentPosition		dpFrom;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( ! docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{
	if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	dcj.dcjCopyTailParaProperties= copyTailParaProps;

	if  ( docEditIncludeDocument( eo, &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( docDeleteSelection( eo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }
