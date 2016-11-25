/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ITEM_SHADING_ADMIN_H
#   define	DOC_ITEM_SHADING_ADMIN_H

#   include	"docItemShading.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

typedef void (*ItemShadingFunction)(	const ItemShading *	is,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitItemShadingList(	NumberedPropertiesList *	isl );

extern int docItemShadingNumber(	NumberedPropertiesList *	isl,
					const ItemShading *	is );

extern int docShadingNumberIsShading(	const NumberedPropertiesList *	isl,
					int			n );

extern void docForAllItemShadings(	const NumberedPropertiesList *	isl,
					ItemShadingFunction	f,
					void *			through );

extern void docGetItemShadingByNumber(	ItemShading *		is,
					const NumberedPropertiesList *	isl,
					int			n );

extern int docMergeItemShadingLists(
				int **				pShadingMap,
				const int *			cmap,
				NumberedPropertiesList *	islTo,
				const NumberedPropertiesList *	islFrom );

#   endif	/*  DOC_ITEM_SHADING_ADMIN_H	*/
