/************************************************************************/
/*									*/
/*  Generic shape drawing functionality.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<limits.h>

#   include	"docDraw.h"
#   include	<docLayout.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<docObject.h>
#   include	<docTreeNode.h>
#   include	<docShapeGeometry.h>
#   include	<geoRectangleOffsets.h>

#   include	<appDebugon.h>

int docDrawShapeGetFill(	int *			pFill,
				struct RGB8Color *	rgb8,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through )
    {
    int		fill= 1;

    if  ( docShapeGetFill( &fill, rgb8, ds ) )
	{ LDEB(1); return -1;	}

    if  ( fill )
	{ docDrawSetColorRgb( dc, through, rgb8 );	}

    *pFill= fill;
    return 0;
    }

int docDrawShapeGetLine(	int *			pLine,
				struct RGB8Color *	rgb8,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through )
    {
    int		line= 1;

    if  ( docShapeGetLine( &line, rgb8, ds ) )
	{ LDEB(1); return -1;	}

    if  ( line )
	{ docDrawSetColorRgb( dc, through, rgb8 );	}

    *pLine= line;
    return 0;
    }


/************************************************************************/
/*									*/
/*  Traverse a tree of shapes and draw the leaves.			*/
/*									*/
/*  1)  Clip leaf shapes: It is unknown whether chidren can protrude	*/
/*	from their parents. As a matter of fact, it is even unclear	*/
/*	whether a free form can leave its rectangle.			*/
/*									*/
/************************************************************************/

static int docDrawDrawingShape(	const DocumentRectangle *	drOutside,
				const AffineTransform2D *	atOutside,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				through )
    {
    int					rval= 0;
    const struct LayoutContext *	lc= dc->dcLayoutContext;

    AffineTransform2D			atHere;
    DocumentRectangle			drHere;
    DocumentRectangle			drNorm;

    const struct DocumentTree *	saveTree= dc->dcTree;
    const struct BufferItem *	saveBodySectNode= dc->dcBodySectNode;

    docShapeGetRects( &drHere, &drNorm, &atHere, drOutside, atOutside, ds );

    dc->dcBodySectNode= bodySectNode;

    if  ( ds->dsDrawing.sdShapeType == SHPtyGROUP )
	{
	int			child;

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;
	    AffineTransform2D	atChild;

	    geoIdentityAffineTransform2D( &atChild );

	    docShapeGetChildRect( &drChild, &atChild, dsChild,
						&drHere, atOutside, ds );

	    if  ( docDrawDrawingShape( &drChild, &atChild,
						    bodySectNode, page, column,
						    dsChild, dc, through ) )
		{ LDEB(child); rval= -1; goto ready;	}
	    }
	}
    else{
	if  ( dc->dcClipRect )
	    {
	    DocumentRectangle	drPixels;

	    docGetPixelRect( &drPixels, lc, drOutside, page );
	    geoNormalizeRectangle( &drPixels, &drPixels );

	    if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
						dc->dcClipRect, &drPixels ) )
		{ goto ready;	}
	    }

	if  ( dc->dcDrawShape						&&
	      (*dc->dcDrawShape)( drOutside, atOutside, page, ds, dc, through )	)
	    { LDEB(1); rval= -1; goto ready;	}

	docResetDrawingContextState( dc );

	if  ( ds->dsDrawing.sd_WrapText == SHAPEtextWRAP_NONE )
	    { drHere.drX1= INT_MAX;	}

	if  ( ds->dsDocumentTree.dtRoot					&&
	      docDrawShapeText( &drHere, bodySectNode,
					page, column, ds, dc, through )	)
	    { LDEB(page); rval= -1; goto ready;	}
	}

  ready:

    dc->dcBodySectNode= saveBodySectNode;
    dc->dcTree= saveTree;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the text inside a shape.					*/
/*									*/
/************************************************************************/

int docDrawShapeText(	const DocumentRectangle *	drOutside,
			const struct BufferItem *	bodySectNode,
			int				page,
			int				column,
			DrawingShape *			ds,
			DrawingContext *		dc,
			void *				through )
    {
    const struct LayoutContext *	lc= dc->dcLayoutContext;

    BlockOrigin				bo ;
    LayoutPosition			lpBelow;

    RectangleOffsets			padding;
    DocumentRectangle			drInside;

    docInitBlockOrigin( &bo );
    docInitLayoutPosition( &lpBelow );

    docShapeGetPadding( &padding, drOutside, &(ds->dsDrawing) );
    geoRectangleSubtractPadding( &drInside, drOutside, &padding );

    if  ( docShapeCheckTextLayout( ds, &drInside, (DocumentRectangle *)0,
				bodySectNode, lc, dc->dcStartTreeLayout ) )
	{ LDEB(page); return -1;	}

    bo.boXShift= drOutside->drX0;
    bo.boYShift= drOutside->drY0;

    lpBelow= ds->dsDocumentTree.dtRoot->biTopPosition;
    if  ( docDrawGroupNode( &lpBelow,
				ds->dsDocumentTree.dtRoot, through, dc, &bo ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the root shape of an object. I.E: The shape that is directly	*/
/*  inserted in the document. Its children are reached through		*/
/*  recursion.								*/
/*									*/
/************************************************************************/

int docDrawShape(	DrawingContext *		dc,
			void *				through,
			const struct BufferItem *	bodySectNode,
			const struct DocumentRectangle * drOutside,
			const InsertedObject *		io )
    {
    const struct LayoutContext * lc= dc->dcLayoutContext;
    DrawingShape *		ds= io->ioDrawingShape;

    int				page= io->ioY0Position.lpPage;
    int				column= io->ioY0Position.lpColumn;

    AffineTransform2D		at;

    geoIdentityAffineTransform2D( &at );

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drPixels;

	docGetPixelRect( &drPixels, lc, drOutside, page );
	geoNormalizeRectangle( &drPixels, &drPixels );

	if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					    &drPixels, dc->dcClipRect ) )
	    { return 0;	}
	}

    docDrawDrawingShape( drOutside, &at, bodySectNode, page, column,
							ds, dc, through );
    return 0;
    }

