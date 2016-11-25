/************************************************************************/
/*									*/
/*  Notes tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_NOTES_TOOL_H
#   define	TED_NOTES_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedFootnotesToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

extern int tedEndnotesToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_NOTES_TOOL_H */
