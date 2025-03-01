/************************************************************************/
/*									*/
/*  The format '*' flag of a field.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_FORMAT_H
#   define DOC_FIELD_FORMAT_H

struct InstructionsComponent;
struct FieldInstructions;

/************************************************************************/
/*									*/
/*  Modifiers to the field format. Follow the \\* flag.			*/
/*									*/
/************************************************************************/

typedef enum FieldFormat
    {
    FIELDformatASIS= 0,

	    /* Case conversion */
    FIELDformatLOWER,
    FIELDformatUPPER,
    FIELDformatFIRSTCAP,
    FIELDformatCAPS,

	    /* Number formatting */
    FIELDformatALPHABETIC_UPPER,	/*  Depends on first letter	*/
    FIELDformatARABIC,
    FIELDformatCARDTEXT,
    FIELDformatDOLLARTEXT,
    FIELDformatHEX_UPPER,		/*  Depends on first letter	*/
    FIELDformatORDINAL,
    FIELDformatORDTEXT,
    FIELDformatROMAN_UPPER,		/*  Depends on first letter	*/

    FIELDformatCHARFORMAT,
    FIELDformatMERGEFORMAT,

    FIELDformatALPHABETIC_LOWER,	/*  Depends on first letter	*/
    FIELDformatHEX_LOWER,		/*  Depends on first letter	*/
    FIELDformatROMAN_LOWER,		/*  Depends on first letter	*/

    FIELDformat_COUNT
    } FieldFormat;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docFieldFormatInt(
			const struct InstructionsComponent *	ic );

extern int docFieldInstructionsAddFormat(
			struct FieldInstructions *		fi,
			int					format );

#   endif /*  DOC_FIELD_FORMAT_H  */
