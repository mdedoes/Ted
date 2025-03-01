/************************************************************************/
/*									*/
/*  Writing RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_WRITER_H
#   define		RTF_WRITER_H

struct SimpleOutputStream;
struct BufferDocument;

struct RtfWriter;
typedef struct RtfWriter RtfWriter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern RtfWriter *	docRtfOpenWriter(
				struct SimpleOutputStream *	sos,
				struct BufferDocument *		bd,
				int				flags );

extern void docRtfCloseWriter(	RtfWriter *	rw );

#   endif	/*	RTF_WRITER_H	*/
