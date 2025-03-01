/************************************************************************/
/*									*/
/*  List tool:								*/
/*									*/
/*  The page on the format tool to edit list properties.		*/
/*									*/
/************************************************************************/

#   ifndef	TED_LIST_TOOL_H
#   define	TED_LIST_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedListToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_LIST_TOOL_H */
