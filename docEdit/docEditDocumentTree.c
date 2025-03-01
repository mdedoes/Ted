/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docShape.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	"docCopyNode.h"
#   include	"docEditImpl.h"

/************************************************************************/
/*									*/
/*  Copy a shape.							*/
/*									*/
/*  1)  Children.							*/
/*  2)  Vertices.							*/
/*  2)  Wrap polygon.							*/
/*  4)  Segment information						*/
/*  5)  Text box content.						*/
/*									*/
/************************************************************************/

DrawingShape * docCopyDrawingShape(
				DocumentCopyJob *	dcj,
				DrawingShape *		from )
    {
    DrawingShape *		rval= (DrawingShape *)0;
    DrawingShape *		to;
    int				saveNumber;

    to= docClaimDrawingShape(
		    &(dcj->dcjEditOperation->eoDocument->bdShapeList) );
    if  ( ! to )
	{ XDEB(to); goto ready;	}

    saveNumber= to->dsShapeNumber;
    *to= *from;
    docInitShapeAllocated( to );
    to->dsShapeNumber= saveNumber;

    /*  1  */
    to->dsChildren= (DrawingShape **)malloc(
				from->dsChildCount* sizeof(DrawingShape *) );
    if  ( ! to->dsChildren )
	{ LXDEB(from->dsChildCount,to->dsChildren); goto ready;	}

    /*  2,3,4  */
    if  ( docCopyShapeDrawing( &(to->dsDrawing), &(from->dsDrawing) ) )
	{ LDEB(1); goto ready;	}

    /*  1  */
    while( to->dsChildCount < from->dsChildCount )
	{
	to->dsChildren[to->dsChildCount]= docCopyDrawingShape( dcj,
					from->dsChildren[to->dsChildCount] );
	if  ( ! to->dsChildren[to->dsChildCount] )
	    {
	    LXDEB(to->dsChildCount,to->dsChildren[to->dsChildCount]);
	    goto ready;
	    }
	to->dsChildCount++;
	}

    /*  5  */
    to->dsSelectionScope= dcj->dcjTargetSelectionScope;
    to->dsSelectionScope.ssTreeType= from->dsSelectionScope.ssTreeType;
    to->dsSelectionScope.ssOwnerNumber= to->dsShapeNumber;
    if  ( from->dsDocumentTree.dtRoot )
	{
	if  ( docCopyDocumentTree( dcj, &(to->dsDocumentTree),
			    &(to->dsSelectionScope), &(from->dsDocumentTree) ) )
	    { XDEB(to->dsDocumentTree.dtRoot); goto ready;	}
	}

    rval= to; to= (DrawingShape *)0; /* steal */

  ready:

    if  ( to )
	{
	EditOperation *	eo= dcj->dcjEditOperation;

	docDeleteDrawingShape( eo->eoDocument, to );
	}

    return rval;
    }

/************************************************************************/

struct DocumentTree * docEditInsertHeaderFooter(
					EditOperation *		eo,
					int			treeType,
					int			textAttrNr,
					int			refPage )
    {
    struct DocumentTree *	dtHdFt;
    struct BufferItem *		bodySectNode;

    struct BufferItem *		treeParaNode;
    const int			ownerNumber= -1;

    DocumentPosition		dpNew;

    int				page;
    const int			column= 0;

    /*  2  */
    if  ( docGetHeaderFooter( &dtHdFt, &bodySectNode, &(eo->eoHeadDp),
						eo->eoDocument, treeType ) )
	{ LDEB(treeType); return (struct DocumentTree *)0;	}

    treeParaNode= docStartDocumentTree( eo->eoDocument, dtHdFt, treeType,
			bodySectNode, ownerNumber, textAttrNr );
    if  ( ! treeParaNode )
	{ XDEB(treeParaNode); return (struct DocumentTree *)0;	}

    /*  4  */
    if  ( docHeadPosition( &dpNew, treeParaNode ) )
	{ LDEB(1); return (struct DocumentTree *)0;	}
    docAvoidParaHeadField( &dpNew, (int *)0, eo->eoDocument );

    page= docHeaderFooterPage( eo->eoDocument, bodySectNode,
							refPage, treeType );
    if  ( page < 0 )
	{ LDEB(page); return (struct DocumentTree *)0;	}

    docInvalidateTreeLayout( dtHdFt );

    dtHdFt->dtPageSelectedUpon= page;
    dtHdFt->dtColumnSelectedIn= column;

    eo->eoTree= dtHdFt;
    docGetSelectionScope( &(eo->eoSelectionScope), bodySectNode );
    eo->eoReformatNeeded= REFORMAT_BODY_SECT;

    /*  5  */
    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    docSetIBarRange( &(eo->eoSelectedRange), &dpNew );

    return dtHdFt;
    }

