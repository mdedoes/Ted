/************************************************************************/
/*									*/
/*  Simple io streams, Push input or output.. Makes it possible to	*/
/*  to send use an unchanged stream as if it were a different one.	*/
/*  One of the advantages is that you can simply close the pushed clone	*/
/*  without any administration to keep track of the fact that output	*/
/*  goes to the same destination.					*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInPushedOpen(
				struct SimpleInputStream *	sisIn );

extern struct SimpleOutputStream * sioOutPushedOpen(
				struct SimpleOutputStream *	sosOut );

