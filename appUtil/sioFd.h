/************************************************************************/
/*									*/
/*  Simple output stream to a particular file descriptor.		*/
/*									*/
/************************************************************************/

struct SimpleOutputStream;
struct SimpleInputStream;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Error return values.						*/
/*									*/
/************************************************************************/

#   define	FDerrPERM		(-1)	/* -EPERM		*/
#   define	FDerrNOENT		(-2)	/* -ENOENT		*/
#   define	FDerrEXIST		(-17)	/* -EXIST		*/
#   define	FDerrACCES		(-13)	/* -EACCES		*/
#   define	FDerrISDIR		(-21)	/* -EISDIR		*/
#   define	FDerrOTHER		(-9999)	/* Unclassified cause	*/

/************************************************************************/
/*									*/
/*  Routine dectarations.						*/
/*									*/
/************************************************************************/

extern struct SimpleOutputStream * sioOutFdOpen( int		fd );

extern struct SimpleInputStream * sioInFdOpen( int		fd );

extern int sioFdOpenFile(	const struct MemoryBuffer *	filename,
				int				read,
				int				write,
				int				append,
				int				create,
				int				exclusive );

extern int sioFdCloseFile(		int			fd );

extern struct SimpleOutputStream * sioOutFdOpenAt(
				    int				fd,
				    long			offset );

extern struct SimpleInputStream * sioInFdOpenAt(
				    int				fd,
				    long			offset );

