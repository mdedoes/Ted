/************************************************************************/
/*									*/
/*  Draw the shape text in Scalable Vector Graphics.			*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<docDraw.h>
#   include	<docShape.h>
#   include	<svgWriter.h>
#   include	"docSvgDrawImpl.h"
#   include	"docHtmlWriteImpl.h"
#   include	"layoutContext.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw the text inside a shape as HTML.				*/
/*									*/
/*  This is an alternative for the standard behavior implemented by	*/
/*  docDrawShapeText().							*/
/*									*/
/************************************************************************/

int docSvgDrawShapeText(	const DocumentRectangle *	drHere,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsw )
    {
    int				rval= 0;
    SvgWriter *			sw= (SvgWriter *)vsw;
    XmlWriter *			xw= &(sw->swXmlWriter);
    const LayoutContext *	lc= dc->dcLayoutContext;
    char			bodyStyle[100+1];

    DocumentRectangle	drRaw;

    HtmlWritingSettings	hws;
    HtmlWritingContext	hwc;

    docInitHtmlWritingSettings( &hws );
    docInitHtmlWritingContext( &hwc );

    hws.hwsLayoutContext= lc;
    hws.hwsDocument= lc->lcDocument;

    docStartHtmlWritingContext( &hwc, &hws, xw->xwSos );

    hwc.hwcXmlWriter.xwSos= xw->xwSos;

    drRaw.drX0= 0;
    drRaw.drY0= 0;
    drRaw.drX1= ( sw->swWide* ( drHere->drX1- drHere->drX0+ 1 ) )/
			    ( sw->swViewBox.drX1- sw->swViewBox.drX0+ 1 )- 1;
    drRaw.drY1= ( sw->swHigh* ( drHere->drY1- drHere->drY0+ 1 ) )/
			    ( sw->swViewBox.drY1- sw->swViewBox.drY0+ 1 )- 1;

    sprintf( bodyStyle, "width:%dpx;padding:0;margin:0;line-height:100%%",
					    drRaw.drX1- drRaw.drX0+ 1 );

    xmlPutString( "<svg ", xw );
    svgWriteRectangleAttributes( sw, drHere );
    svgWriteViewBoxAttribute( sw, "viewBox", &drRaw );
    xmlPutString( ">", xw );
    xmlPutString( "<switch>", xw );
    xmlNewLine( xw );

    xmlPutString( "<foreignObject ", xw );
    svgWriteRectangleAttributes( sw, &drRaw );
    xmlWriteStringAttribute( xw, "requiredExtensions", "http://example.com/SVGExtensions/EmbeddedXHTML" );
    xmlPutString( ">", xw );
    xmlNewLine( xw );

    xmlPutString( "<body ", xw );
    xmlWriteStringAttribute( xw, "xmlns", "http://www.w3.org/1999/xhtml" );
    xmlWriteStringAttribute( xw, "style", bodyStyle );
    xmlPutString( ">", xw );
    xmlNewLine( xw );

    if  ( docHtmlSaveSelection( &hwc, &(ds->dsDocumentTree),
					(const struct DocumentSelection *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    xmlPutString( "</body>", xw );
    xmlNewLine( xw );
    xmlPutString( "</foreignObject>", xw );
    xmlNewLine( xw );

    xmlPutString( "<g>", xw );
    xmlNewLine( xw );

    if  ( docDrawShapeText( drHere, bodySectNode, page, column, ds, dc, vsw ) )
	{ LDEB(1); rval= -1; goto ready;	}

    xmlPutString( "</g>", xw );
    xmlNewLine( xw );

    xmlPutString( "</switch>", xw );
    xmlPutString( "</svg>", xw );
    xmlNewLine( xw );


  ready:

    docCleanHtmlWritingContext( &hwc );

    return rval;
    }

