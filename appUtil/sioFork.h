/************************************************************************/
/*									*/
/*  Simple io streams, Fork input or output.. Usually for debugging	*/
/*  purposes.								*/
/*									*/
/************************************************************************/

struct SimpleOutputStream;
struct SimpleInputStream;

extern struct SimpleInputStream * sioInForkOpen(
				struct SimpleInputStream *	sisIn,
				struct SimpleOutputStream *	sosFork );

extern struct SimpleOutputStream * sioOutForkOpen(
				struct SimpleOutputStream *	sosOut,
				struct SimpleOutputStream *	sosFork );

