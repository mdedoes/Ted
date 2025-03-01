/************************************************************************/
/*									*/
/*  Draw the selection on screen.					*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docScreenDraw.h"
#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<layoutContext.h>
#   include	<docSelect.h>
#   include	<docScanner.h>
#   include	<docTreeNode.h>
#   include	<docParaNode.h>
#   include	"docLineRectangles.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docTextLineSelectedDrawRectangle(
				const DocumentRectangle *	dr,
				struct BufferItem *		paraNode,
				void *				vdtl )
    {
    struct DrawTextLine *	dtl= (struct DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    drawFillRectangle( lc->lcDrawingSurface, dr );

    return SCANadviceOK;
    }

static int docStartTextLineSelected(
				struct DrawTextLine *		dtl,
				int				x0Twips )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;

    int				res;

    LineRectangles		lr;
    CollectRectangles		cr;

    DocumentRectangle		drRedraw;

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
		dc->dcClipRect, &(dtl->dtlLineFrameRectanglePixels) )	)
	{ return SCANadviceSKIP;	}

    dtl->dtlDrawParticulesSeparately= dc->dcDrawParticulesSeparately ||
	    docScreenDrawParticulesSeparately( dtl->dtlParaNode, dtl->dtlTextLine );

    docSetLineRectangles( &lr, lc, dc->dcSelection,
					dtl->dtlParaNode, dtl->dtlTextLine,
					dtl->dtlShiftedBaselinePixels,
					&(dtl->dtlLineFrameRectanglePixels) );

    if  ( ! lr.lrIntersectsSelection )
	{ /*LDEB(1);*/ return SCANadviceSKIP;	}

    docInitCollectRectangles( &cr );

    res= docTextLineSelectedCollectRectangles( &lr, x0Twips,
				dtl->dtlDrawParticulesSeparately, &cr );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    switch( cr.crCount )
	{
	case 0:
	    if  ( dtl->dtlParaNode->biParaLineCount != 1 )
		{ break;	}

	    /*  Empty paragraph at either end of the selection  */
	    drRedraw= *(lr.lrDrThisLineFrame);

	    if  ( lr.lrCmp_ShLh >= 0 )
		{
		drRedraw.drX0= docLayoutXPixels( lc, x0Twips );
		}

	    if  ( lr.lrCmp_StLt <= 0 )
		{
		drRedraw.drX1= drRedraw.drX0+ 3;
		}

	    if  ( ! geoIntersectRectangle( &drRedraw, &drRedraw,
							dc->dcClipRect ) )
		{ break;	}

	    geoShiftRectangle( &drRedraw, -lc->lcOx, -lc->lcOy );
	    drawSetClipRect( lc->lcDrawingSurface, &drRedraw );

	    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddHighlightColor) );
	    drawFillRectangle( lc->lcDrawingSurface, &drRedraw );
	    break;

	case 1:
	    drRedraw= cr.crUnion;

	    if  ( ! geoIntersectRectangle( &drRedraw, &drRedraw,
							dc->dcClipRect ) )
		{ break;	}

	    geoShiftRectangle( &drRedraw, -lc->lcOx, -lc->lcOy );
	    drawSetClipRect( lc->lcDrawingSurface, &drRedraw );

	    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddHighlightColor) );
	    drawFillRectangle( lc->lcDrawingSurface, &drRedraw );
	    break;

	default:
	    drRedraw= cr.crUnion;

	    if  ( ! geoIntersectRectangle( &drRedraw, &drRedraw,
							dc->dcClipRect ) )
		{ break;	}

	    geoShiftRectangle( &drRedraw, -lc->lcOx, -lc->lcOy );
	    drawSetClipRect( lc->lcDrawingSurface, &drRedraw );

	    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddBehindColor) );
	    drawFillRectangle( lc->lcDrawingSurface, &drRedraw );

	    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddHighlightColor) );

	    res= docForAllLineRectangles( &lr, x0Twips,
			dtl->dtlDrawParticulesSeparately,
			docTextLineSelectedDrawRectangle, (void *)dtl );
	    break;
	}

    return SCANadviceOK;
    }

static int docFinishTextLineSelected(
				const struct DrawTextLine *	dtl,
				int				x1Twips )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;

    docOriginalClipping( dc, sdd );

    return 0;
    }

/************************************************************************/

int docScreenDrawSelection(
			struct BufferItem *		selRootNode,
			const struct BufferItem *	bodySectNode,
			ScreenDrawingData *		sdd,
			DrawingContext *		dc,
			const DocumentSelection *	ds,
			int				headPosFlags,
			int				tailPosFlags,
			int				tableRect )
    {
    int				rval= 0;

    LayoutPosition		lpBelow;

    const struct DocumentTree *	saveTree= dc->dcTree;
    const struct BufferItem *	saveBodySectNode= dc->dcBodySectNode;

    dc->dcStartTextLine= docStartTextLineSelected;
    dc->dcFinishTextLine= docFinishTextLineSelected;

    /*
    dc->dcDrawObject= docScreenDrawObjectSelected;
    dc->dcDrawTab= docScreenDrawTabSelected;
    dc->dcDrawFtnsep= docScreenDrawFtnsepSelected;
    dc->dcDrawTextRun= docScreenDrawTextRunSelected;
    */

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddForeColor) );

    dc->dcDrawOrnaments= (DRAW_ORNAMENTS)0;
    dc->dcFinishPage= (FINISH_PAGE)0;
    dc->dcStartPage= (START_PAGE)0;

    dc->dcStartTreeLayout= (START_TREE_LAYOUT)0;
    dc->dcDrawOtherTrees= 0;
    dc->dcPostponeHeadersFooters= 0;

    dc->dcSelection= ds;
    dc->dcSelectionHeadPositionFlags= headPosFlags;
    dc->dcSelectionTailPositionFlags= tailPosFlags;
    dc->dcSelectionIsTableRectangle= tableRect;

    docInitLayoutPosition( &lpBelow );

    dc->dcBodySectNode= bodySectNode;

    if  ( docDrawNode( &lpBelow, selRootNode, (void *)sdd, dc ) )
	{ LDEB(1); rval= -1;	}

    dc->dcBodySectNode= saveBodySectNode;
    dc->dcTree= saveTree;

    return rval;
    }

