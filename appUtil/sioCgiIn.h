/************************************************************************/
/*									*/
/*  Simple io stream derived from the POST or PUT input of a CGI	*/
/*  request. (It consumes the input to the PUT or the POST.		*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct CGIRequest;

extern struct SimpleInputStream * sioInCgiOpen(	struct CGIRequest *	cgir );

