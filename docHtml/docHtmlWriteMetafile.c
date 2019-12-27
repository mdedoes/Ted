/************************************************************************/
/*									*/
/*  Save the images in a document and include references in the	HTML	*/
/*  file.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	<drawMetafileSvg.h>
#   include	"docSvgDrawImpl.h"
#   include	"docHtmlWriteMetafile.h"

#   include	<sioGeneral.h>

#   include	"docHtmlWriteImpl.h"

#   include	<docObject.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

int docHtmlPlayMetafileToSvgElement(
				HtmlWritingContext *	hwc,
				MetafileWriteSvg	playMetafile,
				InsertedObject *	io,
				const MemoryBuffer *	objectData )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    const int			includeSvgDeclaration= 0;
    SimpleOutputStream * 	sosImage= hwc->hwcXmlWriter.xwSos;

    if  ( docSvgPlayMetafileToDocument( hws->hwsLayoutContext,
					io, includeSvgDeclaration,
					objectData, playMetafile, sosImage ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

int docHtmlPlayMetafileToSvgDataUrl(
				HtmlWritingContext *	hwc,
				MetafileWriteSvg	playMetafile,
				const char *		contentType,
				InsertedObject *	io,
				const MemoryBuffer *	objectData )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);

    const int			includeSvgDeclaration= 1;
    SimpleOutputStream * 	sosImage= (SimpleOutputStream *)0;

    sosImage= xmlStartDataUrl( xw, "src", contentType );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docSvgPlayMetafileToDocument( hws->hwsLayoutContext,
					io, includeSvgDeclaration,
					objectData, playMetafile, sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

    xmlFinishDataUrl( xw, sosImage );
    sosImage= (SimpleOutputStream *)0;

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save metafile to SVG.						*/
/*									*/
/************************************************************************/

int docHtmlPlayMetafileToSvgStream(
				HtmlWritingContext *	hwc,
				int			n,
				InsertedObject *	io,
				const MemoryBuffer *	objectData,
				MetafileWriteSvg	playMetafile,
				const char *		contentType,
				const char *		ext )
    {
    int				rval= 0;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			includeSvgDeclaration= 1;

    if  ( ! hws->hwsOpenImageStream )
	{ XDEB(hws->hwsOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hws->hwsOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ LSXDEB(n,contentType,sosImage); rval= -1; goto ready; }

    if  ( docSvgPlayMetafileToDocument( hws->hwsLayoutContext,
					io, includeSvgDeclaration,
					objectData, playMetafile, sosImage ) )
	{ LSDEB(n,contentType); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

