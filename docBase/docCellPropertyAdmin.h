/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of cell properties.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CELL_PROPERTY_ADMIN_H
#   define	DOC_CELL_PROPERTY_ADMIN_H

struct NumberedPropertiesList;
struct CellProperties;

typedef void (*CellPropertiesFunction)( const struct CellProperties *	cp,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCellPropertyList(	struct NumberedPropertiesList *	cpl );

extern int docCellPropertiesNumber(	struct NumberedPropertiesList *	cpl,
					const struct CellProperties *		cp );

extern void docForAllCellProperties(
				const struct NumberedPropertiesList *	cpl,
				CellPropertiesFunction		f,
				void *				through );

extern const struct CellProperties * docGetCellPropertiesByNumber(
					const struct NumberedPropertiesList *	cpl,
					int				n );

extern int docMergeCellPropertiesLists(
				int **				pCellMap,
				const int *			borderMap,
				const int *			shadingMap,
				struct NumberedPropertiesList *	cplTo,
				const struct NumberedPropertiesList *	cplFrom );

#   endif	/*  DOC_CELL_PROPERTY_ADMIN_H	*/
