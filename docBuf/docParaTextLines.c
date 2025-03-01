/************************************************************************/
/*									*/
/*  Manage lines in a paragraph.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docParaParticules.h"
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextLine.h>
#   include	<docTreeType.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Remember about a line in a Text Item.				*/
/*									*/
/************************************************************************/

TextLine * docInsertTextLine(	struct BufferItem *	node,
				int		line )
    {
    TextLine *		tl;
    int			newSize;

    if  ( node->biParaLineCount % 10 )
	{ newSize= node->biParaLineCount+  1; }
    else{ newSize= node->biParaLineCount+ 11; }

    newSize *= sizeof(TextLine);

    tl= (TextLine *)realloc( node->biParaLines, newSize );
    if  ( ! tl )
	{ LXDEB(node->biParaLineCount,tl); return (TextLine *)0; }
    node->biParaLines= tl;

    if  ( line == -1 )
	{ line= node->biParaLineCount; }
    else{
	int		i;

	for ( i= node->biParaLineCount; i > line; i-- )
	    { tl[i]= tl[i-1];	}
	}

    tl += line;

    docInitTextLine( tl );

    node->biParaLineCount++;
    return tl;
    }

/************************************************************************/

void docSetLineFlags(		int *				pFlags,
				const struct BufferItem *	paraNode,
				int				stroff,
				const struct TextLine *		tl )
    {
    const int		paraStrlen= docParaStrlen( paraNode );

    *pFlags= 0;

    if  ( stroff == tl->tlStroff )
	{ *pFlags |= POSflagLINE_HEAD; }
    if  ( stroff == tl->tlStroff+ tl->tlStrlen )
	{ *pFlags |= POSflagLINE_TAIL; }

    if  ( stroff == 0 )
	{ *pFlags |= POSflagPARA_HEAD; }
    if  ( stroff == paraStrlen )
	{ *pFlags |= POSflagPARA_TAIL; }

    return;
    }

/************************************************************************/
/*									*/
/*  Find the line number for a certain string offset in the paragraph.	*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted. We just	*/
/*	fail because there are no lines. We do not crash however.	*/
/*									*/
/************************************************************************/

int docFindLineOfPosition(	int *				pLine,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    const struct BufferItem *	paraNode= dp->dpNode;
    const int		stroff= dp->dpStroff;

    int			l= 0;
    int			r= paraNode->biParaLineCount;
    int			m= ( l+ r )/ 2;

    const TextLine *	tl= paraNode->biParaLines+ m;
    const int		paraStrlen= docParaStrlen( paraNode );

    if  ( r <= 0 )
	{
	SLDEB(docTreeTypeStr(paraNode->biTreeType),paraNode->biParaLineCount);
	return -1;
	}
    if  ( stroff < 0 || stroff > paraStrlen )
	{ LDEB(stroff); return -1;	}

    switch( lastOne )
	{
	case PARAfindPAST:
	    if  ( stroff == paraStrlen )
		{
		*pLine= paraNode->biParaLineCount;
		if  ( pFlags )
		    {
		    *pFlags= POSflagPARA_TAIL;

		    if  ( stroff == 0 )
			{ *pFlags |= POSflagPARA_HEAD; }
		    }
		return 0;
		}

	    /*FALLTHROUGH*/
	case PARAfindLAST:
	    /* Binary search for last: tl->tlStroff <= stroff */
	    while( l < m )
		{
		if  ( tl->tlStroff <= stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tl= paraNode->biParaLines+ m;
		}

	    if  ( stroff < tl->tlStroff )
		{ m--;	}
	    break;

	case PARAfindFIRST:
	    /* Binary search for first: tl->tlStroff+ tl->tlStrlen >= stroff */
	    while( l < m )
		{
		if  ( tl->tlStroff+ tl->tlStrlen < stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tl= paraNode->biParaLines+ m;
		}

	    if  ( stroff > tl->tlStroff+ tl->tlStrlen )
		{ m++;	}
	    break;

	default:
	    LDEB(lastOne); return -1;
	}

    if  ( m < 0 || m >= paraNode->biParaLineCount )
	{
	const int checkGeometry= 0;

	LLLLDEB(stroff,m,lastOne,paraNode->biParaLineCount);
	docListNode( (const struct BufferDocument *)0, 0, paraNode, checkGeometry );
	return -1;
	}

    *pLine= m;

    if  ( pFlags )
	{
	tl= paraNode->biParaLines+ m;
	docSetLineFlags( pFlags, paraNode, stroff, tl );
	}

    return 0;
    }

/************************************************************************/

int docGetLineOfPosition(	int *				pLine,
				const DocumentPosition *	dp,
				int				positionFlags )
    {
    if  ( positionFlags & POSflagLINE_TAIL )
	{
	if  ( docFindLineOfPosition( pLine, (int *)0, dp, PARAfindFIRST ) )
	    { LDEB(dp->dpStroff); return -1;	}
	}
    else{
	if  ( docFindLineOfPosition( pLine, (int *)0, dp, PARAfindLAST ) )
	    { LDEB(dp->dpStroff); return -1;	}
	}

    return 0;
    }
