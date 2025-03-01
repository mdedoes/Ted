/************************************************************************/
/*									*/
/*  Simple io streams, Using the LZ77 algorithm of Microsoft's		*/
/*  compress.exe and lzexpand.dll.					*/
/*									*/
/************************************************************************/

struct SimpleInputStream;

extern struct SimpleInputStream * sioInMsLz77Open(struct SimpleInputStream *	sisMsLz77 );
