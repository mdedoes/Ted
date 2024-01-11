/************************************************************************/
/*									*/
/*  Save the nodes in a BufferDocument to an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<docTreeType.h>
#   include	<docTreeScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docSelect.h>
#   include	<docBuf.h>
#   include	<textOfficeCharset.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfFlags.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start a paragraph node.						*/
/*									*/
/*  0)  While saving a selection.. Make sure that section properties	*/
/*	are saved before the first paragraph in the document.		*/
/*  1)  When we are saving a selection, and the selection is inside a	*/
/*	table cell, do not set the \intbl flag. This is only activated	*/
/*	for copy/paste where it is essential and for undo/redo where it	*/
/*	is irrelevant.							*/
/*  2)  To make WP 8 happy, always save 'intbl' for the first paragraph	*/
/*	in a table row.							*/
/*									*/
/************************************************************************/

static int docRtfEnterParaNode(	RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				const DocumentSelection *	ds )
    {
    int			flattenCell= 0;
    int			firstInRow= 0;

    /*  0  */
    if  ( rw->rwCurrentTree->ptSelection			&&
	  paraNode->biTreeType == DOCinBODY			&&
	  ! rw->rwSectionPropertiesSaved			)
	{
	const struct BufferItem *	sectNode;

	sectNode= docGetSectNode( (struct BufferItem *)paraNode );
	if  ( ! sectNode )
	    { XDEB(sectNode); return -1;	}

	if  ( docRtfSaveSectionPropertiesOfNode( rw, sectNode ) )
	    { LDEB(1); return -1;	}
	}

    /*  1  */
    if  ( ! ( rw->rwSaveFlags & RTFflagEMIT_CELL ) )
	{
	if  ( rw->rwCurrentTree->ptSelectionIsSingleCell )
	    { flattenCell= 1;	}
	}
    else{
	if  ( paraNode->biParaProperties->ppTableNesting > 0		&&
	      rw->rwCurrentTree->ptSelection				&&
	      rw->rwCurrentTree->ptSelectionIsSingleCell		)
	    {
	    const struct BufferItem *	rowNode;

	    rowNode= docGetRowNode( (struct BufferItem *)paraNode );
	    if  ( ! rowNode )
		{ XDEB(rowNode); return -1;	}

	    if  ( rw->rwCurrentTree->ptTableNesting == 0	&&
		  docRtfPushTable( rw, rowNode )		)
		{ LDEB(1);	}
	    }
	}

    if  ( paraNode->biParaProperties->ppTableNesting > 0 )
	{
	struct BufferItem *	rowNode;
	DocumentPosition	dpFirst;

	rowNode= docGetRowNode( (struct BufferItem *)paraNode );
	if  ( ! rowNode					||
	      docHeadPosition( &dpFirst, rowNode )	)
	    { XDEB(rowNode);	}
	else{ firstInRow= paraNode == dpFirst.dpNode;	}
	}

    if  ( docRtfSaveParaNode( rw, paraNode, ds, flattenCell, firstInRow ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start an arbitrary node.						*/
/*									*/
/************************************************************************/

static int docRtfWriteEnterNode( struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voidrw )
    {
    RtfWriter *	rw= (RtfWriter *)voidrw;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevCELL:
	    break;

	case DOClevSECT:

	    if  ( node->biTreeType == DOCinBODY			&&
		  docRtfSaveSectionPropertiesOfNode( rw, node )	)
		{ LDEB(1); return -1;	}

	    if  ( ! rw->rwCurrentTree->ptSelection		&&
		  node->biTreeType == DOCinBODY			&&
		  docRtfSaveSectHeadersFooters( rw, node )	)
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docRtfEnterRowNode( rw, node ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( rw->rwCurrentTree->ptTableNesting > 0	&&
		      docRtfPopTable( rw )			)
		    { LDEB(rw->rwCurrentTree->ptTableNesting);	}
		}
	    break;

	case DOClevPARA:
	    if  ( docRtfEnterParaNode( rw, node, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return 0;
    }

static int docRtfWriteLeaveNode( struct BufferItem *		node,
				const DocumentSelection *	x_ds,
				const struct BufferItem *	bodySectNode,
				void *				voidrw )
    {
    RtfWriter *	rw= (RtfWriter *)voidrw;

    switch( node->biLevel )
	{
	struct BufferItem *	parentNode;

	case DOClevBODY:
	    break;

	case DOClevCELL:
	    parentNode= node->biParent;

	    if  ( docIsRowNode( parentNode ) )
		{
		int	tableNesting= docTableNesting( parentNode );

		if  ( rw->rwcLastNodeLevel != DOClevPARA )
		    { docRtfSaveParaTableNesting( rw, tableNesting );	}

		if  ( ! rw->rwCurrentTree->ptSelection			||
		      ! rw->rwCurrentTree->ptSelectionIsSingleCell	)
		    {
		    if  ( tableNesting > 1 )
			{ docRtfWriteTag( rw, "nestcell" );		}
		    else{ docRtfWriteTag( rw, RTFtag_cell );	}
		    }

		if  ( node->biNumberInParent != parentNode->biChildCount- 1 )
		    { docRtfWriteNextLine( rw );	}
		}
	    break;

	case DOClevSECT:
	    while( rw->rwCurrentTree->ptTableNesting > 0 )
		{
		if  ( docRtfPopTable( rw ) )
		    { LDEB(rw->rwCurrentTree->ptTableNesting);	}
		}

	    if  ( node->biParent					&&
		  node->biNumberInParent < node->biParent->biChildCount- 1 )
		{ docRtfWriteTag( rw, RTFtag_sect );	}

	    docRtfWriteNextLine( rw );
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docRtfLeaveRowNode( rw, node ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    rw->rwcLastNodeLevel= node->biLevel;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save an inidividual header, footer, or footnote.			*/
/*									*/
/************************************************************************/

static void docInitPushedTree(	PushedTree *			pt )
    {
    pt->ptSelection= (const struct DocumentSelection *)0;
    pt->ptSelectionIsSingleCell= 0;
    pt->ptTableNesting= 0;

    docInitParagraphProperties( &(pt->ptParagraphProperties) );
    }

static void docCleanPushedTree(	PushedTree *			pt )
    {
    docCleanParagraphProperties( &(pt->ptParagraphProperties) );
    }

int docRtfSaveDocumentTree(	RtfWriter *			rw,
				const char *			tag,
				struct DocumentTree *		tree,
				int				evenIfAbsent,
				int				forcePar )
    {
    int			rval= 0;
    const int		flags= FLAGtsSCAN_MERGED_CELLS;

    PushedTree *	savePushedTree= rw->rwCurrentTree;

    PushedTree		pt;

    docInitPushedTree( &pt );

    if  ( ! tree->dtRoot )
	{
	if  ( evenIfAbsent )
	    {
	    docRtfWriteDestinationBegin( rw, tag );
	    docRtfWriteDestinationEnd( rw );
	    }

	goto ready;
	}

    if  ( tree->dtRoot->biLevel != DOClevSECT )
	{ SDEB(docLevelStr(tree->dtRoot->biLevel)); rval= -1; goto ready;	}

    docRtfWriteDestinationBegin( rw, tag );
    if  ( docRtfPushAttribute( rw ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rw->rwCurrentTree= &pt;

    docRtfWriteTag( rw, RTFtag_pard );
    docRtfWriteSwitchToPlain( rw );
    docRtfWriteNextLine( rw );

    if  ( docScanTree( rw->rwDocument, tree,
		    docRtfWriteEnterNode, docRtfWriteLeaveNode,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)rw ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    /********************************************************************/
    /*  MS-Word does not pick up paragraph properties of headers and	*/
    /*  footers without a \par						*/
    /********************************************************************/
    if  ( forcePar )
	{
	DocumentPosition	dp;

	if  ( ! docTailPosition( &dp, tree->dtRoot )		&&
	      dp.dpNode->biParaProperties->ppTableNesting == 0	)
	    { docRtfWriteTag( rw, RTFtag_par );	}
	}

    textInitTextAttribute( &(rw->rwTextAttribute) );
    rw->rwTextCharset= FONTcharsetANSI;

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    if  ( docRtfPopAttribute( rw ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    rw->rwCurrentTree= savePushedTree;

    docCleanPushedTree( &pt );

    return rval;
    }

int docRtfWriteSelection(	RtfWriter *			rw,
				const DocumentSelection *	ds )
    {
    int			rval= 0;
    const int		flags= FLAGtsSCAN_MERGED_CELLS;

    PushedTree *	savePushedTree= rw->rwCurrentTree;

    PushedTree		pt;

    docInitPushedTree( &pt );

    if  ( ! rw->rwDocument )
	{ XDEB(rw->rwDocument); rval= -1; goto ready;	}

    if  ( ds )
	{
	pt.ptSelection= ds;

	pt.ptSelectionIsSingleCell= docSelectionInsideCell( pt.ptSelection );

	rw->rwCurrentTree= &pt;

	if  ( docScanSelection( rw->rwDocument, ds,
		    docRtfWriteEnterNode, docRtfWriteLeaveNode,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)rw ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ( rw->rwSaveFlags & RTFflagEMIT_CELL )	&&
	      rw->rwCurrentTree->ptTableNesting > 0	&&
	      pt.ptSelectionIsSingleCell		)
	    {
	    docRtfWriteTag( rw, RTFtag_row );

	    if  ( docRtfPopTable( rw ) )
		{ LDEB(rw->rwCurrentTree->ptTableNesting);	}
	    }
	}
    else{
	rw->rwCurrentTree= &pt;

	if  ( docScanTree( rw->rwDocument, &(rw->rwDocument->bdBody),
		    docRtfWriteEnterNode, docRtfWriteLeaveNode,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)rw ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    while( rw->rwCurrentTree->ptTableNesting > 0 )
	{
	if  ( docRtfPopTable( rw ) )
	    { LDEB(rw->rwCurrentTree->ptTableNesting);	}
	}

  ready:

    rw->rwCurrentTree= savePushedTree;

    docCleanPushedTree( &pt );

    return rval;
    }
