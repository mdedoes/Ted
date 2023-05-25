/************************************************************************/
/*									*/
/*  Layout of a series of lines in a document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stdlib.h>

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	"docParticuleData.h"
#   include	"docLayoutLine.h"
#   include	"layoutContext.h"

#   include	<docParaParticules.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	<docStripFrame.h>
#   include	<docBlockFrame.h>
#   include	<docPropVal.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	SHOW_LINE_CHANGES	0

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space above a particular line.				*/
/*									*/
/*  2)  Do not claim 'space before' at the top of the page.		*/
/*									*/
/************************************************************************/

static int docAboveLine(	int *				pStopCode,
				LayoutPosition *		lp,
				const struct BufferItem *	paraNode,
				int				part,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf )
    {
    int			spaceAboveLineTwips= 0;

    if  ( part == 0 )
	{
	spaceAboveLineTwips= paraNode->biParaTopInset;

	/*  2  */
	if  ( lp->lpAtTopOfColumn )
	    {
	    spaceAboveLineTwips -=
			    paraNode->biParaProperties->ppSpaceBeforeTwips;
	    }
	}

    if  ( lp->lpPageYTwips+ spaceAboveLineTwips >= bf->bfContentRect.drY1 )
	{ *pStopCode= FORMATstopBLOCK_FULL; return 1;	}

    if  ( lp->lpPageYTwips+ spaceAboveLineTwips >= pf->pfParaContentRect.drY1 )
	{ *pStopCode= FORMATstopFRAME_FULL; return 1;	}

    lp->lpPageYTwips += spaceAboveLineTwips;
    /* still at top */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space below a particular line.				*/
/*									*/
/*  Also decide whether it will fit in the current formatting frame.	*/
/*									*/
/*  1)  Only use the space after paragraph property and the insets	*/
/*	of the table when the paragraph is in a table.			*/
/*  2)  Push the position for the next line down until below any shapes	*/
/*	on this line. Until we implement the various wrap modes, this	*/
/*	is the best we can do.						*/
/*									*/
/************************************************************************/

static int docBelowLine(	int *				pStopCode,
				LayoutPosition *		lp,
				const LayoutPosition *		lpLineTop,
				int				lineStride,
				const struct BufferItem *	paraNode,
				int				partUpto,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf,
				const NotesReservation *	nrLine )
    {
    int				spaceBelowLineTwips= 0;
    int				lineBottom;
    int				lineHeight;
    LayoutPosition		lpBelowLine;

    int				flowY1WithNotes;

    int				footnoteHeight;

    int				contentHigh= bf->bfContentRect.drY1- bf->bfContentRect.drY0;
    int				paraContentHigh= pf->pfParaContentRect.drY1- pf->pfParaContentRect.drY0;

    /*  1  */
    if  ( partUpto == paraNode->biParaParticuleCount		&&
	  paraNode->biParaProperties->ppTableNesting > 0	&&
	  paraNode->biNumberInParent == paraNode->biParent->biChildCount- 1 )
	{ spaceBelowLineTwips += paraNode->biParaBottomInset; }

    lpBelowLine= *lpLineTop;
    lpBelowLine.lpPageYTwips += lineStride;
					/********************************/
					/*  But add spacing to find	*/
					/*  position for next line	*/
					/********************************/

    lineBottom= lpBelowLine.lpPageYTwips+ spaceBelowLineTwips;
    lineHeight= lineBottom- lpLineTop->lpPageYTwips;

    footnoteHeight= nrLine->nrFtnsepHeight+ nrLine->nrFootnoteHeight;
    flowY1WithNotes= bf->bfFlowRect.drY1- footnoteHeight;

    if  ( lineBottom > flowY1WithNotes					&&
	  ( lineHeight < contentHigh || ! lp->lpAtTopOfColumn	)	)
	{ *pStopCode= FORMATstopBLOCK_FULL; return 1;	}

    if  ( lineBottom > pf->pfParaContentRect.drY1	&&
	  lineHeight < paraContentHigh 			)
	{ *pStopCode= FORMATstopFRAME_FULL; return 1;	}

    *lp= lpBelowLine;
    lp->lpAtTopOfColumn= 0;

    if  ( partUpto == paraNode->biParaParticuleCount )
	{
	spaceBelowLineTwips= paraNode->biParaBottomInset;

	lp->lpPageYTwips += spaceBelowLineTwips;
	lp->lpAtTopOfColumn= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*									*/
/*  1)  Cope with the output of sgmls: Ignore enormous space before's	*/
/*	in footers.							*/
/*									*/
/************************************************************************/

static int docLayout_Line(	int *				pStopCode,
				TextLine *			resTl,
				NotesReservation *		pNrLine,
				const BlockFrame *		bf,
				struct BufferItem *		paraNode,
				int				redoLineLayout,
				int				part,
				ParticuleData *			pd,
				const LayoutJob *		lj,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;

    int				res;
    int				accepted;

    TextParticule *		tp= paraNode->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LineRun			blr;
    LineLayoutJob		llj;
    LayoutPosition		lpHere;
    NotesReservation		nrLine;

    docInitLineRun( &blr );
    docInitLineLayoutJob( &llj );

    lpHere= *lpTop;

    if  ( redoLineLayout )
	{
	docInitTextLine( tl );
	tl->tlStroff= tp->tpStroff;
	}
    else{
	workTl= *resTl;
	}
    tl->tlFirstParticule= part;

    docInitNotesReservation( &nrLine );

    /*  1  */
    res= docAboveLine( pStopCode, &lpHere, paraNode, part, bf, pf );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0; }

    tl->tlTopPosition= lpHere;
    if  ( redoLineLayout )
	{
	docStartLayoutLine( &blr, &llj, paraNode, part, lc, pd, pf );

	accepted= docLayoutLineBox( &blr, &llj );
	}
    else{
	accepted= tl->tlParticuleCount;
	llj.lljLineStride= tl->tlLineStride;
	}

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
		    tl->tlTopPosition.lpPage, tl->tlTopPosition.lpColumn,
		    bd, lj->ljBodySectNode, paraNode,
		    tl->tlFirstParticule, tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( pStopCode, &lpHere,
				&(tl->tlTopPosition), llj.lljLineStride,
				paraNode, part+ accepted, bf, pf, &nrLine );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0; }

    if  ( redoLineLayout )
	{
	tl->tlFlags= blr.lrLineFlags;
	tl->tlWordCount= blr.lrWordCount;

	tl->tlLineStride= llj.lljLineStride;
	tl->tlRectangle= llj.lljLineRectangle;
	tl->tlStrlen= llj.lljTailStroff- llj.lljHeadStroff;
	tl->tlParticuleCount= accepted;
	tl->tlFrameWidthTwips= PARA_FRAME_WIDTH( pf );

	if  ( lj->ljScreenLayoutLine			&&
	      (*lj->ljScreenLayoutLine)( tl, &llj ) < 0	)
	    { LDEB(accepted); return -1;	}

	if  ( paraNode->biParaProperties->ppAlignment == DOCthaRIGHT )
	    { tl->tlPixelShift= llj.lljPixelShift;	}
	else{ tl->tlPixelShift= 0;			}

	if  ( docPlaceLineInserts( bd, paraNode, tl, pd, pf, bf, &lpHere ) )
	    { LDEB(accepted); }
	}
    else{
	if  ( docPlaceLineInsertsY( bd, paraNode, tl, bf, &lpHere ) )
	    { LDEB(accepted); }
	}

    *lpBottom= lpHere;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Add the rectangle of the old and the new locations of a line to the	*/
/*  rectangle that must be redrawn.					*/
/*									*/
/************************************************************************/

static void docLayoutAddLineToExpose(
				DocumentRectangle *		drChanged,
				const LayoutJob *		lj,
				const ParagraphFrame *		pf,
				const TextLine *		boxLine,
				const TextLine *		tlLine )
    {
    const LayoutContext *	lc= lj->ljContext;
    DocumentRectangle		drLineFramePixels;

    LayoutPosition		lpTop;
    LayoutPosition		lpNextLine;

    const struct BlockOrigin * const	bo= (const struct BlockOrigin *)0;

    docTextLineGetShiftedTop( &lpTop, bo, boxLine );
    docTextLineGetShiftedNextLineTop( &lpNextLine, bo, boxLine );
    docLineFrameRectanglePixels( &drLineFramePixels, &lpTop, &lpNextLine,
							boxLine, bo, pf, lc );

    geoUnionRectangle( drChanged, drChanged, &drLineFramePixels );

    if  ( tlLine )
	{
	DocumentRectangle	drFrameTl;

	docTextLineGetShiftedTop( &lpTop, bo, tlLine );
	docTextLineGetShiftedNextLineTop( &lpNextLine, bo, tlLine );
	docLineFrameRectanglePixels( &drFrameTl, &lpTop, &lpNextLine,
							tlLine, bo, pf, lc );

	geoUnionRectangle( drChanged, drChanged, &drFrameTl );

#	if SHOW_LINE_CHANGES
	appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
				    drChanged->drX0, drChanged->drX1,
				    drChanged->drY0, drChanged->drY1 );
#	endif
	}
#   if SHOW_LINE_CHANGES
    else{
	appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
				    drChanged->drX0, drChanged->drX1,
				    drChanged->drY0, drChanged->drY1 );
	}
#   endif
    }

/************************************************************************/
/*									*/
/*  Layout a series of lines in a paragraph.				*/
/*									*/
/*  1)  Make sure that we have scratch space for the layout routines.	*/
/*  2)  As long as there are any particules to be placed/formatted	*/
/*  3)  Can the current line be reused?					*/
/*  4)  If so just place it at a new position.				*/
/*  5)  Otherwise recalculate layout.					*/
/*  6)  If the line does not fit on this page (column) stop.		*/
/*  7)  Cause the line to be redrawn if either it is reformatted, or it	*/
/*	is moved.							*/
/*  8)  Subtract the space needed for the footnotes in this line from	*/
/*	the space left on this page. (column)				*/
/*  9)  Insert into administration.					*/
/*  10) If the line ends in a page break, make sure nothing will fit on	*/
/*	this page (in this column) anymore.				*/
/*  11) Truncate the number of lines when the paragraph is completely	*/
/*	formatted.							*/
/*									*/
/************************************************************************/

int docLayoutParaLines(		int *				pStopCode,
				int				isRedo,
				const ParagraphFrame *		pf,
				LayoutPosition *		lpHere,
				int *				pLine,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				struct BufferItem *		paraNode,
				int				part )
    {
    int				rval= 0;
    int				stopCode= FORMATstopREADY;

    int				line= (*pLine);
    int				done= 0;

    ParticuleData *		pd= (ParticuleData *)0;

    LayoutPosition		lp= (*lpHere);

    if  ( paraNode->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(paraNode->biLevel)); rval= -1; goto ready;	}

    /*  1  */
    pd= malloc( paraNode->biParaParticuleCount* sizeof(ParticuleData) );
    if  ( ! pd )
	{ LXDEB(paraNode->biParaParticuleCount,pd); rval= -1; goto ready; }

    /*  2  */
    while( part < paraNode->biParaParticuleCount )
	{
	int			accepted;
	TextLine		boxLine;

	int			placeOldLine= 0;
	TextLine *		tlLine= (TextLine *)0;

	NotesReservation	nrLine;

	DocumentRectangle *	drChanged= lj->ljChangedRectanglePixels;

	int			frameWidth;

	docInitNotesReservation( &nrLine );

	frameWidth= PARA_FRAME_WIDTH( pf );

	docInitTextLine( &boxLine );

	/*  3  */
	if  ( line < paraNode->biParaLineCount )
	    {
	    const TextParticule *	tp= paraNode->biParaParticules+ part;

	    tlLine= paraNode->biParaLines+ line;
	    placeOldLine= 1;
	    boxLine= *tlLine;

	    if  ( tlLine->tlFirstParticule+ tlLine->tlParticuleCount >
					paraNode->biParaParticuleCount	||
		  tlLine->tlStroff != tp->tpStroff			||
		  tlLine->tlFrameWidthTwips != frameWidth		)
		{ placeOldLine= 0;	}
	    }

	/*  4, 5  */
	accepted= docLayout_Line( &stopCode, &boxLine, &nrLine, bf,
			paraNode, ! placeOldLine, part, pd, lj, pf, &lp, &lp );

	if  ( accepted < 0 )
	    { LDEB(accepted); rval= -1; goto ready;	}

	/*  6  */
	if  ( accepted == 0 )
	    { break;	}

	/*  7  */
	if  ( drChanged	)
	    {
	    docLayoutAddLineToExpose( drChanged, lj, pf, &boxLine, tlLine );
	    }

	/*  8  */
	if  ( ! isRedo )
	    { docLayoutReserveNoteHeight( bf, &nrLine );	}

	/*  9  */
	if  ( line >= paraNode->biParaLineCount )
	    {
	    tlLine= docInsertTextLine( paraNode, -1 );
	    if  ( ! tlLine )
		{ XDEB(tlLine); rval= -1; goto ready;		}
	    }
	else{
	    tlLine= paraNode->biParaLines+ line;
	    }

	*tlLine= boxLine;

	part += accepted; done += accepted; line++;

	/*  10  */
	if  ( boxLine.tlFlags & TLflagBLOCKBREAK )
	    {
	    int		lastPart;

	    lastPart= boxLine.tlFirstParticule+ boxLine.tlParticuleCount- 1;

	    switch( paraNode->biParaParticules[lastPart].tpKind )
		{
		case TPkindPAGEBREAK:
		    stopCode= FORMATstopPAGE_BREAK;
		    break;
		case TPkindCOLUMNBREAK:
		    stopCode= FORMATstopCOLUMN_BREAK;
		    break;
		default:
		    LDEB(paraNode->biParaParticules[lastPart].tpKind);
		    stopCode= FORMATstopPAGE_BREAK;
		    break;
		}
	    break;
	    }
	}

    *pLine= line;
    *lpHere= lp;
    *pStopCode= stopCode;

    /*  11  */
    if  ( part >= paraNode->biParaParticuleCount	&&
	  paraNode->biParaLineCount > line		)
	{ paraNode->biParaLineCount=  line; }

    rval= done;

  ready:

    if  ( pd )
	{ free( pd );	}

    return rval;
    }

