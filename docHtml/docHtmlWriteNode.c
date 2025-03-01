/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<ctype.h>

#   include	<docTreeType.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docNotesProperties.h>

#   include	<appDebugon.h>

static int dochtmlLeaveBodyNode(	struct BufferItem *	bodyNode,
					HtmlWritingContext *	hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    if  ( ! hws->hwsInlineNotes )
	{
	if  ( docHtmlSaveFootnotes( hwc ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docGetEndnotePlacement( bd ) == FTNplaceDOC_END )
	{
	int			sect= -1;

	if  ( docHtmlSaveEndnotes( sect, hwc ) )
	    { LDEB(sect); return -1;	}
	}

    return 0;
    }

static int docHtmlLeaveNode(
			struct BufferItem *			node,
			const struct DocumentSelection * 	ds,
			const struct BufferItem *		bodySectNode,
			void *					voidhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)voidhwc;

    switch( node->biLevel )
	{
	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY && ! ds )
		{ dochtmlLeaveSectNode( node, hwc );	}
	    break;

	case DOClevBODY:
	    if  ( ! ds )
		{ dochtmlLeaveBodyNode( node, hwc );	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		docHtmlWritelnCloseElement( docHtmlCellElement( node ), hwc );
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{ docHtmlWritelnCloseElement( "tr", hwc );	}
	    break;

	case DOClevPARA:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return SCANadviceOK;
    }

static int docHtmlEnterNode(	struct BufferItem *		node,
				const struct DocumentSelection * ds,
				const struct BufferItem *	bodySectNode,
				void *				voidhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)voidhwc;
    int				tableNesting= docTableNesting( node );

    while( hwc->hwcTableNesting > tableNesting )
	{ docHtmlFinishTable( hwc );	}

    switch( node->biLevel )
	{
	case DOClevBODY:
	    break;

	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY && ! ds )
		{ dochtmlEnterSectNode( node, hwc );	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		if  ( docHtmlEnterCellNode( hwc,
			    node->biParent, node->biNumberInParent, node ) )
		    { LDEB(node->biNumberInParent); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docHtmlEnterRowNode( hwc, node ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( docHtmlSaveParaNode( hwc, node, bodySectNode, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return SCANadviceOK;
    }

/************************************************************************/

int docHtmlSaveSelection(	HtmlWritingContext *		hwc,
				struct DocumentTree *		dt,
				const struct DocumentSelection * ds )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    const int			flags= 0;

    if  ( ds )
	{
	if  ( docScanSelection( hws->hwsDocument, ds,
					docHtmlEnterNode, docHtmlLeaveNode,
					(TreeVisitor)0, (TreeVisitor)0, 
					flags, (void *)hwc ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( hws->hwsDocument, dt,
					docHtmlEnterNode, docHtmlLeaveNode,
					(TreeVisitor)0, (TreeVisitor)0, 
					flags, (void *)hwc ) < 0 )
	    { LDEB(1); return -1;	}
	}

    while( hwc->hwcTableNesting > 0 )
	{ docHtmlFinishTable( hwc );	}

    return 0;
    }

