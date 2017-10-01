/************************************************************************/
/*									*/
/*  Reading of Rtf redo/undo files. (Various utility functions.)	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReadTrace.h"

#   include	<appDebugon.h>

void docRtfInitRtfTraceReader(	RtfTraceReader *	rtr )
    {
    rtr->rtrTraceReadWhat= 0;
    rtr->rtrTraceCommand= -1;
    rtr->rtrTraceSelectionPosition= SELposTAIL;
    rtr->rtrTraceFieldKind= -1;
    rtr->rtrTraceInProps= 0;

    docInitSelectionScope( &(rtr->rtrTraceOldSelectionScope) );
    docInitEditRange( &(rtr->rtrTraceOldRange) );
    rtr->rtrTraceOldCol0= -1;
    rtr->rtrTraceOldCol1= -1;
    rtr->rtrTraceOldPage= -1;
    rtr->rtrTraceOldColumn= -1;

    docInitSelectionScope( &(rtr->rtrTraceNewSelectionScope) );
    docInitEditRange( &(rtr->rtrTraceNewRange) );
    rtr->rtrTraceNewCol0= -1;
    rtr->rtrTraceNewCol1= -1;
    rtr->rtrTraceNewPage= -1;
    rtr->rtrTraceNewColumn= -1;
    }


    /*
    int				rtrTracePropLevel;
    */

