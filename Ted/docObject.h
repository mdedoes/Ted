/************************************************************************/
/*									*/
/*  Text Buffer: Obects inserted in the text.				*/
/*  Objects are independent entities that are embedded in the text.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_H
#   define	DOC_OBJECT_H

#   include	<utilMemoryBuffer.h>

struct BufferDocument;
struct BufferItem;
struct TextParticule;

/************************************************************************/
/*									*/
/*  Kinds of object.							*/
/*									*/
/************************************************************************/

typedef enum ObjectKind
    {
    DOCokUNKNOWN= 0,

    DOCokPICTWMETAFILE,
    DOCokPICTPNGBLIP,
    DOCokPICTJPEGBLIP,
    DOCokPICTEMFBLIP,
    DOCokMACPICT,
    DOCokPMMETAFILE,
    DOCokDIBITMAP,
    DOCokWBITMAP,
    DOCokOLEOBJECT,

    DOCokINCLUDEPICTURE,
    DOCokEPS_FILE,
    DOCokBITMAP_FILE,
	    /************************************************************/
	    /*  To include pictures. In Particular EPS pictures. As	*/
	    /*  this is different from what 'Word' does when you	*/
	    /*  include a picture: they are not	saved in the RTF file.	*/
	    /************************************************************/

    DOCok__COUNT
    } ObjectKind;

typedef struct InsertedObject
    {
    int			ioKind;		/*  Kind of object.		*/
    int			ioResultKind;	/*  Kind of object.		*/
    int			ioTwipsWide;	/*  Width of object.		*/
    int			ioTwipsHigh;	/*  Height of object.		*/
    int			ioScaleX;	/*  In %.			*/
    int			ioScaleY;	/*  In %.			*/
    int			ioPixelsWide;	/*  Width of object on screen	*/
    int			ioPixelsHigh;	/*  Height of object on screen	*/
    int			io_xWinExt;	/*  Of metafile picture.	*/
    int			io_yWinExt;	/*  Of metafile picture.	*/

    int			ioTopCropTwips;
    int			ioBottomCropTwips;
    int			ioLeftCropTwips;
    int			ioRightCropTwips;

    int			ioMetafileIsBitmap;
    int			ioMetafileBitmapBpp;

    int			ioBmBitsPerPixel;
    int			ioBmPlanes;
    int			ioBmBytessPerRow;

    int			ioDragWide;	/*  PixelsWide during resize.	*/
    int			ioDragHigh;	/*  PixelsHigh during resize.	*/

    int			ioMapMode;
    int			ioResultMapMode;
					/*  Used for metafile pictures	*/
					/*  or objects with a result	*/
					/*  that is a metafile picture.	*/
    MemoryBuffer	ioObjectData;
    MemoryBuffer	ioResultData;

    unsigned char *	ioObjectName;
    unsigned char *	ioObjectClass;
    int			ioBliptag;

#   ifdef USE_MOTIF
    unsigned long	ioPixmap;
#   endif

#   ifdef USE_GTK
    void *		ioPixmap;
#   endif

    void *		ioPrivate;
    } InsertedObject;

typedef enum InsertedObjectProperty
    {
    IOpropKIND= 0,
    IOpropRESULT_KIND,

    IOpropOBJTWIPS_WIDE,
    IOpropOBJTWIPS_HIGH,

    IOpropPICTWIPS_WIDE,
    IOpropPICTWIPS_HIGH,

    IOpropOBJSCALE_X,
    IOpropOBJSCALE_Y,

    IOpropPICSCALE_X,
    IOpropPICSCALE_Y,

    IOpropPICX_WIN_EXT,
    IOpropPICY_WIN_EXT,

    IOpropOBJCROP_TOP,
    IOpropOBJCROP_BOTTOM,
    IOpropOBJCROP_LEFT,
    IOpropOBJCROP_RIGHT,

    IOpropPICCROP_TOP,
    IOpropPICCROP_BOTTOM,
    IOpropPICCROP_LEFT,
    IOpropPICCROP_RIGHT,

    IOpropWBMBITSPIXEL,
    IOpropWBMPLANES,
    IOpropWBMWIDTHBYTES,

    IOpropBLIPTAG,

    IOpropPICBMP,
    IOpropPICBPP,

    IOprop_UNSUPPORTED,

    IOprop_COUNT
    } InsertedObjectProperty;

typedef void (*DOC_CLOSE_OBJECT)(	struct BufferDocument *	bd,
					struct BufferItem *	bi,
					struct TextParticule *	tp,
					void *			voidadd );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docGetBitmapForObject(	InsertedObject *	io );

extern int docObjectSetData(	InsertedObject *	io,
				const unsigned char *	bytes,
				int			size );

extern int docSetResultData(	InsertedObject *	io,
				const unsigned char *	bytes,
				int			size );

extern int docSaveObjectTag(	InsertedObject *	io,
				const char *		tag,
				int			arg );

extern int docSaveResultTag(	InsertedObject *	io,
				const char *		tag,
				int			arg );

extern int docSetObjectName(	InsertedObject *	io,
				const unsigned char *	name,
				int			len );

extern int docSetObjectClass(	InsertedObject *	io,
				const unsigned char *	name,
				int			len );

extern void docCleanObject(	InsertedObject *	io );

#   endif	/*  DOC_OBJECT_H	*/
