/************************************************************************/
/*									*/
/*  Translate back and forth between text offset in a line and pixel	*/
/*  X coordinate.							*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<limits.h>
#   include	<stdlib.h>
#   include	<math.h>

#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<textAttribute.h>
#   include	<docSelect.h>
#   include	<layoutContext.h>
#   include	"docDraw.h"
#   include	"docScreenDraw.h"
#   include	"docDrawLine.h"
#   include	"docPositionFindJob.h"
#   include	<docLayout.h>
#   include	<docScreenLayout.h>
#   include	<docScanner.h>
#   include	<docTextRun.h>
#   include	<docTextLine.h>
#   include	<uniUtf8.h>
#   include	<psTextExtents.h>
#   include	<drawDrawingSurface.h>
#   include	<docParaParticules.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	LOG_FINDX	0

/************************************************************************/

static int docFindPositionFound(
				const struct DrawTextLine *	dtl,
				int				stroff,
				int				xPixels,
				int				xTwips )
    {
    PositionFindJob *	pfj= (PositionFindJob *)dtl->dtlThrough;

    const int	dx= pfj->pfjDocXPixels- xPixels;
    const int	dy= pfj->pfjDocYPixels- dtl->dtlShiftedLineCenterPixels;
    const int	distance= sqrt( dx* dx+ dy* dy );

    if  ( ! pfj->pfjFound || pfj->pfjDistance > distance )
	{
	int	lineFlags= 0;
	int	lastOne= 0;
	int	part= 0;
	int	partFlags= 0;

	docSetDocumentPosition( &(pfj->pfjDocumentPosition),
						dtl->dtlParaNode, stroff );

	pfj->pfjFound= 1;
	pfj->pfjDistance= distance;

	docSetLineFlags( &lineFlags, dtl->dtlParaNode, stroff,
							dtl->dtlTextLine );

	if  ( ( lineFlags & POSflagLINE_HEAD ) != 0 )
	    { lastOne= PARAfindLAST;	}
	else{ lastOne= PARAfindFIRST;	}

	if  ( docFindParticuleOfPosition( (int *)0, &partFlags,
				    &(pfj->pfjDocumentPosition), lastOne ) )
	    { LDEB(stroff);	}

	docSetParticuleFlags( &partFlags, dtl->dtlParaNode, part, stroff );

	docSetPositionGeometry( &(pfj->pfjPositionGeometry), 
				dtl->dtlParagraphFrame, dtl->dtlTextLine,
				xPixels, xTwips,
				partFlags | lineFlags );

	if  ( geo2DIXYInBox( pfj->pfjDocXPixels, pfj->pfjDocYPixels,
					&(dtl->dtlLineFrameRectanglePixels) ) )
	    { return SCANadviceSTOP;	}
	else{ return SCANadviceOK;	}
	}

    return SCANadviceOK;
    }

/************************************************************************/

static int docFindPositionDrawParticule(
				const struct DrawTextLine *	dtl,
				int				part,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine )
    {
    PositionFindJob *		pfj= (PositionFindJob *)dtl->dtlThrough;

    const DrawingContext *	dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    int				x0Pixels= docLayoutXPixels( lc, x0Twips );
    int				x1Pixels= docLayoutXPixels( lc, x1Twips );

    const TextParticule *	tp= dtl->dtlParaNode->biParaParticules+ part;

    switch( tp->tpKind )
	{
	case TPkindOBJECT:
	case TPkindTAB:
	case TPkindCHFTNSEP:
	case TPkindCHFTNSEPC:
	    /*  Before this particule */
	    if  ( pfj->pfjDocXPixels < x0Pixels )
		{ return SCANadviceOK;	}

	    /*  After this particule */
	    if  ( pfj->pfjDocXPixels > x1Pixels )
		{ return SCANadviceOK;	}

	    if  ( x1Pixels- pfj->pfjDocXPixels < pfj->pfjDocXPixels- x0Pixels )
		{ 
		return docFindPositionFound( dtl, tp->tpStroff+ tp->tpStrlen,
							x1Pixels, x1Twips );
		}
	    else{
		return docFindPositionFound( dtl, tp->tpStroff,
							x0Pixels, x0Twips );
		}

	case TPkindFIELDHEAD:
	case TPkindFIELDTAIL:
	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	case TPkindOPT_HYPH:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    if  ( tp->tpWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpWide);	}
	    return SCANadviceOK;

	default:
	    if  ( tp->tpWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpWide);	}
	    else{ LDEB(tp->tpKind);		}
	    return SCANadviceOK;
	}
    }

/************************************************************************/
/*									*/
/*  Find the string offsets and the widths in pixels of the positions	*/
/*  immediately before and immediately after a certain pixel width.	*/
/*									*/
/*  1)  Look for the length that results in a pixel with that is less	*/
/*	than or equal to than the requested width.			*/
/*  1a) Care about the peculiarities of UTF-8 encoding.			*/
/*									*/
/************************************************************************/

static int docFindOffsetForWidth(
				int *				pPixelStep,
				int *				pStep,
				const LayoutContext *		lc,
				int				screenFont,
				int				xPixels,
				int				yPixels,
				int				pixelOffset,
				const char *			outputString,
				int				len )
    {
    int			l= 0;
    int			r= len;
    int			m= ( l+ r )/ 2;

    DocumentRectangle	drText;
    int			om;
    int			omm;

    /*  1  */
    while( l < m )
	{
	/*  1a  */
	if  ( UNI_UTF8_INTERN( outputString[m] ) )
	    {
	    int		mm= m;

	    while( UNI_UTF8_INTERN( outputString[mm] ) && mm > l )
		{ mm--;	}
	    if  ( mm > l )
		{ m= mm;	}
	    else{
		mm= m;
		while( UNI_UTF8_INTERN( outputString[mm] ) && mm < r )
		    { mm++;	}

		if  ( mm < r )
		    { m= mm;	}
		else{ m= l; break;	}
		}
	    }

	om= drawGetTextExtents( &drText, lc->lcDrawingSurface,
				xPixels, yPixels,
				screenFont, outputString, m );

	if  ( om <= pixelOffset )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    om= drawGetTextExtents( &drText, lc->lcDrawingSurface,
			xPixels, yPixels, screenFont, outputString, m );

    if  ( om > pixelOffset )
	{
	int	mm= m- 1;

	if  ( mm < 0 )
	    { LDEB(mm); return -1;	}

	/*  1a  */
	while( UNI_UTF8_INTERN( outputString[mm] ) && mm > 0 )
	    { mm--;	}

	if  ( mm < 0 )
	    { LDEB(mm); return -1;	}

	omm= drawGetTextExtents( &drText, lc->lcDrawingSurface,
			xPixels, yPixels, screenFont, outputString, mm );
	SDEB("@@@@");
	LLLLLDEB(mm,m,omm,pixelOffset,om);
	if  ( pixelOffset- omm < om- pixelOffset )
	    { *pPixelStep= omm; *pStep= mm;	}
	else{ *pPixelStep= om; *pStep= m;	}
	}
    else{
	int	mm= m+ 1;

	if  ( mm > len )
	    { LLDEB(mm,len); return -1;	}

	/*  1a  */
	while( UNI_UTF8_INTERN( outputString[mm] ) && mm < len )
	    { mm++;	}

	if  ( mm > len )
	    { LLDEB(mm,len); return -1;	}

	omm= drawGetTextExtents( &drText, lc->lcDrawingSurface,
			xPixels, yPixels, screenFont, outputString, mm );
	if  ( omm- pixelOffset < pixelOffset- om )
	    { *pPixelStep= omm; *pStep= mm;	}
	else{ *pPixelStep= om; *pStep= m;	}
	}

    return 0;
    }

static int docFindOffsetInSegment(
			int *				pPixelStep,
			int *				pStep,
			int *				pXPixels,
			int *				pXTwips,
			int				offset,
			const LayoutContext *		lc,
			const struct AfmFontInfo *	afi,
			int				yPixels,
			int				pixelOffset,
			int				screenFont,
			int				sizeTwips,
			const char *			outputString,
			int				len )
    {
    int			x0Pixels= *pXPixels;
    int			x0Twips= *pXTwips;
    int			x1Pixels;

    DocumentRectangle	drText;
    int			pixelsWide;

    pixelsWide= drawGetTextExtents( &drText, lc->lcDrawingSurface,
			x0Pixels, yPixels, screenFont, outputString, len );

    x1Pixels= x0Twips+ pixelsWide;

    if  ( pixelOffset < x0Pixels		||
	  pixelOffset > x1Pixels		)
	{
	DocumentRectangle	drBBox;
	int			twipsWide;
	const int		vswap= 1;
	const int		withKerning= 0;

	twipsWide= psCalculateStringExtents( &drBBox, outputString, len,
					sizeTwips, withKerning, vswap, afi );

	*pXPixels= x1Pixels;
	*pXTwips += twipsWide;
	return SCANadviceSKIP;
	}

    if  ( docFindOffsetForWidth( pPixelStep, pStep, lc, screenFont,
			    x0Pixels, yPixels,
			    pixelOffset- x0Pixels, outputString, len ) < 0 )
	{ LDEB(1); return -1;	}

    return SCANadviceSTOP;
    }

static int docFindOffsetInSegments(
			int *				pPixelStep,
			int *				pStep,
			int *				pXPixels,
			int *				pXTwips,
			const int			offset,
			const LayoutContext *		lc,
			const struct AfmFontInfo *	afi,
			int				yPixels,
			int				pixelOffset,
			int				fullScreenFont,
			int				scapsScreenFont,
			int				fullSizeTwips,
			int				scapsSizeTwips,
			const char *			outputString,
			const int *			segments,
			int				segmentCount )
    {
    int			seg;
    int			res;
    int			done= 0;

    if  ( segmentCount < 1 )
	{ LDEB(segmentCount); return -1;	}

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
#	    if LOG_FINDX
	    appDebug( "%3d: @%3d pix=%3d FULL \"%*.*s\"\n",
			seg, offset+ done, *pXPixels,
			segments[2* seg+ 0], segments[2* seg+ 0],
			outputString );
#	    endif

	    res= docFindOffsetInSegment( pPixelStep, pStep, pXPixels, pXTwips,
				    offset+ done, lc, afi,
				    yPixels, pixelOffset,
				    fullScreenFont, fullSizeTwips,
				    outputString, segments[2* seg+ 0] );
	    if  ( res < 0 )
		{ LDEB(res); return res;	}

	    if  ( res == SCANadviceSTOP )
		{ *pStep += done; break;	}

	    done += segments[2* seg+ 0];
	    outputString += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
#	    if LOG_FINDX
	    appDebug( "%3d: @%3d pix=%3d SCAP \"%*.*s\"\n",
			seg, offset+ done, *pXPixels,
			segments[2* seg+ 1], segments[2* seg+ 1],
			outputString );
#	    endif

	    res= docFindOffsetInSegment( pPixelStep, pStep, pXPixels, pXTwips,
				offset+ done, lc, afi,
				yPixels, pixelOffset,
				scapsScreenFont, scapsSizeTwips,
				outputString, segments[2* seg+ 1] );
	    if  ( res < 0 )
		{ LDEB(res); return res;	}

	    if  ( res == SCANadviceSTOP )
		{ *pStep += done; break;	}

	    done += segments[2* seg+ 1];
	    outputString += segments[2* seg+ 1];
	    }
	}

    return SCANadviceOK;
    }

/************************************************************************/
/*									*/
/*  Find a position inside the string of a run.				*/
/*									*/
/************************************************************************/

static int docFindPositionDrawTextRun(
				const TextRun *			tr,
				int				x0Twips,
				int				x1Twips,
				const struct DrawTextLine *	dtl,
				const struct LayoutPosition *	baseLine,
				const char *			outputString )
    {
    int				rval= SCANadviceOK;

    const DrawingContext *	dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;

    const char *		printString= outputString;
    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    const int			mirror= 0;
    int				fullScreenFont;
    int				scapsScreenFont;
    int				fullSizeTwips;
    int				scapsSizeTwips;
    const struct AfmFontInfo *	afi;

    PositionFindJob *		pfj= (PositionFindJob *)dtl->dtlThrough;
    int				x0Pixels= docLayoutXPixels( lc, x0Twips );
    int				x1Pixels= docLayoutXPixels( lc, x1Twips );
    int				pixelOffset= pfj->pfjDocXPixels- x0Pixels;

    int				pixelStep= 0;
    int				step= 0;

    DocumentRectangle		drBBox;
    int				twipsWide;

    if  ( pfj->pfjDocXPixels < x0Pixels		||
	  pfj->pfjDocXPixels > x1Pixels		)
	{ goto ready;	}

    switch( tr->trDirection )
	{
	case 0:
	    pixelOffset= pfj->pfjDocXPixels- x0Pixels;
	    break;

	case 1:
	    pixelOffset= x1Pixels- pfj->pfjDocXPixels;
	    break;

	default:
	    LDEB(tr->trDirection);
	    rval= -1; goto ready;
	}

    if  ( docLayoutMakeOutput( &printString, &scratchString,
				&segments, &segmentCount,
				&fullScreenFont, &scapsScreenFont,
				&afi, &fullSizeTwips, &scapsSizeTwips,
				tr, mirror, lc, outputString, tr->trStrlen ) )
	{ LDEB(tr->trStrlen); rval= -1; goto ready;	}

    if  ( segmentCount > 0 )
	{
	int		xPixels= 0;
	int		xTwips= 0;

#	if LOG_FINDX
	appDebug( "%3d: ++++ \"%*.*s\"\n", segmentCount,
				    tr->trStrlen, tr->trStrlen, printString );
#	endif

	if  ( docFindOffsetInSegments( &pixelStep, &step, &xPixels, &xTwips,
				tr->trStroff, lc, afi,
				dtl->dtlShiftedBaselinePixels, pixelOffset,
				fullScreenFont, scapsScreenFont,
				fullSizeTwips, scapsSizeTwips,
				printString, segments, segmentCount ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	twipsWide= psSegmentedTextExtents( &drBBox, printString, step,
				segments, segmentCount, fullSizeTwips, afi );
	}
    else{
	const int			vswap= 1;
	const int			withKerning= 0;

#	if LOG_FINDX
	appDebug( "%3d: ---- \"%*.*s\"\n", 0,
				    tr->trStrlen, tr->trStrlen, printString );
#	endif

	if  ( docFindOffsetForWidth( &pixelStep, &step, lc, fullScreenFont,
			    x0Pixels, dtl->dtlShiftedBaselinePixels,
			    pixelOffset, printString, tr->trStrlen ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	twipsWide= psCalculateStringExtents( &drBBox, printString, step,
				fullSizeTwips, withKerning, vswap, afi );
	}

#   if LOG_FINDX
    appDebug( "%3d: ==== \"%*.*s|%*.*s\"\n", 0,
				    step, step, printString,
				    tr->trStrlen- step, tr->trStrlen- step,
				    printString+ step );
#   endif

    switch( tr->trDirection )
	{
	case 0:
	    rval= docFindPositionFound( dtl, tr->trStroff+ step,
				x0Pixels+ pixelStep, x0Twips+ twipsWide );
	    break;

	case 1:
	    rval= docFindPositionFound( dtl, tr->trStroff+ step,
				x1Pixels- pixelStep, x1Twips- twipsWide );
	    break;

	default:
	    LDEB(tr->trDirection);
	    rval= -1; goto ready;
	}

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Probe a line to find out whether the position is inside the line.	*/
/*  If it is, remember that position.					*/
/*									*/
/*  0)  Avoid shifted table headers.					*/
/*  1)  Determine rectangle (in pixels) for the line.			*/
/*	As pfjDocX,Y have been corrected for scrolling, scrolling is	*/
/*	irrelevant here. The shifts are for table headers &c that are	*/
/*	repeated on a different page.					*/
/*  2)  x, y inside rectangle?						*/
/*  3)  Above line, if near.. select head of line.			*/
/*									*/
/************************************************************************/

static int docFindPositionStartTextLine(
				struct DrawTextLine *		dtl,
				int				x0Twips )
    {
    PositionFindJob *		pfj= (PositionFindJob *)dtl->dtlThrough;

    const int			t= 10;
    const int			a= 3;

    const DocumentRectangle *	drl= &(dtl->dtlLineFrameRectanglePixels);
    DocumentRectangle		drInflated;

    /*  0  */
    if  ( dtl->dtlXShift != 0 || dtl->dtlYShift != 0 )
	{ return SCANadviceSKIP;	}

    drInflated.drX0= drl->drX0- ( a* ( drl->drX1- drl->drX0 ) )/ t;
    drInflated.drX1= drl->drX1+ ( a* ( drl->drX1- drl->drX0 ) )/ t;
    drInflated.drY0= drl->drY0- ( a* ( drl->drY1- drl->drY0 ) )/ t;
    drInflated.drY1= drl->drY1+ ( a* ( drl->drY1- drl->drY0 ) )/ t;

    if  ( ! geo2DIXYInBox( pfj->pfjDocXPixels, pfj->pfjDocYPixels,
							    &drInflated ) )
	{ return SCANadviceSKIP;	}

    {
    const DrawingContext *	dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				xPixels;

    dtl->dtlDrawParticulesSeparately= dc->dcDrawParticulesSeparately ||
	    docScreenDrawParticulesSeparately( dtl->dtlParaNode, dtl->dtlTextLine );

    xPixels= docLayoutXPixels( lc, x0Twips );

    docFindPositionFound( dtl, dtl->dtlTextLine->tlStroff,
							xPixels, x0Twips );
    }

    return SCANadviceOK;
    }

static int docFindPositionFinishTextLine(
				const struct DrawTextLine *	dtl,
				int				x1Twips )
    {
    const DrawingContext *	dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= dc->dcLayoutContext;
    int				x1Pixels;

    x1Pixels= docLayoutXPixels( lc, x1Twips );

    docFindPositionFound( dtl,
		    dtl->dtlTextLine->tlStroff+ dtl->dtlTextLine->tlStrlen,
		    x1Pixels, x1Twips );

    return SCANadviceOK;
    }

static int docFindPositionDrawInlineObject(
				const struct DrawTextLine *	dtl,
				int				part,
				struct InsertedObject *		io,
				const struct DocumentRectangle * drTwips,
				const struct LayoutPosition *	baseLine )
    {
    return docFindPositionDrawParticule( dtl, part,
				    drTwips->drX0, drTwips->drX1, baseLine );
    }

static int docFindPositionDrawTab(
				const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine )
    {
    return docFindPositionDrawParticule( dtl, part,
						x0Twips, x1Twips, baseLine );
    }

static int docFindPositionDrawFtnsep(
				const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine )
    {
    return docFindPositionDrawParticule( dtl, part,
						x0Twips, x1Twips, baseLine );
    }

/************************************************************************/

void docSetFindPositionJob(	PositionFindJob *		pfj,
				const struct LayoutContext *	lc,
				int				docXPixels,
				int				docYPixels )

    {
    DrawingContext *	dc= &(pfj->pfjDrawingContext);

    docInitDrawingContext( dc );

    dc->dcDocument= lc->lcDocument;

    pfj->pfjPage= docGetPageForYPixels( lc, docYPixels );
    docGetPageRectPixels( &(pfj->pfjPageRectPixels), lc, pfj->pfjPage );

    dc->dcLayoutContext= lc;
    dc->dcDrawTableGrid= 0;
    dc->dcClipRect= &(pfj->pfjPageRectPixels);

    dc->dcFirstPage= pfj->pfjPage;
    dc->dcLastPage= pfj->pfjPage;

    docInitDocumentPosition( &(pfj->pfjDocumentPosition) );
    docInitPositionGeometry( &(pfj->pfjPositionGeometry) );
    dc->dcStartTextLine= docFindPositionStartTextLine;
    dc->dcDrawTextRun= docFindPositionDrawTextRun;
    dc->dcFinishTextLine= docFindPositionFinishTextLine;
    dc->dcDrawInlineObject= docFindPositionDrawInlineObject;
    dc->dcDrawTab= docFindPositionDrawTab;
    dc->dcDrawFtnsep= docFindPositionDrawFtnsep;
    dc->dcDrawOtherTrees= 0;
    dc->dcPostponeHeadersFooters= 0;

    dc->dcCurrentColorSet= 0;
    dc->dcCurrentTextAttributeSet= 0;

    pfj->pfjFound= 0;
    pfj->pfjDistance= INT_MAX;

    pfj->pfjDocXPixels= docXPixels;
    pfj->pfjDocYPixels= docYPixels;

    return;
    }
