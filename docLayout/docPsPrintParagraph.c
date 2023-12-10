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
#   include	<docTextLine.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

int docParagraphIsListItem(	const BufferItem *	paraNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    return pp->ppListOverride > 0 && docPsListNodeHasSameListNeighbour( paraNode );
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

/* See Annex H.8.3 in the PDF 2020 Spec about hierarchical lists */
const char * docPsParagraphNodeStartMark(
	    const PrintingState *	ps,
	    const BufferItem *		paraNode,
	    int				firstLine,
	    int *			pListLevelsToOpen )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( docParagraphIsListItem( paraNode ) )
	{
	if  ( firstLine == 0 )
	    {
	    const int	prev= paraNode->biNumberInParent- 1;

	    if  ( prev >= 0 )
		{
		*pListLevelsToOpen= docPsListNodeDeeper( paraNode, parentNode->biChildren[prev] );
		}
	    else{ *pListLevelsToOpen= pp->ppListLevel+ 1;	}
	    }
	else{
	    const TextLine *	thisTl= paraNode->biParaLines+ firstLine;
	    const TextLine *	prevTl= paraNode->biParaLines+ firstLine- 1;

	    if  ( prevTl->tlTopPosition.lpPage != thisTl->tlTopPosition.lpPage )
		{ *pListLevelsToOpen= pp->ppListLevel+ 1;	}
	    else{ *pListLevelsToOpen= 0;			}
	    }

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
	    int				lastLine,
	    int *			pListLevelsToClose )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    /* lastLine refers to the last line. It must be inside the paragraph */
    if  ( lastLine >= paraNode->biParaLineCount )
	{ SLLDEB("####",lastLine,paraNode->biParaLineCount);	}

    if  ( docParagraphIsListItem( paraNode ) )
	{
	const int	next= paraNode->biNumberInParent+ 1;
	int		nextDeeper= 0;

	if  ( lastLine >= paraNode->biParaLineCount- 1 )
	    {
	    if  ( next < paraNode->biParent->biChildCount )
		{
		*pListLevelsToClose= docPsListNodeShallower( paraNode, parentNode->biChildren[next] );
		/* If the next paragraph is at a deeper level, we want it to go to a nested list:
		   So, do not close the item. */
		if  ( ! *pListLevelsToClose )
		    { nextDeeper= docPsListNodeDeeper( parentNode->biChildren[next], paraNode ); }
		}
	    else{ *pListLevelsToClose= pp->ppListLevel+ 1;	}
	    }
	else{
	    const TextLine *	thisTl= paraNode->biParaLines+ lastLine;
	    const TextLine *	nextTl= paraNode->biParaLines+ lastLine+ 1;

	    if  ( nextTl->tlTopPosition.lpPage != thisTl->tlTopPosition.lpPage )
		{ *pListLevelsToClose= pp->ppListLevel+ 1;	}
	    else{ *pListLevelsToClose= 0;			}
	    }

	if  ( nextDeeper )
	    { return (char *)0;		}
	else{ return STRUCTtypeLI;	}
	}
    else{
	return docPsNoListParagraphMark( pp->ppOutlineLevel );
	}
    }
