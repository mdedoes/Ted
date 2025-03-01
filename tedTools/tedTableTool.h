/************************************************************************/
/*									*/
/*  'Table' page of the format tool.					*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABLE_TOOL_H
#   define	TED_TABLE_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedTableToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_TABLE_TOOL_H */
