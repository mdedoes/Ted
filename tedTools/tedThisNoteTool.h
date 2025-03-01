/************************************************************************/
/*									*/
/*  Notes tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_THIS_NOTE_TOOL_H
#   define	TED_THIS_NOTE_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedThisNoteToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_THIS_NOTE_TOOL_H */
