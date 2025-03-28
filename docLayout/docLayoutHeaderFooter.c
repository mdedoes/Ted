/************************************************************************/
/*									*/
/*  Geometry administration for 'DocumentTrees' I.E. headers/footers	*/
/*  and foonotes/endnotes.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docBuf.h>
#   include	"docSelectLayout.h"
#   include	<docTreeType.h>
#   include	<docSelect.h>

#   include	<appDebugon.h>

int docPositionNearHeadFoot(	struct DocumentPosition *	dpNew,
				int				treeType,
				struct BufferItem *		bodySectNode,
				struct BufferDocument *		bd,
				int				page )
    {
    int			part;
    int			line;
    struct BufferItem *	bodyRootNode= bd->bdBody.dtRoot;

    const int		column= 0;

    switch( treeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	    if  ( docGetFirstInColumnForNode( dpNew, &line, &part,
						bodySectNode, page, column ) )
		{
		LLDEB(page,column);
		if  ( docHeadPosition( dpNew, bodySectNode )	&&
		      docHeadPosition( dpNew, bodyRootNode )	)
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docGetLastInColumnForNode( dpNew, &line, &part,
						bodySectNode, page, column ) )
		{
		LLDEB(page,column);
		if  ( docTailPosition( dpNew, bodySectNode )	&&
		      docTailPosition( dpNew, bodyRootNode )	)
		    { LDEB(1); return -1;	}
		}
	    break;

	default:
	    LDEB(treeType); return -1;
	}

    return 0;
    }

