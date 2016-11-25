/************************************************************************/
/*									*/
/*  Read/Write UTF-8 sequences at a time.				*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern int sioInGetUtf8(	struct SimpleInputStream *	sis );

extern int sioOutPutUtf8(	int				symbol,
				struct SimpleOutputStream *	sos );

