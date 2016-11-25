/************************************************************************/
/*									*/
/*  Simple io streams using blocks of memory.				*/
/*									*/
/************************************************************************/

#   ifndef	SIO_MEMORY_H
#   define	SIO_MEMORY_H

struct SimpleInputStream;
struct SimpleOutputStream;
struct MemoryBuffer;

extern struct SimpleInputStream * sioInMemoryOpen(
					const struct MemoryBuffer *	mb );

extern struct SimpleInputStream * sioInMemoryOpenFrom(
					const struct MemoryBuffer *	mb,
					int				from );

extern struct SimpleInputStream * sioInMemoryArrayOpen(
					const struct MemoryBuffer *	mb,
					int				count );

extern struct SimpleOutputStream * sioOutMemoryOpen(
					struct MemoryBuffer *		mb );

extern struct SimpleOutputStream * sioOutMemoryOpenTo(
					struct MemoryBuffer *		mb,
					int				to );

#   endif
