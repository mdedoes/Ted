/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/************************************************************************/

static const char STRUCTtypeARTIFACT[] = "Artifact";

/**
 * Skip empty paragraphs
 * Only mark nodes that land on a single page.
 */
int docPsMarkNode(	const struct BufferItem *		node )
    {
    return node->biTopPosition.lpPage == node->biBelowPosition.lpPage;
    }

int docPsPrintBeginMarkedGroup(	PrintingState *		ps,
				const char *		structureType,
				const MemoryBuffer *	structureAttributes )
    {
    const int	contentId= -1;

    StructItem * structItem= psPdfGroupStructItem( ps, structureType, contentId );

    if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	{ XDEB(structItem); return -1;	}

    if  ( psPdfmarkAppendMarkedGroup( ps, structItem, structureAttributes ) )
	{ LDEB(1); return -1;	}

    return psPdfBeginMarkedContent( ps, structureType, -1, structureAttributes );
    }

int docPsPrintEndMarkedGroup(	PrintingState *		ps,
				const char *		structureType )
    {
    psPdfPopStructItem( ps );
    return psPdfEndMarkedContent( ps, structureType );
    }

/**
 *  Begin an artifact in the output. We want to avoid /Artifacts 
 *  inside marked content. For that reason, we pop a possible 
 *  open /Span from the stack
 */
int docPsPrintBeginTypedArtifact(
			PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName )
    {
    if  ( docPsPrintFinishInline( ps ) )
	{ LDEB(1); return -1;	}

    if  ( psPdfBeginTypedArtifact( ps, typeName, subtypeName ) )
	{ LDEB(ps->psInArtifact); return -1;	}

    ps->psInArtifact++;

    return 0;
    }

/**
 *  Begin an artifact in the output. We want to avoid /Artifacts 
 *  inside marked content. For that reason, we pop a possible 
 *  open /Span from the stack
 */
int docPsPrintBeginArtifact(
			PrintingState *			ps )
    {
    if  ( docPsPrintFinishInline( ps ) )
	{ LDEB(1); return -1;	}

    if  ( psPdfBeginMarkedContent( ps, STRUCTtypeARTIFACT, -1, (const MemoryBuffer *)0 ) )
	{ LDEB(ps->psInArtifact); return -1;	}

    ps->psInArtifact++;

    return 0;
    }

/**
 *  End an artifact. Simply pop the piece of marked content.
 */
int docPsPrintEndArtifact(	PrintingState *		ps )
    {
    ps->psInArtifact--;

    return psPdfEndMarkedContent( ps, STRUCTtypeARTIFACT );
    }
