/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of numbered structs with object	*/
/*  properties.								*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_NUMBERED_PROPERTIES_ADMIN_H
#   define	UTIL_NUMBERED_PROPERTIES_ADMIN_H

#   include	"utilIntegerValueNode.h"
#   include	"utilPagedList.h"

struct NumberedPropertiesList;

typedef void (*NumberedPropertiesFunction)(
					const void *		vpr,
					int			n,
					void *			through );

typedef int (*NumberedPropertiesGetProperty)(
					const void *		vpr,
					int			prop );

typedef int (*NumberedPropertiesCopyProperties)(
					void *			vpto,
					const void *		vpfrom );

typedef int (*NumberedPropertiesMergeProperties)(
					void *			vpto,
					const void *		vpfrom,
					void *			through );

typedef int (*NumberedPropertiesGetNumber)(
			struct NumberedPropertiesList *		npl,
			const void *				vpfrom );

typedef struct NumberedPropertiesList
    {
    PagedList				nplPagedList;
    IntegerValueNode			nplValueNodes;
    int					nplPropCount;
    NumberedPropertiesGetProperty	nplGetProperty;
    NumberedPropertiesCopyProperties	nplCopyProperties;
    NumberedPropertiesGetNumber		nplGetNumber;
    } NumberedPropertiesList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitNumberedPropertiesList(
				    NumberedPropertiesList *	npl );

extern void utilCleanNumberedPropertiesList(
				    NumberedPropertiesList *	npl );

extern void utilForAllNumberedProperties(
				const NumberedPropertiesList *	npl,
				NumberedPropertiesFunction	f,
				void *				through );

extern void utilStartNumberedPropertyList(
				NumberedPropertiesList * 	npl,
				int				propCount,
				NumberedPropertiesGetProperty	getProp,
				NumberedPropertiesCopyProperties copyProps,
				NumberedPropertiesGetNumber	getNumber,
				int				sizeofItem,
				InitPagedListItem		initItem,
				CleanPagedListItem	 	cleanItem );

extern int utilGetPropertyNumber( NumberedPropertiesList *	npl,
				int				make,
				const void *			vob );

extern int utilGetPropertyNumberX( NumberedPropertiesList *	npl,
				int				make,
				int				propCount,
				const void *			vob );

extern int docMergeNumberedPropertiesLists(
				int **				pPropMap,
				NumberedPropertiesList *	nplTo,
				const NumberedPropertiesList *	nplFrom,
				NumberedPropertiesMergeProperties merge,
				void *				through );

#   endif	/*  UTIL_NUMBERED_PROPERTIES_ADMIN_H	*/
