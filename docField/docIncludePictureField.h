/************************************************************************/
/*									*/
/*  Bookkeeping for INCLUDEPICTURE fields.				*/
/*									*/
/************************************************************************/

#   ifndef DOC_INCLUDEPICTURE_FIELD_H
#   define DOC_INCLUDEPICTURE_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct IncludePictureField
    {
    MemoryBuffer	ipfFilename;
    MemoryBuffer	ipfConverter;		/* \c	*/
    unsigned char	ipfDoNotSaveResult;	/* \d	*/

			/**
			 *  Width of the image in twips. Only used 
			 *  when the IncludePicture field is abused to 
			 *  represent a NeXTGraphics object. (\w)
			 */
    int			ipfTwipsWide;

			/**
			 *  Height of the image in twips. Only used 
			 *  when the IncludePicture field is abused to 
			 *  represent a NeXTGraphics object. (\h)
			 */
    int			ipfTwipsHigh;
    } IncludePictureField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitIncludePictureField(		IncludePictureField *	ipf );
extern void docCleanIncludePictureField(	IncludePictureField *	ipf );

extern int docGetIncludePictureField(	IncludePictureField *		ipf,
					const struct DocumentField *	df );

#   endif /*  DOC_INCLUDEPICTURE_FIELD_H  */
