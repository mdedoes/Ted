/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>

#   include	"tedSelect.h"
#   include	<docDraw.h>
#   include	"tedDraw.h"
#   include	<docScreenDraw.h>
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	<docDocumentTree.h>
#   include	<docBuf.h>
#   include	<geo2DInteger.h>
#   include	<layoutContext.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

void tedExposeCurrentSelection(	const EditDocument *			ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SelectionGeometry *		sg= &(td->tdSelectionGeometry);

    int				scrolledX= 0;
    int				scrolledY= 0;

    appDocExposeRectangle( ed, &(sg->sgRectangle), scrolledX, scrolledY );
    }

void tedExposeWholeDocument(	EditDocument *			ed )
    {
    appDocExposeRectangle( ed, (const DocumentRectangle *)0, 0, 0 );
    }

/************************************************************************/
/*									*/
/*  Blinking cursor.							*/
/*									*/
/*  1)  Turned off when we are debugging exposures and redraws.		*/
/*									*/
/************************************************************************/

void tedUndrawIBar(	const EditDocument *	ed )
    {
#   if BLINK_IBAR

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

#   if LOG_REDRAWS
    geoLogRectangle( "BLINK*", &(sg.sgRectangle) );
#   endif

    tedExposeCurrentSelection( ed );

#   endif

    return;
    }

/************************************************************************/

static int tedDrawSelectionHiglight(
				TedDocument *			td,
				ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				SelectionGeometry *		sg,
				const RGB8Color *		highlightColor )
    {
    if  ( td->tdDrawMonochrome )
	{
	utilRGB8SolidWhite( &(sdd->sddForeColor) );
	utilRGB8SolidBlack( &(sdd->sddHighlightColor) );
	}
    else{
	sdd->sddHighlightColor= *highlightColor;
	}

    if  ( docScreenDrawSelection( sdd->sddSelRootNode,
					    sdd->sddSelRootBodySectNode,
					    sdd, dc, ds,
					    sg->sgHead.pgPositionFlags,
					    sg->sgTail.pgPositionFlags,
					    sd->sdInOneTable ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int tedDrawSelection(	TedDocument *			td,
				ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				SelectionGeometry *		sg,
				const RGB8Color *		highlightColor )
    {
    struct BufferItem *	bodySectNode;

    if  ( sdd->sddSelRootNode->biTreeType != DOCinBODY			&&
	  sdd->sddSelRootNode->biTreeType != DOCinSHPTXT		&&
	  docScreenDrawCheckPageOfSelectedTree( sg, &bodySectNode, ds,
			sdd->sddSelRootTree, dc->dcLayoutContext )	)
	{ LDEB(1); return -1; }

    if  ( ! sd->sdIsIBarSelection )
	{
	if  ( tedDrawSelectionHiglight( td, sdd, dc, ds, sd, sg,
							    highlightColor ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sdd->sddSelRootNode->biTreeType != DOCinBODY			&&
	  sdd->sddSelRootNode->biTreeType != DOCinSHPTXT		)
	{
	if  ( docScreenDrawBoxAroundTree( sdd->sddSelRootBodySectNode,
					    sdd->sddSelRootTree, sdd, dc ) )
	    { LDEB(1);	}
	}

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddForeColor) );

    /*  5  */
    if  ( sd->sdIsIBarSelection )
	{
	DocumentRectangle		drIBarPixels;

	docScreenGetIBarRect( &drIBarPixels, &(sg->sgHead),
						dc->dcLayoutContext );

	if  ( geoIntersectRectangle( &drIBarPixels,
				    &drIBarPixels, dc->dcClipRect )	&&
	      ! td->tdShowIBarId					)
	    { docScreenDrawIBar( &drIBarPixels, dc->dcLayoutContext ); }
	}
    else{
	struct InsertedObject *	io;
	int				partO;
	DocumentPosition		dpO;

	docInitDocumentPosition( &dpO );

	if  ( sd->sdIsObjectSelection					&&
	      ! docGetObjectSelection( &partO, &dpO, &io,
						td->tdDocument, ds  )	)
	    {
	    Point2DI		xp[RESIZE_COUNT];
	    DocumentRectangle	drObject;
	    const int		afterObject= 0;

	    tedGetObjectRectangle( &drObject, xp, io, &(sg->sgHead),
				    dc->dcLayoutContext, afterObject, td );

	    if  ( geoIntersectRectangle( (DocumentRectangle *)0,
						&drObject, dc->dcClipRect ) )
		{
		utilRGB8SolidBlack( &(sdd->sddForeColor) );

		sdd->sddHighlightColor= *highlightColor;

		docScreenDrawObjectBlocks( &drObject, xp, dc, sdd );
		}
	    }
	}

    return 0;
    }

/************************************************************************/

static void tedSetScreenDrawingData(
				ScreenDrawingData *	sdd,
				EditDocument *		ed,
				struct DocumentTree *	selRootTree,
				struct BufferItem *	selRootNode,
				struct BufferItem *	selRootBodySectNode )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    utilRGB8SolidBlack( &(sdd->sddForeColor) );
    utilRGB8SolidWhite( &(sdd->sddBehindColor) );

    sdd->sddGridColor= td->tdTableColor;

    sdd->sddFullRect= &(ed->edFullRect);

    sdd->sddSelRootNode= selRootNode;
    sdd->sddSelRootTree= selRootTree;
    sdd->sddSelRootBodySectNode= selRootBodySectNode;
    sdd->sddBoxFirstPage= -1;
    sdd->sddBoxLastPage= -1;

    if  ( selRootTree )
	{
	const struct BufferItem *	selRootSectNode= selRootTree->dtRoot;

	if  ( selRootSectNode->biTreeType == DOCinFOOTNOTE	||
	      selRootSectNode->biTreeType == DOCinENDNOTE	)
	    {
	    sdd->sddBoxFirstPage= selRootSectNode->biTopPosition.lpPage;
	    sdd->sddBoxLastPage= selRootSectNode->biBelowPosition.lpPage;
	    }
	}

    return;
    }

static void tedSetDrawingContext( LayoutContext *		lc,
				DrawingContext *		dc,
				EditDocument *			ed,
				const DocumentRectangle *	drClipPixels )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    struct BufferDocument *	bd= td->tdDocument;
    struct BufferItem * const	rootNode= bd->bdBody.dtRoot;

    tedSetScreenLayoutContext( lc, ed );
    docScreenDrawSetFunctions( dc );

    dc->dcDocument= bd;
    dc->dcLayoutContext= lc;

    dc->dcDrawTableGrid= ( td->tdDrawTableGrid >= 0 );
    dc->dcClipRect= drClipPixels;

    dc->dcFinishTextLine= (FINISH_TEXT_LINE)0;

    dc->dcDrawOtherTrees= 0;
    dc->dcPostponeHeadersFooters= 0;

    dc->dcSelection= (DocumentSelection *)0;
    dc->dcSelectionHeadPositionFlags= 0;
    dc->dcSelectionTailPositionFlags= 0;
    dc->dcSelectionIsTableRectangle= 0;

    dc->dcFirstPage= docGetPageForYPixels( dc->dcLayoutContext,
						    drClipPixels->drY0 );
    dc->dcLastPage=  docGetPageForYPixels( dc->dcLayoutContext,
						    drClipPixels->drY1 );

    if  ( dc->dcFirstPage > rootNode->biBelowPosition.lpPage )
	{ dc->dcFirstPage=  rootNode->biBelowPosition.lpPage; }
    if  ( dc->dcLastPage >  rootNode->biBelowPosition.lpPage )
	{ dc->dcLastPage=   rootNode->biBelowPosition.lpPage; }

    return;
    }

/************************************************************************/
/*									*/
/*  Expose handler for documents.					*/
/*									*/
/*  2)  Clear background.						*/
/*  2b) If the selected area overlaps with the exposed region, draw the	*/
/*	selection background.						*/
/*  3)  Draw text.							*/
/*  4)	Draw page separators.						*/
/*  5)  Draw I bar if necessary.					*/
/*									*/
/************************************************************************/

void tedDrawRectangle(		EditDocument *		ed,
				DocumentRectangle *	drClipPixels )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DrawingSurface		drsf= ed->edDrawingSurface;

    struct BufferDocument *	bd= td->tdDocument;

    DrawingContext		dc;
    ScreenDrawingData		sdd;

    struct BufferItem *		selRootNode= (struct BufferItem *)0;

    const RGB8Color *		highlightColor;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentTree *	selRootTree= (struct DocumentTree *)0;
    struct BufferItem *		selRootBodySectNode= (struct BufferItem *)0;

    LayoutContext		lc;

#   if LOG_REDRAWS
    geoLogRectangle( "REDRAW", drClipPixels );
#   endif

    if  ( tedGetSelection( &ds, &sg, &sd,
		    (struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{
	docInitDocumentSelection( &ds );
	docInitSelectionGeometry( &sg );
	}

    if  ( td->tdVisibleSelectionCopied )
	{ highlightColor= &(td->tdCopiedSelColor);	}
    else{ highlightColor= &(td->tdSelColor);		}

    layoutInitContext( &lc );
    docInitDrawingContext( &dc );

    tedSetDrawingContext( &lc, &dc, ed, drClipPixels );

    /*  2a  */
    if  ( tedHasSelection( ed ) )
	{
	selRootNode= docGetSelectionRoot( &selRootTree,
				    &selRootBodySectNode, bd, &ds );
	if  ( ! selRootNode )
	    { XDEB(selRootNode);	}
	}

    tedSetScreenDrawingData( &sdd, ed,
			selRootTree, selRootNode, selRootBodySectNode );

    /*  2  */
    docOriginalClipping( &dc, &sdd );
    docScreenDrawBackground( &dc, &sdd, &(ed->edBackgroundColor) );

    docDrawSetColorRgb( &dc, (void *)&sdd, &(sdd.sddForeColor) );
    dc.dcCurrentColorSet= 0;
    dc.dcCurrentTextAttributeSet= 0;

    if  ( docDrawPageRange( &dc, (void *)&sdd ) )
	{ LDEB(1);	}

    if  ( selRootNode )
	{
	if  ( tedDrawSelection( td, &sdd, &dc, &ds, &sd, &sg, highlightColor ) )
	    { LDEB(1); return;	}
	}

    if  ( ed->edSelectRectangle.srDirection != DOCselNONE )
	{
	DocumentRectangle	drHair;
	int			blackSet= 0;

				/* Why not use lc? */
	const LayoutContext *	olc= dc.dcLayoutContext;
	int			ox= olc->lcOx;
	int			oy= olc->lcOy;

	drHair= ed->edVisibleRect;
	drHair.drX0= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX0;
	drHair.drX1= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX0;

	if  ( geoIntersectRectangle( &drHair, &drHair, drClipPixels ) )
	    {
	    if  ( ! blackSet )
		{ drawSetForegroundColorBlack( drsf ); blackSet= 1;	}

	    geoShiftRectangle( &drHair, -ox, -oy );
	    drawFillRectangle( drsf, &drHair );
	    }

	if  ( ed->edSelectRectangle.srLTM.drX1 !=
					    ed->edSelectRectangle.srLTM.drX0 )
	    {
	    drHair= ed->edVisibleRect;
	    drHair.drX0= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX1;
	    drHair.drX1= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX1;

	    if  ( geoIntersectRectangle( &drHair, &drHair, drClipPixels ) )
		{
		if  ( ! blackSet )
		    { drawSetForegroundColorBlack( drsf ); blackSet= 1;	}

		geoShiftRectangle( &drHair, -ox, -oy );
		drawFillRectangle( drsf, &drHair );
		}
	    }
	}

    return;
    }

