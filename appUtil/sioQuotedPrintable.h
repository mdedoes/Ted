/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the MIME (RFC 2045)		*/
/*  'Quoted Printable' content transfer encoding.			*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInQuotedPrintableOpen(
				    struct SimpleInputStream * sisQuoted );

extern struct SimpleOutputStream * sioOutQuotedPrintableOpen(
				    struct SimpleOutputStream * sosQuoted );
