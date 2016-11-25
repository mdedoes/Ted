/************************************************************************/
/*									*/
/*  Recalculate TOC fields.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CALCULATE_TOC_FIELD_H
#   define	DOC_CALCULATE_TOC_FIELD_H

struct BufferDocument;
struct DocumentField;
struct RecalculateFields;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRecalculateTocFields( struct RecalculateFields *	rf );

extern int docRecalculateOneTocField(	struct BufferDocument *	bdDoc,
					struct DocumentField *	df );

#   endif /*	DOC_CALCULATE_TOC_FIELD_H	*/
