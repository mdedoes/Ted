/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an EPUB file.				*/
/*									*/
/*  Documents can be validated against:					*/
/*  -  http://threepress.org/document/epub-validate.			*/
/*									*/
/*  See:								*/
/*  -  https://www.safaribooksonline.com/library/view/epub-3-best/9781449329129/ch01.html */
/*  -  https://www.opticalauthoring.com/inside-the-epub-format-the-basics/ */
/*  -  https://github.com/jstallent/BookOfTexas				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<docBuf.h>
#   include	<docCalculateToc.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docHtmlWrite.h"
#   include	<docTreeType.h>
#   include	<docObject.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docDocumentProperties.h>
#   include	<docFields.h>
#   include	"docEpubWrite.h"

#   include	<appDebugon.h>

/************************************************************************/

static SimpleOutputStream * docEpubOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext );

static int docEpubGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static int docEpubGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc );

static void docInitEpubWriter(	EpubWriter *	ew )
    {
    docInitCalculateToc( &(ew->ewCalculateToc) );
    sioZipInitOutput( &(ew->ewZipOutput) );
    }

static void docCleanEpubWriter(	EpubWriter *	ew )
    {
    sioZipCleanOutput( &(ew->ewZipOutput) );
    docCleanCalculateToc( &(ew->ewCalculateToc) );
    }

/************************************************************************/

# define DIR_CONTENT	"document"
# define DIR_MEDIA	"media"
# define NAME_CSS	"document.css"
# define NAME_DOC	"document.xhtml"

const char	DocEpubNameOpf[]= "content.opf";

const char	DocEpubIdNcx[]= "ncx";
const char	DocEpubIdNavig[]= "tocnav";
const char	DocEpubIdCss[]= "css";
const char	DocEpubIdDoc[]= "doc";

const char	DocEpubNameMimetype[]= "mimetype";
const char	DocEpubNameContainer[]= "META-INF/container.xml";
const char	DocEpubNameNcx[]= "toc.ncx";
const char	DocEpubNameNavig[]= "toc.xhtml";
const char	DocEpubNameContent[]= DIR_CONTENT;
const char	DocEpubNameDocAbs[]= DIR_CONTENT "/" NAME_DOC;
const char	DocEpubNameDocRel[]= NAME_DOC;
const char	DocEpubNameMediaAbs[]= DIR_CONTENT "/" DIR_MEDIA;
const char	DocEpubNameMediaRel[]= DIR_MEDIA;

const char	DocEpubNameCssAbs[]= DIR_CONTENT "/" 
					     DIR_MEDIA "/" NAME_CSS;
const char	DocEpubNameCssRel[]= DIR_MEDIA "/" NAME_CSS;

const char	DocEpubMediaNcx[]= "application/x-dtbncx+xml";
const char	DocEpubMediaNavig[]= "application/xhtml+xml";
const char	DocEpubMediaCss[]= "text/css";
const char	DocEpubMediaDoc[]= "application/xhtml+xml";
const char	DocEpubMediaEpub[]= "application/epub+zip";

const char	DocEpubNameContentLen= sizeof(DocEpubNameContent)- 1;
/*
const int	DocEpubNameMediaLenAbs= sizeof(DocEpubNameMediaAbs)- 1;
const int	DocEpubNameMediaLenRel= sizeof(DocEpubNameMediaRel) -1;
const int	DocEpubNameCssLenAbs= sizeof(DocEpubNameCssAbs) -1;
const int	DocEpubNameCssLenRel= sizeof(DocEpubNameCssRel) -1;
*/

/************************************************************************/
/*									*/
/*  Insert a boilerplate container.xml.					*/
/*									*/
/************************************************************************/

static const char *	DocEpubContainerXml[]=
{
  XML_DECLARATION,
  "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">\n",
  "  <rootfiles>\n",
  "    <rootfile full-path=\"", DocEpubNameOpf, "\" media-type=\"application/oebps-package+xml\"/>\n",
  "  </rootfiles>\n",
  "</container>\n",
};

static int docEpubEmitContainerXml(		ZipOutput *	zo )
    {
    int				rval= 0;

    const char * const		nameMember= DocEpubNameContainer;
    SimpleOutputStream *	sosMember= (SimpleOutputStream *)0;
    const int			compressed= 1;

    int				line;

    sosMember= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! sosMember )
	{ SXDEB(nameMember,sosMember); rval= -1; goto ready;	}

    for ( line= 0; line < sizeof(DocEpubContainerXml)/sizeof(char *); line++ )
	{ sioOutPutString( DocEpubContainerXml[line], sosMember ); }

  ready:

    if  ( sosMember && sioOutClose( sosMember ) )
	{ SDEB(nameMember); rval= -1;	}

    return rval;
    }

/************************************************************************/

static int docEpubEmitMimeType(		ZipOutput *	zo )
    {
    int				rval= 0;

    SimpleOutputStream *	sosMember= (SimpleOutputStream *)0;
    const char *		nameMember= DocEpubNameMimetype;
    const int			compressed= 0;

    sosMember= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! sosMember )
	{ SXDEB(nameMember,sosMember); rval= -1; goto ready;	}

    sioOutPutString( DocEpubMediaEpub, sosMember );
    /* Not expected this way
    sioOutPutString( "\n", sosMember );
    */

  ready:

    if  ( sosMember && sioOutClose( sosMember ) )
	{ SDEB(nameMember); rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return the name of the CSS file.					*/
/*									*/
/************************************************************************/

static int docEpubGetCssNameX(		MemoryBuffer *		target,
					int			relative )
    {
    if  ( relative )
	{
	if  ( utilMemoryBufferSetString( target, DocEpubNameCssRel ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    else{
	if  ( utilMemoryBufferSetString( target, DocEpubNameCssAbs ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    }

static int docEpubGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc )
    {
    const int		relative= 1;

    return docEpubGetCssNameX( target, relative );
    }

/************************************************************************/
/*									*/
/*  Emit the stylesheet							*/
/*									*/
/************************************************************************/

static int docEpubWriteCss(		HtmlWritingContext *	hwc )
    {
    int				rval= 0;

    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;
    const int			relative= 0;

    MemoryBuffer		nameMember;
    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;
    const int			compressed= 1;

    utilInitMemoryBuffer( &nameMember );

    if  ( docEpubGetCssNameX( &nameMember, relative ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sosCss= sioOutZipOpen( &(ew->ewZipOutput),
			utilMemoryBufferGetString( &nameMember ), compressed );
    if  ( ! sosCss )
	{ XDEB(sosCss); rval= -1; goto ready;	}

    if  ( docHtmlSaveDocumentStyles( hwc, sosCss ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &nameMember );

    if  ( sosCss )
	{ sioOutClose( sosCss );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit an actual document.						*/
/*									*/
/************************************************************************/

static int docEpubEmitDocument(	const HtmlWritingContext *	hwcEpub,
				struct BufferDocument *		bd,
				const char *			relativeName,
				const DocumentSelection *	ds )
    {
    int				rval= 0;
    EpubWriter *		ew= (EpubWriter *)hwcEpub->hwcPrivate;

    char			absoluteName[200+1];
    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    const int			compressed= 1;

    HtmlWritingContext		hwcDoc;

    docInitHtmlWritingContext( &hwcDoc );

    if  ( DocEpubNameContentLen+ 1+ strlen(relativeName)+ 1 >=
							sizeof(absoluteName) )
	{ SDEB(relativeName); rval= -1; goto ready;	}

    sprintf( absoluteName, "%s/%s", DocEpubNameContent, relativeName );

    sosDoc= sioOutZipOpen( &(ew->ewZipOutput), absoluteName, compressed );
    if  ( ! sosDoc )
	{ SXDEB(absoluteName,sosDoc); rval= -1; goto ready;	}

    docStartHtmlWritingContext( &hwcDoc, hwcEpub->hwcSettings, sosDoc );

    if  ( docHtmlStartDocument( &hwcDoc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docHtmlSaveSelection( &hwcDoc, &(bd->bdBody), ds ) )
	{ XDEB(ds); rval= -1; goto ready;	}

    if  ( docHtmlFinishDocumentBody( &hwcDoc ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanHtmlWritingContext( &hwcDoc );

    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the table of contents of the document.				*/
/*									*/
/************************************************************************/

static DocumentField * docHtmlGetTocForEpub(	TocField *		tf,
						struct BufferDocument *	bd )
    {
    const DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;
    int				fieldNr;

    DocumentField *		rval= (DocumentField *)0;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( bd, fieldNr );

	if  ( ! df						||
	      df->dfKind != DOCfkTOC				||
	      df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	docFieldGetToc( tf, df );
	if  ( tf->tfType == TOCtypeTOC )
	    { rval= df; break;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save a document to EPUB.						*/
/*									*/
/************************************************************************/

int docEpubSaveDocument(	SimpleOutputStream *		sos,
				struct BufferDocument *		bd,
				const struct LayoutContext *	lc )
    {
    HtmlWritingSettings		hws;
    HtmlWritingContext		hwc;
    int				rval= 0;
    EpubWriter			ew;

    const DocumentProperties *	dp= bd->bdProperties;
    DocumentField *		dfToc;

    MemoryBuffer		identifier;
    MemoryBuffer		title;

    const char * const		identifier_X= "Identifier";
    const char * 		title_X= "UnknownTitle";

    utilInitMemoryBuffer( &identifier );
    utilInitMemoryBuffer( &title );

    docInitHtmlWritingContext( &hwc );
    docInitHtmlWritingSettings( &hws );
    docInitEpubWriter( &ew );

    if  ( utilMemoryBufferSetString( &identifier, identifier_X ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &identifier, title_X ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ew.ewZipOutput.zoSosZip= sos;

    ew.ewCalculateToc.ctBdDoc= bd;

    hws.hwsInlineCss= 0;
    hws.hwsGetCssName= docEpubGetCssName;
    hws.hwsInlineImages= 0;
    hws.hwsOpenImageStream= docEpubOpenImageStream;
    hws.hwsGetImageSrc= docEpubGetImageSrc;
    hws.hwsInlineNotes= 1;

    hws.hwsEmitBackground= 0;

    hws.hwsLayoutContext= lc;
    hws.hwsDocument= bd;

    hwc.hwcXmlWriter.xwCrlf= 0;

    hwc.hwcPrivate= (void *)&ew;
						    /* sos is not for HTML */
    docStartHtmlWritingContext( &hwc, &hws, (SimpleOutputStream *)0 );

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{
	if  ( utilCopyMemoryBuffer( &title, &(dp->dpTitle) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEpubEmitMimeType( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docEpubEmitContainerXml( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dfToc= docHtmlGetTocForEpub( &(ew.ewCalculateToc.ctTocField), bd );
    if  ( dfToc )
	{
	if  ( docCollectTocInput( &(ew.ewCalculateToc) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docEpubEmitSimpleOpf( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitCompositeNcx( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitCompositeNavig( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitDocument( &hwc, bd, DocEpubNameDocRel,
					    (const DocumentSelection *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( docEpubEmitSimpleOpf( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitSimpleNcx( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitSimpleNavig( &hwc, &title, &identifier ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitDocument( &hwc, bd, DocEpubNameDocRel,
					    (const DocumentSelection *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( ! hws.hwsInlineCss && docEpubWriteCss( &hwc ) )
	{ LDEB(1); rval= -1; return -1;	}

    if  ( docHtmlSaveImageFiles( &hwc ) )
	{ LDEB(hwc.hwcImageCount); rval= -1; goto ready;	}

    if  ( sioZipFlushOutput( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &identifier );
    utilCleanMemoryBuffer( &title );

    docCleanEpubWriter( &ew );
    docCleanHtmlWritingContext( &hwc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the 'src' value for an embedded image.				*/
/*									*/
/************************************************************************/

int docEpubGetImageSrcX(	MemoryBuffer *		target,
				int			relative,
				int			n,
				const InsertedObject *	io,
				const char *		ext )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    const char *		dir;

    if  ( relative )
	{
	dir= DocEpubNameMediaRel;
	/*dirlen= DocEpubNameMediaLenRel;*/
	}
    else{
	dir= DocEpubNameMediaAbs;
	/*dirlen= DocEpubNameMediaLenAbs;*/
	}

    if  ( 1 || pip->pipBliptag == 0 )
	{ utilMemoryBufferPrintf( target, "%s/i%d.%s", dir, n, ext );			}
    else{ utilMemoryBufferPrintf( target, "%s/b%08lx.%s", dir, pip->pipBliptag, ext );	}

    return 0;
    }

static int docEpubGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const int		relative= 1;

    return docEpubGetImageSrcX( target, relative, n, io, ext );
    }

/************************************************************************/
/*									*/
/*  Open the file to write an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docEpubOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			relative= 0;
    const int			compressed= 1;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( docEpubGetImageSrcX( &src, relative, n, io, ext ) < 0 )
	{ LDEB(1); goto ready;	}

    sosImage= sioOutZipOpen( &(ew->ewZipOutput),
				utilMemoryBufferGetString( &src ), compressed );
    if  ( ! sosImage )
	{ XDEB(sosImage); goto ready; }

  ready:

    utilCleanMemoryBuffer( &src );

    return sosImage;
    }
