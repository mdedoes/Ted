/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<docTextRun.h>
#   include	<docTextLine.h>
#   include	<psTextExtents.h>
#   include	<docAttributes.h>
#   include	<docParticuleData.h>
#   include	<layoutContext.h>
#   include	<psPrint.h>
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>
#   include	<textMsLocale.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a string.							*/
/*									*/
/************************************************************************/

static int docPsPrintFgInvisible(
			const struct BufferDocument *	bd,
			const struct BufferItem *	paraNode,
			const TextAttribute *		ta )
    {
    int					isFilled;
    RGB8Color				back;
    RGB8Color				fore;

    utilRGB8SolidWhite( &back );

    if  ( docGetbackgroundShading( &isFilled, &back, bd, paraNode, ta ) )
	{ LDEB(1); return -1;	}

    docGetColorByNumber( &fore, bd, ta->taTextColorNumber );

    return fore.rgb8Red == back.rgb8Red && fore.rgb8Green == back.rgb8Green && fore.rgb8Blue == back.rgb8Blue;
    }

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

    const int			asArtifact= ps->psTagDocumentStructure &&
					ps->psMarkInvisibleAsArtifact &&
					! ps->psInArtifact &&
					docPsPrintFgInvisible( dc->dcLayoutContext->lcDocument, dtl->dtlParaNode, ta );

    if  ( len > 0 )
	{
	int			y;

	if  ( asArtifact					&&
	      docPsPrintBeginArtifact( ps )			)
	    { LDEB(-1);	}

	docDrawSetFont( dc, (void *)ps, textAttrNr, ta );
	docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

	y= baseline;

	if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	    { psGetSuperBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	    { psGetSubBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	psMoveShowString( ps, s, len, x0Twips+ pd->pdLeftBorderWidth, y );

	ps->psLastPageMarked= ps->psPagesPrinted;

	if  ( asArtifact					&&
	      docPsPrintEndArtifact( ps )			)
	    { LDEB(1);	}
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

    if  ( ! ps->psInArtifact && ps->psTagDocumentStructure )
	{
	const char *	languageTag= (char *)0;
	int		textLocaleId= tr->trTextAttribute->taLocaleId;
	int		docLocaleId= tr->trDocument->bdProperties->dpDefaultLocaleId;

	if  ( textLocaleId > 0 && textLocaleId != docLocaleId )
	    { languageTag= textGetMsLocaleTagById( textLocaleId );	}

	/* Make sure that we emit text inside a /Span if needed */
	if  ( docPsPrintClaimInline( ps, languageTag )	)
	    { LDEB(1); return -1;	}

	}

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

    /* Spans in the PDF document structure are closed as part of the enclosing logic */

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
	const TextLine *	tl= dtl->dtlTextLine;
	int			lineTop= tl->tlTopPosition.lpPageYTwips;

	if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	    {
	    if  ( docPsFinishAnnotation( ps, x1Twips, lineTop, dtl->dtlLineHeight ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    psFlushLink( ps, x1Twips, lineTop, dtl->dtlLineHeight );
	    }
	}

    docPsPrintFinishInline( ps );

    return 0;
    }
