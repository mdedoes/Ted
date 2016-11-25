/************************************************************************/
/*									*/
/*  Ted: Link tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_LINK_TOOL_H
#   define	TED_LINK_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedLinkToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_LINK_TOOL_H  */
