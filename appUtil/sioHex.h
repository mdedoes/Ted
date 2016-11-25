/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in hexadecimal	*/
/*  format.								*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInHexOpen(	SimpleInputStream * sisHex );

extern SimpleOutputStream * sioOutHexOpenFolded(
				    SimpleOutputStream *	sosHex,
				    int				wide );

extern SimpleOutputStream * sioOutHexOpen(	SimpleOutputStream * sosHex );
