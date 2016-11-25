/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_ADMIN_H
#   define	UTIL_TEXT_ATTRIBUTE_ADMIN_H

struct IndexSet;
struct TextAttribute;
struct NumberedPropertiesList;

typedef int (*TextAttributeFunction)(	int			n,
					const struct TextAttribute *	ta,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void textInitTextAttributeList(	struct NumberedPropertiesList *	tal );

extern int textTextAttributeNumber(	struct NumberedPropertiesList *	tal,
					const struct TextAttribute *		ta );

extern const struct TextAttribute * textGetTextAttributeByNumber(
					const struct NumberedPropertiesList *	tal,
					int				n );

extern int textForAllAttributesInList(
				const struct NumberedPropertiesList *	tal,
				const struct IndexSet *		filter,
				TextAttributeFunction		f,
				void *				through );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_ADMIN_H	*/
