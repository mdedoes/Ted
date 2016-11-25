#   ifndef		DOC_FIELD_SCANNER_H
#   define		DOC_FIELD_SCANNER_H

struct DocumentField;
struct EditRange;
struct ChildFields;

typedef int (*TreeFieldVisitor)(
			struct DocumentField *			df,
			const struct EditRange *		er,
			void *					through );

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docScanFieldsInRange(
			const struct ChildFields *	cf,
			const struct EditRange * 	er,
			TreeFieldVisitor		enterField,
			TreeFieldVisitor		leaveField,
			void *				through );

#   endif	/*	DOC_FIELD_SCANNER_H	*/
