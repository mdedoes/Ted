/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<psShading.h>
#   include	<docPageGrid.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docRowProperties.h>
#   include	<docItemShading.h>
#   include	<docPropVal.h>
#   include	<docCellProperties.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

static int docHtmlStartTable(	HtmlWritingContext *		hwc,
				const struct BufferItem *	rowNode,
				const RowProperties *		rp )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				rval= 0;
    int				width;

    int				tableX0;
    int				tableX1;
    int				parentX0;
    int				parentX1;

    const int			page= 0;
    const int			column= 0;

    docBlockFrameTwips( &(hwc->hwcBlockFrame), (struct BufferItem *)rowNode,
				    hws->hwsDocument, page, column );

    docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
						rowNode->biChildren[0] );
    tableX0= hwc->hwcParagraphFrame.pfCellRect.drX0;
    if  ( rowNode->biChildCount > 1 )
	{
	docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
			rowNode->biChildren[rowNode->biChildCount- 1] );
	}
    tableX1= hwc->hwcParagraphFrame.pfCellRect.drX1;

    if  ( rowNode->biParent->biLevel == DOClevCELL )
	{
	docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
							    rowNode->biParent );
	parentX0= hwc->hwcParagraphFrame.pfCellRect.drX0;
	parentX1= hwc->hwcParagraphFrame.pfCellRect.drX1;
	}
    else{
	parentX0= hwc->hwcBlockFrame.bfContentRect.drX0;
	parentX1= hwc->hwcBlockFrame.bfContentRect.drX1;
	}

    width= ( 100* tableX1- 100* tableX0+ 50 )/ ( parentX1- parentX0 );

    docHtmlPutString( "<table style=\"", hwc );

    docCssWriteDimension( &(xw->xwColumn), sos, "width", width, "%" );
    docCssWriteProperty( &(xw->xwColumn), sos, "empty-cells", "show" );
    docCssWriteProperty( &(xw->xwColumn), sos, "border-collapse", "collapse" );

    docHtmlPutString( "\">", hwc );

    hwc->hwcTableNesting++;

  /*ready:*/

    return rval;
    }

int docHtmlFinishTable(	HtmlWritingContext *		hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    docHtmlWritelnCloseElement( "table", hwc );

    hwc->hwcTableNesting--;

    if  ( hws->hwsInlineNotes		&&
	  hwc->hwcTableNesting == 0	)
	{
	if  ( docHtmlSaveDeferredNotes( hwc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

const char * docHtmlCellElement(	const BufferItem *	cellNode )
    {
    const RowProperties *	rp= cellNode->biParent->biRowProperties;
    const CellProperties *	cp= cellNode->biCellProperties;

    if  ( rp->rpIsTableHeader || cp->cpIsRowHeader )
	{ return "th";	}
    else{ return "td";	}
    }

int docHtmlEnterCellNode( HtmlWritingContext *		hwc,
			const struct BufferItem *	rowNode,
			int				col,
			const struct BufferItem *	cellNode )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    int				wide;

    const int			page= 0;
    const int			column= 0;

    BlockFrame			bf;
    ParagraphFrame		pf;

    int				rowspan= 1;
    int				colspan= 1;

    const ItemShading *		is;

    const RowProperties *	rp= rowNode->biRowProperties;
    const CellProperties *	cp= &(rp->rpCells[col]);
    struct BufferDocument *	bd= hws->hwsDocument;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				cellPadding;

    docLayoutInitBlockFrame( &bf );

    docTableDetermineCellspans( &rowspan, &colspan, cellNode );

    docBlockFrameTwips( &bf, cellNode->biChildren[0],
					    bd, page, column );

    docCellFrameTwips( &pf, &bf, cellNode );
    wide= TWIPS_TO_PIXELS( pf.pfCellContentRect.drX1-
					    pf.pfCellContentRect.drX0 );

    docHtmlPutString( "<", hwc );
    docHtmlPutString( docHtmlCellElement( cellNode ), hwc );

    if  ( colspan > 1 )
	{ docHtmlWriteIntAttribute( hwc, "colspan", colspan );	}
    if  ( rowspan > 1 )
	{ docHtmlWriteIntAttribute( hwc, "rowspan", rowspan );	}


    is= docGetItemShadingByNumber( bd, cp->cpShadingNumber );

    docHtmlPutString( " style=\"", hwc );

    docCssWriteDimension( &(xw->xwColumn), sos, "width", wide, "px" );

    if  ( is->isPattern == DOCspSOLID )
	{
	docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, is );
	}

    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			"border-top", cp->cpTopBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			"border-left", cp->cpLeftBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			"border-right", cp->cpRightBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			"border-bottom", cp->cpBottomBorderNumber );

    switch( cp->cpValign )
	{
	case DOCtvaTOP:
	    docCssWriteProperty( &(xw->xwColumn), sos,
					    "vertical-align", "top" );
	    break;
	case DOCtvaCENTERED:
	    docCssWriteProperty( &(xw->xwColumn), sos,
					    "vertical-align", "middle" );
	    break;
	case DOCtvaBOTTOM:
	    docCssWriteProperty( &(xw->xwColumn), sos,
					    "vertical-align", "bottom" );
	    break;

	default:
	    LDEB(cp->cpValign);
	    break;
	}

    cellPadding= TWIPS_TO_PIXELS( rp->rpHalfGapWidthTwips )- 4;
    if  ( cellPadding > 1 )
	{
	docCssWriteDimension( &(xw->xwColumn), sos,
				    "padding-left", cellPadding, "pt" );
	docCssWriteDimension( &(xw->xwColumn), sos,
				    "padding-right", cellPadding, "pt" );
	}

    docHtmlPutString( "\"", hwc );

    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

int docHtmlEnterRowNode(	HtmlWritingContext *		hwc,
				const struct BufferItem *	rowNode )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;
    struct BufferDocument *	bd= hws->hwsDocument;

    int				high;
    int				tableNesting= docTableNesting( rowNode );

    const RowProperties *	rp= rowNode->biRowProperties;
    const ItemShading *		is;

    if  ( hwc->hwcTableNesting == tableNesting			&&
	  rowNode->biRowTableFirst == rowNode->biNumberInParent	)
	{ docHtmlFinishTable( hwc );	}

    if  ( hwc->hwcTableNesting < tableNesting )
	{ docHtmlStartTable( hwc, rowNode, rp );	}

    is= docGetItemShadingByNumber( bd, rp->rpShadingNumber );

    docHtmlPutString( "<tr style=\"", hwc );

    docCssWriteProperty( &(xw->xwColumn), sos, "vertical-align", "top" );

    if  ( is->isPattern == DOCspSOLID )
	{
	docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, is );
	}

    high= rp->rpHeightTwips;
    if  ( high < 0 )
	{ high= -high;	}
    high= TWIPS_TO_PIXELS( high );
    if  ( high > 0 )
	{
	docCssWriteDimension( &(xw->xwColumn), sos, "height", high, "px" );
	}

    docHtmlPutString( "\">", hwc );

    return 0;
    }
