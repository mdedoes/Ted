/************************************************************************/
/*									*/
/*  Font tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_FONT_TOOL_H
#   define	TED_FONT_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFontToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_FONT_TOOL_H */
