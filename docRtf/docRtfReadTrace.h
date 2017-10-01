/************************************************************************/
/*									*/
/*  Reading the RTF undo/redo trace.					*/
/*									*/
/************************************************************************/

#   ifndef		RTF_READ_TRACE_H
#   define		RTF_READ_TRACE_H

#   include	<docSelectionScope.h>
#   include	<docEditRange.h>

typedef struct RtfTraceReader
    {
				/**
				 * Used for reading Undo/Redo traces.
				 * -1: old; 1: new
				 */
    int				rtrTraceReadWhat;
    int				rtrTraceCommand;
    int				rtrTraceSelectionPosition;
    int				rtrTraceFieldKind;
    int				rtrTraceInProps;

    SelectionScope		rtrTraceOldSelectionScope;
    EditRange			rtrTraceOldRange;
    int				rtrTraceOldCol0;
    int				rtrTraceOldCol1;
    int				rtrTraceOldPage;
    int				rtrTraceOldColumn;

    int				rtrTracePropLevel;

    SelectionScope		rtrTraceNewSelectionScope;
    EditRange			rtrTraceNewRange;
    int				rtrTraceNewCol0;
    int				rtrTraceNewCol1;
    int				rtrTraceNewPage;
    int				rtrTraceNewColumn;
    } RtfTraceReader;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfInitRtfTraceReader(	RtfTraceReader *	rtr );

#   endif	/*	RTF_READ_TRACE_H	*/
