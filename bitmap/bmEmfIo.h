#   include	"bmEmf.h"
#   include	<utilColor.h>

#   include	<geoAffineTransform.h>

struct SimpleInputStream;
struct SimpleOutputStream;

extern int bmMetaReadEmfHeader(		EmfHeader *		eh,
					struct SimpleInputStream *	sis );

extern int bmMetaWriteEmfHeader(	const EmfHeader *	eh,
					struct SimpleOutputStream *	sos );

extern int bmEmfReadRectangle(		DocumentRectangle *		dr,
					struct SimpleInputStream *		sis );

extern int bmEmfReadXYWHRectangle(	DocumentRectangle *		dr,
					struct SimpleInputStream *		sis );

extern int bmEmfReadRgb8Color(	RGB8Color *			rgb8,
				struct SimpleInputStream *		sis );

extern int bmEmfReadAffineTransform2D(	AffineTransform2D *		at,
					struct SimpleInputStream *		sis );
