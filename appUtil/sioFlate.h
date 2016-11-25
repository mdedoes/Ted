/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in In/Deflate format	*/
/*  using zlib.								*/
/*									*/
/************************************************************************/

struct SimpleOutputStream;
struct SimpleInputStream;

extern struct SimpleInputStream * sioInFlateOpen(
				struct SimpleInputStream *	sisFlate );

extern struct SimpleOutputStream * sioOutFlateOpen(
				struct SimpleOutputStream *	sosFlate,
				int				gzipEmbedded );
