/************************************************************************/
/*									*/
/*  Traverse a selection of a tree of nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	"docTreeScanner.h"
#   include	<docScanner.h>
#   include	<docTreeType.h>
#   include	<docLayoutPosition.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	"docTreeNode.h"
#   include	"docBuf.h"
#   include	"docSelect.h"
#   include	"docSectHeadersFooters.h"
#   include	"docNotes.h"
#   include	"docDocumentNote.h"
#   include	<docEditRange.h>
#   include	<docFieldScanner.h>
#   include	<docFieldKind.h>
#   include	<docDocumentField.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct TreeScanner
    {
					/**
					 *  The selection that we are scanning.
					 */
    const struct DocumentSelection *	tsSelection;

					/**
					 *  The document that is the object of 
					 *  the scan.
					 */
    const struct BufferDocument *	tsDocument;

					/**
					 *  The document tree that we currently
					 * scan.
					 */
    struct DocumentTree *		tsTree;

					/**
					 *  The section in the document that 
					 *  is relevant for the current branch 
					 *  of the scan.
					 */
    const struct BufferItem *		tsBodySectNode;

					/**
					 *  Pass-through object that this 
					 *  scan manipulates.
					 */
    void *				tsThrough;

					/**
					 *  Visitor function that is called 
					 *  when we enter a node.
					 */
    NodeVisitor				tsEnterNode;

					/**
					 *  Visitor function that is called 
					 *  when we leave a node.
					 */
    NodeVisitor				tsLeaveNode;

					/**
					 *  Visitor function that is called 
					 *  when we enter a tree.
					 */
    TreeVisitor				tsEnterTree;

					/**
					 *  Visitor function that is called 
					 *  when we leave a tree.
					 */
    TreeVisitor				tsLeaveTree;

					/**
					 *  Flags that control the scan.
					 */
    unsigned int			tsFlags;

					/**
					 *  Selection that is potentially 
					 *  used to intersect the paragraph 
					 *  nodes with the selection.
					 *  (allocate it here to save stack 
					 *  space)
					 */
    DocumentSelection			tsParaSelection;
    } TreeScanner;

/************************************************************************/

static int docScanNode(	TreeScanner *			ts,
			struct BufferItem *		node );

static int docScanExcursion(	const TreeScanner *	parentScanner,
				struct DocumentTree *	tree );

/************************************************************************/
/*									*/
/*  Make an excursion into a field. If it is a note or an annotation,	*/
/*  We scan the text of the associated DocumentNote. To avoid recursion,*/
/*  the caller must not call docEnterTreeField() from the DOM of a note.*/
/*									*/
/************************************************************************/

static int docEnterTreeField(
		    struct DocumentField *		df,
		    const struct EditRange *		er,
		    void *				vts )
    {
    int				ret= SCANadviceOK;
    const TreeScanner *		parentScanner= (const TreeScanner *)vts;

    if  ( df->dfKind == DOCfkCHFTN )
	{
	DocumentNote *	dn= docGetNoteOfField( df, parentScanner->tsDocument );

	if  ( ! dn )
	    { XDEB(dn); return -1;	}

	ret= docScanExcursion( parentScanner, &(dn->dnDocumentTree) );
	switch( ret )
	    {
	    case SCANadviceOK:
	    case SCANadviceSTOP:
		break;
	    case SCANadviceSKIP:
	    default:
		LDEB(ret); return -1;
	    }
	}

    return ret;
    }

/************************************************************************/
/*									*/
/*  Make an excursion into a tree that belongs to a certain node.	*/
/*									*/
/************************************************************************/

static int docScanExcursion(	const TreeScanner *	parentScanner,
				struct DocumentTree *	tree )
    {
    TreeScanner		ts= *parentScanner;

    int			ret;

    ts.tsSelection= (const struct DocumentSelection *)0;
    ts.tsTree= tree;

    if  ( ts.tsEnterTree )
	{
	ret= (*ts.tsEnterTree)( tree, ts.tsBodySectNode, ts.tsThrough );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
		return SCANadviceOK;
	    default:
		LDEB(ret); return -1;
	    }
	}

    ret= docScanNode( &ts, tree->dtRoot );
    switch( ret )
	{
	case SCANadviceOK:
	    break;
	case SCANadviceSTOP:
	    return ret;
	case SCANadviceSKIP:
	default:
	    LDEB(ret); return -1;
	}

    if  ( ( ts.tsFlags & FLAGtsSCAN_FOOT_END_NOTES )	&&
	  ! docIsNoteType( tree->dtRoot->biTreeType )	)
	{
	ret= docScanFieldsInRange( &(tree->dtRootFields), (EditRange *)0,
			docEnterTreeField, (TreeFieldVisitor)0, (void *)&ts );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LDEB(ret); return -1;
	    }
	}

    if  ( ts.tsLeaveTree )
	{
	ret= (*ts.tsLeaveTree)( tree, ts.tsBodySectNode, ts.tsThrough );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LDEB(ret); return -1;
	    }
	}

    return SCANadviceOK;
    }

static int docScanExcursions(	const TreeScanner *	parentScanner,
				struct DocumentTree **	trees,
				int			count )
    {
    int			ret;
    int			i;

    for ( i= 0; i < count; i++ )
	{
	if  ( ! trees[i]->dtRoot )
	    { continue;	}

	ret= docScanExcursion( parentScanner, trees[i] );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    return SCANadviceOK;
    }

/************************************************************************/
/*									*/
/*  Make an excursion into the separators that belong to the document	*/
/*  (body)								*/
/*									*/
/************************************************************************/

static int docScanBodySeparators(	const TreeScanner *	bodyScanner )
    {
    const BufferDocument *	bd= bodyScanner->tsDocument;

    struct DocumentTree *	trees[6];
    int				count= 0;

    if  ( bd->bdFtnsep.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdFtnsep);	}
    if  ( bd->bdFtnsepc.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdFtnsepc);	}
    if  ( bd->bdFtncn.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdFtncn);	}

    if  ( bd->bdAftnsep.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdAftnsep);	}
    if  ( bd->bdAftnsepc.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdAftnsepc); }
    if  ( bd->bdAftncn.dtRoot )
	{ trees[count++]= (DocumentTree *)&(bd->bdAftncn);	}

    return docScanExcursions( bodyScanner, trees, count );
    }

/************************************************************************/
/*									*/
/*  Make an excursion into the headers and footers of a section		*/
/*									*/
/************************************************************************/

static int docScanSectionHeadersFooters(
					const TreeScanner *	bodyScanner,
					SectHeadersFooters *	shf )
    {
    struct DocumentTree *	trees[8];
    int				count= 0;

    if  ( shf->shfFirstPageHeader.dtRoot )
	{ trees[count++]= &(shf->shfFirstPageHeader);	}
    if  ( shf->shfLeftPageHeader.dtRoot )
	{ trees[count++]= &(shf->shfLeftPageHeader);	}
    if  ( shf->shfRightPageHeader.dtRoot )
	{ trees[count++]= &(shf->shfRightPageHeader);	}
    if  ( shf->shfLastPageHeader.dtRoot )
	{ trees[count++]= &(shf->shfLastPageHeader);	}

    if  ( shf->shfFirstPageFooter.dtRoot )
	{ trees[count++]= &(shf->shfFirstPageFooter);	}
    if  ( shf->shfLeftPageFooter.dtRoot )
	{ trees[count++]= &(shf->shfLeftPageFooter);	}
    if  ( shf->shfRightPageFooter.dtRoot )
	{ trees[count++]= &(shf->shfRightPageFooter);	}
    if  ( shf->shfLastPageFooter.dtRoot )
	{ trees[count++]= &(shf->shfLastPageFooter);	}

    return docScanExcursions( bodyScanner, trees, count );
    }

/************************************************************************/
/*									*/
/*  Scan a buffer node, hierarchically descending to the leaves of the	*/
/*  document tree.							*/
/*									*/
/************************************************************************/

static int docScanNode(	TreeScanner *			ts,
			struct BufferItem *		node )
    {
    int				i;
    int				ret;
    int				isRow;
    const RowProperties *	rp= (const RowProperties *)0;
    const DocumentSelection *	ds= ts->tsSelection;

    /* Not needed: They are in the same tree.
    int filter=
	ts->tsSelection && docSelectionSameRoot( ts->tsSelection, node );
    */

    if  ( node->biTreeType == DOCinBODY			&&
	  node->biLevel == DOClevSECT			)
	{ ts->tsBodySectNode= node;	}

    if  ( ds )
	{
	if  ( docCompareNodePositionToSelection( node, ds ) != 0 )
	    { return SCANadviceOK;	}

	if  ( node->biLevel == DOClevPARA )
	    {
	    if  ( ds->dsHead.dpNode != node		&&
		  ds->dsTail.dpNode != node		)
		{ ds= (const DocumentSelection *)0;	}
	    else{
		DocumentSelection *	dsTo= &(ts->tsParaSelection);

		ts->tsParaSelection= *ds;

		if  ( ds->dsHead.dpNode != node )
		    { docHeadPosition( &(dsTo->dsHead), node ); }
		if  ( ds->dsTail.dpNode != node )
		    { docTailPosition( &(dsTo->dsTail), node ); }

		if  ( ds->dsDirection >= 0 )
		    { dsTo->dsAnchor= ts->tsParaSelection.dsHead; }
		else{ dsTo->dsAnchor= ts->tsParaSelection.dsTail; }

		ds= dsTo;
		}
	    }
	}

    if  ( ts->tsEnterNode )
	{
	ret= (*ts->tsEnterNode)( node, ds, ts->tsBodySectNode, ts->tsThrough );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
		return SCANadviceOK;
	    default:
		LDEB(ret); return -1;
	    }
	}

    isRow= docIsRowNode( node );
    if  ( isRow )
	{ rp= node->biRowProperties;	}

    for ( i= 0; i < node->biChildCount; i++ )
	{
	int		selected= 1;

	if  ( isRow )
	    {
	    const CellProperties *	cp= &(rp->rpCells[i]);

	    if  ( i >= rp->rpCellCount )
		{ LLDEB(i,rp->rpCellCount); continue;	}

	    if  ( ! ( ts->tsFlags & FLAGtsSCAN_MERGED_CELLS )	&&
		  CELL_MERGED( cp )				)
		{ continue;	}

	    ret= docCompareCellPositionToSelection( &selected,
					node->biChildren[i], ts->tsSelection );
	    }
	else{
	    ret= docCompareNodePositionToSelection(
					node->biChildren[i], ts->tsSelection );
	    }
	if  ( ret > 0 )
	    { break;	}
	if  ( ret < 0 || ! selected )
	    { continue;	}

	ret= docScanNode( ts, node->biChildren[i] );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    if  ( node->biTreeType == DOCinBODY				&&
	  node->biLevel == DOClevSECT				&&
	  ( ts->tsFlags & FLAGtsSCAN_SECTION_HEADERS_FOOTERS )	&&
	  node->biSectHeadersFooters				)
	{
	ret= docScanSectionHeadersFooters( ts, node->biSectHeadersFooters );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    if  ( node->biTreeType == DOCinBODY			&&
	  node->biLevel == DOClevBODY			&&
	  ( ts->tsFlags & FLAGtsSCAN_BODY_SEPARATORS )	)
	{
	ret= docScanBodySeparators( ts );
	switch( ret )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    if  ( ts->tsLeaveNode )
	{
	ret= (*ts->tsLeaveNode)( node, ds, ts->tsBodySectNode, ts->tsThrough );
	switch( ret )
	    {
	    case SCANadviceOK:
	    case SCANadviceSTOP:
		return ret;
	    case SCANadviceSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    return SCANadviceOK;
    }

static void docInitTreeScanner(	TreeScanner *		ts )
    {
    ts->tsSelection= (const struct DocumentSelection *)0;
    ts->tsDocument= (const struct BufferDocument *)0;
    ts->tsTree= (struct DocumentTree *)0;
    ts->tsBodySectNode= (const struct BufferItem *)0;
    ts->tsThrough= (void *)0;
    ts->tsEnterNode= (NodeVisitor)0;
    ts->tsLeaveNode= (NodeVisitor)0;
    ts->tsFlags= 0;

    docInitDocumentSelection( &(ts->tsParaSelection) );

    return;
    }

int docScanTree(	struct BufferDocument *		bd,
			struct DocumentTree *		tree,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			TreeVisitor			enterTree,
			TreeVisitor			leaveTree,
			int				flags,
			void *				through )
    {
    struct BufferItem *	root= tree->dtRoot;
    TreeScanner		ts;
    int			res;
    struct BufferItem *	bodySectNode= (struct BufferItem *)0;

    docInitTreeScanner( &ts );

    if  ( ! root )
	{ XDEB(root); return -1;	}

    if  ( docGetTreeOfNode( (struct DocumentTree **)0,
					    &bodySectNode, bd, root ) )
	{ LDEB(1); return -1;	}

    ts.tsDocument= bd;
    ts.tsTree= tree;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsEnterTree= enterTree;
    ts.tsLeaveTree= leaveTree;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, root );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    if  ( res == SCANadviceOK				&&
	  ( ts.tsFlags & FLAGtsSCAN_FOOT_END_NOTES )	&&
	  ! docIsNoteType( root->biTreeType )		)
	{
	res= docScanFieldsInRange( &(tree->dtRootFields), (EditRange *)0,
		    docEnterTreeField, (TreeFieldVisitor)0, (void *)&ts );
	switch( res )
	    {
	    case SCANadviceOK:
	    case SCANadviceSTOP:
		break;
	    case SCANadviceSKIP:
	    default:
		LDEB(res); return -1;
	    }
	}

    return res;
    }

int docScanSelection(	struct BufferDocument *			bd,
			const struct DocumentSelection *	ds,
			NodeVisitor				enterNode,
			NodeVisitor				leaveNode,
			TreeVisitor				enterTree,
			TreeVisitor				leaveTree,
			int					flags,
			void *					through )
    {
    struct BufferItem *		root= bd->bdBody.dtRoot;
    TreeScanner			ts;
    int				res;
    struct DocumentTree *	tree= (struct DocumentTree *)0;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;

    docInitTreeScanner( &ts );

    if  ( ds )
	{
	root= docGetSelectionRoot( &tree, &bodySectNode, bd, ds );
	if  ( ! root )
	    { XDEB(root); return -1;	}
	}
    else{
	if  ( docGetTreeOfNode( &tree, &bodySectNode, bd, root ) )
	    { LDEB(1); return -1;	}
	}

    ts.tsSelection= ds;
    ts.tsDocument= bd;
    ts.tsTree= tree;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsEnterTree= enterTree;
    ts.tsLeaveTree= leaveTree;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, root );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    if  ( res == SCANadviceOK				&&
	  ( ts.tsFlags & FLAGtsSCAN_FOOT_END_NOTES )	&&
	  ! docIsNoteType( root->biTreeType )		)
	{
	if  ( ds )
	    {
	    EditRange		er;

	    docSetEditRange( &er, ds );

	    res= docScanFieldsInRange( &(tree->dtRootFields), &er,
			docEnterTreeField, (TreeFieldVisitor)0, (void *)&ts );
	    switch( res )
		{
		case SCANadviceOK:
		case SCANadviceSTOP:
		    break;
		case SCANadviceSKIP:
		default:
		    LDEB(res); return -1;
		}
	    }
	else{
	    res= docScanFieldsInRange( &(tree->dtRootFields), (EditRange *)0,
			docEnterTreeField, (TreeFieldVisitor)0, (void *)&ts );
	    switch( res )
		{
		case SCANadviceOK:
		case SCANadviceSTOP:
		    break;
		case SCANadviceSKIP:
		default:
		    LDEB(res); return -1;
		}
	    }
	}

    return res;
    }

int docScanTreeNode(	struct BufferDocument *		bd,
			struct BufferItem *		node,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			TreeVisitor			enterTree,
			TreeVisitor			leaveTree,
			int				flags,
			void *				through )
    {
    TreeScanner			ts;
    int				res;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;
    struct DocumentTree *	tree;

    docInitTreeScanner( &ts );

    if  ( docGetTreeOfNode( &tree, &bodySectNode, bd, node ) )
	{ LDEB(1); return -1;	}

    ts.tsSelection= (const struct DocumentSelection *)0;
    ts.tsDocument= bd;
    ts.tsTree= tree;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsEnterTree= enterTree;
    ts.tsLeaveTree= leaveTree;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, node );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    if  ( res == SCANadviceOK				&&
	  ( ts.tsFlags & FLAGtsSCAN_FOOT_END_NOTES )	&&
	  ! docIsNoteType( node->biTreeType )		)
	{
	DocumentSelection	dsNode;
	EditRange		erNode;

	docSetNodeSelection( &dsNode, node );
	docSetEditRange( &erNode, &dsNode );

	res= docScanFieldsInRange( &(tree->dtRootFields), &erNode,
			docEnterTreeField, (TreeFieldVisitor)0, (void *)&ts );
	switch( res )
	    {
	    case SCANadviceOK:
	    case SCANadviceSTOP:
		break;
	    case SCANadviceSKIP:
	    default:
		LDEB(res); return -1;
	    }
	}

    return res;
    }
