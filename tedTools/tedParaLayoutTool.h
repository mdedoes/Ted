/************************************************************************/
/*									*/
/*  Paragraph layout tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_LAYOUT_TOOL_H
#   define	TED_PARA_LAYOUT_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedParaLayoutToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_PARA_LAYOUT_TOOL_H */
