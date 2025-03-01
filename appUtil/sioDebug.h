/************************************************************************/
/*									*/
/*  Simple io streams, Fork input or output.. Usually for debugging	*/
/*  purposes.								*/
/*									*/
/*  Additionally, a pure debugging output stream is provided.		*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInDebugOpen(
				struct SimpleInputStream *	sisIn );

extern struct SimpleOutputStream * sioOutDebugOpen(
				struct SimpleOutputStream *	sosOut );


extern struct SimpleOutputStream * sioOutAppDebugOpen( void );

