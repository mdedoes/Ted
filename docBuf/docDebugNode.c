#   include	"docBufConfig.h"

#   include	<stdio.h>

#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	"docNodeTree.h"
#   include	<docRowProperties.h>
#   include	<docParaProperties.h>
#   include	"docSelect.h"
#   include	"docParaParticules.h"
#   include	<bidiTree.h>
#   include	<uniUtf8.h>
#   include	<bidiScanner.h>
#   include	<docTreeType.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

#   define	LIST_PARA_STRING	1
#   define	LIST_PARTICULES		1
#   define	LIST_FIRST_LINE		0
#   define	LIST_LINES		1

#   define	IS			2

static int docCheckGroupLeft(	const struct BufferItem *	node )
    {
    int		rval= 0;

    int		prev= 0;
    int		here;

    if  ( node->biParent && node->biNumberInParent > 0 )
	{
	prev= node->biParent->biChildren[node->biNumberInParent-1]->
						biLeftParagraphs;
	}

    here= node->biLeftParagraphs;

    if  ( here- prev !=
	    node->biChildren[node->biChildCount- 1]->biLeftParagraphs )
	{
	SLDEB(docLevelStr(node->biLevel),here- prev);
	LDEB(node->biChildren[node->biChildCount- 1]->biLeftParagraphs);
	rval= -1;
	}

    return rval;
    }

static int docCheckChild(	const BufferItem *	parent,
				const BufferItem *	child,
				int			i,
				const LayoutPosition *	lpTop,
				LayoutPosition *	lpBelow,
				int			checkGeometry )
    {
    int				rval= 0;
    const LayoutPosition *	belowChild;

    if  ( docIsRowNode( child ) )
	{ belowChild= &(child->biRowBelowAllCellsPosition);	}
    else{ belowChild= &(child->biBelowPosition);		}

    if  ( checkGeometry )
	{
	const LayoutPosition *	belowParent;

	if  ( docIsRowNode( parent ) )
	    { belowParent= &(parent->biRowBelowAllCellsPosition);	}
	else{ belowParent= &(parent->biBelowPosition);			}

	if  ( lpTop )
	    {
	    int d= docCompareLayoutPositions( &(child->biTopPosition), lpTop );
	    const char * direction= (char *)0;
	    if  ( d > 0 )
		{ direction= "BELOW";	}
	    if  ( d < 0 )
		{ direction= "ABOVE";	}

	    if  ( direction )
		{
		appDebug( "############## %s %d in %s: %s Expected\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel),
					direction );

		LLDEB(child->biTopPosition.lpPage,lpTop->lpPage);
		LLDEB(child->biTopPosition.lpColumn,lpTop->lpColumn);
		LLDEB(child->biTopPosition.lpPageYTwips,lpTop->lpPageYTwips);
		rval= -1;
		}
	    }

	if  ( docCompareLayoutPositions( &(child->biTopPosition),
					    &(parent->biTopPosition) ) < 0 )
	    {
	    appDebug( "############## %s %d in %s:\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel) );
	    rval= -1;
	    }

	if  ( docCompareLayoutPositions( belowChild, belowParent ) > 0 )
	    {
	    appDebug( "############## %s %d in %s:\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel) );
	    rval= -1;
	    }
	}

    if  ( child->biParent != parent )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	XXDEB(child->biParent,parent);
	rval= -1;
	}

    if  ( child->biNumberInParent != i )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biNumberInParent,i);
	rval= -1;
	}

    if  ( ! docValidChildLevel( parent->biLevel, child->biLevel ) )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biLevel,parent->biLevel);
	rval= -1;
	}

    if  ( child->biTreeType != parent->biTreeType )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biTreeType,parent->biTreeType);
	rval= -1;
	}

    if  ( child->biLevel == DOClevPARA )
	{
	int	tableNestingTree= docTableNesting( child );
	int	tableNestingProp= child->biParaProperties->ppTableNesting;

	if  ( tableNestingTree != tableNestingProp )
	    {
	    SLLDEB("####",tableNestingProp,tableNestingTree);
	    if  ( tableNestingTree > 0 && tableNestingProp == 0 )
		{
		const BufferItem *	rowNode;

		rowNode= docGetRowNode( (BufferItem *)child );
		if  ( ! rowNode )
		    { XDEB(rowNode);	}
		else{
		    LLDEB(rowNode->biRowPropertyNumber,
					rowNode->biRowProperties->rpCellCount);
		    }
		}
	    rval= -1;
	    }
	}
    else{
	if  ( child->biChildCount < 1 )
	    { SLDEB("#######",child->biChildCount);	}
	}

    if  ( lpBelow )
	{ *lpBelow= *belowChild;	}

    return rval;
    }

static int docCheckParaNode(	const struct BufferItem *	paraNode )
    {
    int				rval= 0;

    int				part;
    const TextParticule *	tp;
    int				stroff;
    int				tableNestingTree= docTableNesting( paraNode );
    int				tableNestingProp;

    tableNestingProp= paraNode->biParaProperties->ppTableNesting;

    if  ( paraNode->biLeftParagraphs != paraNode->biNumberInParent+ 1 )
	{
	appDebug( "############## %s:\n",
				    docLevelStr(paraNode->biLevel) );
	LLDEB(paraNode->biLeftParagraphs,paraNode->biNumberInParent+1);
	}

    if  ( tableNestingTree != tableNestingProp )
	{
	SLLDEB("#######",tableNestingTree,tableNestingProp);
	rval= -1;
	}

    stroff= 0;
    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpStroff > docParaStrlen( paraNode )	||
	      tp->tpStroff != stroff			)
	    {
	    appDebug( "############## %s:\n",
				    docLevelStr(paraNode->biLevel) );
	    LLLLDEB(part,tp->tpStroff,stroff,docParaStrlen(paraNode));
	    rval= -1;
	    }

	stroff= tp->tpStroff+ tp->tpStrlen;
	if  ( stroff < 0			||
	      stroff >  docParaStrlen( paraNode )	)
	    {
	    appDebug( "############## %s:\n",
				    docLevelStr(paraNode->biLevel) );
	    LLLLDEB(part,stroff,tp->tpStrlen,docParaStrlen(paraNode));
	    rval= -1;
	    }
	}

    if  ( stroff != docParaStrlen( paraNode ) )
	{
	appDebug( "############## %s:\n",
				    docLevelStr(paraNode->biLevel) );
	LLDEB(stroff,docParaStrlen(paraNode));
	rval= -1;
	}

    return rval;
    }

int docCheckNode(	const struct BufferItem *	node,
			const struct BufferDocument *	bd,
			int				checkGeometry )
    {
    int			i;
    int			rval= 0;

    LayoutPosition	lpHere;

    if  ( node->biLevel == DOClevPARA )
	{
	if  ( docCheckParaNode( node ) )
	    { LDEB(1); rval= -1;	}
	}
    else{
	if  ( node->biChildCount < 1 )
	    { SLDEB("#######",node->biChildCount);	}
	}

    if  ( node->biLevel == DOClevROW && docIsRowNode( node ) )
	{
	const RowProperties *	rp= node->biRowProperties;

	if  ( rp->rpCellCount != node->biChildCount )
	    {
	    appDebug( "############## %s:\n", docLevelStr(node->biLevel) );
	    LLDEB(rp->rpCellCount,node->biChildCount);
	    }
	}

    if  ( node->biChildCount > 0 )
	{
	if  ( docCheckGroupLeft( node ) )
	    { rval= -1;	}

	if  ( ! node->biParent )
	    {
	    DocumentPosition	dp;
	    docInitDocumentPosition( &dp );

	    if  ( ! docHeadPosition( &dp, (struct BufferItem *)node ) )
		{
		const struct BufferItem *	paraNode= dp.dpNode;
		int				n= 1;

		for (;;)
		    {
		    if  ( docNumberOfParagraph( paraNode ) != n )
			{ LLDEB(docNumberOfParagraph(paraNode),n); rval= -1; }

		    paraNode= docNextParagraph( (struct BufferItem *)paraNode );
		    if  ( ! paraNode )
			{ break;	}
		    n++;
		    }
		}
	    }
	}

    for ( i= 0; i < node->biChildCount; i++ )
	{
	LayoutPosition	lpBelowChild= lpHere;

	if  ( docCheckChild( node, node->biChildren[i], i,
				    &lpHere, &lpBelowChild, checkGeometry ) )
	    { rval= -1;	}

	if  ( ! docIsRowNode( node ) )
	    { lpHere= lpBelowChild;	}

	if  ( docCheckNode( node->biChildren[i], bd, checkGeometry ) )
	    { rval= -1;	}
	}

    return rval;
    }

int docCheckRootNode(	const struct BufferItem *		node,
			const struct BufferDocument *	bd,
			int				checkGeometry )
    {
    while( node->biParent )
	{ node= node->biParent;	}

    return docCheckNode( node, bd, checkGeometry );
    }

static void docListChildren(	const struct BufferDocument *	bd,
				int				indent,
				const struct BufferItem *	node,
				int				checkGeometry )
    {
    int			i;

    LayoutPosition	lpHere;

    lpHere= node->biTopPosition;

    for ( i= 0; i < node->biChildCount; i++ )
	{
	LayoutPosition	lpBelowChild= lpHere;

	docCheckChild( node, node->biChildren[i], i,
				    &lpHere, &lpBelowChild, checkGeometry );

	if  ( ! docIsRowNode( node ) )
	    { lpHere= lpBelowChild;	}

	docListNode( bd, indent+ 1, node->biChildren[i], checkGeometry );
	}

    if  ( node->biChildCount > 0 )
	{ docCheckGroupLeft( node );	}

    if  ( checkGeometry							&&
	  ! docIsRowNode( node )					&&
	  ! DOC_SAME_POSITION( &(node->biBelowPosition), &lpHere )	)
	{
	appDebug( "############## %s :\n", docLevelStr(node->biLevel) );

	LLDEB(node->biBelowPosition.lpPage,lpHere.lpPage);
	LLDEB(node->biBelowPosition.lpColumn,lpHere.lpColumn);
	LLDEB(node->biBelowPosition.lpPageYTwips,lpHere.lpPageYTwips);
	}
    }

/************************************************************************/
/*									*/
/*  List the text of a paragraph.					*/
/*									*/
/************************************************************************/

# if LIST_PARA_STRING

static void docListParaString(	int				indent,
				const struct BufferItem *	paraNode,
				int				stroffFrom,
				int				stroffUpto )
    {
    int		stroff= stroffFrom;

    while( stroff < stroffUpto )
	{
	int		todo= 0;

	const char *	str= docParaString( paraNode, stroff );

	while( stroff+ todo < stroffUpto )
	    {
	    unsigned short	symbol;
	    int			step= uniGetUtf8( &symbol, str+ todo );

	    if  ( step < 1 )
		{
		LDEB(step);
		todo= stroffUpto- stroff;

		if  ( todo > 60 )
		    { todo= 60; }

		break;
		}

	    todo += step;
	    if  ( todo >= 60 )
		{ break;	}
	    }

	appDebug( "%*s\"%.*s\"\n",
			IS* indent+ IS, "",
			todo, docParaString( paraNode, stroff ) );

	stroff += todo;
	}
    }

# endif

/************************************************************************/
/*									*/
/*  List the lines in a paragraph.					*/
/*									*/
/************************************************************************/

static void docListParaLines(
			int				indent,
			const struct BufferItem *	paraNode,
			int				checkGeometry )
    {
    int				line;
    const TextLine *		tl;
    LayoutPosition		lpHere;

    tl= paraNode->biParaLines;
    lpHere= paraNode->biTopPosition;
    for ( line= 0; line < paraNode->biParaLineCount; tl++, line++ )
	{
	int	listLine= LIST_LINES || ( line == 0 && LIST_FIRST_LINE );

	if  ( checkGeometry						&&
	      docCompareLayoutPositions( &(tl->tlTopPosition),
							&lpHere ) < 0	)
	    {
	    appDebug( "###### Line= %d\n", line );
	    LDEB(line);
	    listLine= 1;
	    }

	lpHere= tl->tlTopPosition;
	lpHere.lpPageYTwips += tl->tlLineStride;

	if  ( checkGeometry						&&
	      docCompareLayoutPositions( &(paraNode->biBelowPosition),
							&lpHere ) < 0	)
	    {
	    appDebug( "###### Line= %d\n", line );
	    LDEB(line);
	    listLine= 1;
	    }

	if  ( listLine )
	    { docListTextLine( IS* indent+ IS, "LINE", line, paraNode, tl ); }
	}
    }

/************************************************************************/
/*									*/
/*  List a range of particules.						*/
/*									*/
/************************************************************************/

static void docListParaParticules(
				int				indent,
				const struct BufferItem *	paraNode,
				int				partFrom,
				int				partUpto )
    {
    int				part;
    const TextParticule *	tp;
    int				stroff;

    if  ( partFrom == 0 )
	{
	tp= paraNode->biParaParticules;
	stroff= 0;
	}
    else{
	tp= paraNode->biParaParticules+ partFrom;
	stroff= tp->tpStroff;
	}

    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
	int		list= LIST_PARTICULES;
	const char *	label= "PART";

	if  ( tp->tpStroff != stroff	)
	    { list= 1; label= "####";	}

	if  ( list )
	    { docListParticule( IS* indent+ IS, label, part, paraNode, tp ); }

	stroff= tp->tpStroff+ tp->tpStrlen;
	}

    if  ( partUpto == paraNode->biParaParticuleCount	&&
	  stroff != docParaStrlen( paraNode )		)
	{
	SLLDEB("####",stroff,docParaStrlen( paraNode ) );
	if  ( paraNode->biParaParticuleCount > 0 )
	    {
	    part= paraNode->biParaParticuleCount- 1;
	    tp= paraNode->biParaParticules+ part;
	    docListParticule( IS* indent+ IS, "####", part, paraNode, tp );
	    }
	}
    }

typedef struct ParaScan
    {
    const BufferItem *	psParaNode;
    int			psIndent;
    int			psBytesScanned;
    int			psParticulesScanned;
    } ParaScan;

static int docGotParaBidiLevel(		void *		vps,
					int		initiator,
					int		level,
					int		stroffFrom,
					int		stroffUpto )
    {
    int				rval= 0;

    ParaScan *			ps= (ParaScan *)vps;
    int				partFrom;
    int				partUpto;

    int				headFlags= 0;
    int				tailFlags= 0;

    const TextParticule *	tpHead;
    const TextParticule *	tpTail;

    DocumentPosition		dp;

    dp.dpNode= (struct BufferItem *)ps->psParaNode;
    dp.dpStroff= stroffFrom;
    if  ( docFindParticuleOfPosition( &partFrom, &headFlags,
						&dp, PARAfindLAST ) )
	{ SLDEB("####",stroffFrom); rval= -1;	}

    dp.dpNode= (struct BufferItem *)ps->psParaNode;
    dp.dpStroff= stroffUpto;
    if  ( docFindParticuleOfPosition( &partUpto, &tailFlags,
						&dp, PARAfindPAST ) )
	{ SLDEB("####",stroffUpto); rval= -1;	}

    if  ( rval )
	{
	appDebug( "%*sLEVEL= %d STROFF %3d..%3d ####\n",
					    IS* ps->psIndent+ IS, "", level,
					    stroffFrom, stroffUpto );

#	if LIST_PARA_STRING
	docListParaString( ps->psIndent+ 1, ps->psParaNode,
					    stroffFrom, stroffUpto );
#	endif

	return 0;
	}
    else{
	appDebug( "%*sLEVEL= %d STROFF %3d..%3d PART %3d..%3d\n",
					    IS* ps->psIndent+ IS, "", level,
					    stroffFrom, stroffUpto,
					    partFrom, partUpto );

#	if LIST_PARA_STRING
	docListParaString( ps->psIndent+ 1, ps->psParaNode,
					    stroffFrom, stroffUpto );
#	endif
	}

    tpHead= ps->psParaNode->biParaParticules+ partFrom;
    if  ( ! ( headFlags & POSflagPART_HEAD )	||
	  tpHead->tpStroff != stroffFrom	)
	{ SLXLLDEB("####",partFrom,headFlags,tpHead->tpStroff,stroffFrom); }

    if  ( partUpto == ps->psParaNode->biParaParticuleCount )
	{
	tpTail= ps->psParaNode->biParaParticules+ partUpto- 1;

	if  ( ! ( tailFlags & POSflagPARA_TAIL )			||
	      tpTail->tpStroff+ tpTail->tpStrlen != stroffUpto		)
	    {
	    SLXLLDEB("####",partUpto,tailFlags,
				tpTail->tpStroff+tpTail->tpStrlen,stroffUpto);
	    }
	}
    else{
	tpTail= ps->psParaNode->biParaParticules+ partUpto;

	if  ( ! ( tailFlags & POSflagPART_HEAD )	||
	      tpTail->tpStroff != stroffUpto		)
	    { SLXLLDEB("####",partUpto,tailFlags,tpTail->tpStroff,stroffUpto); }
	}

    docListParaParticules( ps->psIndent+ 1,
				    ps->psParaNode, partFrom, partUpto );

    ps->psBytesScanned += stroffUpto- stroffFrom;
    ps->psParticulesScanned += partUpto- partFrom;

    return 0;
    }

static void docListParaNode(	const struct BufferDocument *	bd,
				int				indent,
				const struct BufferItem *	paraNode,
				int				checkGeometry )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    int				listFlat= 0;
    struct BufferItem *		rowNode;
    int				tablenestingTree= 0;
    int				tableNestingProp= paraNode->biParaProperties->ppTableNesting;

    appDebug( "%*s{ NR= %d TN=%d brk=%s %d bytes %d particules, %d lines\n",
			    IS* indent+ IS, "",
			    docNumberOfParagraph( paraNode ),
			    pp->ppTableNesting,
			    docBreakKindStr( pp->ppBreakKind ),
			    docParaStrlen( paraNode ),
			    paraNode->biParaParticuleCount,
			    paraNode->biParaLineCount );

    rowNode= docGetRowNode( (BufferItem *)paraNode );
    if  ( rowNode )
	{
	DocumentPosition	dp;

	if  ( docHeadPosition( &dp, rowNode ) )
	    { SXDEB("####",rowNode);	}
	else{
	    if  ( dp.dpNode == paraNode )
		{
		if  ( rowNode->biRowProperties->rp_Keepfollow !=
				paraNode->biParaProperties->ppKeepWithNext )
		    {
		    SLLDEB("####",rowNode->biRowProperties->rp_Keepfollow,
				paraNode->biParaProperties->ppKeepWithNext);
		    }
		}
	    }

	tablenestingTree++;
	rowNode= docGetRowNode( rowNode->biParent );
	while( rowNode )
	    {
	    tablenestingTree++;
	    rowNode= docGetRowNode( rowNode->biParent );
	    }
	}

    if  ( tableNestingProp != tablenestingTree )
	{ SLLDEB("####",tableNestingProp,tablenestingTree); }

#   if LIST_PARA_STRING
    docListParaString( indent, paraNode, 0, docParaStrlen( paraNode ) );
#   endif

    if  ( 0 && paraNode->biParaBidiRoot && paraNode->biParaParticuleCount > 1 )
	{
	BidiScanner	bs;
	ParaScan	ps;
	int		paraStrlen= docParaStrlen( paraNode );

	ps.psParaNode= paraNode;
	ps.psIndent= indent;
	ps.psBytesScanned= 0;
	ps.psParticulesScanned= 0;

	bidiInitScanner( &bs );
	bs.bsThrough= (void *)&ps;
	bs.bsGotRun= docGotParaBidiLevel;

	if  ( bidiTraverseNodeDisplayOrder( paraNode->biParaBidiRoot, &bs,
							0, paraStrlen ) )
	    { listFlat= 1;	}

	if  ( ps.psBytesScanned != paraStrlen )
	    {
	    SLLDEB("####",ps.psBytesScanned,paraStrlen);
	    listFlat= 1;
	    }
	if  ( paraStrlen > 0						&&
	      ps.psParticulesScanned != paraNode->biParaParticuleCount 	)
	    {
	    SLLDEB("####",ps.psParticulesScanned,
					paraNode->biParaParticuleCount);
	    listFlat= 1;
	    }

	if  ( listFlat )
	    { bidiListNode(paraNode->biParaBidiRoot,(const MemoryBuffer *)0); }
	}
    else{ listFlat= 1;	}

    if  ( listFlat )
	{
	const int	partFrom= 0;
	const int	partUpto= paraNode->biParaParticuleCount;

	docListParaParticules( indent, paraNode, partFrom, partUpto );
	}

    docListParaLines( indent, paraNode, checkGeometry );

    if  ( bd )
	{
	int			part;
	const TextParticule *	tp= paraNode->biParaParticules;
	int			hasObject= 0;

	for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	    {
	    if  ( tp->tpKind == TPkindOBJECT )
		{ hasObject= 1;	}
	    }

	if  ( hasObject )
	    {
	    appDebug( "%*sobjects\n", IS* indent+ IS, "" );
	    docListParaObjects( indent, paraNode, bd );
	    }
	}

    return;
    }

static void docListRowNodeSpecific(
				int				indent,
				const struct BufferItem *	rowNode )
    {
    const RowProperties *	rp= rowNode->biRowProperties;

    if  ( ! rowNode->biParent )
	{ XDEB(rowNode->biParent); return;	}

    if  ( rowNode->biRowTableFirst >= 0			||
	  rowNode->biRowTablePast >= 0			||
	  rowNode->biRowPastHeaderRow >= 0		)
	{
	appDebug(
	    "%*s  Table Head: %d .. %d Body: %d .. %d %s %s\n",
		IS* indent+ IS, "",
		rowNode->biRowTableFirst,
		rowNode->biRowPastHeaderRow,
		rowNode->biRowPastHeaderRow,
		rowNode->biRowTablePast,
		(rowNode)->biRowProperties->rpKeepOnOnePage? "TRKEEP":"",
		(rowNode)->biRowProperties->rp_Keepfollow? "TRKEEPFOLLOW":"" );

	if  ( rowNode->biRowTableFirst		>=
	      rowNode->biParent->biChildCount	)
	    {
	    SLLDEB("####",
		    rowNode->biRowTableFirst,
		    rowNode->biParent->biChildCount);
	    }
	if  ( rowNode->biRowTablePast > rowNode->biParent->biChildCount )
	    {
	    SLLDEB("####",
		    rowNode->biRowTablePast,
		    rowNode->biParent->biChildCount);
	    }
	if  ( rowNode->biRowPastHeaderRow > rowNode->biRowTablePast )
	    {
	    SLLDEB("####",
		    rowNode->biRowPastHeaderRow,
		    rowNode->biRowTablePast);
	    }
	}

    if  ( rowNode->biChildCount != rp->rpCellCount )
	{ SLLDEB("####", rowNode->biChildCount,rp->rpCellCount); }
    }

void docListNode(	const struct BufferDocument *	bd,
			int				indent,
			const struct BufferItem *	node,
			int				checkGeometry )
    {
    int				i;

    for ( i= 0; i < indent; i++ )
	{ appDebug( "%-*s", IS, "." );	}

    appDebug( "NODE %4d %s(%s) LEFT= %d P%d/C%d/Y%d..P%d/C%d/Y%d\n",
			    node->biNumberInParent,
			    docLevelStr( node->biLevel ),
			    docTreeTypeStr( node->biTreeType ),
			    node->biLeftParagraphs,
			    node->biTopPosition.lpPage,
			    node->biTopPosition.lpColumn,
			    node->biTopPosition.lpPageYTwips,
			    node->biBelowPosition.lpPage,
			    node->biBelowPosition.lpColumn,
			    node->biBelowPosition.lpPageYTwips );

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	    appDebug( "%*s{ %4d children\n", IS* indent+ IS, "",
						node->biChildCount );

	    docListChildren( bd, indent, node, checkGeometry );

	    break;

	case DOClevROW:
	    {
	    const RowProperties *	rp= node->biRowProperties;

	    appDebug( "%*s{ %s %4d children %d cells\n", IS* indent+ IS, "",
				docIsRowNode(node)?"T:":"--",
				node->biChildCount,
				docIsRowNode(node)?rp->rpCellCount:0 );

	    if  ( docIsRowNode( node ) )
		{ docListRowNodeSpecific( indent, node );	}

	    docListChildren( bd, indent, node, checkGeometry );
	    }

	    break;

	case DOClevPARA:
	    docListParaNode( bd, indent, node, checkGeometry );
	    break;

	case DOClevOUT:
	default:
	    break;
	}

    appDebug( "%*s}\n", IS* indent+ IS, "" );
    
    return;
    }

void docListRootNode(	const struct BufferDocument *	bd,
			int				indent,
			const struct BufferItem *	node,
			int				checkGeometry )
    {
    while( node->biParent )
	{ node= node->biParent;	}

    docListNode( bd, indent, node, checkGeometry );

    return;
    }

const char * docLevelStr( int level )
    {
    static char	scratch[12];

    switch( level )
	{
	case DOClevANY:		return "ANY ";
	case DOClevOUT:		return "OUT ";

	case DOClevBODY:	return "BODY";
	case DOClevSECT:	return "SECT";
	case DOClevROW:		return "ROW ";
	case DOClevCELL:	return "CELL";
	case DOClevPARA:	return "PARA";
	case DOClevSPAN:	return "SPAN";

	case DOClevCOLUMN:	return "COLM";
	case DOClevTABLE:	return "TABL";

	default:
	    sprintf( scratch, "%-4d", level );
	    return scratch;
	}
    }

