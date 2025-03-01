/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 *  Is this paragraph the first one in a table of contents?
 *  We assume that a table of contents does not span table cells 
 *  or sections. So it is the first if it is the first child in its 
 *  parent, if the previous sibling is not a paragraph, or the 
 *  sibling is not in a table of contents or at a shallower level.
 *
 *  If this is called at a position inside the paragraph, that is
 *  because the paragraph is split over pages. To correctly balance 
 *  marked content markers inside the page, we split TOCs that span 
 *  multiple pages.
 *  The same applies if the previous paragraph is on a different 
 *  page.
 */
static int docPsPrintParagraphTocLevelsToOpen(	
			const BufferItem *	paraNode,
			int			firstLine )
    {
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( paraNode->biParaTocLevel <= 0 )
	{ return 0;	}

    if  ( paraNode->biNumberInParent == 0 || firstLine > 0 )
	{ return paraNode->biParaTocLevel;	}
    else{
	const BufferItem *	prevNode= parentNode->biChildren[paraNode->biNumberInParent-1];

	if  ( prevNode->biLevel != DOClevPARA 						||
	      prevNode->biBelowPosition.lpPage != paraNode->biTopPosition.lpPage	)
	    { return paraNode->biParaTocLevel;	}
	else{
	    if  ( paraNode->biParaTocLevel > prevNode->biParaTocLevel )
		{ return paraNode->biParaTocLevel- prevNode->biParaTocLevel;	}
	    else{ return 0;							}
	    }
	}
    }

/**
 *  Is this paragraph the last one in a table of contents?
 *  We assume that a table of contents does not span table cells 
 *  or sections. So it is the last if it is the last child in its 
 *  parent, if the next sibling is not a paragraph, or the 
 *  sibling is not in a table of contents or at a shallower level.
 *
 *  If this is called at a position inside the paragraph, that is
 *  because the paragraph is split over pages. To correctly balance 
 *  marked content markers inside the page, we split TOCs that span 
 *  multiple pages.
 *  The same applies if the next paragraph is on a different 
 *  page.
 */
static int docPsPrintParagraphTocLevelsToClose(	
			const BufferItem *	paraNode,
			int			lastLine )
    {
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( paraNode->biParaTocLevel <= 0 )
	{ return 0;	}

    if  ( paraNode->biNumberInParent == parentNode->biChildCount- 1 || lastLine < paraNode->biParaLineCount- 1 )
	{ return paraNode->biParaTocLevel;	}
    else{
	const BufferItem *	nextNode= parentNode->biChildren[paraNode->biNumberInParent+1];

	if  ( nextNode->biLevel != DOClevPARA						||
	      nextNode->biTopPosition.lpPage != paraNode->biBelowPosition.lpPage	)
	    { return paraNode->biParaTocLevel;	}
	else{
	    if  ( paraNode->biParaTocLevel > nextNode->biParaTocLevel )
		{ return paraNode->biParaTocLevel- nextNode->biParaTocLevel;	}
	    else{ return 0;							}
	    }
	}
    }

int docPsPrintOpenParaTocLevels(	PrintingState *			ps,
					const struct BufferItem *	paraNode,
					int				firstLine )
    {
    int		level;
    int		tocLevelsToOpen= docPsPrintParagraphTocLevelsToOpen( paraNode, firstLine );

    for ( level= 0; level < tocLevelsToOpen; level++ )
	{
	if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTOC, (MemoryBuffer *)0 ) )
	    { LSDEB(paraNode->biParaTocLevel,STRUCTtypeTOC); return -1;	}
	}

    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTOCI, (MemoryBuffer *)0 ) )
	{ LLDEB(paraNode->biLevel,paraNode->biParaTocLevel); return -1;	}

    return 0;
    }

int docPsPrintCloseParaTocLevels(	PrintingState *			ps,
					const struct BufferItem *	paraNode,
					int				lastLine )
    {
    int		level;
    int		tocLevelsToClose= docPsPrintParagraphTocLevelsToClose( paraNode, lastLine );

    if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeTOCI ) )
	{ LLDEB(paraNode->biLevel,paraNode->biParaTocLevel); return -1;	}

    for ( level= 0; level < tocLevelsToClose; level++ )
	{
	if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeTOC ) )
	    { LSDEB(paraNode->biParaTocLevel,STRUCTtypeTOC); return -1;	}
	}

    return 0;
    }

