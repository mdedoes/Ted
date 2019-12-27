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

static int docRtfSaveShading(	RtfWriter *			rw,
				int				scope,
				const ItemShading *		is )
    {
    if  ( is->isPattern != DOCspSOLID )
	{
	if  ( docRtfWriteProperty( rw, scope, ISpropPATTERN, is->isPattern ) )
	    { LLLDEB(scope,ISpropPATTERN,is->isPattern); return -1;	}

	}

    if  ( is->isForeColor > 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
					ISpropFORE_COLOR, is->isForeColor ) )
	    { LLLDEB(scope,ISpropFORE_COLOR,is->isForeColor); return -1; }
	}
    if  ( is->isBackColor > 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
					ISpropBACK_COLOR, is->isBackColor ) )
	    { LLLDEB(scope,ISpropBACK_COLOR,is->isBackColor); return -1; }
	}
    if  ( is->isLevel > 0 )
	{
	if  ( docRtfWriteProperty( rw, scope,
					ISpropLEVEL, is->isLevel ) )
	    { LLLDEB(scope,ISpropLEVEL,is->isLevel); return -1; }
	}

    return 0;
    }

void docRtfSaveShadingByNumber(	RtfWriter *			rw,
				int				scope,
				int				num )
    {
    const ItemShading *		is;

    is= docGetItemShadingByNumber( rw->rwDocument, num );

    docRtfSaveShading( rw, scope, is );

    return;
    }
