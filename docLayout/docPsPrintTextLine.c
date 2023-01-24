/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<docTextRun.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<psTextExtents.h>
#   include	"docParticuleData.h"
#   include	<psPrint.h>
#   include	<docFields.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docPsPrintRunUnderline(	const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    psUnderlineGeometry( &y0, &h, baseLine->lpPageYTwips,
				TA_FONT_SIZE_TWIPS( ta ), pd[part].pdAfi );
    if  ( h < 10 )
	{ h=  10;	}

    if  ( ps->psTagDocumentStructure		&&
	  docPsPrintBeginArtifact( dc, ps )	)
	{ LDEB(-1); return -1;	}

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    if  ( ps->psTagDocumentStructure			&&
	  docPsPrintEndMarkedContent( dc, ps )		)
	{ LDEB(1); return -1;	}

    return 0;
    }

int docPsPrintRunStrikethrough(	const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    psStrikethroughGeometry( &y0, &h, baseLine->lpPageYTwips,
				TA_FONT_SIZE_TWIPS( ta ), pd[part].pdAfi );
    if  ( h < 10 )
	{ h=  10;	}

    if  ( ps->psTagDocumentStructure		&&
	  docPsPrintBeginArtifact( dc, ps )	)
	{ LDEB(-1); return -1;	}

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    if  ( ps->psTagDocumentStructure			&&
	  docPsPrintEndMarkedContent( dc, ps )		)
	{ LDEB(1); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Print a string.							*/
/*									*/
/************************************************************************/

static void docPsPrintString(	const DrawTextLine *		dtl,
				const ParticuleData *		pd,
				int				baseline,
				int				x0Twips,
				int				textAttrNr,
				const TextAttribute *		ta,
				const char *			s,
				int				len )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

    if  ( len > 0 )
	{
	int			y;

	docDrawSetFont( dc, (void *)ps, textAttrNr, ta );
	docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

	y= baseline;

	if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	    { psGetSuperBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	    { psGetSubBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	psMoveShowString( ps, s, len, x0Twips+ pd->pdLeftBorderWidth, y );

	ps->psLastPageMarked= ps->psPagesPrinted;
	}

    return;
    }

static void docPsPrintSegment(	const DrawTextLine *		dtl,
				int				textAttrNr,
				const TextAttribute *		ta,
				const char *			s,
				int				len )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    docDrawSetFont( dc, (void *)ps, textAttrNr, ta );
    psShowString( ps, s, len );
    return;
    }

static void docPsPrintSegments(	const DrawTextLine *		dtl,
				const ParticuleData *		pd,
				int				baseLine,
				int				x0Twips,
				int				textAttrNr,
				const TextAttribute *		ta,
				const char *			s,
				const int *			segments,
				int				segmentCount )
    {
    TextAttribute		taN= *ta;
    int				seg;

    taN.taSmallCaps= 0;

    if  ( segments[0] > 0 )
	{
	docPsPrintString( dtl, pd, baseLine, x0Twips,
				    textAttrNr, &taN, s, segments[0] );
	s += segments[0];

	docPsPrintSegment( dtl, textAttrNr, ta, s, segments[1] );
	s += segments[1];
	}
    else{
	docPsPrintString( dtl, pd, baseLine, x0Twips,
				    textAttrNr, ta, s, segments[1] );
	s += segments[1];
	}

    for ( seg= 1; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    docPsPrintSegment( dtl, textAttrNr, &taN, s, segments[2* seg+ 0] );
	    s += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    docPsPrintSegment( dtl, textAttrNr, ta, s, segments[2* seg+ 1] );
	    s += segments[2* seg+ 1];
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

int docPsPrintTextRun(		const TextRun *		tr,
				int			x0Twips,
				int			x1Twips,
				const DrawTextLine *	dtl,
				const LayoutPosition *	baseLine,
				const char *		outputString )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    int				rval= 0;

    const char *		printString= outputString;
    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				spanBaseline= baseLine->lpPageYTwips;
    const ParticuleData *	pd= dtl->dtlParticuleData+ tr->trPartFrom;

    if  ( psMakeOutputString( &printString, &scratchString,
				    &segments, &segmentCount,
				    tr->trTextAttribute, tr->trDirection,
				    outputString, tr->trStrlen ) )
	{ LDEB(tr->trStrlen); rval= -1; goto ready;	}

    if  ( segmentCount > 0 )
	{
	docPsPrintSegments( dtl, pd, spanBaseline, x0Twips,
				tr->trTextAttributeNr, tr->trTextAttribute,
				printString, segments, segmentCount );
	}
    else{
	docPsPrintString( dtl, pd, spanBaseline, x0Twips,
				tr->trTextAttributeNr, tr->trTextAttribute,
				printString, tr->trStrlen );
	}

    ps->psLinkParticulesDone += ( tr->trPartUpto- tr->trPartFrom );

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docPsPrintFtnsep(		const DrawTextLine *	dtl,
				int			part,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    int				fontSizeTwips;

    int				xHeight;

    int				y0;
    int				h;

    fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    y0= baseLine->lpPageYTwips- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );
    psFillRectangle( ps, x0Twips, y0, x1Twips- x0Twips, h );

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Start a field							*/
/*									*/
/************************************************************************/

int docPsPrintStartField(	const DrawTextLine *		dtl,
				int				part,
				int				x0Twips,
				const struct DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    const TextLine *		tl= dtl->dtlTextLine;
    int				lineTop= tl->tlTopPosition.lpPageYTwips;

    struct BufferItem *		paraNode= dtl->dtlParaNode;

    const MemoryBuffer *	mbBookmark;

    MemoryBuffer		mbSource;
    MemoryBuffer		mbTarget;

    HyperlinkField		hf;

    docInitHyperlinkField( &hf );

    utilInitMemoryBuffer( &mbSource );
    utilInitMemoryBuffer( &mbTarget );

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	    {
	    psDestPdfmark( ps, lineTop, mbBookmark );
	    }

	goto ready;
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	int			cnt;
	int			closed;

	cnt= docCountParticulesInFieldFwd( paraNode, &closed, part,
					paraNode->biParaParticuleCount );
	if  ( cnt < 0 )
	    { LDEB(cnt); }

	if  ( docSetNoteLinks( &mbTarget, &mbSource, paraNode, df ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/* Include destination to jump to from the target */
	psDestPdfmark( ps, lineTop, &mbSource );

	/* Remember what the target is to use that at the end of the link */
	utilEmptyMemoryBuffer( &(ps->psLinkFile) );

	utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
	if  ( paraNode->biTreeType == DOCinBODY )
	    {
	    struct DocumentNote *	dn;

	    dn= docGetNoteOfField( df, dtl->dtlDocument );
	    if  ( dn )
		{
		if  ( docCollectNoteTitle( &(ps->psLinkTitle),
							dn, dtl->dtlDocument ) )
		    { LDEB(1); rval= -1;	}
		}
	    }

	if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &mbTarget ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ps->psInsideLink= 1;
	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x0Twips;

	goto ready;
	}

    if  ( df->dfKind == DOCfkHYPERLINK )
	{
	if  ( ! docGetHyperlinkField( &hf, df ) )
	    {
	    ps->psInsideLink= 1;
	    ps->psLinkParticulesDone= 0;
	    ps->psLinkRectLeft= x0Twips;

	    utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
	    if  ( utilCopyMemoryBuffer( &(ps->psLinkFile), &(hf.hfFile) ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &(hf.hfBookmark) ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	goto ready;
	}

  ready:

    utilCleanMemoryBuffer( &mbSource );
    utilCleanMemoryBuffer( &mbTarget );

    docCleanHyperlinkField( &hf );

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docPsPrintFinishField(	const DrawTextLine *	dtl,
				int			part,
				int			x1Twips,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psInsideLink )
	{
	const TextLine *	tl= dtl->dtlTextLine;
	int			lineTop= tl->tlTopPosition.lpPageYTwips;

	psFlushLink( ps, x1Twips, lineTop, dtl->dtlLineHeight );
	ps->psInsideLink= 0;
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Print the lines in the paragraph.					*/
/*									*/
/************************************************************************/

int docPsPrintStartTextLine(	struct DrawTextLine *		dtl,
				int				x0Twips )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psInsideLink )
	{ ps->psLinkRectLeft= dtl->dtlX;	}

    return 0;
    }

int docPsPrintFinishTextLine(	const struct DrawTextLine *	dtl,
				int				x1Twips )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psInsideLink )
	{
	TextLine *	tl= dtl->dtlTextLine;
	int		lineTop= tl->tlTopPosition.lpPageYTwips;

	psFlushLink( ps, x1Twips, lineTop, dtl->dtlLineHeight );
	}

    return 0;
    }

