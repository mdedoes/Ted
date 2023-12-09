/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Only handle members of an RTF list as a PDF/HTML list item if it has 
 * neighbour in the same list.
 */
static int docPsListNodeHasListNeighbour(	const BufferItem *	paraNode )
    {
    const BufferItem *		parentNode= paraNode->biParent;
    int				prev= paraNode->biNumberInParent- 1;
    int				next= paraNode->biNumberInParent+ 1;

    int				listOverride= paraNode->biParaProperties->ppListOverride;

    if  ( prev >= 0 )
	{
	const BufferItem *	prevNode= parentNode->biChildren[prev];

	if  ( prevNode->biLevel == DOClevPARA					&&
	      prevNode->biParaProperties->ppListOverride == listOverride	&&
	      prevNode->biBelowPosition.lpPage == paraNode->biTopPosition.lpPage )
	    { return 1;	}
	}

    if  ( next < parentNode->biChildCount )
	{
	const BufferItem *	nextNode= parentNode->biChildren[next];

	if  ( nextNode->biLevel == DOClevPARA					&&
	      nextNode->biParaProperties->ppListOverride == listOverride	&&
	      nextNode->biTopPosition.lpPage == paraNode->biBelowPosition.lpPage )
	    { return 1;	}
	}

    return 0;
    }

static const char * docPsNoListParagraphMark( int outlineLevel )
    {
    switch( outlineLevel )
	{
	case 0: return STRUCTtypeH1;
	case 1: return STRUCTtypeH2;
	case 2: return STRUCTtypeH3;
	case 3: return STRUCTtypeH4;
	case 4: return STRUCTtypeH5;
	case 5: return STRUCTtypeH6;
	default: return STRUCTtypeP;
	}
    }

static int docPsListNodeDeeper(
	    const BufferItem *		paraNode,
	    const BufferItem *		prevNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	prevPp;

    /* A nested table row: split the list */
    if  ( prevNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    /* A page split: split the list */
    if  ( prevNode->biBelowPosition.lpPage != paraNode->biTopPosition.lpPage )
	{ return pp->ppListLevel+ 1;	}

    prevPp= prevNode->biParaProperties;
    if  ( prevPp->ppListOverride != pp->ppListOverride )
	{ return pp->ppListLevel+ 1;	}

    if  ( pp->ppListLevel > prevPp->ppListLevel )
	{ return pp->ppListLevel- prevPp->ppListLevel;	}

    return 0;
    }

static int docPsListNodeShallower(
	    const BufferItem *		paraNode,
	    const BufferItem *		nextNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	nextPp;

    /* A nested table row: split the list */
    if  ( nextNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    /* A page split: split the list */
    if  ( nextNode->biTopPosition.lpPage != paraNode->biBelowPosition.lpPage )
	{ return pp->ppListLevel+ 1;	}

    nextPp= nextNode->biParaProperties;
    if  ( nextPp->ppListOverride != pp->ppListOverride )
	{ return pp->ppListLevel+ 1;	}

    if  ( pp->ppListLevel > nextPp->ppListLevel )
	{ return pp->ppListLevel- nextPp->ppListLevel;	}

    return 0;
    }

int docParagraphIsListItem(	const BufferItem *	paraNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    return pp->ppListOverride > 0 && docPsListNodeHasListNeighbour( paraNode );
    }


/* See Annex H.8.3 in the PDF 2020 Spec about hierarchical lists */
const char * docPsParagraphNodeStartMark(
	    const PrintingState *	ps,
	    const BufferItem *		paraNode,
	    int *			pListLevelsToOpen )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( docParagraphIsListItem( paraNode ) )
	{
	const int	prev= paraNode->biNumberInParent- 1;

	if  ( prev >= 0 )
	    {
	    *pListLevelsToOpen= docPsListNodeDeeper( paraNode, parentNode->biChildren[prev] );
	    }
	else{ *pListLevelsToOpen= pp->ppListLevel+ 1;	}

	return STRUCTtypeLI;
	}
    else{
	return docPsNoListParagraphMark( pp->ppOutlineLevel );
	}
    }

/* See Annex H.8.3 in the PDF 2020 Spec about hierarchical lists */
const char * docPsParagraphNodeEndMark(
	    const PrintingState *	ps,
	    const BufferItem *		paraNode,
	    int *			pListLevelsToClose )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( docParagraphIsListItem( paraNode ) )
	{
	const int	next= paraNode->biNumberInParent+ 1;
	int		nextDeeper= 0;

	if  ( next < paraNode->biParent->biChildCount )
	    {
	    *pListLevelsToClose= docPsListNodeShallower( paraNode, parentNode->biChildren[next] );
	    /* If the next paragraph is at a deeper level, we want it to go to a nested list:
	       So, do not close the item. */
	    if  ( ! *pListLevelsToClose )
		{ nextDeeper= docPsListNodeDeeper( parentNode->biChildren[next], paraNode ); }
	    }
	else{ *pListLevelsToClose= pp->ppListLevel+ 1;	}

	if  ( nextDeeper )
	    { return (char *)0;		}
	else{ return STRUCTtypeLI;	}
	}
    else{
	return docPsNoListParagraphMark( pp->ppOutlineLevel );
	}
    }
