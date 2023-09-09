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
#   include		<utilIndexMapping.h>
#   include		<bmio.h>
#   include		<drawMetafileSvg.h>

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
typedef struct HtmlWritingSettings
    {
				/**
				 *  Directly include CSS in the HTML file.
				 */
    unsigned char		hwsInlineCss;

				/**
				 *  Determine the href attribute of the
				 *  stylesheet
				 */
    HtmlGetCssName		hwsGetCssName;

				/**
				 *  Include image data as a data url
				 */
    unsigned char		hwsInlineImages;

				/**
				 *  Open an output stream to write an image to.
				 */
    HtmlOpenImageStream		hwsOpenImageStream;

				/**
				 *  Determine the src or href attribute of 
				 *  an image or a similar object
				 */
    HtmlGetImageSrc		hwsGetImageSrc;

				/**
				 *  Insert notes at the end of the paragraph 
				 *  or a top level table, rather than at the 
				 *  end of the document.
				 */
    unsigned char		hwsInlineNotes;

    unsigned char		hwsEmitBackground;

    const struct LayoutContext * hwsLayoutContext;
    struct BufferDocument *	hwsDocument;
    } HtmlWritingSettings;

typedef struct HtmlWritingContext
    {
    XmlWriter			hwcXmlWriter;
    const HtmlWritingSettings *	hwcSettings;

    BlockFrame			hwcBlockFrame;
    ParagraphFrame		hwcParagraphFrame;

    void *			hwcPrivate;

    IndexMapping		hwcDeferredNotes;

    int				hwcInHyperlink;
    int				hwcInBookmark;
    int				hwcInPageref;
    int				hwcBytesInLink;
    ParagraphProperties		hwcParagraphProperties;


    int				hwcImageCount;
    int				hwcNoteRefCount;
    int				hwcNoteDefCount;

    int				hwcTableNesting;

    int				hwcCurrentAttributeNumber;
    } HtmlWritingContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitHtmlWritingSettings(	HtmlWritingSettings *	hws );
extern void docInitHtmlWritingContext(	HtmlWritingContext *	hwc );
extern void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc );

extern int docStartHtmlWritingContext(	HtmlWritingContext *		hwc,
					const HtmlWritingSettings *	hws,
					struct SimpleOutputStream *	sos );

extern void docHtmlPutString(		const char *		s,
					HtmlWritingContext *	hwc );

extern void docHtmlNewLine(		HtmlWritingContext *	hwc );

extern void docHtmlWritelnCloseElement(	const char *		element,
					HtmlWritingContext *	hwc );

extern int docHtmlSaveInsertedObject(
				int *				pDone,
				HtmlWritingContext *		hwc,
				int				n,
				const struct BufferItem *	paraNode,
				struct InsertedObject *		io );

extern int docHtmlSaveImageFiles(	HtmlWritingContext *		hwc );

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

extern int docHtmlSaveSelection( HtmlWritingContext *			hwc,
				struct DocumentTree *			dt,
				const struct DocumentSelection *	ds );

extern void docHtmlEmitBackgroundProperty(
					const struct ItemShading *	is,
					HtmlWritingContext *	hwc );

extern int docHtmlEmitProlog(	HtmlWritingContext *	hwc,
				const char *		localeTag,
				const char *		extraNameSpaces );

extern int docHtmlWriteStyleLink( HtmlWritingContext *		hwc,
				const struct MemoryBuffer *	mbStylesheet );

extern int docHtmlWriteTitle(	HtmlWritingContext *		hwc,
				const struct MemoryBuffer *	mbTitle );

extern int docHtmlStartDocument(	HtmlWritingContext *	hwc );
extern int docHtmlFinishDocumentBody(	HtmlWritingContext *	hwc );

extern int docHtmlObjectSaveHow(
			const struct HtmlWritingContext *	hwc,
			int *					pUseDataUrl,
			int *					pType,
			bmWriteBitmap *				pWriteBitmap,
			MetafileWriteSvg *			pWriteSvg,
			const char **				pMimeType,
			const char **				pExt,
			const struct MemoryBuffer **		pObjectData,
			const struct InsertedObject *		io );

extern int docHtmlSaveDeferredNotes(	HtmlWritingContext *		hwc );

extern int docHtmlSaveEndnotes(	int				sect,
				struct HtmlWritingContext *	hwc );

extern int docHtmlSaveFootnotes( struct HtmlWritingContext *	hwc );

extern int docHtmlStartNote(	const struct DocumentField *	df,
				HtmlWritingContext *		hwc,
				const struct BufferItem *	node,
				int				attNr );

extern int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
				int				isNote,
				const struct MemoryBuffer *	fileName,
				const struct MemoryBuffer *	mbTarget,
				const struct MemoryBuffer *	mbSource,
				const struct MemoryBuffer *	mbTitle );

extern int dochtmlEnterSectNode( struct BufferItem *		sectNode,
				struct HtmlWritingContext *	hwc );

extern int dochtmlLeaveSectNode( struct BufferItem *		sectNode,
				struct HtmlWritingContext *	hwc );

extern const char * docHtmlCellElement(
				const struct BufferItem *	cellNode );

extern int docHtmlEnterCellNode(
			struct HtmlWritingContext *	hwc,
			const struct BufferItem *	rowNode,
			int				col,
			const struct BufferItem *	cellNode );

extern int docHtmlEnterRowNode(	struct HtmlWritingContext *	hwc,
				const struct BufferItem *	rowNode );

extern int docHtmlFinishTable(	struct HtmlWritingContext *	hwc );

#   endif	/*	DOC_HTML_WRITE_IMPL_H	*/

