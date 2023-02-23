/************************************************************************/
/*									*/
/*  Save a paragraph to HTML.						*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docHtmlWriteParagraph.h"
#   include	"docWriteCss.h"
#   include	<docTreeNode.h>
#   include	<psShading.h>
#   include	<docTextParticule.h>
#   include	<docPropVal.h>
#   include	<docItemShading.h>
#   include	<utilPropMask.h>
#   include	<docSelect.h>
#   include	<docAttributes.h>
#   include	<docBreakKind.h>
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>
#   include	<docTreeType.h>
#   include	<docParaParticules.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	USE_CSS_GRID	1

static void docHtmlEmitWholeParagraphStyle(
					HtmlParagraphWriter *		hpw )
    {
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    const ItemShading *		is;

    switch( pp->ppBreakKind )
	{
	case DOCibkPAGE:
	case DOCibkCOL:
	    docCssWriteProperty( &(xw->xwColumn), sos,
					    "page-break-before", "always" );

	    if  ( pp->ppSpaceBeforeTwips == 0 )
		{
		const DocumentProperties *	dp= bd->bdProperties;
		const DocumentGeometry *	dg= &(dp->dpGeometry);

		docCssWriteDimension( &(xw->xwColumn), sos,
			"margin-top", dg->dgMargins.roTopOffset/ 20, "pt" );
		}
	    break;
	default:
	    break;
	}

    is= docGetItemShadingByNumber( bd, pp->ppShadingNumber );

    if  ( is->isPattern == DOCspSOLID )
	{ docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, is ); }

    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-top", pp->ppTopBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-left", pp->ppLeftBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-right", pp->ppRightBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-bottom", pp->ppBottomBorderNumber );

    if  ( pp->ppSpaceBeforeTwips > 0 )
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "margin-top",
				( pp->ppSpaceBeforeTwips+ 10 )/ 20, "pt" );
	}
    if  ( pp->ppSpaceAfterTwips > 0 )
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "margin-bottom",
				( pp->ppSpaceAfterTwips+ 10 )/ 20, "pt" );
	}
    }

static void docHtmlEmitParagraphBodyStyle(
					HtmlParagraphWriter *		hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    switch( pp->ppAlignment )
	{
	case DOCthaLEFT:
	    break;
	case DOCthaRIGHT:
	    docCssWriteProperty( &(xw->xwColumn), sos, "text-align", "right" );
	    break;

	case DOCthaCENTERED:
	    docCssWriteProperty( &(xw->xwColumn), sos, "text-align", "center" );
	    break;

	case DOCthaJUSTIFIED:
	    docCssWriteProperty( &(xw->xwColumn), sos, "text-align", "justify" );
	    break;

	default:
	    LDEB(pp->ppAlignment);
	    break;
	}

    }

static int docHtmlCheckForBullet(	HtmlParagraphWriter *	hpw )
    {
    BufferItem *		paraNode= hpw->hpwParaNode;
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;

    if  ( pp->ppLeftIndentTwips >=  100		&&
	  pp->ppFirstIndentTwips <= 100		)
	{
	int			x;
	int			part= 0;
	const TextParticule *	tp;

	x= 0;
	part= 0;
	tp= paraNode->biParaParticules;
	for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	    {
	    x += tp->tpWide;

	    if  ( tp->tpKind == TPkindTAB )
		{
		if  ( x <= pp->ppLeftIndentTwips )
		    {
		    const int		direction= 1;

		    hpw->hpwUseGridForBullet= 1;

		    docSetParaSelection( &(hpw->hpwBulletSelection),
				paraNode, direction,
				0, tp->tpStroff );
		    docSetParaSelection( &(hpw->hpwBodySelection),
				paraNode, direction,
				tp->tpStroff+ tp->tpStrlen,
				docParaStrlen( paraNode ) );

#		    if 0
		    if  ( docCollectReference( &(hpw->hpwBulletContent),
				&(hpw->hpwBulletSelection), hpw->hpwDocument ) )
			{ LDEB(1); return -1;	}
#		    endif
		    }

		break;
		}
	    }
	}

    return 0;
    }

static int docHtmlTextIndentsToStyle(	HtmlParagraphWriter *	hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    if  ( pp->ppLeftIndentTwips <= -100	||
	  pp->ppLeftIndentTwips >=  100	)
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "padding-left",
				( pp->ppLeftIndentTwips+ 10 )/ 20, "pt" );
	}

    if  ( pp->ppFirstIndentTwips <= -100	||
	  pp->ppFirstIndentTwips >=  100	)
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "text-indent",
				pp->ppFirstIndentTwips/ 20, "pt" );
	}

    return 0;
    }

static int docHtmlStartBulletedRow(	HtmlParagraphWriter *	hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

#   if USE_CSS_GRID
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;
#   endif

    docHtmlPutString( "<div", hwc );
    docHtmlPutString( " style=\"", hwc );

#   if USE_CSS_GRID
    docCssWriteProperty( &(xw->xwColumn), sos, "display", "grid" );
    docCssWriteProperty( &(xw->xwColumn), sos,
				"grid-template-rows", "100%" );
    /* Filthy HACK! */
    docCssWriteDimension( &(xw->xwColumn), sos, "grid-template-columns",
			( pp->ppLeftIndentTwips+ 10 )/ 20, "pt auto" );
#   endif

    docHtmlEmitWholeParagraphStyle( hpw );

    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );

    return 0;
    }

static int docHtmlWriteBulletColumn(	HtmlParagraphWriter *	hpw )
    {
    const ParagraphProperties *	pp= hpw->hpwParagraphProperties;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				bulletIndent;

    bulletIndent= pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips;

    docHtmlPutString( "<div", hwc );
    docHtmlPutString( " style=\"", hwc );

#   if USE_CSS_GRID
    docCssWriteNumber( &(xw->xwColumn), sos, "grid-column-start", 1 );
#   else
    docCssWriteProperty( &(xw->xwColumn), sos, "display", "inline-block" );
    docCssWriteDimension( &(xw->xwColumn), sos, "width",
			( pp->ppLeftIndentTwips+ 10 )/ 20, "pt" );
#   endif

    if  ( bulletIndent >= 100 )
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "text-indent",
						bulletIndent/ 20, "pt" );
	}

    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );

    if  ( docHtmlEmitParagraphSelection( hpw, &(hpw->hpwBulletSelection) ) )
	{ LDEB(hpw->hpwUseGridForBullet); return -1;	}

    docHtmlPutString( "</div>", hwc );

    return 0;
    }

static int docHtmlWriteBodyColumn(	HtmlParagraphWriter *	hpw )
    {
#   if USE_CSS_GRID
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;
#   endif

#   if USE_CSS_GRID
    docHtmlPutString( "<div", hwc );
    docHtmlPutString( " style=\"", hwc );

    docCssWriteNumber( &(xw->xwColumn), sos, "grid-column-start", 2 );
#   endif

    docHtmlEmitParagraphBodyStyle( hpw );

#   if USE_CSS_GRID
    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );
#   endif

    if  ( docHtmlEmitParagraphSelection( hpw, &(hpw->hpwBodySelection) ) )
	{ LDEB(hpw->hpwUseGridForBullet); return -1;	}

#   if USE_CSS_GRID
    docHtmlPutString( "</div>", hwc );
#   endif

    return 0;
    }

static int docHtmlWriteUnbulletedParagraph(	HtmlParagraphWriter *	hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

  docHtmlPutString( "<div", hwc );
    docHtmlPutString( " style=\"", hwc );

    if  ( docHtmlTextIndentsToStyle( hpw ) )
	{ LDEB(1); return -1;	}

    docHtmlEmitWholeParagraphStyle( hpw );
    docHtmlEmitParagraphBodyStyle( hpw );

    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );

    if  ( docHtmlEmitParagraphSelection( hpw, hpw->hpwParagraphSelection ) )
	{ LDEB(hpw->hpwUseGridForBullet); return -1;	}

    docHtmlWritelnCloseElement( "div", hwc );

    return 0;
    }

static int docHtmlWriteBulletedParagraph(	HtmlParagraphWriter *	hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

    if  ( docHtmlStartBulletedRow( hpw ) )
	{ LDEB(1); return -1;	}

    if  ( docHtmlWriteBulletColumn( hpw ) )
	{ LDEB(1); return -1;	}

    if  ( docHtmlWriteBodyColumn( hpw ) )
	{ LDEB(1); return -1;	}

    docHtmlWritelnCloseElement( "div", hwc );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a 'Paragraph'							*/
/*									*/
/*  But not as a <P>...</P>.						*/
/*									*/
/************************************************************************/

int docHtmlSaveParaNode( HtmlWritingContext *			hwc,
			struct BufferItem *			paraNode,
			const struct BufferItem *		bodySectNode,
			const struct DocumentSelection *	ds )
    {
    int					rval= 0;

    const HtmlWritingSettings *		hws= hwc->hwcSettings;

    PropertyMask			ppSetMask;

    HtmlParagraphWriter			hpw;

    hpw.hpwHtmlWriter= hwc;
    hpw.hpwAfterSpace= 0;

    hpw.hpwUseGridForBullet= 0;

    hpw.hpwParaNode= paraNode;
    hpw.hpwParagraphProperties= paraNode->biParaProperties;
    hpw.hpwDocument= hws->hwsDocument;
    utilInitMemoryBuffer( &(hpw.hpwBulletContent) );

    docInitDocumentSelection( &(hpw.hpwBulletSelection) );
    docInitDocumentSelection( &(hpw.hpwBodySelection) );
    hpw.hpwParagraphSelection= ds;

    if  ( docParagraphIsEmpty( paraNode ) )
	{
	docHtmlPutString( "<div>&#160;</div>", hwc );
	docHtmlNewLine( hwc );
	goto ready;
	}

    if  ( docHtmlCheckForBullet( &hpw ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( hpw.hpwUseGridForBullet )
	{
	if  ( docHtmlWriteBulletedParagraph( &hpw ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( docHtmlWriteUnbulletedParagraph( &hpw ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( hws->hwsInlineNotes			&&
	  paraNode->biTreeType == DOCinBODY	&&
	  hwc->hwcTableNesting == 0		)
	{
	if  ( docHtmlSaveDeferredNotes( hwc ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
			    &(hwc->hwcParagraphProperties),
			    &ppSetMask, paraNode->biParaProperties,
			    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1);	}

  ready:

    utilCleanMemoryBuffer( &(hpw.hpwBulletContent) );

    return rval;
    }
