/************************************************************************/
/*									*/
/*  Ted: Bookmark tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_BOOKMARK_TOOL_H
#   define	TED_BOOKMARK_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedBookmarkToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );


#   endif	/*  TED_BOOKMARK_TOOL_H  */
