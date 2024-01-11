/************************************************************************/
/*									*/
/*  Manage documner node hierarchy from the rtf parser.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docRowNodeProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docTreeNode.h>
#   include	<docParaBuilder.h>
#   include	<docSelect.h>
#   include	<docTextParticule.h>
#   include	<docBreakKind.h>
#   include	<docRecalculateFields.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Finish a section in the document.					*/
/*									*/
/************************************************************************/

static int docRtfCloseSect(	RtfReader *		rr,
				struct BufferItem *	sectNode )
    {
    RowProperties *	rp= &(rr->rrTreeStack->rtsRowProperties);

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	PropertyMask		spUpdMask;

	utilPropMaskClear( &spUpdMask );
	utilPropMaskFill( &spUpdMask, SPprop_COUNT );

	/*  Not clear if this is desirable  */
	if  ( docUpdSectProperties( (PropertyMask *)0,
				sectNode->biSectProperties,
				&spUpdMask, &(rr->rrSectionProperties) ) )
	    { LDEB(1); return -1;	}
	}

    docCleanRowProperties( rp );
    docInitRowProperties( rp );
    rr->rrTreeStack->rtsRowCellX= rp->rpLeftIndentTwips;
    utilEmptyMemoryBuffer( &(rr->rrTreeStack->rtsRowSummary) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a cell.							*/
/*									*/
/************************************************************************/

static int docRtfCloseCell(	RtfReader *		rr,
				struct BufferItem *		cellNode )
    {
    if  ( cellNode->biChildCount == 0 )
	{ LDEB(cellNode->biChildCount); return 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a node.							*/
/*									*/
/************************************************************************/

static int docRtfCloseNode(	RtfReader *		rr,
				struct BufferItem *	node )
    {
    switch( node->biLevel )
	{
	case DOClevSECT:
	    if  ( docRtfCloseSect( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( docRtfCloseRow( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfCloseCell( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docRtfCloseParagraph( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Pop a node from the 'stack' of nodes that is built by the parser.	*/
/*									*/
/************************************************************************/

static int docRtfPopNode(		RtfReader *		rr,
					struct BufferItem *	node )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    while( rts->rtsNode )
	{
	if  ( docRtfCloseNode( rr, rts->rtsNode ) )
	    { LDEB(1); return -1;	}

	if  ( rts->rtsNode == node )
	    {
	    rts->rtsNode= rts->rtsNode->biParent;
	    rts->rtsLevel= rts->rtsNode->biLevel;
	    return 0;
	    }

	rts->rtsNode= rts->rtsNode->biParent;
	}

    XDEB(rts->rtsNode); return -1;
    }

/************************************************************************/
/*									*/
/*  Handle cell tags that relate to the document hierarchy.		*/
/*									*/
/************************************************************************/

static int docRtfHierarchyCell(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    struct BufferItem *	levelNode= (struct BufferItem *)0;
    int			paraInCell= 1;

    RtfTreeStack *	rts= rr->rrTreeStack;

    /*  1  */
    if  ( rts->rtsNode )
	{ levelNode= docGetCellNode( rts->rtsNode );	}

    if ( rcw->rcwID == DOClevCELL		&&
	 rts->rtsNode				&&
	 rts->rtsNode->biLevel == DOClevCELL	)
	{
	int	cc= rts->rtsNode->biChildCount;

	if  ( cc > 0						  &&
	      rts->rtsNode->biChildren[cc-1]->biLevel != DOClevPARA )
	      { paraInCell= 0;	}
	}

    if  ( ! rts->rtsNode )
	{ SXDEB(rcw->rcwWord,rts->rtsNode); return -1;	}

    if  ( ! levelNode						||
	  ( rts->rtsNode->biLevel <= DOClevCELL && paraInCell )	)
	{
	struct BufferItem *	paraNode= docRtfGetParaNode( rr );

	if  ( ! paraNode )
	    { SDEB(rcw->rcwWord); return -1; }

	/* cope with: {\intbl etc\par}\cell */
	if  ( paraNode->biParaProperties->ppTableNesting == 0 )
	    {
	    docSetParaTableNesting( paraNode, rr->rrDocument );
	    docRtfSetForRow( paraNode );
	    if  ( rr->rrState )
		{
		rr->rrState->rrsParagraphProperties.ppTableNesting= 
				    paraNode->biParaProperties->ppTableNesting;
		}
	    }
	}

    /* MS-Word does this: The first clue about a table is \cell */
    if  ( rts->rtsNode->biLevel == DOClevPARA			&&
	  rts->rtsNode->biParaProperties->ppTableNesting == 0	)
	{
	struct BufferItem *	rowNode= docGetRowLevelNode( rts->rtsNode );
	DocumentPosition	dpFirst;

	if  ( ! rowNode || docHeadPosition( &dpFirst, rowNode ) )
	    { XDEB(rowNode);	}
	else{
	    if  ( dpFirst.dpNode != rts->rtsNode	&&
		  rts->rtsNode->biNumberInParent > 0	)
		{
		BufferItem *	cellNode= rts->rtsNode->biParent;
		BufferItem *	freshCellNode;
		BufferItem *	freshRowNode;

		if  ( docSplitGroupNode( rr->rrDocument,
					    &freshCellNode, cellNode,
					    rts->rtsNode->biNumberInParent ) )
		    { LDEB(rts->rtsNode->biNumberInParent); return -1;	}

		if  ( docSplitGroupNode( rr->rrDocument, &freshRowNode,
					    cellNode->biParent,
					    cellNode->biNumberInParent ) )
		    { LDEB(cellNode->biNumberInParent); return -1;	}

		docRowNodeResetRowProperties( freshRowNode, rr->rrDocument );
		}
	    }

	/*LDEB(rts->rtsNode->biParaTableNesting);*/
	docParaNodeSetTableNesting( rts->rtsNode, 1, rr->rrDocument );
	docRtfSetForRow( rts->rtsNode );
	if  ( rr->rrState )
	    {
	    rr->rrState->rrsParagraphProperties.ppTableNesting= 
				rts->rtsNode->biParaProperties->ppTableNesting;
	    }
	}

    levelNode= docGetCellNode( rts->rtsNode );
    if  ( ! levelNode )
	{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
    if  ( docRtfPopNode( rr, levelNode ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle tags that relate to the document hierarchy.			*/
/*									*/
/************************************************************************/

int docRtfHierarchy(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    struct BufferItem *	levelNode= (struct BufferItem *)0;

    RtfTreeStack *	rts= rr->rrTreeStack;

    switch( rcw->rcwID )
	{
	case DOClevSECT:
	    /*  1  */
	    if  ( rts->rtsNode )
		{ levelNode= docGetSectNode( rts->rtsNode );	}
	    if  ( ! levelNode )
		{
		if  ( ! docRtfGetParaNode( rr ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}
	    if  ( ! rts->rtsNode )
		{ SXDEB(rcw->rcwWord,rts->rtsNode); return -1;	}

	    levelNode= docGetSectNode( rts->rtsNode );
	    if  ( ! levelNode )
		{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}

	    break;

	case DOClevROW:
	case DOClevNESTROW:
	    /*  1  */
	    if  ( rts->rtsNode )
		{ levelNode= docGetRowLevelNode( rts->rtsNode );	}
	    /* ignore stray \row tags rather than fail */
	    if  ( ! levelNode )
		{ LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord); return 0;	}
	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevCELL:
	case DOClevNESTCELL:
	    if  ( docRtfHierarchyCell( rcw, rr ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevPARA:
	    levelNode= docRtfGetParaNode( rr );
	    if  ( ! levelNode )
		{ SDEB(rcw->rcwWord); return -1; }

	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevNONESTTABLES:
	    /* should be a destination */
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID); break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start/Close a section.						*/
/*									*/
/************************************************************************/

static int docRtfStartSect(	RtfReader *	rr )
    {
    struct BufferItem *		sectNode;
    RtfTreeStack *		rts= rr->rrTreeStack;

    sectNode= docInsertNode( rr->rrDocument, rts->rtsNode, -1, DOClevSECT );
    if  ( ! sectNode )
	{ SXDEB(docLevelStr(rts->rtsLevel),sectNode); return -1; }
    rts->rtsNode= sectNode;

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	rts->rtsSelectionScope= sectNode->biSectSelectionScope;

	docParaBuilderSetSelectionScope( rts->rtsParagraphBuilder,
						&(rts->rtsSelectionScope) );

	if  ( docCopySectionProperties( sectNode->biSectProperties,
					 &(rr->rrSectionProperties) ) )
	    { LDEB(1); return -1;	}

	if  ( sectNode->biNumberInParent > 0 )
	    {
	    struct BufferItem *	prevSectNode;

	    prevSectNode= sectNode->biParent->biChildren[
					sectNode->biNumberInParent- 1];

	    if  ( docCopySectHeadersFooters( sectNode, rr->rrDocument,
					    prevSectNode, rr->rrDocument ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

static int docRtfStartCell(	RtfReader *	rr )
    {
    struct BufferItem *	cellNode;
    RtfTreeStack *	rts= rr->rrTreeStack;

    cellNode= docInsertNode( rr->rrDocument, rts->rtsNode, -1, DOClevCELL );
    if  ( ! cellNode )
	{ SXDEB(docLevelStr(rts->rtsLevel),cellNode); return -1; }
    rts->rtsNode= cellNode;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish the tree that is currently under construction.		*/
/*									*/
/************************************************************************/

int docRtfFinishCurrentTree(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    int			rval= 0;
    RtfTreeStack *	rts= rr->rrTreeStack;

    if  ( docRtfPopTreeFromFieldStack( rr, rts ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRenumberSeqFields( (int *)0, rts->rtsTree, rr->rrDocument );

    while( rts->rtsNode )
	{
	if  ( docRtfCloseNode( rr, rts->rtsNode ) )
	    { LDEB(1); rval= -1; goto ready;	}

	rts->rtsNode= rts->rtsNode->biParent;
	}

  ready:

    return rval;
    }

/************************************************************************/

static int docRtfStartNextCell(	int *		pRowNesting,
				int		tableNestingTo,
				RtfReader *	rr )
    {
    int			rowNestingTo;
    RtfTreeStack *	rts= rr->rrTreeStack;

    rowNestingTo= tableNestingTo;
    if  ( rowNestingTo == 0 )
	{ rowNestingTo= 1;	}

    if  ( tableNestingTo > 0				&&
	  rts->rtsNode->biParent->biLevel == DOClevROW	&&
	  ! rts->rtsNode->biParent->biRowForTable	)
	{
	if  ( docRtfPopNode( rr, rts->rtsNode->biParent ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)--;
	return 0;
	}

    if  ( rowNestingTo > *pRowNesting )
	{
	if  ( *pRowNesting < 1 )
	    { LDEB(*pRowNesting); return -1;	}

	if  ( docRtfStartRow( rr, tableNestingTo > 0 ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)++;
	return 0;
	}

    if  ( rowNestingTo < *pRowNesting )
	{ LLDEB(rowNestingTo,*pRowNesting); return -1;	}

    if  ( docRtfStartParagraph( rr ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docRtfStartNode(	int *		pRowNesting,
				RtfReader *	rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;
    int			tableNestingTo;

    tableNestingTo= rrs->rrsParagraphProperties.ppTableNesting;

    switch( rts->rtsNode->biLevel )
	{
	case DOClevBODY:
	    if  ( docRtfStartSect( rr ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docRtfStartRow( rr, tableNestingTo > 0 ) )
		{ LDEB(1); return -1;	}
	    (*pRowNesting)++;
	    break;

	case DOClevROW:
	    if  ( docRtfStartCell( rr ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfStartNextCell( pRowNesting, tableNestingTo, rr ) )
		{ LDEB(tableNestingTo); return -1;	}
	    break;

	default:
	    SDEB(docLevelStr(rts->rtsNode->biLevel));
	    return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the current paragraph. If there is none, make one.		*/
/*									*/
/************************************************************************/

struct BufferItem * docRtfGetParaNode(	RtfReader *	rr )
    {
    int			rowNesting;
    RtfTreeStack *	rts= rr->rrTreeStack;

    if  ( ! rts->rtsNode )
	{ XDEB(rts->rtsNode); return (struct BufferItem *)0;	}

    rowNesting= docRowNesting( rts->rtsNode );

    while( rts->rtsNode->biLevel != DOClevPARA )
	{
	if  ( docRtfStartNode( &rowNesting, rr ) )
	    { LDEB(1); return (struct BufferItem *)0;	}
	}

    return rts->rtsNode;
    }

/************************************************************************/
/*									*/
/*  Return the current section. If there is none, make one.		*/
/*									*/
/************************************************************************/

struct BufferItem * docRtfGetSectNode(	RtfReader *	rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;
    struct BufferItem *	sectNode= docGetSectNode( rts->rtsNode );

    if  ( ! sectNode )
	{
	int	rowNesting= 0;

	while( rts->rtsNode->biLevel != DOClevSECT )
	    {
	    if  ( docRtfStartNode( &rowNesting, rr ) )
		{ LDEB(1); return (struct BufferItem *)0;	}
	    }

	sectNode= rts->rtsNode;
	}

    if  ( ! sectNode || sectNode->biLevel != DOClevSECT )
	{ XDEB(sectNode); return (struct BufferItem *)0;	}

    return sectNode;
    }

/************************************************************************/
/*									*/
/*  Handle \page or \col: Finish the current paragraph and remember	*/
/*  that the subequent one starts at the appropriate position.		*/
/*									*/
/************************************************************************/

static int docRtfSetParaBreak(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    if  ( rcw->rcwID == TPkindPAGEBREAK )
	{ rr->rrParagraphBreakOverride= DOCibkPAGE; return 1; }

    if  ( rcw->rcwID == TPkindCOLUMNBREAK )
	{ rr->rrParagraphBreakOverride= DOCibkCOL; return 1; }

    SDEB(rcw->rcwWord);
    return 0;
    }

int docRtfBreakParticule(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;
    struct BufferItem *	paraNode;

    if  ( rr->rrInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ SXDEB(rcw->rcwWord,paraNode); return -1; }

    switch( rcw->rcwID )
	{
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    {
	    int			done= 0;
	    int			paraStrlen= docParaStrlen( paraNode );

	    int			atHead= paraStrlen == 0 ||
					paraStrlen == rr->rrParaHeadFieldTailOffset;

	    if  ( atHead )
		{
		if  ( rr->rrParagraphBreakOverride == -1		   &&
		      paraNode->biParaProperties->ppBreakKind == DOCibkNONE )
		    { done= docRtfSetParaBreak( rcw, rr );	}
		}
	    else{
		if  ( docRtfPopNode( rr, paraNode ) )
		    { SDEB(rcw->rcwWord); return -1;	}

		paraNode= docRtfGetParaNode( rr );
		if  ( ! paraNode )
		    { SXDEB(rcw->rcwWord,paraNode); return -1; }

		done= docRtfSetParaBreak( rcw, rr );
		}

	    if  ( ! done						&&
		  docParaBuilderAppendSpecialParticule(
				rts->rtsParagraphBuilder,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(done); return -1;	}

	    docRtfResetParagraphReadingState( rr );

	    break;
	    }

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

