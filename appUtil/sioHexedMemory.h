/************************************************************************/
/*									*/
/*  Simple io streams to/from MemoryBuffers with hexed (binary) content.*/
/*  E.G: The 'Objects' in an RTF file.					*/
/*									*/
/************************************************************************/

#   ifndef	SIO_HEXED_MEMORY_BUFFER_H
#   define	SIO_HEXED_MEMORY_BUFFER_H

struct SimpleInputStream;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct SimpleInputStream * sioInHexedMemoryOpen(
					const struct MemoryBuffer *	mb );

extern struct SimpleOutputStream * sioOutHexedMemoryOpen(
					struct MemoryBuffer *		mb );

#   endif	/*  SIO_HEXED_MEMORY_BUFFER_H	*/
