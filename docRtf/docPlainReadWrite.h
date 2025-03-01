/************************************************************************/
/*									*/
/*  Read/Write plain text documents and selections (For Copy/Paste)	*/
/*									*/
/************************************************************************/

#   ifndef		DOC_PLAIN_RW_H
#   define		DOC_PLAIN_RW_H

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

extern struct BufferDocument *	docPlainReadFile(
				    struct SimpleInputStream *	sis,
				    int *			pMxL,
				    const struct DocumentGeometry * dg );

extern int docPlainSaveDocument(struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct DocumentSelection * ds,
				int				fold );

#   endif	/*	DOC_PLAIN_RW_H	*/
