/************************************************************************/
/*									*/
/*  Bookkeeping for IF fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_IF_FIELD_H
#   define DOC_IF_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct IfField
    {
    MemoryBuffer	iffExpression1;
    MemoryBuffer	iffOperator;
    MemoryBuffer	iffExpression2;
    MemoryBuffer	iffTrueText;
    MemoryBuffer	iffFalseText;
    } IfField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitIfField(	IfField *	iff );
extern void docCleanIfField(	IfField *	iff );

extern int docGetIfField(		IfField *			iff,
					const struct DocumentField *	df );

#   endif /*  DOC_IF_FIELD_H  */
