/************************************************************************/
/*									*/
/*  Print SVG : draw one line of text.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>

#   include	<psTextExtents.h>
#   include	"docSvgDrawImpl.h"
#   include	<docDraw.h>
#   include	<docDrawLine.h>
#   include	<docTextRun.h>
#   include	<docTabStop.h>
#   include	<fontDocFont.h>
#   include	<fontDocFontList.h>
#   include	<docDocumentProperties.h>
#   include	<svgWriter.h>
#   include	<docParticuleData.h>
#   include	<docBuf.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

int docSvgDrawTextRun(		const TextRun *		tr,
				int			x0Twips,
				int			x1Twips,
				const DrawTextLine *	dtl,
				const LayoutPosition *	baseline,
				const char *		printString )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				fontSizeTwips;
    int				textSizeTwips;

    int				spanBaseline= baseline->lpPageYTwips;

    const ParticuleData *	pd= dtl->dtlParticuleData+ tr->trPartFrom;

    fontSizeTwips= TA_FONT_SIZE_TWIPS( tr->trTextAttribute );
    textSizeTwips= fontSizeTwips;

    if  ( tr->trStrlen > 0 )
	{
	int			y;
	const DocumentFont *	df;

	docDrawSetFont( dc, (void *)sw,
			    tr->trTextAttributeNr, tr->trTextAttribute );
	docDrawSetColorNumber( dc, (void *)sw,
			    tr->trTextAttribute->taTextColorNumber );

	df= fontFontListGetFontByNumber(
		    dc->dcLayoutContext->lcDocument->bdProperties->dpFontList,
		    tr->trTextAttribute->taFontNumber );

	y= spanBaseline;

	if  ( tr->trTextAttribute->taSuperSub == TEXTvaSUPERSCRIPT )
	    {
	    psGetSuperBaseline( &y, spanBaseline, fontSizeTwips, pd->pdAfi );
	    textSizeTwips= SUPERSUB_SIZE( fontSizeTwips );
	    }

	if  ( tr->trTextAttribute->taSuperSub == TEXTvaSUBSCRIPT )
	    {
	    psGetSubBaseline( &y, spanBaseline, fontSizeTwips, pd->pdAfi );
	    textSizeTwips= SUPERSUB_SIZE( fontSizeTwips );
	    }

	xmlPutString( "<text ", xw );

#	if 1
	xmlWriteBufferAttribute( xw, "font-family", &(df->dfName) );
	xmlWriteIntAttribute( xw, "font-size", textSizeTwips );
	xmlWriteRgb8Attribute( xw, "fill", &(dc->dcCurrentColor) );
	if  ( tr->trTextAttribute->taFontIsSlanted )
	    { xmlWriteStringAttribute( xw, "font-style", "italic" );	}
	if  ( tr->trTextAttribute->taFontIsBold )
	    { xmlWriteStringAttribute( xw, "font-weight", "bold" );	}

	if  ( tr->trTextAttribute->taTextIsUnderlined )
	    {
	    /* Does not work with Firefox */
	    xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcCurrentColor) );
	    xmlWriteStringAttribute( xw, "text-decoration", "underline" );
	    }
#	else
	{
	char	scratch[25];

	sprintf( scratch, "t%d", textAttrNr );
	xmlWriteStringAttribute( xw, "class", scratch );
	xmlWriteIntAttribute( xw, "font-size", textSizeTwips );
	}
#	endif

	xmlWriteIntAttribute( xw, "x", x0Twips+ pd->pdLeftBorderWidth );
	xmlWriteIntAttribute( xw, "y", y );

	xmlPutString( ">", xw );
	xmlNewLine( xw );

	xmlEscapeCharacters( xw, printString, tr->trStrlen );
	xmlPutString( "</text>", xw );
	xmlNewLine( xw );
	}

    return 0;
    }

int docSvgDrawFtnsep(		const DrawTextLine *	dtl,
				int			part,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				fontSizeTwips;

    int				xHeight;

    int				y0;
    int				h;

    fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    y0= baseLine->lpPageYTwips- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)sw, ta->taTextColorNumber );

    xmlPutString( "<line ", xw );
    xmlWriteIntAttribute( xw, "x1", x0Twips );
    xmlWriteIntAttribute( xw, "y1", y0 );
    xmlWriteIntAttribute( xw, "x2", x1Twips );
    xmlWriteIntAttribute( xw, "y2", y0 );
    xmlWriteIntAttribute( xw, "stroke-width", h );
    xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcCurrentColor) );
    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw tabs.								*/
/*									*/
/************************************************************************/

static int svgDrawTab(		DrawingContext *	dc,
				const DrawTextLine *	dtl,
				XmlWriter *		xw,
				const TextAttribute *	ta,
				int			x0,
				int			x1,
				int			baseLine,
				int			step,
				const char *		tabProc )
    {
    x0= step* ( ( x0+ step- 1 )/ step );
    if  ( x1 <= x0 )
	{ return 0;	}

LDEB(1);
    return 0;
    }

int docSvgDrawTab(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);

    int				x0= x0Twips+ dtl->dtlLineHeight/ 4;
    int				x1= x1Twips- dtl->dtlLineHeight/2;

    int				spanBaseline= baseLine->lpPageYTwips;

    switch( leader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							60, "dot-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							60, "dot-tab" );
		}

	    break;

	case DOCtlUNDERLINE:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							20, "ul-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							20, "ul-tab" );
		}

	    break;

	case DOCtlHYPH:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							140, "dash-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							140, "dash-tab" );
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
/*  Print the lines in the paragraph.					*/
/*									*/
/************************************************************************/

int docSvgStartTextLine(	struct DrawTextLine *		dtl,
				int				x0Twips )
    { return 0;	}

