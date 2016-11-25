/************************************************************************/
/*									*/
/*  Headers/Footers tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_HEADER_FOOTER_TOOL_H
#   define	TED_HEADER_FOOTER_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedHeaderFooterToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_HEADER_FOOTER_TOOL_H */
