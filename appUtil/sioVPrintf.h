/************************************************************************/
/*									*/
/*  VPrintf for sio.							*/
/*									*/
/************************************************************************/

#   ifndef	SIO_VPRINTF_H
#   define	SIO_VPRINTF_H

#   include	<stdarg.h>

struct SimpleOutputStream;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int sioOutVPrintf(	struct SimpleOutputStream *	sos,
				const char *			format,
				va_list				ap );
#   endif
