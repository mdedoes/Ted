/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in UTF16 format 	*/
/*  while the program communicates in UTF8 format.			*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInUtf8_16Open(
				    struct SimpleInputStream * 	sisUtf16,
				    int				endian );

extern struct SimpleOutputStream * sioOutUtf8_16Open(
				    struct SimpleOutputStream *	sosUtf16,
				    int				endian,
				    int				bom );
