/************************************************************************/
/*									*/
/*  Tabs tool:								*/
/*									*/
/*  The page on the format tool to edit tab stops.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABS_TOOL_H
#   define	TED_TABS_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedTabsToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_TABS_TOOL_H */
