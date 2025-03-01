#   include	"docLayoutConfig.h"

#   include	"docSectLayout.h"
#   include	<docTreeNode.h>
#   include	<docSectProperties.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

BufferItem * docFindFirstSectionOnPage(
		BufferItem *		bodyNode,
		int			page )
    {
    int		sect;

    for ( sect= 0; sect < bodyNode->biChildCount; sect++ )
	{
	if  ( bodyNode->biChildren[sect]->biBelowPosition.lpPage >= page )
	    { return bodyNode->biChildren[sect];	}
	}

    LLDEB(bodyNode->biChildCount,sect);
    return (struct BufferItem *)0;
    }

BufferItem * docFindLastSectionOnPage(
		BufferItem *		bodyNode,
		int			page )
    {
    int		sect;

    for ( sect= bodyNode->biChildCount- 1; sect >= 0; sect-- )
	{
	if  ( bodyNode->biChildren[sect]->biTopPosition.lpPage <= page )
	    { return bodyNode->biChildren[sect];	}
	}

    LLDEB(bodyNode->biChildCount,sect);
    return (struct BufferItem *)0;
    }

/**
 *  Decide whether to print a page header as an artifact.
 *  Though this is only relevant in the context of printing
 *  PostScript that will be converted to PDF, the implementation
 *  does not use any PostScript specific information.
 */
int docDrawHeaderAsArtifact(
		int			page,
		const BufferItem *	bodySectNode )
    {
    if  ( bodySectNode->biSectFirstPageHeaderNoPdfArtifact	&&
	  page == bodySectNode->biTopPosition.lpPage		)
	{ return 0;	}

    if  ( bodySectNode->biSectLastPageHeaderNoPdfArtifact	&&
	  page == bodySectNode->biBelowPosition.lpPage		)
	{ return 0;	}

    return 1;
    }

/**
 *  Decide whether to print a page footer as an artifact.
 *  Though this is only relevant in the context of printing
 *  PostScript that will be converted to PDF, the implementation
 *  does not use any PostScript specific information.
 */
int docDrawFooterAsArtifact(
		int			page,
		const BufferItem *	bodySectNode )
    {
    if  ( bodySectNode->biSectFirstPageFooterNoPdfArtifact	&&
	  page == bodySectNode->biTopPosition.lpPage		)
	{ return 0;	}

    if  ( bodySectNode->biSectLastPageFooterNoPdfArtifact	&&
	  page == bodySectNode->biBelowPosition.lpPage		)
	{ return 0;	}

    return 1;
    }

