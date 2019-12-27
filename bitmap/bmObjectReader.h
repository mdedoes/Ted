/************************************************************************/
/*									*/
/*  Management of the io streams for reading the data of objects.	*/
/*									*/
/************************************************************************/

#   ifndef	BM_OBJECT_READER_H
#   define	BM_OBJECT_READER_H

#   include	"bmio.h"

struct MemoryBuffer;
struct RasterImage;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bmReadRasterObject(	struct RasterImage *		ri,
				bmReadBitmap			readRaster,
				const char *			suggestedExtension,
				const struct MemoryBuffer *	mb );

#   endif	/*  BM_OBJECT_READER_H	*/
