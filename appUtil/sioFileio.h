/************************************************************************/
/*									*/
/*  Simple io streams using raw file IO.				*/
/*									*/
/************************************************************************/

# ifndef SIO_FILEIO_H
# define SIO_FILEIO_H

struct SimpleOutputStream;
struct SimpleInputStream;
struct MemoryBuffer;

extern struct SimpleOutputStream * sioOutFileioOpen(
				    const struct MemoryBuffer *	filename );

extern struct SimpleOutputStream * sioOutFileioOpenForAppend(
				    const struct MemoryBuffer *	filename );

extern struct SimpleOutputStream * sioOutFileioOpenS(
				    const char *		filename );

extern struct SimpleOutputStream * sioOutFileioOpenTempFile(
				    struct MemoryBuffer *	filename,
				    const char *		template );

extern struct SimpleInputStream * sioInFileioOpen(
				    const struct MemoryBuffer *	filename );

extern struct SimpleInputStream * sioInFileioOpenS(
				    const char *		filename );

# endif
