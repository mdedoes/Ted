/************************************************************************/
/*									*/
/*  Screen Drawing functionality: Utility functions.			*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	"docScreenDraw.h"
#   include	<docLayout.h>
#   include	"docResizeHandles.h"
#   include	<layoutContext.h>
#   include	<docDocumentTree.h>
#   include	<docTreeNode.h>
#   include	<docScreenLayout.h>
#   include	"docSelectionGeometry.h"
#   include	<geo2DInteger.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Restore/install original clip rect.					*/
/*									*/
/************************************************************************/

void docOriginalClipping(	DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drRestore;

	drRestore= *(dc->dcClipRect);
	geoShiftRectangle( &drRestore, -lc->lcOx, -lc->lcOy );

	drawSetClipRect( lc->lcDrawingSurface, &drRestore );
	}
    else{
	drawNoClipping( lc->lcDrawingSurface );
	}
    }

/************************************************************************/
/*									*/
/*  Draw the white rectangle for a page.				*/
/*									*/
/************************************************************************/

void docScreenDrawPageRect(	ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				int				page )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    DocumentRectangle		drPage;

    docGetPageRectPixels( &drPage, lc, page );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drPage, &drPage, dc->dcClipRect )	)
	{ return;	}

    geoShiftRectangle( &drPage, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drPage );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw an I Bar.							*/
/*									*/
/************************************************************************/

void docScreenGetIBarRect(	DocumentRectangle *		drPixels,
				const PositionGeometry *	pg,
				const LayoutContext *		lc )
    {
    docGetPixelRectForPos( drPixels, lc,
		    pg->pgXTwips, pg->pgXTwips,
		    &(pg->pgTopPosition), &(pg->pgBelowLinePosition) );

    /* Do not trust XTwips yet */
    drPixels->drX0= drPixels->drX1= pg->pgXPixels;

    return;
    }

int docScreenDrawIBar(	const DocumentRectangle *	drPixels,
			const LayoutContext *		lc )
    {
    DocumentRectangle	drShifted= *drPixels;

    geoShiftRectangle( &drShifted, -lc->lcOx, -lc->lcOy );

    drawFillRectangle( lc->lcDrawingSurface, &drShifted );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the eight blocks around a selected object.			*/
/*									*/
/************************************************************************/

void docScreenDrawObjectBlocks(	const DocumentRectangle *	drObj,
				const Point2DI *		xp,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				i;

    DocumentRectangle		drBox;

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddHighlightColor) );

    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	drBox.drX0= xp[i].x+ 1;
	drBox.drY0= xp[i].y+ 1;
	drBox.drX1= drBox.drX0+ RESIZE_BLOCK- 1;
	drBox.drY1= drBox.drY0+ RESIZE_BLOCK- 1;

	geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
	drawFillRectangle( lc->lcDrawingSurface, &drBox );
	}

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddForeColor) );
    drawSetLineAttributes( lc->lcDrawingSurface,
		    1, LineStyleSolid, LineCapButt, LineJoinMiter,
		    (const unsigned char *)0, 0 );

    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	drBox.drX0= xp[i].x+ 1;
	drBox.drY0= xp[i].y+ 1;
	drBox.drX1= drBox.drX0+ RESIZE_BLOCK- 2;
	drBox.drY1= drBox.drY0+ RESIZE_BLOCK- 2;

	geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
	drawRectangle( lc->lcDrawingSurface, &drBox );
	}

    drBox= *drObj;

    geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
    drawRectangle( lc->lcDrawingSurface, &drBox );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the box around an active footnote, header or footer.		*/
/*									*/
/************************************************************************/

static void docScreenDrawTreeBox( ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentRectangle *	drBox )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    static unsigned char	dot[]= { 1, 2 };
    RGB8Color			rgb8;

    utilRGB8SolidBlack( &rgb8 );

    drawSetLineAttributes( lc->lcDrawingSurface,
		    1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
		    dot, sizeof( dot ) );

    docDrawSetColorRgb( dc, (void *)sdd, &rgb8 );

    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY0- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY1- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY1- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX0- lc->lcOx, drBox->drY1- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX1- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY1- lc->lcOy );

    }

int docScreenDrawBoxAroundTree(
			    const struct BufferItem *	bodySectNode,
			    const struct DocumentTree *	tree,
			    ScreenDrawingData *		sdd,
			    DrawingContext *		dc )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    
    DocumentRectangle		drIntersect;
    DocumentRectangle		drBox;

    const int			justUsed= 0;

    if  ( tree->dtPageSelectedUpon < 0 || tree->dtColumnSelectedIn < 0 )
	{
	LLDEB(tree->dtPageSelectedUpon,tree->dtColumnSelectedIn);
	SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
	return -1;
	}

    if  ( docGetBoxAroundTree( &drBox, tree, justUsed,
		    tree->dtPageSelectedUpon, tree->dtColumnSelectedIn, lc ) )
	{ LDEB(1); return -1;	}

    if  ( ! geoIntersectRectangle( &drIntersect, &drBox, dc->dcClipRect ) )
	{ return 0;	}

    docScreenDrawTreeBox( sdd, dc, &drBox );

    return 0;
    }

int docScreenDrawCheckPageOfSelectedTree(
			    struct SelectionGeometry *		sg,
			    struct BufferItem **		pBodySectNode,
			    const struct DocumentSelection *	ds,
			    struct DocumentTree *		selRootTree,
			    const LayoutContext *		lc )
    {
    int			changed= 0;
    DocumentRectangle	drChanged;

    struct BufferItem *	bodySectNode;

    geoInitRectangle( &drChanged );

    if  ( docCheckPageOfSelectedTree( &changed, &bodySectNode, &drChanged,
			selRootTree, lc, docStartScreenLayoutForTree ) )
	{ LDEB(1); return -1;	}

    if  ( changed )
	{
	const int		lastLine= 1;

	docSelectionGeometry( sg, ds, bodySectNode, lastLine, lc );
	}

    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a page: Draw an external tree box if needed.			*/
/*									*/
/*  1)  Selection not in an external tree?				*/
/*  2)  In range? covers the case where both numbers are -1 because	*/
/*	nothing needs to be done.					*/
/*  3)  Calculate box on this page.					*/
/*  4)  Intersects clip rectangle?					*/
/*  5)  Draw!								*/
/*									*/
/************************************************************************/

static int docScreenDrawStartPage(
				void *				vsdd,
				const struct DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				const char *			why,
				int				page )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    struct DocumentTree *	selRootTree= sdd->sddSelRootTree;
    struct BufferItem *		selRootNode= sdd->sddSelRootNode;

    const LayoutContext *	lc= dc->dcLayoutContext;

    const int			justUsed= 0;
    DocumentRectangle		drBox;
    DocumentRectangle		drIntersect;

    /*  1  */
    if  ( ! selRootTree || ! selRootNode )
	{ return 0;	}

    /*  2  */
    if  ( sdd->sddBoxFirstPage > page		||
	  sdd->sddBoxLastPage < page		)
	{ return 0;	}

    /*  3  */
    if  ( docGetBoxAroundTree( &drBox, selRootTree,
					justUsed,
					selRootTree->dtPageSelectedUpon,
					selRootTree->dtColumnSelectedIn,
					lc ) )
	{ LDEB(1); return 0;	}

    /*  4  */
    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drIntersect, &drBox, dc->dcClipRect ) )
	{ return 0;	}

    /*  5  */
    docScreenDrawTreeBox( sdd, dc, &drBox );

    return 0;
    }

/************************************************************************/

void docScreenDrawBackground(		DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const RGB8Color *	bgColor )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    DocumentRectangle		drFill= *(dc->dcClipRect);

    int				page;

    docDrawSetColorRgb( dc, (void *)sdd, bgColor );

    geoShiftRectangle( &drFill, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drFill );

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddBehindColor) );

    for ( page= dc->dcFirstPage; page <= dc->dcLastPage; page++ )
	{ docScreenDrawPageRect( sdd, dc, page );	}
    }

void docScreenDrawSetFunctions(		DrawingContext *	dc )
    {
    dc->dcSetColorRgb= docScreenDrawSetColorRgb;
    dc->dcSetFont= docScreenDrawSetFont;
    dc->dcDrawShape= docScreenDrawDrawingShape;
    dc->dcDrawInlineObject= docScreenDrawInlineObject;
    dc->dcDrawTab= docScreenDrawTab;
    dc->dcDrawFtnsep= docScreenDrawFtnsep;
    dc->dcDrawTextRun= docScreenDrawTextRun;
    dc->dcDrawUnderline= docScreenDrawRunUnderline;
    dc->dcDrawStrikethrough= docScreenDrawRunStrikethrough;

    dc->dcStartTextLine= docScreenDrawStartTextLine;
    dc->dcDrawOrnaments= docScreenDrawOrnaments;
    dc->dcStartPage= docScreenDrawStartPage;
    dc->dcStartTreeLayout= docStartScreenLayoutForTree;

    return;
    }
