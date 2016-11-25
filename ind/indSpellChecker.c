/************************************************************************/
/*									*/
/*  Obtain the list of dictionaries.					*/
/*									*/
/*  The initialistation of the character classification should be	*/
/*  based on some kind of knowledge, NOT on the name of the language.	*/
/*									*/
/************************************************************************/

#   include	<string.h>
#   include	<stdlib.h>

#   include	"indSpellChecker.h"
#   include	"indSpellScanJob.h"
#   include	<textLocaleTag.h>

#   include	<appDebugon.h>

/************************************************************************/

void indInitSpellDictionary(	SpellDictionary *	sd )
    {
    sd->sdLocaleTag= (char *)0;
    sd->sdLocaleLabel= (char *)0;
    utilInitMemoryBuffer( &(sd->sdFileName) );

    sd->sdBackend= (void *)0;

    sd->sdOpenDictionary= (SpellOpenDictionary)0;
    sd->sdCloseDictionary= (SpellCloseDictionary)0;
    sd->sdLearnWord= (SpellDictionaryClassifyWord)0;
    sd->sdForgetWord= (SpellDictionaryClassifyWord)0;
    sd->sdIgnoreWord= (SpellDictionaryClassifyWord)0;
    sd->sdGuessWord= (SpellDictionaryGuessWord)0;
    sd->sdCheckWord= (SpellDictionaryCheckWord)0;
    }

void indInitSpellChecker(	SpellChecker *		sc )
    {
    sc->scSetup= (SpellCheckerSetup)0;
    sc->scFindNext= (SpellCheckerFindNext)0;
    sc->scCorrect= (SpellCheckerCorrect)0;
    sc->scDoNotCheck= (SpellCheckerDoNotCheck)0;
    sc->scSetLocale= (SpellCheckerSetLocale)0;

    utilInitMemoryBuffer( &(sc->scPrivateDirectory) );
    utilInitMemoryBuffer( &(sc->scSystemDirectory) );

    sc->scTarget= (void *)0;
    sc->scBackend= (void *)0;

    sc->scDictionaries= (SpellDictionary *)0;
    sc->scDictionaryCount= 0;

    sc->scCloseChecker= (SpellCloseChecker)0;
    }

void indCleanDictionary(	SpellChecker *		sc,
				SpellDictionary *	sd )
    {
    if  ( sd->sdCloseDictionary )
	{ (*sd->sdCloseDictionary)( sc, sd );	}

    if  ( sd->sdLocaleTag )
	{ free( sd->sdLocaleTag );	}
    if  ( sd->sdLocaleLabel )
	{ free( sd->sdLocaleLabel );	}
    utilCleanMemoryBuffer( &(sd->sdFileName) );

    return;
    }

void indCleanSpellChecker(	SpellChecker *		sc )
    {
    int			i;
    SpellDictionary *	sd;

    sd= sc->scDictionaries;
    for ( i= 0; i < sc->scDictionaryCount; sd++, i++ )
	{ indCleanDictionary( sc, sd );	}

    if  ( sc->scDictionaries )
	{ free( sc->scDictionaries );	}

    if  ( sc->scCloseChecker )
	{ (*sc->scCloseChecker)( sc );	}

    utilCleanMemoryBuffer( &(sc->scPrivateDirectory) );
    utilCleanMemoryBuffer( &(sc->scSystemDirectory) );

    return;
    }

int indSpellDictionarySetLocaleLabel(	SpellDictionary *	sd,
					const char *		localeLabel )
    {
    char * ll= (char *)0;

    if  ( localeLabel )
	{
	ll= strdup( localeLabel );
	if  ( ! ll )
	    { XDEB(ll); return -1;	}
	}

    if  ( sd->sdLocaleLabel )
	{ free( sd->sdLocaleLabel );	}

    sd->sdLocaleLabel= ll;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a dictionary to the checker.					*/
/*									*/
/************************************************************************/

SpellDictionary * indSpellCheckerAddDictionary(
					SpellChecker *		sc,
					const char *		lang_tag,
					const MemoryBuffer *	filename )
    {
    SpellDictionary *		fresh;

    fresh= (SpellDictionary *)realloc( sc->scDictionaries,
			(sc->scDictionaryCount+ 1)*sizeof(SpellDictionary) );
    if  ( ! fresh )
	{ XDEB(fresh); return (SpellDictionary *)0;	}
    sc->scDictionaries= fresh;

    fresh += sc->scDictionaryCount;

    indInitSpellDictionary( fresh );
    fresh->sdLocaleTag= strdup( lang_tag );
    fresh->sdLocaleLabel= strdup( lang_tag );

    if  ( utilCopyMemoryBuffer( &(fresh->sdFileName), filename ) )
	{ LDEB(1);	}

    sc->scDictionaryCount++;

    return fresh;
    }

int spellFindDictionary( const SpellDictionary *	sd,
			int				count,
			const char *			localeTag )
    {
    int		best= 0;
    int		found= -1;

    int		dict;

    for ( dict= 0; dict < count; sd++, dict++ )
	{
	int	score= textMatchLocaleTags( localeTag, sd->sdLocaleTag );

	if  ( score > best )
	    { found= dict; best= score;	}
	}

    return found;
    }

int spellGetDictionary( const SpellDictionary *		sd,
			int				count,
			const char *			localeTag )
    {
    int		dict;

    for ( dict= 0; dict < count; sd++, dict++ )
	{
	if  ( ! strcmp( localeTag, sd->sdLocaleTag ) )
	    { return dict;	}
	}

    return -1;
    }

int spellGuessWord( IndGuessList *		igl,
		    struct SpellDictionary *	sd,
		    SpellComplain		complain,
		    void *			through,
		    const struct SpellChecker * sc,
		    const char *		word )
    {
    if  ( ! sd )
	{ XDEB(sd); return -1;	}
    if  ( ! sd->sdGuessWord )
	{ XDEB(sd->sdGuessWord); return -1;	}
    if  ( ! sd->sdOpenDictionary )
	{ XDEB(sd->sdOpenDictionary); return -1;	}

    if  ( ! sd->sdBackend					&&
	  (*sd->sdOpenDictionary)( sd, complain, through, sc )	)
	{ return -1;	}

    indCleanGuessList( igl );
    indInitGuessList( igl );

    return (*sd->sdGuessWord)( igl, sd, complain, through, sc, word );
    }

int spellIgnoreWord(	struct SpellDictionary *	sd,
			SpellComplain			complain,
			void *				through,
			const struct SpellChecker *	sc,
			const char *			word )
    {
    if  ( ! sd )
	{ XDEB(sd); return -1;	}
    if  ( ! sd->sdIgnoreWord )
	{ XDEB(sd->sdIgnoreWord); return -1;	}
    if  ( ! sd->sdOpenDictionary )
	{ XDEB(sd->sdOpenDictionary); return -1;	}

    if  ( ! sd->sdBackend					&&
	  (*sd->sdOpenDictionary)( sd, complain, through, sc )	)
	{ return -1;	}

    return (*sd->sdIgnoreWord)( sd, complain, through, sc, word );
    }

int spellLearnWord(	struct SpellDictionary *	sd,
			SpellComplain			complain,
			void *				through,
			const struct SpellChecker *	sc,
			const char *			word )
    {
    if  ( ! sd )
	{ XDEB(sd); return -1;	}
    if  ( ! sd->sdLearnWord )
	{ XDEB(sd->sdLearnWord); return -1;	}
    if  ( ! sd->sdOpenDictionary )
	{ XDEB(sd->sdOpenDictionary); return -1;	}

    if  ( ! sd->sdBackend					&&
	  (*sd->sdOpenDictionary)( sd, complain, through, sc )	)
	{ return -1;	}

    return (*sd->sdLearnWord)( sd, complain, through, sc, word );
    }

int spellForgetWord(	struct SpellDictionary *	sd,
			SpellComplain			complain,
			void *				through,
			const struct SpellChecker *	sc,
			const char *			word )
    {
    if  ( ! sd )
	{ XDEB(sd); return -1;	}
    if  ( ! sd->sdForgetWord )
	{ XDEB(sd->sdForgetWord); return -1;	}
    if  ( ! sd->sdOpenDictionary )
	{ XDEB(sd->sdOpenDictionary); return -1;	}

    if  ( ! sd->sdBackend					&&
	  (*sd->sdOpenDictionary)( sd, complain, through, sc )	)
	{ return -1;	}

    return (*sd->sdForgetWord)( sd, complain, through, sc, word );
    }

int spellCheckWord(	int *				pFoundStart,
			int *				pFoundLength,
			struct SpellScanJob *		ssj,
			const char *			paraStr,
			int				head,
			int				tail,
			int				next,
			int				atEnd )
    {
    int				rval= 0;
    struct SpellChecker *	sc= ssj->ssjChecker;
    struct SpellDictionary *	sd= ssj->ssjDictionary;

    if  ( ! sd || ! sc )
	{ XXDEB(sd,sc); rval= -1; goto ready;	}

    if  ( ssj->ssjOverrideLocaleTag				&&
	  strcmp( ssj->ssjOverrideLocaleTag, sd->sdLocaleTag )	)
	{
	int	d;

	d= spellFindDictionary( sc->scDictionaries, sc->scDictionaryCount,
						ssj->ssjOverrideLocaleTag );
	if  ( d >= 0 )
	    { sd= &(sc->scDictionaries[d]);	}
	}

    if  ( ! sd->sdCheckWord )
	{ XDEB(sd->sdCheckWord); rval= -1; goto ready;	}
    if  ( ! sd->sdOpenDictionary )
	{ XDEB(sd->sdOpenDictionary); rval= -1; goto ready;	}

    if  ( ! sd->sdBackend					&&
	  (*sd->sdOpenDictionary)( sd, ssj->ssjComplain,
					  ssj->ssjThrough, sc )	)
	{ rval= -1; goto ready;	}


    rval= (*sd->sdCheckWord)( pFoundStart, pFoundLength, ssj, sd,
					paraStr, head, tail, next, atEnd );

    /*
    appDebug("%s#%*.*s:%d\n", sd->sdLocaleTag,
				tail-head, tail-head, paraStr+ head, rval );
    */

  ready:

    return rval;
    }

