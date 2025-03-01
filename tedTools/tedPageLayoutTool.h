/************************************************************************/
/*									*/
/*  Intermediary Routines for the page layout page.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_PAGE_LAYOUT_TOOL_H
#   define	TED_PAGE_LAYOUT_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedPageLayoutToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_PAGE_LAYOUT_TOOL_H */
