/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BORDER_PROPERTY_ADMIN_H
#   define	DOC_BORDER_PROPERTY_ADMIN_H

#   include	"docBorderProperties.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

typedef void (*BorderPropertiesFunction)( const BorderProperties * bp,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBorderPropertyList(	NumberedPropertiesList *	bpl );

extern int docBorderPropertiesNumber(	NumberedPropertiesList *	bpl,
					const BorderProperties *	bp );

extern void docForAllBorderProperties(
				const NumberedPropertiesList *	bpl,
				BorderPropertiesFunction	f,
				void *				through );

extern void docGetBorderPropertiesByNumber(
					BorderProperties *		bp,
					const NumberedPropertiesList *	bpl,
					int				n );

extern int docBorderNumberIsBorder(	const NumberedPropertiesList *	bpl,
					int				n );

extern int docMergeBorderPropertiesLists(
				int **				pBorderMap,
				const int *			cmap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom );

#   endif	/*  DOC_BORDER_PROPERTY_ADMIN_H	*/
