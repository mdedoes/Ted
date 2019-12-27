/************************************************************************/
/*									*/
/*  Find utility routines.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docTreeNode.h"
#   include	"docFind.h"
#   include	"docNodeTree.h"
#   include	<docTreeType.h>
#   include	"docDocumentNote.h"
#   include	"docSelect.h"
#   include	<docDocumentProperties.h>
#   include	"docFields.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Look for something in a document.					*/
/*									*/
/*  The 'Find Next' action scans the document in the following order:	*/
/*									*/
/*  1)  The body of the document.					*/
/*  2)  The external items of the sections of the document in the order	*/
/*	of the sections. [No attempt is made to start with a particular	*/
/*	section.]							*/
/*  3)  Per section, the headers and footers are tried in the order of	*/
/*	the 'DOC_FindHeaderFooters[]' array.				*/
/*  4)  Then the foot/endnotes are tried in the order in which they	*/
/*	appear in the internal administration of the document.		*/
/*									*/
/*  A search wraps (And will wrap indefinitely) when the tree in which	*/
/*  it started is left for the second time.				*/
/*									*/
/************************************************************************/

typedef int (*STEP_TREE)(	DocumentPosition *	dp,
				struct DocumentTree **	pTree,
				int *			pPage,
				int *			pColumn,
				int			samePhase,
				const struct BufferDocument *	bd );

typedef struct BufferItem * (*STEP_PARA)(	struct BufferItem *	node );

typedef int (*START_PARA)(	DocumentPosition *	dp,
				struct BufferItem *		node );

typedef struct DocumentFindJob
    {
    DocumentSelection		dfjStartSelection;
    DocumentPosition		dfjCurrentPosition;

    int				dfjTreePage;
    int				dfjTreeColumn;

    int				dfjWrapCount;
    int				dfjReverse;

    STEP_TREE			dfjStepTree;
    STEP_PARA			dfjStepPara;
    START_PARA			dfjStartPara;
    PARA_FIND_STRING		dfjFindString;

    struct DocumentTree *	dfjTree;
    struct BufferDocument *	dfjDocument;
    void *			dfjThrough;
    } DocumentFindJob;

/**/

static int DOC_FindDocNoteItems[]=
    {
    DOCinFTNSEP,	DOCinFTNSEPC,	DOCinFTNCN,
    DOCinAFTNSEP,	DOCinAFTNSEPC,	DOCinAFTNCN,
    };

static int DOC_FindDocNoteItemCount= 
			sizeof(DOC_FindDocNoteItems)/sizeof(int);

/************************************************************************/

static int docFindGetHeaderFooterIndex(	int	treeType )
    {
    int		i;

    for ( i= 0; i < DOC_HeaderFooterTypeCount; i++ )
	{
	if  ( DOC_HeaderFooterTypes[i] == treeType )
	    { return i;	}
	}

    LDEB(treeType); return -1;
    }

static int docFindGetDocNoteItemIndex(	int	treeType )
    {
    int		i;

    for ( i= 0; i < DOC_FindDocNoteItemCount; i++ )
	{
	if  ( DOC_FindDocNoteItems[i] == treeType )
	    { return i;	}
	}

    LDEB(treeType); return -1;
    }

/************************************************************************/

static struct DocumentTree * docFindGetHeaderFooter(
				const struct BufferItem *	bodySectNode,
				const DocumentProperties *	dp,
				int				hf )
    {
    unsigned char		applies= 1;
    struct DocumentTree *	tree;

    tree= docSectionHeaderFooter( bodySectNode, &applies, dp,
						DOC_HeaderFooterTypes[hf] );

    if  ( ! tree )
	{ XDEB(tree); return (struct DocumentTree *)0;	}
    if  ( ! applies )
	{ return (struct DocumentTree *)0;	}

    return tree;
    }

/************************************************************************/
/*									*/
/*  Initialize a find job.						*/
/*									*/
/*  NOTE that a document without a body is not always handeled		*/
/*	correctly. solve this in the caller by making an empty body. It	*/
/*	is not worth the hassle to find a work around here.		*/
/*									*/
/************************************************************************/

static int docFindInitFindJob(	DocumentFindJob *		dfj,
				int				reverse,
				const DocumentPosition *	dpFrom,
				struct BufferDocument *		bd )
    {
    dfj->dfjTree= (struct DocumentTree *)0;
    dfj->dfjTreePage= -1;
    dfj->dfjTreeColumn= -1;
    dfj->dfjReverse= reverse;

    if  ( dpFrom->dpNode )
	{
	dfj->dfjCurrentPosition= *dpFrom;
	if  ( reverse )
	    { docPrevPosition( &(dfj->dfjCurrentPosition) );	}

	if  ( dpFrom->dpNode->biTreeType != DOCinBODY )
	    {
	    struct DocumentTree *	tree;
	    struct BufferItem *	bodySectNode;

	    if  ( docGetTreeOfNode( &tree, &bodySectNode, bd, dpFrom->dpNode ) )
		{ LDEB(1); return -1;	}

	    dfj->dfjTree= tree;
	    dfj->dfjTreePage= tree->dtPageSelectedUpon;
	    dfj->dfjTreeColumn= tree->dtColumnSelectedIn;
	    }
	}
    else{
	struct BufferItem *	bodyNode= bd->bdBody.dtRoot;

	if  ( reverse )
	    {
	    if  ( docTailPosition( &(dfj->dfjCurrentPosition), bodyNode ) )
		{ LDEB(1); return 1;	}
	    }
	else{
	    if  ( docHeadPosition( &(dfj->dfjCurrentPosition), bodyNode ) )
		{ LDEB(1); return 1;	}
	    }

	dfj->dfjTree= &(bd->bdBody);
	}

    docInitDocumentSelection( &(dfj->dfjStartSelection) );
    docSetIBarSelection( &(dfj->dfjStartSelection),
					    &(dfj->dfjCurrentPosition) );

    dfj->dfjWrapCount= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the next header or footer to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextHeaderFooter(	DocumentPosition *	dpNew,
					struct DocumentTree **	pTree,
					int *			pTreePage,
					int *			pTreeColumn,
					const struct BufferDocument *	bd,
					int			sectFrom,
					int			hfFrom )
    {
    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;
    const DocumentProperties *	dp= bd->bdProperties;

    int				sect;

    for ( sect= sectFrom; sect < bodyNode->biChildCount; sect++ )
	{
	struct BufferItem *		bodySectNode;
	int			hf;
	struct DocumentTree *		tree;

	bodySectNode= bodyNode->biChildren[sect];

	for ( hf= hfFrom; hf < DOC_HeaderFooterTypeCount; hf++ )
	    {
	    int			page;
	    int			usedInDoc;

	    tree= docFindGetHeaderFooter( bodySectNode, dp, hf );
	    if  ( ! tree || ! tree->dtRoot )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &usedInDoc, bodySectNode,
					    DOC_HeaderFooterTypes[hf], bd );

	    if  ( page < 0 || ! usedInDoc )
		{ continue;	}

	    if  ( docHeadPosition( dpNew, tree->dtRoot ) )
		{
		SDEB(docTreeTypeStr(DOC_HeaderFooterTypes[hf]));
		continue;
		}

	    *pTree= tree;
	    *pTreePage= page;
	    *pTreeColumn= 0; /* irrelevant */
	    return 0;
	    }

	hfFrom= 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note to look in for a find action.			*/
/*									*/
/************************************************************************/

static int docFindNextNote(	DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				const struct BufferDocument *	bd,
				struct DocumentField *		dfNote )
    {
    DocumentNote *	dn;

    dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 );
    while( dfNote )
	{
	if  ( dn->dnDocumentTree.dtRoot )
	    {
	    if  ( docHeadPosition( dp, dn->dnDocumentTree.dtRoot ) )
		{ LDEB(1); return -1;	}

	    *pTree= &(dn->dnDocumentTree);
	    return 0;
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 );
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note item to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextSeparator( DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				int *				pTreePage,
				int *				pTreeColumn,
				const struct BufferDocument *	bd,
				int				sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep < DOC_FindDocNoteItemCount; sep++ )
	{
	struct DocumentTree *		tree;
	DocumentNote *		dn;
	int			page= -1;
	int			column= -1;

	tree= docDocumentNoteSeparator( (struct BufferDocument *)bd,
						DOC_FindDocNoteItems[sep] );
	if  ( ! tree || ! tree->dtRoot )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		if  ( dn->dnReferringColumn < 0 )
		    { continue;	}
		page= dn->dnReferringPage;
		column= dn->dnReferringColumn;
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    case DOCinAFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
		    { continue;	}
		page= tree->dtRoot->biTopPosition.lpPage;
		column= tree->dtRoot->biTopPosition.lpColumn;
		break;

	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docHeadPosition( dp, tree->dtRoot ) )
	    { LDEB(1); return -1;	}

	*pTree= tree;
	*pTreePage= page;
	*pTreeColumn= column;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next tree to look in in a find action.			*/
/*									*/
/*  It is the responsibility of the caller to avoid loops. This routine	*/
/*  just returns the next tree indefinitely. (It wraps from the end to	*/
/*  the beginning.)							*/
/*									*/
/************************************************************************/

static int docFindNextNextTree(	DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				int *				pPage,
				int *				pColumn,
				int				samePhase,
				const struct BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    struct DocumentField *	dfNote= (struct DocumentField *)0;
    int				sectFrom;
    int				sepFrom;

    const struct BufferItem *		selSectNode;
    const SelectionScope *	ss;

    selSectNode= docGetSectNode( dp->dpNode );
    if  ( ! selSectNode )
	{ XDEB(selSectNode); return -1;	}
    ss= &(selSectNode->biSectSelectionScope);

    phaseTo= 0;
    sectFrom= 0; hfFrom= 0; sepFrom= 0;
    switch( ss->ssTreeType )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 1;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssOwnerSectNr;
	    hfFrom= docFindGetHeaderFooterIndex( selSectNode->biTreeType );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectNode->biTreeType,hfFrom); return -1;	}
	    hfFrom++;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    dfNote= docGetFieldByNumber( bd, ss->ssOwnerNumber );

	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectNode->biTreeType );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectNode->biTreeType,sepFrom); return -1; }
	    sepFrom++;

	    break;

	default:
	    LDEB(ss->ssTreeType);
	    LDEB(selSectNode->biTreeType);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 1 )
	{
	res= docFindNextHeaderFooter( dp, pTree,
				    pPage, pColumn, bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 2 )
	{
	res= docFindNextNote( dp, pTree, bd, dfNote );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 3 )
	{
	res= docFindNextSeparator( dp, pTree, pPage, pColumn, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docDocumentHead( dp, (struct BufferDocument *)bd ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Prepare for the next 'Find Prev' attempt.				*/
/*									*/
/************************************************************************/

static int docFindPrevHeaderFooter(
				DocumentPosition *		dpNew,
				struct DocumentTree **		pTree,
				int *				pTreePage,
				int *				pTreeColumn,
				const struct BufferDocument *	bd,
				int				sectFrom,
				int				hfFrom )
    {
    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;
    const DocumentProperties *	dp= bd->bdProperties;

    int				sect;
    int				hf;
    struct DocumentTree *		tree;

    for ( sect= sectFrom; sect >= 0; sect-- )
	{
	struct BufferItem *	bodySectNode= bodyNode->biChildren[sect];

	for ( hf= hfFrom; hf >= 0; hf-- )
	    {
	    int			page;
	    int			usedInDoc;

	    tree= docFindGetHeaderFooter( bodySectNode, dp, hf );
	    if  ( ! tree || ! tree->dtRoot )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &usedInDoc, bodySectNode,
					    DOC_HeaderFooterTypes[hf], bd );

	    if  ( page < 0 || ! usedInDoc )
		{ continue;	}

	    if  ( docTailPosition( dpNew, tree->dtRoot ) )
		{ LDEB(1); return -1;	}

	    *pTree= tree;
	    *pTreePage= page;
	    *pTreeColumn= 0; /* irrelevant */
	    return 0;
	    }

	hfFrom= DOC_HeaderFooterTypeCount- 1;
	}

    return 1;
    }

static int docFindPrevNote(	DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				const struct BufferDocument *	bd,
				struct DocumentField *		dfNote )
    {
    DocumentNote *	dn;

    dfNote= docGetPrevNoteInDocument( &dn, bd, dfNote, -1 );
    while( dfNote )
	{
	if  ( dn->dnDocumentTree.dtRoot )
	    {
	    if  ( docTailPosition( dp, dn->dnDocumentTree.dtRoot ) )
		{ LDEB(1); return -1;	}

	    *pTree= &(dn->dnDocumentTree);
	    return 0;
	    }

	dfNote= docGetPrevNoteInDocument( &dn, bd, dfNote, -1 );
	}

    return 1;
    }

static int docFindPrevSeparator( DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				const struct BufferDocument *	bd,
				int				sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep >= 0; sep-- )
	{
	struct DocumentTree *		tree;
	DocumentNote *		dn;

	tree= docDocumentNoteSeparator( (struct BufferDocument *)bd,
					    DOC_FindDocNoteItems[sep] );
	if  ( ! tree || ! tree->dtRoot )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		if  ( dn->dnReferringColumn < 0 )
		    { continue;	}
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    case DOCinAFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
		    { continue;	}
		break;

	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docTailPosition( dp, tree->dtRoot ) )
	    { LDEB(1); return -1;	}

	*pTree= tree;
	return 0;
	}

    return 1;
    }

static int docFindPrevPrevTree(	DocumentPosition *		dp,
				struct DocumentTree **		pTree,
				int *				pPage,
				int *				pColumn,
				int				samePhase,
				const struct BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    struct DocumentField *	dfNote= (struct DocumentField *)0;
    int				sectFrom;
    int				sepFrom;

    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;
    const struct BufferItem *		selSectNode;
    const SelectionScope *	ss;

    selSectNode= docGetSectNode( dp->dpNode );
    if  ( ! selSectNode )
	{ XDEB(selSectNode); return -1;	}
    ss= &(selSectNode->biSectSelectionScope);

    sectFrom= bodyNode->biChildCount- 1;
    hfFrom= DOC_HeaderFooterTypeCount- 1;
    sepFrom= DOC_FindDocNoteItemCount- 1;

    switch( ss->ssTreeType )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 3;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssOwnerSectNr;
	    hfFrom= docFindGetHeaderFooterIndex( selSectNode->biTreeType );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectNode->biTreeType,hfFrom); return -1;	}

	    hfFrom--;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    dfNote= docGetFieldByNumber( bd, ss->ssOwnerNumber );
	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectNode->biTreeType );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectNode->biTreeType,sepFrom); return -1; }
	    sepFrom--;

	    break;

	default:
	    LDEB(ss->ssTreeType);
	    LDEB(selSectNode->biTreeType);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 3 )
	{
	res= docFindPrevSeparator( dp, pTree, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 2 )
	{
	res= docFindPrevNote( dp, pTree, bd, dfNote );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 1 )
	{
	res= docFindPrevHeaderFooter( dp, pTree, pPage, pColumn,
						bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docTailPosition( dp, bodyNode ) )
	{ LDEB(1); return 1;	}

    return 0;
    }

static int docFindInCurrentTree(
				DocumentSelection *		ds,
				const DocumentFindJob *		dfj )
    {
    struct BufferItem *		node= dfj->dfjCurrentPosition.dpNode;

    DocumentPosition		dpHere;

    if  ( node->biLevel != DOClevPARA )
	{ LLDEB(node->biLevel,DOClevPARA); return -1;	}

    dpHere= dfj->dfjCurrentPosition;

    for (;;)
	{
	int	ret= (*dfj->dfjFindString)( ds, node,
				dfj->dfjDocument, dfj->dfjTree,
				&dpHere, dfj->dfjThrough );

	if  ( ret == 0 )
	    { return ret;		}
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	node= (*dfj->dfjStepPara)( node );
	if  ( ! node )
	    { return 1;	}
	if  ( (*dfj->dfjStartPara)( &dpHere, node ) )
	    { LDEB(1); return 1;	}
	}
    }

/************************************************************************/

typedef int (*FIND_IN_TREE)(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const struct BufferDocument *	bd,
				PARA_FIND_STRING		findNext,
				void *				through );

static int docFindStepInDocument(
				DocumentSelection *		ds,
				DocumentFindJob *		dfj )
    {
    int				ret= -1;
    DocumentSelection		dsNew;

    const int			samePhase= 0;

    docInitDocumentSelection( &dsNew );

    for (;;)
	{
	ret= docFindInCurrentTree( &dsNew, dfj );
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	if  ( ret == 0 )
	    {
	    if  ( dfj->dfjReverse )
		{
		dsNew.dsDirection=  -1;
		dsNew.dsAnchor= dsNew.dsTail;
		}
	    else{
		dsNew.dsDirection=  1;
		dsNew.dsAnchor= dsNew.dsHead;
		}

	    docSetSelectionScope( &dsNew );

	    if  ( dsNew.dsHead.dpNode->biTreeType != DOCinBODY )
		{
		struct DocumentTree *	tree;
		struct BufferItem *	bodySectNode;

		if  ( docGetTreeOfNode( &tree, &bodySectNode,
				(struct BufferDocument *)dfj->dfjDocument,
				dsNew.dsHead.dpNode ) )
		    { LDEB(1); return -1;	}

		tree->dtPageSelectedUpon= dfj->dfjTreePage;
		tree->dtColumnSelectedIn= dfj->dfjTreeColumn;
		}

	    *ds= dsNew;

	    return 0;
	    }

	if  ( docSelectionSameRoot( &(dfj->dfjStartSelection),
					    dfj->dfjCurrentPosition.dpNode ) )
	    {
	    if  ( dfj->dfjWrapCount > 0 )
		{ break;	}

	    dfj->dfjWrapCount++;
	    }

	ret= (*dfj->dfjStepTree)( &(dfj->dfjCurrentPosition),
				    &(dfj->dfjTree),
				    &(dfj->dfjTreePage),
				    &(dfj->dfjTreeColumn),
				    samePhase, dfj->dfjDocument );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;	}
	if  ( ret > 0 )
	    { return ret;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Look for the next occurrence of something in a whole document	*/
/*  including the section headers and footers.				*/
/*									*/
/************************************************************************/

int docFindFindNextInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    const int			reverse= 0;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindNextNextTree;
    dfj.dfjStepPara= docNextParagraph;
    dfj.dfjStartPara= docHeadPosition;
    dfj.dfjFindString= findNext;

    dfj.dfjDocument= bd;
    dfj.dfjThrough= through;

    if  ( dpFrom && dpFrom->dpNode )
	{
	if  ( docGetTreeOfNode( &(dfj.dfjTree),
				(struct BufferItem **)0, bd, dpFrom->dpNode ) )
	    { LDEB(1); return -1;	}
	}
    else{ dfj.dfjTree= &(bd->bdBody);	}

    return docFindStepInDocument( ds, &dfj );
    }

int docFindFindPrevInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through )
    {
    const int			reverse= 1;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindPrevPrevTree;
    dfj.dfjStepPara= docPrevParagraph;
    dfj.dfjStartPara= docTailPosition;
    dfj.dfjFindString= findPrev;

    dfj.dfjDocument= bd;
    dfj.dfjThrough= through;

    if  ( dpFrom && dpFrom->dpNode )
	{
	if  ( docGetTreeOfNode( &(dfj.dfjTree),
				(struct BufferItem **)0, bd, dpFrom->dpNode ) )
	    { LDEB(1); return -1;	}
	}
    else{ dfj.dfjTree= &(bd->bdBody);	}

    return docFindStepInDocument( ds, &dfj );
    }

/************************************************************************/
/*									*/
/*  Find something forward/backward in one branch of a tree.		*/
/*									*/
/*  This can be done by walking over the leaf nodes of the tree.	*/
/*									*/
/************************************************************************/

int docFindFindNextInCurrentTree(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    const int			reverse= 0;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindNextNextTree;
    dfj.dfjStepPara= docNextParagraph;
    dfj.dfjStartPara= docHeadPosition;
    dfj.dfjFindString= findNext;

    dfj.dfjDocument= bd;
    dfj.dfjTree= tree;
    dfj.dfjThrough= through;

    return docFindInCurrentTree( ds, &dfj );
    }

/************************************************************************/
/*									*/
/*  Navigation between trees with the PageUp/PageDown keys.		*/
/*									*/
/************************************************************************/

int docNextSimilarTree(		DocumentPosition *	dp,
				struct DocumentTree **	pTree,
				struct BufferDocument *	bd )
    {
    int				page= dp->dpNode->biTopPosition.lpPage;
    int				column= dp->dpNode->biTopPosition.lpColumn;
    const int			samePhase= 1;
    struct DocumentTree *	tree= (struct DocumentTree *)0;

    int ret= docFindNextNextTree( dp, &tree, &page, &column, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pTree= tree;
    return ret;
    }

int docPrevSimilarTree(		DocumentPosition *	dp,
				struct DocumentTree **		pTree,
				struct BufferDocument *	bd )
    {
    int				page= dp->dpNode->biTopPosition.lpPage;
    int				column= dp->dpNode->biTopPosition.lpColumn;
    const int			samePhase= 1;
    struct DocumentTree *	tree= (struct DocumentTree *)0;

    int ret= docFindPrevPrevTree( dp, &tree, &page, &column, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pTree= tree;
    return ret;
    }
