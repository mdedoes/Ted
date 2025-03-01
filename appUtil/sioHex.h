/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in hexadecimal	*/
/*  format.								*/
/*									*/
/************************************************************************/

struct SimpleOutputStream;
struct SimpleInputStream;

extern struct SimpleInputStream * sioInHexOpen(
				struct SimpleInputStream * sisHex );

extern struct SimpleOutputStream * sioOutHexOpenFolded(
				struct SimpleOutputStream *	sosHex,
				int				wide,
				int				lastNl );

extern struct SimpleOutputStream * sioOutHexOpen(
				struct SimpleOutputStream * 	sosHex );
