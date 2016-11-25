/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space above a particular line.				*/
/*									*/
/*  1)  Cope with the output of jade: Ignore enormous space before's	*/
/*	in footers.							*/
/*  2)  Do not claim 'space before' at the top of the page.		*/
/*									*/
/************************************************************************/

static void docAboveLine(	LayoutPosition *		lp,
				int				fromLinePos,
				const BufferItem *		paraBi,
				int				part,
				const DocumentGeometry *	dg,
				AppDrawingData *		add )
    {
    int			spaceAboveLineTwips= 0;

    /*  1  */
    if  ( part == 0					&&
	  ! fromLinePos					&&
	  paraBi->biInExternalItem == DOCinBODY		)
	{
	spaceAboveLineTwips= paraBi->biTopInsetTwips;

	/*  2  */
	if  ( lp->lpAtTopOfColumn )
	    { spaceAboveLineTwips -= paraBi->biParaSpaceBeforeTwips;	}
	}

    lp->lpPageYTwips += spaceAboveLineTwips;
    /* still at top */

    return;
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
/*									*/
/************************************************************************/

static int docBelowLine(	const LayoutPosition *		lpTop,
				LayoutPosition *		lp,
				const TextLine *		tl,
				const BufferItem *		paraBi,
				int				past,
				const ParagraphFrame *		pf,
				const NotesReservation *	nrLine,
				AppDrawingData *		add )
    {
    int			spaceBelowLineTwips= 0;
    int			lineBottom;
    int			lineHeight;
    int			frameBottom;

    int			footnoteHeight;

    if  ( past == paraBi->biParaParticuleCount )
	{
	/*  1  */
	if  ( paraBi->biParaInTable )
	    {
	    if  ( paraBi->biNumberInParent ==
					paraBi->biParent->biChildCount- 1 )
		{
		const BufferItem *	bi= paraBi;

		while( bi )
		    {
		    spaceBelowLineTwips += bi->biBottomInsetTwips;

		    if  ( bi->biLevel == DOClevROW )
			{ break;	}

		    bi= bi->biParent;
		    }
		}
	    }
	}

    lineBottom= lp->lpPageYTwips+ tl->tlLineHeightTwips+ spaceBelowLineTwips;
    lineHeight= lineBottom- lpTop->lpPageYTwips;

    footnoteHeight= nrLine->nrFtnsepHeight+ nrLine->nrFootnoteHeight;
    frameBottom= pf->pfPageY1Twips- footnoteHeight;

    if  ( pf->pfHasBottom				&&
	  lineBottom > frameBottom			&&
	  lineHeight < pf->pfPageHigh			&&
	  ( pf->pfStripHigh < 0			||
	    lineHeight < pf->pfStripHigh	)	)
	{
	lp->lpPageYTwips= pf->pfPageY1Twips+ 1;
	lp->lpAtTopOfColumn= 0;
	return 1;
	}

    lp->lpPageYTwips += tl->tlLineSpacingTwips;
    lp->lpAtTopOfColumn= 0;

    if  ( past == paraBi->biParaParticuleCount )
	{
	spaceBelowLineTwips= paraBi->biBottomInsetTwips;

	lp->lpPageYTwips += spaceBelowLineTwips;
	lp->lpAtTopOfColumn= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Place a line on the page without reformatting its text. Just the	*/
/*  position changes, not the layout of the text inside the line.	*/
/*									*/
/************************************************************************/

static int docPlace_Line(	TextLine *			resTl,
				NotesReservation *		pNrLine,
				const BufferItem *		paraBi,
				int				part,
				ParticuleData *			pd,
				const ParagraphLayoutContext *	plc,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom,
				const ScreenLayout *		sl )
    {
    BufferDocument *		bd= plc->plcBd;
    AppDrawingData *		add= plc->plcAdd;

    int				accepted;
    int				res;
    int				i;

    TextParticule *		tp= paraBi->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LayoutPosition		lp;
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    const int			fromLinePos= 0;

    NotesReservation		nrLine;

    lp= *lpTop;
    sp= &(paraBi->biParent->biParent->biParent)->biSectProperties;
    dg= &(sp->spDocumentGeometry);

    workTl= *resTl;
    /*
    tl->tlStroff= tp->tpStroff;
    tl->tlFirstParticule= part;
    */

    docInitNotesReservation( &nrLine );

    /*  1  */
    docAboveLine( &lp, fromLinePos, paraBi, part, dg, add );

    tl->tlTopPosition= lp;
    accepted= tl->tlParticuleCount;

    if  ( accepted < 1 )
	{ LDEB(accepted); docListItem(0,paraBi); return -1; }

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
					    tl->tlTopPosition.lpPage,
					    tl->tlTopPosition.lpColumn,
					    bd, paraBi, tl->tlFirstParticule,
					    tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( lpTop, &lp, tl, paraBi, part+ accepted,
							pf, &nrLine, add );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ *lpBottom= lp; return 0; }

    if  ( sl && sl->slLayoutLine					&&
	  (*sl->slLayoutLine)( tl, paraBi, bd, part, accepted, add,
							pf, pd ) < 0	)
	{ LDEB(accepted); return -1;	}

    tl->tlX0Pixels= pf->pfX0Pixels;
    tl->tlX1Pixels= pf->pfX1Pixels;

    for ( i= 0; i < accepted; tp++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    df->dfPage= lp.lpPage;
	    }
	}

    *lpBottom= lp;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*									*/
/*  1)  Cope with the output of sgmls: Ignore enormous space before's	*/
/*	in footers.							*/
/*									*/
/************************************************************************/

int docLayout_Line(		TextLine *			resTl,
				NotesReservation *		pNrLine,
				const BlockFrame *		bf,
				int				fromLinePos,
				const BufferItem *		paraBi,
				int				part,
				ParticuleData *			pd,
				const ParagraphLayoutContext *	plc,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom,
				const ScreenLayout *		sl )
    {
    BufferDocument *		bd= plc->plcBd;
    AppDrawingData *		add= plc->plcAdd;

    int				accepted;
    int				res;
    int				i;

    TextParticule *		tp= paraBi->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LayoutPosition		lp;
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    NotesReservation		nrLine;

    lp= *lpTop;
    sp= &(paraBi->biParent->biParent->biParent)->biSectProperties;
    dg= &(sp->spDocumentGeometry);

    docInitTextLine( tl );
    tl->tlStroff= tp->tpStroff;
    tl->tlFirstParticule= part;

    docInitNotesReservation( &nrLine );

    /*  1  */
    docAboveLine( &lp, fromLinePos, paraBi, part, dg, add );

    tl->tlTopPosition= lp;
    accepted= docLayoutLineBox( bd, tl, paraBi, part,
					&(add->addPhysicalFontList), pd, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn,
					bd, paraBi, tl->tlFirstParticule,
					tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( lpTop, &lp, tl, paraBi, part+ accepted,
							pf, &nrLine, add );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ *lpBottom= lp; return 0; }

    /* above
    tl->tlStroff= tp->tpStroff;
    tl->tlFirstParticule= part;
    */
    tl->tlStrlen=
	    tp[accepted-1].tpStroff+ tp[accepted-1].tpStrlen- tp->tpStroff;
    tl->tlParticuleCount= accepted;
    tl->tlColumnWidthTwips= bf->bfColumnWidthTwips;

    if  ( sl && sl->slLayoutLine					&&
	  (*sl->slLayoutLine)( tl, paraBi, bd, part, accepted, add,
							pf, pd ) < 0	)
	{ LDEB(accepted); return -1;	}

    tl->tlX0Pixels= pf->pfX0Pixels;
    tl->tlX1Pixels= pf->pfX1Pixels;

    for ( i= 0; i < accepted; tp++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    df->dfPage= lp.lpPage;
	    }
	}

    *lpBottom= lp;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Layout a series of lines in a paragraph.				*/
/*									*/
/*  1)  Make sure that we have scratch space for the layout routines.	*/
/*  2)  As long as there are any particules to be placed/formapped	*/
/*  3)  Can the current line be reused?					*/
/*  4)  If so just palce it at a new position.				*/
/*  5)  Otherwise recalculate layout.					*/
/*  6)  If the line does not fit on this page (column) stop.		*/
/*  7)  Cause the line to be redrawn if either it is reformatted, or it	*/
/*	is moved.							*/
/*  8)  Subtract the space needed for the footnotes in this line from	*/
/*	the space left on this page. (column)				*/
/*  9)  Insert into administration.					*/
/*  10) If the line ends in a page break, make sure nothing will fit on	*/
/*	this page (column) anymore.					*/
/*  11) Truncate the number of lines when the paragraph is completely	*/
/*	formatted.							*/
/*									*/
/************************************************************************/

int docLayoutLines(		ParagraphLayoutPosition *	plp,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				BufferItem *			paraBi )
    {
    ParagraphStripPosition *	psp= &(plp->plpProgress);
    int				part= psp->pspPart;

    int				done= 0;
    int				line= psp->pspLine;


    ParticuleData *		pd;

    LayoutPosition		lp;

    lp= plp->plpPos;

    /*  1  */
    if  ( docPsClaimParticuleData( paraBi, &pd ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}

    /*  2  */
    while( part < paraBi->biParaParticuleCount )
	{
	int			accepted;
	TextLine		boxLine;
	TextLine *		tlLine= (TextLine *)0;

	NotesReservation	nrLine;

	DocumentRectangle *	drChanged= plc->plcChangedRectanglePixels;

	docInitNotesReservation( &nrLine );

	/*  3  */
	if  ( line < paraBi->biParaLineCount )
	    {
	    const TextParticule *	tp= paraBi->biParaParticules+ part;

	    tlLine= paraBi->biParaLines+ line;

	    if  ( tlLine->tlStroff != tp->tpStroff			||
		  tlLine->tlColumnWidthTwips != bf->bfColumnWidthTwips	)
		{ tlLine= (TextLine *)0;	}
	    }

	/*  4  */
	if  ( tlLine )
	    {
	    boxLine= *tlLine;

	    accepted= docPlace_Line( &boxLine, &nrLine,
					paraBi, part, pd, plc,
					&(plp->plpFormattingFrame),
					&lp, &lp, &(plc->plcScreenLayout) );
	    }
	else{
	    const int		fromLinePos= 0;

	    /*  5  */
	    /*  NO!
	    if  ( paraBi->biParaLineCount > line )
		{ paraBi->biParaLineCount=  line; }
	    */

	    accepted= docLayout_Line( &boxLine, &nrLine, bf, fromLinePos,
					paraBi, part, pd, plc,
					&(plp->plpFormattingFrame),
					&lp, &lp, &(plc->plcScreenLayout) );
	    }

	if  ( accepted < 0 )
	    { LDEB(accepted); return -1;	}

	/*  6  */
	if  ( accepted == 0 )
	    { break;	}

	/*  7  */
	if  ( drChanged	)
	    {
	    DocumentRectangle	drBox;

	    drBox.drX0= boxLine.tlX0Pixels;
	    drBox.drX1= boxLine.tlX1Pixels;
	    drBox.drY0= TL_TOP_PIXELS( plc->plcAdd, &boxLine );
	    drBox.drY1= TL_BELOW_PIXELS( plc->plcAdd, &boxLine );

	    docUnionRectangle( drChanged, drChanged, &drBox );

	    if  ( tlLine )
		{
		DocumentRectangle	drTl;

		drTl.drX0= tlLine->tlX0Pixels;
		drTl.drX1= tlLine->tlX1Pixels;
		drTl.drY0= TL_TOP_PIXELS( plc->plcAdd, tlLine );
		drTl.drY1= TL_BELOW_PIXELS( plc->plcAdd, tlLine );

		if  ( drTl.drX0 != drBox.drX0	||
		      drTl.drX1 != drBox.drX1	||
		      drTl.drY0 != drBox.drY0	||
		      drTl.drY1 != drBox.drY1	)
		    {
		    docUnionRectangle( drChanged, drChanged, &drTl );
		    docUnionRectangle( drChanged, drChanged, &drBox );
		    }
		}
	    else{
		docUnionRectangle( drChanged, drChanged, &drBox );
		}
	    }

	/*  8  */
	docLayoutReserveNoteHeight( &(plp->plpFormattingFrame), bf, &nrLine );

	/*  9  */
	if  ( line >= paraBi->biParaLineCount )
	    {
	    tlLine= docInsertTextLine( paraBi, -1 );
	    if  ( ! tlLine )
		{ XDEB(tlLine); return -1;		}
	    }
	else{
	    tlLine= paraBi->biParaLines+ line;
	    }

	*tlLine= boxLine;

	part += accepted; done += accepted; line++;

	/*  10  */
	if  ( boxLine.tlHasPageBreak )
	    {
	    lp.lpPageYTwips= plp->plpFormattingFrame.pfPageY1Twips+ 1;
	    lp.lpAtTopOfColumn= 0;
	    break;
	    }
	}

    psp->pspLine= line;
    plp->plpPos= lp;

    /*  11  */
    if  ( part >= paraBi->biParaParticuleCount	&&
	  paraBi->biParaLineCount > line	)
	{ paraBi->biParaLineCount=  line; }

    return done;
    }

