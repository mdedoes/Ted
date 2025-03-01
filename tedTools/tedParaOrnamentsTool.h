/************************************************************************/
/*									*/
/*  Paragraph borders and shading tool:					*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_ORNAMENTS_TOOL_H
#   define	TED_PARA_ORNAMENTS_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedParaOrnamentsToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_PARA_ORNAMENTS_TOOL_H */
