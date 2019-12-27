/************************************************************************/
/*									*/
/*  Save a drawing shape to HTML. (SVG actually)			*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_METAFILE_H
#   define		DOC_HTML_WRITE_METAFILE_H

struct HtmlWritingContext;
struct InsertedObject;
struct MemoryBuffer;

#   include	<drawMetafileSvg.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docHtmlPlayMetafileToSvgElement(
				struct HtmlWritingContext *	hwc,
				MetafileWriteSvg		playMetafile,
				struct InsertedObject *		io,
				const struct MemoryBuffer *	objectData );

extern int docHtmlPlayMetafileToSvgStream(
				struct HtmlWritingContext *	hwc,
				int				n,
				struct InsertedObject *		io,
				const struct MemoryBuffer *	objectData,
				MetafileWriteSvg		playMetafile,
				const char *			contentType,
				const char *			ext );

extern int docHtmlPlayMetafileToSvgDataUrl(
				struct HtmlWritingContext *	hwc,
				MetafileWriteSvg		playMetafile,
				const char *			contentType,
				struct InsertedObject *		io,
				const struct MemoryBuffer *	objectData );

#   endif	/*	DOC_HTML_WRITE_METAFILE_H	*/
