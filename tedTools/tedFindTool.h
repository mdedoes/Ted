/************************************************************************/
/*									*/
/*  Find Tool.								*/
/*									*/
/************************************************************************/

#   ifndef	TED_FIND_TOOL_H
#   define	TED_FIND_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;
struct DocumentSelection;
struct SelectionDescription;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFindToolSetSelectionAsPattern(
				void *					vft,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd );

extern int tedAppFindToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

extern int tedDocFindToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_FIND_TOOL_H */
