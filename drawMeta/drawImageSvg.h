/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac to SVG.					*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_IMAGE_SVG_H
#   define		DRAW_IMAGE_SVG_H

struct SvgWriter;
struct RasterImage;
struct DocumentRectangle;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int drawRasterImageSvg(
			struct SvgWriter *			sw,
			const struct RasterImage *		riSrc,
			const struct DocumentRectangle *	drSrc,
			const struct DocumentRectangle *	drDest );

extern int drawRasterImageSvgFromData(
			struct SvgWriter *			sw,
			const char *				contentType,
			const struct MemoryBuffer *		mb,
			const struct DocumentRectangle *	drDest );

#   endif	/*	DRAW_IMAGE_SVG_H	*/
