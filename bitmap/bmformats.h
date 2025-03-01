#   ifndef	BM_FORMATS_H
#   define	BM_FORMATS_H

#   include	"bitmap.h"

struct MemoryBuffer;

# define USE_BMP 1
# define USE_ICO 1
# define USE_WBMP 1
# define USE_XWD 1
# define USE_GIF 1
# define USE_EPS 1
# define USE_PDF 1
# define USE_XBM 1
# define USE_WMF 1
# define USE_PBM 1
# define USE_RTF 1

/************************************************************************/
/*									*/
/*  Routines.								*/
/*									*/
/************************************************************************/

# if USE_LIBTIFF

extern int bmReadTiffFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWriteTiffFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteTiffFile( const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_LIBWEBP

extern int bmReadWebpFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWriteWebpFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteWebpFile( const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_BMP

extern int bmReadBmpFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteBmpFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteBmpFile(	const struct MemoryBuffer * filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

# endif

# if USE_ICO

extern int bmReadIcoFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteIcoFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteIcoFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

# endif

# if USE_WBMP

extern int bmReadWbmpFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteWbmpFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteWbmpFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

# endif

# if USE_XWD

extern int bmReadXwdFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

# endif

# if USE_GIF

extern int bmReadGifFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteGifFile(	const BitmapDescription *	bd,
				int				privateFormat );

extern int bmWriteGifFile(	const struct MemoryBuffer *	filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

# endif

# if USE_EPS

extern int bmCanWriteEpsFile(	const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteEpsFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_PDF

extern int bmCanWritePdfFile(	const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWritePdfFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_LIBJPEG

extern int bmReadJpegFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWriteJpegFile(	const struct MemoryBuffer *	filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWriteJpegFile(	const BitmapDescription *	bd,
				int				privateFormat );

# endif

# if USE_XBM

extern int bmReadXbmFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteXbmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteXbmFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_WMF

extern int bmCanWriteWmfFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteWmfFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_LIBPNG

extern int bmReadPngFile(	const struct MemoryBuffer * filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWritePngFile(	const BitmapDescription *	bd,
				int				privateFormat );

extern int bmWritePngFile(	const struct MemoryBuffer * filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_LIBXPM

extern int bmReadXpmFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteXpmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteXpmFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_PBM

extern int bmReadPbmFile(	const struct MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWritePbmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWritePbmFile(	const struct MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

# endif

# if USE_RTF

extern int bmWriteRtfFile(	const struct MemoryBuffer *	filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWriteRtfFile(	const BitmapDescription *	bd,
				int				privateFormat );

# endif

#   endif /*  BM_FORMATS_H	*/
