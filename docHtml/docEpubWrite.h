/************************************************************************/
/*									*/
/*  EPUB writing.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EPUB_WRITE_H
#   define	DOC_EPUB_WRITE_H

#   include	<xmlWriter.h>
#   include	<docCalculateToc.h>
#   include	<sioZip.h>

struct InsertedObject;
struct HtmlWritingContext;
struct MemoryBuffer;

typedef struct EpubWriter
    {
    CalculateToc	ewCalculateToc;
    ZipOutput		ewZipOutput;
    } EpubWriter;

typedef struct EpubXmlWriter
    {
    struct HtmlWritingContext *	exwHtmlWriter;
    XmlWriter			exwXmlWriter;
    } EpubXmlWriter;

extern const char	DocEpubNameOpf[];

extern const char	DocEpubIdNcx[];
extern const char	DocEpubIdNavig[];
extern const char	DocEpubIdCss[];
extern const char	DocEpubIdDoc[];

extern const char	DocEpubNameMimetype[];
extern const char	DocEpubNameContainer[];
extern const char	DocEpubNameNcx[];
extern const char	DocEpubNameNavig[];
extern const char	DocEpubNameContent[];
extern const char	DocEpubNameDocAbs[];
extern const char	DocEpubNameDocRel[];
extern const char	DocEpubNameMediaAbs[];
extern const char	DocEpubNameMediaRel[];

extern const char	DocEpubNameCssAbs[];
extern const char	DocEpubNameCssRel[];

extern const char	DocEpubMediaNcx[];
extern const char	DocEpubMediaNavig[];
extern const char	DocEpubMediaCss[];
extern const char	DocEpubMediaDoc[];
extern const char	DocEpubMediaEpub[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docEpubGetImageSrcX(	struct MemoryBuffer *		target,
				int				relative,
				int				n,
				const struct InsertedObject *	io,
				const char *			ext );

extern int docEpubEmitSimpleOpf( struct HtmlWritingContext *	hwc,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	identifier );

extern int docEpubEmitSimpleNcx( struct HtmlWritingContext *	hwc,
				const MemoryBuffer *		title,
				const struct MemoryBuffer *	identifier );

extern int docEpubEmitCompositeNcx(
				struct HtmlWritingContext *	hwc,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	identifier );

extern int docEpubEmitSimpleNavig(
				struct HtmlWritingContext *	hwc,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	identifier );

extern int docEpubEmitCompositeNavig(
				struct HtmlWritingContext * 	hwc,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	identifier );

#   endif	/*	DOC_EPUB_WRITE_H	*/
