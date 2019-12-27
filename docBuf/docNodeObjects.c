/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docTreeScanner.h"
#   include	<docScanner.h>
#   include	<docListDepth.h>
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	"docObjects.h"
#   include	<bitmap.h>
#   include	<docObject.h>
#   include	<docObjectIo.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Close the objects in a buffer node.					*/
/*									*/
/************************************************************************/

typedef struct CleanTreeObjects
    {
    int				ctoBulletsDeleted;
    int				ctoParagraphCount;
    struct DocumentTree *	ctoTree;
    struct CleanTreeObjects *	ctoParent;
    } CleanTreeObjects;

typedef struct CleanNodeObjects
    {
    CleanTreeObjects *		cnoTreeObjects;
    struct BufferDocument *	cnoDocument;
    } CleanNodeObjects;

static void docInitCleanNodeObjects(	CleanNodeObjects *	cno )
    {
    cno->cnoTreeObjects= (CleanTreeObjects *)0;
    cno->cnoDocument= (struct BufferDocument *)0;
    }

static CleanTreeObjects * docPushCleanTreeObjects(
				CleanNodeObjects *	cno,
				DocumentTree *		tree )
    {
    CleanTreeObjects *	cto= malloc( sizeof(CleanTreeObjects) );

    if  ( ! cto )
	{ XDEB(cto); return cto;	}

    cto->ctoBulletsDeleted= 0;
    cto->ctoParagraphCount= 0;
    cto->ctoTree= tree;
    cto->ctoParent= cno->cnoTreeObjects;

    cno->cnoTreeObjects= cto;

    return cto;
    }

static int docPopCleanTreeObjects(
				CleanNodeObjects *	cno,
				DocumentTree *		tree )
    {
    CleanTreeObjects *	cto= cno->cnoTreeObjects;

    if  ( ! cno->cnoTreeObjects )
	{ XDEB(cno->cnoTreeObjects); return -1;	}
    if  ( cno->cnoTreeObjects->ctoTree != tree )
	{ XXDEB(cno->cnoTreeObjects->ctoTree,tree); return -1;	}

    cno->cnoTreeObjects= cto->ctoParent;

    free( cto );

    return 0;
    }

static void docCleanParaObjects(	CleanNodeObjects *	cno,
					BufferItem *		paraNode )
    {
    int			part;
    TextParticule *	tp;

    CleanTreeObjects *	cto= cno->cnoTreeObjects;

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind == TPkindOBJECT )
	    { docCloseParticuleObject( cno->cnoDocument, tp );	}
	}

    if  ( paraNode->biParaProperties->ppListOverride > 0		||
	  paraNode->biParaProperties->ppOutlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docRemoveParagraphFromList( paraNode,
					    cto->ctoTree, cno->cnoDocument ) )
	    { LDEB(1);	}

	if  ( paraNode->biParaProperties->ppListOverride > 0 )
	    { cto->ctoBulletsDeleted++;	}
	}

    return;
    }

static void docCleanTreeObjects(
				int *			pBulletsDeleted,
				int *			pParagraphCount,
				struct DocumentTree *	tree,
				struct BufferDocument *	bd )
    {
    if  ( tree->dtRoot )
	{
	docCleanNodeObjects( pBulletsDeleted, pParagraphCount,
					    tree, bd, tree->dtRoot );
	}
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docVisitCleanNode(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					vcno )
    {
    CleanNodeObjects *	cno= (CleanNodeObjects *)vcno;
    CleanTreeObjects *	cto= cno->cnoTreeObjects;

    switch( node->biLevel )
	{
	case DOClevPARA:
	    docCleanParaObjects( cno, node );
	    cto->ctoParagraphCount++;
	    break;

	default:
	    break;
	}

    return SCANadviceOK;
    }

static int docVisitCleanEnterTree(
			struct DocumentTree *		tree,
			const struct BufferItem *	bodySectNode,
			void *				vcno )
    {
    CleanNodeObjects *	cno= (CleanNodeObjects *)vcno;

    if  ( ! docPushCleanTreeObjects( cno, tree ) )
	{ LDEB(1); return -1;	}

    return SCANadviceOK;
    }

static int docVisitCleanLeaveTree(
			struct DocumentTree *		tree,
			const struct BufferItem *	bodySectNode,
			void *				vcno )
    {
    CleanNodeObjects *	cno= (CleanNodeObjects *)vcno;

    if  ( docPopCleanTreeObjects( cno, tree ) )
	{ LDEB(1); return -1;	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docCleanNodeObjects(	int *			pBulletsDeleted,
				int *			pParagraphCount,
				struct DocumentTree *	tree,
				struct BufferDocument *	bd,
				struct BufferItem *	node )
    {
    int			rval= 0;
    const int		flags= FLAGtsSCAN_SECTION_HEADERS_FOOTERS;

    CleanNodeObjects	cno;
    CleanTreeObjects *	cto;

    docInitCleanNodeObjects( &cno );
    cno.cnoDocument= bd;

    cto= docPushCleanTreeObjects( &cno, tree );
    if  ( ! cto )
	{ XDEB(cto); rval= -1; goto ready;	}

    if  ( docScanTreeNode( bd, node,
			    docVisitCleanNode, (NodeVisitor)0,
			    docVisitCleanEnterTree, docVisitCleanLeaveTree,
			    flags, &cno ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    else{
	if  ( pBulletsDeleted )
	    { *pBulletsDeleted += cto->ctoBulletsDeleted;	}
	if  ( pParagraphCount )
	    { *pParagraphCount += cto->ctoParagraphCount;	}
	}

  ready:

    while( cno.cnoTreeObjects )
	{
	if  ( docPopCleanTreeObjects( &cno, cno.cnoTreeObjects->ctoTree ) )
	    { LDEB(1); return -1;	}
	}

    return rval;
    }

/**
 *  Clean the objects in the separators directly attached to the 
 *  document.
 *
 *  TODO: Combine with the tree scanner in the same way as the 
 *  section headers and footers.
 */
void docCleanDocumentObjects(	struct BufferDocument *	bd )
    {
    if  ( bd->bdBody.dtRoot->biChildCount > 0 )
	{
	docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdBody), bd );
	}

    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdFtnsep), bd );
    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdFtnsepc), bd );
    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdFtncn), bd );

    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdAftnsep), bd );
    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdAftnsepc), bd );
    docCleanTreeObjects( (int *)0, (int *)0, &(bd->bdAftncn), bd );

    return;
    }

InsertedObject * docMakeRasterObject(
				int *			pObjectNumber,
				struct BufferDocument *	bd,
				RasterImage *		ri )
    {
    InsertedObject *		rval= (InsertedObject *)0;

    InsertedObject *		io= (InsertedObject *)0;
    int				objectNumber;

    io= docClaimObject( &objectNumber, bd );
    if  ( ! io )
	{ XDEB(io); goto ready;	}

    if  ( docSaveRasterBytesToObject( io, ri ) )
	{ LDEB(1); goto ready;	}

    rval= io; io= (InsertedObject *)0; /* steal */
    *pObjectNumber= objectNumber;

  ready:

    if  ( io )
	{ docDeleteObject( bd, objectNumber );	}

    return rval;
    }

