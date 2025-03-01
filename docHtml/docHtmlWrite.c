/************************************************************************/
/*									*/
/*  Save a Document into an HTML file.					*/
/*									*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<textMsLocale.h>

#   include	<docBuf.h>
#   include	<psShading.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docItemShading.h>
#   include	<docDocumentProperties.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void docHtmlWritelnCloseElement(	const char *		element,
					HtmlWritingContext *	hwc )
    {
    docHtmlPutString( "</", hwc );
    docHtmlPutString( element, hwc );
    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );
    }

void docHtmlPutString(		const char *		s,
				HtmlWritingContext *	hwc )
    { xmlPutString( s, &(hwc->hwcXmlWriter) );	}

void docHtmlNewLine(	HtmlWritingContext *	hwc )
    {
    xmlNewLine(  &(hwc->hwcXmlWriter) );
    }

void docHtmlWriteStringAttribute(	HtmlWritingContext *	hwc,
					const char *		name,
					const char *		value )
    {
    xmlWriteStringAttribute( &(hwc->hwcXmlWriter), name, value );
    }

void docHtmlWriteIntAttribute(		HtmlWritingContext *	hwc,
					const char *		name,
					int			value )
    {
    xmlWriteIntAttribute( &(hwc->hwcXmlWriter), name, value );
    }

void docHtmlEscapeCharacters(	HtmlWritingContext *	hwc,
				const char *		ss,
				int			len )
    {
    xmlEscapeCharacters( &(hwc->hwcXmlWriter), ss, len );
    }

static void docHtmlEscapeString(	const char *		s,
					HtmlWritingContext *	hwc )
    {
    xmlEscapeCharacters( &(hwc->hwcXmlWriter), s, strlen( s ) );

    return;
    }

/************************************************************************/
/*									*/
/*  Change attributes.							*/
/*									*/
/************************************************************************/

void docHtmlChangeAttributes(	HtmlWritingContext *		hwc,
				int				taNr )
    {
    if  ( taNr == hwc->hwcCurrentAttributeNumber )
	{ return;	}

    if  ( hwc->hwcCurrentAttributeNumber >= 0 )
	{
	docHtmlPutString( "</span>", hwc );
	}

    if  ( taNr >= 0 )
	{
	char	scratch[20];

	sprintf( scratch, "t%d", taNr );

	docHtmlPutString( "<span", hwc );
	docHtmlWriteStringAttribute( hwc, "class", scratch );
	docHtmlPutString( ">", hwc );
	}

    hwc->hwcCurrentAttributeNumber= taNr;
    }

void docHtmlEmitBackgroundProperty(
				const ItemShading *		is,
				HtmlWritingContext *		hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    const struct BufferDocument * bd= hws->hwsDocument;

    int				isFilled= 0;
    RGB8Color			rgb8;

    char			scratch[39];

    if  ( is->isPattern == DOCspSOLID )
	{
	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, is ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    {
	    sprintf( scratch, "\"#%02x%02x%02x\"",
			    rgb8.rgb8Red, rgb8.rgb8Green, rgb8.rgb8Blue );

	    docHtmlWriteStringAttribute( hwc, "BGCOLOR", scratch );
	    }
	}

    return;
    }

/************************************************************************/

static void docHtmlSaveMetaElement(	HtmlWritingContext *	hwc,
					const char *		name,
					const MemoryBuffer *	content )
    {
    if  ( ! utilMemoryBufferIsEmpty( content ) )
	{
	docHtmlPutString( "<meta name=\"", hwc );
	docHtmlEscapeString( name, hwc );
	docHtmlPutString( "\" content=\"", hwc );
	docHtmlEscapeString( utilMemoryBufferGetString( content ), hwc );
	docHtmlPutString( "\"/>", hwc );
	docHtmlNewLine( hwc );
	}

    return;
    }

/************************************************************************/

int docHtmlEmitProlog(	HtmlWritingContext *	hwc,
			const char *		localeTag,
			const char *		extraNameSpaces )
    {
    docHtmlPutString( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n", hwc );

#   define USE_HTML5 1
#   if USE_HTML5
    docHtmlPutString( "<!DOCTYPE html", hwc );
#   else
    docHtmlPutString( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" ",
									hwc );
    docHtmlPutString( "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"",
									hwc );
#   endif

    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "<html", hwc );
    docHtmlWriteStringAttribute( hwc,
				"xmlns", "http://www.w3.org/1999/xhtml" );

    if  ( extraNameSpaces )
	{
	docHtmlPutString( " ", hwc );
	docHtmlPutString( extraNameSpaces, hwc );
	}

    if  ( localeTag )
	{
	docHtmlWriteStringAttribute( hwc, "lang", localeTag );
	docHtmlWriteStringAttribute( hwc, "xml:lang", localeTag );
	}
    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

int docHtmlWriteStyleLink(	HtmlWritingContext *	hwc,
				const MemoryBuffer *	mbStylesheet )
    {
    docHtmlPutString( "<link ", hwc );
    docHtmlWriteStringAttribute( hwc, "rel", "stylesheet" );
    docHtmlWriteStringAttribute( hwc, "type", "text/css; charset=UTF-8" );
    docHtmlWriteStringAttribute( hwc, "href",
				utilMemoryBufferGetString( mbStylesheet ) );
    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

int docHtmlWriteTitle(	HtmlWritingContext *	hwc,
			const MemoryBuffer *	mbTitle )
    {
    docHtmlPutString( "<title>", hwc );

    if  ( ! utilMemoryBufferIsEmpty( mbTitle ) )
	{ docHtmlEscapeString( utilMemoryBufferGetString( mbTitle ), hwc ); }
    else{ docHtmlEscapeString( ".", hwc );	}

    docHtmlPutString( "</title>", hwc );

    return 0;
    }

int docHtmlStartDocument(	HtmlWritingContext *	hwc )
    {
    int				rval= 0;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    const DocumentProperties *	dp= hws->hwsDocument->bdProperties;

    MemoryBuffer		stylesheet;
    const char *		localeTag;

    utilInitMemoryBuffer( &stylesheet );

    localeTag= textGetMsLocaleTagById( dp->dpDefaultLocaleId );

    docHtmlEmitProlog( hwc, localeTag, (const char *)0 );

    docHtmlPutString( "<head>", hwc );
    docHtmlNewLine( hwc );
    docHtmlPutString( "<meta ", hwc );
    docHtmlWriteStringAttribute( hwc, "http-equiv", "content-type" );
    docHtmlWriteStringAttribute( hwc, "content", "text/html; charset=UTF-8" );
    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    docHtmlWriteTitle( hwc, &(dp->dpTitle) );

    if  ( ! hws->hwsInlineCss )
	{
	if  ( ! hws->hwsGetCssName )
	    { XDEB(hws->hwsInlineCss); rval= -1; goto ready;	}

	if  ( (*hws->hwsGetCssName)( &stylesheet, hwc ) < 0 )
	    { LDEB(1); return -1;	}

	docHtmlWriteStyleLink( hwc, &stylesheet );
	}

    docHtmlSaveMetaElement( hwc, "description", &(dp->dpSubject) );
    docHtmlSaveMetaElement( hwc, "keywords", &(dp->dpKeywords) );
    docHtmlSaveMetaElement( hwc, "comment", &(dp->dpDoccomm) );
    docHtmlSaveMetaElement( hwc, "author", &(dp->dpAuthor) );

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpHlinkbase) ) )
	{
	docHtmlPutString( "<base href=\"", hwc );
	docHtmlEscapeString( utilMemoryBufferGetString( &(dp->dpHlinkbase) ), hwc );
	docHtmlPutString( "\"/>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( hws->hwsInlineCss )
	{
	docHtmlPutString( "<style type=\"text/css\">", hwc );
	docHtmlNewLine( hwc );
	docHtmlNewLine( hwc );

	if  ( docHtmlSaveDocumentStyles( hwc, hwc->hwcXmlWriter.xwSos ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docHtmlPutString( "</style>", hwc );
	docHtmlNewLine( hwc );
	}

    docHtmlPutString( "</head>", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "<body", hwc );
    docHtmlWriteStringAttribute( hwc, "class", "ted" );
    /*
    if  ( lang )
	{ docHtmlWriteStringAttribute( hwc, "lang", lang );	}
    */
    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

  ready:

    utilCleanMemoryBuffer( &stylesheet );

    return rval;
    }

int docHtmlFinishDocumentBody(	HtmlWritingContext *	hwc )
    {
    docHtmlPutString( "</body></html>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

