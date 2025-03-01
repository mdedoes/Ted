/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docParticuleData.h"
#   include	"docLayoutLine.h"

#   include	<docObjectProperties.h>
#   include	<docShape.h>
#   include	<docObject.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docBuf.h>
#   include	<docStripFrame.h>
#   include	<docBlockFrame.h>
#   include	<docObjects.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculations on line inserts.					*/
/*									*/
/************************************************************************/

static int docPlaceShape(	const struct BufferItem *	paraNode,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const ParagraphFrame *	pf,
				const BlockFrame *	bf,
				int			xChar )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    int				x1;
    LayoutPosition		lpBelowShape;

    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docShapePageRectangle( &(io->ioY0Position), &lpBelowShape,
				&(io->ioX0Twips), &x1,
				ds, paraNode, lpLineTop, pf, bf, xChar );

    docDrawingShapeInvalidateTextLayout( ds );

    return 0;
    }

static int docPlaceShapeY(	const struct BufferItem *	paraNode,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const BlockFrame *	bf )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    LayoutPosition		lpBelowShape;

    BlockFrame			bfShape;

    docLayoutInitBlockFrame( &bfShape );

    docShapePageY( &(io->ioY0Position), &lpBelowShape, &bfShape,
						ds, paraNode, lpLineTop, bf );

    docDrawingShapeInvalidateTextLayout( ds );

    docLayoutCleanBlockFrame( &bfShape );

    return 0;
    }

int docPlaceLineInserts(	struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				const TextLine *		tl,
				const ParticuleData *		pd,
				const ParagraphFrame *		pf,
				const BlockFrame *		bf,
				const LayoutPosition *		lp )
    {
    int				i;
    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ tl->tlFirstParticule;
    for ( i= 0; i < tl->tlParticuleCount; pd++, tp++, i++ )
	{
	if  ( tp->tpKind == TPkindFIELDHEAD )
	    {
	    if  ( docSetPageOfField( &(bd->bdFieldList),
					tp->tpObjectNumber,  lp->lpPage ) )
		{ LDEB(lp->lpPage); docListNode(bd,0,paraNode,0);	}
	    }

	if  ( tp->tpKind == TPkindOBJECT )
	    {
	    InsertedObject *	io;

	    io= docGetObject( bd, tp->tpObjectNumber );
	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io);	}

	    if  ( io && io->ioKind == DOCokDRAWING_SHAPE )
		{
		const DrawingShape *	ds= io->ioDrawingShape;
		int			inLine= ds->dsDrawing.sd_fPseudoInline;

		if  ( ! inLine &&
		      docPlaceShape( paraNode, &(tl->tlTopPosition),
						    io, pf, bf, pd->pdX0 ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

int docPlaceLineInsertsY(struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				const TextLine *		tl,
				const BlockFrame *		bf,
				const LayoutPosition *		lp )
    {
    int				i;
    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ tl->tlFirstParticule;
    for ( i= 0; i < tl->tlParticuleCount; tp++, i++ )
	{
	if  ( tp->tpKind == TPkindFIELDHEAD )
	    {
	    docSetPageOfField( &(bd->bdFieldList),
					tp->tpObjectNumber,  lp->lpPage );
	    }

	if  ( tp->tpKind == TPkindOBJECT )
	    {
	    InsertedObject *	io;

	    io= docGetObject( bd, tp->tpObjectNumber );
	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io);	}

	    if  ( io && io->ioKind == DOCokDRAWING_SHAPE )
		{
		const DrawingShape *	ds= io->ioDrawingShape;
		int			inLine= ds->dsDrawing.sd_fPseudoInline;

		if  ( ! inLine &&
		      docPlaceShapeY( paraNode, &(tl->tlTopPosition), io, bf ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

