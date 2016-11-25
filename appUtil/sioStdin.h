/************************************************************************/
/*									*/
/*  Simple input stream from stdin.					*/
/*									*/
/*  NOTE that this is for non-interactive programs. No attempt is made	*/
/*	to synchronize with stdout.					*/
/*									*/
/************************************************************************/

struct SimpleInputStream;

extern struct SimpleInputStream * sioInStdinOpen( void );
