#   include	"bmWmf.h"

struct SimpleInputStream;

extern int bmMetaReadWmfHeader(		WmfHeader *		wh,
					struct SimpleInputStream *	sis );

extern int bmWmfReadRectangle(		DocumentRectangle *		dr,
					struct SimpleInputStream *		sis );

extern int bmWmfReadHWYXRectangle(	DocumentRectangle *		dr,
					struct SimpleInputStream *		sis );

extern int bmMetaReadWmfFileHeader(	unsigned long *		pKey,
					WmfFileHeader *		wfh,
					struct SimpleInputStream *	sis );

extern int bmWmfGetColor(		struct SimpleInputStream *	sis,
					RGB8Color *		rgb8 );
