/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	<docTreeType.h>
#   include	"docSectHeadersFooters.h"
#   include	"docHeaderFooterScopes.h"
#   include	<docParaProperties.h>
#   include	"docSelect.h"
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	"docAttributes.h"
#   include	"docNodeTree.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Kinds of Headers/Footers.						*/
/*									*/
/*  The arrays are a mapping from the options to the header/footer data	*/
/*  structure that is used in the implementation.			*/
/*									*/
/************************************************************************/

const int DOC_HeaderScopes[PAGES__COUNT]=
    {
    DOCinFIRST_HEADER,	/*  PAGES_FIRST_PAGE		*/
    DOCinRIGHT_HEADER,	/*  PAGES_SUBSEQUENT_PAGES	*/
    DOCinRIGHT_HEADER,	/*  PAGES_ALL_PAGES		*/
    DOCinRIGHT_HEADER,	/*  PAGES_ODD_PAGES		*/
    DOCinLEFT_HEADER,	/*  PAGES_EVEN_PAGES		*/
    DOCinLAST_HEADER,	/*  PAGES_LAST_PAGE		*/
    };

const int DOC_FooterScopes[PAGES__COUNT]=
    {
    DOCinFIRST_FOOTER,	/*  PAGES_FIRST_PAGE		*/
    DOCinRIGHT_FOOTER,	/*  PAGES_SUBSEQUENT_PAGES	*/
    DOCinRIGHT_FOOTER,	/*  PAGES_ALL_PAGES		*/
    DOCinRIGHT_FOOTER,	/*  PAGES_ODD_PAGES		*/
    DOCinLEFT_FOOTER,	/*  PAGES_EVEN_PAGES		*/
    DOCinLAST_FOOTER,	/*  PAGES_LAST_PAGE		*/
    };

/************************************************************************/
/*									*/
/*  Determine what header/footer applies for a certain page.		*/
/*									*/
/************************************************************************/

static int docCheckTreeIsEmpty(	int *			pIsEmpty,
				const struct BufferDocument *	bd,
				const struct DocumentTree *	dt )
    {
    int			isEmpty= 1;

    if  ( dt && dt->dtRoot )
	{
	DocumentPosition		dpBegin;
	DocumentPosition		dpNext;

	if  ( docHeadPosition( &dpBegin, dt->dtRoot ) )
	    { LDEB(1); docInitDocumentPosition( &dpBegin );	}

	dpNext= dpBegin;
	if  ( ! docNextPosition( &dpNext ) )
	    { isEmpty= 0;	}
	else{
	    const ParagraphProperties *	pp= dpBegin.dpNode->biParaProperties;

	    if  ( pp->ppTableNesting > 0 )
		{ isEmpty= 0;	}

	    if  ( docBorderNumberIsBorder( bd, pp->ppTopBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBottomBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppLeftBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppRightBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBetweenBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBarNumber ) )
		{ isEmpty= 0;	}

	    if  ( docShadingNumberIsShading( bd, pp->ppShadingNumber ) )
		{ isEmpty= 0;	}
	    }
	}

    if  ( pIsEmpty )
	{ *pIsEmpty= isEmpty;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Decide what header should go to this page.				*/
/*									*/
/************************************************************************/

static int docWhatPageHeader(	struct DocumentTree **		pTree,
				int *			pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int			page,
				const struct BufferDocument *	bd,
				int			mindEndpg )
    {
    const DocumentProperties *	dp= bd->bdProperties;
    const SectionProperties *	sp= bodySectNode->biSectProperties;

    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;
    struct DocumentTree *		tree= (struct DocumentTree *)0;

    if  ( ! shf )
	{ XDEB(shf); return -1;	}

    if  ( page == bodySectNode->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	tree= &(shf->shfFirstPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinFIRST_HEADER;
	}

    if  ( mindEndpg					&&
	  sp->spHasEndPage				&&
	  page == bodySectNode->biBelowPosition.lpPage	)
	{
	tree= &(shf->shfLastPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLAST_HEADER;
	}

    if  ( page % 2 && dp->dpHasFacingPages )
	{
	tree= &(shf->shfLeftPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLEFT_HEADER;
	}
    else{
	tree= &(shf->shfRightPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinRIGHT_HEADER;
	}
    }

static int docWhatPageFooter(	struct DocumentTree **		pTree,
				int *			pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int			page,
				const struct BufferDocument *	bd,
				int			mindEndpg )
    {
    const DocumentProperties *	dp= bd->bdProperties;
    const SectionProperties *	sp= bodySectNode->biSectProperties;

    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;
    struct DocumentTree *		tree= (struct DocumentTree *)0;

    if  ( ! shf )
	{ XDEB(shf); return -1;	}

    if  ( page == bodySectNode->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	tree= &(shf->shfFirstPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinFIRST_FOOTER;
	}

    if  ( mindEndpg					&&
	  sp->spHasEndPage				&&
	  page == bodySectNode->biBelowPosition.lpPage	)
	{
	tree= &(shf->shfLastPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLAST_FOOTER;
	}

    if  ( page % 2 && dp->dpHasFacingPages )
	{
	tree= &(shf->shfLeftPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLEFT_FOOTER;
	}
    else{
	tree= &(shf->shfRightPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinRIGHT_FOOTER;
	}
    }

int docLayoutWhatPageHeader(	struct DocumentTree **		pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd )
    {
    const int	mindEndpg= 0;

    return docWhatPageHeader( pTree, pIsEmpty, bodySectNode,
						    page, bd, mindEndpg );
    }

int docDrawWhatPageHeader(	struct DocumentTree **		pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd )
    {
    const int	mindEndpg= 1;

    return docWhatPageHeader( pTree, pIsEmpty, bodySectNode,
						    page, bd, mindEndpg );
    }

int docLayoutWhatPageFooter(	struct DocumentTree **		pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd )
    {
    const int	mindEndpg= 0;

    return docWhatPageFooter( pTree, pIsEmpty, bodySectNode,
						    page, bd, mindEndpg );
    }

int docDrawWhatPageFooter(	struct DocumentTree **		pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd )
    {
    const int	mindEndpg= 1;

    return docWhatPageFooter( pTree, pIsEmpty, bodySectNode,
						    page, bd, mindEndpg );
    }

/************************************************************************/
/*									*/
/*  Decide what footer should go to this page.				*/
/*									*/
/************************************************************************/

int docWhatPagesForHeaderFooter( const DocumentProperties *	dp,
				const SectionProperties *	sp,
				int				treeType )
    {
    switch( treeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    return PAGES_FIRST_PAGE;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    return PAGES_EVEN_PAGES;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{ return PAGES_ODD_PAGES;		}
	    else{
		if  ( sp->spHasTitlePage )
		    { return PAGES_SUBSEQUENT_PAGES;	}
		else{ return PAGES_ALL_PAGES;		}
		}
	    /*unreachable*/

	case DOCinLAST_HEADER:
	case DOCinLAST_FOOTER:
	    return PAGES_LAST_PAGE;

	default:
	    LSDEB(treeType,docTreeTypeStr(treeType));
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Return a particular header or footer by scope.			*/
/*									*/
/************************************************************************/

struct DocumentTree *	docSectionHeaderFooter(
				const BufferItem *		bodySectNode,
				unsigned char *			pApplies,
				const DocumentProperties *	dp,
				int				treeType )
    {
    const SectionProperties *	sp= bodySectNode->biSectProperties;
    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;

    if  ( ! shf )
	{ XDEB(shf); return (struct DocumentTree *)0;	}

    switch( treeType )
	{
	case DOCinFIRST_HEADER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasTitlePage;	}
	    return &(shf->shfFirstPageHeader);

	case DOCinLEFT_HEADER:
	    if  ( pApplies )
		{ *pApplies= dp->dpHasFacingPages;	}
	    return &(shf->shfLeftPageHeader);

	case DOCinRIGHT_HEADER:
	    if  ( pApplies )
		{ *pApplies= 1;	}
	    return &(shf->shfRightPageHeader);

	case DOCinLAST_HEADER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasEndPage;	}
	    return &(shf->shfLastPageHeader);

	case DOCinFIRST_FOOTER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasTitlePage;	}
	    return &(shf->shfFirstPageFooter);

	case DOCinLEFT_FOOTER:
	    if  ( pApplies )
		{ *pApplies= dp->dpHasFacingPages;	}
	    return &(shf->shfLeftPageFooter);

	case DOCinRIGHT_FOOTER:
	    if  ( pApplies )
		{ *pApplies= 1;	}
	    return &(shf->shfRightPageFooter);

	case DOCinLAST_FOOTER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasEndPage;	}
	    return &(shf->shfLastPageFooter);

	default:
	    LDEB(treeType); return (struct DocumentTree *)0;
	}
    }

int docSectionHasHeaderFooter(	const struct BufferItem *	bodySectNode,
				unsigned char *			pApplies,
				const DocumentProperties *	dp,
				int				treeType )
    {
    struct DocumentTree *	dt;

    dt= docSectionHeaderFooter( bodySectNode, pApplies, dp, treeType );
    if  ( ! dt || ! dt->dtRoot )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Return the first page where a particular kind of header/footer can	*/
/*  be used in a document. The existence of the tree is checked by the	*/
/*  caller.								*/
/*									*/
/************************************************************************/

int docSectionHeaderFooterFirstPage(
				int *				pUsedByDocument,
				const struct BufferItem *	bodySectNode,
				int				treeType,
				const BufferDocument *		bd )
    {
    const SectionProperties *	sp= bodySectNode->biSectProperties;
    int				topPage= bodySectNode->biTopPosition.lpPage;
    int				belowPage= bodySectNode->biBelowPosition.lpPage;
    int				page;

    const struct BufferItem *		prevNode= (const struct BufferItem *)0;

    if  ( bodySectNode->biNumberInParent > 0 )
	{
	prevNode= bodySectNode->biParent->biChildren[
					    bodySectNode->biNumberInParent- 1];

	if  ( prevNode->biBelowPosition.lpPage >= belowPage )
	    { *pUsedByDocument= 0; return -1;	}
	}

    switch( treeType )
	{
	case DOCinBODY:
	    LDEB(treeType); *pUsedByDocument= 0; return -1;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    if  ( ! sp->spHasTitlePage )
		{ *pUsedByDocument= 0; return -1;	}

	    if  ( prevNode && prevNode->biBelowPosition.lpPage >= topPage )
		{ *pUsedByDocument= 0; return topPage;	}
	    page= topPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    if  ( ! bd->bdProperties->dpHasFacingPages )
		{ *pUsedByDocument= 0; return -1;	}
	    if  ( topPage % 2 )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( bd->bdProperties->dpHasFacingPages )
		{
		if  ( topPage % 2 == 0 )
		    {
		    if  ( sp->spHasTitlePage )
			{ page= topPage+ 2;	}
		    else{ page= topPage;	}
		    }
		else{ page= topPage+ 1;	}
		}
	    else{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 1;	}
		else{ page= topPage;	}
		}
	    break;

	case DOCinLAST_HEADER:
	case DOCinLAST_FOOTER:
	    page= bodySectNode->biBelowPosition.lpPage;
	    break;

	default:
	    LDEB(treeType); *pUsedByDocument= 0; return -1;
	}

    *pUsedByDocument= page <= bodySectNode->biBelowPosition.lpPage;

    return page;
    }

/************************************************************************/
/*									*/
/*  Return the Header/Footer corrsponding to 'treeType'. If the		*/
/*  selection itself is in an external tree, go to the section in the	*/
/*  body to get the header or footer.					*/
/*									*/
/************************************************************************/

int docGetHeaderFooter(		struct DocumentTree **		pTree,
				struct BufferItem **		pBodySectNode,
				const DocumentPosition *	dp,
				struct BufferDocument *		bd,
				int				treeType )
    {
    struct DocumentTree *	headerFooterTree;
    struct BufferItem *		bodySectNode;
    unsigned char		applies= 1;

    if  ( docGetTreeOfNode( (DocumentTree **)0, &bodySectNode, bd, dp->dpNode ) )
	{ LDEB(1); return -1;	}

    headerFooterTree= docSectionHeaderFooter( bodySectNode, &applies,
					    bd->bdProperties, treeType );
    if  ( ! headerFooterTree )
	{ XDEB(headerFooterTree); return -1;	}

    *pTree= headerFooterTree; *pBodySectNode= bodySectNode; return 0;
    }

/************************************************************************/
/*									*/
/*  Determine whether that document has headers and footers at all.	*/
/*									*/
/************************************************************************/

int docInquireHeadersFooters(	int *			pDocHasHeaders,
				int *			pDocHasFooters,
				const struct BufferDocument *	bd )
    {
    int			i;
    int			hasPageHeader= 0;
    int			hasPageFooter= 0;

    const struct BufferItem *	bodyNode= bd->bdBody.dtRoot;

    for ( i= 0; i < bodyNode->biChildCount; i++ )
	{
	int			j;
	struct BufferItem *		sectNode= bodyNode->biChildren[i];

	for ( j= 0; j < PAGES__COUNT; j++ )
	    {
	    struct DocumentTree *	dt;
	    unsigned char	applies;

	    applies= 1;
	    dt= docSectionHeaderFooter( sectNode, &applies,
				    bd->bdProperties, DOC_HeaderScopes[j] );
	    if  ( dt && dt->dtRoot && applies )
		{ hasPageHeader= 1;	}

	    applies= 1;
	    dt= docSectionHeaderFooter( sectNode, &applies,
				    bd->bdProperties, DOC_FooterScopes[j] );
	    if  ( dt && dt->dtRoot && applies )
		{ hasPageFooter= 1;	}
	    }
	}

    *pDocHasHeaders= hasPageHeader;
    *pDocHasFooters= hasPageFooter;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the page, nearest to the current page that can hold a		*/
/*  particular kinfd of header or footer.				*/
/*									*/
/************************************************************************/

static int DOC_TryPageOffsets[]= { 0, 1, -1, 2 };

int docHeaderFooterPage(	const struct BufferDocument *	bd,
				const struct BufferItem *	bodySectNode,
				int				currentPage,
				int				treeType )
    {
    switch( treeType )
	{
	int		isEmpty;
	int		i;
	struct DocumentTree *	eiTry;
	int		ttForPg;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    return bodySectNode->biTopPosition.lpPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	    for ( i= 0; i < sizeof(DOC_TryPageOffsets)/sizeof(int); i++ )
		{
		int pg= currentPage+ DOC_TryPageOffsets[i];

		if  ( pg < bodySectNode->biTopPosition.lpPage	||
		      pg > bodySectNode->biBelowPosition.lpPage	)
		    { continue;	}

		ttForPg= docDrawWhatPageHeader( &eiTry, &isEmpty,
							bodySectNode, pg, bd );
		if  ( ttForPg == treeType )
		    { return pg;	}
		}
	    break;

	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    for ( i= 0; i < sizeof(DOC_TryPageOffsets)/sizeof(int); i++ )
		{
		int pg= currentPage+ DOC_TryPageOffsets[i];

		if  ( pg < bodySectNode->biTopPosition.lpPage	||
		      pg > bodySectNode->biBelowPosition.lpPage	)
		    { continue;	}

		ttForPg= docDrawWhatPageFooter( &eiTry, &isEmpty,
							bodySectNode, pg, bd );
		if  ( ttForPg == treeType )
		    { return pg;	}
		}
	    break;

	case DOCinLAST_HEADER:
	case DOCinLAST_FOOTER:
	    if  ( bodySectNode->biBelowPosition.lpPage >
				    bodySectNode->biTopPosition.lpPage )
		{ return bodySectNode->biBelowPosition.lpPage;	}
	    break;

	default:
	    LDEB(treeType); return -1;
	}

    return -1;
    }


/************************************************************************/
/*									*/
/*  Force recalculation of header/fooet layout.				*/
/*									*/
/************************************************************************/

void docInvalidateSectHeaderFooterLayout(
				struct BufferItem *	sectNode )
    {
    SectHeadersFooters *	shf= sectNode->biSectHeadersFooters;

    if  ( shf )
	{
	docInvalidateTreeLayout( &(shf->shfFirstPageHeader) );
	docInvalidateTreeLayout( &(shf->shfLeftPageHeader) );
	docInvalidateTreeLayout( &(shf->shfRightPageHeader) );
	docInvalidateTreeLayout( &(shf->shfLastPageHeader) );

	docInvalidateTreeLayout( &(shf->shfFirstPageFooter) );
	docInvalidateTreeLayout( &(shf->shfLeftPageFooter) );
	docInvalidateTreeLayout( &(shf->shfRightPageFooter) );
	docInvalidateTreeLayout( &(shf->shfLastPageFooter) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Bookkeeping of section headers and footers.				*/
/*									*/
/************************************************************************/

void docCleanSectHeadersFooters( struct BufferDocument *		bd,
				SectHeadersFooters *		shf )
    {
    docCleanDocumentTree( bd, &(shf->shfFirstPageHeader) );
    docCleanDocumentTree( bd, &(shf->shfLeftPageHeader) );
    docCleanDocumentTree( bd, &(shf->shfRightPageHeader) );
    docCleanDocumentTree( bd, &(shf->shfLastPageHeader) );

    docCleanDocumentTree( bd, &(shf->shfFirstPageFooter) );
    docCleanDocumentTree( bd, &(shf->shfLeftPageFooter) );
    docCleanDocumentTree( bd, &(shf->shfRightPageFooter) );
    docCleanDocumentTree( bd, &(shf->shfLastPageFooter) );

    return;
    }

void docInitSectHeadersFooters( SectHeadersFooters *		shf )
    {
    docInitDocumentTree( &(shf->shfFirstPageHeader) );
    docInitDocumentTree( &(shf->shfLeftPageHeader) );
    docInitDocumentTree( &(shf->shfRightPageHeader) );
    docInitDocumentTree( &(shf->shfLastPageHeader) );

    docInitDocumentTree( &(shf->shfFirstPageFooter) );
    docInitDocumentTree( &(shf->shfLeftPageFooter) );
    docInitDocumentTree( &(shf->shfRightPageFooter) );
    docInitDocumentTree( &(shf->shfLastPageFooter) );
    }
