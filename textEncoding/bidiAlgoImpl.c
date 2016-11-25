#   include	"textEncodingConfig.h"

#   include	<stdlib.h>

#   include	"ucdBidiClass.h"
#   include	"bidiAlgoImpl.h"
#   include	"bidiScanner.h"
#   include	"bidiAlgo.h"
#   include	"bidiAlgoImpl.h"

#   include	"bidiDebug.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start processing the data in a paragraph.				*/
/*									*/
/*  After that, you can feed the data or the classification of the data	*/
/*  to the paragraph.							*/
/*									*/
/*  paraInitiator: The embedding of the paragraph. It is determined	*/
/*	by the caller.							*/
/*  gotLevel: A callback for a run. Typically, this is			*/
/*	bidiTreeBuilderAddRun() or a custom callback that performs	*/
/*	additional work besides.					*/
/*  through: A passthrough argument. It can be a BidiTreeBuilder or an	*/
/*	application specific object that holds one.			*/
/*									*/
/************************************************************************/

ParaState * bidiOpenParagraph(	int			paraInitiator,
				const struct BidiScanner *	bs )
    {
    int		paraLevel;
    int		sos;

    ParaState *	ps;

    if  ( paraInitiator == UCDbidi_RLI	)
	{
	paraLevel= 1;
	sos= UCDbidi_R;
	}
    else{
	if  ( paraInitiator != UCDbidi_LRI )
	    { SDEB(ucdBidiClassStr(paraInitiator)); return (ParaState *)0; }

	paraLevel= 0;
	sos= UCDbidi_L;
	}

    ps= malloc( sizeof(ParaState) );
    if  ( ! ps )
	{ XDEB(ps); return (ParaState *)0;	}

    ps->psScanner= bs;

    ps->psParaLevel= paraLevel;
    ps->ps_X10_sos= sos;

    ps->ps_stack[0].rlInitiator= paraInitiator;
    ps->ps_stack[0].rlPrev= -1;
    ps->ps_stack[0].rlNext= -1;
    ps->ps_stack[0].rlHas_X9_Content= 0;
    ps->ps_stack[0].rlPushedFrom= -1;
    ps->ps_stack[0].rlPushedUpto= -1;

    ps->ps_X_level= ps->psParaLevel;
    ps->ps_I_level= ps->ps_X_level;
    ps->ps_X_initiator= paraInitiator;
SDEB(ucdBidiClassStr(ps->ps_X_initiator));
    ps->ps_I_initiator= ps->ps_X_initiator;
SDEB(ucdBidiClassStr(ps->ps_I_initiator));
    ps->ps_stack[ps->ps_X_level].rlInitiator= ps->ps_X_initiator;
    ps->ps_stack[ps->ps_X_level].rlPrev= -1;
    ps->ps_stack[ps->ps_X_level].rlNext= -1;
    ps->ps_stack[ps->ps_X_level].rlHas_X9_Content= 0;
    ps->ps_stack[ps->ps_X_level].rlPushedFrom= -1;
    ps->ps_stack[ps->ps_X_level].rlPushedUpto= -1;

    ps->ps_ImplicitState.is_WNI_LevelHead= -1;
    ps->ps_ImplicitState.is_WNI_CurrentInitiator= paraInitiator;
    ps->ps_ImplicitState.is_WNI_CurrentLevel= ps->psParaLevel;
SDEB(ucdBidiClassStr(ps->ps_ImplicitState.is_WNI_CurrentInitiator));
LDEB(ps->ps_ImplicitState.is_WNI_CurrentLevel);
    ps->psUpto= -1;

    ps->ps_QueuedExplicits= (QueuedExplicit *)0;
    ps->ps_QueuedExplicitCount= 0;

    bidiInitImplicitState( &(ps->ps_ImplicitState) );
    bidiStartImplicitRun( &(ps->ps_ImplicitState),
				ps->ps_X10_sos,
				ps->ps_X_initiator,
				ps->ps_X_level );

    return ps;
    }

static int bidiEmitLevel(	ParaState *	ps,
				int		initiator,
				int		level,
				int		from,
				int		upto )
    {
    const BidiScanner *	bs= ps->psScanner;

    if  ( from < 0 || from >= upto )
	{ LLDEB(from,upto); return -1;	}

    switch( initiator )
	{
	case UCDbidi_PDF:
	    if  ( bs->bsGotRun					&&
		  (*bs->bsGotRun)( bs->bsThrough,
					initiator, level, from, upto )	)
		{ LLLDEB(level,from,upto); return -1;	}
	    break;

	case UCDbidi_LRE:
	case UCDbidi_RLE:
	case UCDbidi_LRO:
	case UCDbidi_RLO:
	case UCDbidi_LRI:
	case UCDbidi_RLI:
	case UCDbidi_FSI:
	case UCDbidi_L:
	case UCDbidi_R:
	case UCDbidi_ON:
	    if  ( bs->bsGotRun					&&
		  (*bs->bsGotRun)( bs->bsThrough,
					initiator, level, from, upto )	)
		{
		SLLLDEB(ucdBidiClassStr(initiator),level,from,upto);
		return -1;
		}
	    break;

	default:
	    CSDEB(initiator,ucdBidiClassStr(initiator)); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Implementation of the actions in the algorithm.			*/
/*									*/
/*  The actions are only driven by the state along the _run_. So we	*/
/*  never consult the actual input.					*/
/*  Two exceptions are made:						*/
/*  -   To leave positions that are controled by rule X9 alone.		*/
/*  -   For sanity checks: Refuse to perform actions that are against	*/
/*	the logic of the algorithm. (Only where checks are easy.)	*/
/*									*/
/************************************************************************/

int bidiStartLevel(	ParaState *	ps,
			int		from,
			int		upto,
			int		initiator )
    {
    ImplicitState *	is= &(ps->ps_ImplicitState);

    int			level= ps->ps_X_level+ 1;
    int			mod2= 0;

    if  ( initiator == UCDbidi_RLE	||
	  initiator == UCDbidi_RLO	||
	  initiator == UCDbidi_RLI	)
	{ mod2= 1;	}

    if  ( level % 2 != mod2 )
	{ level++;	}
    if  ( level <= BIDI_MAX_DEPTH )
	{
	ps->ps_stack[ps->ps_X_level].rlNext= level;

	ps->ps_stack[level].rlPrev= ps->ps_X_level;
	ps->ps_stack[level].rlNext= -1;
	ps->ps_X_level= level;
	ps->ps_X_initiator= initiator;
	ps->ps_stack[ps->ps_X_level].rlInitiator= ps->ps_X_initiator;
	ps->ps_stack[ps->ps_X_level].rlPushedFrom= from;
	ps->ps_stack[ps->ps_X_level].rlPushedUpto= upto;
	ps->ps_stack[ps->ps_X_level].rlHas_X9_Content= 0;

	appDebug( "%s(%3d) PUSH  %s XL=%d OFF=%d..%d\n",
			__FILE__, __LINE__,
			ucdBidiClassStr(initiator), ps->ps_X_level,
			from, upto );
	}

LDEB(is->is_N1_neutralHead);
    if  ( is->is_N1_neutralHead < 0 )
	{
	if  ( bidiEmitLevel( ps, initiator, level, from, upto ) )
	    { LLDEB(is->is_WNI_LevelHead,upto); return -1;	}
	}
    else{
	QueuedExplicit *	fresh;

	fresh= realloc( ps->ps_QueuedExplicits,
		    (ps->ps_QueuedExplicitCount+ 1)* sizeof(QueuedExplicit) );
	if  ( ! fresh )
	    { LXDEB(ps->ps_QueuedExplicitCount,fresh); return -1;	}

	ps->ps_QueuedExplicits= fresh;

	fresh += ps->ps_QueuedExplicitCount++;

	fresh->qeInitiator= initiator;
	fresh->qeFrom= from;
	fresh->qeUpto= upto;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a PDF, PDI or finish an explicit level at the end of the	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

static int bidiPopLevel(	ParaState *	ps,
				int		from,
				int		upto )
    {
    RunLevel *	rl= ps->ps_stack+ ps->ps_X_level;
    int		l= rl->rlPrev;

    if  ( l >= 0 )
	{
	/*  Mountain without a top ? */
	if  ( ! ps->ps_stack[ps->ps_X_level].rlHas_X9_Content )
	    {
	    appDebug( "%s(%3d) REMOVE PUSH %s XL=%d OFF=%d..%d\n",
			__FILE__, __LINE__,
			ucdBidiClassStr(rl->rlInitiator), ps->ps_X_level,
			rl->rlPushedFrom, rl->rlPushedUpto );

	    rl->rlPushedFrom= -1;
	    rl->rlPushedUpto= -1;

	    appDebug( "%s(%3d) REMOVE POP  %s XL=%d OFF=%d..%d\n",
			__FILE__, __LINE__,
			ucdBidiClassStr(rl->rlInitiator), ps->ps_X_level,
			from, upto );
	    }
	else{
	    appDebug( "%s(%3d) POP   %s XL=%d OFF=%d..%d\n",
			__FILE__, __LINE__,
			ucdBidiClassStr(rl->rlInitiator), ps->ps_X_level,
			from, upto );
	    }

	ps->ps_X_level= l;
	ps->ps_stack[ps->ps_X_level].rlNext= -1;
	ps->ps_X_initiator= ps->ps_stack[ps->ps_X_level].rlInitiator;
	}

    return 0;
    }

int bidiFinishEmbedding(	ParaState *	ps,
				int		from,
				int		upto )
    {
    RunLevel *	rl= ps->ps_stack+ ps->ps_X_level;
    int		l= rl->rlPrev;

    if  ( l >= 0					&&
	  ( rl->rlInitiator == UCDbidi_LRE	||
	    rl->rlInitiator == UCDbidi_RLE	||
	    rl->rlInitiator == UCDbidi_LRO	||
	    rl->rlInitiator == UCDbidi_RLO	)	)
	{
	return bidiPopLevel( ps, from, upto );
	}

    return 0;
    }

int bidiFinishIsolate(		ParaState *	ps,
				int		from,
				int		upto )
    {
    RunLevel *	rl= ps->ps_stack+ ps->ps_X_level;
    int		l= rl->rlPrev;

    while  ( l >= 0					&&
	     ( rl->rlInitiator == UCDbidi_RLE	||
	       rl->rlInitiator == UCDbidi_LRO	||
	       rl->rlInitiator == UCDbidi_RLO	)	)
	{
	if  ( bidiPopLevel( ps, from, upto ) )
	    { LLDEB(from,upto); return -1;	}

	rl= ps->ps_stack+ ps->ps_X_level;
	l= rl->rlPrev;
	}

    if  ( l >= 0					&&
	  ( rl->rlInitiator == UCDbidi_LRI	||
	    rl->rlInitiator == UCDbidi_RLI	||
	    rl->rlInitiator == UCDbidi_FSI	)	)
	{
	return bidiPopLevel( ps, from, upto );
	}

    return 0;
    }

static int bidi_WNI_FlushLevel(	ParaState *	ps,
				int		upto )
    {
    ImplicitState *	is= &(ps->ps_ImplicitState);

    if  ( is->is_WNI_LevelHead < upto )
	{
	const int 	ci= is->is_WNI_CurrentInitiator;
	const int	cl= is->is_WNI_CurrentLevel;

	EMIT_DEB(ci,cl,is->is_WNI_LevelHead,upto);
	if  ( bidiEmitLevel( ps, ci, cl, is->is_WNI_LevelHead, upto ) )
	    { LLDEB(is->is_WNI_LevelHead,upto); return -1;	}
	}

    is->is_WNI_LevelHead= -1;
    is->is_WNI_CurrentInitiator= -1;
    is->is_WNI_CurrentLevel= -1;

    return 0;
    }

static int bidi_X10_eor(	ParaState *	ps,
				int		next )
    {
    int		level;

    if  ( ps->ps_I_level > next )
	{ level= ps->ps_I_level;	}
    else{ level= next;		}

    return ( level % 2 == 0 )?UCDbidi_L:UCDbidi_R;
    }

int bidi_X9_keep(	ParaState *	ps,
			int		from,
			int		upto )
    {
    ImplicitState *	is= &(ps->ps_ImplicitState);

    if  ( ps->ps_I_level != ps->ps_X_level )
	{
	int			eor= bidi_X10_eor( ps, ps->ps_X_level );
	const BidiScanner *	bs= ps->psScanner;

	if  ( bidiFlushRun( bs, is, eor, from ) )
	    { LLDEB(from,upto); return -1;	}

	ps->ps_X10_sos= eor;

	bidiInitImplicitState( is );
	bidiStartImplicitRun( is, ps->ps_X10_sos, ps->ps_X_initiator, ps->ps_X_level );
	}

    ps->ps_I_level= ps->ps_X_level;
    ps->ps_I_initiator= ps->ps_X_initiator;
    ps->ps_stack[ps->ps_I_level].rlHas_X9_Content= 1;

    if  ( is->is_WNI_head < 0 )
	{ is->is_WNI_head= from;	}
    if  ( is->is_WNI_upto < upto )
	{ is->is_WNI_upto= upto;	}

    return 0;
    }

/************************************************************************/

/************************************************************************/
/*									*/
/*  Finish the paragraph.						*/
/*									*/
/*  2)  Determine the eor.						*/
/*  3)  Flush the W4,W5,W6 administration. We do not expect any number	*/
/*	after the end of the paragraph. This also flushes any postponed	*/
/*	explicit controls.						*/
/*  4)  Handle the eor as a strong character.				*/
/*  5)  Flush the remaining explicit controls as removed.		*/
/*									*/
/************************************************************************/

int bidiFinishParagraph(	ParaState *	ps,
				int		offset )
    {
    ImplicitState *	is= &(ps->ps_ImplicitState);
    int			eor= bidi_X10_eor( ps, ps->ps_X_level );
    const BidiScanner *	bs= ps->psScanner;

    ps->psUpto= offset;

SLLDEB("FINISH",ps->ps_X_level,ps->ps_I_level);

    /*  2  */
    if  ( bidiFlushRun( bs, is, eor, offset ) )
	{ LDEB(offset); return -1;	}

    /*  6  */
LDEB(6);
    if  ( is->is_WNI_LevelHead >= 0				&&
	  bidi_WNI_FlushLevel( ps, offset )			)
	{ LDEB(offset); return -1;	}

    return 0;
    }

