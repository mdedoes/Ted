/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible combinations*/
/*  of rows in the document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_PROPERTIES_ADMIN_H
#   define	DOC_ROW_PROPERTIES_ADMIN_H

struct RowProperties;
struct NumberedPropertiesList;

typedef void (*RowPropertiesFunction)(
			const struct RowProperties *	 	rp,
			int					n,
			void *					through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRowPropertiesList(
			struct NumberedPropertiesList *		rpl );

extern int docRowPropertiesNumberImpl(
			struct NumberedPropertiesList *		rpl,
			const struct RowProperties *		rp );

extern const struct RowProperties * docGetRowPropertiesByNumberImpl(
			const struct NumberedPropertiesList *	rpl,
			int					n );

#   endif	/*  DOC_ROW_PROPERTIES_ADMIN_H	*/
