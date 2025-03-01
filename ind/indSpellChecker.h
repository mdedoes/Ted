/************************************************************************/
/*									*/
/*  Spell checking interface.						*/
/*									*/
/************************************************************************/

#   ifndef	IND_SPELL_CHECHER_H
#   define	IND_SPELL_CHECHER_H

#   include	"indGuessList.h"
#   include	"spellComplain.h"
#   include	<utilMemoryBuffer.h>

struct SpellScanJob;
struct SpellDictionary;
struct SpellChecker;

typedef int (*SpellOpenDictionary)(	struct SpellDictionary * sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc );

typedef void (*SpellCloseDictionary)(	const struct SpellChecker *	sc,
					struct SpellDictionary *	sd );

typedef int (*SpellDictionaryClassifyWord)( struct SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef int (*SpellDictionaryGuessWord)( IndGuessList *		igl,
					struct SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef int (*SpellDictionaryCheckWord)(
				int *				pFoundStart,
				int *				pFoundLength,
				struct SpellScanJob *		ssj,
				struct SpellDictionary *	sd,
				const char *			paraStr,
				int				head,
				int				tail,
				int				next,
				int				atEnd );

typedef struct SpellDictionary
    {
			/**
			 * http://tools.ietf.org/html/rfc5646 conformant 
			 * locale tag.
			 */
    char *		sdLocaleTag;
			/**
			 * The locale tag, translated to a string aimed 
			 * at people.
			 */
    char *		sdLocaleLabel;
			/**
			 * The file holding the dictionary. (If known)
			 */
    MemoryBuffer	sdFileName;

			/**
			 *  The backend implementation.
			 *  (EnchantDict in the enchant case.)
			 *  (IndSpellDictionary in the ind case.)
			 */
    void *		sdBackend;

			/**
			 *  Open the dictionary: Called before any action 
			 *  that potentially uses the backend.
			 */
    SpellOpenDictionary	sdOpenDictionary;

			/**
			 *  Close the dictionary: Called if the dictionary 
			 *  is no longer actively needed.
			 */
    SpellCloseDictionary sdCloseDictionary;

			/**
			 *  Persistently learn a word: I.E: It is accepted
			 *  from now on.
			 */
    SpellDictionaryClassifyWord	sdLearnWord;

			/**
			 *  Persistently forget a word: I.E: It is no longer
			 *  accepted from now on.
			 */
    SpellDictionaryClassifyWord	sdForgetWord;

			/**
			 *  Temporaily learn a word: I.E: It is accepted
			 *  until the dictionary is closed.
			 */
    SpellDictionaryClassifyWord	sdIgnoreWord;

			/**
			 *  Sugest alternatives for a word.
			 */
    SpellDictionaryGuessWord	sdGuessWord;

			/**
			 *  Check a word as part of a contiguous list of 
			 *  words.
			 */
    SpellDictionaryCheckWord	sdCheckWord;
    } SpellDictionary;

struct SpellChecker;

typedef int (*SpellCheckerSetup)(	struct SpellChecker * 	sc,
					SpellComplain		complain,
					void *			through );

typedef int (*SpellCheckerFindNext)(	void *			target,
					MemoryBuffer *		mbFound,
					struct SpellScanJob *	ssj );

typedef int (*SpellCheckerCorrect)(	void *			target,
					const char * 		correction );

typedef void (*SpellCheckerDoNotCheck)(	void *			target );

typedef void (*SpellCheckerSetLocale)(	void *			target,
					const char * 		localeId );

typedef void (*SpellCloseChecker)(	struct SpellChecker *	sc );

typedef struct SpellChecker
    {
    SpellCheckerSetup		scSetup;
    SpellCheckerFindNext	scFindNext;
				/**
				 *  Replace the current selection in 
				 *  the document with the given word as 
				 *  a spell checking correction.
				 */
    SpellCheckerCorrect		scCorrect;
				/**
				 *  Remember (inside the document) 
				 *  that the current selection is 
				 *  excludes from spell checking
				 */
    SpellCheckerDoNotCheck	scDoNotCheck;
				/**
				 *  Remember (inside the document) 
				 *  that the current selection is 
				 *  in a particular locale.
				 */
    SpellCheckerSetLocale	scSetLocale;

    MemoryBuffer		scPrivateDirectory;
    MemoryBuffer		scSystemDirectory;

    void *			scTarget;
				/**
				 *  The backend implementation.
				 *  (EnchantBroker in the echhant case.)
				 */
    void *			scBackend;

    SpellDictionary *		scDictionaries;
    int				scDictionaryCount;

    SpellCloseChecker		scCloseChecker;
    } SpellChecker;

typedef enum SpellCheckError
    {
    SCerrNO_ERROR= 0,
    SCerrDIR_NO_ACCESS,
    SCerrDICT_NO_ACCESS,
    SCerrDIR_DOES_NOT_EXIST,
    SCerrDIR_HAS_NO_DICTS,
    SCerrPRIVATE_DIR_NOT_MADE,
    SCerrPRIVATE_DICT_NO_ACCESS,
    SCerrPRIVATE_DICT_WRONG_FORMAT,

    SCerr_COUNT
    } SpellCheckError;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

extern void indInitSpellDictionary(	SpellDictionary *	sd );
extern void indInitSpellChecker(	SpellChecker *		sc );
extern void indCleanSpellChecker(	SpellChecker *		sc );

extern void indCleanDictionary(		SpellChecker *		sc,
					SpellDictionary *	sd );

extern int indSpellDictionarySetLocaleLabel(
					SpellDictionary *	sd,
					const char *		localeLabel );

extern SpellDictionary * indSpellCheckerAddDictionary(
					SpellChecker *		sc,
					const char *		lang_tag,
					const MemoryBuffer *	filename );

extern int spellGuessWord(	IndGuessList *			igl,
				struct SpellDictionary *	sd,
				SpellComplain			complain,
				void *				through,
				const struct SpellChecker *	sc,
				const char *			word );

extern int spellIgnoreWord(	struct SpellDictionary *	sd,
				SpellComplain			complain,
				void *				through,
				const struct SpellChecker *	sc,
				const char *			word );

extern int spellLearnWord(	struct SpellDictionary *	sd,
				SpellComplain			complain,
				void *				through,
				const struct SpellChecker *	sc,
				const char *			word );

extern int spellForgetWord(	struct SpellDictionary *	sd,
				SpellComplain			complain,
				void *				through,
				const struct SpellChecker *	sc,
				const char *			word );

extern int spellSetup(		struct SpellChecker *		sc,
				SpellComplain			complain,
				void *				through );

extern int spellSetupEnchant(	struct SpellChecker *		sc,
				SpellComplain			complain,
				void *				through );

extern int spellSetupInd(	SpellChecker *			sc,
				SpellComplain			complain,
				int				complain_none,
				void *				through );

extern int spellFindDictionary( const struct SpellDictionary *	sd,
			int					count,
			const char *				localeTag );
extern int spellGetDictionary( const struct SpellDictionary *	sd,
			int					count,
			const char *				localeTag );

#   endif	/* IND_SPELL_CHECHER_H */
