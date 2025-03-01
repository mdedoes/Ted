/************************************************************************/
/*									*/
/*  Image property tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_BITMAP_TOOL_H
#   define	TED_BITMAP_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedImageToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_BITMAP_TOOL_H */
