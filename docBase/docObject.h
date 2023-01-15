/************************************************************************/
/*									*/
/*  Text Buffer: Obects inserted in the text.				*/
/*  Objects are independent entities that are embedded in the text.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_H
#   define	DOC_OBJECT_H

#   include	<utilMemoryBuffer.h>

#   include	<bitmap.h>
#   include	"docLayoutPosition.h"
#   include	"docPictureProperties.h"

struct DrawingSurface;
struct DrawingShape;

/************************************************************************/
/*									*/
/*  Kinds of object.							*/
/*									*/
/************************************************************************/

typedef struct InsertedObject
    {
			/**
			 *  Kind of object
			 *  enum ObjectKind: DOCokSOMETHING values
			 */
    int			ioKind;

			/**
			 *  Kind of result object
			 *  Used where the (OLE) object is stored as an 
			 *  image or a piece of text.
			 *  enum ObjectKind: DOCokSOMETHING values
			 */
    int			ioResultKind;

			/**
			 *  Width of the object: objw in rtf
			 *  NOTE: this is a property of the object. Not of 
			 *  the way in which it is included. That is 
			 *  controlled by the scale.
			 */
    int			ioTwipsWide;

			/**
			 *  Height of the object: objh in rtf
			 *  NOTE: this is a property of the object. Not of 
			 *  the way in which it is included. That is 
			 *  controlled by the scale.
			 */
    int			ioTwipsHigh;

			/**
			 *  Horizontal scale of the object as it is set 
			 *  by the user, or it is read from the input 
			 *  file. (In %)
			 */
    short int		ioScaleXSet;

			/**
			 *  Vertical scale of the object as it is set 
			 *  by the user, or it is read from the input 
			 *  file. (In %)
			 */
    short int		ioScaleYSet;

			/**
			 *  Horizontal scale of the object as it is used.
			 *  Objects are scaled to fit on the page or in 
			 *  a table cell. (In %)
			 *  (So used <= set)
			 */
    short int		ioScaleXUsed;

			/**
			 *  Vertical scale of the object as it is used.
			 *  Objects are scaled to fit on the page or in 
			 *  a table cell. (In %)
			 *  (So used <= set)
			 */
    short int		ioScaleYUsed;
#if 1
    int			ioPixelsWide;	/*  Width of object on screen	*/
    int			ioPixelsHigh;	/*  Height of object on screen	*/
#endif

    PictureProperties	ioPictureProperties;

    LayoutPosition	ioY0Position;
    int			ioX0Twips;

    unsigned char	ioRtfResultKind; /*  From rslt* tags.		*/
    unsigned char	ioRtfEmbedKind;  /*  From objemb.. tags.	*/

    unsigned char	ioInline;	/*  Part of the text flow?	*/

    int			ioTopCropTwips;
    int			ioBottomCropTwips;
    int			ioLeftCropTwips;
    int			ioRightCropTwips;

    MemoryBuffer	ioObjectData;
    MemoryBuffer	ioResultData;
    MemoryBuffer	ioObjectName;
    MemoryBuffer	ioObjectClass;

				/**
				 *  The alternative text for HTML and 
				 *  structured PDF production.
				 */
    MemoryBuffer	ioAltText;

    struct DrawingShape *	ioDrawingShape;

    struct DrawingSurface *	ioDrawingSurface;
    RasterImage			ioRasterImage;
    } InsertedObject;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitInsertedObject(	InsertedObject *		io );
extern void docCleanInsertedObject(	InsertedObject *		io );

extern int docObjectSetData(	InsertedObject *		io,
				int				prop,
				const char *			bytes,
				int				size );

extern int docSaveObjectTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern int docSaveResultTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern void docObjectAdaptToPictureGeometry(
				InsertedObject *		io,
				const PictureProperties *	pip );

extern void docObjectGetCropRect(
				struct DocumentRectangle *	dr,
				const PictureProperties *	pip,
				const BitmapDescription *	bd );

extern void docObjectGetPixelRectangle(
				struct DocumentRectangle *	drPixels,
				const InsertedObject *		io,
				double				pixelsPerTwip );

extern void docObjectSetPixelSize(
				InsertedObject *		io,
				double				pixelsPerTwip );

extern int docSetObjectProperty(	InsertedObject *	io,
					int			prop,
					int			value );

extern int docGetObjectProperty(	const InsertedObject *	io,
					int			prop );

#   endif	/*  DOC_OBJECT_H	*/
