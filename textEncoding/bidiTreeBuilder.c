/************************************************************************/
/*									*/
/*  Build trees to represent the hierarchy defined by the unicode	*/
/*  bidirectionals algorithm.						*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>

#   include	"textEncodingConfig.h"

#   include	"bidiTreeBuilder.h"
#   include	"bidiTreeBuilderImpl.h"
#   include	"ucdBidiClass.h"
#   include	"bidiTree.h"
#   include	"bidiAlgo.h"
#   include	"bidiAlgoExplicit.h"

#   include	<appDebugon.h>

void bidiStartTreeBuilder(	BidiTreeBuilder *	btb,
				int			bottomLevel )
    {
    btb->btbBottomNode= (BidiNode *)0;
    btb->btbCurrentNode= (BidiNode *)0;

    btb->btbOffset= 0;
    btb->btbHighestLevel= bottomLevel;

    while( btb->btbExplicitLevel )
	{
	btb->btbExplicitLevel= bidiPopExplicitLevel( btb->btbExplicitLevel );
	}

    return;
    }

BidiTreeBuilder * bidiOpenTreeBuilder( void )
    {
    BidiTreeBuilder *	btb= malloc( sizeof(BidiTreeBuilder) );

    if  ( ! btb )
	{ XDEB(btb); return (BidiTreeBuilder *)0;	}

    btb->btbBottomNode= (BidiNode *)0;
    btb->btbCurrentNode= (BidiNode *)0;

    btb->btbOffset= 0;
    btb->btbHighestLevel= 0;

    btb->btbExplicitLevel= (struct ExplicitLevel *)0;

    return btb;
    }

int bidiFinishTreeBuilder(		struct BidiScanner *	bs,
					BidiTreeBuilder *	btb,
					int			upto )
    {
    if  ( btb->btbCurrentNode )
	{
	if  ( bidiStretchNode( btb->btbCurrentNode, upto ) )
	    { LDEB(upto); return -1;	}
	}

    while( btb->btbExplicitLevel )
	{
	btb->btbExplicitLevel= bidiPopExplicitLevel( btb->btbExplicitLevel );
	}

    btb->btbCurrentNode= (BidiNode *)0;

    return 0;
    }

void bidiCloseTreeBuilder(		BidiTreeBuilder *	btb )
    {
    free( btb );
    }

BidiNode * bidiTreeBuilderGetRootNode(	BidiTreeBuilder *	btb )
    { return btb->btbBottomNode; }

int bidiTreeBuilderGetCurrentRun(	BidiRun *		br,
					BidiTreeBuilder *	btb )
    {
    BidiNode *		bn;

    if  ( ! btb->btbCurrentNode )
	{ XDEB(btb->btbCurrentNode); return -1;	}

    bn= bidiFindNode( br, btb->btbCurrentNode, btb->btbOffset );
    if  ( ! bn )
	{ LXDEB(btb->btbOffset,bn); return -1;	}

    return br->brFrom;
    }

int bidiTreeBuilderStartExistingTree(	BidiTreeBuilder *	btb,
					BidiNode *		bottomNode,
					int			byteOffset )
    {
    BidiNode *		bidiNode= (BidiNode *)0;
    BidiRun		bidiRun;

    bidiInitRun( &bidiRun );

    if  ( ! bottomNode )
	{ XDEB(bottomNode); return -1;	}

    bidiNode= bidiFindNode( &bidiRun, bottomNode, byteOffset );
    if  ( ! bidiNode )
	{
	LXDEB(byteOffset,bidiNode);
	bidiListNode(bottomNode, (const struct MemoryBuffer *)0);
	return -1;
	}

    btb->btbOffset= byteOffset;
    btb->btbCurrentNode= bottomNode;
    btb->btbHighestLevel= bottomNode->bnRun.brLevel;

    btb->btbCurrentNode= bidiNode;
    btb->btbHighestLevel= bidiRun.brLevel;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Push an explicit level on the stack.				*/
/*									*/
/************************************************************************/

static int bidiTreeBuilderPushXLevel(
				BidiTreeBuilder *	btb,
				BidiNode *		parent,
				int			sos,
				int			iInitiator,
				int			xLevel,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	fresh;

    fresh= bidiNodeAddChild( parent, initiator, level, from, upto );
    if  ( ! fresh )
	{
	LSDEB(parent->bnRun.brLevel,
	    ucdBidiClassStr(parent->bnRun.brInitiator));
	LSLDEB(level,ucdBidiClassStr(initiator),from);
	return -1;
	}

    btb->btbCurrentNode= fresh;
    btb->btbOffset= upto;

    btb->btbExplicitLevel= bidiPushExplicitLevel(
		    btb->btbExplicitLevel, sos, iInitiator, xLevel );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add an implicit level to the tree. We do not push an explicit	*/
/*  level and we do not re-initialize the implicit state.		*/
/*									*/
/************************************************************************/

static int bidiTreeBuilderStartILevel(
				BidiTreeBuilder *	btb,
				BidiNode *		parent,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	fresh;

    fresh= bidiNodeAddChild( parent, initiator, level, from, upto );
    if  ( ! fresh )
	{
	LSDEB(parent->bnRun.brLevel,
	    ucdBidiClassStr(parent->bnRun.brInitiator));
	LSLDEB(level,ucdBidiClassStr(initiator),from);
	return -1;
	}

    btb->btbCurrentNode= fresh;
    btb->btbOffset= upto;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start building a bidirectional tree.				*/
/*									*/
/************************************************************************/

static int bidiTreeBuilderStartRoot(	BidiTreeBuilder *	btb,
					int			initiator,
					int			level,
					int			from,
					int			upto )
    {
    switch( initiator )
	{
	BidiNode *	fresh;

	case UCDbidi_LRE:
	case UCDbidi_RLE:

	case UCDbidi_LRO:
	case UCDbidi_RLO:

	case UCDbidi_LRI:
	case UCDbidi_RLI:
	case UCDbidi_FSI:

	    if  ( level < 0 || level > 1 )
		{
		LSLDEB(level,ucdBidiClassStr(initiator),from);
		return -1;
		}

	    if  ( btb->btbExplicitLevel )
		{ XDEB(btb->btbExplicitLevel); return -1;	}

	    if  ( from != 0 )
		{ LDEB(from);	}

	    fresh= bidiNodeStartTree( initiator, level, from, upto );
	    if  ( ! fresh )
		{
		LSLDEB(level,ucdBidiClassStr(initiator),from);
		return -1;
		}

	    btb->btbBottomNode= fresh;
	    btb->btbCurrentNode= fresh;
	    btb->btbOffset= upto;

	    btb->btbHighestLevel= level;

	    btb->btbExplicitLevel= bidiPushExplicitLevel(
		    btb->btbExplicitLevel, level, level, level );

	    if  ( ! btb->btbExplicitLevel )
		{ XDEB(btb->btbExplicitLevel); return -1;	}

	    return 0;

	default:
	    LSLDEB(level,ucdBidiClassStr(initiator),from);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Pop a level that happens to be explicit (from the initiator) from	*/
/*  the stack.								*/
/*									*/
/*  1)  Delete empty nodes, but not the root node.			*/
/*	In corner cases, this may lead to results that do not conform	*/
/*	to the official tr9 specification.				*/
/*									*/
/************************************************************************/

static int bidiTreeBuilderPopExplicitLevel(
				const struct BidiScanner *	bs,
				BidiTreeBuilder *		btb,
				int				eor,
				int				upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    if  ( ! btb->btbCurrentNode || ! btb->btbExplicitLevel )
	{ XXDEB(btb->btbCurrentNode,btb->btbExplicitLevel); return -1;	}

    if  ( bidiStretchNode( current, upto ) )
	{ LDEB(upto); return -1;	}

    if  ( bidiFlushRun( bs, &(btb->btbExplicitLevel->el_ImplicitState),
								eor, upto ) )
	{ LLDEB(eor,upto); return -1;	}

    btb->btbExplicitLevel= bidiPopExplicitLevel( btb->btbExplicitLevel );
    btb->btbCurrentNode= current->bnParent;

    /*  1  */
    if  ( current->bnParent					&&
	  current->bnRun.brUpto == current->bnRun.brFrom	)
	{ bidiNodeDeleteNode( current );	}

    return 0;
    }

static int bidiTreeBuilderPopImplicitLevel(
				BidiTreeBuilder *		btb,
				int				upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    if  ( ! current )
	{ XDEB(current); return 0;	}

    btb->btbCurrentNode= current->bnParent;

    /*  1  */
    if  ( current->bnParent					&&
	  current->bnRun.brUpto == current->bnRun.brFrom	)
	{ bidiNodeDeleteNode( current );	}

    return 0;
    }

static int bidiTreeBuilderToLowerLevelImplicit(
				BidiTreeBuilder *		btb,
				int				initiator,
				int				level,
				int				from,
				int				upto )
    {
    if  ( ! btb->btbCurrentNode )
	{ XDEB(btb->btbCurrentNode); return -1;	}

    if  ( bidiStretchNode( btb->btbCurrentNode, from ) )
	{ LDEB(from); return -1;	}

    while( btb->btbCurrentNode && btb->btbCurrentNode->bnRun.brLevel > level )
	{
	switch( btb->btbCurrentNode->bnRun.brInitiator )
	    {
	    case UCDbidi_L:
	    case UCDbidi_R:

		if  ( bidiTreeBuilderPopImplicitLevel( btb, from ) )
		    { LDEB(upto); return -1;	}

		continue;

	    default:
		LSLLDEB(btb->btbCurrentNode->bnRun.brLevel,
		    ucdBidiClassStr(btb->btbCurrentNode->bnRun.brInitiator),
		    from,upto);
		return -1;
	    }
	}

    return 0;
    }

static int bidiTreeBuilderPopLevel(
				const struct BidiScanner *	bs,
				BidiTreeBuilder *		btb,
				int				upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    if  ( ! current )
	{ XDEB(current); return -1;	}

    switch( current->bnRun.brInitiator )
	{
	case UCDbidi_LRE:
	case UCDbidi_LRO:

	    if  ( bidiTreeBuilderPopExplicitLevel( bs, btb, UCDbidi_L, upto ) )
		{ LDEB(upto); return -1;	}

	    return 0;

	case UCDbidi_RLE:
	case UCDbidi_RLO:

	    if  ( bidiTreeBuilderPopExplicitLevel( bs, btb, UCDbidi_R, upto ) )
		{ LDEB(upto); return -1;	}

	    return 0;

	case UCDbidi_LRI:
	case UCDbidi_FSI:
	case UCDbidi_RLI:

	    if  ( bidiTreeBuilderPopExplicitLevel( bs, btb, UCDbidi_ON, upto ) )
		{ LDEB(upto); return -1;	}

	    return 0;

	case UCDbidi_L:
	case UCDbidi_R:

	    if  ( bidiTreeBuilderPopImplicitLevel( btb, upto ) )
		{ LDEB(upto); return -1;	}

	    return 0;

	default:
	    LSLDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator),upto);
	    return -1;
	}

    return 0;
    }

static int bidiTreeBuilderToLowerLevel(
				const struct BidiScanner *	bs,
				BidiTreeBuilder *		btb,
				int				initiator,
				int				level,
				int				from,
				int				upto )
    {
    if  ( bidiStretchNode( btb->btbCurrentNode, from ) )
	{ LDEB(from); return -1;	}

    while( btb->btbCurrentNode && btb->btbCurrentNode->bnRun.brLevel > level )
	{
	if  ( bidiTreeBuilderPopLevel( bs, btb, from ) )
	    { LDEB(from); return -1;	}
	}

    if  ( ! btb->btbCurrentNode )
	{ XDEB(btb->btbCurrentNode); return -1;	}

    if  ( btb->btbCurrentNode->bnRun.brLevel == level		&&
	  btb->btbCurrentNode->bnRun.brInitiator == initiator	)
	{
	btb->btbOffset= upto;
	return 0;
	}

    LSDEB(btb->btbCurrentNode->bnRun.brLevel,
		ucdBidiClassStr(btb->btbCurrentNode->bnRun.brInitiator));
    LLDEB(btb->btbCurrentNode->bnRun.brLevel,level);
    return -1;
    }

static int bidiTreeBuilderLevel2Up(
				BidiTreeBuilder *	btb,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    switch( current->bnRun.brInitiator )
	{
	case UCDbidi_LRE:
	case UCDbidi_LRO:
	case UCDbidi_LRI:

	    switch( initiator )
		{
		case UCDbidi_LRE:
		case UCDbidi_LRO:
		case UCDbidi_LRI:

		    if  ( bidiTreeBuilderPushXLevel( btb, current,
					UCDbidi_L, UCDbidi_L, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		case UCDbidi_L:

		    if  ( bidiTreeBuilderStartILevel( btb, current,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}
	    break;

	case UCDbidi_RLE:
	case UCDbidi_RLO:
	case UCDbidi_RLI:

	    switch( initiator )
		{
		case UCDbidi_RLE:
		case UCDbidi_RLO:
		case UCDbidi_RLI:

		    if  ( bidiTreeBuilderPushXLevel( btb, current,
					UCDbidi_R, UCDbidi_R, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		case UCDbidi_R:

		    if  ( bidiTreeBuilderStartILevel( btb, current,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}
	    break;

	case UCDbidi_ON:
	case UCDbidi_FSI:
	default:
	    SLLLDEB(ucdBidiClassStr(initiator),level,from,upto);
	    return -1;
	}

    return 0;
    }

static int bidiTreeBuilderLevel1Up(
				BidiTreeBuilder *	btb,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    switch( current->bnRun.brInitiator )
	{
	case UCDbidi_LRE:
	case UCDbidi_LRO:
	case UCDbidi_LRI:

	    switch( initiator )
		{
		case UCDbidi_R:

		    if  ( bidiTreeBuilderStartILevel( btb, current,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		case UCDbidi_RLE:
		case UCDbidi_RLO:
		case UCDbidi_RLI:

		    if  ( bidiTreeBuilderPushXLevel( btb, current,
					UCDbidi_R, UCDbidi_R, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}
	    break;

	case UCDbidi_L:

	    switch( initiator )
		{
		case UCDbidi_RLE:
		case UCDbidi_RLO:
		case UCDbidi_RLI:

		    if  ( ! current->bnParent )
			{ XDEB(current->bnParent); return -1;	}

		    if  ( bidiStretchNode( btb->btbCurrentNode, from ) )
			{ LDEB(from); return -1;	}

		    if  ( bidiTreeBuilderPushXLevel( btb, current->bnParent,
					UCDbidi_R, UCDbidi_R, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}
	    break;

	case UCDbidi_RLE:
	case UCDbidi_RLO:
	case UCDbidi_RLI:

	    switch( initiator )
		{
		case UCDbidi_L:

		    if  ( bidiTreeBuilderStartILevel( btb, current,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		case UCDbidi_LRE:
		case UCDbidi_LRO:
		case UCDbidi_LRI:

		    if  ( bidiTreeBuilderPushXLevel( btb, current,
					UCDbidi_L, UCDbidi_L, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}
	    break;

	case UCDbidi_R:

	    switch( initiator )
		{
		case UCDbidi_LRE:
		case UCDbidi_LRO:
		case UCDbidi_LRI:

		    if  ( ! current->bnParent )
			{ XDEB(current->bnParent); return -1;	}

		    if  ( bidiStretchNode( btb->btbCurrentNode, from ) )
			{ LDEB(from); return -1;	}

		    if  ( bidiTreeBuilderPushXLevel( btb, current->bnParent,
					UCDbidi_L, UCDbidi_L, level,
					initiator, level, from, upto ) )
			{ LDEB(level); return -1;	}

		    return 0;

		default:
		    LSDEB(level,ucdBidiClassStr(initiator));
		    LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
		    return -1;
		}

	case UCDbidi_ON:
	case UCDbidi_FSI:
	default:
	    SLLLDEB(ucdBidiClassStr(initiator),level,from,upto);
	    return -1;
	}

    return 0;
    }

static int bidiTreeBuilderToHigherLevel(
				BidiTreeBuilder *	btb,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    if  ( level > current->bnRun.brLevel+ 2 )
	{
	LSDEB(current->bnRun.brLevel,
			ucdBidiClassStr(current->bnRun.brInitiator));
	LSDEB(level,ucdBidiClassStr(initiator));
	return -1;
	}

    if  ( level == current->bnRun.brLevel+ 2 )
	{
	return bidiTreeBuilderLevel2Up( btb, initiator, level, from, upto );
	}

    if  ( level == current->bnRun.brLevel+ 1 )
	{
	return bidiTreeBuilderLevel1Up( btb, initiator, level, from, upto );
	}

    LSDEB(current->bnRun.brLevel,ucdBidiClassStr(current->bnRun.brInitiator));
    LSDEB(level,ucdBidiClassStr(initiator));
    return -1;
    }

static int bidiTreeBuilderToSameLevel(
				BidiTreeBuilder *	btb,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    BidiNode *	current= btb->btbCurrentNode;

    if  ( current->bnRun.brLevel == level		&&
	  current->bnRun.brInitiator == initiator	)
	{
	if  ( bidiStretchNode( btb->btbCurrentNode, upto ) )
	    { LDEB(upto); return -1;	}

	/*btb->btbCurrentNode= current;*/
	btb->btbOffset= upto;
	return 0;
	}

    LDEB(1); return -1;
    }

/************************************************************************/
/*									*/
/*  Add a run to the bidirectional tree. (Or extend an existing one)	*/
/*									*/
/*  This is to serve as the implementation of the implicit part of the	*/
/*  bidirectional algorithm.						*/
/*									*/
/************************************************************************/

int bidiTreeBuilderAddRun(	BidiTreeBuilder *	btb,
				int			initiator,
				int			level,
				int			from,
				int			upto )
    {
    int		rval;

    if  ( from < btb->btbOffset -1 )
	{ LLDEB(from,btb->btbOffset); return -1;	}

    switch( initiator )
	{
	/*
	case UCDbidi_LRE:
	case UCDbidi_LRO:

	case UCDbidi_RLE:
	case UCDbidi_RLO:

	case UCDbidi_LRI:
	case UCDbidi_RLI:
	case UCDbidi_FSI:

	case UCDbidi_ON:
	*/

	case UCDbidi_L:
	case UCDbidi_R:
	    break;

	default:
	    SLLLDEB(ucdBidiClassStr(initiator),level,from,upto);
	    return -1;
	}

    if  ( ! btb->btbCurrentNode )
	{
	rval= bidiTreeBuilderStartRoot( btb, initiator, level, from, upto );
	if  ( rval < 0 )
	    { LDEB(rval); return -1;	}

	return rval;
	}

    if  ( btb->btbCurrentNode->bnRun.brLevel > level )
	{
	rval= bidiTreeBuilderToLowerLevelImplicit( btb,
				    initiator, level, from, upto );
	if  ( rval < 0 )
	    {
	    LLLDEB(btb->btbCurrentNode->bnRun.brLevel,level,rval);
	    return -1;
	    }
	}
    else{
	if  ( btb->btbCurrentNode->bnRun.brLevel < level )
	    {
	    rval= bidiTreeBuilderToHigherLevel( btb,
					initiator, level, from, upto );
	    if  ( rval < 0 )
		{ LDEB(rval); return -1;	}
	    }
	else{
	    rval= bidiTreeBuilderToSameLevel( btb,
					initiator, level, from, upto );
	    if  ( rval < 0 )
		{ LDEB(rval); return -1;	}
	    }
	}

    if  ( btb->btbHighestLevel < level )
	{ btb->btbHighestLevel=  level;	}

    return rval;
    }

int bidiTreeBuilderHandleImplicitClass(
				const struct BidiScanner *	bs,
				BidiTreeBuilder *	 	btb,
				int				klass,
				int				offset )
    {
    if  ( ! btb->btbExplicitLevel )
	{ XDEB(btb->btbExplicitLevel); return -1;	}

    return bidiHandleImplicitClass( bs,
		&(btb->btbExplicitLevel->el_ImplicitState), klass, offset );
    }

int bidiTreeBuilderCloseExplicitRun(
				const struct BidiScanner *	bs,
				BidiTreeBuilder *		btb,
				int				eor,
				int				upto )
    {
    BidiNode *	node;

    if  ( ! btb->btbCurrentNode )
	{ XLDEB(btb->btbCurrentNode,upto); return -1;	}

    if  ( bidiTreeBuilderPopExplicitLevel( bs, btb, eor, upto ) )
	{ LLDEB(eor,upto); return -1;	}

    btb->btbCurrentNode->bnRun.brUpto= upto;

    node= btb->btbCurrentNode;

    btb->btbCurrentNode= btb->btbCurrentNode->bnParent;

    /* Delete empty children (not the root) */
    if  ( btb->btbCurrentNode				&&
	  node->bnRun.brUpto == node->bnRun.brFrom	)
	{ bidiNodeDeleteNode( node );	}
    else{
	node= node->bnParent;
	while( node )
	    {
	    if  ( node->bnRun.brUpto < upto )
		{ node->bnRun.brUpto=  upto;	}

	    node= node->bnParent;
	    }
	}

    return 0;
    }

int bidiTreeLevelSwitchToIsolateLevel(
				const struct BidiScanner *	bs,
				struct BidiTreeBuilder *	btb,
				int				level,
				int				offset )
    {
    int		rval;

    const int	initiator= (level == 1)?UCDbidi_RLI:UCDbidi_LRI;
    const int	from= offset;
    const int	upto= offset;

    if  ( level < 0 || level > 1 )
	{ LDEB(level); return -1;	}

    if  ( ! btb->btbCurrentNode )
	{
	int	from0= 0;
	int	uptoOffset= offset;

	rval= bidiTreeBuilderStartRoot( btb, initiator, level, from0, uptoOffset );
	if  ( rval < 0 )
	    { LDEB(rval); return -1;	}

	return rval;
	}

    if  ( btb->btbCurrentNode->bnRun.brLevel > level )
	{
	rval= bidiTreeBuilderToLowerLevel( bs, btb,
				    initiator, level, from, upto );
	if  ( rval < 0 )
	    { LDEB(rval); return -1;	}
	}
    else{
	if  ( btb->btbCurrentNode->bnRun.brLevel < level )
	    {
	    rval= bidiTreeBuilderToHigherLevel( btb,
					initiator, level, from, upto );
	    if  ( rval < 0 )
		{ LDEB(rval); return -1;	}
	    }
	else{
	    rval= bidiTreeBuilderToSameLevel( btb,
					initiator, level, from, upto );
	    if  ( rval < 0 )
		{ LDEB(rval); return -1;	}
	    }
	}

    if  ( btb->btbHighestLevel < level )
	{ btb->btbHighestLevel=  level;	}

    return rval;
    }
