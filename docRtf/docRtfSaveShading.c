/************************************************************************/
/*									*/
/*  Read/Write shadings to/from rtf.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<psShading.h>
#   include	<docItemShading.h>
#   include	"docRtfWriterImpl.h"
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save shading definition.						*/
/*									*/
/************************************************************************/

static void docRtfSaveShading(	RtfWriter *			rw,
				const ItemShading *		is,
				const char * const * 		patTags,
				int				patTagCount,
				const char *			foreTag,
				const char *			backTag,
				const char *			levelTag )
    {
    if  ( is->isPattern != DOCspSOLID )
	{
	docRtfWriteEnumTag( rw, patTags, is->isPattern,
						patTagCount, PSshd_COUNT );
	}

    if  ( is->isForeColor > 0 )
	{ docRtfWriteArgTag( rw, foreTag, is->isForeColor );	}
    if  ( is->isBackColor > 0 )
	{ docRtfWriteArgTag( rw, backTag, is->isBackColor );	}
    if  ( is->isLevel > 0 )
	{ docRtfWriteArgTag( rw, levelTag, is->isLevel );	}

    return;
    }

void docRtfSaveShadingByNumber(	RtfWriter *			rw,
				int				num,
				const char * const * 		patTags,
				int				patTagCount,
				const char *			foreTag,
				const char *			backTag,
				const char *			levelTag )
    {
    const ItemShading *		is;

    is= docGetItemShadingByNumber( rw->rwDocument, num );

    docRtfSaveShading( rw, is, patTags, patTagCount,
					    foreTag, backTag, levelTag );

    return;
    }
