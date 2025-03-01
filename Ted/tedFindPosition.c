/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	<docDraw.h>
#   include	<docNodeTree.h>
#   include	<docTreeNode.h>
#   include	<docSelect.h>
#   include	<docSelectionGeometry.h>
#   include	<docDocumentTree.h>
#   include	<docBuf.h>
#   include	<docPositionFindJob.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Translate a Window coordinate to a position in a text buffer.	*/
/*									*/
/************************************************************************/

int tedFindPosition(	DocumentPosition *		dp,
			struct BufferItem *		selRootNode,
			const struct BufferItem *	bodySectNode,
			const LayoutContext *		lc,
			int				docXPixels,
			int				docYPixels )
    {
    PositionFindJob	pfj;

    docSetFindPositionJob( &pfj, lc, docXPixels, docYPixels );

    pfj.pfjDrawingContext.dcBodySectNode= bodySectNode;

    if  ( docDrawNode( (LayoutPosition *)0, selRootNode,
				(void *)&pfj, &(pfj.pfjDrawingContext) ) )
	{ LDEB(1); return 1;	}
    if  ( ! pfj.pfjFound )
	{ return 1;		}

    *dp= pfj.pfjDocumentPosition;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a node for a particular position.				*/
/*									*/
/************************************************************************/

int tedFindPositionForCoordinates(	DocumentPosition *	dpFound,
					PositionGeometry *	pgFound,
					struct DocumentTree **	pTree,
					struct BufferItem **	pSelSectNode,
					struct BufferItem **	pBodySectNode,
					struct EditDocument *	ed,
					int			docXPixels,
					int			docYPixels )
    {
    LayoutContext		lc;
    PositionFindJob		pfj;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    docSetFindPositionJob( &pfj, &lc, docXPixels, docYPixels );

    if  ( docDrawPageRange( &(pfj.pfjDrawingContext), (void *)&pfj ) )
	{ LDEB(1); return -1;	}

    if  ( pfj.pfjFound )
	{
	struct BufferItem *	selSectNode= (struct BufferItem *)0;
	struct BufferItem *	bodySectNode= (struct BufferItem *)0;

	selSectNode= docGetSectNode( pfj.pfjDocumentPosition.dpNode );

	if  ( pTree || pBodySectNode )
	    {
	    if  ( docGetRootOfSelectionScope( pTree, &bodySectNode,
		    lc.lcDocument, &(selSectNode->biSectSelectionScope) ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( dpFound )
	    { *dpFound= pfj.pfjDocumentPosition;	}
	if  ( pgFound )
	    { *pgFound= pfj.pfjPositionGeometry;	}
	if  ( pSelSectNode )
	    { *pSelSectNode= selSectNode;		}
	if  ( pBodySectNode )
	    { *pBodySectNode= bodySectNode;		}

	return 0;
	}

    return 1;
    }
