#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	<docLayout.h>
#   include	<layoutContext.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docSelect.h>
#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<geoRectangle.h>
#   include	<docHeaderFooterScopes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw external trees such as headers/footers/footnotes/endnotes.	*/
/*									*/
/************************************************************************/

static int docDrawTreeRoot(	LayoutPosition *		lpBelow,
				DocumentTree *			tree,
				const struct BufferItem *	bodySectNode,
				DrawingContext *		dc,
				void *				through )
    {
    int				rval;
    const struct DocumentTree *	saveTree= dc->dcTree;
    const struct BufferItem *	saveBodySectNode= dc->dcBodySectNode;

    dc->dcBodySectNode= bodySectNode;
    dc->dcTree= tree;

    if  ( dc->dcStartTree && (*dc->dcStartTree)( through, dc, tree ) )
	{ LDEB(tree->dtRoot->biTreeType); return -1; }

    rval= docDrawNode( lpBelow, tree->dtRoot, through, dc );
    if  ( rval )
	{ LDEB(rval);	}

    if  ( dc->dcFinishTree && (*dc->dcFinishTree)( through, dc, tree ) )
	{ LDEB(tree->dtRoot->biTreeType); return -1; }

    dc->dcBodySectNode= saveBodySectNode;
    dc->dcTree= saveTree;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Print/Draw Headers/Footers.						*/
/*									*/
/************************************************************************/

static int docDrawHeaderFooter(	struct BufferItem *	bodySectNode,
				void *			through,
				struct DocumentTree *	tree,
				DrawingContext *	dc,
				int			page )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    const int			column= 0; /* irrelevant */
    LayoutPosition		lpBelow;

    docInitLayoutPosition( &lpBelow );

    if  ( dc->dcClipRect )
	{
	const int		justUsed= 1;
	DocumentRectangle	drExtern;

	if  ( docGetBoxAroundTree( &drExtern, tree, justUsed,
							    page, column, lc ) )
	    { LDEB(1); return -1;	}

	if  ( ! geoIntersectRectangle( &drExtern, &drExtern, dc->dcClipRect ) )
	    { return 0;	}
	}

    if  ( page != tree->dtPageFormattedFor	||
	  column != tree->dtColumnFormattedFor	)
	{
	const int	adjustDocument= 0;

	/*  We do not expect the tree to change height here	*/
	if  ( docLayoutDocumentTree( tree, (DocumentRectangle *)0,
			    page, column, tree->dtY0UsedTwips,
			    bodySectNode, lc, dc->dcStartTreeLayout,
			    adjustDocument ) )
	    { LLDEB(page,column); return -1;	}
	}

    if  ( docDrawTreeRoot( &lpBelow, tree, bodySectNode, dc, through ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docDrawPageHeader(	struct BufferItem *		bodySectNode,
			void *				through,
			DrawingContext *		dc,
			int				page )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;

    struct DocumentTree *	tree= (struct DocumentTree *)0;
    int				isEmpty;

    docDrawWhatPageHeader( &tree, &isEmpty, bodySectNode, page, bd );
    if  ( ! tree || ! tree->dtRoot )
	{ return 0;	}

    if  ( docDrawHeaderFooter( bodySectNode, through, tree, dc, page ) )
	{ LDEB(page); return 1;	}

    return 0;
    }

int docDrawPageFooter(	struct BufferItem *		bodySectNode,
			void *				through,
			DrawingContext *		dc,
			int				page )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;

    struct DocumentTree *	tree= (struct DocumentTree *)0;
    int				isEmpty;

    docDrawWhatPageFooter( &tree, &isEmpty, bodySectNode, page, bd );
    if  ( ! tree || ! tree->dtRoot )
	{ return 0;	}

    if  ( docDrawHeaderFooter( bodySectNode, through, tree, dc, page ) )
	{ LDEB(page); return 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the notes on a page.						*/
/*									*/
/*  1)  Calculate the position of the separator.			*/
/*  2)  In clipped drawing.. Will the separator be visible anyway?	*/
/*  3)  If necassary recalculate the layout of the separator.		*/
/*  4)  Draw it.							*/
/*									*/
/************************************************************************/

static int docDrawNoteSeparator(
			LayoutPosition *		lpBelow,
			void *				through,
			DrawingContext *		dc,
			const struct BufferItem *	bodySectNode,
			const DocumentNote *		dnFirstNote,
			int				treeType )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    const struct DocumentTree *	firstNoteTree;
    struct DocumentTree *	noteSepTree;
    int				y0Twips;

    int				ret;

    DocumentRectangle		drExtern;
    int				page;
    int				column;

    const int			adjustDocument= 0;

    firstNoteTree= &(dnFirstNote->dnDocumentTree);
    page= firstNoteTree->dtRoot->biTopPosition.lpPage;
    column= firstNoteTree->dtRoot->biTopPosition.lpColumn;

    /*  1  */
    ret= docNoteSeparatorRectangle( &drExtern, &noteSepTree, &y0Twips,
				dnFirstNote, treeType, bodySectNode, lc );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return 0;	}

    /*  2  */
    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drExtern, &drExtern, dc->dcClipRect ) )
	{ return 0;	}

    /*  3  */
    if  ( page != noteSepTree->dtPageFormattedFor	||
	  column != noteSepTree->dtColumnFormattedFor	)
	{
	/*  We do not expect the tree to change height here	*/
	if  ( docLayoutDocumentTree( noteSepTree, (DocumentRectangle *)0,
			page, column, y0Twips,
			bodySectNode, lc, dc->dcStartTreeLayout,
			adjustDocument ) )
	    { LDEB(1); return -1;	}
	}

    /*  4  */
    if  ( docDrawTreeRoot( lpBelow, noteSepTree, bodySectNode, dc, through ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docDrawFootnotesForColumn(	int				page,
				int				column,
				void *				through,
				DrawingContext *		dc )
    {
    int				rval= 0;
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;
    DocumentField *		dfNote;
    DocumentNote *		dn;
    const BufferItem *		bodySectNode;

    LayoutPosition		lpBelow;

    docInitLayoutPosition( &lpBelow );

    dfNote= docGetFirstNoteFromColumn( &dn, bd,
					    page, column, DOCinFOOTNOTE );
    if  ( ! dfNote )
	{ goto ready;	}

    bodySectNode= docGetBodySectNodeOfScope( &(dfNote->dfSelectionScope), bd );

    if  ( docDrawNoteSeparator( &lpBelow, through, dc, bodySectNode, dn,
							    DOCinFTNSEP ) )
	{ LDEB(page);	}

    for ( ;
	  dfNote;
	  dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, DOCinFOOTNOTE ) )
	{
	struct DocumentTree *	tree= &(dn->dnDocumentTree);

	if  ( dn->dnReferringPage < page )
	    { LLDEB(page,dn->dnReferringPage); continue;	}
	if  ( dn->dnReferringPage > page )
	    { break;	}
	if  ( dn->dnReferringColumn < column )
	    { LLLDEB(page,column,dn->dnReferringColumn); continue;	}
	if  ( dn->dnReferringColumn > column )
	    { break;	}
	if  ( ! tree->dtRoot )
	    { XDEB(tree->dtRoot); continue;	}

	if  ( docDrawTreeRoot( &lpBelow, tree, bodySectNode, dc, through ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the notes belonging to a section immediately below its text.	*/
/*									*/
/*  1)  Are there any?							*/
/*  2)  Get separator.							*/
/*  3)  Draw the separator.						*/
/*  4)  Draw the endnotes of this section.				*/
/*									*/
/************************************************************************/

int docDrawEndnotesForSection(		LayoutPosition *	lpBelow,
					int			sect,
					void *			through,
					DrawingContext *	dc )
    {
    int				rval= 0;
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;
    struct BufferItem *		bodySectNode;

    DocumentNote *		dn;
    DocumentField *		dfNote;
    struct DocumentTree *	tree;

    LayoutPosition		lpHere;
    DocumentPosition		dp;

    BlockOrigin			bo;

    docInitBlockOrigin( &bo );

    /*  1  */
    if  ( sect >= 0 )
	{
	bodySectNode= bd->bdBody.dtRoot->biChildren[sect];
	dfNote= docGetFirstNoteOfSection( &dn, bd, sect, DOCinENDNOTE );

	if  ( ! dfNote )
	    { goto ready;	}

	if  ( docTailPosition( &dp, bodySectNode ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	int lastSect= bd->bdBody.dtRoot->biChildCount- 1;
	bodySectNode= bd->bdBody.dtRoot->biChildren[lastSect];
	dfNote= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );     

	if  ( ! dfNote )
	    { goto ready;	}

	if  ( docDocumentTail( &dp, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    lpHere= dp.dpNode->biBelowPosition;

    tree= &(dn->dnDocumentTree);
    if  ( dc->dcLastPage >= 0					&&
	  tree->dtRoot->biTopPosition.lpPage > dc->dcLastPage	)
	{ goto ready;	}

    if  ( docDrawToColumnOfNode( bodySectNode, bodySectNode,
						through, &lpHere, dc, &bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  2  */
    if  ( dc->dcFirstPage < 0					||
	  tree->dtRoot->biBelowPosition.lpPage >= dc->dcFirstPage	)
	{
	if  ( docDrawNoteSeparator( lpBelow, through, dc,
					bodySectNode, dn, DOCinAFTNSEP ) )
	    { LDEB(sect);	}
	}

    /*  4  */
    for ( ;
	  dfNote;
	  dfNote= docGetNextNoteInSection( &dn, bd, sect,
						    dfNote, DOCinENDNOTE ) )
	{
	tree= &(dn->dnDocumentTree);
	if  ( dc->dcLastPage >= 0					&&
	      tree->dtRoot->biTopPosition.lpPage > dc->dcLastPage	)
	    { goto ready;	}

	if  ( docDrawToColumnOfNode( bodySectNode, bodySectNode,
						through, &lpHere, dc, &bo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( dc->dcFirstPage < 0					||
	      tree->dtRoot->biBelowPosition.lpPage >= dc->dcFirstPage	)
	    {
	    if  ( docDrawTreeRoot( lpBelow, tree, bodySectNode, dc, through ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	lpHere= tree->dtRoot->biBelowPosition;
	}

  ready:

    return rval;
    }

int docDrawEndnotesForDocument(		LayoutPosition *	lpBelow,
					void *			through,
					DrawingContext *	dc )
    {
    return docDrawEndnotesForSection( lpBelow, -1, through, dc );
    }

