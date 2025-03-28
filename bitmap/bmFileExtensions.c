/************************************************************************/
/*									*/
/*  Build an array of file extensions for the image file formats.	*/
/*									*/
/************************************************************************/

#   include	"bitmapConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	"bitmap.h"
#   include	"bmformat.h"

#   include	<utilFileExtension.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make the list of bitmap FileExtensions that can be opened/saved.	*/
/*									*/
/************************************************************************/

int bmMakeFileExtensions(	struct AppFileExtension **	pAfeList,
				int *				pAfeCount )
    {
    int				i;
    AppFileExtension *		afe;
    int				afeCount;

    const BitmapFileFormat *	bff;
    BitmapFileType **		pbft;

    afeCount=  bmNumberOfFileTypes+ bmNumberOfFileFormats;
    afe= (AppFileExtension *)malloc( afeCount * sizeof(AppFileExtension) );
    if  ( ! afe )
	{ LXDEB(afeCount,afe); return -1;	}

    afeCount= 0;

    bff= bmFileFormats;
    for ( i= 0; i < bmNumberOfFileFormats; bff++, i++ )
	{
	unsigned	flags= APPFILE_IS_BASIC_TYPE;

	afe[afeCount].afeId= bff->bffId;
	afe[afeCount].afeFilter= bff->bffFileType->bftFileFilter;
	afe[afeCount].afeDescription= bff->bffDescription;
	afe[afeCount].afeExtension= bff->bffFileType->bftFileExtension;

	if  ( bff->bffFileType->bftRead )
	    { flags |= APPFILE_CAN_OPEN;	}
	if  ( bff->bffFileType->bftWrite )
	    { flags |= APPFILE_CAN_SAVE;	}

	afe[afeCount].afeUseFlags= flags;

	afeCount++;
	}

    pbft= bmFileTypes;
    for ( i= 0; i < bmNumberOfFileTypes; pbft++, i++ )
	{
	afe[afeCount].afeId= (*pbft)->bftContentType;
	afe[afeCount].afeFilter= (*pbft)->bftFileFilter;
	afe[afeCount].afeDescription= (*pbft)->bftTypeDescription;
	afe[afeCount].afeExtension= (*pbft)->bftFileExtension;

	if  ( ! (*pbft)->bftRead )
	    { afe[afeCount].afeUseFlags= 0;			}
	else{ afe[afeCount].afeUseFlags= APPFILE_CAN_OPEN;	}

	afeCount++;
	}

    *pAfeList= afe;
    *pAfeCount= afeCount;

    return 0;
    }
