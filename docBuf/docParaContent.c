/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docParaParticules.h"
#   include	<docTextParticule.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<bidiTree.h>
#   include	<docParaProperties.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the particule number for a certain string position.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted.		*/
/*									*/
/************************************************************************/

int docFindParticuleOfPosition(	int *				pPart,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    const struct BufferItem *	paraNode= dp->dpNode;
    const int			stroff= dp->dpStroff;

    int				l= 0;
    int				r= paraNode->biParaParticuleCount;
    int				m= ( l+ r )/ 2;

    const TextParticule *	tp= paraNode->biParaParticules+ m;
    const int			paraStrlen= docParaStrlen( paraNode );

    if  ( r <= 0 )
	{ LDEB(r); return -1;	}
    if  ( stroff < 0 || stroff > paraStrlen )
	{ LLDEB(stroff,paraStrlen); return -1;	}

    switch( lastOne )
	{
	case PARAfindPAST:
	    if  ( stroff == paraStrlen )
		{
		if  ( pPart )
		    { *pPart= paraNode->biParaParticuleCount;	}

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
	    /* Binary search for last: tp->tpStroff <= stroff */
	    while( l < m )
		{
		if  ( tp->tpStroff <= stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tp= paraNode->biParaParticules+ m;
		}

	    if  ( stroff < tp->tpStroff )
		{ m--;	}
	    break;

	case PARAfindFIRST:
	    /* Binary search for first: tp->tpStroff+ tp->tpStrlen >= stroff */
	    while( l < m )
		{
		if  ( tp->tpStroff+ tp->tpStrlen < stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tp= paraNode->biParaParticules+ m;
		}

	    if  ( stroff > tp->tpStroff+ tp->tpStrlen )
		{ m++;	}
	    break;

	default:
	    LDEB(lastOne); return -1;
	}

    if  ( m < 0 || m >= paraNode->biParaParticuleCount )
	{
	const int checkGeometry= 0;

	LLLLDEB(stroff,m,lastOne,paraNode->biParaParticuleCount);
	docListNode( (const struct BufferDocument *)0, 0, paraNode, checkGeometry );
	return -1;
	}

    if  ( pPart )
	{ *pPart= m;	}

    if  ( pFlags )
	{ docSetParticuleFlags( pFlags, paraNode, m, stroff );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Intersect a selection (if any) with the paragraph to save.		*/
/*									*/
/*  *pPartFrom is set to the first particule that is relevant to save	*/
/*	the paragraph.							*/
/*  *pPartUpto is set to past the last particule that is relevant to	*/
/*	save the paragraph.						*/
/*  *pStroffFrom is set to the offset that starts the range to save.	*/
/*  *pStroffUpto is set to the offset that ends the range to save.	*/
/*									*/
/************************************************************************/

int docIntersectSelectionWithParagraph(
				    DocumentSelection *		pDsPara,
				    int *			pPartFrom,
				    int *			pPartUpto,
				    int *			pHeadFlags,
				    int *			pTailFlags,
				    const struct BufferItem *	paraNode,
				    const DocumentSelection *	ds )
    {
    int			partFrom= 0;
    int			partUpto= paraNode->biParaParticuleCount;
    int			headFlags= 0;
    int			tailFlags= 0;

    DocumentSelection	dsPara;

    if  ( docHeadPosition( &(dsPara.dsHead), (struct BufferItem *)paraNode ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(dsPara.dsTail), (struct BufferItem *)paraNode ) )
	{ LDEB(1); return -1;	}

    if  ( ds )
	{
	if  ( docComparePositions( &(ds->dsTail), &(dsPara.dsHead) ) < 0 )
	    { return 1;	}

	if  ( docComparePositions( &(dsPara.dsTail), &(ds->dsHead) ) < 0 )
	    { return 1;	}

	if  ( ds->dsHead.dpNode == paraNode )
	    {
	    dsPara.dsHead.dpStroff= ds->dsHead.dpStroff;

	    if  ( docFindParticuleOfPosition( &partFrom, &headFlags,
						&(ds->dsHead), PARAfindLAST ) )
		{ LDEB(dsPara.dsHead.dpStroff); return -1;	}

	    headFlags |= POSflagPARA_FOUND;
	    }
	else{
	    headFlags= POSflagPARA_HEAD|POSflagPART_HEAD;
	    }

	if  ( ds->dsTail.dpNode == paraNode )
	    {
	    dsPara.dsTail.dpStroff= ds->dsTail.dpStroff;

	    if  ( docFindParticuleOfPosition( &partUpto, &tailFlags,
						&(ds->dsTail), PARAfindPAST ) )
		{ LDEB(dsPara.dsTail.dpStroff); return -1;	}

	    tailFlags |= POSflagPARA_FOUND;
	    }
	else{
	    tailFlags= POSflagPARA_TAIL|POSflagPART_TAIL;
	    }
	}
    else{
	headFlags= POSflagPARA_HEAD|POSflagPART_HEAD;
	tailFlags= POSflagPARA_TAIL|POSflagPART_TAIL;
	}

    if  ( pDsPara )
	{
	const int	direction= 1;

	docSetRangeSelection( pDsPara,
			    &(dsPara.dsHead), &(dsPara.dsTail), direction );
	}

    if  ( pHeadFlags )
	{ *pHeadFlags= headFlags;	}
    if  ( pTailFlags )
	{ *pTailFlags= tailFlags;	}

    if  ( pPartFrom )
	{ *pPartFrom= partFrom;	}
    if  ( pPartUpto )
	{ *pPartUpto= partUpto;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the (last) directional run that stroffFrom belongs to		*/
/*									*/
/************************************************************************/

int docParaDelimitDirectionalRun(
				int *				pDirection,
				int *				pStroffUpto,
				int *				pPartUpto,
				const struct BufferItem *	paraNode,
				int				stroffFrom )
    {
    int		paraStrlen= docParaStrlen( paraNode );

    if  ( stroffFrom < 0 )
	{ LDEB(stroffFrom); return -1;	}
    if  ( stroffFrom > paraStrlen )
	{ LLDEB(stroffFrom,paraStrlen); return 1;	}

    if  ( paraNode->biParaBidiRoot )
	{
	int			direction;
	DocumentPosition	dpUpto;
	int			res;

	dpUpto.dpNode= (struct BufferItem *)paraNode;
	dpUpto.dpStroff= stroffFrom;

	res= bidiDelimitDirectionalRun( &direction, &(dpUpto.dpStroff),
					paraNode->biParaBidiRoot, stroffFrom );
	if  ( res < 0 )
	    { LDEB(res); return -1;	}
	if  ( res > 0 )
	    {
	    LLDEB(stroffFrom,paraStrlen);
	    /*
	    return res;
	    */
	    }

	if  ( dpUpto.dpStroff > paraStrlen )
	    {
LLDEB(dpUpto.dpStroff,paraStrlen);
	    dpUpto.dpStroff= paraStrlen;
	    }

	if  ( pPartUpto )
	    {
	    if  ( docFindParticuleOfPosition( pPartUpto, (int *)0,
						&dpUpto, PARAfindPAST ) )
		{ LDEB(dpUpto.dpStroff); return -1;	}
	    }

	if  ( pDirection )
	    { *pDirection= direction;		}
	if  ( pStroffUpto )
	    { *pStroffUpto= dpUpto.dpStroff;	}

	return 0;
	}
    else{
	if  ( pDirection )
	    { *pDirection= paraNode->biParaProperties->ppRToL;		}
	if  ( pStroffUpto )
	    { *pStroffUpto= paraStrlen;				}
	if  ( pPartUpto )
	    { *pPartUpto= paraNode->biParaParticuleCount;	}

	return 0;
	}
    }
