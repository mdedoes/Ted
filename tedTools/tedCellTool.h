/************************************************************************/
/*									*/
/*  Cell tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_CELL_TOOL_H
#   define	TED_CELL_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedCellToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_CELL_TOOL_H */
