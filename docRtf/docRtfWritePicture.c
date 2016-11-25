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
#   include	"docRtfTags.h"
#   include	<docPictureProperties.h>
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write the tags that describe the geometry of a picture object.	*/
/*									*/
/************************************************************************/

int docRtfSavePictureTags(	RtfWriter *			rw,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
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

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALE_X ) )
	{ docRtfWriteArgTag( rw, "picscalex", pipSet->pipScaleXSet ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALE_Y ) )
	{ docRtfWriteArgTag( rw, "picscaley", pipSet->pipScaleYSet ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICTWIPS_WIDE ) )
	{ docRtfWriteArgTag( rw, "picwgoal", pipSet->pipTwipsWide );	}
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICTWIPS_HIGH ) )
	{ docRtfWriteArgTag( rw, "pichgoal", pipSet->pipTwipsHigh );	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_TOP ) )
	{ docRtfWriteArgTag( rw, "piccropt", pipSet->pipTopCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_BOTTOM ) )
	{ docRtfWriteArgTag( rw, "piccropb", pipSet->pipBottomCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_LEFT ) )
	{ docRtfWriteArgTag( rw, "piccropl", pipSet->pipLeftCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_RIGHT ) )
	{ docRtfWriteArgTag( rw, "piccropr", pipSet->pipRightCropTwips ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropBLIPTAG ) )
	{ docRtfWriteArgTag( rw, "bliptag", pipSet->pipBliptag ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMBITSPIXEL ) )
	{ docRtfWriteArgTag( rw, "wbmbitspixel", pipSet->pipBmBitsPerPixel ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMPLANES ) )
	{ docRtfWriteArgTag( rw, "wbmplanes", pipSet->pipBmPlanes );	}
    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMWIDTHBYTES ) )
	{ docRtfWriteArgTag( rw, "wbmwidthbytes", pipSet->pipBmBytesPerRow ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropDEFSHP ) )
	{ docRtfWriteFlagTag( rw, "defshp", pipSet->pipPictureIsWordArt ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALED ) )
	{ docRtfWriteFlagTag( rw, "picscaled", pipSet->pipPictIsScaled ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICBMP ) )
	{ docRtfWriteFlagTag( rw, "picbmp", pipSet->pipMetafileIsRaster ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropBLIPUPI ) )
	{ docRtfWriteArgTag( rw, "blipupi", pipSet->pipBmUnitsPerInch ); }

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
				const struct MemoryBuffer *	pictureData )
    {
    PropertyMask	pipSetMask;

    docRtfWriteDestinationBegin( rw, RTFtag_pict );

    docRtfPicturePropertyMask( &pipSetMask, pip );
    docRtfSavePictureTags( rw, &pipSetMask, pip );

    docRtfWriteMemoryBuffer( rw, pictureData );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

