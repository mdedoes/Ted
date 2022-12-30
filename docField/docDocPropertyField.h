/************************************************************************/
/*									*/
/*  Bookkeeping for DOCPROPERTY fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_DOCPROPERTY_FIELD_H
#   define DOC_DOCPROPERTY_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct DocPropertyField
    {
    MemoryBuffer	dpfPropertyName;
    unsigned char	dpfCaseShift;		/* \*	*/
    } DocPropertyField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitDocPropertyField(		DocPropertyField *	dpf );
extern void docCleanDocPropertyField(		DocPropertyField *	dpf );

extern int docGetDocPropertyField(
				DocPropertyField *		dpf,
				const struct DocumentField *	df );

#   endif /*  DOC_DOCPROPERTY_FIELD_H  */
