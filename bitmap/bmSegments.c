#   include	"bitmapConfig.h"

#   include	<string.h>
#   include	"bmSegments.h"
#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Build an administration of the 'segments' in an image.		*/
/*									*/
/*  All pixels in a segment have the same color, and the segment is	*/
/*  connected.								*/
/*									*/
/************************************************************************/

typedef struct LookUp
    {
    int				lu_x0, lu_x1;
    SegmentEdge *		lu_edge;
    SegmentNode *		lu_node;
    int				lu_use;
    int				lu_compid;
    } LookUp;

typedef struct CompIndex
    {
    BitmapSegment *	ci_compo;
    int			ci_count;
    } CompIndex;

#   define	LU_UNSPEC	1
#   define	LU_ENDED	2
#   define	LU_EXTENDED	3
#   define	LU_MERGED	4

/************************************************************************/
/*									*/
/*  Insert a node in the administration of a segment.			*/
/*									*/
/************************************************************************/

static int bmRememberSegmentNode(	BitmapSegment *		bs,
					SegmentNode *		sn )
    {
    SegmentNode **	nw;
    int			row= sn->snY0;

    nw= (SegmentNode **)realloc( bs->bsNodes,
			    ( bs->bsNodeCount+ 1 )* sizeof(SegmentNode *) );

    if  ( ! nw )
	{ XDEB(nw); return -1;	}

    bs->bsNodes= nw;
    nw[bs->bsNodeCount++]= sn;

    if  ( bs->bsNodeCount <= 1 )
	{ bs->bsY0= bs->bsY1= row; }
    else{
	if  ( bs->bsY0 >  row	)
	    { bs->bsY0 =  row;	}
	if  ( bs->bsY1 <= row		)
	    { bs->bsY1 =  row+ 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert an edge in the administration of a segment.			*/
/*									*/
/************************************************************************/

static int bmRememberSegmentEdge(	BitmapSegment *		bs,
					SegmentEdge *		se )
    {
    SegmentEdge **	nw;
    int			i;
    DataRun *		dr;

    nw= (SegmentEdge **)realloc( bs->bsEdges,
			    ( bs->bsEdgeCount+ 1 )* sizeof(SegmentEdge *) );

    if  ( ! nw )
	{ XDEB(nw); return -1;	}

    bs->bsEdges= nw;
    nw[bs->bsEdgeCount++]= se;

    dr= se->seRuns;
    if  ( bs->bsEdgeCount <= 1 )
	{ bs->bsX0= dr->drX0; bs->bsX1= dr->drX1; }

    for ( i= 0; i < se->seRunCount; dr++, i++ )
	{
	if  ( bs->bsX0 >  dr->drX0	)
	    { bs->bsX0 =  dr->drX0;	}
	if  ( bs->bsX1 <  dr->drX1	)
	    { bs->bsX1 =  dr->drX1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a fresh node.						*/
/*									*/
/************************************************************************/

static SegmentNode * bmSegmentNode(	BitmapSegment *	bs,
					int		row )
    {
    SegmentNode *	sn;

    sn= (SegmentNode *)malloc( sizeof(SegmentNode) );

    if  ( ! sn )
	{ XDEB(sn); return sn;	}

    sn->snDownEdges= (SegmentEdge **)0;
    sn->snUpEdges= (SegmentEdge **)0;
    sn->snUpEdgeCount= 0;
    sn->snDownEdgeCount= 0;
    sn->snY0= row;

    if  ( bs && bmRememberSegmentNode( bs, sn ) )
	{ free( sn ); return (SegmentNode *)0;	}

    return sn;
    }

/************************************************************************/
/*									*/
/*  Insert a new edge into a node.					*/
/*									*/
/************************************************************************/

static int bmConnectSegmentEdgeToNode(	SegmentNode *	sn,
					SegmentEdge *	se,
					int		up )
    {
    short int *		pcount;
    SegmentEdge ***	pold;

    unsigned	sz;
    SegmentEdge **	nw;

    if  ( up )
	{ pcount= &sn->snUpEdgeCount;	pold= &sn->snUpEdges;	 }
    else{ pcount= &sn->snDownEdgeCount;	pold= &sn->snDownEdges; }

    sz= ( *pcount+ 1 )* sizeof(SegmentEdge *);
    nw= (SegmentEdge **)realloc( *pold, sz );

    if  ( ! nw )
	{ XDEB(nw); return -1;	}
    *pold= nw;
    nw[(*pcount)++]= se;

    if  ( up )
	{ se->seTo= sn;		}
    else{ se->seFrom= sn;	}

    return 0;
    }

/************************************************************************/
/*  Allocate an edge. Insert the first run.				*/
/*  1)  To help subsequent realloc()s a little, more space than needed	*/
/*	is claimed for the runs.					*/
/************************************************************************/

static SegmentEdge * bmSegmentEdge(	BitmapSegment *	bs,
					SegmentNode *	from,
					int		x0,
					int		x1 )
    {
    SegmentEdge *	se;

    se= (SegmentEdge *)malloc( sizeof(SegmentEdge) );

    if  ( ! se )
	{ XDEB(se); return se;	}

    se->seRuns= (DataRun *)malloc( 5* sizeof(DataRun) );
    if  ( ! se->seRuns )
	{ free( se ); return (SegmentEdge *)0;	}

    se->seRuns->drX0= x0;
    se->seRuns->drX1= x1;
    se->seRuns->drRepeatCount= 1;

    se->seRunCount= 1;
    se->seFrom= from;
    se->seTo= (SegmentNode *)0;

    if  ( bmRememberSegmentEdge( bs, se ) )
	{
	free( se->seRuns ); free( se );
	return (SegmentEdge *)0;
	}

    if  ( from && bmConnectSegmentEdgeToNode( from, se, 0 ) )
	{
	free( se->seRuns ); free( se );

	if  ( bs )
	    { bs->bsEdgeCount--;	}
	return (SegmentEdge *)0;
	}

    return se;
    }

/************************************************************************/
/*									*/
/*  Insert a run into an edge.						*/
/*									*/
/*  NOTE								*/
/*  1)	That the first run has been inserted at creation time, so no	*/
/*	initial allocation of the array of runs is necessary.		*/
/*  2)  To help subsequent realloc()s a little, more space than needed	*/
/*	is claimed for the runs.					*/
/*									*/
/************************************************************************/

static int bmAddRunToSegmentEdge(	BitmapSegment *		bs,
					SegmentEdge *		se,
					int			x0,
					int			x1 )
    {
    DataRun *	dr;
    int		nrun;
    unsigned	sz;

    if  ( se->seRunCount > 0 )
	{
	dr= se->seRuns+ se->seRunCount- 1;

	if  ( dr->drX0 == x0 && dr->drX1 == x1 )
	    { dr->drRepeatCount++; return 0;	}
	}

    if  ( se->seRunCount % 5 )
	{ nrun= se->seRunCount+ 1; }
    else{ nrun= se->seRunCount+ 5; }

    /*  2  */
    sz= nrun* sizeof(DataRun);
    dr= (DataRun *)realloc( se->seRuns, sz );
    if  ( ! dr )
	{ LDEB(dr); return -1;	}

    se->seRuns= dr;
    dr += se->seRunCount++;

    dr->drX0= x0;
    dr->drX1= x1;
    dr->drRepeatCount= 1;

    if  ( bs->bsX0 >  x0	)
	{ bs->bsX0 =  x0;	}
    if  ( bs->bsX1 <  x1	)
	{ bs->bsX1 =  x1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Destroy a segemnt.							*/
/*									*/
/************************************************************************/

static void bmFreeEdge(		SegmentEdge *	se )
    {
    if  ( se->seRuns )
	{ free( se->seRuns );	}

    free( se );
    }

static void bmFreeNode(		SegmentNode *	sn )
    {
    if  ( sn->snUpEdges )
	{ free( sn->snUpEdges );	}
    if  ( sn->snDownEdges )
	{ free( sn->snDownEdges );	}

    free( sn );
    }

static void bmFreeSegment(	BitmapSegment * bs )
    {
    int		i;

    for ( i= 0; i < bs->bsEdgeCount; i++ )
	{
	if  ( bs->bsEdges[i] )
	    { bmFreeEdge( bs->bsEdges[i] );	}
	}

    for ( i= 0; i < bs->bsNodeCount; i++ )
	{
	if  ( bs->bsNodes[i] )
	    { bmFreeNode( bs->bsNodes[i] );	}
	}

    if  ( bs->bsEdges )
	{ free( bs->bsEdges ); }

    if  ( bs->bsNodes )
	{ free( bs->bsNodes ); }

    free( bs );

    return;
    }

/************************************************************************/
/*  Construct a new component.						*/
/************************************************************************/
static BitmapSegment * bmNewSegment( void )
    {
    BitmapSegment *	bs;

    bs= (BitmapSegment *)malloc( sizeof(BitmapSegment) );

    if  ( ! bs )
	{ XDEB(bs); return bs;	}

    bs->bsEdges= (SegmentEdge **) malloc( sizeof(SegmentEdge *) );
    bs->bsNodes= (SegmentNode **) malloc( sizeof(SegmentNode *) );

    if  ( ! bs->bsNodes		||
	  ! bs->bsEdges		)
	{
	XXDEB(bs->bsEdges,bs->bsNodes); bmFreeSegment( bs );
	return (BitmapSegment *)0;
	}

    bs->bsX0= bs->bsX1= bs->bsY0= bs->bsY1= -1;

    bs->bsNodeCount= bs->bsEdgeCount= 0;

    return bs;
    }

/************************************************************************/
/*									*/
/*  Copy everything in one component to another one.			*/
/*									*/
/************************************************************************/

static int bmMoveComponentContents(	BitmapSegment *	to,
					BitmapSegment *	from )
    {
    int	i;

    for ( i= 0; i < from->bsNodeCount; i++ )
	{
	if  ( bmRememberSegmentNode( to, from->bsNodes[i] ) )
	    { LDEB(i); return -1;	}

	from->bsNodes[i]= (SegmentNode *)0;
	}

    for ( i= 0; i < from->bsEdgeCount; i++ )
	{
	if  ( bmRememberSegmentEdge( to, from->bsEdges[i] ) )
	    { LDEB(i); return -1;	}

	from->bsEdges[i]= (SegmentEdge *)0;
	}

    bmFreeSegment( from );

    return 0;
    }

/************************************************************************/
/*  1)  Find the first '0' pixel on of after position 'col' in a row	*/
/*  2)  Find the first '1' pixel on of after position 'col' in a row	*/
/************************************************************************/

/*  1  */
static int bmc0Run( const unsigned char *	buf,
		    int				bytesPerRow,
		    int				xsz,
		    int				col,
		    int				nfret )
    {
    int		colbyte= col/ 8;
    int		bit= col- 8* colbyte;
    int		c;

    while( colbyte < bytesPerRow )
	{
	if  ( ( c= buf[colbyte] ) != 0xff )
	    {
	    while( bit < 8 )
		{
		if  ( ! ( c & Bmc1Masks[bit] ) )
		    {
		    int	rval= 8* colbyte+ bit;
		    if  ( rval >= xsz )
			{ return nfret;	}
		    else{ return rval;	}
		    }
		bit++;
		}
	    }

	colbyte++; bit= 0;
	}

    return nfret;
    }

/*  2  */
static int bmc1Run( const unsigned char *	buf,
		    int				bytesPerRow,
		    int				xsz,
		    int				col,
		    int				nfret )
    {
    int		colbyte= col/ 8;
    int		bit= col- 8* colbyte;
    int		c;

    while( colbyte < bytesPerRow )
	{
	if  ( ( c= buf[colbyte] ) != 0x00 )
	    {
	    while( bit < 8 )
		{
		if  ( c & Bmc1Masks[bit] )
		    {
		    int	rval= 8* colbyte+ bit+ 1;
		    if  ( rval >= xsz )
			{ return nfret;	}
		    else{ return rval;	}
		    }
		bit++;
		}
	    }

	colbyte++; bit= 0;
	}

    return nfret;
    }

static int bmGetDataRun(	int *				pX0,
				int *				pX1,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				row,
				int				x0 )
    {
    int		x1;

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    x0= bmc1Run( buffer+ row* bd->bdBytesPerRow,
			    bd->bdBytesPerRow, bd->bdPixelsWide, x0, -1 );

	    if  ( x0 < 0 )
		{ return 1;	}

	    x1= bmc0Run( buffer+ row* bd->bdBytesPerRow,
				    bd->bdBytesPerRow, bd->bdPixelsWide,
				    x0+ 1, bd->bdPixelsWide );
	    break;

	case BMcoWHITEBLACK:
	    x0= bmc0Run( buffer+ row* bd->bdBytesPerRow,
			    bd->bdBytesPerRow, bd->bdPixelsWide, x0, -1 );

	    if  ( x0 < 0 )
		{ return 1;	}

	    x1= bmc1Run( buffer+ row* bd->bdBytesPerRow,
				    bd->bdBytesPerRow, bd->bdPixelsWide,
				    x0+ 1, bd->bdPixelsWide );
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    *pX0= x0; *pX1= x1; return 0;
    }

/************************************************************************/
/*									*/
/*  Make 'sn' the terminating branch node				*/
/*									*/
/*  1)  Make the new edge that branches off.				*/
/*  2)  Insert the old edge as an up going edge in the node.		*/
/*  3)  ?								*/
/*  4)  Remove one run from the edge that came from above.		*/
/*  5)  The new edge continues down at this location.			*/
/*									*/
/************************************************************************/

static int bcBranch(	CompIndex *		ci,
			LookUp *		lu,
			SegmentNode *		sn )
    {
    SegmentEdge *	se;
    DataRun *		dr;

    /*  1  */
    se= bmSegmentEdge( ci->ci_compo, sn, lu->lu_x0, lu->lu_x1 );
    if  ( ! se )
	{ XDEB(se); return -1;	}

    /*  2  */
    if  ( bmConnectSegmentEdgeToNode( sn, lu->lu_edge, 1 ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    ci->ci_count++;

    /*  4  */
    dr= lu->lu_edge->seRuns+ lu->lu_edge->seRunCount- 1;
    if  ( dr->drRepeatCount > 1 )
	{ dr->drRepeatCount--;		}
    else{ lu->lu_edge->seRunCount--;	}

    /*  5  */
    lu->lu_edge= se;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Create a new component.						*/
/*									*/
/*  Memory management of the components is rudimentary. I think it will	*/
/*  do for this application.						*/
/*									*/
/************************************************************************/

static int bmStartNewSegment(	int *			pnco,
				int *			pmco,
				CompIndex **		pcomps,
				SegmentEdge **		pse,
				int			x0,
				int			x1,
				int			row )
    {
    int			nco= *pnco;
    int			mco= *pmco;
    CompIndex *		comps= *pcomps;
    BitmapSegment *	bs;

    SegmentNode *	sn;
    SegmentEdge *	se;

    bs= bmNewSegment();
    if  ( ! bs )
	{ XDEB(bs); return -1;	}

    if  ( nco >= mco )
	{
	mco= ( 3* mco )/2+ 1;
	comps= (CompIndex *)realloc( comps, mco* sizeof(CompIndex) );
	if  ( ! comps )
	    { return -1;	}
	}

    comps[nco].ci_compo= bs;
    comps[nco].ci_count= 1;

    sn= bmSegmentNode( bs, row );
    if  ( ! sn )
	{ XDEB(sn); return -1;	}
    se= bmSegmentEdge( bs, sn, x0, x1 );
    if  ( ! se )
	{ XDEB(se); return -1;	}

    *pcomps= comps;
    *pnco= nco+ 1;
    *pmco= mco;
    *pse= se;

    return nco;
    }

/************************************************************************/
/*									*/
/*  Get rid of a component because it has been merged with another one.	*/
/*  Memory management of the components is rudimentary. I think it will	*/
/*  do for this application.						*/
/*									*/
/************************************************************************/

static int bcMergeComp(	CompIndex *	comps,
			int *		pnco,
			int		newcomp,
			int		oldcomp,
			LookUp *	olu,
			int		onlu,
			LookUp *	nlu,
			int		nnlu )
    {
    int		nco= *pnco;

    if  ( oldcomp == newcomp )
	{ comps[oldcomp].ci_count--; return 0; }

    if  ( comps[oldcomp].ci_compo )
	{
	if  ( bmMoveComponentContents(	comps[newcomp].ci_compo,
					comps[oldcomp].ci_compo )	)
	    { LDEB(1); return -1;	}

	comps[oldcomp].ci_compo= (BitmapSegment *)0;
	comps[oldcomp].ci_count= 0;

	while( nco > 0 && ! comps[nco- 1].ci_compo )
	    { nco--;	}

	*pnco= nco;

	while( --onlu >= 0 )
	    {
	    if  ( olu->lu_compid == oldcomp	)
		{ olu->lu_compid= newcomp; }
	    olu++;
	    }
	while( --nnlu >= 0 )
	    {
	    if  ( nlu->lu_compid == oldcomp	)
		{ nlu->lu_compid= newcomp; }
	    nlu++;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the count of newcomp					*/
/*									*/
/************************************************************************/

static void bcCountComp(	CompIndex *	comps,
				int		newcomp,
				LookUp *	olu,
				int		onlu,
				LookUp *	nlu,
				int		nnlu )
    {
    comps[newcomp].ci_count= 0;

    while( --onlu >= 0 )
	{
	if  ( olu->lu_use != LU_ENDED	&&
	      olu->lu_compid == newcomp	)
	    { comps[newcomp].ci_count++; }
	olu++;
	}

    while( --nnlu >= 0 )
	{
	if  ( nlu->lu_compid == newcomp	)
	    { comps[newcomp].ci_count++; }
	nlu++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Terminate an edge with a node.					*/
/*  This destroys one reference to the component.			*/
/*									*/
/*  1)  Make the end node.						*/
/*  2)  Attach the edge to it.						*/
/*									*/
/************************************************************************/

static int bcTerminate(	CompIndex *	ci,
			LookUp *	plu,
			int		row )
    {
    SegmentNode *	sn;

    /*  1  */
    sn= bmSegmentNode( ci->ci_compo, row );
    if  ( ! sn )
	{ XDEB(sn); return -1;	}

    /*  2  */
    if  ( bmConnectSegmentEdgeToNode( sn, plu->lu_edge, 1 ) )
	{ LDEB(1); return -1;	}

    plu->lu_use= LU_ENDED;
    plu->lu_node= sn;
    ci->ci_count--;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the different black connected components on a page.		*/
/*  For every successive scanline, the relationships of the runs of	*/
/*  black with those on the previous one are determined.		*/
/*									*/
/************************************************************************/

int bcComponents(	BitmapSegment ***		pSegments,
			int *				pCount,
			const unsigned char *		buffer,
			const BitmapDescription *	bd )
    {
    int			row;

    int			pixelsWide= bd->bdPixelsWide;

    int			nnlu, onlu, beg, end;
    LookUp *		lu1= (LookUp *)malloc( pixelsWide* sizeof(LookUp) );
    LookUp *		lu2= (LookUp *)malloc( pixelsWide* sizeof(LookUp) );
    LookUp *		olu= lu1;
    LookUp *		nlu= lu2;
    LookUp *		swap;

    SegmentNode *	sn;
    SegmentEdge *	se;

    int			x0, x1;

    int			nco= 0, mco= 30;
    CompIndex *		comps= (CompIndex *)malloc( mco* sizeof(CompIndex) );
    int			compid;
    BitmapSegment **	retcomps= (BitmapSegment **)0;

    if  ( ! lu1 || ! lu2 || ! comps )
	{ XXDEB(lu1,lu2); XDEB(comps); goto failure; }

    onlu= 0;

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	x0= 0; nnlu= 0; beg= end= 0;

	for (;;)
	    {
	    int		ret= bmGetDataRun( &x0, &x1, buffer, bd, row, x0 );

	    if  ( ret < 0 )
		{ LDEB(ret); goto failure;	}
	    if  ( ret > 0 )
		{ break;			}

	    /*
	    APP_DEB(appDebug( "++ RUN %5d .. %5d AT %3d\n", x0, x1, nnlu ));
	    */

	    /************************************************************/
	    /*  Identify what kind of a run we found.			*/
	    /*  1)  First one that does not end before x0.		*/ 
	    /*  2)  First one that does begin after x1.			*/ 
	    /************************************************************/
	    /*  1  */
	    while( beg < onlu )
		{
		compid= olu[beg].lu_compid;
		if  ( olu[beg].lu_x1 >  x0 )
		    { break;	}
		if  ( olu[beg].lu_use == LU_UNSPEC )
		    {
		    /*
		    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
				"ENDED", comps, olu, beg, beg+1 ));
		    */
		    if  ( bcTerminate( comps+ compid, olu+ beg, row ) )
			{ goto failure;	}
		    }
		else{
		    /*
		    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
				"READY", comps, olu, beg, beg+1 ));
		    */
		    }
		beg++;
		}
	    end= beg;

	    /*  2  */
	    while( end < onlu )
		{
		if  ( olu[end].lu_x0 >= x1 )
		    { break;	}
		/*
		APP_DEB(bcPrint( olu[end].lu_x0, olu[end].lu_x1, end,
			    "PREVIOUS", comps, olu, end, end+1 ));
		*/
		end++;
		}

	    if  ( end > beg )
		{
		compid= olu[beg].lu_compid;

		if  ( beg < end- 1 )
		    {
		    /*
		    APP_DEB(bcPrint( x0, x1, nnlu,
				"MERGE", comps, olu, beg, end ));
		    */
		    sn= bmSegmentNode( comps[compid].ci_compo, row );
		    if  ( ! sn )
			{ XDEB(sn); goto failure;	}

		    while( beg < end- 1 )
			{
			if  ( bcMergeComp( comps, &nco,
					   compid, olu[beg+1].lu_compid,
					   olu+ beg, onlu- beg,
					   nlu, nnlu ) )
			    { goto failure;	}
			if  ( bmConnectSegmentEdgeToNode( sn,
						    olu[beg].lu_edge, 1 ) )
			    { LDEB(1); goto failure;	}
			/*
			APP_DEB(bcPrintEdge(beg,olu[beg].lu_edge));
			*/
			olu[beg].lu_use= LU_MERGED;
			olu[beg].lu_node= sn;
			olu[beg].lu_compid= compid;
			beg++;
			}

		    bcCountComp( comps, compid, olu+beg, onlu-beg, nlu, nnlu );
		    if  ( bmConnectSegmentEdgeToNode( sn,
							olu[beg].lu_edge, 1 ) )
			{ goto failure;	}
		    se= bmSegmentEdge( comps[compid].ci_compo, sn, x0, x1 );
		    if  ( ! se )
			{ XDEB(se); goto failure;	}
		    se->seRunCount= 0;	/* Oups, against an extra row	*/
		    olu[beg].lu_node= sn;
		    olu[beg].lu_edge= se;
		    olu[beg].lu_compid= compid;
		    /*
		    APP_DEB(bcPrint( x0, x1, nnlu,
				"-----", comps, olu, beg, end ));
		    */
		    }

		if  ( olu[beg].lu_use != LU_UNSPEC )
		    {
		    /*
		    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
				"SPLIT", comps, olu, beg, beg+1 ));
		    */
		    sn= nlu[nnlu-1].lu_node;
		    if  ( ! sn )
			{
			sn= bmSegmentNode( comps[compid].ci_compo, row );
			if  ( ! sn )
			    { XDEB(sn); goto failure;	}
			if  ( bcBranch( comps+compid, nlu+ nnlu- 1, sn ) )
			    { LDEB(1); goto failure;	}
			nlu[nnlu-1].lu_node= sn;
			}
		    else{ comps[compid].ci_count++; }
		    se= bmSegmentEdge( comps[compid].ci_compo, sn, x0, x1 );
		    if  ( ! se )
			{ XDEB(se); goto failure;	}
		    /* olu[beg].lu_use= LU_EXTENDED; see above */
		    nlu[nnlu].lu_x0= x0; nlu[nnlu].lu_x1= x1;
		    nlu[nnlu].lu_edge= se;
		    nlu[nnlu].lu_compid= compid;
		    nlu[nnlu].lu_use= LU_UNSPEC;
		    nlu[nnlu].lu_node= sn;
		    nnlu++;
		    }
		else{
		    compid= olu[beg].lu_compid;
		    /*
		    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
				"EXTENDED", comps, olu, beg, beg+1 ));
		    */
		    if  ( bmAddRunToSegmentEdge( comps[compid].ci_compo,
						olu[beg].lu_edge, x0, x1 ) )
			{ LDEB(1); goto failure;	}
		    olu[beg].lu_use= LU_EXTENDED;
		    nlu[nnlu].lu_x0= x0; nlu[nnlu].lu_x1= x1;
		    nlu[nnlu].lu_edge= olu[beg].lu_edge;
		    nlu[nnlu].lu_compid= compid;
		    nlu[nnlu].lu_use= LU_UNSPEC;
		    nlu[nnlu].lu_node= (SegmentNode *)0;
		    nnlu++;
		    }
		}
	    else{
		compid= bmStartNewSegment( &nco, &mco, &comps, &se,
								x0, x1, row );
		if  ( compid < 0 )
		    { LDEB(compid); goto failure;	}
		nlu[nnlu].lu_compid= compid;
		nlu[nnlu].lu_x0= x0; nlu[nnlu].lu_x1= x1;
		nlu[nnlu].lu_edge= se;
		nlu[nnlu].lu_node= (SegmentNode *)0;
		nlu[nnlu].lu_use= LU_UNSPEC;
		nnlu++;
		/*
		APP_DEB(bcPrint( x0, x1, nnlu,
			    "NEW", comps, nlu, nnlu-1, nnlu ));
		*/
		}
	    x0= x1+ 1;
	    }

	while( beg < onlu )
	    {
	    compid= olu[beg].lu_compid;
	    if  ( olu[beg].lu_use == LU_UNSPEC )
		{
		/*
		APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
			    "ENDED", comps, olu, beg, beg+1 ));
		*/
		if  ( bcTerminate( comps+ compid, olu+ beg, row ) )
		    { goto failure;	}
		}
	    else{
		/*
		APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
			    "READY", comps, olu, beg, beg+1 ));
		*/
		}
	    beg++;
	    }

	swap= nlu; nlu= olu; olu= swap;
	onlu= nnlu;
	}

    /*
    APP_DEB(appDebug( "%-5d ROWS:\n", row ));
    */
    beg= 0;
    while( beg < onlu )
	{
	compid= olu[beg].lu_compid;
	if  ( olu[beg].lu_use == LU_UNSPEC )
	    {
	    /*
	    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
			"BOTTOM", comps, olu, beg, beg+1 ));
	    */
	    if  ( bcTerminate( comps+ compid, olu+ beg, row ) )
		{ goto failure;	}
	    }
	else{
	    /*
	    APP_DEB(bcPrint( olu[beg].lu_x0, olu[beg].lu_x1, beg,
			"ERROR", comps, olu, beg, beg+1 ));
	    */
	    goto failure;
	    }
	beg++;
	}

    retcomps= (BitmapSegment **)malloc( nco* sizeof(BitmapSegment *) );
    if  ( ! retcomps )
	{ goto failure;	}
    end= nco; nco= 0;
    for ( beg= 0; beg < end; beg++ )
	{
	if  ( comps[beg].ci_compo )
	    { retcomps[nco++]= comps[beg].ci_compo; }
	}

    if  ( lu1 )
	{ free( lu1 );		}
    if  ( lu2 )
	{ free( lu2 );		}
    if  ( comps )
	{ free( comps );	}

    *pCount= nco;
    *pSegments= retcomps;

    return 0;

failure:
    for ( beg= 0; beg < nco; beg++ )
	{ bmFreeSegment( comps[beg].ci_compo ); }

    if  ( lu1 )
	{ free( lu1 );		}
    if  ( lu2 )
	{ free( lu2 );		}
    if  ( comps )
	{ free( comps );	}

    return -1;
    }

int bmcDrawComponent(	const BitmapSegment *	bs,
			unsigned char *		buffer,
			int			col0,
			int			row0,
			int			bytesPerRow,
			int			colorEncoding	)
    {
    SegmentEdge **	se;
    int			i, j;

#   if 0
    bmDrawBox( buffer,
	bs->bsX0- col0, row0,
	bs->bsX1- col0, row0+ bs->bsY1- bs->bsY0,
	1, bytesPerRow, colorEncoding );
    bmDrawLine( buffer,
	bs->bsX0- col0, row0,
	bs->bsX1- col0, row0+ bs->bsY1- bs->bsY0,
	2, bytesPerRow );
    bmDrawLine( buffer,
	bs->bsX0- col0, row0+ bs->bsY1- bs->bsY0,
	bs->bsX1- col0, row0,
	2, bytesPerRow );

    return 0;
#   endif

    se= bs->bsEdges;
    for ( i= 0; i < bs->bsEdgeCount; i++, se++ )
	{
	int		row= se[0]->seFrom->snY0- bs->bsY0+ row0;
	DataRun *	dr= se[0]->seRuns;

	for ( j= 0; j < se[0]->seRunCount; j++, dr++ )
	    {
	    bmFillBlock( buffer,
			    dr->drX0- col0, row,
			    dr->drX1- col0+ 1, row+ dr->drRepeatCount,
			    bytesPerRow );

	    row += dr->drRepeatCount;
	    }
	}

    return 0;
    }

/************************************************************************/
/*  Return information on a component. If a buffer pointer is given,	*/
/*  a bitmap for the component is returned.				*/
/************************************************************************/
int bmComponentBitmap(	unsigned char **		pBuffer,
			BitmapDescription *		bdout,
			BitmapSelection *		bsel,
			const BitmapDescription *	bdin,
			const void *			voidbs )
    {
    const BitmapSegment *	bs= (const BitmapSegment *)voidbs;
    BitmapDescription		resbd= *bdin;
    int				byte= 8* resbd.bdBitsPerPixel;
    unsigned char *		buffer;

    int				colorEncoding= bdin->bdColorEncoding;

    resbd.bdPixelsWide= bs->bsX1- bs->bsX0;
    resbd.bdPixelsWide= ( ( resbd.bdPixelsWide+ byte- 1 )/ byte ) * byte;
    resbd.bdPixelsHigh= bs->bsY1- bs->bsY0;

    resbd.bdBytesPerRow= resbd.bdPixelsWide/ byte;

    resbd.bdBufferLength= resbd.bdBytesPerRow* resbd.bdPixelsHigh;

    if  ( pBuffer )
	{
	buffer= bmBackgroundBuffer( resbd.bdBufferLength, colorEncoding );
	if  ( ! buffer )
	    { LLDEB(resbd.bdBufferLength,buffer); return -1;	}

	if  ( bmcDrawComponent(	bs,
				buffer,
				bs->bsX0, 0,
				resbd.bdBytesPerRow,
				colorEncoding		) )
	    { free( buffer ); return -1;	}

	*pBuffer= buffer;
	}

    *bdout= resbd;

    bsel->bsX0= bs->bsX0;
    bsel->bsY0= bs->bsY0;
    bsel->bsPixelsWide= resbd.bdPixelsWide;
    bsel->bsPixelsHigh= resbd.bdPixelsHigh;

    return 0;
    }

/************************************************************************/
/*  Return information on the geometry of a component.			*/
/************************************************************************/
void bmcStatistics(	const BitmapSegment *		bs,
			int *				pN,
			float *				pSx,
			float *				pSy,
			float *				pSxx,
			float *				pSyy,
			float *				pSxy	)
    {
    SegmentEdge **	se;
    int			i, j;

    *pN= 0;
    *pSx= 0;
    *pSy= 0;
    *pSxx= 0;
    *pSyy= 0;
    *pSxy= 0;

    se= bs->bsEdges;
    for ( i= 0; i < bs->bsEdgeCount; i++, se++ )
	{
	DataRun *	dr;

	int		y0= se[0]->seFrom->snY0;

	dr= se[0]->seRuns;
	for ( j= 0; j < se[0]->seRunCount; j++, dr++ )
	    {
	    int		x0= dr->drX0;
	    int		x1= dr->drX1+ 1;
	    int		xx0= x0* ( x0+ 1 );
	    int		xx1= x1* ( x1+ 1 );
	    int		sx= xx1/2- xx0/2;
	    double	xxx0= xx0* ( 2.0* x0+ 1.0 );
	    double	xxx1= xx1* ( 2.0* x1+ 1.0 );

	    int		y1= y0+ dr->drRepeatCount;
	    int		yy0= y0* ( y0+ 1 );
	    int		yy1= y1* ( y1+ 1 );
	    int		sy= yy1/2- yy0/2;
	    double	yyy0= yy0* ( 2.0* y0+ 1.0 );
	    double	yyy1= yy1* ( 2.0* y1+ 1.0 );

	    *pN += dr->drRepeatCount* ( x1- x0 );
	    *pSx += dr->drRepeatCount* sx;
	    *pSy += ( x1- x0 )* sy;
	    *pSxx += dr->drRepeatCount* ( xxx1/6- xxx0/6 );
	    *pSyy += ( x1- x0 )* ( yyy1/6- yyy0/6 );
	    *pSxy += sx* sy;

	    y0 += dr->drRepeatCount;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fit a line segment to a collection of dots.				*/
/*									*/
/*  The line segment is from A= ( a1, a2 ) to B= ( b1, b2 ).		*/
/*  Its length l: l= sqrt( (b1-a1)^2 + (b2-a2)^2 ).			*/
/*									*/
/*  For a point C= ( c1, c2 ) be:					*/
/*  r= { (a2-c2)*(a2-b2)- (a1-c1)*(b1-a1) }/ l^2.			*/
/*  s= { (a2-c2)*(b1-a1)- (a1-c1)*(b2-a2) }/ l^2.			*/
/*									*/
/*  The distance of A to the projection p of C on the line segment is:	*/
/*	r* l.								*/
/*  The distance of B to the projection p of C on the line segment is:	*/
/*	(1-r)* l.							*/
/*  The distance of C to the projection p of C on the line segment is:	*/
/*	s* l.								*/
/*									*/
/************************************************************************/
