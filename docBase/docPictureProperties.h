/************************************************************************/
/*									*/
/*  Picture Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PICTURE_PROPERTIES_H
#   define	DOC_PICTURE_PROPERTIES_H

struct PropertyMask;

/************************************************************************/
/*									*/
/*  Properties of a picture.						*/
/*									*/
/************************************************************************/

typedef struct PictureProperties
    {
    unsigned char	pipType;
    signed char		pipMapMode;

    int			pip_xWinExt;	/*  Of metafile picture.	*/
    int			pip_yWinExt;	/*  Of metafile picture.	*/

			/**
			 *  Width of the image or object.
			 *  This is a property of the image. Not of the 
			 *  way in which it is included. That is controlled by 
			 *  the scale.
			 */
    int			pipTwipsWide;

			/**
			 *  Height of the image or object.
			 *  This is a property of the image. Not of the 
			 *  way in which it is included. That is controlled by 
			 *  the scale.
			 */
    int			pipTwipsHigh;

    int			pipScaleXSet;
    int			pipScaleYSet;
    int			pipScaleXUsed;
    int			pipScaleYUsed;

    int			pipTopCropTwips;
    int			pipBottomCropTwips;
    int			pipLeftCropTwips;
    int			pipRightCropTwips;

    unsigned long	pipBliptag;

    int			pipMetafileBitmapBpp;
    int			pipBmUnitsPerInch;
    int			pipBmBitsPerPixel;
    int			pipBmPlanes;
    int			pipBmBytesPerRow;

    unsigned int	pipPictureIsWordArt:1;
    unsigned int	pipMetafileIsRaster:1;
    unsigned int	pipPictIsScaled:1;
    } PictureProperties;

#   define docPictureIsCropped(pip) ( \
		(pip)->pipLeftCropTwips > 0	|| \
		(pip)->pipRightCropTwips > 0	|| \
		(pip)->pipTopCropTwips > 0	|| \
		(pip)->pipBottomCropTwips > 0	)

/************************************************************************/
/*									*/
/*  Numbers of picture properies.					*/
/*									*/
/************************************************************************/

typedef enum PictureProperty
    {
    PIPpropTYPE= 0,

    PIPpropPICX_WIN_EXT,
    PIPpropPICY_WIN_EXT,

    PIPpropPICTWIPS_WIDE,
    PIPpropPICTWIPS_HIGH,

    PIPpropPICSCALE_X,
    PIPpropPICSCALE_Y,

    PIPpropPICCROP_TOP,
    PIPpropPICCROP_BOTTOM,
    PIPpropPICCROP_LEFT,
    PIPpropPICCROP_RIGHT,

    PIPpropBLIPTAG,

    PIPpropPICBPP,
    PIPpropBLIPUPI,
    PIPpropWBMBITSPIXEL,
    PIPpropWBMPLANES,
    PIPpropWBMWIDTHBYTES,

    PIPpropDEFSHP,
    PIPpropPICBMP,
    PIPpropPICSCALED,

    PIPprop_COUNT
    } PictureProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitPictureProperties(	PictureProperties *	pip );

extern int docUpdPictureProperties(
			struct PropertyMask *		pDoneMask,
			PictureProperties *		pipTo,
			const struct PropertyMask *	pipUpdMask,
			const PictureProperties *	pipFrom );

extern void docPicturePropertyDifference(
			struct PropertyMask *		pDifMask,
			const PictureProperties *	pip1,
			const struct PropertyMask *	cmpMask,
			const PictureProperties *	pip2 );

extern int docSetPictureProperty(	PictureProperties *	pip,
					int			prop,
					int			val );

extern int docGetPictureProperty(	const PictureProperties *	pip,
					int				prop );

#   endif	/*  DOC_PICTURE_PROPERTIES_H	*/
