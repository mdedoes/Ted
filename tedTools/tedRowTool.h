/************************************************************************/
/*									*/
/*  Row tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_ROW_TOOL_H
#   define	TED_ROW_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedRowToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_ROW_TOOL_H */
