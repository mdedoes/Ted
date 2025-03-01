/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in 12 bits LZW	*/
/*  format.								*/
/*									*/
/*  Various implementations of the LZW algorithm differ in details of	*/
/*  the coding of the compressed stream.				*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInLzwGifOpen(
				struct SimpleInputStream *	sisLzw,
				int				codeSize );

extern struct SimpleOutputStream * sioOutLzwGifOpen(
				struct SimpleOutputStream *	sosLzw,
				int				codeSize );
