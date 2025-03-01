#   ifndef		BIDI_ALGO_EXPLICIT_H
#   define		BIDI_ALGO_EXPLICIT_H

#   include	"bidiAlgoImplicit.h"

/************************************************************************/
/*									*/
/*  Implementation of the explicit processing in the 'Unicode		*/
/*  Bidirectional Algorithm' as described in				*/
/*  http://unicode.org/reports/tr9/ Revision 31 (2014-06-05)		*/
/*									*/
/*  The comments do not explain the algorithm, but only explain the	*/
/*  implementation.							*/
/*									*/
/*  Actually, we implement is a simplification of the tr9 algorithm:	*/
/*  Explicit controls are not removed and border cases where explicit	*/
/*  runs are adjacent, the output may actually differ.			*/
/*									*/
/************************************************************************/

typedef struct ExplicitLevel
    {
				/**
			 	*  The sos for this level:
			 	*/
    unsigned char		el_X_sos;

				/**
			 	*  The initiator of this level
			 	*/
    unsigned char		el_X_initiator;

				/**
			 	*  The explicit level
			 	*/
    int				el_X_level;

				/**
			 	*  Work area for implicit processing.
			 	*/
    ImplicitState		el_ImplicitState;

    struct ExplicitLevel *	el_Prev;
    } ExplicitLevel;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern struct ExplicitLevel * bidiPushExplicitLevel(
				struct ExplicitLevel *	prev,
				int			sos,
				int			initiator,
				int			level );

extern struct ExplicitLevel * bidiPopExplicitLevel(
				struct ExplicitLevel *	current );

#   endif	/*	BIDI_ALGO_EXPLICIT_H	*/
