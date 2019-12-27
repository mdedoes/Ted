/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated in stead of typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_H
#   define DOC_FIELD_H

struct BufferDocument;
struct DocumentSelection;
struct DocumentPosition;
struct RecalculateFields;
struct DocumentField;
struct TextAttribute;
struct PropertyMask;
struct MemoryBuffer;
struct ParagraphBuilder;

/************************************************************************/
/*									*/
/*  Kind of field. Is an index in a descriptive array of field kinds.	*/
/*									*/
/*  Fields in the 'Ted' sense are not necessarily fields in the 'Word'	*/
/*  sense. Everything inside the text, that can possibly result in	*/
/*  a piece of document, but is possibly under the control of the	*/
/*  application is a field in the sense of 'Ted'.			*/
/*									*/
/************************************************************************/

/*  f1  */
typedef int (*CALCULATE_TEXT_PARTICULES)(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

/*  f2  */
typedef int (*CALCULATE_TEXT_STRING)(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

/*  g  */
#   define	FIELDdoNOTHING		0
#   define	FIELDdoNEVER		0
#   define	FIELDdoDOC_FORMATTED	(1<<0)
#   define	FIELDdoPAGE_NUMBER	(1<<1)
#   define	FIELDdoDOC_INFO		(1<<2)
#   define	FIELDdoDOC_COMPLETE	(1<<3)
#   define	FIELDdoCHFTN		(1<<4)
#   define	FIELDdoLISTTEXT		(1<<5)
#   define	FIELDdoSEQ		(1<<6)
#   define	FIELDdoTOC		(1<<7) /* Have their own call */
#   define	FIELDdoINCLUDETEXT	(1<<8) /* Have their own call */
#   define	FIELDdoMERGE		(1<<9) /* Have their own call */

typedef struct FieldKindInformation
    {
				/**
				 *  The (case insensitive) string that 
				 *  identifies the field in the field 
				 *  instructions. NOTE that the notion of a 
				 *  field is abused by 'Ted' for a few things 
				 *  that are not fields in MS-Word.
				 */
    const char *		fkiLabel;

				/**
				 *  The maximum level in the block hierarchy 
				 *  that certainly holds the result/content of 
				 *  this kind of field.
				 */
    int				fkiLevel;

				/**
				 *  Tells whether this kind of field is a 
				 *  field in the RTF file.
				 */
    int				fkiIsFieldInRtf;

				/**
				 *  Tells whether this kind of field is a 
				 *  destination in the RTF file.
				 */
    int				fkiIsDestinationInRtf;

				/**
				 *  Recalculate a TEXT level field. The 
				 *  standard workhorse 
				 *  docRecalculateParaStringTextParticules()
				 *  can be used for all substitutions that 
				 *  simply return a string. In its turn it uses 
				 *  fki->fkiCalculateTextString() to obtain
				 *  the string. Other particule evluators might 
				 *  decide to do something similar.
				 *
				 *  Returns the number of the tail particule 
				 *  of the field.
				 */
    CALCULATE_TEXT_PARTICULES	fkiCalculateTextParticules;

				/**
				 *  Recalculate the string for a TEXT level 
				 *  field.
				 */
    CALCULATE_TEXT_STRING	fkiCalculateTextString;

				/** 
				 *  When to recalculate fields in the text.
				 */
    unsigned int		fkiCalculateWhen;

				/** 
				 *  Is the field result editable like the 
				 *  text of a HYPERLINK or readonly like the
				 *  result of a PAGEREF?
				 */
    unsigned char		fkiResultEditable;

				/**  
				 *  Is a field of this kind constrained to a 
				 *  single paragraph.
				 */
    unsigned char		fkiSingleParagraph;

				/**  
				 *  Do not split instructions on white space.
				 */
    unsigned char		fkiKeepInstructionsSpace;

				/**
				 *  Some things that are a destination in RTF 
				 *  are stored as a field in Ted. This is the 
				 *  control word of the destination.
				 */
    const char *		fkiRtfTag;
    } FieldKindInformation;

/************************************************************************/
/*									*/
/*  Field Kind Information.						*/
/*									*/
/************************************************************************/

extern const FieldKindInformation	DOC_FieldKinds[];
extern const int			DOC_FieldKindCount;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docSuggestNewBookmarkName(
			    struct MemoryBuffer *		markName,
			    const struct BufferDocument *	bd,
			    const struct DocumentSelection *	ds );

extern int docMakeBookmarkUnique(
			    const struct BufferDocument *	bd,
			    struct MemoryBuffer *		markName );

extern int docFieldKindFromInstructions(
				int *				pKeepSpace,
				const char *			text,
				int				size );

extern struct DocumentField * docFindFieldForPosition(
				struct BufferDocument *		bd,
				const struct DocumentPosition *	dp );

extern struct DocumentField * docFindTypedFieldForPosition(
				struct BufferDocument *		bd,
				const struct DocumentPosition *	dp,
				int				kind,
				int				lastOne );

extern struct DocumentField * docFindTypedFieldInSelection(
				struct BufferDocument *		bd,
				const struct DocumentSelection * ds,
				int				kind,
				int				lastOne );

extern int docSetHyperlinkAttribute(	struct TextAttribute *	taSet,
					struct PropertyMask *		taSetMask,
					struct BufferDocument *	bd );

extern int docRemoveHyperlinkAttribute(	struct TextAttribute *	taSet,
					struct PropertyMask *		taSetMask,
					struct BufferDocument *	bd );

#   endif /*  DOC_FIELD_H  */
