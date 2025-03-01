/****************************************************************/
/*								*/
/*  Setup of spellchecking.					*/
/*								*/
/****************************************************************/

#   include	"appFrameConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	<indSpellChecker.h>

#   include	<appDebugon.h>

static int spellCompareDictionaryTags(
				const void *		sd1,
				const void *		sd2 )
    {
    return strcmp( ((const SpellDictionary *)sd1)->sdLocaleTag,
				    ((const SpellDictionary *)sd2)->sdLocaleTag );
    }

/************************************************************************/
/*									*/
/*  Find dictionaries.							*/
/*									*/
/*  1)  Remove duplicates and less specific prefixes.			*/
/*									*/
/************************************************************************/

int spellSetup(		struct SpellChecker *	sc,
			SpellComplain		complain,
			void *			through )
    {
    int			complain_none= 1;

#   if USE_ENCHANT
    complain_none= 0;
#   endif

#   define USE_DFA 1
#   if USE_DFA

    if  ( spellSetupInd( sc, complain, complain_none, through ) < 0 )
	{ LDEB(1); return -1;	}

#   endif /* USE_DFA */

#   if USE_ENCHANT

    if  ( spellSetupEnchant( sc, complain, through ) < 0 )
	{ LDEB(1); return -1;	}

#   endif /* USE_ENCHANT */

    qsort( sc->scDictionaries, sc->scDictionaryCount, 
				    sizeof(SpellDictionary),
				    spellCompareDictionaryTags );

    /*  1  */
    if  ( sc->scDictionaryCount > 0 )
	{
	int			to= 0;
	SpellDictionary *	sds= sc->scDictionaries;

	int			fr;
	int			len;

	len= strlen( sds[to].sdLocaleTag );

	for ( fr= 1; fr < sc->scDictionaryCount; fr++ )
	    {
	    if  ( strncmp( sds[to].sdLocaleTag, sds[fr].sdLocaleTag, len ) )
		{ to++;					}
	    else{ indCleanDictionary( sc, sds+ to );	}

	    sds[to]= sds[fr];
	    len= strlen( sds[to].sdLocaleTag );
	    }

	sc->scDictionaryCount= to+ 1;
	}

    return 0;
    }
