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
struct RGB8Color;

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

extern int docCssWriteProperty(	int *				pCol,
				struct SimpleOutputStream *	sos,
				const char *			property,
				const char *			value );

extern int docCssWriteNumber(	int *				pCol,
				struct SimpleOutputStream *	sos,
				const char *			property,
				int				value );

extern int docCssWriteDimension( int *				pCol,
				struct SimpleOutputStream *	sos,
				const char *			property,
				int				value,
				const char *			unit );

extern int docCssWriteRgb8Color( int *				pCol,
				struct SimpleOutputStream *	sos,
				const char *			property,
				const struct RGB8Color *	rgb8 );

extern int docCssWritelnProperty( struct SimpleOutputStream *	sos,
				int				indent,
				const char *			property,
				const char *			value );

extern int docCssWritelnNumber( struct SimpleOutputStream *	sos,
				int				indent,
				const char *			property,
				int				value );

extern int docCssWritelnDimension( struct SimpleOutputStream *	sos,
				int				indent,
				const char *			property,
				int				value,
				const char *			unit );

extern int docCssWritelnRgb8Color( struct SimpleOutputStream *	sos,
				int				indent,
				const char *			property,
				const struct RGB8Color *	rgb8 );

#   endif	/*	DOC_WRITE_CSS_H	*/

