/************************************************************************/
/*									*/
/*  Unicode to byte mappings for legacy encodings.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"uniLegacyMapping.h"

#   include	<appDebugon.h>

/************************************************************************/

IndexMapping UNI_SymbolToGlyphMapping;
IndexMapping UNI_DingbatsToGlyphMapping;

IndexMapping UNI_CP1250ToGlyphMapping;
IndexMapping UNI_CP1251ToGlyphMapping;
IndexMapping UNI_CP1252ToGlyphMapping;
IndexMapping UNI_CP1253ToGlyphMapping;
IndexMapping UNI_CP1254ToGlyphMapping;

IndexSet UNI_SymbolCodeSet;
IndexSet UNI_DingbatsCodeSet;

/************************************************************************/
/*									*/
/*  Fill a legacy mapping.						*/
/*									*/
/************************************************************************/

int textSetLegacyMapping(	IndexMapping *	im,
				IndexSet *	is,
				const int *	unicodes )
    {
    if  ( utilIndexMappingBuildBackward( im, unicodes, 256 )	)
	{ LDEB(1); return -1;	}
    if  ( utilIndexSetAddArray( is, unicodes, 256 )	)
	{ LDEB(1); return -1;	}

    return 0;
    }
