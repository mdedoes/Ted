/************************************************************************/
/*									*/
/*  Read/Write shadings to/from rtf.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docItemShadingAdmin.h>
#   include	<psShading.h>
#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"

/************************************************************************/
/*									*/
/*  Save border definition.						*/
/*									*/
/************************************************************************/

static void docRtfSaveShading(	RtfWriter *			rwc,
				const ItemShading *		is,
				const char * const * 		patTags,
				int				patTagCount,
				const char *			foreTag,
				const char *			backTag,
				const char *			levelTag )
    {
    if  ( is->isPattern != DOCspSOLID )
	{
	docRtfWriteEnumTag( rwc, patTags, is->isPattern,
						patTagCount, PSshd_COUNT );
	}

    if  ( is->isForeColor > 0 )
	{ docRtfWriteArgTag( rwc, foreTag, is->isForeColor );	}
    if  ( is->isBackColor > 0 )
	{ docRtfWriteArgTag( rwc, backTag, is->isBackColor );	}
    if  ( is->isLevel > 0 )
	{ docRtfWriteArgTag( rwc, levelTag, is->isLevel );	}

    return;
    }

void docRtfSaveShadingByNumber(	RtfWriter *			rwc,
				int				num,
				const char * const * 		patTags,
				int				patTagCount,
				const char *			foreTag,
				const char *			backTag,
				const char *			levelTag )
    {
    const BufferDocument *		bd= rwc->rwDocument;
    const NumberedPropertiesList *	isl= &(bd->bdItemShadingList);

    ItemShading				is;

    docGetItemShadingByNumber( &is, isl, num );

    docRtfSaveShading( rwc, &is, patTags, patTagCount,
					    foreTag, backTag, levelTag );

    return;
    }
