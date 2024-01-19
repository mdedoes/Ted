/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/*  Levels in the tree correspond to the ilvl value of the paragraphs.	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docTreeScanner.h"
#   include	<docScanner.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	"docDocumentNote.h"
#   include	"docSectHeadersFooters.h"
#   include	"docParaNodeProperties.h"
#   include	<docListAdmin.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<docListNumberTreeImpl.h>

#   include	<appDebugon.h>

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

int docGetListOfParagraph(	ListOverride **			pLo,
				DocumentList **			pDl,
				int				listOverride,
				const struct BufferDocument *	bd )
    {
    return docGetListForStyle( pLo, pDl, listOverride, bd->bdProperties->dpListAdmin );
    }

int docGetListLevel(	int *				startPath,
			int *				formatPath,
			ListOverride **			pLo,
			DocumentList **			pDl,
			const ListLevel **		pLl,
			int				listOverride,
			int				listLevel,
			const struct BufferDocument *	bd )
    {
    ListOverride *		lo;
    DocumentList *		dl;
    const ListLevel *		ll;

    if  ( docGetListForStyle( &lo, &dl, listOverride,
					    bd->bdProperties->dpListAdmin ) )
	{ LDEB(listOverride); return -1;	}

    if  ( docListGetFormatPath( startPath, formatPath, &ll,
						listLevel, dl, lo ) )
	{ LDEB(listLevel); return -1;	}

    if  ( pLo )
	{ *pLo= lo;	}
    if  ( pDl )
	{ *pDl= dl;	}
    if  ( pLl )
	{ *pLl= ll;	}

    return 0;
    }

int docGetListLevelOfParagraph(	int *				startPath,
				int *				formatPath,
				ListOverride **			pLo,
				DocumentList **			pDl,
				const ListLevel **		pLl,
				const BufferItem *		paraNode,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    return docGetListLevel( startPath, formatPath, pLo, pDl, pLl, pp->ppListOverride, pp->ppListLevel, bd );
    }

/************************************************************************/
/*									*/
/*  Remove a paragraph from its list.					*/
/*									*/
/************************************************************************/

int docRemoveParagraphFromList(	struct BufferItem *			paraNode,
				struct DocumentTree *			dt,
				const struct BufferDocument *		bd )
    {
    int		rval= 0;
    int		paraNr= docNumberOfParagraph( paraNode );

    int		listOverride;
    int		listLevel;
    int		outlineLevel;

    if  ( docParaNodeRemoveListProperties(
		    &listOverride, &listLevel, &outlineLevel, paraNode, bd ) )
	{ LDEB(1); return -1;	}

    if  ( listOverride > 0 )
	{
	if  ( docListNumberTreesDeleteParagraph( &(dt->dtListNumberTrees),
						    listOverride, paraNr ) )
	    { LDEB(paraNr); rval= -1;	}
	}

    if  ( outlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeDeleteParagraph(
					&(dt->dtOutlineTree), paraNr ) )
	    { LDEB(paraNr); rval= -1;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the first list in the selection.				*/
/*									*/
/************************************************************************/

typedef struct FindList
    {
    int	fl_ls;
    int	fl_level;
    int	fl_paraNr;
    int	fl_multiList;
    int	fl_multiLevel;
    } FindList;

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docFindListEnterNode( struct BufferItem *		node,
				const struct DocumentSelection * ds,
				const struct BufferItem *	bodySectNode,
				void *				through )
    {
    FindList *	fl= (FindList *)through;

    if  ( node->biLevel == DOClevPARA )
	{
	const ParagraphProperties *	pp;

	pp= node->biParaProperties;
	if  ( fl->fl_ls < 0		&&
	      pp->ppListOverride >= 1	)
	    {
	    fl->fl_paraNr= docNumberOfParagraph( node );
	    fl->fl_ls= pp->ppListOverride;
	    }
	if  ( fl->fl_ls < 0 || pp->ppListOverride != fl->fl_ls )
	    { fl->fl_multiList= 1;	}

	if  ( fl->fl_ls > 0			&&
	      pp->ppListOverride == fl->fl_ls	)
	    {
	    if  ( fl->fl_level < 0 )
		{ fl->fl_level= pp->ppListLevel;	}
	    else{
		if  ( fl->fl_level != pp->ppListLevel )
		    { fl->fl_multiLevel= 1;	}
		}
	    }
	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docFindListOfSelection(	int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const struct DocumentSelection * ds,
				struct DocumentTree *		tree,
				struct BufferDocument *		bd )
    {
    const int	flags= 0;
    FindList	fl;

    fl.fl_ls= -1;
    fl.fl_level= -1;
    fl.fl_paraNr= -1;
    fl.fl_multiList= 0;
    fl.fl_multiLevel= 0;

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
		    docFindListEnterNode, (NodeVisitor)0,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, &fl ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( bd, tree,
		    docFindListEnterNode, (NodeVisitor)0,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, &fl ) < 0 )
	    { LDEB(1); return -1;	}
	}

    if  ( fl.fl_paraNr < 0 )
	{ return 1;	}

    *pLs= fl.fl_ls;
    *pLevel= fl.fl_level;
    *pMultiList= fl.fl_multiList;
    *pMultiLevel= fl.fl_multiLevel;
    *pParaNr= fl.fl_paraNr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  The ruler of a list was changed: transfer properties to the		*/
/*  paragraphs in the list.						*/
/*									*/
/************************************************************************/

int docAdaptParagraphToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const struct BufferDocument *	bd )
    {
    int				indentChanged= 0;

    ListOverride *		lo;
    DocumentList *		dl;
    const ListLevel *		ll;

    int * const			startPath= (int *)0;
    int * const			formatPath= (int *)0;

    if  ( docGetListLevelOfParagraph( startPath, formatPath,
			    &lo, &dl, &ll, paraNode, bd ) )
	{ LDEB(1); }
    else{
	if  ( docParaNodeAdaptPropertiesToListLevel( &indentChanged,
							paraNode, ll, bd ) )
	    { LDEB(1);		}
	}

    if  ( pChanged )
	{ *pChanged= indentChanged;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs at a 	*/
/*  certain level of the list.						*/
/*									*/
/************************************************************************/

static int docApplyListRulerLevel(
				const ListNumberTreeNode *	lntn,
				int				level,
				const DocumentList *		dl,
				const ListOverride *		lo,
				const struct BufferDocument *	bd,
				const struct DocumentTree *	dt )
    {
    const ListLevel *		ll= (const ListLevel *)0;
    const ListOverrideLevel *	lol= (const ListOverrideLevel *)0;

    int				rval= 0;

    int				i;

    if  ( level < lo->loLevelCount )
	{ lol= &(lo->loLevels[level]);	}
    if  ( level < dl->dlLevelCount )
	{ ll= &(dl->dlLevels[level]);	}

    if  ( lol && lol->lolOverrideFormat )
	{ ll= &(lol->lolListLevel);	}

    for ( i= 0; i < lntn->lntnChildCount; i++ )
	{
	struct BufferItem *	paraNode;
	int			changed= 0;
	ListNumberTreeNode *	child= lntn->lntnChildren[i];

	if  ( docApplyListRulerLevel( child, level+ 1, dl, lo, bd, dt ) )
	    { LLDEB(level,i); rval= -1;	}

	if  ( child->lntnParaNr < 0 )
	    { continue;	}

	paraNode= docGetParagraphByNumber( dt, child->lntnParaNr );
	if  ( ! paraNode )
	    { LXDEB(child->lntnParaNr,paraNode); rval= -1; continue; }

	if  ( paraNode->biParaProperties->ppListLevel != level )
	    { LLDEB(child->lntnParaNr,level); rval= -1;	}

	if  ( ! ll )
	    { XDEB(ll); continue;		}

	if  ( docParaNodeAdaptPropertiesToListLevel( &changed,
							paraNode, ll, bd ) )
	    { LDEB(1); return -1;	}

	if  ( changed )
	    { docInvalidateParagraphLayout( paraNode ); }
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs in one of	*/
/*  the document trees.							*/
/*									*/
/************************************************************************/

static int docApplyListRulerToTree(	const DocumentList *		dl,
					const ListOverride *		lo,
					const struct BufferDocument *	bd,
					const struct DocumentTree *	dt )
    {
    if  ( ! dt->dtRoot						||
	  dt->dtListNumberTrees.lntTreeCount <= lo->loIndex	)
	{ return 0;	}

    return docApplyListRulerLevel( dt->dtListNumberTrees.lntTrees+ lo->loIndex,
							    0, dl, lo, bd, dt );
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs. We only	*/
/*  expect paragraphs in the document body, but it does no harm to	*/
/*  cope with them in the other trees as well.				*/
/*									*/
/************************************************************************/

int docApplyListRuler(	const DocumentList *		dl,
			const ListOverride *		lo,
			struct BufferDocument *		bd )
    {
    int			i;
    int			rval= 0;
    struct BufferItem *	bodyNode;

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdBody) ) )
	{ LDEB(1); rval= -1;	}

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdFtnsep) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdFtnsepc) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdFtncn) ) )
	{ LDEB(1); rval= -1;	}

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdAftnsep) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdAftnsepc) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdAftncn) ) )
	{ LDEB(1); rval= -1;	}

    bodyNode= bd->bdBody.dtRoot;
    for ( i= 0; i < bodyNode->biChildCount; i++ )
	{
	struct BufferItem *	sectNode= bodyNode->biChildren[i];
	SectHeadersFooters *	shf= sectNode->biSectHeadersFooters;

	if  ( shf )
	    {
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfFirstPageHeader) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLeftPageHeader) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfRightPageHeader) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLastPageHeader) ) )
		{ LDEB(1); rval= -1;	}

	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfFirstPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLeftPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfRightPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLastPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    }
	}

    for ( i= 0; i < bd->bdNotesList.nlNoteCount; i++ )
	{
	DocumentNote *	dn= bd->bdNotesList.nlNotes[i];

	if  ( ! dn )
	    { continue;	}

	if  ( docApplyListRulerToTree( dl, lo, bd, &(dn->dnDocumentTree) ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

