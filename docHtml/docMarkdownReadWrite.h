/************************************************************************/
/*									*/
/*  Read/Write markdown text documents and selections (For Copy/Paste)	*/
/*									*/
/************************************************************************/

#   ifndef		DOC_MARKDOWN_RW_H
#   define		DOC_MARKDOWN_RW_H

struct DocumentGeometry;
struct BufferDocument;
struct DocumentSelection;
struct SimpleInputStream;
struct SimpleOutputStream;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct BufferDocument *	docMarkdownReadFile(
				    struct SimpleInputStream *	sis,
				    const struct DocumentGeometry * dg );

extern int docMarkdownSaveDocument(struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct DocumentSelection * ds );

#   endif	/*	DOC_MARKDOWN_RW_H	*/
