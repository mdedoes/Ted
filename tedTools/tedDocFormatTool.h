/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_DOC_FORMAT_TOOL_H
#   define	TED_DOC_FORMAT_TOOL_H

/************************************************************************/
/*									*/
/*  The different pages in a Format tool:				*/
/*									*/
/************************************************************************/

# define USE_FIND_TOOL 1

typedef enum DocToolSubjectIndex
    {
#   if USE_FIND_TOOL
    TED_DOCtsiFIND,
#   endif

    TED_DOCtsi_COUNT

    } DocToolSubjectIndex;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  TED_DOC_FORMAT_TOOL_H */
