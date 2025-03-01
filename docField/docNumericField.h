/************************************************************************/
/*									*/
/*  Bookkeeping for numeric fields such as section, sectionpages, page	*/
/*  fields.								*/
/*									*/
/************************************************************************/

#   ifndef DOC_NUMERIC_FIELD_H
#   define DOC_NUMERIC_FIELD_H

struct DocumentField;

typedef struct NumericField
    {
    unsigned char	nfNumberFormat;		/* \*	*/
    } NumericField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitNumericField(	NumericField *			nf );
extern void docCleanNumericField(	NumericField *			nf );

extern int docGetNumericField(		NumericField *			nf,
					int				expect,
					const struct DocumentField *	df );

extern int docSetNumericField(		struct DocumentField *		df,
					int				kindInt,
					const char *			kindStr,
					const NumericField *		nf );

#   endif /*  DOC_NUMERIC_FIELD_H  */
