/************************************************************************/
/*									*/
/*  Screen drawing.							*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"docScreenDraw.h"
#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<layoutContext.h>
#   include	<docTextRun.h>
#   include	<docTabStop.h>
#   include	<geoGrid.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	<docSelect.h>
#   include	<docStripFrame.h>
#   include	<utilIndexMapping.h>
#   include	<docLayout.h>
#   include	<docScreenLayout.h>
#   include	<psTextExtents.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static void docScreenDrawParticuleLine(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const DrawTextLine *	dtl,
					int			head,
					int			past,
					int			direction,
					int			textColorNr,
					int			y0,
					int			h )
    {
    struct BufferItem *		paraNode= dtl->dtlParaNode;

    const LayoutContext *	lc= dc->dcLayoutContext;
    const ParagraphFrame *	pf= dtl->dtlParagraphFrame;
    TextLine *			tl= (TextLine *)dtl->dtlTextLine;
    const TextParticule *	tp= paraNode->biParaParticules;
    DocumentPosition		dpHead;
    DocumentPosition		dpTail;

    int				docX0Twips;
    int				docX1Twips;
    int				docX0Pixels;
    int				docX1Pixels;
    DocumentRectangle		drFill;

    docSetDocumentPosition( &dpHead, paraNode, tp[head].tpStroff );
    docLayoutLineXOfPosition( &docX0Pixels, &docX0Twips,
				&dpHead, lc, pf, paraNode, tl );

    docSetDocumentPosition( &dpTail, paraNode,
				tp[past- 1].tpStroff+ tp[past- 1].tpStrlen );
    docLayoutLineXOfPosition( &docX1Pixels, &docX1Twips,
				&dpTail, lc, pf, paraNode, tl );

    if  ( direction != 0 )
	{
	drFill.drX0= docX1Pixels;
	drFill.drX1= docX0Pixels;
	}
    else{
	drFill.drX0= docX0Pixels;
	drFill.drX1= docX1Pixels;
	}
    drFill.drY0= y0;
    drFill.drY1= y0+ h- 1;

    if  ( drFill.drX1 > drFill.drX0 )
	{
	docDrawSetColorNumber( dc, (void *)sdd, textColorNr );

	geoShiftRectangle( &drFill, -lc->lcOx, -lc->lcOy );
	drawFillRectangle( lc->lcDrawingSurface, &drFill );
	}

    return;
    }

int docScreenDrawRunUnderline(	const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				baselinePixels;
    int				screenFont;
    int				y0;
    int				h;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    drawGetUnderlineGeometry( &h, &y0,
			    lc->lcDrawingSurface, screenFont, baselinePixels );

    docScreenDrawParticuleLine( dc, sdd, dtl, part, upto, direction,
					    ta->taTextColorNumber, y0, h );

    return 0;
    }

int docScreenDrawRunStrikethrough( const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				baselinePixels;
    int				screenFont;
    int				y0;
    int				h;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    drawGetStrikethroughGeometry( &h, &y0,
			    lc->lcDrawingSurface, screenFont, baselinePixels );

    docScreenDrawParticuleLine( dc, sdd, dtl, part, upto, direction,
						ta->taTextColorNumber, y0, h );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a tab.								*/
/*									*/
/*  1)  Tabs need not to be drawn.					*/
/*									*/
/************************************************************************/

static int tedDrawTabImpl(	const DrawTextLine *		dtl,
				const DocumentRectangle *	drSpan,
				int				baselinePixels,
				int				colorNumber,
				int				lineWidth,
				int				lineStyle,
				const unsigned char *		dot,
				int				dotCount )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    drawSetLineAttributes( lc->lcDrawingSurface,
			lineWidth, lineStyle, LineCapButt, LineJoinMiter,
			dot, dotCount );

    docDrawSetColorNumber( dc, sdd, colorNumber );

    drawLine( lc->lcDrawingSurface,
			drSpan->drX0- lc->lcOx, baselinePixels- lc->lcOy,
			drSpan->drX1- lc->lcOx, baselinePixels- lc->lcOy );

    return 0;
    }

int docScreenDrawTab(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;
    double			xfac= lc->lcPixelsPerTwip;

    int				screenFont;

    int				pixelSize;
    int				baselinePixels;

    DocumentRectangle		drSpan= dtl->dtlLineFrameRectanglePixels;

    baselinePixels= docLayoutYPixels( lc, baseLine );

    drSpan.drX0= docLayoutXPixels( lc, x0Twips );
    drSpan.drX1= docLayoutXPixels( lc, x1Twips );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	{ return 0;	}

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    pixelSize= COORDtoGRID( xfac, 2* ta->taFontSizeHalfPoints );

    switch( leader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    drSpan.drX0= 3* ( ( drSpan.drX0+ 2 )/ 3 );

	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		static const unsigned char	boldDot[]= { 2, 1 };

		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			2, LineStyleSingleDash, boldDot, sizeof( boldDot ) );
		}
	    else{
		static const unsigned char	dot[]= { 1, 2 };

		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			1, LineStyleSingleDash, dot, sizeof( dot ) );
		}

	    break;

	case DOCtlUNDERLINE:
	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			2, LineStyleSolid, (const unsigned char *)0, 0 );
		}
	    else{
		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			1, LineStyleSolid, (const unsigned char *)0, 0 );
		}

	    break;

	case DOCtlHYPH:
	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    drSpan.drX0= 7* ( ( drSpan.drX0+ 6 )/ 7 );

	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		static const unsigned char	boldDash[]= { 4, 3 };

		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			2, LineStyleSingleDash, boldDash, sizeof( boldDash ) );
		}
	    else{
		static const unsigned char	dash[]= { 3, 4 };

		tedDrawTabImpl( dtl, &drSpan, baselinePixels,
			ta->taTextColorNumber,
			1, LineStyleSingleDash, dash, sizeof( dash ) );
		}

	    break;

	case DOCtlTHICK:
	    LDEB(leader);
	    break;
	case DOCtlEQUAL:
	    LDEB(leader);
	    break;
	default:
	    LDEB(leader);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a footnote separator.						*/
/*									*/
/************************************************************************/

int docScreenDrawFtnsep( const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				y0;
    int				h;
    int				baselinePixels;

    int				screenFont;
    DocumentRectangle		drSpan= dtl->dtlLineFrameRectanglePixels;

    baselinePixels= docLayoutYPixels( lc, baseLine );

    drSpan.drX0= docLayoutXPixels( lc, x0Twips );
    drSpan.drX1= docLayoutXPixels( lc, x1Twips );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	{ return 0;	}

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );
    if  ( screenFont < 0 )
	{ LLDEB(textAttrNr,screenFont); return 0;	}

    drawGetStrikethroughGeometry( &h, &y0, lc->lcDrawingSurface,
						screenFont, baselinePixels );

    drSpan.drY0= y0;
    drSpan.drY1= y0+ h- 1;

    docDrawSetColorNumber( dc, (void *)sdd, ta->taTextColorNumber );

    geoShiftRectangle( &drSpan, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drSpan );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the individual segments of a smallcaps string.			*/
/*									*/
/************************************************************************/

static void docScreenDrawSegments(
				DrawingContext *	dc,
				int			x,
				int			y,
				int			fullScreenFont,
				int			scapsScreenFont,
				const char *		s,
				const int *		segments,
				int			segmentCount )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				seg;

    DocumentRectangle		drText;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    drawString( lc->lcDrawingSurface,
					x, y, fullScreenFont,
					s, segments[2* seg+ 0] );

	    drawGetTextExtents( &drText, lc->lcDrawingSurface,
					x, y, fullScreenFont,
					s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x= drText.drX1+ 1;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    drawString( lc->lcDrawingSurface,
					x, y, scapsScreenFont,
					s, segments[2* seg+ 1] );

	    drawGetTextExtents( &drText, lc->lcDrawingSurface,
					x, y, scapsScreenFont,
					s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x= drText.drX1+ 1;
	    }
	}

    return;
    }

/************************************************************************/

int docScreenDrawTextRun(	const TextRun *		tr,
				int			x0Twips,
				int			x1Twips,
				const DrawTextLine *	dtl,
				const LayoutPosition *	baseLine,
				const char *		outputString )
    {
    int				rval= 0;

    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				baselinePixels;
    int				fullScreenFont;
    int				y;

    const int			mirror= tr->trDirection;

    DocumentRectangle		drSpan= dtl->dtlLineFrameRectanglePixels;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    drSpan.drX0= docLayoutXPixels( lc, x0Twips );
    drSpan.drX1= docLayoutXPixels( lc, x1Twips );

    if  ( dc->dcClipRect )
	{
	if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	    { return 0;	}
	}

    fullScreenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
							tr->trTextAttributeNr );
    if  ( fullScreenFont < 0 )
	{
	fullScreenFont= docOpenScreenFont( lc, tr->trTextAttributeNr );

	if  ( fullScreenFont < 0 )
	    {
	    LLDEB(tr->trTextAttributeNr,fullScreenFont);
	    rval= -1; goto ready;
	    }
	}

    if  ( psMakeOutputString( &outputString, &scratchString,
				    &segments, &segmentCount,
				    tr->trTextAttribute, mirror,
				    outputString, tr->trStrlen ) )
	{ LDEB(tr->trStrlen); rval= -1; goto ready;	}

    switch( tr->trTextAttribute->taSuperSub )
	{
	case TEXTvaSUPERSCRIPT:
	    drawGetSuperBaseline( &y,
			lc->lcDrawingSurface, fullScreenFont, baselinePixels );
	    break;

	case TEXTvaSUBSCRIPT:
	    drawGetSubBaseline( &y,
			lc->lcDrawingSurface, fullScreenFont, baselinePixels );
	    break;

	default:
	    LDEB(tr->trTextAttribute->taSuperSub);
	    /*FALLTHROUGH*/
	case TEXTvaREGULAR:
	    y= baselinePixels;
	    break;
	}

    docDrawSetFont( dc, (void *)sdd,  tr->trTextAttributeNr,
				tr->trTextAttribute );
    docDrawSetColorNumber( dc, (void *)sdd,
				tr->trTextAttribute->taTextColorNumber );

    if  ( segmentCount > 0 )
	{
	int	scapsScreenFont;

	scapsScreenFont= docLayoutScapsScreenFont( lc, tr->trTextAttribute );
	if  ( scapsScreenFont < 0 )
	    { LDEB(scapsScreenFont); rval= -1; goto ready;	}

	docScreenDrawSegments( dc, drSpan.drX0- lc->lcOx, y- lc->lcOy,
				fullScreenFont, scapsScreenFont,
				outputString, segments, segmentCount );
	}
    else{
	drawString( lc->lcDrawingSurface,
				drSpan.drX0- lc->lcOx, y- lc->lcOy,
				fullScreenFont, outputString, tr->trStrlen );
	}

    /*
    appDebug( "[%3d,%3d] -- %3d:\"%*.*s\"\n",
				y, drSpan.drX0, tr->trStrlen,
				tr->trStrlen, tr->trStrlen, outputString );
    */

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Start drawing a line of text.					*/
/*									*/
/*  0)  It does not make sense to draw a line that does not intersect	*/
/*	the clipping rectangle.						*/
/*  1)  For optimal conformance with the print layout, draw the		*/
/*	particules on the line separately.				*/
/*									*/
/************************************************************************/

int docScreenDrawStartTextLine(	struct DrawTextLine *	dtl,
				int			x0Twips )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;

    /*  0  */
    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
		dc->dcClipRect, &(dtl->dtlLineFrameRectanglePixels) )	)
	{ return 1;	}

    /*  1  */
    dtl->dtlDrawParticulesSeparately= dc->dcDrawParticulesSeparately ||
	    docScreenDrawParticulesSeparately( dtl->dtlParaNode, dtl->dtlTextLine );

#   define	DRAW_DIAGNOSTIC_BACKGROUND	0
#   if		DRAW_DIAGNOSTIC_BACKGROUND
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DocumentRectangle		drLine;

    RGB8Color			behind;

    docGetPixelRect( &drLine, lc, &(dtl->dtlTextRectangle),
					dtl->dtlShiftedTopPosition.lpPage );

    utilRGB8SolidRGB( &behind, 192, 192, 192 );
    docDrawSetColorRgb( dc, (void *)sdd, &behind );

    geoShiftRectangle( &drLine, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drLine );
    }
#   endif

    return 0;
    }

int docScreenDrawParticulesSeparately(
				const struct BufferItem *	paraNode,
				const struct TextLine *		tl )
    {
    return 1;
    }
