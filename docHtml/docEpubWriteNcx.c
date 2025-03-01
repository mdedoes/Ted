/************************************************************************/
/*									*/
/*  The NCX file inside an EPUB document.				*/
/*									*/
/*  See:								*/
/*  -  https://www.opticalauthoring.com/inside-the-epub-format-the-still-useful-ncx-file/ */
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<docCalculateToc.h>
#   include	<docEvalField.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docEpubWrite.h"

#   include	<appDebugon.h>

/************************************************************************/

static void docEpubEmitNcxMetaElement(	XmlWriter *		xw,
					const char *		name,
					const char *		value )
    {
    sioOutPutString( "    <meta", xw->xwSos );

    xmlWriteStringAttribute( xw, "name", name );
    xmlWriteStringAttribute( xw, "content", value );

    sioOutPutString( "/>\n", xw->xwSos );
    }

static int docEpubEmitNcxHead(		XmlWriter *		xw,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					int			depth )
    {
    char		scratch[10];

    sprintf( scratch, "%d", depth );

    sioOutPutString( "    <head>\n", xw->xwSos );

    docEpubEmitNcxMetaElement( xw, "dtb:uid",
				    utilMemoryBufferGetString( identifier ) );
    docEpubEmitNcxMetaElement( xw, "dtb:depth", scratch );
    docEpubEmitNcxMetaElement( xw, "dtb:totalPageCount", "0" );
    docEpubEmitNcxMetaElement( xw, "dtb:maxPageNumber", "0" );

    sioOutPutString( "    </head>\n", xw->xwSos );

    sioOutPutString( "    <docTitle><text>", xw->xwSos );
    xmlEscapeCharacters( xw, (const char *)title->mbBytes, title->mbSize );
    sioOutPutString( "</text></docTitle>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubCloseNavPoint(	XmlWriter *		xw,
					int			level )
    {
    sioOutPrintf( xw->xwSos, "%*s</navPoint>\n", 4+ 4* level, "" );
    return 0;
    }

static int docEpubStartNcxNavPoint(	XmlWriter *		xw,
					int			level,
					const char *		id,
					int			playOrder,
					const MemoryBuffer *	label,
					const char *		src )
    {
    sioOutPrintf( xw->xwSos, "%*s<navPoint", 4+ 4* level, "" );
    xmlWriteStringAttribute( xw, "id", id );
    xmlWriteIntAttribute( xw, "playOrder", playOrder );
    sioOutPutString( ">\n", xw->xwSos );

    sioOutPrintf( xw->xwSos, "%*s<navLabel><text>", 8+ 4* level, "" );
    xmlEscapeCharacters( xw, (const char *)label->mbBytes, label->mbSize );
    sioOutPutString( "</text></navLabel>\n", xw->xwSos );

    sioOutPrintf( xw->xwSos, "%*s<content", 8+ 4* level, "" );
    xmlWriteStringAttribute( xw, "src", src );
    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static const char *	DocEpubNcxHead[]=
{
  XML_DECLARATION,
  /*
  "<!DOCTYPE ncx PUBLIC \"-//NISO//DTD ncx 2005-1//EN\"\n",
  "  \"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">\n",
  */
  "<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">\n",
};
static const char	DocEpubNcxTail[]= "</ncx>\n";

static int docEpubStartNcx(	XmlWriter *		xw,
				HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier,
				int			depth )
    {
    EpubWriter *		ew= (EpubWriter *)hwcEpub->hwcPrivate;

    const char * const		nameMember= DocEpubNameNcx;
    const int			compressed= 1;

    int				l;

    xw->xwSos= sioOutZipOpen( &(ew->ewZipOutput), nameMember, compressed );
    if  ( ! xw->xwSos )
	{ SXDEB(nameMember,xw->xwSos); return -1;	}

    for ( l= 0; l < sizeof(DocEpubNcxHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubNcxHead[l], xw->xwSos ); }

    if  ( docEpubEmitNcxHead( xw, title, identifier, depth ) )
	{ SDEB(nameMember); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit a Contents NCX that only holds the main document.		*/
/*									*/
/************************************************************************/

int docEpubEmitSimpleNcx(	HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    const int			depth= 1;
    int				playOrder= 1;

    XmlWriter			xw;

    xmlInitXmlWriter( &xw );

    if  ( docEpubStartNcx( &xw, hwcEpub, title, identifier, depth ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( "    <navMap>\n", xw.xwSos );

    docEpubStartNcxNavPoint( &xw, 0, DocEpubIdDoc, playOrder++,
						    title, DocEpubNameDocAbs );
    docEpubCloseNavPoint( &xw, 0 );

    sioOutPutString( "    </navMap>\n", xw.xwSos );

    sioOutPutString( DocEpubNcxTail, xw.xwSos );

  ready:

    if  ( xw.xwSos && sioOutClose( xw.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a Contents NCX that follows the structure of a table of	*/
/*  contents.								*/
/*									*/
/*  Note the toilsome mapping between the level numbers in the TOC and	*/
/*  those in the NCX. This is to cope with situations where the TOC	*/
/*  skips levels.							*/
/*									*/
/************************************************************************/

int docEpubEmitCompositeNcx(	HtmlWritingContext *	hwcEpub,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwcEpub->hwcSettings;
    EpubWriter *		ew= (EpubWriter *)hwcEpub->hwcPrivate;

    int				playOrder= 1;
    const TocEntry *		te;
    int				l;

    char			id[20+1];
    char			src[150+1];

    MemoryBuffer		mbLabel;
    XmlWriter			xw;

    int				ncxLevel;
    int				m[PPoutline_COUNT];

    utilInitMemoryBuffer( &mbLabel );
    xmlInitXmlWriter( &xw );

    if  ( docEpubStartNcx( &xw, hwcEpub,
			title, identifier, ew->ewCalculateToc.ctDepth ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( "    <navMap>\n", xw.xwSos );

    for ( ncxLevel= 0; ncxLevel < PPoutline_COUNT; ncxLevel++ )
	{ m[ncxLevel]= 0;	}
    ncxLevel= 0;

    te= ew->ewCalculateToc.ctEntries;
    for ( l= 0; l < ew->ewCalculateToc.ctEntryCount; te++, l++ )
	{
#	if 0
	sioOutPrintf( xw.xwSos, "%*s<--  m[%d]=%d teLevel= %d -->\n",
			    4+ 4* ncxLevel, "",
			    ncxLevel, m[ncxLevel], te->teLevel );
#	endif

	while( m[ncxLevel] > te->teLevel )
	    { docEpubCloseNavPoint( &xw, --ncxLevel );	}

	sprintf( src, "%s#%.*s", DocEpubNameDocAbs,
					    te->teMarkName->mbSize,
					    te->teMarkName->mbBytes );

	utilEmptyMemoryBuffer( &mbLabel );
	if  ( docCalculateRefFieldValue( &mbLabel, te->teMarkName,
							hws->hwsDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	sprintf( id, "np-%d", playOrder );
	docEpubStartNcxNavPoint( &xw, ncxLevel, id, playOrder++, &mbLabel, src );
	m[++ncxLevel]= te->teLevel+ 1;

#	if 0
	sioOutPrintf( xw.xwSos, "%*s<--  m[%d]:=%d teLevel= %d -->\n",
			    4+ 4* ncxLevel, "",
			    ncxLevel, m[ncxLevel], te->teLevel );
#	endif
	}

    while( ncxLevel > 0 )
	{ docEpubCloseNavPoint( &xw, --ncxLevel );	}

    sioOutPutString( "    </navMap>\n", xw.xwSos );

    sioOutPutString( DocEpubNcxTail, xw.xwSos );

  ready:

    utilCleanMemoryBuffer( &mbLabel );

    if  ( xw.xwSos && sioOutClose( xw.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

