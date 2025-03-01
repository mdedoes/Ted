/************************************************************************/
/*									*/
/*  Simple io streams for character set conversions.			*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInIconvOpen(
					struct SimpleInputStream *	sisIn,
					const char *		tocode,
					const char *		frcode );

extern struct SimpleOutputStream * sioOutIconvOpen(
					struct SimpleOutputStream *	sosOut,
					const char *		tocode,
					const char *		frcode );

