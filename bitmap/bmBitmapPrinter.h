/************************************************************************/
/*									*/
/*  Emit raster images to PostScript.					*/
/*									*/
/************************************************************************/

#   ifndef	MB_BITMAP_PRINTER_H
#   define	MB_BITMAP_PRINTER_H

struct SimpleOutputStream;
struct BitmapDescription;
struct DocumentRectangle;
struct AffineTransform2D;

/************************************************************************/
/*									*/
/*  Context for writing images to PostScript.				*/
/*									*/
/************************************************************************/

typedef struct BitmapPrinter
    {
    int				bpUseFilters;
    int				bpIndexedImages;
    struct SimpleOutputStream *	bpOutput;
    struct SimpleOutputStream *	bpHexed;
    struct SimpleOutputStream *	bpBase85;
    struct SimpleOutputStream *	bpFlate;
    } BitmapPrinter;

extern int bmPsOpenBitmapPrinter(
				BitmapPrinter *			bp,
				struct SimpleOutputStream *	sos,
				int				useFilters,
				int				indexedImages );

extern void bmCloseBitmapPrinter(	BitmapPrinter *		bp );

extern void bmStartEpsFile(	struct SimpleOutputStream *	sos,
				const char *			filename,
				const struct BitmapDescription *	bd );

/************************************************************************/
/*									*/
/*  Write part of a bitmap to postscript.				*/
/*									*/
/************************************************************************/

extern int bmPsPrintBitmap(	struct SimpleOutputStream *	sos,
				double				xscale,
				double				yscale,
				int				ox,
				int				oy,
				const struct DocumentRectangle *	drSrc,
				int				useFilters,
				int				indexedImages,
				const struct BitmapDescription *	bd,
				const unsigned char *		buffer );

extern int bmPsPrintRasterImage( struct SimpleOutputStream *	sos,
				const struct AffineTransform2D * at,
				const struct DocumentRectangle * drSrc,
				int				onWhite,
				int				useFilters,
				int				indexedImages,
				const struct BitmapDescription * bd,
				const unsigned char *		buffer );

extern void bmPsWriteImageInstructions(
				struct SimpleOutputStream *	sos,
				const struct BitmapDescription *	bd,
				int				onWhite,
				int				wide,
				int				high,
				const char *			source,
				int				indexedImages );

extern int bmPsWriteBitmapData(	BitmapPrinter *			bp,
				const struct DocumentRectangle *	drSrc,
				const struct BitmapDescription *	bd,
				const unsigned char *		inputBuffer );

extern int bmPsRowStringSize(	const struct BitmapDescription *	bd,
				int				pixelsWide,
				int				indexedImages );

#   endif
