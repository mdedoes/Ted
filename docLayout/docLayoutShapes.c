/************************************************************************/
/*									*/
/*  Layout utility functions relating to DrawingShapes			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<docPageGrid.h>
#   include	<docTreeNode.h>
#   include	<docShape.h>
#   include	"docLayout.h"
#   include	"docLayoutDocumentTree.h"
#   include	<docFrameProperties.h>
#   include	<docParaProperties.h>
#   include	<docSectProperties.h>
#   include	<docStripFrame.h>
#   include	<docBlockFrame.h>
#   include	<docObject.h>
#   include	<docShapeGeometry.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Remember that the layout calculations for a shape must be redone.	*/
/*									*/
/************************************************************************/

void docDrawingShapeInvalidateTextLayout(	DrawingShape *	ds )
    {
    int		child;

    docInvalidateTreeLayout( &(ds->dsDocumentTree) );

    for ( child= 0; child < ds->dsChildCount; child++ )
	{ docDrawingShapeInvalidateTextLayout( ds->dsChildren[child] ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the page rectangle for a shape.				*/
/*									*/
/************************************************************************/

void docShapePageY(	LayoutPosition *		lpShapeTop,
			LayoutPosition *		lpBelowShape,
			BlockFrame *			bfShape,
			const DrawingShape *		ds,
			const struct BufferItem *	paraNode,
			const LayoutPosition *		lpLineTop,
			const BlockFrame *		bf )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				yRefProp= sp->spYReference;

    DocumentRectangle		drShape;
    int				shapeHigh;

    docPlaceRootShapeRect( &drShape, &(ds->dsShapeProperties), 0, 0 );
    shapeHigh= drShape.drY1= drShape.drY0+ 1;

    if  ( yRefProp == FYrefIGNORE )
	{ yRefProp= ds->dsDrawing.sdYReference; }

    *lpShapeTop= *lpLineTop;

    docLayoutFrameY( bfShape,
			yRefProp, ds->dsDrawing.sdYPosition, sp->spRect.drY0,
			lpLineTop, &(paraNode->biTopPosition),
			bf, -shapeHigh, shapeHigh );

    lpShapeTop->lpPageYTwips= bfShape->bfContentRect.drY0;

    *lpBelowShape= *lpShapeTop;
    lpBelowShape->lpPageYTwips += shapeHigh;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle for a shape that is not embedded in the	*/
/*  text line. Shapes that are embedded in the text line are handled	*/
/*  as part of the normal text flow of the paragraph.			*/
/*									*/
/************************************************************************/

void docShapePageRectangle(	LayoutPosition *		lpShapeTop,
				LayoutPosition *		lpBelowShape,
				int *				pX0,
				int *				pX1,
				const DrawingShape *		ds,
				const struct BufferItem *	paraNode,
				const LayoutPosition *		lpLineTop,
				const ParagraphFrame *		pfRef,
				const BlockFrame *		bfRef,
				int				xChar )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				xRefProp= sp->spXReference;

    DocumentRectangle		drShape;
    int				shapeWide;
    BlockFrame			bfShape;

    docLayoutInitBlockFrame( &bfShape );

    docPlaceRootShapeRect( &drShape, &(ds->dsShapeProperties), 0, 0 );
    shapeWide= drShape.drX1- drShape.drX0+ 1;

    docShapePageY( lpShapeTop, lpBelowShape, &bfShape,
					    ds, paraNode, lpLineTop, bfRef );

    if  ( xRefProp == FXrefIGNORE )
	{ xRefProp= ds->dsDrawing.sdXReference; }

    docLayoutFrameX( &bfShape,
	    xRefProp, ds->dsDrawing.sdXPosition, sp->spRect.drX0,
	    paraNode->biParaProperties->ppTableNesting > 0 &&
				    ds->dsDrawing.sd_fLayoutInCell,
	    pfRef, bfRef, xChar, shapeWide );

    *pX0= bfShape.bfContentRect.drX0;
    *pX1= bfShape.bfContentRect.drX0+ shapeWide;

    docLayoutCleanBlockFrame( &bfShape );

    return;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout of a text box (shptxt) if necessary.		*/
/*									*/
/************************************************************************/

int docShapeCheckTextLayout(
			DrawingShape *			ds,
			const DocumentRectangle *	drInside,
			DocumentRectangle *		drChanged,
			const struct BufferItem *	bodySectNode,
			const struct LayoutContext *	lc,
			START_TREE_LAYOUT		startTreeLayout )
    {
    /*  We do not expect the tree to change height here	*/
    const int		adjustDocument= 0;
    DocumentTree *	tree= &(ds->dsDocumentTree);
    BufferItem *	treeRoot= tree->dtRoot;

    DocumentGeometry	dgBox;

    const int		page= 0;
    const int		column= 0;

    geoInitDocumentGeometry( &dgBox );

    docShapeRootGeometry( &dgBox, &(ds->dsDrawing), drInside );

    ds->dsDocumentTree.dtRoot->biSectDocumentGeometry= dgBox;

    if  ( docLayoutDocumentTree( tree,
		drChanged, page, column, dgBox.dgMargins.roTopOffset,
		bodySectNode, lc, startTreeLayout, adjustDocument ) )
	{ LLDEB(page,column); return -1;	}

    treeRoot->biTopPosition.lpPage= page;
    treeRoot->biTopPosition.lpColumn= column;
    treeRoot->biTopPosition.lpPageYTwips= tree->dtY0UsedTwips;

    treeRoot->biBelowPosition.lpPage= page;
    treeRoot->biBelowPosition.lpColumn= column;
    treeRoot->biBelowPosition.lpPageYTwips= tree->dtY1UsedTwips;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle of a shape that adapts to to its text.	*/
/*  The (buggy) behavior of MS-Word is as follows:			*/
/*									*/
/*  Take the width of the shape and adapt the height to the height that	*/
/*  the texts gets at that height.					*/
/*									*/
/*  The BUG in word is that initially it uses the height from the file,	*/
/*  rather than that it determines the height when the file is read. It	*/
/*  does however adapt the height of the shape when the height of its	*/
/*  contents changes.							*/
/*									*/
/************************************************************************/

int docShapePrelayout(	InsertedObject *		io,
			const struct BufferItem *	bodySectNode,
			const struct LayoutContext *	lc )
    {
    DrawingShape *	ds= io->ioDrawingShape;
    ShapeProperties *	sp= &(ds->dsShapeProperties);
    ShapeDrawing *	sd= &(ds->dsDrawing);

    struct BufferItem *	textRoot= ds->dsDocumentTree.dtRoot;

    if  ( textRoot )
	{
	const int		recursively= 1;

	/*TODO: Should not be necessary */
	docDelimitTables( textRoot, recursively );
	}

    if  ( sd->sd_fPseudoInline )
	{
	DocumentRectangle	drOutside;

	docPlaceRootShapeRect( &drOutside, sp, 0, 0 );

	if  ( textRoot && sd->sd_fFitShapeToText )
	    {
	    int			round;

	    for ( round= 0; round < 3; round++ )
		{
		const int		page= 0;

		RectangleOffsets	padding;
		DocumentRectangle	drInside;

		docShapeGetPadding( &padding, &drOutside, sd );
		geoRectangleSubtractPadding( &drInside, &drOutside, &padding );

		if  ( docShapeCheckTextLayout( ds,
				&drInside, (DocumentRectangle *)0,
				bodySectNode, lc, (START_TREE_LAYOUT)0 ) )
		    { LDEB(page); return -1;	}

		drInside.drY0= textRoot->biTopPosition.lpPageYTwips-
					EMUtoTWIPS( sd->sd_dyTextTop );
		drInside.drY1= textRoot->biBelowPosition.lpPageYTwips+
					EMUtoTWIPS( sd->sd_dyTextBottom );

		geoRectangleAddMargins( &drOutside, &drInside, &padding );
		}
	    }

	io->ioInline= 1;
	io->ioScaleYSet= 100;
	io->ioScaleYUsed= 100;
	io->ioTwipsWide= drOutside.drX1- drOutside.drX0+ 1;
	io->ioTwipsHigh= drOutside.drY1- drOutside.drY0+ 1;

	io->ioPictureProperties.pipScaleYSet= io->ioScaleYSet;
	io->ioPictureProperties.pipScaleYUsed= io->ioScaleYUsed;
	io->ioPictureProperties.pipTwipsWide= io->ioTwipsWide;
	io->ioPictureProperties.pipTwipsHigh= io->ioTwipsHigh;

	sp->spRect.drY1= sp->spRect.drY0+ io->ioTwipsHigh;
	}

    return 0;
    }

