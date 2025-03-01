/************************************************************************/
/*									*/
/*  sioHttp.[ch]:	Retrieve an URL.				*/
/*									*/
/************************************************************************/

#   ifndef	SIO_HTTP_H
#   define	SIO_HTTP_H

#   include     "appSystem.h"

struct SimpleInputStream;
struct SimpleOutputStream;

extern int sioHttpOpen(		struct SimpleInputStream **	pSisBody,
				struct SimpleInputStream **	pSisHeader,
				struct SimpleOutputStream **	pSosBody,
				struct SimpleOutputStream **	pSosHeader,
				const char *		host,
				const char *		port,
				const char *		url,
				const char *		method,
				void *			through,
				APP_COMPLAIN		complain );

#   endif
