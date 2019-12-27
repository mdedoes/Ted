/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated in stead of typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_DOCUMENT_FIELD_H
#   define DOC_DOCUMENT_FIELD_H

#   include	<utilMemoryBuffer.h>
#   include	"docFieldInstructions.h"
#   include	"docSelectionScope.h"
#   include	"docEditRange.h"
#   include	"docChildFields.h"

struct SimpleLocale;

typedef struct DocumentField
    {
				/**
				 *  The kind of field: DOCfkXXXX
				 */
    unsigned char		dfKind;

				/**
				 *  Field data. Unused by Ted.
				 */
    MemoryBuffer		dfData;

				/**
				  *  The instructions of the field.
				  */
    FieldInstructions		dfInstructions;

				/**
				  *  The tree in the document that owns the 
				  *  field. For (BODY) fields that span 
				  *  sections, the section number is that of 
				  *  the head position of the field.
				  */
    SelectionScope		dfSelectionScope;

				/**
				  *  The position in the document of the head 
				  *  particule of the field. For instruction 
				  *  fields, dfHeadPosition is ABUSED to hold 
				  *  the number of the instructions component 
				  *  in the parent and the offset inside the 
				  *  component.
				  */
    EditPosition		dfHeadPosition;

				/**
				  *  The position in the document of the 
				  *  the tail particule of the field. For 
				  *  instruction fields, dfTailPosition holds 
				  *  the number of the instructions component 
				  *  in the parent and the offset inside the 
				  *  component.
				  */
    EditPosition		dfTailPosition;

				/*
				 * If a field spans part of a table,
				 * these can be used to give the first/last 
				 * column covered by the field. Is only 
				 * used for bookmarks.
				 */
    short			dfFirstColumn;
    short			dfLastColumn;

				/*
				 * Flags that reflect the status of the 
				 * field content.
				 */
    unsigned char		dfDirty;
    unsigned char		dfEdited;
    unsigned char		dfLocked;
    unsigned char		dfPrivate;

				/**
				 *  The children of the field in the field 
				 *  hierarchy. E.G: The hyperlinks inside a 
				 *  table of contents field.
				 */
    ChildFields			dfResultFields;

				/**
				 *  The parent of this field (If any)
				 */
    struct DocumentField *	dfParent;

				/**
				 *  The index of this field in the ChildFields 
				 *  of its parent.
				 */
    int				dfNumberInParent;

				/**
				 * The index of the field.
				 * ABUSED to keep a linked list of free fields
				 */
    int				dfFieldNumber;

				/**
				 *  Reference to the note of a chftn field.
				 *
				 *  ABUSED to remember the sequence number
				 *  of a SEQ field. [In that case, it counts 
				 *  from 1.]
				 */
    int				dfNoteIndex;

				/**
				 *  Make it possible to refer to the page 
				 *  number of a field.
				 */
    int				dfPage;

				/**
				 *  The fields that occur inside the 
				 *  instructions of the field. In 
				 *  = (Formula) and IF fields, the expression
				 *  uses fields inside the instructions to 
				 *  refer to a variety of values.
				 */
    ChildFields			dfInstructionFields;
    } DocumentField;

# define docGetBodySectNrOfField( df ) \
		((df)->dfSelectionScope.ssTreeType == DOCinBODY? \
		 (df)->dfSelectionScope.ssSectNr: \
		 (df)->dfSelectionScope.ssOwnerSectNr)

# define docEditPositionInField( df, ep ) \
	( docCompareEditPositions( &((df)->dfHeadPosition), (ep) ) <= 0 && \
	  docCompareEditPositions( &((df)->dfTailPosition), (ep) ) >= 0 )

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern DocumentField * docGetFirstField(const ChildFields *	rootFields );
extern DocumentField * docGetLastField(	const ChildFields *	rootFields );

extern DocumentField * docGetNextField(	const ChildFields *	rootFields,
					DocumentField *		df );

extern DocumentField * docGetPrevField(	const ChildFields *	rootFields,
					DocumentField *		df );

DocumentField * docGetFirstFieldOfSection(
					const ChildFields *	rootFields,
					int			sect );

extern DocumentField * docGetNextFieldInSection(
					const ChildFields *	rootFields,
					int			sect,
					DocumentField *		df );

extern void docInitDocumentField(	DocumentField *		df );

extern void docCleanDocumentField(	DocumentField *		df );

extern int docSetFieldInst(		DocumentField *		df,
					int			keepSpace,
					const char *		instBytes,
					int			instSize );

extern int docAddToFieldData(		DocumentField *		df,
					const char *		bytes,
					int			size );

extern int docFieldGetTc(		const DocumentField *	df,
					char *			pFlag,
					int *			pLevel,
					int *			pNumbered );

extern int docInsertChildField(		DocumentField *		parent,
					ChildFields *		cf,
					DocumentField *		df );

extern int docDeleteChildField(		ChildFields *		cf,
					DocumentField *		df );

extern DocumentField * docFindChildField( const ChildFields *	cf,
					const EditPosition *	ep,
					int			lastOne );

extern DocumentField * docFindTypedChildField(
					const ChildFields *	cf,
					const EditPosition *	ep,
					int			type );

extern void docSetFieldTail(		DocumentField *		dfPa,
					const EditPosition *	epTail );

extern void docSetFieldHead(		DocumentField *		dfPa,
					const EditPosition *	epHead );

extern int docAddResultChildToField(	DocumentField *		dfPa,
					DocumentField *		dfCh );

extern int docAddInstructionsChildToField(
					DocumentField *		dfPa,
					DocumentField *		dfCh );

extern int docInsertFieldInTree(	ChildFields *		cf,
					DocumentField *		df );

extern DocumentField *	docFieldGetCommonParent(
					DocumentField *	dfLeft,
					DocumentField *	dfRight );

extern int docFieldFormatInteger( MemoryBuffer *		mbResult,
				int				format,
				int				value );

extern int docFieldFormatNumber( MemoryBuffer *			mbResult,
				const MemoryBuffer *		picture,
				double				value,
				const struct SimpleLocale *	sl );

extern int docCopyFieldProperties(	DocumentField *		dfTo,
					const DocumentField *	dfFrom );

extern int docSetFieldProperty(		DocumentField *		df,
					int			prop,
					int			val );

extern int docAllocateInstructionComponents(	DocumentField *		df,
						int			n );

extern DocumentField * docFindFieldInRange(
					const EditRange *	er,
					const ChildFields *	cf,
					int			lastOne,
					int			kind );

extern void docShiftChildFieldReferences( const ChildFields *	cf,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift );

extern const DocumentField * docGetLocationParent(
					const DocumentField *	df );

#   endif /*  DOC_DOCUMENT_FIELD_H  */
