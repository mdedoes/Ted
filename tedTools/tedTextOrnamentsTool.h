/************************************************************************/
/*									*/
/*  Text color, border and shading tool:				*/
/*									*/
/************************************************************************/

#   ifndef	TED_TEXT_ORNAMENTS_TOOL_H
#   define	TED_TEXT_ORNAMENTS_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedTextOrnamentsToolFillSubjectTypes(
				struct EditApplication *	ea,
				struct InspectorSubjectType *	ist,
				struct FormatToolSubjectType *	ftst );

#   endif	/*  TED_PARA_ORNAMENTS_TOOL_H */
