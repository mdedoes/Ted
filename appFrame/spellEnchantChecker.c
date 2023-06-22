/************************************************************************/
/*									*/
/*  Utility functions for scanning documents for typographical errors	*/
/*  using enchant.							*/
/*									*/
/*  http://www.abisource.com/viewvc/enchant/trunk/src/enchant.h?view=markup */
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_ENCHANT

#   include	<string.h>

#   include	<indSpellChecker.h>

		/* apt-get install libenchant-dev */
# if ENCHANT_VERSION == 1
#   include	<enchant/enchant.h>
# endif

		/* apt-get install libenchant-2-dev */
# if ENCHANT_VERSION == 2
#   include	<enchant-2/enchant.h>
# endif

#   include	<appDebugon.h>

static int spellEnchantOpenDictionary(	struct SpellDictionary * sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc )
    {
    EnchantBroker *	broker= (EnchantBroker *)sc->scBackend;

    if  ( sd->sdBackend )
	{ return 0;	}
    if  ( ! sc->scBackend )
	{ XDEB(sc->scBackend); return -1;	}

    sd->sdBackend= (void *)enchant_broker_request_dict( broker,
							    sd->sdLocaleTag );
    if  ( ! sd->sdBackend )
	{ SXDEB(sd->sdLocaleTag,sd->sdBackend); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a dictionary.							*/
/*									*/
/************************************************************************/

static void spellEnchantCleanDict(	const SpellChecker *	sc,
					SpellDictionary *	sd )
    {
    if  ( ! sc->scBackend )
	{ XDEB(sc->scBackend); return;	}

    if  ( sd->sdBackend )
	{
	enchant_broker_free_dict( sc->scBackend, sd->sdBackend );
	sd->sdBackend= (void *)0;
	}
    }

static int indScanCheckWordEnchant(
			int *				pFoundStart,
			int *				pFoundLength,
			struct SpellScanJob *		ssj,
			SpellDictionary *		sd,
			const char *			paraStr,
			int				head,
			int				tail,
			int				next,
			int				atEnd )
    {
    int			res;

    const int		length= tail- head;

    /* Do not check numbers */
    if  ( isdigit( paraStr[head] ) ||  paraStr[head] == '-' )
	{
	char *	e;
	int	matched;

	(void) strtod( paraStr+ head, &e );

	matched= e- ( paraStr+ head );

	if  ( matched >= length )
	    { return 0;	}
	}

    res= enchant_dict_check( (EnchantDict *)sd->sdBackend,
						paraStr+ head, length );
    if  ( res )
	{
	*pFoundStart= head;
	*pFoundLength= length;
	/*
	appDebug( ">> \"%.*s\"\n", *pFoundLength, paraStr+ *pFoundStart );
	*/
	}

    return res;
    }

static int spellEnchantLearnWord(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const SpellChecker *	sc,
					const char *		word )
    {
    enchant_dict_add( (EnchantDict *)sd->sdBackend,
						word, strlen( word ) );

    return 0;
    }

static int spellEnchantForgetWord(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const SpellChecker *	sc,
					const char *		word )
    {
    enchant_dict_remove( (EnchantDict *)sd->sdBackend,
						word, strlen( word ) );

    return 0;
    }

static int spellEnchantIgnoreWord(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const SpellChecker *	sc,
					const char *		word )
    {
    enchant_dict_add_to_session( (EnchantDict *)sd->sdBackend,
						word, strlen( word ) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make guesses.							*/
/*									*/
/************************************************************************/

static int spellEnchantGuess(	IndGuessList *		igl,
				SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker * 	sc,
				const char *		word )
    {
    size_t		count= 0;
    int			i;
    char **		guesses= (char **)0;

    guesses= enchant_dict_suggest( (EnchantDict *)sd->sdBackend,
						word, strlen( word ), &count );
    if  ( ! guesses )
	{ /*XDEB(guesses);*/ return 0;	}

    for ( i= 0; i < count; i++ )
	{
	if  ( indAddGuess( igl, guesses[i], 1 ) )
	    { LSDEB(i,guesses[i]); return -1;	}
	}

    enchant_dict_free_string_list( (EnchantDict *)sd->sdBackend, guesses );

    return (int)count;
    }

static void spellEnchantGotDict(	const char * const	lang_tag,
					const char * const	provider_name,
					const char * const	provider_desc,
					const char * const	provider_file,
					void *			through )
    {
    SpellChecker *	sc= (SpellChecker *)through;
    MemoryBuffer	filename;
    SpellDictionary *	sd;

    utilInitMemoryBuffer( &filename );

    if  ( provider_file )
	{
	if  ( utilMemoryBufferSetString( &filename, provider_file ) )
	    { SDEB(provider_file); goto ready;	}
	}

    /* No duplicates .. (Gives dfa checkers priority) */
    if  ( spellGetDictionary( sc->scDictionaries, sc->scDictionaryCount,
							    lang_tag ) >= 0 )
	{ goto ready;	}

    sd= indSpellCheckerAddDictionary( sc, lang_tag, &filename );
    if  ( ! sd )
	{ SXDEB(lang_tag,sd); goto ready;	}

    sd->sdOpenDictionary= spellEnchantOpenDictionary;
    sd->sdCloseDictionary= spellEnchantCleanDict;

    sd->sdLearnWord= spellEnchantLearnWord;
    sd->sdForgetWord= spellEnchantForgetWord;
    sd->sdIgnoreWord= spellEnchantIgnoreWord;
    sd->sdGuessWord= spellEnchantGuess;
    sd->sdCheckWord= indScanCheckWordEnchant;

  ready:

    utilCleanMemoryBuffer( &filename );

    return;
    }

static void spellEnchantCloseChecker(	struct SpellChecker *	sc )
    {
    if  ( sc->scBackend )
	{
	enchant_broker_free( (EnchantBroker *)sc->scBackend );
	}
    }

int spellSetupEnchant(	struct SpellChecker *		sc,
			SpellComplain			complain,
			void *				through )
    {
    if  ( ! sc->scBackend )
	{
	EnchantBroker *	broker= enchant_broker_init();
	if  ( ! broker )
	    { XDEB(broker); return -1;	}

	sc->scBackend= broker;
	sc->scCloseChecker= spellEnchantCloseChecker;
	}

    enchant_broker_list_dicts( (EnchantBroker *)sc->scBackend,
				    spellEnchantGotDict, (void *)sc );

    return 0;
    }

#   endif /* USE_ENCHANT */
