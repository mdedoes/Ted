/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of border properties.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BORDER_PROPERTY_ADMIN_H
#   define	DOC_BORDER_PROPERTY_ADMIN_H

struct BorderProperties;
struct NumberedPropertiesList;

typedef void (*BorderPropertiesFunction)(
			const struct BorderProperties *		bp,
			int					n,
			void *					through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBorderPropertyList(	struct NumberedPropertiesList *	bpl );

extern int docBorderPropertiesNumberImpl(
			struct NumberedPropertiesList *		bpl,
			const struct BorderProperties *		bp );

extern void docForAllBorderProperties(
			const struct NumberedPropertiesList *	bpl,
			BorderPropertiesFunction		f,
			void *					through );

extern const struct BorderProperties * docGetBorderPropertiesByNumberImpl(
			const struct NumberedPropertiesList *	bpl,
			int					n );

extern int docBorderNumberIsBorderImpl(
			const struct NumberedPropertiesList *	bpl,
			int					n );

extern int docMergeBorderPropertiesLists(
			int **					pBorderMap,
			const int *				colorMap,
			struct NumberedPropertiesList *		bplTo,
			const struct NumberedPropertiesList *	bplFrom );

#   endif	/*  DOC_BORDER_PROPERTY_ADMIN_H	*/
