/************************************************************************/
/*									*/
/*  Draw objects inserted in the document.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	"docScreenDraw.h"
#   include	<layoutContext.h>
#   include	<docObjectProperties.h>
#   include	<docObject.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clip and draw the visible part of a pixmap.				*/
/*									*/
/*  X11 can get in serious trouble if we draw to negative coordinates	*/
/*  so really avoid this. To hide the similarity to MS-Windows we say	*/
/*  this is an optimization.						*/
/*									*/
/*  1)  Destination rectangle.						*/
/*  2)  Clip if needed.							*/
/*  3)  Shift by scrollbars.						*/
/*  4)  Do not draw what protrudes on the top and left sides of the	*/
/*	destination. (Clipping usually takes care of right/bottom).	*/
/*  5)  Draw if something left. The source coordinates reflect what	*/
/*	has been cut off on the top and left by clipping.		*/
/*									*/
/************************************************************************/

static void docScreenDrawObjectPixmap(
				const InsertedObject *		io,
				int				x0,
				int				y0,
				const DocumentRectangle *	drClip,
				const LayoutContext *		lc )
    {
    DocumentRectangle	drSrc;
    int			destX0;
    int			destY0;

    /*  1  */
    drSrc.drX0= 0;
    drSrc.drY0= 0;
    drSrc.drX1= io->ioPixelsWide- 1;
    drSrc.drY1= io->ioPixelsHigh- 1;

    /*  2  */
    if  ( drClip )
	{
	DocumentRectangle	drClipSrc= *drClip;

	geoShiftRectangle( &drClipSrc, -x0, -y0 );

	if  ( ! geoIntersectRectangle( &drSrc, &drSrc, &drClipSrc ) )
	    { return;	}
	}

    destX0= x0+ drSrc.drX0- lc->lcOx;
    destY0= y0+ drSrc.drY0- lc->lcOy;

    /*  5  */
    drawChildSurface( lc->lcDrawingSurface, io->ioDrawingSurface,
						destX0, destY0, &drSrc );
    }

/************************************************************************/
/*									*/
/*  Draw an object.							*/
/*									*/
/************************************************************************/

int docScreenDrawInlineObject(
			const DrawTextLine *			dtl,
			int					part,
			InsertedObject *			io,
			const struct DocumentRectangle *	drTwips,
			const LayoutPosition *			baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const DocumentRectangle *	drClip= dc->dcClipRect;
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				x0;
    int				y0;

    DocumentRectangle		drObject;

    if  ( io->ioInline )
	{
	int			baselinePixels;

	baselinePixels= docLayoutYPixels( lc, baseLine );

	x0= docLayoutXPixels( lc, drTwips->drX0 );
	y0= baselinePixels- io->ioPixelsHigh;
	/* x1= docLayoutXPixels( lc, x1Twips ); */
	}
    else{
	x0= docLayoutXPixels( lc, io->ioX0Twips );
	y0= docLayoutYPixels( lc, &(io->ioY0Position) );
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{ return 0;	}

    if  ( io->ioDrawingSurface				&&
	  ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokPICTEMFBLIP	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	)
	{
	docScreenDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT				&&
	  io->ioDrawingSurface					&&
	  ( io->ioResultKind == DOCokPICTWMETAFILE	||
	    io->ioResultKind == DOCokPICTEMFBLIP	||
	    io->ioResultKind == DOCokMACPICT		||
	    io->ioResultKind == DOCokPICTPNGBLIP	||
	    io->ioResultKind == DOCokPICTJPEGBLIP	)	)
	{
	docScreenDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    if  ( io->ioKind == DOCokEPS_FILE				&&
	  io->ioDrawingSurface					)
	{
	docScreenDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    drObject.drX0= x0;
    drObject.drY0= y0;
    drObject.drX1= drObject.drX0+ io->ioPixelsWide- 1;
    drObject.drY1= drObject.drY0+ io->ioPixelsHigh- 1;

    drawSetLineAttributes( lc->lcDrawingSurface,
			    1, LineStyleSolid, LineCapButt, LineJoinMiter,
			    (const unsigned char *)0, 0 );

    geoShiftRectangle( &drObject, -lc->lcOx, -lc->lcOy );
    drawRectangle( lc->lcDrawingSurface, &drObject );

    return 0;
    }

