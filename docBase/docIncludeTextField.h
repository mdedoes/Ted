/************************************************************************/
/*									*/
/*  Bookkeeping for INCLUDETEXT fields.				*/
/*									*/
/************************************************************************/

#   ifndef DOC_INCLUDETEXT_FIELD_H
#   define DOC_INCLUDETEXT_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct IncludeTextField
    {
    MemoryBuffer	itfFilename;
    MemoryBuffer	itfBookmark;
    unsigned char	itfConstantFields;	/*  \!	*/
    MemoryBuffer	itfClassName;		/*  \c	*/
    MemoryBuffer	itfNamespace;		/*  \n	*/
    MemoryBuffer	itfTransform;		/*  \t	*/
    MemoryBuffer	itfXPath;		/*  \x	*/
    } IncludeTextField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitIncludeTextField(		IncludeTextField *	itf );
extern void docCleanIncludeTextField(		IncludeTextField *	itf );

extern int docGetIncludeTextField(	IncludeTextField *		itf,
					const struct DocumentField *	df );

#   endif /*  DOC_INCLUDETEXT_FIELD_H  */
