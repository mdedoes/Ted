/************************************************************************/
/*									*/
/*  Elementary functionalite to emit XML.				*/
/*									*/
/************************************************************************/

#   ifndef		XML_WRITER_H
#   define		XML_WRITER_H

struct SimpleOutputStream;
struct RGB8Color;
struct MemoryBuffer;

typedef struct XmlWriter
    {
    struct SimpleOutputStream *	xwSos;
    int				xwCrlf;
    int				xwColumn;
    } XmlWriter;

extern const char	XML_DECLARATION[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void xmlInitXmlWriter(		XmlWriter *		xw );

extern void xmlCloseXmlWriter(		XmlWriter *		xw );

extern void xmlPutString(		const char *		s,
					XmlWriter *		xw );

extern void xmlNewLine(			XmlWriter *		xw );

extern void xmlWriteStringAttribute(	XmlWriter *		xw,
					const char *		name,
					const char *		value );

extern void xmlWriteBufferAttribute(	XmlWriter *		xw,
					const char *		name,
					const struct MemoryBuffer *	mb );

extern void xmlWriteIntAttribute(	XmlWriter *		xw,
					const char *		name,
					int			value );

extern void xmlWriteRgb8Attribute(	XmlWriter *		xw,
					const char *		name,
					const struct RGB8Color *	rgb8 );

extern void xmlEscapeCharacters(	XmlWriter *		xw,
					const char *		ss,
					int			len );

extern void xmlEscapeBuffer(	XmlWriter *			xw,
				const struct MemoryBuffer *	mb );

extern struct SimpleOutputStream * xmlStartDataUrl(
				XmlWriter *		xw,
				const char *		attributeName,
				const char *		contentType );

extern void xmlFinishDataUrl(	XmlWriter *			xw,
				struct SimpleOutputStream *	sosImage );

extern int xmlWriteDataUrl(	XmlWriter *			xw,
				const char *			attributeName,
				const char *			contentType,
				const struct MemoryBuffer *	objectData );

#   endif	/*	XML_WRITER_H	*/

