/************************************************************************/
/*									*/
/*  Read/Write objects.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_IO_H
#   define	DOC_OBJECT_IO_H

struct MemoryBuffer;
struct InsertedObject;
struct RasterImage;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docReadWmfObject(	struct InsertedObject *		io,
				const struct MemoryBuffer *	filename );

extern int docReadWmfSize(	struct InsertedObject *		io,
				const struct MemoryBuffer *	mb );

extern int docReadEmfObject(	struct InsertedObject *		io,
				const struct MemoryBuffer *	filename );

extern int docReadEmfSize(	struct InsertedObject *		io,
				const struct MemoryBuffer *	mb );

extern int docReadRasterObject(	struct InsertedObject *		io,
				const struct MemoryBuffer *	filename );

extern int docReadRasterSize(	struct InsertedObject *		io );

extern int docReadEpsObject(	const struct MemoryBuffer *	fullName,
				struct InsertedObject *		io );

extern int docReadFileObject(	const struct MemoryBuffer *	fullName,
				struct InsertedObject *		io );

extern int docGetRasterImageForObjectData(
				int				kind,
				struct RasterImage *		ri,
				const struct MemoryBuffer *	mb );

extern int docGetRasterImageForObject(	struct InsertedObject *	io );

extern int docSaveRasterBytesToObject(	struct InsertedObject *	io,
					struct RasterImage *	ri );

#   endif	/*  DOC_OBJECT_IO_H	*/
