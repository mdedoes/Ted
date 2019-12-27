/************************************************************************/
/*									*/
/*  Save a drawing shape to HTML. (SVG actually)			*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_DRAWING_SHAPE_H
#   define		DOC_HTML_WRITE_DRAWING_SHAPE_H

struct HtmlWritingContext;
struct SimpleOutputStream;
struct InsertedObject;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docHtmlSaveDrawingShapeToSvgElement(
				struct HtmlWritingContext *	hwc,
				struct InsertedObject *	io,
				int				includeSvgDecl,
				struct SimpleOutputStream *	sosImage );

extern int docHtmlSaveDrawingShapeToSvgStream(
				struct HtmlWritingContext *	hwc,
				int				n,
				struct InsertedObject *		io,
				const char *			contentType,
				const char *			ext );

extern void docHtmlSetShapeStyle(
				char *				style,
				struct HtmlWritingContext *	hwc,
				const struct BufferItem *	paraNode,
				const struct InsertedObject *	io );

extern int docHtmlSaveDrawingShapeToSvgDataUrl(
				struct HtmlWritingContext *	hwc,
				const char *			contentType,
				struct InsertedObject *		io );

#   endif	/*	DOC_HTML_WRITE_DRAWING_SHAPE_H	*/
