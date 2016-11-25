/************************************************************************/
/*									*/
/*  Save attributes as CSS.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_WRITE_CSS_H
#   define		DOC_WRITE_CSS_H

struct IndexSet;
struct ItemShading;
struct SimpleOutputStream;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docCssEmitBackgroundStyle(
				int *				pCol,
				struct SimpleOutputStream *	sos,
				const struct BufferDocument *	bd,
				const struct ItemShading *	is );

extern int docCssUseBackgroundStyle(
				const struct ItemShading *	is,
				const struct BufferDocument *	bd );

extern void docCssEmitBorderStyleByNumber(
				int *				pCol,
				struct SimpleOutputStream *	sos,
				const struct BufferDocument *	bd,
				const char *			whatBorder,
				int				num );

extern int docCssSaveTextAttributeStyles(
				struct SimpleOutputStream *	sos,
				const struct IndexSet *		used,
				const struct BufferDocument *	bd );

#   endif	/*	DOC_WRITE_CSS_H	*/

