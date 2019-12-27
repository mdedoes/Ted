/************************************************************************/
/*									*/
/*  The Navication file inside an EPUB document.			*/
/*									*/
/*  Documents can be validated against:					*/
/*  -  http://threepress.org/document/epub-validate.			*/
/*									*/
/*  See:								*/
/*  -  https://www.opticalauthoring.com/inside-the-epub-format-the-navigation-file/ */
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<docCalculateToc.h>
#   include	<docEvalField.h>
#   include	<sioGeneral.h>
#   include	"docEpubWrite.h"
#   include	"docHtmlWriteImpl.h"
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>
#   include	<textMsLocale.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docEpubEmitNavigHead(	HtmlWritingContext *	hwc,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    BufferDocument *		bd= hws->hwsDocument;
    const DocumentProperties *	dp= (bd->bdProperties);
    const char *		localeTag;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);

    MemoryBuffer		mbStylesheet;

    utilInitMemoryBuffer( &mbStylesheet );

    if  ( utilMemoryBufferSetString( &mbStylesheet, DocEpubNameCssAbs ) )
	{ LDEB(1); rval= -1; goto ready;	}

    localeTag= textGetMsLocaleTagById( dp->dpDefaultLocaleId );

    docHtmlEmitProlog( hwc, localeTag,
			    "xmlns:epub=\"http://www.idpf.org/2007/ops\"");

    docHtmlPutString( "<head>\r\n", hwc );

    docHtmlWriteStyleLink( hwc, &mbStylesheet );
    docHtmlWriteTitle( hwc, title );

    docHtmlPutString( "</head>\r\n", hwc );
    docHtmlPutString( "<body>\r\n", hwc );

    docHtmlPutString( "<nav id=\"toc\" epub:type=\"toc\">\r\n", hwc );

    docHtmlPutString( "<h1>", hwc );
    xmlEscapeBuffer( xw, title );
    docHtmlPutString( "</h1>\r\n", hwc );

  ready:

    utilCleanMemoryBuffer( &mbStylesheet );

    return rval;
    }

/************************************************************************/

static int docEpubCloseNavPoint(	HtmlWritingContext *	hwc,
					int			level )
    {
    XmlWriter *		xw= &(hwc->hwcXmlWriter);

    sioOutPrintf( xw->xwSos, "%*s</li>\n", 4+ 4* level, "" );
    return 0;
    }

static int docEpubStartNavPoint(	HtmlWritingContext *	hwc,
					int			indent,
					const MemoryBuffer *	label,
					const char *		href )
    {
    XmlWriter *		xw= &(hwc->hwcXmlWriter);

    sioOutPrintf( xw->xwSos, "%*s<li><a", 4+ 4* indent, "" );
    xmlWriteStringAttribute( xw, "href", href );
    sioOutPutString( ">", xw->xwSos );

    xmlEscapeCharacters( xw, (const char *)label->mbBytes, label->mbSize );
    sioOutPutString( "</a>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubStartNavig(	HtmlWritingContext *		hwcNavig,
				const HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *		title,
				const MemoryBuffer *		identifier )
    {
    EpubWriter *		ew= (EpubWriter *)hwcEpub->hwcPrivate;

    const char * const		nameMember= DocEpubNameNavig;
    const int			compressed= 1;

    hwcNavig->hwcXmlWriter.xwSos=
		sioOutZipOpen( &(ew->ewZipOutput), nameMember, compressed );
    if  ( ! hwcNavig->hwcXmlWriter.xwSos )
	{ SXDEB(nameMember,hwcNavig->hwcXmlWriter.xwSos); return -1;	}

    if  ( docEpubEmitNavigHead( hwcNavig, title, identifier ) )
	{ SDEB(nameMember); return -1;	}

    docHtmlPutString( "<ol>\r\n", hwcNavig );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit a Contents NCX that only holds the main document.		*/
/*									*/
/************************************************************************/

int docEpubEmitSimpleNavig(	HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    HtmlWritingContext		hwcNavig;

    docInitHtmlWritingContext( &hwcNavig );

    hwcNavig.hwcSettings= hwcEpub->hwcSettings;
    hwcNavig.hwcPrivate= hwcEpub->hwcPrivate;

    if  ( docEpubStartNavig( &hwcNavig, hwcEpub, title, identifier ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docEpubStartNavPoint( &hwcNavig, 0, title, DocEpubNameDocAbs );
    docEpubCloseNavPoint( &hwcNavig, 0 );

    docHtmlPutString( "</ol>\r\n", &hwcNavig );
    docHtmlPutString( "</nav>\r\n", &hwcNavig );
    docHtmlPutString( "</body>\r\n", &hwcNavig );
    docHtmlPutString( "</html>\r\n", &hwcNavig );

  ready:

    if  ( hwcNavig.hwcXmlWriter.xwSos			&&
	  sioOutClose( hwcNavig.hwcXmlWriter.xwSos )	)
	{ LDEB(1); rval= -1;	}

    docCleanHtmlWritingContext( &hwcNavig );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a Navigation document that follows the structure of a table of	*/
/*  contents.								*/
/*									*/
/************************************************************************/

static int docEpubStartEntry(	HtmlWritingContext *	hwcNavig,
				const TocEntry *	te )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwcNavig->hwcSettings;

    char			src[250+1];
    MemoryBuffer		mbLabel;

    utilInitMemoryBuffer( &mbLabel );

    sprintf( src, "%s#%.*s", DocEpubNameDocAbs,
					    te->teMarkName->mbSize,
					    te->teMarkName->mbBytes );

    if  ( docCalculateRefFieldValue( &mbLabel, te->teMarkName,
						hws->hwsDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docEpubStartNavPoint( hwcNavig, te->teLevel, &mbLabel, src );

  ready:

    utilCleanMemoryBuffer( &mbLabel );

    return rval;
    }

static int docEpubEmitEntries(	HtmlWritingContext *	hwcNavig,
				int			level,
				const TocEntry *	te,
				int			count )
    {
    int		done= 0;

    if  ( te->teLevel < level || te->teLevel >= PPoutline_COUNT )
	{ LLDEB(te->teLevel,level); return -1;	}

    done= 0;
    while( done < count && te->teLevel >= level )
	{
	int	step;

	if  ( te->teLevel > level )
	    {
	    docHtmlPutString( "<li><span>..</span><ol>\r\n", hwcNavig );

	    step= docEpubEmitEntries( hwcNavig, level+ 1, te, count- done );
	    if  ( step < 1 )
		{ LLDEB(step,count); return -1;	}

	    docHtmlPutString( "</ol>\r\n", hwcNavig );

	    done += step; te += step;
	    }
	else{
	    if  ( docEpubStartEntry( hwcNavig, te ) )
		{ LDEB(done); return -1;	}

	    done++; te++;

	    if  ( done < count && te->teLevel > level )
		{
		docHtmlPutString( "<ol>\r\n", hwcNavig );

		step= docEpubEmitEntries( hwcNavig, level+ 1, te, count- done );
		if  ( step < 1 )
		    { LLDEB(step,count); return -1;	}

		docHtmlPutString( "</ol>\r\n", hwcNavig );

		done += step; te += step;
		}
	    }

	docHtmlPutString( "</li>\r\n", hwcNavig );
	}

    return done;
    }

int docEpubEmitCompositeNavig(	HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    EpubWriter *		ew= (EpubWriter *)hwcEpub->hwcPrivate;

    const int			level= 0;
    int				step;

    HtmlWritingContext		hwcNavig;

    docInitHtmlWritingContext( &hwcNavig );

    hwcNavig.hwcSettings= hwcEpub->hwcSettings;
    hwcNavig.hwcPrivate= hwcEpub->hwcPrivate;

    if  ( docEpubStartNavig( &hwcNavig, hwcEpub, title, identifier ) )
	{ LDEB(1); rval= -1; goto ready;	}

    step= docEpubEmitEntries( &hwcNavig, level,
				    ew->ewCalculateToc.ctEntries,
				    ew->ewCalculateToc.ctEntryCount );
    if  ( step < ew->ewCalculateToc.ctEntryCount )
	{
	LLDEB(step,ew->ewCalculateToc.ctEntryCount);
	rval= -1; goto ready;
	}

    docHtmlPutString( "</ol>", &hwcNavig );
    docHtmlPutString( "</nav>", &hwcNavig );
    docHtmlPutString( "</body>", &hwcNavig );
    docHtmlPutString( "</html>", &hwcNavig );

  ready:

    if  ( hwcNavig.hwcXmlWriter.xwSos			&&
	  sioOutClose( hwcNavig.hwcXmlWriter.xwSos )	)
	{ LDEB(1); rval= -1;	}

    docCleanHtmlWritingContext( &hwcNavig );

    return rval;
    }

