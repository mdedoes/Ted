/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the PostsScript ASCII base 85	*/
/*  encoding.								*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInBase85Open(
				struct SimpleInputStream * sisBase85 );

extern struct SimpleOutputStream * sioOutBase85Open(
				struct SimpleOutputStream * sosBase85 );
