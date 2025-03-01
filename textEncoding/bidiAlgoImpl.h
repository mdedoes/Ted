#   ifndef		BIDI_ALGO_IMPL_H
#   define		BIDI_ALGO_IMPL_H

#   include	"bidiAlgoImplicit.h"

/************************************************************************/
/*									*/
/*  Implementation of the 'Unicode Bidirectional Algorithm' as		*/
/*  described in http://unicode.org/reports/tr9/ Revision 31		*/
/*  (2014-06-05)							*/
/*									*/
/*  The comments do not explain the algorithm, but only explain the	*/
/*  implementation.							*/
/*									*/
/************************************************************************/

/**
 *  A level on the stack that follows the nesting of RLE, LRE, RLO, LRO and PDF.
 *
 *  On one hand the stack serves to keep track of the nesting and the 
 *  associated isolation/embedding/overrides.
 *  the level callbacks in a monotonous way: The offsets in the calls follow
 *  the numerical order of the integers. (The integers are some representation 
 *  of the offset in the input in reading=storage order.) This is not true for 
 *  BNs and explicit controls that are refused: The REMOVED and REJECTED calls
 *  are issued as soon as the symbol is encountered. Usually that is long
 *  before the embedding level of the surrounding symbols is known.
 *
 *  To deliver the callbacks in a monotonous way, we have to postpone 
 *  the emission of RLE, LRE, RLO, LRO and PDF until we know the 
 *  implicit embedding level of the characters that precede them. While 
 *  doing so, we skip mountains without a top. (No characters that are 
 *  kept by X9 between the push and the pop.) But we do descend into 
 *  valleys without a bottom. (No characters that are kept by X9 between the 
 *  pop and the push.)
 */
typedef struct RunLevel
    {
			/**
			 *  Offset to the previous level. (-1) at the 
			 *  bottom of the stack.
			 */
    int			rlPrev;
			/**
			 *  Offset to the next level. (-1) at the 
			 *  top of the stack.
			 */
    int			rlNext;
			/**
			 *  The Initiator of this level.
			 *  LRI, RLI, FSI (Isolate)
			 *  LRE, RLE, (Embedding)
			 *  LRO, RLO. (Override)
			 */
    unsigned char	rlInitiator;

			/**
			 *  Set when actual content that is not removed 
			 *  by X9 is discovered.
			 */
    unsigned char	rlHas_X9_Content;

			/**
			 *  Offset where this explicit level was pushed.
			 *  Is reset to -1 once the actual code is emitted.
			 *  We accomodate for an upto because the explicit 
			 *  instruction might use space in the consumer 
			 *  buffer.
			 */
    int			rlPushedFrom;
    int			rlPushedUpto;
    } RunLevel;

typedef struct QueuedExplicit
    {
    unsigned char	qeInitiator;
    int			qeFrom;
    int			qeUpto;
    } QueuedExplicit;

# define BIDI_MAX_DEPTH		125  /* BD2: Maximum explicit depth */

typedef struct ParaState
    {
			/**
			 *  Embedding level of the paragraph as a whole.
			 */
    int			psParaLevel;

			/**
			 *  Level as it is set by LRE, RLE, LRO, RLO, RLI,
			 *  LRI, FSI. It is ignored in empty runs. This highly 
			 *  complicates the implementation of the bidi 
			 *  algorithm.
			 */
    int			ps_X_level;

			/**
			 *  Explicit initiator as it is pushed by LRE, RLE,
			 *  LRO, RLO, LRI, RLI, FSI.
			 */
    int			ps_X_initiator;

			/**
			 *  The current level of symbols that have been 
			 *  accepted following rule X9. It runs behind 
			 *  ps_X_level. Every time the values differ, a new 
			 *  explicit run starts.
			 */
    int			ps_I_level;

			/**
			 *  Implicit initiator as it is activated by a symbol 
			 *  that is kept by X9
			 */
    int			ps_I_initiator;

			/**
			 *  The sos value at the end of the run following 
			 *  the definition in X10. Because X9 has just 
			 *  removed the explicit controls and BN, it is not 
			 *  directly controlled by the nesting level of the 
			 *  explicit tree, but rather by the explicit level of 
			 *  the last symbol that was kept by X9.
			 */
    unsigned char	ps_X10_sos;

			/**
			 *  The state for implicit processing.
			 */
    ImplicitState	ps_ImplicitState;

			/**
			 *  Not negative finishing the paragraph
			 */
    int			psUpto;

    RunLevel		ps_stack[BIDI_MAX_DEPTH+1];

    QueuedExplicit *	ps_QueuedExplicits;
    int			ps_QueuedExplicitCount;

    const struct BidiScanner *	psScanner;
    } ParaState;

/************************************************************************/

extern int bidiStartLevel(		ParaState *	ps,
					int		from,
					int		upto,
					int		initiator );

extern int bidiFinishEmbedding(		ParaState *	ps,
					int		from,
					int		upto );

extern int bidiFinishIsolate(		ParaState *	ps,
					int		from,
					int		upto );

extern int bidi_X9_keep(		ParaState *	ps,
					int		from,
					int		upto );

#   endif	/*	BIDI_ALGO_IMPL_H	*/
