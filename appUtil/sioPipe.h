/************************************************************************/
/*									*/
/*  sioPipe.[ch]:	sio equivalents to popen/pclose.		*/
/*									*/
/************************************************************************/

#   ifndef	SIO_PIPE_H
#   define	SIO_PIPE_H

struct SimpleInputStream;
struct SimpleOutputStream;
struct MemoryBuffer;

extern struct SimpleInputStream * sioInPipeOpen(
				const struct MemoryBuffer *	command );

extern struct SimpleOutputStream * sioOutPipeOpen(
				const struct MemoryBuffer *	command );

#   endif
