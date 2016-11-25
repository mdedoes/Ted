/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/*  Levels in the tree correspond to the ilvl value of the paragraphs.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Determine the path of number styles and formatting instructions	*/
/*  to a certain level.							*/
/*									*/
/************************************************************************/

int docListGetFormatPath(	int *				startPath,
				int *				formatPath,
				const DocumentListLevel **	pDll,
				int				ilvl,
				const DocumentList *		dl,
				const ListOverride *		lo )
    {
    int				level= 0;
    const DocumentListLevel *	dllFound= (const DocumentListLevel *)0;

    if  ( dl->dlLevelCount < 1			||
	  dl->dlLevelCount > DLmaxLEVELS	)
	{ LLDEB(dl->dlLevelCount,DLmaxLEVELS); return -1; }

    while( level < dl->dlLevelCount )
	{
	const DocumentListLevel *	dll= (const DocumentListLevel *)0;
	const ListOverrideLevel *	lol= (const ListOverrideLevel *)0;

	int				startAt= 0;

	if  ( level < lo->loLevelCount )
	    { lol= &(lo->loLevels[level]);	}
	if  ( level < dl->dlLevelCount )
	    { dll= &(dl->dlLevels[level]);	}

	if  ( dll )
	    { startAt= dll->dllStartAt;	}
	if  ( lol && lol->lolOverrideStartAt )
	    { startAt= lol->lolListLevel.dllStartAt;	}

	if  ( lol && lol->lolOverrideFormat )
	    { dll= &(lol->lolListLevel);	}

	startPath[level]= startAt;
	formatPath[level]= dll->dllNumberStyle;

	if  ( level == ilvl )
	    { dllFound= dll;	}

	level++;
	}

    if  ( ! dllFound )
	{ XDEB(dllFound); return -1; }

    *pDll= dllFound;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect list related information for a paragraph.			*/
/*									*/
/*  1)  Paranoia: check list number.					*/
/*  2)  Get override and number tree.					*/
/*  3)  Link override to list if nexessary.				*/
/*  4)  Paranoia again: Is it an existing list?				*/
/*									*/
/************************************************************************/

int docGetListOfOverride(	ListOverride *			lo,
				const DocumentListTable *	dlt )
    {
    int				li;
    const DocumentList *	dl;

    if  ( lo->loListIndex >= 0 )
	{ return lo->loListIndex;	}

    dl= dlt->dltLists;
    for ( li= 0; li < dlt->dltListCount; dl++, li++ )
	{
	if  ( dl->dlListID == lo->loListID )
	    { lo->loListIndex= li; return li; }
	}

    LDEB(lo->loListID);
    return -1;
    }

int docGetListOfParagraph(	ListOverride **			pLo,
				ListNumberTreeNode **		pRoot,
				DocumentList **			pDl,
				int				ls,
				BufferDocument *		bd )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    ListOverrideTable *		lot= &(dp->dpListOverrideTable);
    DocumentListTable *		dlt= &(dp->dpListTable);

    ListOverride *		lo;
    DocumentList *		dl= (DocumentList *)0;
    ListNumberTreeNode *	root;

    /*  1  */
    if  ( ls < 0				||
	  ls >= lot->lotOverrideCount		||
	  ls >= bd->bdListNumberTreeCount	)
	{
	LLLDEB(ls,lot->lotOverrideCount,bd->bdListNumberTreeCount);
	return -1;
	}

    /*  2  */
    lo= lot->lotOverrides+ ls;
    root= bd->bdListNumberTrees+ ls;

    /*  3,4  */
    if  ( lo->loListIndex < 0 && docGetListOfOverride( lo, dlt ) < 0 )
	{ LLDEB(lo->loListIndex,dlt->dltListCount); return -1; }

    dl= dlt->dltLists+ lo->loListIndex;

    *pLo= lo; *pRoot= root; *pDl= dl;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the list number field at the head of a fresh paragraph.		*/
/*									*/
/************************************************************************/

int docInsertListtextField(		BufferItem *		paraBi,
					BufferDocument *	bd )
    {
    TextParticule *		tp= paraBi->biParaParticules;

    ListNumberTreeNode *	root;
    int				paraNr= docNumberOfParagraph( paraBi );

    if  ( docInsertParaHeadField( paraBi, bd,
				    DOCfkLISTTEXT, tp->tpTextAttributeNumber ) )
	{ LDEB(1); return -1;	}

    if  ( paraBi->biParaListOverride >= bd->bdListNumberTreeCount )
	{
	LLLDEB(paraNr,paraBi->biParaListOverride,bd->bdListNumberTreeCount);
	return -1;
	}

    root= &(bd->bdListNumberTrees[paraBi->biParaListOverride]);

    if  ( docListNumberTreeInsertParagraph( root,
					paraBi->biParaListLevel, paraNr ) )
	{ LDEB(paraNr); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove a paragraph from its list.					*/
/*									*/
/************************************************************************/

int docRemoveParagraphFromList(		BufferItem *		paraBi,
					BufferDocument *	bd )
    {
    int				rval= 0;
    ListNumberTreeNode *	root;
    int				paraNr= docNumberOfParagraph( paraBi );

    if  ( paraBi->biParaListOverride >= bd->bdListNumberTreeCount )
	{
	LLLDEB(paraNr,paraBi->biParaListOverride,bd->bdListNumberTreeCount);
	paraBi->biParaListOverride= 0;
	paraBi->biParaListLevel= 0;
	return -1;
	}

    root= &(bd->bdListNumberTrees[paraBi->biParaListOverride]);

    if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	{ LDEB(paraNr); rval= -1;	}

    paraBi->biParaListOverride= 0;
    paraBi->biParaListLevel= 0;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the first list in the selection.				*/
/*									*/
/************************************************************************/

int docFindListOfSelection(	int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const DocumentSelection *	ds )
    {
    BufferItem *			bi;

    int					ls= -1;
    int					level= -1;
    int					paraNr= -1;
    int					multiList= 0;
    int					multiLevel= 0;

    bi= ds->dsBegin.dpBi;
    while( bi )
	{
	const ParagraphProperties *	pp;

	pp= &(bi->biParaProperties);
	if  ( ls < 0			&&
	      pp->ppListOverride >= 1	)
	    {
	    paraNr= docNumberOfParagraph( bi );
	    ls= pp->ppListOverride;
	    }
	if  ( ls < 0 || pp->ppListOverride != ls )
	    { multiList= 1;	}

	if  ( ls > 0			&&
	      pp->ppListOverride == ls	)
	    {
	    if  ( level < 0 )
		{ level= pp->ppListLevel;	}
	    else{
		if  ( level != pp->ppListLevel )
		    { multiLevel= 1;	}
		}
	    }

	if  ( bi == ds->dsEnd.dpBi )
	    { break;	}

	bi= docNextParagraph( bi );
	if  ( ! bi )
	    { XDEB(bi);	}
	}

    if  ( paraNr < 0 )
	{ return 1;	}

    *pLs= ls;
    *pLevel= level;
    *pMultiList= multiList;
    *pMultiLevel= multiLevel;
    *pParaNr= paraNr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  The ruler of a list was changed: transfer properties to the		*/
/*  paragraphs in the list.						*/
/*									*/
/*  NOTE: The implementation relies on the fact that the root of the	*/
/*	tree is not a paragraph that has to be adjusted.		*/
/*									*/
/************************************************************************/

static int docApplyListRulerLevel(
				const ListNumberTreeNode *	lntn,
				int				level,
				const DocumentList *		dl,
				const ListOverride *		lo,
				BufferItem *			rootBi )
    {
    const DocumentListLevel *	dll= (const DocumentListLevel *)0;
    const ListOverrideLevel *	lol= (const ListOverrideLevel *)0;

    int				rval= 0;

    int				i;
    const ListNumberTreeNode *	child;

    if  ( level < lo->loLevelCount )
	{ lol= &(lo->loLevels[level]);	}
    if  ( level < dl->dlLevelCount )
	{ dll= &(dl->dlLevels[level]);	}

    if  ( lol && lol->lolOverrideFormat )
	{ dll= &(lol->lolListLevel);	}

    child= lntn->lntnChildren;
    for ( i= 0; i < lntn->lntnChildCount; child++, i++ )
	{
	BufferItem *	paraBi;
	int		changed= 0;
	int		tabsChanged= 0;

	const int	pixels= 0;

	if  ( docApplyListRulerLevel( child, level+ 1, dl, lo, rootBi ) )
	    { LLDEB(level,i); rval= -1;	}

	if  ( child->lntnParagraphNumber < 0 )
	    { continue;	}

	paraBi= docGetParagraphByNumber( rootBi, child->lntnParagraphNumber );
	if  ( ! paraBi )
	    { LXDEB(child->lntnParagraphNumber,paraBi); rval= -1; continue; }

	if  ( paraBi->biParaListLevel != level )
	    {
	    LLLDEB(child->lntnParagraphNumber,paraBi->biParaListLevel,level);
	    rval= -1;
	    }

	if  ( ! dll )
	    { XDEB(dll); continue;		}

	if  ( paraBi->biParaFirstIndentTwips != dll->dllFirstIndentTwips )
	    {
	    paraBi->biParaFirstIndentTwips= dll->dllFirstIndentTwips;
	    changed= 1;
	    }

	if  ( paraBi->biParaLeftIndentTwips != dll->dllLeftIndentTwips )
	    {
	    paraBi->biParaLeftIndentTwips= dll->dllLeftIndentTwips;
	    changed= 1;
	    }

	if  ( docCopyTabStopList( &tabsChanged, &(paraBi->biParaTabStopList),
					    &(dll->dllTabStopList), pixels ) )
	    { LDEB(1); return -1;	}

	if  ( tabsChanged )
	    { changed= 1;	}

	if  ( changed )
	    { paraBi->biParaLineCount= 0; }

	}

    return rval;
    }

int docApplyListRuler(	const ListNumberTreeNode *	root,
			const DocumentList *		dl,
			const ListOverride *		lo,
			BufferDocument *		bd )
    { return docApplyListRulerLevel( root, 0, dl, lo, &(bd->bdItem) ); }

