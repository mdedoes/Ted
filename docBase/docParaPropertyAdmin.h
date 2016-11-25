/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of paragraph properties.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PROPERTY_ADMIN_H
#   define	DOC_PARA_PROPERTY_ADMIN_H

struct ParagraphProperties;
struct NumberedPropertiesList;

typedef void (*ParagraphPropertiesFunction)(
				const struct ParagraphProperties *	cp,
				int				n,
				void *				through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParagraphPropertyList(
				struct NumberedPropertiesList *	ppl );

extern int docParagraphPropertiesNumber(struct NumberedPropertiesList *	ppl,
				const struct ParagraphProperties *	pp );

extern void docForAllParagraphProperties(
				const struct NumberedPropertiesList *	ppl,
				ParagraphPropertiesFunction	f,
				void *				through );

extern const struct ParagraphProperties * docGetParagraphPropertiesByNumber(
				const struct NumberedPropertiesList *	ppl,
				int					n );

extern int docMergeParagraphPropertiesLists(
				int **				pParagraphMap,
				const int *			listStyleMap,
				const int *			rulerMap,
				const int *			borderMap,
				const int *			shadingMap,
				const int *			frameMap,
				struct NumberedPropertiesList *	pplTo,
				const struct NumberedPropertiesList *	pplFrom );

#   endif	/*  DOC_PARA_PROPERTY_ADMIN_H	*/
