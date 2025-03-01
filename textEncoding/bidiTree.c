/************************************************************************/
/*									*/
/*  A node in the bidirectional tree.					*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"bidiTree.h"
#   include	"ucdBidiClass.h"
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

void bidiInitRun(		BidiRun *	br )
    {
    br->brFrom= 0;
    br->brUpto= 0;
    br->brLevel= 0;
    br->brInitiator= UCDbidi_ON;
    }

void bidiInitTreeNode(			BidiNode *	bn )
    {
    bidiInitRun( &(bn->bnRun) );

    bn->bnParent= (BidiNode *)0;
    bn->bnChildren= (BidiNode **)0;
    bn->bnChildCount= 0;
    bn->bnNumberInParent= 0;

    return;
    }

void bidiCleanTreeNode(			BidiNode *	bn )
    {
    int		i;

    for ( i= 0; i < bn->bnChildCount; i++ )
	{
	if  ( bn->bnChildren[i] )
	    {
	    bidiCleanTreeNode( bn->bnChildren[i] );
	    free( bn->bnChildren[i] );
	    }
	}

    if  ( bn->bnChildren )
	{ free( bn->bnChildren );	}

    return;
    }

BidiNode * bidiNodeStartTree(	int		initiator,
				int		level,
				int		from,
				int		upto )
    {
    BidiNode *	fresh= (BidiNode *)0;

    if  ( level < 0 || level > 1 )
	{ LDEB(level); return (BidiNode *)0;	}

    fresh= malloc( sizeof(BidiNode) );
    if  ( ! fresh )
	{ XDEB(fresh); return (BidiNode *)0;	}

    bidiInitTreeNode( fresh );

    fresh->bnRun.brLevel= level;
    fresh->bnRun.brInitiator= initiator;
    fresh->bnRun.brFrom= from;
    fresh->bnRun.brUpto= upto;

    return fresh;
    }

static int bidiNodeFindChild(	BidiNode *		parent,
				int			offset )
    {
    int	l= 0;
    int	r= parent->bnChildCount;
    int	m= ( l+ r )/ 2;

    /* ge */
    while( l < m )
	{
	if  ( parent->bnChildren[m]->bnRun.brFrom < offset )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    if  ( offset <= parent->bnChildren[m]->bnRun.brFrom )
	{ return m;		}
    else{ return m+ 1;	}
    }

/************************************************************************/
/*									*/
/*  Path the initiator of a partial run. I.E between the children of	*/
/*  a node.								*/
/*									*/
/************************************************************************/

static int bidiSetPartialRunInitiator(	BidiRun *		br )
    {
    switch( br->brInitiator )
	{
	case UCDbidi_R:
	case UCDbidi_L:
	    return 0;

	case UCDbidi_LRO:
	case UCDbidi_LRE:
	case UCDbidi_LRI:
	    br->brInitiator= UCDbidi_L;
	    return 0;

	case UCDbidi_RLO:
	case UCDbidi_RLE:
	case UCDbidi_RLI:
	    br->brInitiator= UCDbidi_R;
	    return 0;

	case UCDbidi_FSI:
	default:
	    SDEB(ucdBidiClassStr(br->brInitiator));
	    return -1;
	}
    }

BidiNode * bidiFindNode(	BidiRun *		br,
				BidiNode *		parent,
				int			offset )
    {
    if  ( ! parent )
	{ XDEB(parent); return (BidiNode *)0;	}

    if  ( offset < parent->bnRun.brFrom || offset > parent->bnRun.brUpto )
	{
	LLLDEB(parent->bnRun.brFrom,offset,parent->bnRun.brUpto);
	return (BidiNode *)0;
	}

    while( parent->bnChildCount > 0 )
	{
	int		nip= bidiNodeFindChild( parent, offset );
	BidiNode *	child;

	/* past last child */
	if  ( nip >= parent->bnChildCount )
	    {
	    child= parent->bnChildren[nip-1];

	    *br= parent->bnRun;
	    if  ( bidiSetPartialRunInitiator( br ) )
		{ LDEB(1); return (BidiNode *)0;	}
	    br->brFrom= child->bnRun.brUpto;
	    return parent;
	    }

	/* inside child */
	child=  parent->bnChildren[nip];
	if  ( offset >= child->bnRun.brFrom	&&
	      offset <= child->bnRun.brUpto	)
	    {
	    parent= child; continue;
	    }

	/* before child */
	*br= parent->bnRun;
	br->brUpto= child->bnRun.brFrom;
	if  ( nip > 0 )
	    {
	    if  ( bidiSetPartialRunInitiator( br ) )
		{ LDEB(1); return (BidiNode *)0;	}
	    br->brFrom= parent->bnChildren[nip-1]->bnRun.brUpto;
	    }

	return parent;
	}

    *br= parent->bnRun;
    return parent;
    }

BidiNode * bidiNodeAddChild(	BidiNode *	parent,
				int		initiator,
				int		level,
				int		from,
				int		upto )
    {
    BidiNode *	child= (BidiNode *)0;
    BidiNode **	children= (BidiNode **)0;
    int		nip= 0;

    if  ( from < 0 || upto < from )
	{ LLDEB(from,upto); return (BidiNode *)0;	}

    if  ( initiator != UCDbidi_R	&&
	  initiator != UCDbidi_L	&&
	  initiator != UCDbidi_LRO	&&
	  initiator != UCDbidi_RLO	&&
	  initiator != UCDbidi_LRE	&&
	  initiator != UCDbidi_RLE	&&
	  initiator != UCDbidi_FSI	&&
	  initiator != UCDbidi_LRI	&&
	  initiator != UCDbidi_RLI	)
	{ SDEB(ucdBidiClassStr(initiator)); return (BidiNode *)0;	}

    if  ( level < parent->bnRun.brLevel	||
	  level > parent->bnRun.brLevel+ 2	)
	{
	LLLLDEB(level,parent->bnRun.brLevel,from,upto);
	SSDEB(ucdBidiClassStr(initiator),
			    ucdBidiClassStr(parent->bnRun.brInitiator));
	return (BidiNode *)0;
	}

    if  ( level == parent->bnRun.brLevel )
	{
	if  ( ( initiator != UCDbidi_R			&&
	        initiator != UCDbidi_L			)	||
	      ( parent->bnRun.brInitiator != UCDbidi_LRO	&&
	        parent->bnRun.brInitiator != UCDbidi_RLO	&&
	        parent->bnRun.brInitiator != UCDbidi_LRE	&&
	        parent->bnRun.brInitiator != UCDbidi_RLE	&&
	        parent->bnRun.brInitiator != UCDbidi_FSI	&&
	        parent->bnRun.brInitiator != UCDbidi_LRI	&&
	        parent->bnRun.brInitiator != UCDbidi_RLI	)	)
	    {
	    SSDEB(ucdBidiClassStr(initiator),
			    ucdBidiClassStr(parent->bnRun.brInitiator));
	    return (BidiNode *)0;
	    }
	}

    children= (BidiNode **)realloc( parent->bnChildren,
			    ( parent->bnChildCount+ 1 )* sizeof(BidiNode *) );
    if  ( ! children )
	{ LXDEB(parent->bnChildCount,children); return (BidiNode *)0;	}
    parent->bnChildren= children;

    /*  Find the last child of parent with bnFrom >= from; if there is	*/
    /*  a previous one, it must have bnUpto >= from. (Avoid overlap)	*/

    if  ( parent->bnChildCount > 0 )
	{
	nip= bidiNodeFindChild( parent, from );

	/*  Overlap with subsequent is wrong	*/
	if  ( nip < parent->bnChildCount		&&
	      parent->bnChildren[nip]->bnRun.brFrom > upto	)
	    {
	    LLLDEB(nip,parent->bnChildren[nip]->bnRun.brFrom,upto);
	    return (BidiNode *)0;
	    }

	/*  Overlap with previous is wrong	*/
	if  ( nip > 0					&&
	      parent->bnChildren[nip-1]->bnRun.brUpto > from	)
	    {
	    LLLDEB(nip,parent->bnChildren[nip-1]->bnRun.brUpto,from);
	    return (BidiNode *)0;
	    }
	}

    child= malloc( sizeof(BidiNode) );
    if  ( ! child )
	{ XDEB(child); return (BidiNode *)0;	}

    bidiInitTreeNode( child );

    child->bnParent= parent;
    child->bnNumberInParent= nip;
    child->bnRun.brLevel= level;
    child->bnRun.brInitiator= initiator;
    child->bnRun.brFrom= from;
    child->bnRun.brUpto= upto;

    if  ( nip < parent->bnChildCount )
	{
	int	i;

	for ( i= parent->bnChildCount; i > nip; i-- )
	    {
	    parent->bnChildren[i]= parent->bnChildren[i-1];
	    parent->bnChildren[i]->bnNumberInParent= i;
	    }
	}

    parent->bnChildren[nip]= child;
    parent->bnChildCount++;

    return child;
    }

void bidiNodeDeleteNode(	BidiNode *	node )
    {
    BidiNode *	parent= node->bnParent;

    if  ( node->bnParent )
	{
	if  ( node->bnNumberInParent >= parent->bnChildCount )
	    { LLDEB(node->bnNumberInParent,parent->bnChildCount);	}
	else{
	    int		i;

	    parent->bnChildCount--;

	    for ( i= node->bnNumberInParent; i < parent->bnChildCount; i++ )
		{ parent->bnChildren[i]= parent->bnChildren[i+1];	}
	    }
	}

    bidiCleanTreeNode( node );
    free( node );

    return;
    }

int bidiStretchNode(	BidiNode *	bn,
			int		upto )
    {
    while( bn )
	{
	BidiNode *	parent= bn->bnParent;

	if  ( parent && bn->bnNumberInParent < parent->bnChildCount- 1 )
	    {
	    const BidiNode *	next;

	    next= parent->bnChildren[bn->bnNumberInParent+1];

	    if  ( upto > next->bnRun.brFrom )
		{ LLDEB(upto,next->bnRun.brFrom); return -1;	}
	    }

	if  ( bn->bnRun.brUpto < upto )
	    { bn->bnRun.brUpto=  upto;	}

	bn= parent;
	}

    return 0;
    }

int bidiShiftNode(	BidiNode *	bn,
			int		from,
			int		by )
    {
    int		i;

    if  ( bn->bnRun.brFrom > from )
	{ bn->bnRun.brFrom += by;	}

    i= 0;
    while( i < bn->bnChildCount && bn->bnChildren[i]->bnRun.brUpto < from )
	{ i++;	}
    while( i < bn->bnChildCount )
	{
	if  ( bidiShiftNode( bn->bnChildren[i], from, by ) )
	    { LDEB(i); return -1;	}

	i++;
	}

    if  ( bn->bnRun.brUpto >= from )
	{ bn->bnRun.brUpto += by;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Debugging routine to list tree contents.				*/
/*									*/
/************************************************************************/

static void bidiListNodeHead(
			const BidiNode *	bn,
			int			indent,
			int			maxIndent,
			const char *		syms )
    {
    int		i;
    char	scratch[20];

    appDebug( "%3d ", bn->bnRun.brFrom );

    if  ( indent > 0 )
	{
	for ( i= 0; i < indent- 1; i++ )
	    {
	    appDebug( "   |" );
	    }
	appDebug( "   +" );
	}

    sprintf( scratch, "%d:%s",
		    bn->bnRun.brLevel,
		    ucdBidiClassStr( bn->bnRun.brInitiator ) );
    appDebug( "---+ %-6s", scratch );

    for ( i= indent; i < maxIndent; i++ )
	{
	appDebug( "    " );
	}

    appDebug( "%3d..%3d ", bn->bnRun.brFrom, bn->bnRun.brUpto );

    appDebug( "\n" );
    }

static void bidiListNodeTail(
			const BidiNode *	bn,
			int			indent,
			const char *		syms )
    {
    int		i;

    appDebug( "%3d ", bn->bnRun.brUpto );

    for ( i= 0; i < indent; i++ )
	{
	appDebug( "   |" );
	}

    appDebug( "\n" );
    }

static void bidiListNodeRange(
			const BidiNode *	bn,
			int			indent,
			int			maxIndent,
			int			last,
			const char *		syms,
			int			from,
			int			upto )
    {
    int		i;

    appDebug( "    " );

    for ( i= 0; i < indent; i++ )
	{
	appDebug( "   |" );
	}

    if  ( last )
	{ appDebug( "   \\" );	}
    else{ appDebug( "   |" );	}

    for ( i= indent; i < maxIndent; i++ )
	{
	appDebug( "    " );
	}

    appDebug( "       %3d..%3d: %d: ", from, upto, bn->bnRun.brLevel );

    if  ( syms )
	{ appDebug( " \"%.*s\"", upto- from, syms+ from );	}

    appDebug( "\n" );
    }

static void bidiListNodeImpl(
			const BidiNode *	bn,
			int			indent,
			int			maxIndent,
			const char *		syms )
    {
    int			i;
    int			from;

    if  ( ! bn )
	{ XDEB(bn); return;	}

    bidiListNodeHead( bn, indent, maxIndent, syms );

    from= bn->bnRun.brFrom;
    for ( i= 0; i < bn->bnChildCount; i++ )
	{
	const BidiNode *	ch= bn->bnChildren[i];

	if  ( ch->bnParent != bn )
	    { SXXDEB("####",ch->bnParent,bn);	}
	if  ( ch->bnNumberInParent != i )
	    { SLLDEB("####",ch->bnNumberInParent,i);	}
	if  ( ch->bnRun.brFrom < from )
	    { SLLLDEB("####",i,ch->bnRun.brFrom,from);	}

	if  ( ch->bnRun.brFrom > from )
	    {
	    const int	last= i == bn->bnChildCount- 1;

	    bidiListNodeRange( bn, indent, maxIndent, last, syms,
						from, ch->bnRun.brFrom );
	    }

	bidiListNodeImpl( ch, indent+ 1, maxIndent, syms );

	from= ch->bnRun.brUpto;
	}

    if  ( bn->bnRun.brUpto > from )
	{
	const int	last= 1;

	bidiListNodeRange( bn, indent, maxIndent, last, syms,
						from, bn->bnRun.brUpto );
	}

    bidiListNodeTail( bn, indent, syms );
    }

static int bidiMaxIndent(
			const BidiNode *	bn,
			int			indent )
    {
    int		maxIndent= indent;

    if  ( bn )
	{
	int		i;

	for ( i= 0; i < bn->bnChildCount; i++ )
	    {
	    int	childIndent= bidiMaxIndent( bn->bnChildren[i], indent+ 1 );

	    if  ( maxIndent < childIndent )
		{ maxIndent=  childIndent;	}
	    }
	}

    return maxIndent;
    }

void bidiListNode(	const BidiNode *	bn,
			const MemoryBuffer *	symbols )
    {
    const char *	syms= (const char *)0;
    int			indent= 0;
    int			maxIndent= bidiMaxIndent( bn, indent );

    if  ( ! bn )
	{ XDEB(bn); return;	}

    if  ( symbols )
	{ syms= utilMemoryBufferGetString( symbols );	}

    bidiListNodeImpl( bn, indent, maxIndent, syms );
    }

