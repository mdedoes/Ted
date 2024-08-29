/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<string.h>

#   include	"docPsPrintImpl.h"
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	<psPrint.h>
#   include	<docTextLine.h>
#   include	<docParaParticules.h>

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
static const char * docPsParagraphNodeStartMark(
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
static const char * docPsParagraphNodeEndMark(
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

/**
 *  Start printing a slice of a paragraph. Currently, we make 
 *  a /Div content item. Once we have the structural hierarchy 
 *  in place, consider using /NonStruct: The paragraph that holds 
 *  the lines determines the document structure.
 */
int docPsPrintStartLines( void *			vps,
			struct DrawingContext *		dc,
			const BufferItem *		paraNode,
			int				firstLine,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;
    int			listLevelsToOpen= 0;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	const char *	mark;

	mark= docPsParagraphNodeStartMark( ps, paraNode, firstLine, &listLevelsToOpen );
	if  ( mark )
	    {
	    if  ( listLevelsToOpen > 0	)
		{
		if  ( docPsPrintOpenListLevels( ps, dc, paraNode, listLevelsToOpen ) )
		    { LDEB(listLevelsToOpen); return -1;	}
		}
	    else{
		if  ( paraNode->biParaTocLevel > 0 && docPsPrintOpenParaTocLevels( ps, paraNode, firstLine ) )
		    { LDEB(paraNode->biParaTocLevel); return -1;	}
		}

	    if  ( docPsPrintBeginMarkedGroup( ps, mark, (MemoryBuffer *)0 ) )
		{ LSDEB(paraNode->biLevel,mark); return -1;	}
	    }
	}

    return 0;
    }

/**
 *  Finish printing a slice of a paragraph. If we are in an open 
 *  span, close the span.
 */
int docPsPrintFinishLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			int				lastLine,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	int		listLevelsToClose= 0;
	const char *	mark;

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(paraNode->biLevel); return -1;	}

	mark= docPsParagraphNodeEndMark( ps, paraNode, lastLine, &listLevelsToClose );

	if  ( mark )
	    {
	    if  ( ps->psCurrentStructItem		&&
		  ! strcmp( ps->psCurrentStructItem->siStructureType, STRUCTtypeLBODY ) )
		{
		if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeLBODY ) )
		    { SSDEB(docLevelStr(paraNode->biLevel),STRUCTtypeLBODY); return -1;	}
		}

	    if  ( docPsPrintEndMarkedGroup( ps, mark ) )
		{ SSDEB(docLevelStr(paraNode->biLevel),mark); return -1;	}
	    }

	if  ( listLevelsToClose > 0 )
	    {
	    if  ( docPsPrintCloseListLevels( ps, dc, paraNode, listLevelsToClose )	)
		{ LSDEB(listLevelsToClose,mark); return -1;	}
	    }
	else{
	    if  ( paraNode->biParaTocLevel > 0 && docPsPrintCloseParaTocLevels( ps, paraNode, lastLine ) )
		{ LDEB(paraNode->biParaTocLevel); return -1;}
	    }
	}

    return 0;
    }

