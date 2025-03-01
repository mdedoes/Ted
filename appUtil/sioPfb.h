/************************************************************************/
/*									*/
/*  Simple io streams using that reads PostScript 'pfb' fonts as if	*/
/*  they are in 'pfa' format.						*/
/*									*/
/************************************************************************/

struct SimpleInputStream;

extern struct SimpleInputStream * sioInPfbOpen(	struct SimpleInputStream * sisPfb );
