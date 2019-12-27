/************************************************************************/
/*									*/
/*  A node in the bidirectional tree.					*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"bidiTree.h"
#   include	"bidiScanner.h"

#   include	<appDebugon.h>

/**
 *  Traverse ranges of contiguous children (Ignoring X9 classes)
 */
typedef struct TraverseNode
    {
    int			tnFrom;
    int			tnUpto;
    const BidiScanner *	tnScanner;
    } TraverseNode;

/************************************************************************/

static int bidiTraverseNodeOdd(		const BidiNode *	bn,
					TraverseNode *		tn,
					int			u );

static int bidiTraverseNodeEven(	const BidiNode *	bn,
					TraverseNode *		tn,
					int			f );

/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Got a non-technical (potentially adjacent) run at a certain level	*/
/*									*/
/*  1)  Is anything queued at a different level? .. It is no longer	*/
/*	part of an adjacent run so flush it.				*/
/*  2)  Levels < 2 are not affected by lower levels that swap them	*/
/*	again. So we can flush them immediately as the scan runs in the	*/
/*	correct order. This covers the vast majority of cases. In this	*/
/*	way we avoid the extra effort of the queue to find contiguous	*/
/*	runs for L2. [In our data structure, we keep the symbols that	*/
/*	are removed by X9, so runs at a ceratin level can be		*/
/*	interrupted by an X9 symbol at a different level.]		*/
/*  3)  Levels >= 2 are queued to find out whether they are contiguous	*/
/*	later on.							*/
/*									*/
/************************************************************************/

static int bidiTraverseGotLevel(	TraverseNode *		tn,
					int			initiator,
					int			level,
					int			from,
					int			upto )
    {
    int			res;
    const BidiScanner *	bs= tn->tnScanner;

    if  ( from < tn->tnFrom || upto > tn->tnUpto )
	{ LLLLDEB(from,tn->tnFrom,upto,tn->tnUpto); return -1;	}

    /*  1  */

    /*
    appDebug( "GOT %3d .. %3d L=%d %s\n",
		    from, upto, level, bidiEmbeddingStr( initiator ) );
    */

    /*  2  */
    res= (*bs->bsGotRun)( bs->bsThrough, initiator, level, from, upto );
    if  ( res < 0 )
	{ LLLLDEB(level,from,upto,res); return -1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Delimit a run at the same level as the node. The run consists of	*/
/*  any runs in the node and children at the same level. (They are the	*/
/*  result of implicit processing.)					*/
/*									*/
/*  Return the the number of children that we include. *pHead receives	*/
/*  the head of the run that we found.					*/
/*									*/
/*  The run is a level run in the sense of definition BD7 the unicode	*/
/*  tr9 document.							*/
/*									*/
/************************************************************************/

static int bidiTraverseCollectLevelRunBwd(
					int *			pHead,
					const BidiNode *	bn,
					int			l,
					int			from )
    {
    int		step= 0;
    int		head= bn->bnChildren[l]->bnRun.brUpto;

    while( l >= 0 )
	{
	const BidiNode *	ch= bn->bnChildren[l];

	if  ( ch->bnRun.brUpto < from )
	    { break;	}

	head= ch->bnRun.brUpto;

	if  ( ch->bnRun.brFrom < ch->bnRun.brUpto		&&
	      ch->bnRun.brLevel > bn->bnRun.brLevel		)
	    { break;	}

	head= ch->bnRun.brFrom;
	l--; step++;
	}

    if  ( head < from )
	{ head=  from;	}

    *pHead= head;
    return step;
    }

/************************************************************************/
/*									*/
/*  Delimit a run at the same level as the node. The run consists of	*/
/*  any runs in the node and children at the same level. (They are the	*/
/*  result of implicit processing.)					*/
/*									*/
/*  Return the the number of children that we include. *pTail receives	*/
/*  the tail of the run that we found.					*/
/*									*/
/*  The run is a level run in the sense of definition BD7 the unicode	*/
/*  tr9 document.							*/
/*									*/
/************************************************************************/

static int bidiTraverseCollectLevelRunFwd(
					int *			pTail,
					const BidiNode *	bn,
					int			f,
					int			upto )
    {
    int		step= 0;
    int		tail= bn->bnChildren[f]->bnRun.brFrom;

    while( f < bn->bnChildCount )
	{
	const BidiNode *	ch= bn->bnChildren[f];

	if  ( ch->bnRun.brFrom >= upto )
	    { break;	}

	tail= ch->bnRun.brFrom;

	if  ( ch->bnRun.brFrom < ch->bnRun.brUpto		&&
	      ch->bnRun.brLevel > bn->bnRun.brLevel		)
	    { break;	}

	tail= ch->bnRun.brUpto;
	f++; step++;
	}

    if  ( tail > upto )
	{ tail=  upto;	}

    *pTail= tail;
    return step;
    }

/************************************************************************/
/*									*/
/*  Count contiguous children that have a higher level. (Backwards).	*/
/*									*/
/************************************************************************/

static int bidiTraverseCollectDeeperBwd( int *			pHead,
					const BidiNode *	bn,
					int			l,
					int			from )
    {
    int			step= 0;
    int			head= bn->bnChildren[l]->bnRun.brUpto;

    while( l >= 0 )
	{
	const BidiNode *	ch= bn->bnChildren[l];

	if  ( ch->bnRun.brUpto < from			||
	      ch->bnRun.brUpto < head			||
	      ch->bnRun.brLevel == bn->bnRun.brLevel	)
	    { break;	}

	head= ch->bnRun.brFrom;
	l--; step++;
	}

    *pHead= head;
    return step;
    }

/************************************************************************/
/*									*/
/*  Count contiguous children that have a higher level (Forward)	*/
/*									*/
/************************************************************************/

static int bidiTraverseCollectDeeperFwd( int *			pTail,
					const BidiNode *	bn,
					int			f,
					int			upto )
    {
    int			step= 0;
    int			tail= bn->bnChildren[f]->bnRun.brFrom;

    while( f < bn->bnChildCount )
	{
	const BidiNode *	ch= bn->bnChildren[f];

	if  ( ch->bnRun.brFrom >= upto 			||
	      ch->bnRun.brFrom > tail			||
	      ch->bnRun.brLevel == bn->bnRun.brLevel	)
	    { break;	}

	tail= ch->bnRun.brUpto;
	f++; step++;
	}

    *pTail= tail;
    return step;
    }

/************************************************************************/
/*									*/
/*  Recursively scan the text runs and the children in bn.		*/
/*  This is a backward scan for an odd value of level.			*/
/*									*/
/*  1)  Scan content upto this child.					*/
/*  2)  That was all there is to do?					*/
/*  3)  Look for a contiguous range of (non-empty) children.		*/
/*  5)  Scan content after the last child.				*/
/*									*/
/************************************************************************/

static int bidiTraverseNodeOdd(		const BidiNode *	bn,
					TraverseNode *		tn,
					int			l )
    {
    int			from= tn->tnFrom;
    int			upto= tn->tnUpto;


    if  ( from < bn->bnRun.brFrom )
	{ from= bn->bnRun.brFrom;	}
    if  ( upto > bn->bnRun.brUpto )
	{ upto= bn->bnRun.brUpto;	}

    while( l >= 0 )
	{
	int			head= 0;
	int			step;
	int			deeper;

	/*  1  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectLevelRunBwd( &head, bn, l, from );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}

	if  ( upto > head )
	    {
	    int res;

	    res= bidiTraverseGotLevel( tn,
			    bn->bnRun.brInitiator, bn->bnRun.brLevel,
			    head, upto );
	    if  ( res < 0 )
		{ LLLLDEB(bn->bnRun.brLevel,head,upto,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}

	    upto= head;
	    }

	l -= step;

	/*  2  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectDeeperBwd( &head, bn, l, from );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	if  ( step == 0 )
	    { break;	}

	for ( deeper= l- step+ 1; deeper <= l; deeper++ )
	    {
	    int		res;

	    res= bidiTraverseNodeEven( bn->bnChildren[deeper], tn, 0 );
	    if  ( res < 0 )
		{ LLDEB(deeper,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}
	    }

	upto= head;
	l -= step;
	}

    /*  5  */
    if  ( from < upto )
	{
	int res;

	res= bidiTraverseGotLevel( tn,
				bn->bnRun.brInitiator, bn->bnRun.brLevel,
				from, upto );
	if  ( res < 0 )
	    { LLLLDEB(bn->bnRun.brLevel,from,upto,res); return -1;	}
	if  ( res > 0 )
	    { return res;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recursively scan the text runs and the children in bn.		*/
/*  This is a forward scan for an even value of level.			*/
/*									*/
/*  1)  Scan content upto this child.					*/
/*  2)  That was all there is to do?					*/
/*  3)  Look for a contiguous range of (non-empty) children.		*/
/*  5)  Scan content after the last child.				*/
/*									*/
/************************************************************************/

static int bidiTraverseNodeEven(	const BidiNode *	bn,
					TraverseNode *		tn,
					int			f )
    {
    int		from= tn->tnFrom;
    int		upto= tn->tnUpto;

    if  ( from < bn->bnRun.brFrom )
	{ from= bn->bnRun.brFrom;	}
    if  ( upto > bn->bnRun.brUpto )
	{ upto= bn->bnRun.brUpto;	}

    while( f < bn->bnChildCount )
	{
	int			tail= 0;
	int			step;
	int			deeper;

	/*  1  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectLevelRunFwd( &tail, bn, f, upto );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}

	if  ( from < tail )
	    {
	    int res;

	    res= bidiTraverseGotLevel( tn,
				bn->bnRun.brInitiator, bn->bnRun.brLevel,
				from, tail );
	    if  ( res < 0 )
		{ LLLLDEB(bn->bnRun.brLevel,from,tail,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}

	    from= tail;
	    }

	f += step;

	/*  2  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectDeeperFwd( &tail, bn, f, upto );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	if  ( step == 0 )
	    { break;	}

	for ( deeper= f+ step- 1; deeper >= f; deeper-- )
	    {
	    int			res;
	    BidiNode *		dpr= bn->bnChildren[deeper];
	    int			l= dpr->bnChildCount- 1;

	    while( l >= 0 )
		{
		if  ( dpr->bnChildren[l]->bnRun.brFrom <= upto )
		    { break;	}
		l--;
		}

	    res= bidiTraverseNodeOdd( dpr, tn, l );
	    if  ( res < 0 )
		{ LLDEB(deeper,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}
	    }

	from= tail;
	f += step;
	}

    /*  5  */
    if  ( from < upto )
	{
	int	res;

	res= bidiTraverseGotLevel( tn,
				bn->bnRun.brInitiator, bn->bnRun.brLevel,
				from, upto );
	if  ( res < 0 )
	    { LLLLDEB(bn->bnRun.brLevel,from,upto,res); return -1;	}
	if  ( res > 0 )
	    { return res;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the child that completely holds the range of text from _from_	*/
/*  upto _upto_.							*/
/*									*/
/************************************************************************/

static int bidiTraverseFindChild(	const BidiNode *	bn,
					int			from,
					int			upto )
    {
    int			f= 0;
    int			l= bn->bnChildCount- 1;

    while( f <= l && bn->bnChildren[f]->bnRun.brUpto < from )
	{ f++;	}
    while( l >= f && bn->bnChildren[l]->bnRun.brFrom > upto )
	{ l--;	}

    if  ( f != l					||
	  f >= bn->bnChildCount				||
	  bn->bnChildren[f]->bnRun.brFrom > from	||
	  l < 0						||
	  bn->bnChildren[l]->bnRun.brUpto < upto	)
	{ return -1;	}
    else{ return f;	}
    }

/************************************************************************/
/*									*/
/*  Traverse a range in the text in display order. Typically this is to */
/*  lay out the	contents of a line or to draw a line of text.		*/
/*									*/
/*  Approach:								*/
/*  1)  No overlap: Do nothing.						*/
/*  2)  Find the top node in the root that holds the complete selection	*/
/*									*/
/*  Return:								*/
/*  0)  On success (Callback returned 0 on all occasions)		*/
/*  1)  On success (Callback returned 1: scanning has stopped.		*/
/*  -1) On error							*/
/************************************************************************/

int bidiTraverseNodeDisplayOrder(
			const BidiNode *	root,
			const BidiScanner *	bs,
			int			from,
			int			upto )
    {
    int			rval= 0;
    TraverseNode	tn;

    if  ( ! bs->bsGotRun )
	{ XDEB(bs->bsGotRun); return -1;	}

    /*  1  */
    if  ( ! root )
	{ XDEB(root); return 0;	}
    if  ( root->bnRun.brUpto <= from || root->bnRun.brFrom >= upto )
	{ return 0;	}

    tn.tnFrom= from;
    tn.tnUpto= upto;
    tn.tnScanner= bs;

    /*  2  */
    while( root->bnChildCount > 0 )
	{
	int ch= bidiTraverseFindChild( root, from, upto );

	if  ( ch < 0 )
	    { break;	}

	root= root->bnChildren[ch];
	continue;
	}

    if  ( root->bnRun.brLevel % 2 == 0 )
	{
	int	res;
	int	f= 0;

	while( f < root->bnChildCount )
	    {
	    if  ( root->bnChildren[f]->bnRun.brUpto >= from )
		{ break;	}
	    f++;
	    }

	res= bidiTraverseNodeEven( root, &tn, f );
	if  ( res < 0 )
	    { LLLDEB(from,upto,res); rval= -1; goto ready;	}
	if  ( res > 0 )
	    { rval= res; goto ready;	}
	}
    else{
	int	res;
	int	l= root->bnChildCount- 1;

	while( l >= 0 )
	    {
	    if  ( root->bnChildren[l]->bnRun.brFrom <= upto )
		{ break;	}
	    l--;
	    }

	res= bidiTraverseNodeOdd( root, &tn, l );
	if  ( res < 0 )
	    { LLLDEB(from,upto,res); rval= -1; goto ready;	}
	if  ( res > 0 )
	    { rval= res; goto ready;	}
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Recursively scan the text runs and the children in bn.		*/
/*  This is a forward scan in logical order.				*/
/*									*/
/*  1)  Scan content upto this child.					*/
/*  2)  That was all there is to do?					*/
/*  3)  Look for a contiguous range of (non-empty) children.		*/
/*  5)  Scan content after the last child.				*/
/*									*/
/************************************************************************/

static int bidiTraverseNodeForward(	const BidiNode *	bn,
					TraverseNode *		tn,
					int			f )
    {
    int		from= tn->tnFrom;
    int		upto= tn->tnUpto;

    if  ( from < bn->bnRun.brFrom )
	{ from= bn->bnRun.brFrom;	}
    if  ( upto > bn->bnRun.brUpto )
	{ upto= bn->bnRun.brUpto;	}

    while( f < bn->bnChildCount )
	{
	int			tail= 0;
	int			step;
	int			deeper;

	/*  1  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectLevelRunFwd( &tail, bn, f, upto );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}

	if  ( from < tail )
	    {
	    int res;

	    res= bidiTraverseGotLevel( tn, bn->bnRun.brInitiator, bn->bnRun.brLevel,
								from, tail );
	    if  ( res < 0 )
		{ LLLLDEB(bn->bnRun.brLevel,from,tail,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}

	    from= tail;
	    }

	f += step;

	/*  2  */
	if  ( from >= upto )
	    { break;	}

	step= bidiTraverseCollectDeeperFwd( &tail, bn, f, upto );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	if  ( step == 0 )
	    { break;	}

	for ( deeper= f; deeper < f+ step; deeper++ )
	    {
	    int res;

	    res= bidiTraverseNodeForward( bn->bnChildren[deeper], tn,
				    bn->bnChildren[deeper]->bnChildCount- 1 );
	    if  ( res < 0 )
		{ LLDEB(deeper,res); return -1;	}
	    if  ( res > 0 )
		{ return res;	}
	    }

	from= tail;
	f += step;
	}

    /*  5  */
    if  ( from < upto )
	{
	int	res;

	res= bidiTraverseGotLevel( tn, bn->bnRun.brInitiator, bn->bnRun.brLevel,
							    from, upto );
	if  ( res < 0 )
	    { LLLLDEB(bn->bnRun.brLevel,from,upto,res); return -1;	}
	if  ( res > 0 )
	    { return res;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Traverse a range in the text in logical order. Typically this is to */
/*  save text or to search for a value.					*/
/*									*/
/*  Approach:								*/
/*  1)  No overlap: Do nothing.						*/
/*  2)  Find the top node in the root that holds the complete selection	*/
/*									*/
/************************************************************************/

int bidiTraverseNodeLogicalOrder(
			const BidiNode *	root,
			const BidiScanner *	bs,
			int			from,
			int			upto )
    {
    int			rval= 0;
    TraverseNode	tn;

    int			res;
    int			f= 0;


    if  ( ! bs->bsGotRun )
	{ XDEB(bs->bsGotRun); return -1;	}

    /*  1  */
    if  ( root->bnRun.brUpto <= from || root->bnRun.brFrom >= upto )
	{ return 0;	}

    tn.tnFrom= from;
    tn.tnUpto= upto;
    tn.tnScanner= bs;

    /*  2  */
    while( root->bnChildCount > 0 )
	{
	int ch= bidiTraverseFindChild( root, from, upto );

	if  ( ch < 0 )
	    { break;	}

	root= root->bnChildren[ch];
	continue;
	}

    f= 0;
    while( f < root->bnChildCount )
	{
	if  ( root->bnChildren[f]->bnRun.brUpto >= from )
	    { break;	}
	f++;
	}

    res= bidiTraverseNodeForward( root, &tn, f );
    if  ( res < 0 )
	{ LLLDEB(from,upto,res); rval= -1; goto ready;	}
    if  ( res > 0 )
	{ rval= res; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the (last) directional run that stroffFrom belongs to		*/
/*									*/
/************************************************************************/

typedef struct FindRun
    {
    int		frDirection;
    int		frFrom;
    int		frUpto;
    } FindRun;

static int bidiGotDirectionalRun(	void *		vfr,
					int		initiator,
					int		level,
					int		from,
					int		upto )
    {
    FindRun *	fr= (FindRun *)vfr;

    fr->frDirection= level % 2;
    fr->frFrom= from;
    fr->frUpto= upto;

    return 1;
    }

int bidiDelimitDirectionalRun(	int *			pDirection,
				int *			pUpto,
				const BidiNode *	root,
				int			from )
    {
    FindRun	fr;
    int		res;

    BidiScanner	bs;

    bidiInitScanner( &bs );
    bs.bsGotRun= bidiGotDirectionalRun;
    bs.bsThrough= &fr;

    if  ( from < root->bnRun.brFrom || from > root->bnRun.brUpto )
	{ LLLDEB(root->bnRun.brFrom,from,root->bnRun.brUpto); return 1;	}

    fr.frDirection= -1;
    fr.frFrom= -1;
    fr.frUpto= -1;

    res= bidiTraverseNodeDisplayOrder( root, &bs, from, root->bnRun.brUpto );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res == 0 )
	{ LLLDEB(from,root->bnRun.brUpto,res); return 1;	}

    *pDirection= fr.frDirection;
    *pUpto= fr.frUpto;

    return 0;
    }
