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

#   include	<appSystem.h>
#   include	<sioGeneral.h>
#   include	<sioFileio.h>

#   include	<docBuf.h>
#   include	<docObject.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docHtmlWrite.h"

#   include	<appDebugon.h>

typedef struct HtmlWriter
    {
			/**
			 * The file name if any. This is an absolute name.
			 */
    const MemoryBuffer *	hwFilename;
			/**
			 * The relative file name if any. It is only set if 
			 * hwFilename is set.
			 */
    MemoryBuffer		hwRelativeName;
    MemoryBuffer		hwFilesDirectoryAbs;
    MemoryBuffer		hwFilesDirectoryRel;
    } HtmlWriter;

static SimpleOutputStream * docHtmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext );

static int docHtmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static int docHtmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc );

static int docDocHtmlWriteCss(		HtmlWritingContext *	hwc );

static void docInitHtmlWriter(	HtmlWriter *	hw )
    {
    hw->hwFilename= (const MemoryBuffer *)0;
    utilInitMemoryBuffer( &(hw->hwRelativeName) );
    utilInitMemoryBuffer( &(hw->hwFilesDirectoryAbs) );
    utilInitMemoryBuffer( &(hw->hwFilesDirectoryRel) );
    }

static void docCleanHtmlWriter(	HtmlWriter *	hw )
    {
    utilCleanMemoryBuffer( &(hw->hwRelativeName) );
    utilCleanMemoryBuffer( &(hw->hwFilesDirectoryAbs) );
    utilCleanMemoryBuffer( &(hw->hwFilesDirectoryRel) );
    }

/**
 *  Added to the file name at hwBaseLength to make up the name
 *  of the media directory.
 */
const char DocHtmlDirectorySuffix[]= "_files";
const int  DocHtmlDirectorySuffixLength= sizeof(DocHtmlDirectorySuffix)- 1;

/************************************************************************/
/*									*/
/*  Save a document to HTML.						*/
/*									*/
/************************************************************************/

int docHtmlSaveDocument(	SimpleOutputStream *		sos,
				struct BufferDocument *		bd,
				const MemoryBuffer *		filename,
				const struct LayoutContext *	lc,
				int				inlineImages )
    {
    int				rval= 0;
    HtmlWritingSettings		hws;
    HtmlWritingContext		hwc;
    HtmlWriter			hw;

    if  ( filename && utilMemoryBufferIsEmpty( filename ) )
	{ filename= (const MemoryBuffer *)0;	}

    docInitHtmlWritingSettings( &hws );
    docInitHtmlWritingContext( &hwc );
    docInitHtmlWriter( &hw );

    hws.hwsInlineCss= ( bd->bdObjectList.iolPagedList.plItemCount == 0 || ! filename );
    hws.hwsGetCssName= docHtmlGetCssName;
    hws.hwsInlineImages= inlineImages || ! filename;
    hws.hwsOpenImageStream= docHtmlOpenImageStream;
    hws.hwsGetImageSrc= docHtmlGetImageSrc;
    hws.hwsInlineNotes= 0;

    hws.hwsEmitBackground= 1;

    hws.hwsLayoutContext= lc;
    hws.hwsDocument= bd;

    hw.hwFilename= filename;

    hwc.hwcXmlWriter.xwCrlf= 0;
    docStartHtmlWritingContext( &hwc, &hws, sos );

    hwc.hwcPrivate= (void *)&hw;

    if  ( hw.hwFilename )
	{
	if  ( fileGetRelativeName( &(hw.hwRelativeName), hw.hwFilename ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(hw.hwFilesDirectoryAbs),
						    hw.hwFilename ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(hw.hwFilesDirectoryRel),
						    &(hw.hwRelativeName) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( fileChangeExtension( &(hw.hwFilesDirectoryAbs),
						    (const char *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( fileChangeExtension( &(hw.hwFilesDirectoryRel),
						    (const char *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilMemoryBufferAppendString( &(hw.hwFilesDirectoryAbs),
						DocHtmlDirectorySuffix ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilMemoryBufferAppendString( &(hw.hwFilesDirectoryRel),
						DocHtmlDirectorySuffix ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docHtmlStartDocument( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docHtmlSaveSelection( &hwc, &(bd->bdBody),
					(const struct DocumentSelection *)0 ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( docHtmlFinishDocumentBody( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ( hwc.hwcImageCount > 0 && ! hws.hwsInlineImages )	||
	  ! hws.hwsInlineCss					)
	{
	if  ( fileTestDirectory( &(hw.hwFilesDirectoryAbs) )	&&
	      fileMakeDirectory( &(hw.hwFilesDirectoryAbs) )	)
	    {
	    LLLDEB(hwc.hwcImageCount,hws.hwsInlineCss,hws.hwsInlineImages);
	    rval= -1; goto ready;
	    }
	}

    if  ( ! hws.hwsInlineCss && docDocHtmlWriteCss( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ( hwc.hwcImageCount > 0 && ! hws.hwsInlineImages )	&&
            docHtmlSaveImageFiles( &hwc ) )
	{
	LLDEB(hwc.hwcImageCount,hws.hwsInlineImages);
	rval= -1; goto ready;
	}

  ready:

    docCleanHtmlWritingContext( &hwc );
    docCleanHtmlWriter( &hw );

    return rval;
    }

/************************************************************************/

static int docHtmlGetNameX(		MemoryBuffer *		target,
					const HtmlWriter *	hw,
					int			relative,
					const char *		slashName )
    {
    if  ( relative )
	{
	if  ( utilCopyMemoryBuffer( target, &(hw->hwFilesDirectoryRel) ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( utilCopyMemoryBuffer( target, &(hw->hwFilesDirectoryAbs) ) )
	    { LDEB(1); return -1;	}
	}

    if  ( utilMemoryBufferAppendString( target, slashName ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the 'src' value for an embedded image.				*/
/*									*/
/************************************************************************/

static int docHtmlGetImageSrcX(		MemoryBuffer *		target,
					HtmlWriter *		hw,
					int			relative,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);
    char			scratch[50];

    if  ( 1 || pip->pipBliptag == 0 )
	{ sprintf( scratch, "/i%d.%s", n, ext );			}
    else{ sprintf( scratch, "/b%08lx.%s", pip->pipBliptag, ext );	}

    if  ( docHtmlGetNameX( target, hw, relative, scratch ) )
	{ SDEB(scratch); return -1;	}

    return 0;
    }

static int docHtmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    HtmlWriter *	hw= (HtmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docHtmlGetImageSrcX( target, hw, relative, n, io, ext );
    }

/************************************************************************/
/*									*/
/*  Open the file to write an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docHtmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    HtmlWriter *		hw= (HtmlWriter *)hwc->hwcPrivate;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			relative= 0;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( docHtmlGetImageSrcX( &src, hw, relative, n, io, ext ) < 0 )
	{ LDEB(1); goto ready;	}

    sosImage= sioOutFileioOpen( &src );
    if  ( ! sosImage )
	{ XDEB(sosImage); return sosImage; }

  ready:

    utilCleanMemoryBuffer( &src );

    return sosImage;
    }

/************************************************************************/
/*									*/
/*  Return the name of the CSS file.					*/
/*									*/
/************************************************************************/

static int docHtmlGetCssNameX(		MemoryBuffer *		target,
					int			relative,
					HtmlWriter *		hw )
    {
    const char DocHtmlCssName[]= "/document.css";

    if  ( docHtmlGetNameX( target, hw, relative, DocHtmlCssName ) )
	{ SDEB(DocHtmlCssName); return -1;	}

    return 0;
    }

static int docHtmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc )
    {
    HtmlWriter *	hw= (HtmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docHtmlGetCssNameX( target, relative, hw );
    }

static int docDocHtmlWriteCss(		HtmlWritingContext *	hwc )
    {
    int				rval= 0;

    HtmlWriter *		hw= (HtmlWriter *)hwc->hwcPrivate;
    const int			relative= 0;

    MemoryBuffer		nameCss;
    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;

    utilInitMemoryBuffer( &nameCss );

    if  ( docHtmlGetCssNameX( &nameCss, relative, hw ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sosCss= sioOutFileioOpen( &nameCss );
    if  ( ! sosCss )
	{ XDEB(sosCss); rval= -1; goto ready;	}

    if  ( docHtmlSaveDocumentStyles( hwc, sosCss ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &nameCss );

    if  ( sosCss )
	{ sioOutClose( sosCss );	}

    return rval;
    }
