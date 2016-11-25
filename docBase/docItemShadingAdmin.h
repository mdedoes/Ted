/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ITEM_SHADING_ADMIN_H
#   define	DOC_ITEM_SHADING_ADMIN_H

struct NumberedPropertiesList;
struct ItemShading;

typedef void (*ItemShadingFunction)(	const struct ItemShading *	is,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitItemShadingList(	struct NumberedPropertiesList *	isl );

extern int docItemShadingNumberImpl(	struct NumberedPropertiesList *	isl,
					const struct ItemShading *	is );

extern int docShadingNumberIsShadingImpl(
					const struct NumberedPropertiesList *	isl,
					int				n );

extern void docForAllItemShadings(	const struct NumberedPropertiesList *	isl,
					ItemShadingFunction	f,
					void *			through );

extern const struct ItemShading * docGetItemShadingByNumberImpl(
					const struct NumberedPropertiesList *	isl,
					int				n );

extern int docMergeItemShadingLists(
				int **				pShadingMap,
				const int *			colorMap,
				struct NumberedPropertiesList *	islTo,
				const struct NumberedPropertiesList *	islFrom );

#   endif	/*  DOC_ITEM_SHADING_ADMIN_H	*/
