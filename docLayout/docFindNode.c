/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docBuf.h>
#   include	"docLayout.h"
#   include	<geoRectangle.h>
#   include	<docHeaderFooterScopes.h>

#   include	<appDebugon.h>

int docFindHeaderFooterForY(		struct DocumentTree **	pTree,
					struct BufferItem **	pSelSectNode,
					int *			pColumn,
					const struct LayoutContext *	lc,
					struct BufferItem *	bodySectNode,
					struct BufferDocument *	bd,
					int			page,
					int			docY )
    {
    int				isEmpty;
    struct DocumentTree *	tree;

    DocumentRectangle		drExtern;
    const int			justUsed= 0;

    /*  4  */
    tree= (struct DocumentTree *)0;
    docDrawWhatPageHeader( &tree, &isEmpty, bodySectNode, page, bd );
    if  ( tree && tree->dtRoot )
	{
	const int		headerColumn= 0;

	if  ( docGetBoxAroundTree( &drExtern, tree, justUsed,
						    page, headerColumn, lc ) )
	    { LDEB(1);	}
	else{
	    if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		{
		*pTree= tree;
		*pSelSectNode= tree->dtRoot;
		*pColumn= headerColumn;

		return 0;
		}
	    }
	}

    /*  5  */
    tree= (struct DocumentTree *)0;
    docDrawWhatPageFooter( &tree, &isEmpty, bodySectNode, page, bd );
    if  ( tree && tree->dtRoot )
	{
	const int		footerColumn= 0;

	if  ( docGetBoxAroundTree( &drExtern, tree, justUsed,
						    page, footerColumn, lc ) )
	    { LDEB(1);	}
	else{
	    if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		{
		*pTree= tree;
		*pSelSectNode= tree->dtRoot;
		*pColumn= footerColumn;

		return 0;
		}
	    }
	}

    return 1;
    }

