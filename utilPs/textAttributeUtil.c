/************************************************************************/
/*									*/
/*  Utility functions around text attributes.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	"textAttributeUtil.h"
#   include	<geoGrid.h>
#   include	"psFontInfo.h"

/************************************************************************/
/*									*/
/*  Translate twips to pixels.						*/
/*									*/
/************************************************************************/

int textGetPixelSize(		double				pixelsPerTwip,
				const TextAttribute *		ta )
    {
    int			twipsSize= 10* ta->taFontSizeHalfPoints;

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT		||
	  ta->taSuperSub == DOCfontSUBSCRIPT		)
	{
	return COORDtoGRID( pixelsPerTwip, SUPERSUB_SIZE( twipsSize ) );
	}
    else{
	return COORDtoGRID( pixelsPerTwip, twipsSize );
	}

    }

