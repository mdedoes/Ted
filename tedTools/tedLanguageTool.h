/************************************************************************/
/*									*/
/*  Language tool:							*/
/*									*/
/*  The page on the format tool to set the language. (and possibly	*/
/*  exclude text from spell checking)					*/
/*									*/
/************************************************************************/

#   ifndef	TED_LANGUAGE_TOOL_H
#   define	TED_LANGUAGE_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedLanguageToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_LANGUAGE_TOOL_H */
