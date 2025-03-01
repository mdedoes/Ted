/************************************************************************/
/*									*/
/*  Unicode to byte mappings for legacy encodings.			*/
/*									*/
/************************************************************************/

#   ifndef	UNI_LEGACY_MAPPING_H
#   define	UNI_LEGACY_MAPPING_H

#   include	<utilIndexMapping.h>
#   include	<utilIndexSet.h>

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern IndexMapping UNI_SymbolToGlyphMapping;
extern IndexMapping UNI_DingbatsToGlyphMapping;

extern IndexMapping UNI_CP1250ToGlyphMapping;
extern IndexMapping UNI_CP1251ToGlyphMapping;
extern IndexMapping UNI_CP1252ToGlyphMapping;
extern IndexMapping UNI_CP1253ToGlyphMapping;
extern IndexMapping UNI_CP1254ToGlyphMapping;

extern IndexSet UNI_SymbolCodeSet;
extern IndexSet UNI_DingbatsCodeSet;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int textSetLegacyMapping(	IndexMapping *	im,
					IndexSet *	is,
					const int *	unicodes );

#   endif
