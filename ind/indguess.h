#   ifndef	IND_GUESS_H
#   define	IND_GUESS_H

#   include	"indlocal.h"
#   include	"indSpellGuessContext.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int indINDguess(		IND *				ind,
				const unsigned short *		ucods,
				int				len,
				SpellGuessContext *		sgc,
				int				how );

extern int indWRDguess(		IND *				ind,
				unsigned short *		ucods,
				int				ulen,
				SpellGuessContext *		sgc );


#   endif	/* IND_GUESS_H */
