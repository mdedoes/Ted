#   ifndef		BIDI_ALGO_IMPLICIT_H
#   define		BIDI_ALGO_IMPLICIT_H

struct BidiScanner;

/************************************************************************/
/*									*/
/*  Implementation of the implicit processing in the 'Unicode		*/
/*  Bidirectional Algorithm' as described in				*/
/*  http://unicode.org/reports/tr9/ Revision 31 (2014-06-05)		*/
/*									*/
/*  The comments do not explain the algorithm, but only explain the	*/
/*  implementation.							*/
/*									*/
/************************************************************************/

typedef struct ImplicitState
    {
			/**
			 *  Initiator that started the implicit run
			 */
    unsigned char	is_I_initiator;
			/**
			 *  The (implicit) level of this run
			 */
    unsigned char	is_I_level;

			/**
			 *  The replacement for NSM's following W1.
			 */
    unsigned char	is_W1_replacement;

			/**
			 *  The replacement for ENs according to rule W2.
			 *  Possible values are EN (Do not replace) and AN
			 *  (Replace with an AN.)
			 */
    unsigned char	is_W2_replacement;

    unsigned char	is_W4_prevNum;
    unsigned char	is_W4_prevSep;
    int			is_W4_sepPos;

    unsigned char	is_W5_prevNum;
    int			is_W5_termHead;

    unsigned char	is_W7_replacement;
    unsigned char	is_W4_W7_replacement;
    unsigned char	is_W5_W7_replacement;

			/**
			 *  The preceding strong class(L,R,AL) encountered 
			 *  on the way. It is used to implement N1: Neutrals 
			 *  take the direction of the surrounding strong 
			 *  text of both sides have the same direction.
			 */
    unsigned char	is_N1_prevStrong;

			/**
			 *  The position of the first neutral character in 
			 *  a sequence of neutrals. (Or -1 of none is found 
			 *  yet.) It is used to give them a direction once 
			 *  the direction on both sides of the stretch is 
			 *  known. (N1 and N2)
			 *
			 *  is_N1_neutralHead is reset every time a strong 
			 *  character is encountered (Or a character is 
			 *  rewritten to a strong one.)
			 */
    int			is_N1_neutralHead;

			/**
			 *  The (explicit) embedding level. It is used to 
			 *  determine the direction of a series of neutrals 
			 *  with different directions on both sides.
			 */
    unsigned char	is_N2_e;

			/**
			 *  The first position in the run that has passed 
			 *  rule X9 and that will be used by the W*,N*,I* 
			 *  rules. (-1 in an empty run)
			 */
    int			is_WNI_head;
			/**
			 *  Past the last position in the run that has passed 
			 *  rule X9 and that will be used by the W*,N*,I* 
			 *  rules. (-1 in an empty run)
			 */
    int			is_WNI_upto;

			/**
			 *  The head of a range of symbols at the same 
			 *  level. We are only concerned with symbols that 
			 *  pass rule X9.
			 */
    int			is_WNI_LevelHead;

			/**
			 *  The initiator of a range of symbols at the same 
			 *  level. We are only concerned with symbols that 
			 *  pass rule X9.
			 */
    int			is_WNI_CurrentInitiator;

			/**
			 *  The level of a range of symbols at the same 
			 *  level. We are only concerned with symbols that 
			 *  pass rule X9.
			 */
    int			is_WNI_CurrentLevel;
    } ImplicitState;

/************************************************************************/

extern void bidiStartImplicitRun(	struct ImplicitState *		is,
					int				sos,
					int				iInitr,
					int				xLvl );

extern int bidiFlushRun(		const struct BidiScanner *	bs,
					struct ImplicitState *		is,
					int				eor,
					int				off );

extern void bidiInitImplicitState(	struct ImplicitState *		is );

#   endif	/*	BIDI_ALGO_IMPLICIT_H	*/
