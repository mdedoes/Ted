/************************************************************************/
/*									*/
/*  Save in image in the document.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<docObjectProperties.h>
#   include	<geoUnits.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	"docRtfTags.h"
#   include	<docPictureProperties.h>
#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write the tags that describe the geometry of a picture object.	*/
/*									*/
/************************************************************************/

static const int PictRegularProperties[]=
{
    PIPpropPICSCALE_X,
    PIPpropPICSCALE_Y,
    PIPpropPICTWIPS_WIDE,
    PIPpropPICTWIPS_HIGH,
    PIPpropPICCROP_TOP,
    PIPpropPICCROP_BOTTOM,
    PIPpropPICCROP_LEFT,
    PIPpropPICCROP_RIGHT,
    PIPpropBLIPTAG,
    PIPpropWBMBITSPIXEL,
    PIPpropWBMPLANES,
    PIPpropWBMWIDTHBYTES,
    PIPpropDEFSHP,
    PIPpropPICSCALED,
    PIPpropPICBMP,
    PIPpropBLIPUPI,
};

static const int PictRegularPropertyCount=
			sizeof(PictRegularProperties)/sizeof(int);

int docRtfSavePictureTags(	RtfWriter *			rw,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    const int			scope= RTCscopePICT;

    if  ( PROPmaskISSET( pipSetMask, PIPpropTYPE ) )
	{
	switch( pipSet->pipType )
	    {
	    case DOCokPICTWMETAFILE:
		docRtfWriteArgTag( rw, "wmetafile", pipSet->pipMapMode );

		if  ( pipSet->pipMetafileIsRaster )
		    {
		    if  ( pipSet->pipMetafileBitmapBpp > 0 )
			{
			docRtfWriteArgTag( rw, "picbpp",
					    pipSet->pipMetafileBitmapBpp );
			}
		    }
		break;

	    case DOCokMACPICT:
		docRtfWriteTag( rw, "macpict" );
		break;
	    case DOCokPICTPNGBLIP:
		docRtfWriteTag( rw, "pngblip" );
		break;
	    case DOCokPICTJPEGBLIP:
		docRtfWriteTag( rw, "jpegblip" );
		break;
	    case DOCokPICTEMFBLIP:
		docRtfWriteTag( rw, "emfblip" );
		break;
	    case DOCokDIBITMAP:
		docRtfWriteTag( rw, "dibitmap" );
		break;
	    case DOCokWBITMAP:
		docRtfWriteTag( rw, "wbitmap" );
		break;
	    case DOCokPMMETAFILE:
		docRtfWriteTag( rw, "pmmetafile" );
		break;

	    default:
		LDEB(pipSet->pipType);
		break;
	    }
	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICX_WIN_EXT ) )
	{
	int	xExt= pipSet->pip_xWinExt;

	if  ( xExt == 0 )
	    {
	    xExt= (int)( 100000.0* pipSet->pipTwipsWide )/
						    ( 20* POINTS_PER_M );
	    }

	docRtfWriteArgTag( rw, "picw", xExt );
	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICY_WIN_EXT ) )
	{
	int	yExt= pipSet->pip_yWinExt;

	if  ( yExt == 0 )
	    {
	    yExt= (int)( 100000.0* pipSet->pipTwipsHigh )/
						    ( 20* POINTS_PER_M );
	    }

	docRtfWriteArgTag( rw, "pich", yExt );
	}

    if  ( docRtfWriteItemProperties( rw, scope, pipSet,
			(RtfGetIntProperty)docGetPictureProperty, pipSetMask,
			PictRegularProperties, PictRegularPropertyCount ) )
	{ LLDEB(scope,PictRegularPropertyCount); return -1;	}

    return 0;
    }

static int docRtfPicturePropertyMask(
				PropertyMask *			pipSetMask,
				const PictureProperties *	pipSet )
    {
    int			rval= 0;
    PictureProperties	pipDef;
    PropertyMask	pipAllMask;

    utilPropMaskClear( pipSetMask );
    utilPropMaskClear( &pipAllMask );
    utilPropMaskFill( &pipAllMask, PIPprop_COUNT );

    docInitPictureProperties( &pipDef );

    if  ( docUpdPictureProperties( pipSetMask, &pipDef, &pipAllMask, pipSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    /*docCleanPictureProperties( &pipDef );*/

    return rval;
    }

int docRtfSavePicture(		RtfWriter *			rw,
				const PictureProperties *	pip,
				const struct MemoryBuffer *	altText,
				const struct MemoryBuffer *	pictureData )
    {
    PropertyMask	pipSetMask;

    docRtfWriteDestinationBegin( rw, RTFtag_pict );

    docRtfPicturePropertyMask( &pipSetMask, pip );
    docRtfSavePictureTags( rw, &pipSetMask, pip );

    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	docRtfWriteDestinationBegin( rw, "*\\picprop" );
	docRtfSaveShapeString( rw, "wzDescription", altText );
	docRtfWriteDestinationEnd( rw );
	}

    docRtfWriteMemoryBuffer( rw, pictureData );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

