/************************************************************************/
/*									*/
/*  Column Tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_COLUMN_TOOL_H
#   define	TED_COLUMN_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;


/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedColumnToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_COLUMN_TOOL_H */
