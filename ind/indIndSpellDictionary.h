/************************************************************************/
/*									*/
/*  Spell checking interface.						*/
/*									*/
/************************************************************************/

#   ifndef	IND_IND_SPELL_DICTIONARY_H
#   define	IND_IND_SPELL_DICTIONARY_H

struct IND;

typedef struct IndSpellDictionary
    {
			/**
			 *  The DFA as stored on disk
			 */
    struct IND *	isdStaticInd;
			/**
			 *  The DFA holding forgotten words
			 */
    struct IND *	isdForgotInd;
			/**
			 *  The DFA holding learnt words
			 */
    struct IND *	isdLearntInd;
    } IndSpellDictionary;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/* IND_IND_SPELL_DICTIONARY_H */
