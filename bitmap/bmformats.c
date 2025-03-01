#   include	"bitmapConfig.h"

#   include	"bmformats.h"
#   include	"bmformat.h"
#   include	<string.h>
#   include	<appSystem.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

#   if	USE_LIBTIFF
#	include	<tiffio.h>

static BitmapFileType	bmTiffFile=
    {
    bmWriteTiffFile,
    bmCanWriteTiffFile,
    bmReadTiffFile,
    "tiff",
    "*.tiff",
    "image/tiff",
    "Tagged Image Format ( *.tiff )",
    };

static BitmapFileType	bmTifFile=
    {
    bmWriteTiffFile,
    bmCanWriteTiffFile,
    bmReadTiffFile,
    "tif",
    "*.tif",
    "image/tiff",
    "Tagged Image Format ( *.tif )",
    };

#   endif	/* USE_LIBTIFF */

static BitmapFileType	bmBmpFile=
    {
    bmWriteBmpFile,
    bmCanWriteBmpFile,
    bmReadBmpFile,
    "bmp",
    "*.bmp",
    "image/bmp",
    "Microsoft Bitmap ( *.bmp )",
    };

static BitmapFileType	bmIcoFile=
    {
    bmWriteIcoFile,
    bmCanWriteIcoFile,
    bmReadIcoFile,
    "ico",
    "*.ico",
    "image/x-icon",
    "Microsoft Icon ( *.ico )",
    };

static BitmapFileType	bmXwdFile=
    {
    NULL, /* bmWriteXwdFile, */
    NULL, /* bmCanWriteXwdFile, */
    bmReadXwdFile,
    "xwd",
    "*.xwd",
    "image/x-xwindowdump",
    "X-Window Dump ( *.xwd )",
    };

#   if	USE_LIBJPEG

static BitmapFileType	bmJpgFile=
    {
    bmWriteJpegFile,
    bmCanWriteJpegFile,
    bmReadJpegFile,
    "jpg",
    "*.jpg",
    "image/jpeg",
    "JPEG ( *.jpg )",
    };

static BitmapFileType	bmJpegFile=
    {
    bmWriteJpegFile,
    bmCanWriteJpegFile,
    bmReadJpegFile,
    "jpeg",
    "*.jpeg",
    "image/jpeg",
    "JPEG ( *.jpeg )",
    };

#   endif /* USE_LIBJPEG */

static BitmapFileType	bmGifFile=
    {
    bmWriteGifFile,
    bmCanWriteGifFile,
    bmReadGifFile,
    "gif",
    "*.gif",
    "image/gif",
    "Compuserve GIF ( *.gif )",
    };

static BitmapFileType	bmEpsFile=
    {
    bmWriteEpsFile,
    bmCanWriteEpsFile,
    NULL, /* bmReadEpsFile, */
    "eps",
    "*.eps",
    "application/postscript",
    "Encapsulated Postscript ( *.eps )",
    };

static BitmapFileType	bmPdfFile=
    {
    bmWritePdfFile,
    bmCanWritePdfFile,
    NULL, /* bmReadPdfFile, */
    "pdf",
    "*.pdf",
    "application/pdf",
    "Acrobat PDF ( *.pdf )",
    };

static BitmapFileType	bmRtfFile=
    {
    bmWriteRtfFile,
    bmCanWriteRtfFile,
    NULL, /* bmReadRtfFile, */
    "rtf",
    "*.rtf",
    "application/rtf",
    "Rich Text Format ( *.rtf )",
    };

static BitmapFileType	bmXbmFile=
    {
    bmWriteXbmFile,
    bmCanWriteXbmFile,
    bmReadXbmFile,
    "xbm",
    "*.xbm",
    "image/x-xbitmap",
    "X-Windows bitmap ( *.xbm )",
    };

static BitmapFileType	bmWmfFile=
    {
    bmWriteWmfFile,
    bmCanWriteWmfFile,
    NULL, /* bmReadWmfFile, */
    "wmf",
    "*.wmf",
    "application/x-msmetafile",
    "Windows Meta File ( *.wmf )",
    };

#   if USE_LIBPNG

static BitmapFileType	bmPngFile=
    {
    bmWritePngFile,
    bmCanWritePngFile,
    bmReadPngFile,
    "png",
    "*.png",
    "image/png",
    "Portable Network Graphics ( *.png )",
    };

#   endif /* USE_LIBPNG */

#   if USE_LIBXPM

static BitmapFileType	bmXpmFile=
    {
    bmWriteXpmFile,
    bmCanWriteXpmFile,
    bmReadXpmFile,
    "xpm",
    "*.xpm",
    "image/x-xpixmap",
    "X11 Pixmap ( *.xpm )",
    };
#   endif /* USE_LIBXPM */

static BitmapFileType	bmPgmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "pgm",
    "*.pgm",
    "image/x-portable-graymap",
    "Portable Gray Map ( *.pgm )",
    };

static BitmapFileType	bmPbmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "pbm",
    "*.pbm",
    "image/x-portable-bitmap",
    "Portable Bitmap ( *.pbm )",
    };

static BitmapFileType	bmPpmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "ppm",
    "*.ppm",
    "image/x-portable-pixmap",
    "Portable Pixmap ( *.ppm )",
    };

static BitmapFileType	bmPnmFile=
    {
    NULL,
    NULL,
    bmReadPbmFile,
    "pnm",
    "*.pnm",
    "image/x-portable-anymap",
    "Portable Anymap ( *.pnm )",
    };

static BitmapFileType	bmWbmpFile=
    {
    bmWriteWbmpFile,
    bmCanWriteWbmpFile,
    bmReadWbmpFile,
    "wbmp",
    "*.wbmp",
    "image/vnd.wap.wbmp",
    "WAP Wireless Bitmap Format ( *.wbmp )",
    };

#   if USE_LIBWEBP

static BitmapFileType	bmWebpFile=
    {
    bmWriteWebpFile,
    bmCanWriteWebpFile,
    bmReadWebpFile,
    "webp",
    "*.webp",
    "image/webp",
    "Google WEBP ( *.webp )",
    };

#   endif

BitmapFileType * bmFileTypes[]=
    {
#   if USE_LIBPNG
    &bmPngFile,
#   endif
#   if USE_LIBJPEG
    &bmJpegFile,
    &bmJpgFile,
#   endif
    &bmBmpFile,
    &bmGifFile,
    &bmXwdFile,
    &bmEpsFile,
    &bmPdfFile,
    &bmRtfFile,
    &bmXbmFile,
    &bmIcoFile,
#   if USE_LIBTIFF
    &bmTiffFile,
    &bmTifFile,
#   endif
#   if USE_LIBWEBP
    &bmWebpFile,
#   endif
#   if USE_LIBXPM
    &bmXpmFile,
#   endif
    &bmPgmFile,
    &bmPbmFile,
    &bmPpmFile,
    &bmPnmFile,
    &bmWbmpFile,
    &bmWmfFile,
    };

BitmapFileFormat	bmFileFormats[]=
    {
	{ "PNG  Portable Network Graphics", "png1File",
			    0,				&bmPngFile },
	{ "JPG  Independent JPEG Group (.jpg)", "jpg1File",
			    1,				&bmJpgFile },
	{ "JPEG  Independent JPEG Group (.jpeg)", "jpeg1File",
			    1,				&bmJpegFile },
	{ "BMP  Microsoft Windows 3.x bitmap", "bmp3File",
			    40,				&bmBmpFile },
	{ "GIF  Compuserve GIF", "gif87File",
			    87,				&bmGifFile },
	{ "XWD  X/11 Window dump", "xwd11File",
			    11,				&bmXwdFile },
	{ "XWD  X/10 Window dump", "xwd10File",
			    10,				&bmXwdFile },
	{ "EPS  Encapsulated Postscript", "eps1File",
			    1,				&bmEpsFile },
	{ "EPS  Encapsulated Postscript (Level 2)", "eps2File",
			    2,				&bmEpsFile },
	{ "PDF  Acrobat Portable Document Format", "pdf2File",
			    12,				&bmPdfFile },
	{ "XBM  X/11 Bitmap File", "xbm11File",
			    11,				&bmXbmFile },
	{ "ICO  Microsoft 3.x Windows Icon", "ico3File",
			    40,				&bmIcoFile },
#	if USE_LIBTIFF
	{ "TIFF  No compression", "tiffPlainFile",
			    COMPRESSION_NONE,		&bmTiffFile },
	{ "TIFF  CCITT run length", "tiffCcittRleFile",
			    COMPRESSION_CCITTRLE,	&bmTiffFile },
	{ "TIFF  Fax 3 format", "tiffFax3File",
			    COMPRESSION_CCITTFAX3,	&bmTiffFile },
	{ "TIFF  Fax 4 format", "tiffFax4File",
			    COMPRESSION_CCITTFAX4,	&bmTiffFile },
	{ "TIFF  Lempel-Ziv & Welch", "tiffLzwFile",
			    COMPRESSION_LZW,		&bmTiffFile },
	{ "TIFF  NeXT 2-bit RLE", "tiffNextFile",
			    COMPRESSION_NEXT,		&bmTiffFile },
	{ "TIFF  Run length, aligned", "tiffRleFile",
			    COMPRESSION_CCITTRLEW,	&bmTiffFile },
	{ "TIFF  Packbits", "tiffPackbitsFile",
			    COMPRESSION_PACKBITS,	&bmTiffFile },
	{ "TIFF  Thunderscan", "tiffThunderFile",
			    COMPRESSION_THUNDERSCAN,	&bmTiffFile },
	{ "TIFF  JPEG", "tiffJpegFile",
			    COMPRESSION_JPEG,		&bmTiffFile },
	{ "TIFF  Deflate", "tiffDeflateFile",
			    COMPRESSION_DEFLATE,	&bmTiffFile },
	{ "TIFF  Adobe Deflate", "tiffAdobeDeflateFile",
			    COMPRESSION_ADOBE_DEFLATE,	&bmTiffFile },
#	endif /* USE_LIBTIFF */

#	if USE_LIBXPM
	{ "XPM  X11 Pixmap File", "xpm1File",
			    0,				&bmXpmFile },
#	endif /* USE_XPM */

#	if USE_LIBWEBP
	{ "WEBP  Lossy", "webpLossyFile",
			    1,				&bmWebpFile },
	{ "WEBP  Lossless", "webpLosslessFile",
			    2,				&bmWebpFile },
#	endif

	{ "PGM  Gray Map (2:Text)", "pgm2File",
			    2,				&bmPgmFile },
	{ "PGM  Gray Map (5:Raw)", "pgm5File",
			    5,				&bmPgmFile },
	{ "PBM  Bitmap (1:Text)", "pgm1File",
			    1,				&bmPbmFile },
	{ "PBM  Bitmap (4:Raw)", "pgm4File",
			    4,				&bmPbmFile },
	{ "PPM  Pixmap (3:Text)", "pgm3File",
			    3,				&bmPbmFile },
	{ "PPM  Pixmap (6:Raw)", "pgm6File",
			    6,				&bmPbmFile },
	{ "WBMP Wireless Bitmap", "wbmp0File",
			    0,				&bmWbmpFile },
	{ "WMF  Windows Meta File", "wmfFile",
			    0,				&bmWmfFile },
	{ "RTF  Rich Text Format (\\pngblip)", "rtfPngFile",
			    0,				&bmRtfFile },
	{ "RTF  Rich Text Format (\\jpegblip)", "rtfJpegFile",
			    1,				&bmRtfFile },
	{ "RTF  Rich Text Format (\\wmetafile)", "rtfWmfFile",
			    2,				&bmRtfFile },
    };

const int bmNumberOfFileFormats= sizeof(bmFileFormats)/sizeof(BitmapFileFormat);
const int bmNumberOfFileTypes= sizeof(bmFileTypes)/sizeof(BitmapFileType *);


static int bmTestWrite(		const BitmapDescription *	bd,
				const struct MemoryBuffer *	ext,
				const BitmapFileFormat *	bff )
    {
    const BitmapFileType *	bft= bff->bffFileType;

    if  ( ! utilMemoryBufferIsEmpty( ext ) &&
	  strcmp( bft->bftFileExtension, utilMemoryBufferGetString( ext ) ) )
	{ return -1;	}

    if  ( ! bft->bftWrite )
	{ return -1;	}
    if  ( (*bft->bftTestCanWrite) ( bd, bff->bffPrivate ) )
	{ return -1;	}

    return 0;
    }

int bmSuggestFormat(	const MemoryBuffer *		filename,
			int				suggestedFormat,
			const BitmapDescription *	bd )
    {
    int				format= suggestedFormat;

    MemoryBuffer		ext;

    if  ( format >= bmNumberOfFileFormats )
	{ LLDEB(format,bmNumberOfFileFormats); format= -1; goto ready;	}

    utilInitMemoryBuffer( &ext );

    if  ( fileGetFileExtension( &ext, filename ) )
	{ LDEB(1); format= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &ext )		||
	  format < 0					||
	  bmTestWrite( bd, &ext, bmFileFormats+ format ) )
	{
	int				i;
	const BitmapFileFormat *	bff= bmFileFormats;

	for ( i= 0; i < bmNumberOfFileFormats; bff++, i++ )
	    {
	    if  ( ! bmTestWrite( bd, &ext, bff ) )
		{ format= i; break; }
	    }

	if  ( i == bmNumberOfFileFormats )
	    { SLLDEB(utilMemoryBufferGetString(&ext),i,bmNumberOfFileFormats); }
	}

  ready:

    utilCleanMemoryBuffer( &ext );

    return format;
    }

int bmCanSaveAsContentType(	const BitmapDescription *	bd,
				const char *			contentType )
    {
    int				i;
    const BitmapFileFormat *	bff= bmFileFormats;

    for ( i= 0; i < bmNumberOfFileFormats; bff++, i++ )
	{
	const BitmapFileType *	bft= bff->bffFileType;

	if  ( ! bft->bftWrite )
	    { continue;	}

	if  ( strcmp( bft->bftContentType, contentType ) )
	    { continue;	}

	if  ( ! (*bft->bftTestCanWrite) ( bd, bff->bffPrivate ) )
	    { return 1;	}
	}

    return 0;
    }

const char * bmGetContentTypeOfFormat(	int			fileFormat )
    {
    const BitmapFileFormat *	bff;

    if  ( fileFormat < 0 || fileFormat >= bmNumberOfFileFormats )
	{ LLDEB(fileFormat,bmNumberOfFileFormats); return (const char *)0; }

    bff= bmFileFormats+ fileFormat;

    return bff->bffFileType->bftContentType;
    }
