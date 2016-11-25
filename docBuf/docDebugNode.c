#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	<appDebugon.h>
#   include	"docNodeTree.h"

#   define	LIST_PARA_STRING	1
#   define	LIST_PARTICULES		1
#   define	LIST_FIRST_LINE		0
#   define	LIST_LINES		1

#   define	IS			2

static int docCheckGroupLeft(	const BufferItem *	bi )
    {
    int		rval= 0;

    int		prev= 0;
    int		here;

    if  ( bi->biParent && bi->biNumberInParent > 0 )
	{
	prev= bi->biParent->biChildren[bi->biNumberInParent-1]->
						biLeftParagraphs;
	}

    here= bi->biLeftParagraphs;

    if  ( here- prev !=
	    bi->biChildren[bi->biChildCount- 1]->biLeftParagraphs )
	{
	SLDEB(docLevelStr(bi->biLevel),here- prev);
	LDEB(bi->biChildren[bi->biChildCount- 1]->biLeftParagraphs);
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
		{ direction= "ABOVE";	}
	    if  ( d < 0 )
		{ direction= "BELOW";	}

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
	int	tableNesting= docTableNesting( child );

	if  ( tableNesting != child->biParaTableNesting )
	    {
	    SDEB("#######");
	    LLDEB(tableNesting,child->biParaTableNesting);
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

int docCheckNode(	const BufferItem *	bi,
			int			checkGeometry )
    {
    int			i;
    int			rval= 0;

    LayoutPosition	lpHere;

    if  ( bi->biLevel == DOClevPARA )
	{
	int			part;
	const TextParticule *	tp;
	int			stroff;
	int			tableNesting= docTableNesting( bi );

	if  ( bi->biLeftParagraphs != bi->biNumberInParent+ 1 )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
	    LLDEB(bi->biLeftParagraphs,bi->biNumberInParent+1);
	    }

	if  ( tableNesting != bi->biParaTableNesting )
	    {
	    SDEB("#######");
	    LLDEB(tableNesting,bi->biParaTableNesting);
	    rval= -1;
	    }

	stroff= 0;
	tp= bi->biParaParticules;
	for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	    {
	    if  ( tp->tpStroff > docParaStrlen( bi )	||
		  tp->tpStroff != stroff		)
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLLLDEB(part,tp->tpStroff,stroff,docParaStrlen(bi));
		rval= -1;
		}

	    stroff= tp->tpStroff+ tp->tpStrlen;
	    if  ( stroff < 0			||
		  stroff >  docParaStrlen( bi )	)
		{
		appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
		LLLLDEB(part,stroff,tp->tpStrlen,docParaStrlen(bi));
		rval= -1;
		}
	    }
	if  ( stroff != docParaStrlen( bi ) )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(bi->biLevel) );
	    LLDEB(stroff,docParaStrlen(bi));
	    rval= -1;
	    }
	}
    else{
	if  ( bi->biChildCount < 1 )
	    { SLDEB("#######",bi->biChildCount);	}
	}

    if  ( bi->biLevel == DOClevROW )
	{
	if  ( docIsRowNode( bi )			&&
	      bi->biRowCellCount != bi->biChildCount	)
	    {
	    appDebug( "############## %s:\n", docLevelStr(bi->biLevel) );
	    LLDEB(bi->biRowCellCount,bi->biChildCount);
	    }
	}

    if  ( bi->biChildCount > 0 )
	{
	if  ( docCheckGroupLeft( bi ) )
	    { rval= -1;	}

	if  ( ! bi->biParent )
	    {
	    DocumentPosition	dp;
	    docInitDocumentPosition( &dp );

	    if  ( ! docHeadPosition( &dp, (BufferItem *)bi ) )
		{
		const BufferItem *	paraBi= dp.dpNode;
		int			n= 1;

		for (;;)
		    {
		    if  ( docNumberOfParagraph( paraBi ) != n )
			{ LLDEB(docNumberOfParagraph(paraBi),n); rval= -1; }

		    paraBi= docNextParagraph( (BufferItem *)paraBi );
		    if  ( ! paraBi )
			{ break;	}
		    n++;
		    }
		}
	    }
	}

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	LayoutPosition	lpBelowChild= lpHere;

	if  ( docCheckChild( bi, bi->biChildren[i], i,
				    &lpHere, &lpBelowChild, checkGeometry ) )
	    { rval= -1;	}

	if  ( ! docIsRowNode( bi ) )
	    { lpHere= lpBelowChild;	}

	if  ( docCheckNode( bi->biChildren[i], checkGeometry ) )
	    { rval= -1;	}
	}

    return rval;
    }

int docCheckRootNode(	const BufferItem *	bi,
			int			checkGeometry )
    {
    while( bi->biParent )
	{ bi= bi->biParent;	}

    return docCheckNode( bi, checkGeometry );
    }

static void docListChildren(	int			indent,
				const BufferItem *	bi,
				int			checkGeometry )
    {
    int			i;

    LayoutPosition	lp;

    lp= bi->biTopPosition;

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	docCheckChild( bi, bi->biChildren[i], i, &lp, &lp, checkGeometry );

	docListNode( indent+ 1, bi->biChildren[i], checkGeometry );
	}

    if  ( bi->biChildCount > 0 )
	{ docCheckGroupLeft( bi );	}

    if  ( checkGeometry						&&
	  ! docIsRowNode( bi )					&&
	  ! DOC_SAME_POSITION( &(bi->biBelowPosition), &lp )	)
	{
	appDebug( "############## %s :\n", docLevelStr(bi->biLevel) );

	LLDEB(bi->biBelowPosition.lpPage,lp.lpPage);
	LLDEB(bi->biBelowPosition.lpColumn,lp.lpColumn);
	LLDEB(bi->biBelowPosition.lpPageYTwips,lp.lpPageYTwips);
	}
    }

void docListNode(	int			indent,
			const BufferItem *	bi,
			int			checkGeometry )
    {
    int				i;
    LayoutPosition		lpHere;
    const TextLine *		tl;

    int				stroff;
    const TextParticule *	tp;

    for ( i= 0; i < indent; i++ )
	{ appDebug( "%-*s", IS, "." );	}

    appDebug( "NODE %4d 0x%08lx: %s LEFT= %d PG: %d:%d..%d:%d\n",
			    bi->biNumberInParent,
			    (unsigned long)bi,
			    docLevelStr( bi->biLevel ),
			    bi->biLeftParagraphs,
			    bi->biTopPosition.lpPage,
			    bi->biTopPosition.lpColumn,
			    bi->biBelowPosition.lpPage,
			    bi->biBelowPosition.lpColumn );
    switch( bi->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	    appDebug( "%*s{ %4d children\n", IS* indent+ IS, "",
						bi->biChildCount );

	    docListChildren( indent, bi, checkGeometry );

	    break;

	case DOClevROW:
	    appDebug( "%*s{ %s %4d children %d cells\n", IS* indent+ IS, "",
				docIsRowNode(bi)?"T:":"--",
				bi->biChildCount,
				bi->biRowCellCount );

	    if  ( bi->biRowTableFirst >= 0	||
		  bi->biRowTablePast >= 0	||
		  bi->biRowTableHeaderRow >= 0	)
		{
		appDebug( "%*s  Table %d .. %d Header@%d\n", IS* indent+ IS, "",
						bi->biRowTableFirst,
						bi->biRowTablePast,
						bi->biRowTableHeaderRow );

		if  ( bi->biRowTableFirst >= bi->biParent->biChildCount )
		    {
		    SLLDEB("####",
			    bi->biRowTableFirst,bi->biParent->biChildCount);
		    }
		if  ( bi->biRowTablePast > bi->biParent->biChildCount )
		    {
		    SLLDEB("####",
			    bi->biRowTablePast,bi->biParent->biChildCount);
		    }
		}

	    if  ( docIsRowNode( bi )				&&
		  bi->biChildCount != bi->biRowCellCount	)
		{ SLLDEB("####", bi->biChildCount,bi->biRowCellCount); }

	    docListChildren( indent, bi, checkGeometry );

	    break;

	case DOClevPARA:
	    appDebug( "%*s{ NR= %d TN=%d %d particules, %d lines\n",
						IS* indent+ IS, "",
						docNumberOfParagraph( bi ),
						bi->biParaTableNesting,
						bi->biParaParticuleCount,
						bi->biParaLineCount );

#	    if LIST_PARA_STRING
	    {
	    int		length= docParaStrlen( bi );

	    stroff= 0;
	    while( stroff < length )
		{
		int		l= length- stroff;

		if  ( l > 60 )
		    { l= 60; }

		appDebug( "%*s\"%.*s\"\n",
				IS* indent+ IS, "",
				(int)l, (char *)docParaString( bi, stroff ) );

		stroff += l;
		}
	    }
#	    endif

	    stroff= 0;
	    tp= bi->biParaParticules;
	    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
		{
		int		list= LIST_PARTICULES;
		const char *	label= "PART";

		if  ( tp->tpStroff != stroff	)
		    { list= 1; label= "####";	}

		if  ( list )
		    { docListParticule( IS* indent+ IS, label, i, bi, tp ); }

		stroff= tp->tpStroff+ tp->tpStrlen;
		}

	    if  ( stroff != docParaStrlen( bi ) )
		{
		SLLDEB("####",stroff,docParaStrlen( bi ) );
		if  ( bi->biParaParticuleCount > 0 )
		    {
		    i= bi->biParaParticuleCount- 1;
		    tp= bi->biParaParticules+ i;
		    docListParticule( IS* indent+ IS, "####", i, bi, tp );
		    }
		}

	    tl= bi->biParaLines;
	    lpHere= bi->biTopPosition;
	    for ( i= 0; i < bi->biParaLineCount; tl++, i++ )
		{
		int	listLine= LIST_LINES || ( i == 0 && LIST_FIRST_LINE );

		if  ( docCompareLayoutPositions( &(tl->tlTopPosition),
								&lpHere ) < 0 )
		    {
		    appDebug( "###### Line= %d\n", i );
		    LDEB(i);
		    listLine= 1;
		    }

		lpHere= tl->tlTopPosition;
		lpHere.lpPageYTwips += tl->tlLineStride;

		if  ( docCompareLayoutPositions( &(bi->biBelowPosition),
								&lpHere ) < 0 )
		    {
		    appDebug( "###### Line= %d\n", i );
		    LDEB(i);
		    listLine= 1;
		    }

		if  ( listLine )
		    { docListTextLine( IS* indent+ IS, "LINE", i, bi, tl ); }
		}

	    break;
	case DOClevOUT:
	default:
	    break;
	}

    appDebug( "%*s}\n", IS* indent+ IS, "" );
    
    return;
    }

void docListRootNode(	int			indent,
			const BufferItem *	bi,
			int			checkGeometry )
    {
    while( bi->biParent )
	{ bi= bi->biParent;	}

    docListNode( indent, bi, checkGeometry );

    return;
    }

