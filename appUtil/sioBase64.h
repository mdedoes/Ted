/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the MIME (RFC 2045)		*/
/*  'Base 64' content transfer encoding.				*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInBase64Open(
				struct SimpleInputStream * sisBase64 );

extern struct SimpleOutputStream * sioOutBase64OpenFolded(
				struct SimpleOutputStream *	sosBase64,
				int				wide,
				int				lastNl );

extern struct SimpleOutputStream * sioOutBase64Open(
				struct SimpleOutputStream * sosBase64 );
