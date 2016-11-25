/************************************************************************/
/*									*/
/*  'Table of Contents' page of the format tool.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOC_TOOL_H
#   define	TED_TOC_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedTocToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_TOC_TOOL_H */
