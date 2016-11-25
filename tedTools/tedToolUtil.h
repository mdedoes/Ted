/************************************************************************/
/*									*/
/*  Format tool shared pieced:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOOL_UTIL_H
#   define	TED_TOOL_UTIL_H

#   include	<guiBase.h>

struct InspectorSubject;
struct DocumentSelection;
struct SelectionGeometry;
struct SelectionDescription;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFormatValidateDimension(		int *		pNewValue,
						int *		pChanged,
						APP_WIDGET	w,
						int		oldValue );

extern void tedRefreshParaSubjectControls(
			struct InspectorSubject *		is,
			const struct DocumentSelection *	ds,
			const struct SelectionGeometry *	sg,
			const struct SelectionDescription *	sd,
			const unsigned char *			cmdEnabled );

#   endif	/*  TED_TOOL_UTIL_H */
