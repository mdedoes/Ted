/************************************************************************/
/*									*/
/*  Spell Tool.								*/
/*									*/
/************************************************************************/

#   ifndef	TED_SPELL_TOOL_H
#   define	TED_SPELL_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedSpellToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_SPELL_TOOL_H */
