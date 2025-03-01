/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated in stead of typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_CHILD_FIELDS_H
#   define DOC_CHILD_FIELDS_H

struct DocumentField;

typedef struct ChildFields
    {
    struct DocumentField **	cfChildren;
    int				cfChildCount;
    } ChildFields;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitChildFields(		ChildFields *		cf );
extern void docCleanChildFields(	ChildFields *		cf );

#   endif /*  DOC_CHILD_FIELDS_H  */
