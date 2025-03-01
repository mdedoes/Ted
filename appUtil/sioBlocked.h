/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in 256 byte packets	*/
/*  The last packet of the stream has zero length indicator.		*/
/*  [Like in the GIF spec.]						*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInBlockedOpen(
				struct SimpleInputStream * sisBlocks );

extern struct SimpleOutputStream * sioOutBlockedOpen(
				struct SimpleOutputStream * sosBlocks );
