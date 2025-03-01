#   include	"textEncodingConfig.h"

#   include	<stdlib.h>

#   include	"bidiAlgoExplicit.h"

#   include	<appDebugon.h>

struct ExplicitLevel * bidiPopExplicitLevel( struct ExplicitLevel *	el )
    {
    struct ExplicitLevel *	prev= el->el_Prev;

    free( el );

    return prev;
    }

struct ExplicitLevel * bidiPushExplicitLevel(
				struct ExplicitLevel *	prev,
				int			sos,
				int			initiator,
				int			level )
    {
    struct ExplicitLevel *	next= malloc( sizeof(ExplicitLevel) );

    if  ( ! next )
	{ XDEB(next); return (struct ExplicitLevel *)0;	}

    next->el_Prev= prev;
    next->el_X_sos= sos;
    next->el_X_initiator= initiator;
    next->el_X_level= level;

    bidiInitImplicitState( &(next->el_ImplicitState) );

    bidiStartImplicitRun( &(next->el_ImplicitState), sos, initiator, level );

    return next;
    }

