#   ifndef	IND_SPELL_GUESS_CONTEXT_H		/*  {{	*/
#   define	IND_SPELL_GUESS_CONTEXT_H

#   include	"indGuessList.h"

struct SpellDictionary;

typedef struct SpellGuessContext
    {
    IndGuessList *		sgcGuessList;
    struct SpellDictionary *	sgcDictionary;
    } SpellGuessContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int indGuess(	void *				voidind,
			const char *			word,
			SpellGuessContext *		sgc,
			int				how );

extern int indGuessWord(    void *				voidind,
			    const char *			word,
			    SpellGuessContext *			sgc );

extern void indInitSpellGuessContext(	SpellGuessContext *	sgc,
					IndGuessList *		igl,
					struct SpellDictionary * sd );

extern int indCollectGuesses(	IndGuessList *			igl,
				struct SpellDictionary *	sd,
				const char *			word );

#   endif /*	IND_SPELL_GUESS_CONTEXT_H		    }}	*/
