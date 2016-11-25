/************************************************************************/
/*									*/
/*  Manage documner node hierarchy from the rtf parser.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docItemShadingAdmin.h>
#   include	<docParaParticules.h>

/************************************************************************/
/*									*/
/*  Apply the row properties that have been collected to a row.		*/
/*									*/
/************************************************************************/

static int docRtfSetRowProperties(	RtfReader *		rrc,
					BufferItem *		rowNode )
    {
    PropertyMask		rpDoneMask;

    rrc->rrcRowProperties.rpShadingNumber= docItemShadingNumber(
				&(rrc->rrcDocument->bdItemShadingList),
				&(rrc->rrcRowShading) );

    rrc->rrcRowProperties.rpFrameNumber= docFramePropertiesNumber(
				&(rrc->rrcDocument->bdFramePropertyList),
				&(rrc->rrcRowFrameProperties) );

    utilPropMaskClear( &rpDoneMask );

    if  ( docChangeRowProperties( &rpDoneMask, rowNode,
					    &(rrc->rrcRowPropertyMask),
					    &(rrc->rrcRowProperties) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the level in the hierarchy for docRtfReadGroup().		*/
/*									*/
/*  1)  Empty paragraphs get one empty particule to have a height.	*/
/*									*/
/************************************************************************/

static int docRtfCheckParagraph(	RtfReader *		rrc,
					BufferItem *		paraNode )
    {
    int			addEmptyParticule= 0;

    if  ( paraNode->biParaParticuleCount == 0 )
	{ addEmptyParticule= 1;	}
    else{
	const TextParticule *	tp;

	tp= paraNode->biParaParticules+ paraNode->biParaParticuleCount- 1;

	if  ( tp->tpKind == DOCkindLINEBREAK	||
	      tp->tpKind == DOCkindPAGEBREAK	||
	      tp->tpKind == DOCkindCOLUMNBREAK	)
	    { addEmptyParticule= 1;	}

	if  ( tp->tpKind == DOCkindFIELDTAIL )
	    {
	    const DocumentFieldList *	dfl= &(rrc->rrcDocument->bdFieldList);
	    const DocumentField *	df;

	    df= docGetFieldByNumber( dfl, tp->tpObjectNumber );
	    if  ( df->dfKind == DOCfkCHFTN )
		{ addEmptyParticule= 1;	}
	    }
	}

    if  ( addEmptyParticule )
	{
	RtfReadingState *	rrs= rrc->rrcState;

	const int		part= paraNode->biParaParticuleCount;
	const int		stroff= docParaStrlen( paraNode );
	const int		count= 0;

	TextAttribute		ta;
	int			textAttrNr;

	docPlainTextAttribute( &ta, rrc->rrcDocument );

	if  ( rrs )
	    {
	    if  ( rrs->rrsTextShadingChanged )
		{ docRtfRefreshTextShading( rrc, rrs );	}

	    ta= rrs->rrsTextAttribute;
	    }

	textAttrNr= docTextAttributeNumber( rrc->rrcDocument, &ta );
	if  ( textAttrNr < 0 )
	    { LDEB(textAttrNr); return -1;	}

	if  ( ! docInsertTextParticule( paraNode, part,
				    stroff, count, DOCkindSPAN, textAttrNr ) )
	    { LDEB(1); return -1;	}

	rrc->rrcAfterNoteref= 0;
	rrc->rrcAtParaHead= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a section in the document.					*/
/*									*/
/************************************************************************/

static int docRtfCloseSect(	RtfReader *		rrc,
				BufferItem *		sectNode )
    {
    if  ( sectNode->biTreeType == DOCinBODY )
	{
	PropertyMask		spUpdMask;

	utilPropMaskClear( &spUpdMask );
	utilPropMaskFill( &spUpdMask, SPprop_COUNT );

	/*  Not clear if this is desirable  */
	if  ( docUpdSectProperties( (PropertyMask *)0,
				&(sectNode->biSectProperties),
				&spUpdMask, &(rrc->rrcSectionProperties) ) )
	    { LDEB(1); return -1;	}
	}

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

static int docRtfCloseRow(	RtfReader *		rrc,
				BufferItem *		rowNode )
    {
    if  ( rowNode->biRowForTable )
	{
	if  ( docRtfSetRowProperties( rrc, rowNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docCleanRowProperties( &(rowNode->biRowProperties) );
	docInitRowProperties( &(rowNode->biRowProperties) );
	}

    if  ( docIsRowNode( rowNode )				&&
	  rowNode->biRowCellCount != rowNode->biChildCount	)
	{
	LLDEB(rowNode->biRowCellCount,rowNode->biChildCount);
	/*docListNode(0,rowNode,0);*/

	/*  1  */
	while( rowNode->biRowCellCount < rowNode->biChildCount )
	    {
	    CellProperties	cp;

	    const DocumentAttributeMap * const dam= (const DocumentAttributeMap *)0;

	    docInitCellProperties( &cp );
	    docCopyCellProperties( &cp,
			&(rowNode->biRowCells[rowNode->biRowCellCount-1]), dam );

	    /* Add zero width dummy columns to avoid crashes */
	    while( rowNode->biRowCellCount < rowNode->biChildCount )
		{
		const int	shiftTail= 0;

		if  ( docInsertRowColumn( &(rowNode->biRowProperties),
				rowNode->biRowCellCount, shiftTail, &cp, dam ) )
		    { LDEB(rowNode->biRowCellCount); return -1;	}
		}

	    docCleanCellProperties( &cp );
	    }

	/*  2  */
	if  ( rowNode->biRowCellCount > rowNode->biChildCount )
	    {
	    const int	shiftTail= 0; /* irrelevant at end */

	    docDeleteColumnsFromRow( &(rowNode->biRowProperties),
			    rowNode->biChildCount,
			    rowNode->biRowCellCount- rowNode->biChildCount,
			    shiftTail );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a cell.							*/
/*									*/
/************************************************************************/

static int docRtfCloseCell(	RtfReader *		rrc,
				BufferItem *		cellNode )
    {
    if  ( cellNode->biChildCount == 0 )
	{ LDEB(cellNode->biChildCount); return 0;	}

    return 0;
    }

/************************************************************************/

static void docRtfSetForRow(	BufferItem *	node )
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
/*  Close a paragraph in the document.					*/
/*									*/
/*  1)  Give up on syntactically incorrect rtf files (more '}' than	*/
/*	'{') in the hope that is we avoid the crash, the user can	*/
/*	decide what to do.						*/
/*									*/
/************************************************************************/

static int docRtfCloseParagraph(	RtfReader *	rrc,
					BufferItem *	paraNode )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    BufferDocument *		bd= rrc->rrcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				paraNr;
    int				textAttributeNumber;
    const int			mindTable= 1;

    ListNumberTreeNode *	root;

    /* 1  */
    if  ( ! rrs )
	{ return 0;	}

    if  ( docRtfCheckParagraph( rrc, paraNode ) )
	{ LDEB(1); return -1;	}

    textAttributeNumber= paraNode->biParaParticules->tpTextAttrNr;

    paraNr= docNumberOfParagraph( paraNode );

    if  ( docRtfPopParaFromFieldStack( rrc, paraNr ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSetParaProperties( &(paraNode->biParaProperties),
							bd, mindTable, rrs ) )
	{ LDEB(1); return -1;	}

    rrs->rrsParagraphBreakOverride= -1;

    if  ( paraNode->biParaListOverride > 0 )
	{
	DocumentField *			dfHead= (DocumentField *)0;
	DocumentSelection		dsInsideHead;
	DocumentSelection		dsAroundHead;
	int				partBegin= -1;
	int				partEnd= -1;

	const ListOverrideTable *	lot;

	lot= &(dp->dpListAdmin.laListOverrideTable);

	if  ( paraNode->biParaListOverride >= lot->lotOverrideCount )
	    {
	    LLDEB(paraNode->biParaListOverride,lot->lotOverrideCount);
	    return -1;
	    }

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, paraNode, bd ) )
	    {
	    if  ( docInsertParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
			&partBegin, &partEnd, paraNode, bd, rrc->rrcTree,
			DOCfkLISTTEXT, textAttributeNumber ) )
		{
		LDEB(paraNode->biParaListOverride); return -1;
		}
	    }

	root= docGetListNumberTree( &(rrc->rrcTree->eiListNumberTrees),
						paraNode->biParaListOverride );
	if  ( ! root )
	    { LXDEB(paraNode->biParaListOverride,root); return -1;	}

	if  ( docListNumberTreeInsertParagraph( root,
				    paraNode->biParaListLevel, paraNr ) )
	    { LDEB(paraNr);	}
	}

    if  ( paraNode->biParaOutlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeInsertParagraph(
				    &(rrc->rrcTree->dtOutlineTree),
				    paraNode->biParaOutlineLevel, paraNr ) )
	    { LDEB(paraNr);	}
	}

    if  ( paraNode->biParaTableNesting > 0 )
	{ docRtfSetForRow( paraNode );	}
    else{
	BufferItem *	rowNode= docGetRowLevelNode( paraNode );

	if  ( rowNode->biRowForTable )
	    {
	    if  ( rowNode->biChildCount == 1 )
		{
		rowNode->biRowForTable= 0;
		}
	    else{
		LDEB(rowNode->biChildCount);
		LLDEB(paraNode->biParaTableNesting,rowNode->biRowForTable);
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a node.							*/
/*									*/
/************************************************************************/

static int docRtfCloseNode(	RtfReader *		rrc,
				BufferItem *		node )
    {
    switch( node->biLevel )
	{
	case DOClevSECT:
	    if  ( docRtfCloseSect( rrc, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( docRtfCloseRow( rrc, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfCloseCell( rrc, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docRtfCloseParagraph( rrc, node ) )
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

static int docRtfPopNode(		RtfReader *	rrc,
					BufferItem *	node )
    {
    while( rrc->rrcNode )
	{
	if  ( docRtfCloseNode( rrc, rrc->rrcNode ) )
	    { LDEB(1); return -1;	}

	if  ( rrc->rrcNode == node )
	    {
	    rrc->rrcNode= rrc->rrcNode->biParent;
	    rrc->rrcLevel= rrc->rrcNode->biLevel;
	    return 0;
	    }

	rrc->rrcNode= rrc->rrcNode->biParent;
	}

    XDEB(rrc->rrcNode); return -1;
    }

/************************************************************************/
/*									*/
/*  Handle cell tags that relate to the document hierarchy.		*/
/*									*/
/************************************************************************/

static int docRtfHierarchyCell(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    BufferItem *	levelNode= (BufferItem *)0;
    int			paraInCell= 1;

    /*  1  */
    if  ( rrc->rrcNode )
	{ levelNode= docGetCellNode( rrc->rrcNode );	}

    if ( rcw->rcwID == DOClevCELL		&&
	 rrc->rrcNode				&&
	 rrc->rrcNode->biLevel == DOClevCELL	)
	{
	int	cc= rrc->rrcNode->biChildCount;

	if  ( cc > 0						  &&
	      rrc->rrcNode->biChildren[cc-1]->biLevel != DOClevPARA )
	      { paraInCell= 0;	}
	}

    if  ( ! levelNode						||
	  ( rrc->rrcNode->biLevel <= DOClevCELL && paraInCell )	)
	{
	BufferItem *	paraNode= docRtfGetParaNode( rrc );

	if  ( ! paraNode )
	    { SDEB(rcw->rcwWord); return -1; }

	/* cope with: {\intbl etc\par}\cell */
	if  ( paraNode->biParaTableNesting == 0 )
	    {
	    docSetParaTableNesting( paraNode );
	    docRtfSetForRow( paraNode );
	    if  ( rrc->rrcState )
		{
		rrc->rrcState->rrsParagraphProperties.ppTableNesting= 
					    paraNode->biParaTableNesting;
		}
	    }
	}
    if  ( ! rrc->rrcNode )
	{ SXDEB(rcw->rcwWord,rrc->rrcNode); return -1;	}

    /* MS-Word does this: The first clue about a table is \cell */
    if  ( rrc->rrcNode->biLevel == DOClevPARA	&&
	  rrc->rrcNode->biParaTableNesting == 0	)
	{
	/*LDEB(rrc->rrcNode->biParaTableNesting);*/
	rrc->rrcNode->biParaTableNesting= 1;
	docRtfSetForRow( rrc->rrcNode );
	if  ( rrc->rrcState )
	    {
	    rrc->rrcState->rrsParagraphProperties.ppTableNesting= 
					rrc->rrcNode->biParaTableNesting;
	    }
	}

    levelNode= docGetCellNode( rrc->rrcNode );
    if  ( ! levelNode )
	{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
    if  ( docRtfPopNode( rrc, levelNode ) )
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
			RtfReader *		rrc )
    {
    BufferItem *	levelNode= (BufferItem *)0;

    switch( rcw->rcwID )
	{
	case DOClevSECT:
	    /*  1  */
	    if  ( rrc->rrcNode )
		{ levelNode= docGetSectNode( rrc->rrcNode );	}
	    if  ( ! levelNode )
		{
		if  ( ! docRtfGetParaNode( rrc ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}
	    if  ( ! rrc->rrcNode )
		{ SXDEB(rcw->rcwWord,rrc->rrcNode); return -1;	}

	    levelNode= docGetSectNode( rrc->rrcNode );
	    if  ( ! levelNode )
		{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
	    if  ( docRtfPopNode( rrc, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}

	    break;

	case DOClevROW:
	case DOClevNESTROW:
	    /*  1  */
	    if  ( rrc->rrcNode )
		{ levelNode= docGetRowLevelNode( rrc->rrcNode );	}
	    /* ignore stray \row tags rather than fail */
	    if  ( ! levelNode )
		{ LSDEB(rrc->rrcCurrentLine,rcw->rcwWord); return 0;	}
	    if  ( docRtfPopNode( rrc, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevCELL:
	case DOClevNESTCELL:
	    if  ( docRtfHierarchyCell( rcw, rrc ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevPARA:
	    levelNode= docRtfGetParaNode( rrc );
	    if  ( ! levelNode )
		{ SDEB(rcw->rcwWord); return -1; }

	    if  ( docRtfPopNode( rrc, levelNode ) )
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

static int docRtfStartParagraph(	RtfReader *	rrc )
    {
    BufferItem *		paraNode;
    RtfReadingState *		rrs= rrc->rrcState;
    const int			mindTable= 1;

    paraNode= docInsertNode( rrc->rrcDocument, rrc->rrcNode, -1, DOClevPARA );
    if  ( ! paraNode )
	{ SXDEB(docLevelStr(rrc->rrcLevel),paraNode); return -1; }
    rrc->rrcNode= paraNode;

    if  ( docRtfSetParaProperties( &(paraNode->biParaProperties),
				    rrc->rrcDocument, mindTable, rrs ) )
	{ LDEB(1); return -1;	}

    /*  Silly old Jade  */
    if  ( paraNode->biParaLeftIndentTwips < 0	 )
	{ paraNode->biParaLeftIndentTwips= 0;	}
    if  ( paraNode->biParaFirstIndentTwips+ paraNode->biParaLeftIndentTwips < 0 )
	{ paraNode->biParaFirstIndentTwips= -paraNode->biParaLeftIndentTwips; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start/Close a section.						*/
/*									*/
/************************************************************************/

static int docRtfStartSect(	RtfReader *	rrc )
    {
    BufferItem *		sectNode;

    sectNode= docInsertNode( rrc->rrcDocument, rrc->rrcNode, -1, DOClevSECT );
    if  ( ! sectNode )
	{ SXDEB(docLevelStr(rrc->rrcLevel),sectNode); return -1; }
    rrc->rrcNode= sectNode;

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	rrc->rrcSelectionScope= sectNode->biSectSelectionScope;

	if  ( docCopySectionProperties( &(sectNode->biSectProperties),
					 &(rrc->rrcSectionProperties) ) )
	    { LDEB(1); return -1;	}

	if  ( sectNode->biNumberInParent > 0 )
	    {
	    BufferItem *	prevSectNode;

	    prevSectNode= sectNode->biParent->biChildren[
					sectNode->biNumberInParent- 1];

	    if  ( docCopySectHeadersFooters( sectNode, rrc->rrcDocument,
					    prevSectNode, rrc->rrcDocument ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a row.							*/
/*									*/
/************************************************************************/

static int docRtfStartRow(	RtfReader *	rrc,
				int		forTable )
    {
    BufferItem *		rowNode;

    rowNode= docInsertNode( rrc->rrcDocument, rrc->rrcNode, -1, DOClevROW );
    if  ( ! rowNode )
	{ SXDEB(docLevelStr(rrc->rrcLevel),rowNode); return -1; }
    rowNode->biRowForTable= forTable;
    rrc->rrcNode= rowNode;

    if  ( docRtfSetRowProperties( rrc, rowNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRtfStartCell(	RtfReader *	rrc )
    {
    BufferItem *		cellNode;

    cellNode= docInsertNode( rrc->rrcDocument, rrc->rrcNode, -1, DOClevCELL );
    if  ( ! cellNode )
	{ SXDEB(docLevelStr(rrc->rrcLevel),cellNode); return -1; }
    rrc->rrcNode= cellNode;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish the node that is currently under construction.		*/
/*									*/
/************************************************************************/

int docRtfFinishCurrentNode(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    while(  rrc->rrcNode )
	{
	if  ( docRtfCloseNode( rrc, rrc->rrcNode ) )
	    { LDEB(1); return -1;	}

	rrc->rrcNode= rrc->rrcNode->biParent;
	}

    return 0;
    }

/************************************************************************/

static int docRtfStartNextCell(	int *		pRowNesting,
				int		tableNestingTo,
				RtfReader *	rrc )
    {
    int			rowNestingTo;

    rowNestingTo= tableNestingTo;
    if  ( rowNestingTo == 0 )
	{ rowNestingTo= 1;	}

    if  ( tableNestingTo > 0				&&
	  rrc->rrcNode->biParent->biLevel == DOClevROW	&&
	  ! rrc->rrcNode->biParent->biRowForTable	)
	{
	if  ( docRtfPopNode( rrc, rrc->rrcNode->biParent ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)--;
	return 0;
	}

    if  ( rowNestingTo > *pRowNesting )
	{
	if  ( *pRowNesting < 1 )
	    { LDEB(*pRowNesting); return -1;	}

	if  ( docRtfStartRow( rrc, tableNestingTo > 0 ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)++;
	return 0;
	}

    if  ( rowNestingTo < *pRowNesting )
	{ LLDEB(rowNestingTo,*pRowNesting); return -1;	}

    if  ( docRtfStartParagraph( rrc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docRtfStartNode(	int *		pRowNesting,
				RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    int			tableNestingTo;

    tableNestingTo= rrs->rrsParagraphProperties.ppTableNesting;

    switch( rrc->rrcNode->biLevel )
	{
	case DOClevBODY:
	    if  ( docRtfStartSect( rrc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docRtfStartRow( rrc, tableNestingTo > 0 ) )
		{ LDEB(1); return -1;	}
	    (*pRowNesting)++;
	    break;

	case DOClevROW:
	    if  ( docRtfStartCell( rrc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfStartNextCell( pRowNesting, tableNestingTo, rrc ) )
		{ LDEB(tableNestingTo); return -1;	}
	    break;

	default:
	    SDEB(docLevelStr(rrc->rrcNode->biLevel));
	    return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the current paragraph. If there is none, make one.		*/
/*									*/
/************************************************************************/

BufferItem * docRtfGetParaNode(	RtfReader *	rrc )
    {
    int			rowNesting;

    if  ( ! rrc->rrcNode )
	{ XDEB(rrc->rrcNode); return (BufferItem *)0;	}

    rowNesting= docRowNesting( rrc->rrcNode );

    while( rrc->rrcNode->biLevel != DOClevPARA )
	{
	if  ( docRtfStartNode( &rowNesting, rrc ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    return rrc->rrcNode;
    }

/************************************************************************/
/*									*/
/*  Return the current section. If there is none, make one.		*/
/*									*/
/************************************************************************/

BufferItem * docRtfGetSectNode(	RtfReader *	rrc )
    {
    BufferItem *	sectNode= docGetSectNode( rrc->rrcNode );

    if  ( ! sectNode )
	{
	int	rowNesting= 0;

	while( rrc->rrcNode->biLevel != DOClevSECT )
	    {
	    if  ( docRtfStartNode( &rowNesting, rrc ) )
		{ LDEB(1); return (BufferItem *)0;	}
	    }

	sectNode= rrc->rrcNode;
	}

    if  ( ! sectNode || sectNode->biLevel != DOClevSECT )
	{ XDEB(sectNode); return (BufferItem *)0;	}

    return sectNode;
    }

