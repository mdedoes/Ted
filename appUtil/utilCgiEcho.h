#   include	"appCgiIn.h"

struct SimpleOutputStream;

/************************************************************************/
/*									*/
/*  Echo some information on a cgi request.				*/
/*									*/
/************************************************************************/

extern void utilCgiEchoValueList(	void *			tree,
					const char *		label,
					struct SimpleOutputStream *	sos );

extern void utilCgiEchoRequest(		const CGIRequest *	cgir,
					struct SimpleOutputStream *	sos );

