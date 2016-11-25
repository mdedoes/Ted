/************************************************************************/
/*									*/
/*  Management of the io streams for reading the data of objects.	*/
/*									*/
/************************************************************************/

#   ifndef	BM_OBJECT_READER_H
#   define	BM_OBJECT_READER_H

#   include	"bmio.h"

struct SimpleInputStream;
struct MemoryBuffer;
struct RasterImage;

typedef struct ObjectReader
    {
    struct SimpleInputStream *	orSisMem;
    struct SimpleInputStream *	orSisHex;
    } ObjectReader;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void bmInitObjectReader(	struct ObjectReader *		or );
extern void bmCleanObjectReader( struct ObjectReader *		or );

extern int bmOpenObjectReader(	ObjectReader *			or,
				const struct MemoryBuffer *	mb );

extern int bmReadRasterObject(	struct RasterImage *		ri,
				bmReadBitmap			readRaster,
				const char *			suggestedExtension,
				const struct MemoryBuffer *	mb );

#   endif	/*  BM_OBJECT_READER_H	*/
