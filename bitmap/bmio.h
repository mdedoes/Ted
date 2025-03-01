/************************************************************************/
/*									*/
/*  Read/Write raster images from/to input/output streams.		*/
/*									*/
/************************************************************************/

#   ifndef	BM_IO_H
#   define	BM_IO_H

struct BitmapDescription;
struct SimpleInputStream;
struct SimpleOutputStream;
struct RasterImage;
struct DocumentRectangle;

typedef int (*bmReadBitmap)(	struct BitmapDescription *		pBd,
				unsigned char **			pBuffer,
				struct SimpleInputStream *		sis );

typedef int (*bmWriteBitmap)(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bmWmfWriteWmf(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmRtfWriteWmfRtf(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmRtfWriteRtf(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmPngWritePng(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmRtfWritePngRtf(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmJpegWriteJfif(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmRtfWriteJfifRtf(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmGifWriteGif(	const struct BitmapDescription *	bd,
				const unsigned char *			buffer,
				struct SimpleOutputStream *		sos );

extern int bmBmpReadDib(	struct BitmapDescription *	bd,
				unsigned char **		pBuffer,
				struct SimpleInputStream *	sis );

extern int bmJpegReadJfif(	struct BitmapDescription *	bd,
				unsigned char **		pBuffer,
				struct SimpleInputStream *	sis );

extern int bmPngReadPng(	struct BitmapDescription *	bd,
				unsigned char **		pBuffer,
				struct SimpleInputStream *	sis );

extern int bmGifReadGif(	struct BitmapDescription *	bd,
				unsigned char **		pBuffer,
				struct SimpleInputStream *	sis );

extern int bmEpsTestJpegEmbeddable(
			unsigned int *			pPixelsWide,
			unsigned int *			pPixelsHigh,
			unsigned int *			pComponentCount,
			unsigned int *			pBitsPerCompunent,
			struct SimpleInputStream *	sis );

extern int bmPsPrintJpegImage(
			struct SimpleOutputStream *	sos,
			double				xscale,
			double				yscale,
			int				componentCount,
			int				ox,
			int				oy,
			int				pixelsWide,
			int				pixelsHigh,
			int				bitsPerComponent,
			struct SimpleInputStream *	sis );

extern int bmMacPictGetImage(	struct RasterImage *		ri,
				struct DocumentRectangle *	drSource,
				struct DocumentRectangle *	drDest,
				struct SimpleInputStream *	sis,
				int				packed,
				int				direct,
				int				clipRegion );
#   endif
