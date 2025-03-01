/************************************************************************/
/*									*/
/*  Description of raster image file formats.				*/
/*									*/
/************************************************************************/

#   ifndef	BM_FORMAT_H
#   define	BM_FORMAT_H

struct MemoryBuffer;
struct BitmapDescription;

/************************************************************************/
/*									*/
/*  Description of a bitmap file format.				*/
/*									*/
/************************************************************************/

typedef struct	BitmapFileType
    {
			/**
			 *  Write the image to file.
			 */
    int (*bftWrite)(	const struct MemoryBuffer *	filename,
			const unsigned char *		buffer,
			const struct BitmapDescription * bd,
			int				privateFormat );

			/**
			 *  Test whether the image can be written in this 
			 *  format.
			 */
    int (*bftTestCanWrite)( const struct BitmapDescription * bd,
			int				privateFormat );

			/**
			 *  Read an image from file.
			 */
    int (*bftRead)(	const struct MemoryBuffer *	filename,
			unsigned char **		pBuffer,
			struct BitmapDescription *	bd,
			int *				pPrivateFormat );

			    /**
			     *  The traditional file extension for this 
			     *  type of image.
			     */
    const char *	bftFileExtension;
    const char *	bftFileFilter;

			    /**
			     *  IETF style content type (Mime type)
			     */
    const char *	bftContentType;
    const char *	bftTypeDescription;
    } BitmapFileType;

typedef struct	BitmapFileFormat
    {
    const char *	bffDescription;
    const char *	bffId;
    int			bffPrivate;
    BitmapFileType *	bffFileType;
    } BitmapFileFormat;

/************************************************************************/
/*									*/
/*  Catalog of available raster file formats.				*/
/*									*/
/************************************************************************/

extern BitmapFileFormat	bmFileFormats[];
extern const int	bmNumberOfFileFormats;

extern BitmapFileType *	bmFileTypes[];
extern const int	bmNumberOfFileTypes;

#   endif
