/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/*  Additionally, the HTML writing code is used to save documents in	*/
/*  EPUB format. See http://www.idpf.org/specs.htm.			*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_IMPL_H
#   define		DOC_HTML_WRITE_IMPL_H

#   include		<xmlWriter.h>
#   include		<docBlockFrame.h>
#   include		<docStripFrame.h>
#   include		<docParaProperties.h>
#   include		<textAttribute.h>
#   include		<utilIndexMapping.h>

struct InsertedObject;
struct DocumentField;
struct ItemShading;
struct DocumentSelection;
struct DocumentTree;

			/**
			 * Map sizes: Assume 96 dpi
			 */
#   define	TWIPS_TO_PIXELS(x)	(((x)+7)/15)

#   define	USE_PNG			1
#   define	USE_GIF			0

/************************************************************************/
/*									*/
/*  Information used when writing HTML.					*/
/*									*/
/************************************************************************/

struct HtmlWritingContext;

typedef struct SimpleOutputStream * (*HtmlOpenImageStream)(
					struct HtmlWritingContext *	hwc,
					int				n,
					const struct InsertedObject *	io,
					const char *			mimeT,
					const char *			ext );

typedef int (*HtmlGetImageSrc)(		struct MemoryBuffer *		target,
					struct HtmlWritingContext *	hwc,
					int				n,
					const struct InsertedObject *	io,
					const char *			ext );

typedef int (*HtmlGetCssName)(		struct MemoryBuffer *		target,
					struct HtmlWritingContext *	hwc );
typedef struct HtmlWritingContext
    {
    XmlWriter			hwcXmlWriter;

    const struct LayoutContext * hwcLayoutContext;
    BlockFrame			hwcBlockFrame;
    ParagraphFrame		hwcParagraphFrame;

    HtmlOpenImageStream		hwcOpenImageStream;
    HtmlGetImageSrc		hwcGetImageSrc;
    HtmlGetCssName		hwcGetCssName;
    void *			hwcPrivate;
    struct BufferDocument *	hwcDocument;

    IndexMapping		hwcDeferredNotes;

    int				hwcInlineCss;
    int				hwcInlineNotes;

    int				hwcInHyperlink;
    int				hwcInBookmark;
    int				hwcInPageref;
    int				hwcBytesInLink;
    TextAttribute		hwcDefaultAttribute;
    ParagraphProperties		hwcParagraphProperties;


    int				hwcImageCount;
    int				hwcNoteRefCount;
    int				hwcNoteDefCount;

    int				hwcTableNesting;

    int				hwcCurrentAttributeNumber;
					/**
					 *  Support bullets? Only in  browser!
					 */
    unsigned char		hwcSupportsBullets;
    unsigned char		hwcEmitBackground;
    } HtmlWritingContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitHtmlWritingContext(	HtmlWritingContext *	hwc );
extern void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc );

extern void docHtmlPutString(		const char *		s,
					HtmlWritingContext *	hwc );

extern void docHtmlNewLine(		HtmlWritingContext *	hwc );

extern int docHtmlSaveImgElement(
				int *				pDone,
				HtmlWritingContext *		hwc,
				int				n,
				const struct BufferItem *	paraNode,
				struct InsertedObject *		io );

extern int docHtmlSaveImages(	HtmlWritingContext *		hwc );

extern void docHtmlChangeAttributes(
				HtmlWritingContext *		hwc,
				int				taNr );

extern void docHtmlWriteStringAttribute( HtmlWritingContext *	hwc,
					const char *		name,
					const char *		value );

extern void docHtmlWriteIntAttribute(	HtmlWritingContext *	hwc,
					const char *		name,
					int			value );

extern void docHtmlEscapeCharacters(	HtmlWritingContext *	hwc,
					const char *		ss,
					int			len );

extern int docHtmlStartField(	const struct DocumentField *	df,
				HtmlWritingContext *		hwc,
				const struct BufferItem *	node,
				int				attNr );

extern int docHtmlFinishField(	const struct DocumentField *	df,
				HtmlWritingContext *		hwc );

extern int docHtmlSaveParaNode( HtmlWritingContext *		hwc,
				struct BufferItem *		paraNode,
				const struct BufferItem *	bodySectNode,
				const struct DocumentSelection *	ds );

extern int docHtmlSaveDocumentStyles(	HtmlWritingContext *	hwc,
					struct SimpleOutputStream *	sos );

extern int docHtmlSaveSelection(	HtmlWritingContext *		hwc,
					struct DocumentTree *		dt,
					const struct DocumentSelection *	ds );

extern void docHtmlEmitBackgroundProperty(
					const struct ItemShading *	is,
					HtmlWritingContext *	hwc );

extern int docHtmlStartDocument(	HtmlWritingContext *	hwc );
extern int docHtmlFinishDocument(	HtmlWritingContext *	hwc );

extern int docHtmlSaveNotes(		HtmlWritingContext *	hwc );

extern int docHtmlObjectSaveHow(int *				pType,
				const char **			pMimeType,
				const char **			pExt,
				const struct MemoryBuffer **	pObjectData,
				const struct InsertedObject *	io );

extern int docHtmlSaveDeferredNotes(	HtmlWritingContext *		hwc );

extern int docHtmlStartNote(	const struct DocumentField *	df,
				HtmlWritingContext *		hwc,
				const struct BufferItem *	node,
				int				attNr );

extern int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
				int				isNote,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	markName,
				const struct MemoryBuffer *	refName,
				const char *			title,
				int				titleSize );

#   endif	/*	DOC_HTML_WRITE_IMPL_H	*/

