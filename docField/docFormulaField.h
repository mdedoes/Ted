/************************************************************************/
/*									*/
/*  Bookkeeping for = (FORMULA) fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_FORMULA_FIELD_H
#   define DOC_FORMULA_FIELD_H

struct DocumentField;
struct MemoryBuffer;

typedef struct FormulaField
    {
				/**
				 *  The formula. It can hold field references.
				 */
    const struct MemoryBuffer *	ffFormula;

				/**
				 *  Sequence number of the component that 
				 *  holds the formula.
				 */
    int				ffFormulaComponent;

				/*
				 *  The Microsoft documentation mentions 
				 *  a bookmark, but I could not determine its 
				 *  meaning or role.
    const struct MemoryBuffer *	ffBookmark;
				 */

				/**
				 *  An optional numeric picture to format the 
				 *  value. (\\# option)
				 */
    const struct MemoryBuffer *	ffPicture;
    } FormulaField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitFormulaField(	FormulaField *	ff );
extern void docCleanFormulaField(	FormulaField *	ff );

extern int docGetFormulaField(		FormulaField *			ff,
					const struct DocumentField *	df );

#   endif /*  DOC_FORMULA_FIELD_H  */
