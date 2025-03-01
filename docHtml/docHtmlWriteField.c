/************************************************************************/
/*									*/
/*  Save the fields in a document to HTML.				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	"docHtmlWriteImpl.h"
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docField.h>
#   include	<docDocPropertyField.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
			int				isNote,
			const MemoryBuffer *		fileName,
			const MemoryBuffer *		mbTarget,
			const MemoryBuffer *		mbSource,
			const MemoryBuffer *		mbTitle )
    {
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				afterSpace;
    int				needed= 0;

    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( mbTarget && utilMemoryBufferIsEmpty( mbTarget ) )
	{ mbTarget= (const MemoryBuffer *)0;	}
    if  ( mbSource && utilMemoryBufferIsEmpty( mbSource ) )
	{ mbSource= (const MemoryBuffer *)0;	}

    docHtmlPutString( "<a", hwc );

    if  ( isNote )
	{
	docHtmlWriteStringAttribute( hwc, "style", "text-decoration:none" );
	}
    afterSpace= 0;

    if  ( fileName || mbTarget )
	{
	needed += 1+ 6;
	if  ( fileName )
	    { needed += fileName->mbSize;	}
	if  ( mbTarget )
	    { needed += 1+ mbTarget->mbSize;	}
	needed += 1;
	}

    if  ( mbSource )
	{ needed += 1+ 6+ mbSource->mbSize+ 1;	}

    if  ( xw->xwColumn > 5		&&
	  xw->xwColumn+ needed > 76	)
	{ docHtmlNewLine( hwc ); afterSpace= 1;		}

    if  ( fileName || mbTarget )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "href=\"", hwc );

	if  ( fileName )
	    { xmlEscapeBuffer( xw, fileName );	}

	if  ( mbTarget )
	    {
	    (void)sioOutPutByte( '#', sos );
	    xmlEscapeBuffer( xw, mbTarget );
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( mbSource )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "id=\"", hwc );
	xmlEscapeBuffer( xw, mbSource );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( mbTitle && ! utilMemoryBufferIsEmpty( mbTitle ) )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "title=\"", hwc );
	xmlEscapeBuffer( xw, mbTitle );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    docHtmlPutString( ">", hwc );

    return 0;
    }

static void docHtmlStartFieldText(	const DocumentField *		df,
					const FieldKindInformation *	fki,
					HtmlWritingContext *		hwc )
    {
    MemoryBuffer	mbClasses;
    DocPropertyField	dpf;


    utilInitMemoryBuffer( &mbClasses );
    docInitDocPropertyField( &dpf );

    utilMemoryBufferAppendString( &mbClasses, docFieldKindStr( df->dfKind ) );

    switch( df->dfKind )
	{
	case DOCfkDOCPROPERTY:
	if  ( ! docGetDocPropertyField( &dpf, df ) )
	    {
	    utilMemoryBufferAppendString( &mbClasses, " " );
	    utilMemoryAppendBuffer( &mbClasses, &(dpf.dpfPropertyName) );
	    }
	default:
	    break;
	}

    docHtmlPutString( "<span class=\"", hwc );
    xmlEscapeBuffer( &(hwc->hwcXmlWriter), &mbClasses );
    docHtmlPutString( "\">", hwc );

    docCleanDocPropertyField( &dpf );
    utilCleanMemoryBuffer( &mbClasses );

    return;
    }

static void docHtmlFinishFieldText(	HtmlWritingContext *	hwc )
    {
    docHtmlPutString( "</span>", hwc );
    return;
    }

int docHtmlStartField(	const DocumentField *		df,
			HtmlWritingContext *		hwc,
			const struct BufferItem *	node,
			int				attNr )
    {
    int				rval= 0;

    HyperlinkField		hf;
    const MemoryBuffer *	mbBookmark= (const MemoryBuffer *)0;

    const FieldKindInformation * fki= DOC_FieldKinds+ df->dfKind;

    docInitHyperlinkField( &hf );

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    hwc->hwcInHyperlink++;
	    if  ( /*! hwc->hwcInBookmark &&*/ hwc->hwcInHyperlink == 1 )
		{ docHtmlStartNote( df, hwc, node, attNr );	}
	    break;

	case DOCfkHYPERLINK:
	    hwc->hwcInHyperlink++;
	    if  ( /*! hwc->hwcInBookmark &&*/ hwc->hwcInHyperlink == 1 )
		{
		if  ( ! docGetHyperlinkField( &hf, df ) )
		    {
		    const int	isNote= 0;

		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc, isNote,
					    &(hf.hfFile), &(hf.hfBookmark),
					    (const MemoryBuffer *)0,
					    (const MemoryBuffer *)0 );

		    docHtmlChangeAttributes( hwc, attNr );

		    hwc->hwcBytesInLink= 0;
		    }
		}
	    break;

	case DOCfkBOOKMARK:
	    hwc->hwcInBookmark++;
	    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
		{
		if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	)
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlPutString( "<span id=\"", hwc );
		    xmlEscapeBuffer( &(hwc->hwcXmlWriter), mbBookmark );
		    docHtmlPutString( "\">", hwc );

		    docHtmlChangeAttributes( hwc, attNr );
		    }
		else{
		    docHtmlPutString( "<span id=\"", hwc );
		    xmlEscapeBuffer( &(hwc->hwcXmlWriter), mbBookmark );
		    docHtmlPutString( "\"/>", hwc );
		    }
		}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref++;
	    if  ( fki->fkiSingleParagraph )
		{ docHtmlStartFieldText( df, fki, hwc );	}
	    break;

	default:
	    if  ( fki->fkiSingleParagraph )
		{ docHtmlStartFieldText( df, fki, hwc ); }
	    break;
	}

    docCleanHyperlinkField( &hf );

    return rval;
    }

int docHtmlFinishField(	const DocumentField *		df,
			HtmlWritingContext *		hwc )
    {
    const FieldKindInformation * fki= DOC_FieldKinds+ df->dfKind;

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	case DOCfkHYPERLINK:
	    if  ( /*! hwc->hwcInBookmark &&*/ hwc->hwcInHyperlink == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</a>", hwc );
		}
	    hwc->hwcInHyperlink--;
	    if  ( hwc->hwcInHyperlink < 0 )
		{ hwc->hwcInHyperlink= 0;	}
	    break;

	case DOCfkBOOKMARK:
	    if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	)
		{
		docHtmlChangeAttributes( hwc, -1 );
		docHtmlPutString( "</span>", hwc );
		}

	    hwc->hwcInBookmark--;
	    if  ( hwc->hwcInBookmark < 0 )
		{ hwc->hwcInBookmark= 0;	}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref--;
	    if  ( fki->fkiSingleParagraph )
		{ docHtmlFinishFieldText( hwc );	}
	    break;

	default:
	    if  ( fki->fkiSingleParagraph )
		{ docHtmlFinishFieldText( hwc );	}
	    break;
	}
    
    return 0;
    }
