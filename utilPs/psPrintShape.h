#   ifndef	PS_PRINT_SHAPE_H
#   define	PS_PRINT_SHAPE_H

struct SimpleOutputStream;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void psDrawRoundRectPath(
			struct SimpleOutputStream *		sos,
			const struct DocumentRectangle *	drAround,
			int					r,
			const char *				psOperator );

#   endif	/*  PS_PRINT_SHAPE_H  */
