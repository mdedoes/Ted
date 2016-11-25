/************************************************************************/
/*									*/
/*  Simple io stream derived from the POST or PUT input of a CGI	*/
/*  request.								*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>
#   include	<appCgiIn.h>

extern SimpleInputStream * sioInCgiOpen(	CGIRequest *	cgir );

