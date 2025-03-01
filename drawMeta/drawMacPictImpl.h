/************************************************************************/
/*									*/
/*  Display Mac Pict images.						*/
/*									*/
/************************************************************************/

#   include	<geoRectangle.h>

struct SimpleInputStream;
struct MacPictHeader;

/************************************************************************/
/*									*/
/*  Information that can be extracted from the file header.		*/
/*									*/
/************************************************************************/

typedef struct MacPictHeader
    {
    int			mphPictureSize;
    int			mphVersion;
    DocumentRectangle	mphFrame;
    } MacPictHeader;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMacPictGetDeviceHeader(	struct MacPictHeader *		mph,
					struct SimpleInputStream *	sis );
