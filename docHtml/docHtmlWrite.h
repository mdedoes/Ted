/************************************************************************/
/*									*/
/*  Save a Document as a HTML file.					*/
/*									*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/*  Additionally, the HTML writing code is used to save documents in	*/
/*  EPUB format. See http://www.idpf.org/specs.htm.			*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_H
#   define		DOC_HTML_WRITE_H

struct SimpleOutputStream;
struct MemoryBuffer;
struct BufferDocument;
struct LayoutContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docHtmlSaveDocument(	struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct MemoryBuffer *	filename,
				const struct LayoutContext *	lc,
				int				inlineImages );

extern int docEmlSaveDocument(	struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct LayoutContext *	lc );

extern int docEpubSaveDocument(	struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct LayoutContext *	lc );

#   endif	/*	DOC_HTML_WRITE_H	*/

