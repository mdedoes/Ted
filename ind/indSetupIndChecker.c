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

#   include	<appSystem.h>
#   include	<sioFileio.h>

#   include	"indSpellChecker.h"
#   include	"indSpellGuessContext.h"
#   include	"indIndSpellDictionary.h"
#   include	"ind.h"
#   include	"indlocal.h"
#   include	"indPrivateDict.h"
#   include	"indSpellScanJob.h"
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

#   define FILEL 600
static const char * const	INDSPELL_DfaExtension= "dfa";

/************************************************************************/
/*									*/
/*  Build the name of a private dictionary.				*/
/*									*/
/************************************************************************/

static int indSpellPrivateDictionaryName(
				    MemoryBuffer *		dictName,
				    const char *		locale,
				    const MemoryBuffer *	privateDicts,
				    int				readOnly,
				    SpellComplain		complain,
				    void *			through )
    {
    int			rval= 0;
    const char *	suffix= ".changes";

    MemoryBuffer	relative;
    MemoryBuffer	home;
    MemoryBuffer	dicts;

    utilInitMemoryBuffer( &relative );
    utilInitMemoryBuffer( &home );
    utilInitMemoryBuffer( &dicts );

    if  ( fileHomeDirectory( &home ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( fileAbsoluteName( &dicts, privateDicts, 0, &home ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( fileTestDirectory( &dicts ) )
	{
	if  ( readOnly )
	    { rval= 1; goto ready;	}

	if  ( fileMakeDirectory( &dicts ) )
	    {
	    (*complain)( SCerrPRIVATE_DIR_NOT_MADE,
				utilMemoryBufferGetString( &dicts ), through );
	    rval= 1; goto ready;
	    }
	}

    if  ( utilMemoryBufferSetString( &relative, locale ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferAppendString( &relative, suffix ) )
	{ LDEB(1); return -1;	}

    if  ( fileAbsoluteName( dictName, &relative, 0, &dicts ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &relative );
    utilCleanMemoryBuffer( &home );
    utilCleanMemoryBuffer( &dicts );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Clean a SpellChecker						*/
/*									*/
/************************************************************************/

static void indSpellFreeIndDict(	IndSpellDictionary *	isd )
    {
    if  ( isd->isdStaticInd )
	{ indINDfree( isd->isdStaticInd );	}

    if  ( isd->isdForgotInd )
	{ indINDfree( isd->isdForgotInd );	}

    if  ( isd->isdLearntInd )
	{ indINDfree( isd->isdLearntInd );	}

    free( isd );

    return;
    }

/************************************************************************/
/*									*/
/*  Open index files for a certain dictionary.				*/
/*									*/
/************************************************************************/

static int indSpellOpenDictionary(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const SpellChecker *	sc )
    {
    int				rval= 0;
    int				ret;
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    const char *		sdf;

    const int			readOnly= 1;

    MemoryBuffer		dictName;
    IndSpellDictionary *	isd= (IndSpellDictionary *)0;

    utilInitMemoryBuffer( &dictName );

    if  ( sd->sdBackend )
	{ goto ready;	}

    isd= malloc( sizeof(IndSpellDictionary) );
    if  ( ! isd )
	{ XDEB(isd); rval= -1; goto ready;	}

    isd->isdStaticInd= (IND *)0;
    isd->isdForgotInd= (IND *)0;
    isd->isdLearntInd= (IND *)0;

    sdf= utilMemoryBufferGetString( &(sd->sdFileName) );
    isd->isdStaticInd= indRead( sdf, 1 );
    if  ( ! isd->isdStaticInd )
	{
	(*complain)( SCerrDICT_NO_ACCESS, sdf, through );
	rval= -1; goto ready;
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocaleTag,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sis= sioInFileioOpen( &dictName );
    if  ( sis )
	{
	if  ( indReadPrivateDictionary( sis,
				&(isd->isdLearntInd), &(isd->isdForgotInd) ) )
	    {
	    (*complain)( SCerrPRIVATE_DICT_WRONG_FORMAT,
						    sd->sdLocaleTag, through );
	    }
	}

    sd->sdBackend= isd; isd= (IndSpellDictionary *)0; /* steal */

  ready:

    if  ( isd )
	{ indSpellFreeIndDict( isd );	}

    if  ( sis )
	{ sioInClose( sis );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close a dictionary.							*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static void indSpellCleanDict(	const SpellChecker *	sc,
				SpellDictionary *	sd )
    {
    if  ( sd->sdBackend )
	{
	indSpellFreeIndDict( sd->sdBackend );
	sd->sdBackend= (void *)0;
	}
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Learn/Forget/Ignore a word.						*/
/*									*/
/************************************************************************/

static int indSpellLearnWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    int			rval= 0;
    int			ret;
    SimpleOutputStream * sos= (SimpleOutputStream *)0;
    IndSpellDictionary * isd= (IndSpellDictionary *)0;

    const int		readOnly= 0;

    MemoryBuffer	dictName;

    utilInitMemoryBuffer( &dictName );

    isd= (IndSpellDictionary *)sd->sdBackend;

    if  ( ! isd->isdLearntInd )
	{
	isd->isdLearntInd= indINDmake( 0 );
	if  ( ! isd->isdLearntInd )
	    { XDEB(isd->isdLearntInd); rval= -1; goto ready;	}
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocaleTag,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sos= sioOutFileioOpenForAppend( &dictName );
    if  ( ! sos )
	{ rval= -1; goto ready;	}

    if  ( indLearnWord( sos, isd->isdLearntInd, isd->isdForgotInd, word ) )
	{ SDEB(word); rval= -1; goto ready;	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

static int indSpellForgetWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    int			rval= 0;
    int			ret;
    SimpleOutputStream * sos= (SimpleOutputStream *)0;
    IndSpellDictionary * isd= (IndSpellDictionary *)0;

    const int		readOnly= 0;

    MemoryBuffer	dictName;

    utilInitMemoryBuffer( &dictName );

    isd= (IndSpellDictionary *)sd->sdBackend;

    if  ( ! isd->isdForgotInd )
	{
	isd->isdLearntInd= indINDmake( 0 );
	if  ( ! isd->isdForgotInd )
	    { XDEB(isd->isdForgotInd); rval= -1; goto ready;	}
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocaleTag,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sos= sioOutFileioOpen( &dictName );
    if  ( ! sos )
	{ rval= -1; goto ready;	}

    if  ( indForgetWord( sos, isd->isdLearntInd, isd->isdForgotInd, word ) )
	{ SDEB(word);	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int indSpellIgnoreWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    IndSpellDictionary * isd= (IndSpellDictionary *)0;

    isd= (IndSpellDictionary *)sd->sdBackend;

    if  ( indMoveWord( isd->isdForgotInd, isd->isdLearntInd, word ) )
	{ SDEB((char *)word); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Make guesses.							*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int indSpellGuess(	IndGuessList *		igl,
				SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker * 	sc,
				const char *		word )
    {
    int			count= 0;
    int			limit;

    limit= indCollectGuesses( igl, sd, word );

    for ( count= 0; count < igl->iglGuessCount; count++ )
	{
	if  ( igl->iglGuesses[count].igsScore < limit )
	    { break;	}
	}

    return count;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Look for an unacceptable word.					*/
/*									*/
/************************************************************************/

static int indScanCheckWordInd(
			int *				pFoundFrom,
			int *				pFoundUpto,
			struct SpellScanJob *		ssj,
			struct SpellDictionary *	sd,
			const char *			paraStr,
			int				head,
			int				tail,
			int				next,
			int				atEnd )
    {
    int		count;

    if  ( indNewPossibility( ssj, head ) )
	{ CDEB(paraStr[head]); return -1;	}

    {
    int	from= head;

    while( from < tail )
	{ indAddCharacterToPossibilities( ssj, paraStr[from++] ); }
    }

    /*  Does the word match here?  */
    count= indCountPossibilities( ssj, sd, tail, atEnd );

    if  ( count == 0 )
	{
	PossibleWord *	maxpw;

	maxpw= indMaximalPossibility( ssj );

	if  ( ! maxpw )
	    { XDEB(maxpw); return -1;	}

	*pFoundFrom= maxpw->pwStartAt;
	*pFoundUpto= maxpw->pwInsertionPoint;

	return 1;
	}

    {
    int	from= tail;

    while( from < next )
	{ indAddCharacterToPossibilities( ssj, paraStr[from++] ); }
    }

    /*  Is the word with the trailing white space a valid prefix? */
    /* count= */ (void) indCountPossibilities( ssj, sd, next, atEnd );

    indRejectPossibilities( ssj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Obtain the list of dictionaries.					*/
/*									*/
/************************************************************************/

static int indSpellFoundDictionary(	const MemoryBuffer *	filename,
					void *			voidsc )
    {
    int				rval= 0;
    SpellChecker *		sc= (SpellChecker *)voidsc;
    SpellDictionary *		sd;

    MemoryBuffer		base;
    int				i;

    utilInitMemoryBuffer( &base );

    for ( i= 0; i < sc->scDictionaryCount; i++ )
	{
	if  ( utilEqualMemoryBuffer( &(sc->scDictionaries[i].sdFileName),
								filename ) )
	    { goto ready;	}
	}

    if  ( fileGetBaseName( &base, filename ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sd= indSpellCheckerAddDictionary( sc,
			    utilMemoryBufferGetString( &base ), filename );
    if  ( ! sd )
	{ XDEB(sd); rval= -1; goto ready;	}

    sd->sdOpenDictionary= indSpellOpenDictionary;
    sd->sdCloseDictionary= indSpellCleanDict;

    sd->sdLearnWord= indSpellLearnWord;
    sd->sdForgetWord= indSpellForgetWord;
    sd->sdIgnoreWord= indSpellIgnoreWord;
    sd->sdGuessWord= indSpellGuess;
    sd->sdCheckWord= indScanCheckWordInd;

  ready:

    utilCleanMemoryBuffer( &base );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Configure a spell checker.						*/
/*									*/
/************************************************************************/

int spellSetupInd(		SpellChecker *	sc,
				SpellComplain	complain,
				int		complain_none,
				void *		through )
    {
    if  ( ! fileTestDirectory( &(sc->scSystemDirectory) ) )
	{
	if  ( appForAllFiles( &(sc->scSystemDirectory), INDSPELL_DfaExtension,
					(void *)sc, indSpellFoundDictionary ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sc->scDictionaryCount == 0 )
	{
	const char * dn= utilMemoryBufferGetString( &(sc->scSystemDirectory) );

	if  ( fileTestDirectory( &(sc->scSystemDirectory) ) )
	    {
	    if  ( complain_none )
		{
		(*complain)( SCerrDIR_DOES_NOT_EXIST, dn, through );
		}
	    }
	else{
	    if  ( fileTestFileReadable( &(sc->scSystemDirectory) ) )
		{ (*complain)( SCerrDIR_NO_ACCESS, dn, through );	}
	    else{
		if  ( complain_none )
		    {
		    (*complain)( SCerrDIR_HAS_NO_DICTS, dn, through );
		    }
		}
	    }

	return 1;
	}

    return 0;
    }

