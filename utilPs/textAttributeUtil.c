/************************************************************************/
/*									*/
/*  Utility functions around text attributes.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	"textAttributeUtil.h"
#   include	"textAttribute.h"
#   include	<geoGrid.h>
#   include	"psTextExtents.h"

/************************************************************************/
/*									*/
/*  Translate twips to pixels.						*/
/*									*/
/************************************************************************/

int textGetPixelSize(		double				pixelsPerTwip,
				const TextAttribute *		ta )
    {
    int			twipsSize= TA_FONT_SIZE_TWIPS( ta );

    if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT		||
	  ta->taSuperSub == TEXTvaSUBSCRIPT		)
	{
	return COORDtoGRID( pixelsPerTwip, SUPERSUB_SIZE( twipsSize ) );
	}
    else{
	return COORDtoGRID( pixelsPerTwip, twipsSize );
	}
    }

