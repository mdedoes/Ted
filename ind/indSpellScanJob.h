#   ifndef	IND_SPEL_SCAN_JOB_H			/*  {{	*/
#   define	IND_SPEL_SCAN_JOB_H

struct SpellDictionary;
struct SpellChecker;

#   include	"spellComplain.h"

/************************************************************************/
/*									*/
/*  A possible 'word' that is collected during the scan of the input.	*/
/*									*/
/************************************************************************/
#   define	FORM_MAX	60

typedef struct PossibleWord
    {
    int				pwStartAt;
    int				pwInsertionPoint;
    int				pwRejectedAt;
    int				pwAcceptedAt;
    char			pwForm[FORM_MAX+2];

    struct PossibleWord *	pwNext;
    } PossibleWord;

typedef struct SpellScanJob
    {
			/**
			 * Position upto which everything has been accepted 
			 * in the current paragraph or in the span with a 
			 * certain language.
			 */
    int			ssjAcceptedPos;
			/**
			 * A list of 'Possible Words'. IE: prefixes of 
			 * entries in the dictionary that still are a 
			 * candidate to be accepted by the checker
			 */
    PossibleWord *	ssjPossibleWords;
			/**
			 * The number of candidates in the ssjPossibleWords list
			 */
    int			ssjPossibleWordCount;

    struct SpellDictionary *	ssjDictionary;
    struct SpellChecker *	ssjChecker;

			/**
			 * To pass error messages back to the user of the caller
			 */
    SpellComplain	ssjComplain;
    void *		ssjThrough;

			/* UNUSED
			 * Parsed regular expression to skip to the first word 
			 * in the paragraph or an independent span inside
			 * the paragraph. This is used when the search starts 
			 * from the head of the paragraph.
    void *		ssjFirstWordExpr;
			 */
			/**
			 * Parsed regular expression to skip to the next word
			 * in the paragraph. This is used when the search 
			 * starts somewhere in the middle of the paragraph.
			 */
    void *		ssjToWordExpr;
			/**
			 * Parsed regular expression to find the word it the 
			 * current position and the chracters that follow.
			 * This is used to delimit the successive words in 
			 * the paragraph.
			 */
    void *		ssjNextWordExpr;

			/**
			 * Locale tag that overrides the dictionary.
			 */
    const char *	ssjOverrideLocaleTag;
    } SpellScanJob;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void	indLogPossibilities(	SpellScanJob *		ssj );

extern int	indNewPossibility(	SpellScanJob *		ssj,
					int			position );

extern void indAddCharacterToPossibilities(
					SpellScanJob *		ssj,
					int			c );

extern void indRejectPossibilities(	SpellScanJob *		ssj );

extern PossibleWord * indMaximalPossibility(	SpellScanJob *	ssj );

extern void indInitSpellScanJob(	SpellScanJob *	ssj );
extern void indCleanSpellScanJob(	SpellScanJob *	ssj );

extern int indStartSpellScanJob(	SpellScanJob *	ssj,
					int		acceptedPos,
					const char *	localeTag );

extern int indCountPossibilities(
			SpellScanJob *			ssj,
			struct SpellDictionary *	sd,
			int				position,
			int				rejectPrefices );

extern int indSkipToWord(	SpellScanJob *			ssj,
				int *				pHere,
				const char *			paraStr,
				int				here,
				int				upto );

extern int indFindNextWord(	SpellScanJob *			ssj,
				int *				pHead,
				int *				pTail,
				int *				pNext,
				const char *			paraStr,
				int				here,
				int				upto );

extern int spellCheckWord(	int *				pFoundStart,
				int *				pFoundLength,
				struct SpellScanJob *		ssj,
				const char *			paraStr,
				int				head,
				int				tail,
				int				next,
				int				atEnd );

#   endif /*	IND_SPEL_SCAN_JOB_H			    }}	*/
