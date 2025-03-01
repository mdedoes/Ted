#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	<layoutContext.h>

#   include	<docObjectProperties.h>
#   include	<docShape.h>
#   include	<docObject.h>
#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<docShapeGeometry.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw shapes that are not part of the normal text flow of the	*/
/*  document. Shapes that are part of the flow are drawn along with the	*/
/*  other particules in the line.					*/
/*									*/
/************************************************************************/

typedef struct DrawPageShapes
    {
    DrawingContext *	dpsDrawingContext;
    void *		dpsThrough;
    int			dpsPage;
    int			dpsBelowText;
    } DrawPageShapes;

/************************************************************************/
/*									*/
/*  Draw the shape that belongs to an object if it is on the current	*/
/*  page. [Inline objects are skipped.]					*/
/*									*/
/************************************************************************/

static int docDrawShapesVisitObject(	int		n,
					void *		vio,
					void *		vdps )
    {
    InsertedObject *		io= (InsertedObject *)vio;
    const DrawPageShapes *	dps= (const DrawPageShapes *)vdps;
    DrawingContext *		dc= dps->dpsDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    DrawingShape *		ds;
    const ShapeProperties *	sp;

    DocumentRectangle		drOutside;

    struct BufferItem *		bodySectNode;

    if  ( io->ioKind != DOCokDRAWING_SHAPE		||
	  io->ioY0Position.lpPage != dps->dpsPage	)
	{ return 0;	}

    ds= io->ioDrawingShape;
    sp= &(ds->dsShapeProperties);

    /* Inline: do not draw it here */
    if  ( ds->dsDrawing.sd_fPseudoInline )
	{ return 0;	}

    /* Not part of this round */
    if  ( sp->spShapeBelowText != dps->dpsBelowText )
	{ return 0;	}

    bodySectNode= docGetBodySectNodeOfScope( &(ds->dsSelectionScope),
							    lc->lcDocument );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); return -1;	}

    docPlaceRootShapeRect( &drOutside, &(io->ioDrawingShape->dsShapeProperties),
				io->ioX0Twips, io->ioY0Position.lpPageYTwips );

    if  ( docDrawShape( dc, dps->dpsThrough, bodySectNode, &drOutside, io ) )
	{ LDEB(n);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the shapes for this page					*/
/*									*/
/************************************************************************/

int docDrawShapesForPage(		void *			through,
					DrawingContext *	dc,
					int			belowText,
					int			page )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    struct BufferDocument *	bd= lc->lcDocument;

    DrawPageShapes		dps;

    dps.dpsDrawingContext= dc;
    dps.dpsThrough= through;
    dps.dpsPage= page;
    dps.dpsBelowText= belowText;

    if  ( utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					docDrawShapesVisitObject, &dps ) < 0 )
	{ LDEB(page); return -1;	}

    return 0;
    }

