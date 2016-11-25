/************************************************************************/
/*									*/
/*  Read/Write RTF files.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_RW_H
#   define	DOC_RTF_RW_H

struct DocumentSelection;
struct BufferDocument;
struct SimpleInputStream;
struct SimpleOutputStream;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docWriteRtfMail(	struct SimpleOutputStream *	sos,
				const char *			mimeBoundary,
				struct BufferDocument *		bd );

extern int docRtfSaveDocument(	struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				const struct DocumentSelection * bs,
				int				flags );

extern struct BufferDocument *	docRtfReadFile(
				struct SimpleInputStream *	sis,
				unsigned int			flags );

#    endif	/*  DOC_RTF_RW_H	*/
