/************************************************************************/
/*									*/
/*  Format tool subjects.						*/
/*									*/
/************************************************************************/

#   ifndef	TED_FORMAT_TOOL_SUBJECT_H
#   define	TED_FORMAT_TOOL_SUBJECT_H

struct InspectorSubject;
struct DocumentSelection;
struct SelectionGeometry;
struct SelectionDescription;
struct SelectionDescription;
struct BufferDocument;

/************************************************************************/

typedef void (*ToolRefreshPage)( void *				tool,
				int *				pEnabled,
				int *				pPref,
				struct InspectorSubject *		is,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

typedef void (*ToolResetCache)(	void *				tool );

/************************************************************************/
/*									*/
/*  Represents a format tool.						*/
/*									*/
/************************************************************************/

typedef struct FormatToolSubjectType
    {
				/**
				 *  Adapt the tool to the current selection
				 *  in the (current) document.
				 */
    ToolRefreshPage		ftstRefreshPage;

    ToolResetCache		ftstResetCache;

    } FormatToolSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  TED_FORMAT_TOOL_SUBJECT_H */
