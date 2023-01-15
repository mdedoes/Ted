/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<geoGrid.h>
#   include	<geoRectangle.h>

#   include	"docObject.h"
#   include	"docObjectProperties.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Manage inserted objects.						*/
/*									*/
/************************************************************************/

void docCleanInsertedObject( InsertedObject * io )
    {
    utilCleanMemoryBuffer( &io->ioObjectData );
    utilCleanMemoryBuffer( &io->ioResultData );
    utilCleanMemoryBuffer( &io->ioObjectName );
    utilCleanMemoryBuffer( &io->ioObjectClass );

    utilCleanMemoryBuffer( &io->ioAltText );

    bmCleanRasterImage( &(io->ioRasterImage) );
    }

void docInitInsertedObject(	InsertedObject *	io )
    {
    io->ioKind= DOCokUNKNOWN;
    io->ioResultKind= DOCokUNKNOWN;
    io->ioRtfResultKind= RESULTkindUNKNOWN;
    io->ioRtfEmbedKind= EMBEDkindOBJEMB;

    docInitPictureProperties( &(io->ioPictureProperties) );

    io->ioInline= 1;

    io->ioTwipsWide= 0;
    io->ioTwipsHigh= 0;

    io->ioPixelsWide= 0;
    io->ioPixelsHigh= 0;
    io->ioScaleXSet= 100;
    io->ioScaleYSet= 100;
    io->ioScaleXUsed= 100;
    io->ioScaleYUsed= 100;

    io->ioX0Twips= 0;
    docInitLayoutPosition( &(io->ioY0Position) );

    io->ioTopCropTwips= 0;
    io->ioBottomCropTwips= 0;
    io->ioLeftCropTwips= 0;
    io->ioRightCropTwips= 0;

    utilInitMemoryBuffer( &io->ioObjectData );
    utilInitMemoryBuffer( &io->ioResultData );
    utilInitMemoryBuffer( &io->ioObjectName );
    utilInitMemoryBuffer( &io->ioObjectClass );

    utilInitMemoryBuffer( &io->ioAltText );
    
    io->ioDrawingShape= (struct DrawingShape *)0;

    io->ioDrawingSurface= (struct DrawingSurface *)0;
    bmInitRasterImage( &(io->ioRasterImage) );

    return;
    }

void docObjectAdaptToPictureGeometry(	InsertedObject *		io,
					const PictureProperties *	pip )
    {
    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    io->ioScaleXSet= pip->pipScaleXSet;
    io->ioScaleYSet= pip->pipScaleYSet;
    io->ioScaleXUsed= pip->pipScaleXUsed;
    io->ioScaleYUsed= pip->pipScaleYUsed;

    io->ioTopCropTwips= pip->pipTopCropTwips;
    io->ioBottomCropTwips= pip->pipBottomCropTwips;
    io->ioLeftCropTwips= pip->pipLeftCropTwips;
    io->ioRightCropTwips= pip->pipRightCropTwips;

    return;
    }

int docObjectSetData(	InsertedObject *	io,
			int			prop,
			const char *		bytes,
			int			size )
    {
    switch( prop )
	{
	case IOprop_OBJDATA:
	    return utilMemoryBufferSetBytes( &(io->ioObjectData),
				    (const unsigned char *)bytes, size );

	case IOprop_OBJNAME:
	    return utilMemoryBufferSetBytes( &(io->ioObjectName),
				    (const unsigned char *)bytes, size );

	case IOprop_OBJCLASS:
	    return utilMemoryBufferSetBytes( &(io->ioObjectClass),
				    (const unsigned char *)bytes, size );

	case IOprop_RESULT:
	    return utilMemoryBufferSetBytes( &(io->ioResultData),
				    (const unsigned char *)bytes, size );

	default:
	    LDEB(prop); return -1;
	}

    }

/************************************************************************/
/*									*/
/*  Find the pixel rectangle inside the cropping margins of an image.	*/
/*									*/
/************************************************************************/

void docObjectGetCropRect(	DocumentRectangle *		drSrc,
				const PictureProperties *	pip,
				const BitmapDescription *	bd )
    {
    int		c1Twips;

    drSrc->drX0= 0;
    drSrc->drY0= 0;
    drSrc->drX1= bd->bdPixelsWide- 1;
    drSrc->drY1= bd->bdPixelsHigh- 1;

    if  ( pip->pipLeftCropTwips+ pip->pipRightCropTwips > 0 )
	{
	drSrc->drX0= ( bd->bdPixelsWide* pip->pipLeftCropTwips ) /
							    pip->pipTwipsWide;
	c1Twips= pip->pipTwipsWide- pip->pipRightCropTwips;
	drSrc->drX1= ( bd->bdPixelsWide* c1Twips )/ pip->pipTwipsWide;

	if  ( drSrc->drX0 < 0 )
	    { drSrc->drX0=  0;	}
	if  ( drSrc->drX1 > bd->bdPixelsWide- 1 )
	    { drSrc->drX1=  bd->bdPixelsWide- 1;	}
	}

    if  ( pip->pipTopCropTwips+ pip->pipBottomCropTwips > 0 )
	{
	drSrc->drY0= ( bd->bdPixelsHigh* pip->pipTopCropTwips ) /
							    pip->pipTwipsHigh;
	c1Twips= pip->pipTwipsHigh- pip->pipBottomCropTwips;
	drSrc->drY1= ( bd->bdPixelsHigh* c1Twips )/ pip->pipTwipsHigh;

	if  ( drSrc->drY0 < 0 )
	    { drSrc->drY0=  0;	}
	if  ( drSrc->drY1 > bd->bdPixelsHigh- 1 )
	    { drSrc->drY1=  bd->bdPixelsHigh- 1;	}
	}

    return;
    }

void docObjectSetPixelSize(	InsertedObject *		io,
				double				pixelsPerTwip )
    {
    io->ioPixelsWide= COORDtoGRID( pixelsPerTwip,
				( io->ioScaleXUsed* io->ioTwipsWide )/ 100 );
    io->ioPixelsHigh= COORDtoGRID( pixelsPerTwip,
				( io->ioScaleYUsed* io->ioTwipsHigh )/ 100 );

    if  ( io->ioPixelsWide < 1 )
	{ io->ioPixelsWide=  1;	}
    if  ( io->ioPixelsHigh < 1 )
	{ io->ioPixelsHigh=  1;	}

    return;
    }

int docSetObjectProperty(	InsertedObject *	io,
				int			prop,
				int			value )
    {
    switch( prop )
	{
	case IOpropRESULT_KIND:
	    io->ioRtfResultKind= value;
	    break;

	case IOpropEMBED_KIND:
	    io->ioRtfEmbedKind= value;
	    break;

	case IOpropOBJTWIPS_WIDE:
	    io->ioTwipsWide= value;
	    break;
	case IOpropOBJTWIPS_HIGH:
	    io->ioTwipsHigh= value;
	    break;

	case IOpropOBJSCALE_X:
	    io->ioScaleXSet= value;
	    break;
	case IOpropOBJSCALE_Y:
	    io->ioScaleYSet= value;
	    break;

	case IOpropOBJCROP_TOP:
	    io->ioTopCropTwips= value;
	    break;
	case IOpropOBJCROP_BOTTOM:
	    io->ioBottomCropTwips= value;
	    break;
	case IOpropOBJCROP_LEFT:
	    io->ioLeftCropTwips= value;
	    break;
	case IOpropOBJCROP_RIGHT:
	    io->ioRightCropTwips= value;
	    break;

	default:
	    LLDEB(prop,value);
	    return -1;
	}

    return 0;
    }

int docGetObjectProperty(	const InsertedObject *	io,
				int			prop )
    {
    switch( prop )
	{
	case IOpropRESULT_KIND:
	    return io->ioRtfResultKind;

	case IOpropEMBED_KIND:
	    return io->ioRtfEmbedKind;

	case IOpropOBJTWIPS_WIDE:
	    return io->ioTwipsWide;
	case IOpropOBJTWIPS_HIGH:
	    return io->ioTwipsHigh;

	case IOpropOBJSCALE_X:
	    return io->ioScaleXSet;
	case IOpropOBJSCALE_Y:
	    return io->ioScaleYSet;

	case IOpropOBJCROP_TOP:
	    return io->ioTopCropTwips;
	case IOpropOBJCROP_BOTTOM:
	    return io->ioBottomCropTwips;
	case IOpropOBJCROP_LEFT:
	    return io->ioLeftCropTwips;
	case IOpropOBJCROP_RIGHT:
	    return io->ioRightCropTwips;

	default:
	    LDEB(prop);
	    return -1;
	}
    }

