/************************************************************************/
/*									*/
/*  Write the OPF package metadata file.				*/
/*									*/
/*  See									*/
/*  -  https://www.opticalauthoring.com/inside-the-epub-format-the-opf-file/ */
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<sioZip.h>
#   include	<textMsLocale.h>

#   include	<docBuf.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docObject.h>
#   include	<docDocumentProperties.h>
#   include	"docEpubWrite.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Constant content of the OPF Package Metadata Document.		*/
/*									*/
/************************************************************************/

static const char *	DocEpubOpfMetaHead[]=
{
  "  <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\r\n",
  "    xmlns:dcterms=\"http://purl.org/dc/terms/\"\r\n",
  "    xmlns:opf=\"http://www.idpf.org/2007/opf\">\r\n",
};

static const char	DocEpubOpfMetaTail[]= "  </metadata>\n";

/************************************************************************/

static const char	DocEpubIdentifierName[]= "dcidentifier";

/************************************************************************/
/*									*/
/*  Emit a meta information element for the opf package file.		*/
/*									*/
/************************************************************************/

static void docEpubEmitOpfMetaElement(	XmlWriter *		xw,
					const char *		id,
					const char *		name,
					const MemoryBuffer *	value )
    {
    sioOutPutString( "    <", xw->xwSos );
    sioOutPutString( name, xw->xwSos );
    if  ( id )
	{ xmlWriteStringAttribute( xw, "id", id );	}
    sioOutPutString( ">", xw->xwSos );

    xmlEscapeCharacters( xw, (const char *)value->mbBytes, value->mbSize );

    sioOutPutString( "</", xw->xwSos );
    sioOutPutString( name, xw->xwSos );
    sioOutPutString( ">", xw->xwSos );
    xmlNewLine( xw );
    }

static void docEpubEmitOpfMetaElementIfSet(
					XmlWriter *		xw,
					const char *		id,
					const char *		name,
					const MemoryBuffer *	value )
    {
    if  ( ! utilMemoryBufferIsEmpty( value ) )
	{ docEpubEmitOpfMetaElement( xw, id, name, value );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit the matadata in an OPF package file. I.E:			*/
/*									*/
/*  Translate document information to Dublin Core Metadata.		*/
/*  See http://dublincore.org/documents/usageguide/elements.shtml	*/
/*									*/
/*  Dublin Core		RTF						*/
/*  ------ ----		---						*/
/*  Title		title						*/
/*  Creator		author						*/
/*  Subject		subject+ keywords				*/
/*  Description		doccom						*/
/*  Publisher		company (Do not repeat the autor value)		*/
/*  Contributor		-- (Do not repeat the autor value)		*/
/*  Date		revtim or creatim				*/
/*  Type		-- Not available				*/
/*  Format		-- Irrelevant					*/
/*  Identifier		-- Must be made up.				*/
/*  Source		-- Not available				*/
/*  Language		deflang						*/
/*  Relation		-- Not available				*/
/*  Coverage		-- Not available				*/
/*  Rights		-- Not available				*/
/*									*/
/************************************************************************/

static int docEpubEmitMetaData(	XmlWriter *			xw,
				const MemoryBuffer *		title,
				const MemoryBuffer *		identifier,
				const struct BufferDocument *	bd )
    {
    int				rval= 0;
    const DocumentProperties *	dp= bd->bdProperties;

    MemoryBuffer		mbLang;
    const char *		localeTag;

    int				l;

    utilInitMemoryBuffer( &mbLang );
    if  ( utilMemoryBufferSetString( &mbLang, "en-US" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( l= 0; l < sizeof(DocEpubOpfMetaHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubOpfMetaHead[l], xw->xwSos ); }

    docEpubEmitOpfMetaElement( xw, (char *)0, "dc:title", title );

    localeTag= textGetMsLocaleTagById( dp->dpDefaultLocaleId );
    if  ( localeTag )
	{
	if  ( utilMemoryBufferSetString( &mbLang, localeTag ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    docEpubEmitOpfMetaElement( xw, (char *)0, "dc:language", &mbLang );
    docEpubEmitOpfMetaElement( xw, DocEpubIdentifierName,
					"dc:identifier", identifier );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:creator", &(dp->dpAuthor) );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:subject", &(dp->dpSubject) );
    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:subject", &(dp->dpKeywords) );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:description", &(dp->dpDoccomm) );
    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:publisher", &(dp->dpCompany) );

    {
    char		scratch[100];
    const char		format[]= "%FT%XZ";

    strftime( scratch, sizeof(scratch), format, &(dp->dpRevtim) );

    sioOutPutString( "    <meta ", xw->xwSos );
    xmlWriteStringAttribute( xw, "property", "dcterms:modified" );
    sioOutPutString( ">", xw->xwSos );
    sioOutPutString( scratch, xw->xwSos );
    sioOutPutString( "</meta>\r\n", xw->xwSos );
    /* <meta property="dcterms:modified">2015-12-15T09:18:00Z</meta> */
    }

    sioOutPutString( DocEpubOpfMetaTail, xw->xwSos );

  ready:

    utilCleanMemoryBuffer( &mbLang );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Constant content.							*/
/*									*/
/************************************************************************/

static const char *	DocEpubOpfHead[]=
{
  XML_DECLARATION,
  "<package xmlns=\"http://www.idpf.org/2007/opf\" version=\"3.0\" unique-identifier=\"", DocEpubIdentifierName, "\">\n",
};
static const char	DocEpubOpfTail[]= "</package>\n";

static int docEpubStartOpf(	ZipOutput *			zo,
				XmlWriter *			xw,
				const MemoryBuffer *		title,
				const MemoryBuffer *		identifier,
				const struct BufferDocument *	bd )
    {
    const char * const		nameMember= DocEpubNameOpf;
    const int			compressed= 1;

    int				l;

    xw->xwSos= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! xw->xwSos )
	{ SXDEB(nameMember,xw->xwSos); return -1;	}

    for ( l= 0; l < sizeof(DocEpubOpfHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubOpfHead[l], xw->xwSos ); }

    if  ( docEpubEmitMetaData( xw, title, identifier, bd ) )
	{ SDEB(nameMember); return -1;	}

    return 0;
    }

/************************************************************************/

static int docEpubEmitManifestItem(	XmlWriter *		xw,
					const char *		id,
					const char *		href,
					const char *		mediaType,
					const char *		properties )
    {
    sioOutPutString( "    <item", xw->xwSos );

    xmlWriteStringAttribute( xw, "id", id );
    xmlWriteStringAttribute( xw, "href", href );
    xmlWriteStringAttribute( xw, "media-type", mediaType );

    if  ( properties )
	{ xmlWriteStringAttribute( xw, "properties", properties );	}

    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubEmitSpineItem(	XmlWriter *		xw,
					const char *		id )
    {
    sioOutPutString( "    <itemref", xw->xwSos );

    xmlWriteStringAttribute( xw, "idref", id );

    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubAddImageToOpf(	int			n,
					void *			vio,
					void *			vexw )
    {
    int				rval= 0;

    const InsertedObject *	io= (InsertedObject *)vio;
    const PictureProperties *	pip= &(io->ioPictureProperties);
    EpubXmlWriter *		exw= (EpubXmlWriter *)vexw;
    XmlWriter *			xw= &(exw->exwXmlWriter);

    const char *		mimeType;
    const char *		ext;
    int				type;
    int				useDataUrl;

    const MemoryBuffer *	mb= (const MemoryBuffer *)0;
    bmWriteBitmap		writeBitmap= (bmWriteBitmap)0;
    MetafileWriteSvg		writeSvg= (MetafileWriteSvg)0;

    MemoryBuffer		href;
    char			id[20+1];
    const int			relative= 0;

    utilInitMemoryBuffer( &href );

    if  ( docHtmlObjectSaveHow( exw->exwHtmlWriter, &type, &useDataUrl,
				&writeBitmap, &writeSvg,
				&mimeType, &ext, &mb, io ) )
	{ goto ready;	}

    if  ( ! useDataUrl )
	{
	if  ( docEpubGetImageSrcX( &href, relative, n, io, ext ) < 0 )
	    { LDEB(n); rval= -1; goto ready;	}

	if  ( 1 || pip->pipBliptag == 0 )
	    { sprintf( id, "i%d", n );			}
	else{ sprintf( id, "b%08lx", pip->pipBliptag );	}

	if  ( docEpubEmitManifestItem( xw, id,
		utilMemoryBufferGetString( &href ), mimeType, (char *)0 ) )
	    { SDEB(id); rval= -1; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &href );

    return rval;
    }

static int docEpubAddImagesToOpf(	EpubXmlWriter *			exw )
    {
    HtmlWritingContext *	hwc= exw->exwHtmlWriter;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    const BufferDocument *	bd= hws->hwsDocument;

    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					docEpubAddImageToOpf, (void *)exw );

    return 0;
    }

/************************************************************************/

static const char	DocEpubOpfManifestHead[]= "<manifest>\n";
static const char	DocEpubOpfManifestTail[]= "</manifest>\n";

static const char *	DocEpubSpineHead[]=
{
  "<spine toc=\"", DocEpubIdNcx, "\">\n",
};
static const char	DocEpubSpineTail[]= "</spine>\n";

/************************************************************************/

int docEpubEmitSimpleOpf(	HtmlWritingContext *	hwc,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier )
    {
    int				rval= 0;

    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    int				l;

    EpubXmlWriter		exw;

    xmlInitXmlWriter( &(exw.exwXmlWriter) );
    exw.exwHtmlWriter= hwc;

    if  ( docEpubStartOpf( &(ew->ewZipOutput), &(exw.exwXmlWriter),
				    title, identifier, hws->hwsDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( DocEpubOpfManifestHead, exw.exwXmlWriter.xwSos );

    docEpubEmitManifestItem( &(exw.exwXmlWriter),
			DocEpubIdNcx, DocEpubNameNcx,
			DocEpubMediaNcx, (char *)0 );
    docEpubEmitManifestItem( &(exw.exwXmlWriter),
			DocEpubIdNavig, DocEpubNameNavig,
			DocEpubMediaNavig, "nav" );
    docEpubEmitManifestItem( &(exw.exwXmlWriter),
			DocEpubIdCss, DocEpubNameCssAbs,
			DocEpubMediaCss, (char *)0 );
    docEpubEmitManifestItem( &(exw.exwXmlWriter),
			DocEpubIdDoc, DocEpubNameDocAbs,
			DocEpubMediaDoc, (char *)0 );

    docEpubAddImagesToOpf( &exw );

    sioOutPutString( DocEpubOpfManifestTail, exw.exwXmlWriter.xwSos );

    for ( l= 0; l < sizeof(DocEpubSpineHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubSpineHead[l], exw.exwXmlWriter.xwSos ); }

    docEpubEmitSpineItem( &(exw.exwXmlWriter), DocEpubIdNavig );
    docEpubEmitSpineItem( &(exw.exwXmlWriter), DocEpubIdDoc );

    sioOutPutString( DocEpubSpineTail, exw.exwXmlWriter.xwSos );

    sioOutPutString( DocEpubOpfTail, exw.exwXmlWriter.xwSos );

  ready:

    if  ( exw.exwXmlWriter.xwSos && sioOutClose( exw.exwXmlWriter.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

