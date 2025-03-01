/************************************************************************/
/*									*/
/*  The stack of fields we keep while reading/copying/building a	*/
/*  document.								*/
/*									*/
/************************************************************************/

#   ifndef		DOC_FIELD_STACK_H
#   define		DOC_FIELD_STACK_H

struct DocumentField;
struct DocumentTree;

typedef enum FieldStackPiece
    {
    FSpieceFIELD,
    FSpieceFLDINST,
    FSpieceFLDRSLT,

    FSpiece_COUNT
    } FieldStackPiece;

typedef struct FieldStackLevel
    {
				/**
				 *  The child field. Result or Instructions
				 */
    struct DocumentField *	fslField;
				/**
				 *  Is the child to be inserted in the
				 *  Result or the Instructions?
				 */
    int				fslPiece;
				/**
				 *  The previous (parent) level.
				 */
    struct FieldStackLevel *	fslPrev;
    } FieldStackLevel;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docFieldStackPopLevel(	FieldStackLevel **		pFsl,
					struct DocumentTree *		dt );

extern int docFieldStackPushLevel(	FieldStackLevel **		pFsl,
					struct DocumentField *		df,
					int				piece );

extern void docCleanFieldStack(		FieldStackLevel *		fsl );

#   endif	/*	DOC_FIELD_STACK_H	*/
