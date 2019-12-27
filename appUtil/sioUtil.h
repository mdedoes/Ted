/************************************************************************/
/*									*/
/*  Simple IO utility functions.					*/
/*									*/
/************************************************************************/

#   ifndef	SIO_UTIL_H
#   define	SIO_UTIL_H

struct SimpleOutputStream;
struct SimpleInputStream;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int sioCopyStream(	struct SimpleOutputStream *	sos,
				struct SimpleInputStream *	sis );

extern int sioCopyMemoryBufferToFile(
			const char *			fileName,
			const struct MemoryBuffer *	mb );

extern int sioCopyInputStreamToFile(
			const char *			fileName,
			struct SimpleInputStream *	sis );

#   endif
