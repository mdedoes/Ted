/************************************************************************/
/*									*/
/*  SVG printing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_SVG_DRAW_H
#   define		DOC_SVG_DRAW_H

struct SimpleOutputStream;
struct LayoutContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSvgDrawDocument(
			struct SimpleOutputStream *	sos,
			const char *			applicationName,
			const char *			applicationReference,
			const struct LayoutContext *	lc );

#   endif	/*	DOC_SVG_DRAW_H	*/
